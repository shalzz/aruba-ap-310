/*
 * Freescale Three Speed Ethernet Controller driver
 *
 * This software may be used and distributed according to the
 * terms of the GNU Public License, Version 2, incorporated
 * herein by reference.
 *
 * Copyright 2004-2011 Freescale Semiconductor, Inc.
 * (C) Copyright 2003, Motorola, Inc.
 * author Andy Fleming
 *
 */
#ifdef powerpc
#include <config.h>
#include <common.h>
#include <malloc.h>
#include <net.h>
#include <command.h>
#include <tsec.h>
#include <asm/errno.h>
#include <asm/processor.h>

#include "miiphy.h"

DECLARE_GLOBAL_DATA_PTR;

#define MAXCONTROLLERS	(8)

static int relocated = 0;

static struct tsec_private *privlist[MAXCONTROLLERS];
static int num_tsecs = 0;

int tsec_send(struct eth_device *dev,
		     volatile void *packet, int length);
static int tsec_recv(struct eth_device *dev);
static int tsec_init(struct eth_device *dev, bd_t * bd);
static int tsec_initialize(bd_t * bis, struct tsec_info_struct *tsec_info);
static void tsec_halt(struct eth_device *dev);
static void init_registers(volatile tsec_t * regs);
static void startup_tsec(struct eth_device *dev);
static int init_phy(struct eth_device *dev);
void write_phy_reg(struct tsec_private *priv, uint regnum, uint value);
uint read_phy_reg(struct tsec_private *priv, uint regnum);
static struct phy_info *get_phy_info(struct eth_device *dev);
static void phy_run_commands(struct tsec_private *priv, struct phy_cmd *cmd);
static void adjust_link(struct eth_device *dev);
static void relocate_cmds(void);
#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII) \
	&& !defined(BITBANGMII)
static int tsec_miiphy_write(char *devname, unsigned char addr,
			     unsigned char reg, unsigned short value);
static int tsec_miiphy_read(char *devname, unsigned char addr,
			    unsigned char reg, unsigned short *value);
#endif
#ifdef CONFIG_MCAST_TFTP
static int tsec_mcast_addr (struct eth_device *dev, u8 mcast_mac, u8 set);
#endif

/* Default initializations for TSEC controllers. */

static struct tsec_info_struct tsec_info[] = {
#ifdef CONFIG_TSEC1
	STD_TSEC_INFO(1),	/* TSEC1 */
#endif
#ifdef CONFIG_TSEC2
	STD_TSEC_INFO(2),	/* TSEC2 */
#endif
#ifdef CONFIG_MPC85XX_FEC
	{
		.regs = (tsec_t *)(TSEC_BASE_ADDR + 0x2000),
		.miiregs = (tsec_mdio_t *)(MDIO_BASE_ADDR),
		.devname = CONFIG_MPC85XX_FEC_NAME,
		.phyaddr = FEC_PHY_ADDR,
		.flags = FEC_FLAGS
	},			/* FEC */
#endif
#ifdef CONFIG_TSEC3
	STD_TSEC_INFO(3),	/* TSEC3 */
#endif
#ifdef CONFIG_TSEC4
	STD_TSEC_INFO(4),	/* TSEC4 */
#endif
};

#ifdef CONFIG_PHY_MV154x
unsigned pmap[4] = { 0, 0x800, 0x1000, 0x1800 };

static unsigned
aruba_linkcrypt_read(int port, unsigned reg)
{
	unsigned short dlow, dhigh;

	reg |= pmap[port];
	/* enable linkcrypt page */
	miiphy_write("eth0", port, 0x16, 0x10);

	/* read from 'reg' */
	miiphy_write("eth0", port, 0x0, reg);

	miiphy_read("eth0", port, 0x2, &dlow);
	miiphy_read("eth0", port, 0x3, &dhigh);

	/* restore to page 0 */
	miiphy_write("eth0", port, 0x16, 0x0);

	return (dhigh << 16) | dlow;
}

static void
aruba_linkcrypt_write(int port, unsigned reg, unsigned val)
{
	unsigned short dlow, dhigh;

	reg |= pmap[port];

	dhigh = val >> 16;
	dhigh &= 0xffff;

	dlow = val & 0xffff;

	/* enable linkcrypt page */
	miiphy_write("eth0", port, 0x16, 0x10);

	/* write to 'reg' */
	miiphy_write("eth0", port, 0x1, reg);

	miiphy_write("eth0", port, 0x2, dlow);
	miiphy_write("eth0", port, 0x3, dhigh);

	/* restore to page 0 */
	miiphy_write("eth0", port, 0x16, 0x0);
}

unsigned
aruba_xmdio_read(int port, int dev, unsigned reg)
{
	unsigned short val;

	/* set to page 0 */
	miiphy_write("eth0", port, 0x16, 0x0);

	/* program device and reg */
	miiphy_write("eth0", port, 0xd, dev);
	miiphy_write("eth0", port, 0xe, reg);

	/* indicate data access required */
	miiphy_write("eth0", port, 0xd, dev | 0x4000);

	/* read value */
	miiphy_read("eth0", port, 0xe, &val);

	return val;
}

void
aruba_xmdio_write(int port, int dev, unsigned reg, unsigned val)
{
	/* set to page 0 */
	miiphy_write("eth0", port, 0x16, 0x0);

	/* program device and reg */
	miiphy_write("eth0", port, 0xd, dev);
	miiphy_write("eth0", port, 0xe, reg);

	/* indicate data access required */
	miiphy_write("eth0", port, 0xd, dev | 0x4000);

	/* write value */
	miiphy_write("eth0", port, 0xe, val);
}

#define Y0	0x0eb00100
#define A0	0x0eb00200
#define A1	0x0eb10200

/*
 * We used to just read the revision. However, for A1, the revision is
 * still the same, but the PHY ID register has had its revision field
 * incremented. Fortunately, the basic ID is the same, so Linux will
 * not be affected.  Since the init script is different between A0 and
 * A1, we need to distinguish A0 from A1, which we now do by combining
 * the ID register value with the revision register value
 */
static void
mv1540_get_version(int addr, unsigned *v)
{
	unsigned short val;
	unsigned short id;

	*v = 0;

	/* register 31_251.15:0 has the revision */
	miiphy_write("eth0", addr, 0x16, 0xfb);

	miiphy_read("eth0", addr, 0x1f, &val);

	miiphy_write("eth0", addr, 0x16, 0x0000);

	/* read ID */
	miiphy_read("eth0", addr, 0x3, &id);

	*v = ((unsigned)id << 16) | val;

}

#define MAX_FRAME_SIZE 9100

void tsec_154x_init(int eeeEnable)
{
	int i;
	unsigned short reg;
	int startAddr, endAddr;
	unsigned val, version1540;

	startAddr = 0;
	endAddr = startAddr + 4;

#ifdef CONFIG_APBOOT_notyet
	printf("PHY:   ");
#endif
	mv1540_get_version(startAddr, &version1540);

	for (i = startAddr; i < endAddr; i++) {
#ifndef CONFIG_ARDMORE
		/* Enable QSGMII AN */
		/* Set page to 4. */
		miiphy_write("eth0", i, 0x16, 4);
		/* Enable AN */
		miiphy_write("eth0", i, 0x0, 0x1140);
		/* Set page to 0. */
		miiphy_write("eth0", i, 0x16, 0);
#endif

		/* Power up the phy */
		miiphy_read("eth0", i, 0, &reg);
		reg &= ~(1 << 11);
		miiphy_write("eth0", i, 0, reg);
		udelay(100);
		switch (version1540) {
		case Y0:
			if (eeeEnable) {
				/* set ELU#0 default match */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x10);
				/* RW U1 P0 R1 H0104 */
				miiphy_write("eth0", i, 0x1, 0x0104);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				miiphy_write("eth0", i, 0x3, 0x4000);
				/* RW U1 P0 R1 H0904 */
				miiphy_write("eth0", i, 0x1, 0x0904);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				miiphy_write("eth0", i, 0x3, 0x4000);
				/* RW U1 P0 R1 H1104 */
				miiphy_write("eth0", i, 0x1, 0x1104);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				miiphy_write("eth0", i, 0x3, 0x4000);
				/* RW U1 P0 R1 H1904 */
				miiphy_write("eth0", i, 0x1, 0x1904);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				miiphy_write("eth0", i, 0x3, 0x4000);
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);

				/* set ILU#0 default match */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x0010);
				/* RW U1 P0 R1 H0207 */
				miiphy_write("eth0", i, 0x1, 0x0207);
				/* RW U1 P0 R2 h4000 */
				miiphy_write("eth0", i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				miiphy_write("eth0", i, 0x3, 0x0000);
				/* RW U1 P0 R1 H0A07 */
				miiphy_write("eth0", i, 0x1, 0x0a07);
				/* RW U1 P0 R2 h4000 */
				miiphy_write("eth0", i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				miiphy_write("eth0", i, 0x3, 0x0000);
				/* RW U1 P0 R1 H1207 */
				miiphy_write("eth0", i, 0x1, 0x1207);
				/* RW U1 P0 R2 h4000 */
				miiphy_write("eth0", i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				miiphy_write("eth0", i, 0x3, 0x0000);
				/* RW U1 P0 R1 H1A07 */
				miiphy_write("eth0", i, 0x1, 0x1a07);
				/* RW U1 P0 R2 h4000 */
				miiphy_write("eth0", i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				miiphy_write("eth0", i, 0x3, 0x0000);
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);

				/* change the wiremac ipg from 12 to 11 */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x0010);
				/* RW U1 P0 R1 H0041 */
				miiphy_write("eth0", i, 0x1, 0x0041);
				/* RW U1 P0 R2 h00b1 */
				miiphy_write("eth0", i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				miiphy_write("eth0", i, 0x3, 0x0002);
				/* RW U1 P0 R1 H0841 */
				miiphy_write("eth0", i, 0x1, 0x0841);
				/* RW U1 P0 R2 h00b1 */
				miiphy_write("eth0", i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				miiphy_write("eth0", i, 0x3, 0x0002);
				/* RW U1 P0 R1 H1041 */
				miiphy_write("eth0", i, 0x1, 0x1041);
				/* RW U1 P0 R2 h00b1 */
				miiphy_write("eth0", i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				miiphy_write("eth0", i, 0x3, 0x0002);
				/* RW U1 P0 R1 H1841 */
				miiphy_write("eth0", i, 0x1, 0x1841);
				/* RW U1 P0 R2 h00b1 */
				miiphy_write("eth0", i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				miiphy_write("eth0", i, 0x3, 0x0002);
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);


				/* change the setting to not drop badtag */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x0010);
				/* RW U1 P0 R1 H000b */
				miiphy_write("eth0", i, 0x1, 0x000b);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				miiphy_write("eth0", i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H080b */
				miiphy_write("eth0", i, 0x1, 0x080b);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				miiphy_write("eth0", i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H100b */
				miiphy_write("eth0", i, 0x1, 0x100b);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				miiphy_write("eth0", i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H180b */
				miiphy_write("eth0", i, 0x1, 0x180b);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				miiphy_write("eth0", i, 0x3, 0x0fb4);
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);

				/* set MACSec EEE Entry/Exit Timer */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x0010);
				/* RW U1 P0 R1 H03C0 */
				miiphy_write("eth0", i, 0x1, 0x03c0);
				/* RW U1 P0 R2 H111E */
				miiphy_write("eth0", i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				miiphy_write("eth0", i, 0x3, 0x111e);
				/* RW U1 P0 R1 H0BC0 */
				miiphy_write("eth0", i, 0x1, 0x0bc0);
				/* RW U1 P0 R2 H111E */
				miiphy_write("eth0", i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				miiphy_write("eth0", i, 0x3, 0x111e);
				/* RW U1 P0 R1 H13C0 */
				miiphy_write("eth0", i, 0x1, 0x13c0);
				/* RW U1 P0 R2 H111E */
				miiphy_write("eth0", i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				miiphy_write("eth0", i, 0x3, 0x111e);
				/* RW U1 P0 R1 H1BC0 */
				miiphy_write("eth0", i, 0x1, 0x1bc0);
				/* RW U1 P0 R2 H111E */
				miiphy_write("eth0", i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				miiphy_write("eth0", i, 0x3, 0x111e);
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);

				/* Start of EEE Workaround */
				/* RW U1 P0-3 R22 H00FB */
				miiphy_write("eth0", i, 0x16, 0x00FB);
				/* RW U1 P0-3 R11 H1120 */
				miiphy_write("eth0", i, 0xB , 0x1120);
				/* RW U1 P0-3 R8  H3666 */
				miiphy_write("eth0", i, 0x8 , 0x3666);
				/* RW U1 P0-3 R22 H00FF */
				miiphy_write("eth0", i, 0x16, 0x00FF);
				/* RW U1 P0-3 R17 H0F0C */
				miiphy_write("eth0", i, 0x11, 0x0F0C);
				/* RW U1 P0-3 R16 H2146 */
				miiphy_write("eth0", i, 0x10, 0x2146);
				/* RW U1 P0-3 R17 Hc090 */
				miiphy_write("eth0", i, 0x11, 0xC090);
				/* RW U1 P0-3 R16 H2147 */
				miiphy_write("eth0", i, 0x10, 0x2147);
				/* RW U1 P0-3 R17 H0000 */
				miiphy_write("eth0", i, 0x11, 0x0000);
				/* RW U1 P0-3 R16 H2000 */
				miiphy_write("eth0", i, 0x10, 0x2000);
				/* RW U1 P0-3 R17 H6000 */
				miiphy_write("eth0", i, 0x11, 0x6000);
				/* RW U1 P0-3 R16 H2143 */
				miiphy_write("eth0", i, 0x10, 0x2143);
				/* RW U1 P0-3 R17 HC004 */
				miiphy_write("eth0", i, 0x11, 0xC004);
				/* RW U1 P0-3 R16 H2100 */
				miiphy_write("eth0", i, 0x10, 0x2100);
				/* RW U1 P0-3 R17 H49E8 */
				miiphy_write("eth0", i, 0x11, 0x49E8);
				/* RW U1 P0-3 R16 H2144 */
				miiphy_write("eth0", i, 0x10, 0x2144);
				/* RW U1 P0-3 R17 H3180 */
				miiphy_write("eth0", i, 0x11, 0x3180);
				/* RW U1 P0-3 R16 H2148 */
				miiphy_write("eth0", i, 0x10, 0x2148);
				/* RW U1 P0-3 R17 HFC44 */
				miiphy_write("eth0", i, 0x11, 0xFC44);
				/* RW U1 P0-3 R16 H214B */
				miiphy_write("eth0", i, 0x10, 0x214B);
				/* RW U1 P0-3 R17 H7FD2 */
				miiphy_write("eth0", i, 0x11, 0x7FD2);
				/* RW U1 P0-3 R16 H214C */
				miiphy_write("eth0", i, 0x10, 0x214C);
				/* RW U1 P0-3 R17 H2240 */
				miiphy_write("eth0", i, 0x11, 0x2240);
				/* RW U1 P0-3 R16 H214D */
				miiphy_write("eth0", i, 0x10, 0x214D);
				/* RW U1 P0-3 R17 H3008 */
				miiphy_write("eth0", i, 0x11, 0x3008);
				/* RW U1 P0-3 R16 H214E */
				miiphy_write("eth0", i, 0x10, 0x214E);
				/* RW U1 P0-3 R17 H3DF0 */
				miiphy_write("eth0", i, 0x11, 0x3DF0);
				/* RW U1 P0-3 R16 H214F */
				miiphy_write("eth0", i, 0x10, 0x214F);
				/* RW U1 P0-3 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0);

				/* Enable EEE Auto-neg advertisement on P0-P7 ports */
				/* RW U1 P0-3 R13 H0007 */
				miiphy_write("eth0", i, 0xD , 0x0007);
				/* RW U1 P0-3 R14 H003C */
				miiphy_write("eth0", i, 0xE , 0x003C);
				/* RW U1 P0-3 R13 H4007 */
				miiphy_write("eth0", i, 0xD , 0x4007);
				/* RW U1 P0-3 R14 H0006 */
				miiphy_write("eth0", i, 0xE , 0x0006);

				/* Soft-Reset on P0-P7 ports */
				/* RW U1 P0-3 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0);
				/* RW U1 P0-3 R0  H9140 */
				miiphy_write("eth0", i, 0x0 , 0x9140);

				/* Enable MACsec EEE Master Mode on P0-3 ports */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x0010);
				/* RW U1 P0 R1  H03C1 */
				miiphy_write("eth0", i, 0x1 , 0x03C1);
				/* RW U1 P0 R2  H0001 */
				miiphy_write("eth0", i, 0x2 , 0x0001);
				/* RW U1 P0 R3  H0000 */
				miiphy_write("eth0", i, 0x3 , 0x0000);
				/* RW U1 P0 R1  H0BC1 */
				miiphy_write("eth0", i, 0x1 , 0x0BC1);
				/* RW U1 P0 R2  H0001 */
				miiphy_write("eth0", i, 0x2 , 0x0001);
				/* RW U1 P0 R3  H0000 */
				miiphy_write("eth0", i, 0x3 , 0x0000);
				/* RW U1 P0 R1  H13C1 */
				miiphy_write("eth0", i, 0x1 , 0x13C1);
				/* RW U1 P0 R2  H0001 */
				miiphy_write("eth0", i, 0x2 , 0x0001);
				/* RW U1 P0 R3  H0000 */
				miiphy_write("eth0", i, 0x3 , 0x0000);
				/* RW U1 P0 R1  H1BC1 */
				miiphy_write("eth0", i, 0x1 , 0x1BC1);
				/* RW U1 P0 R2  H0001 */
				miiphy_write("eth0", i, 0x2 , 0x0001);
				/* RW U1 P0 R3  H0000 */
				miiphy_write("eth0", i, 0x3 , 0x0000);
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);
			} else {
				/* set ELU#0 default match  */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x0010);
				/* RW U1 P0 R1 H0104 */
				miiphy_write("eth0", i, 0x1, 0x0104);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				miiphy_write("eth0", i, 0x3, 0x4000);
				/* RW U1 P0 R1 H0904 */
				miiphy_write("eth0", i, 0x1, 0x0904);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				miiphy_write("eth0", i, 0x3, 0x4000);
				/* RW U1 P0 R1 H1104 */
				miiphy_write("eth0", i, 0x1, 0x1104);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				miiphy_write("eth0", i, 0x3, 0x4000);
				/* RW U1 P0 R1 H1904 */
				miiphy_write("eth0", i, 0x1, 0x1904);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				miiphy_write("eth0", i, 0x3, 0x4000);
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);

				/* set ILU#0 default match */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x0010);
				/* RW U1 P0 R1 H0207 */
				miiphy_write("eth0", i, 0x1, 0x0207);
				/* RW U1 P0 R2 h4000 */
				miiphy_write("eth0", i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				miiphy_write("eth0", i, 0x3, 0x0000);
				/* RW U1 P0 R1 H0A07 */
				miiphy_write("eth0", i, 0x1, 0x0a07);
				/* RW U1 P0 R2 h4000 */
				miiphy_write("eth0", i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				miiphy_write("eth0", i, 0x3, 0x0000);
				/* RW U1 P0 R1 H1207 */
				miiphy_write("eth0", i, 0x1, 0x1207);
				/* RW U1 P0 R2 h4000 */
				miiphy_write("eth0", i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				miiphy_write("eth0", i, 0x3, 0x0000);
				/* RW U1 P0 R1 H1A07 */
				miiphy_write("eth0", i, 0x1, 0x1a07);
				/* RW U1 P0 R2 h4000 */
				miiphy_write("eth0", i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				miiphy_write("eth0", i, 0x3, 0x0000);
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);

				/* change the wiremac ipg from 12 to 11 */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x0010);
				/* RW U1 P0 R1 H0041 */
				miiphy_write("eth0", i, 0x1, 0x0041);
				/* RW U1 P0 R2 h00b1 */
				miiphy_write("eth0", i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				miiphy_write("eth0", i, 0x3, 0x0002);
				/* RW U1 P0 R1 H0841 */
				miiphy_write("eth0", i, 0x1, 0x0841);
				/* RW U1 P0 R2 h00b1 */
				miiphy_write("eth0", i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				miiphy_write("eth0", i, 0x3, 0x0002);
				/* RW U1 P0 R1 H1041 */
				miiphy_write("eth0", i, 0x1, 0x1041);
				/* RW U1 P0 R2 h00b1 */
				miiphy_write("eth0", i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				miiphy_write("eth0", i, 0x3, 0x0002);
				/* RW U1 P0 R1 H1841 */
				miiphy_write("eth0", i, 0x1, 0x1841);
				/* RW U1 P0 R2 h00b1 */
				miiphy_write("eth0", i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				miiphy_write("eth0", i, 0x3, 0x0002);
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);

				/* wire mac: enable large frames */
				val = aruba_linkcrypt_read(i, 0x40);
				val &= ~0xfffc; /* 0x3fff << 2 */
				val |= (MAX_FRAME_SIZE << 2);
				aruba_linkcrypt_write(i, 0x40, val);

				/* system mac: enable large frames */
				val = aruba_linkcrypt_read(i, 0x50);
				val &= ~0xfffc; /* 0x3fff << 2 */
				val |= (MAX_FRAME_SIZE << 2);
				aruba_linkcrypt_write(i, 0x50, val);

				/* change the setting to not drop badtag */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x0010);
				/* RW U1 P0 R1 H000b */
				miiphy_write("eth0", i, 0x1, 0x000b);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				miiphy_write("eth0", i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H080b */
				miiphy_write("eth0", i, 0x1, 0x080b);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				miiphy_write("eth0", i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H100b */
				miiphy_write("eth0", i, 0x1, 0x100b);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				miiphy_write("eth0", i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H180b */
				miiphy_write("eth0", i, 0x1, 0x180b);
				/* RW U1 P0 R2 h0000 */
				miiphy_write("eth0", i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				miiphy_write("eth0", i, 0x3, 0x0fb4);
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);

				/* set MACSec EEE Entry/Exit Timer */
				/* RW U1 P0 R22 H0010 */
				miiphy_write("eth0", i, 0x16, 0x0010);
				/* RW U1 P0 R1 H03C0 */
				miiphy_write("eth0", i, 0x1, 0x03c0);
				/* RW U1 P0 R2 H111E */
				miiphy_write("eth0", i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				miiphy_write("eth0", i, 0x3, 0x111e);
				/* RW U1 P0 R1 H0BC0 */
				miiphy_write("eth0", i, 0x1, 0x0bc0);
				/* RW U1 P0 R2 H111E */
				miiphy_write("eth0", i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				miiphy_write("eth0", i, 0x3, 0x111e);
				/* RW U1 P0 R1 H13C0 */
				miiphy_write("eth0", i, 0x1, 0x13c0);
				/* RW U1 P0 R2 H111E */
				miiphy_write("eth0", i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				miiphy_write("eth0", i, 0x3, 0x111e);
				/* RW U1 P0 R1 H1BC0 */
				miiphy_write("eth0", i, 0x1, 0x1bc0);
				/* RW U1 P0 R2 H111E */
				miiphy_write("eth0", i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				miiphy_write("eth0", i, 0x3, 0x111e);
				/*  RW U1 P0 R22 H0000 */
				/*  RW U1 P0-3 R22 H00FB */
				/*  RW U1 P0-3 R11 H1120 */
				/*  RW U1 P0-3 R8  H3666 */
				/*  RW U1 P0-3 R22 H00FF */
				/*  RW U1 P0-3 R17 H0F0C */
				/*  RW U1 P0-3 R16 H2146 */
				/*  RW U1 P0-3 R17 Hc090 */
				/*  RW U1 P0-3 R16 H2147 */
				/*  RW U1 P0-3 R17 H0000 */
				/*  RW U1 P0-3 R16 H2000 */
				/*  RW U1 P0-3 R17 H6000 */
				/*  RW U1 P0-3 R16 H2143 */
				/*  RW U1 P0-3 R17 HC004 */
				/*  RW U1 P0-3 R16 H2100 */
				/*  RW U1 P0-3 R17 H49E8 */
				/*  RW U1 P0-3 R16 H2144 */
				/*  RW U1 P0-3 R17 H3180 */
				/*  RW U1 P0-3 R16 H2148 */
				/*  RW U1 P0-3 R17 HFC44 */
				/*  RW U1 P0-3 R16 H214B */
				/*  RW U1 P0-3 R17 H7FD2 */
				/*  RW U1 P0-3 R16 H214C */
				/*  RW U1 P0-3 R17 H2240 */
				/*  RW U1 P0-3 R16 H214D */
				/*  RW U1 P0-3 R17 H3008 */
				/*  RW U1 P0-3 R16 H214E */
				/*  RW U1 P0-3 R17 H3DF0 */
				/*  RW U1 P0-3 R16 H214F */
				/*  RW U1 P0-3 R22 H0000 */
				/*  RW U1 P0-3 R13 H0007 */
				/*  RW U1 P0-3 R14 H003C */
				/*  RW U1 P0-3 R13 H4007 */
				/*  RW U1 P0-3 R14 H0006 */
				/*  RW U1 P0-3 R22 H0000 */
				/*  RW U1 P0-3 R0  H9140 */
				/*  RW U1 P0 R22 H0010 */
				/*  RW U1 P0 R1  H03C1 */
				/*  RW U1 P0 R2  H0001 */
				/*  RW U1 P0 R3  H0000 */
				/*  RW U1 P0 R1  H0BC1 */
				/*  RW U1 P0 R2  H0001 */
				/*  RW U1 P0 R3  H0000 */
				/*  RW U1 P0 R1  H13C1 */
				/*  RW U1 P0 R2  H0001 */
				/*  RW U1 P0 R3  H0000 */
				/*  RW U1 P0 R1  H1BC1 */
				/*  RW U1 P0 R2  H0001 */
				/*  RW U1 P0 R3  H0000 */
				/* RW U1 P0 R22 H0000 */
				miiphy_write("eth0", i, 0x16, 0x0000);

				if (eeeEnable == 0) {
					/* Disable MACSec */
					miiphy_write("eth0", i, 0x16, 0x12);
					miiphy_read("eth0", i, 27, &reg);
					reg &= ~(1 << 13);
					miiphy_write("eth0", i, 27, reg);
				}
				miiphy_write("eth0", i, 0x16, 0x0000);
			}
			break;
		case A0:
			/* This script applies to 88E15XX MACSec Rev A0 PHY Initialization */
			/* Date: September 29, 2010 */
			/* This script is used as example to perform phy and macsec initialization for Port 0-3 */
			/* - MACSec Initialization */
			/* - PHY Initialization  */
			/* For ARUBA ONLY THIS IS VALID ONLY FOR A0 REVISION */
			/* RW u1 P0-3 R22 H0000 */
			miiphy_write("eth0", i, 0x16, 0x0000);
			/* --------------------------------------- */
			/* MACSec Initialization */
			/* --------------------------------------- */
			/* Disable drop_bad_tag bit for Port 0-3 */
			/* RW u1 P0 R22 H0010 */
			miiphy_write("eth0", i, 0x16, 0x0010);
			/* RW u1 P0 R1 H000b */
			miiphy_write("eth0", i, 0x1, 0x000b);
			/* RW u1 P0 R2 h0000 */
			miiphy_write("eth0", i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			miiphy_write("eth0", i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H080b */
			miiphy_write("eth0", i, 0x1, 0x080b);
			/* RW u1 P0 R2 h0000 */
			miiphy_write("eth0", i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			miiphy_write("eth0", i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H100b */
			miiphy_write("eth0", i, 0x1, 0x100b);
			/* RW u1 P0 R2 h0000 */
			miiphy_write("eth0", i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			miiphy_write("eth0", i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H180b */
			miiphy_write("eth0", i, 0x1, 0x180b);
			/* RW u1 P0 R2 h0000 */
			miiphy_write("eth0", i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			miiphy_write("eth0", i, 0x3, 0x0fb4);
			/* RW u1 P0 R22 H0000 */
			miiphy_write("eth0", i, 0x16, 0x0000);
			/* --------------------------------------- */
			/* PHY Initialization */
			/* --------------------------------------- */
			/* RW u1 P0-3 R22 h00FA */
			miiphy_write("eth0", i, 0x16, 0x00fa);
			/* RW u1 P0-3 R8 h0010 */
			miiphy_write("eth0", i, 0x8, 0x0010);
			/* RW u1 P0-3 R22 h00FB */
			miiphy_write("eth0", i, 0x16, 0x00fb);
			/* RW u1 P0-3 R1 h4099 */
			miiphy_write("eth0", i, 0x1, 0x4099);
			/* RW u1 P0-3 R3 h1120 */
			miiphy_write("eth0", i, 0x3, 0x1120);
			/* RW u1 P0-3 R11 h113C */
			miiphy_write("eth0", i, 0xb, 0x113c);
			/* RW u1 P0-3 R14 h8100 */
			miiphy_write("eth0", i, 0xe, 0x8100);
			/* RW u1 P0-3 R15 h112A */
			miiphy_write("eth0", i, 0xf, 0x112a);
			/* RW u1 P0-3 R22 h00FC */
			miiphy_write("eth0", i, 0x16, 0x00fc);
			/* RW u1 P0-3 R1 h20B0 */
			miiphy_write("eth0", i, 0x1, 0x20b0);
			/* RW u1 P0-3 R22 h00FF	 */
			miiphy_write("eth0", i, 0x16, 0x00ff);
			/* RW u1 P0-3 R17 h0000 */
			miiphy_write("eth0", i, 0x11, 0x0000);
			/* RW u1 P0-3 R16 h2000 */
			miiphy_write("eth0", i, 0x10, 0x2000);
			/* RW u1 P0-3 R17 h4444 */
			miiphy_write("eth0", i, 0x11, 0x4444);
			/* RW u1 P0-3 R16 h2140 */
			miiphy_write("eth0", i, 0x10, 0x2140);
			/* RW u1 P0-3 R17 h8064 */
			miiphy_write("eth0", i, 0x11, 0x8064);
			/* RW u1 P0-3 R16 h2141 */
			miiphy_write("eth0", i, 0x10, 0x2141);
			/* RW u1 P0-3 R17 h0108 */
			miiphy_write("eth0", i, 0x11, 0x0108);
			/* RW u1 P0-3 R16 h2144 */
			miiphy_write("eth0", i, 0x10, 0x2144);
			/* RW u1 P0-3 R17 h0F16 */
			miiphy_write("eth0", i, 0x11, 0x0f16);
			/* RW u1 P0-3 R16 h2146 */
			miiphy_write("eth0", i, 0x10, 0x2146);
			/* RW u1 P0-3 R17 h8C44 */
			miiphy_write("eth0", i, 0x11, 0x8c44);
			/* RW u1 P0-3 R16 h214B */
			miiphy_write("eth0", i, 0x10, 0x214b);
			/* RW u1 P0-3 R17 h0F90 */
			miiphy_write("eth0", i, 0x11, 0x0f90);
			/* RW u1 P0-3 R16 h214C */
			miiphy_write("eth0", i, 0x10, 0x214c);
			/* RW u1 P0-3 R17 hBA33 */
			miiphy_write("eth0", i, 0x11, 0xba33);
			/* RW u1 P0-3 R16 h214D */
			miiphy_write("eth0", i, 0x10, 0x214d);
			/* RW u1 P0-3 R17 h39AA */
			miiphy_write("eth0", i, 0x11, 0x39aa);
			/* RW u1 P0-3 R16 h214F */
			miiphy_write("eth0", i, 0x10, 0x214f);
			/* RW u1 P0-3 R17 h8433 */
			miiphy_write("eth0", i, 0x11, 0x8433);
			/* RW u1 P0-3 R16 h2151 */
			miiphy_write("eth0", i, 0x10, 0x2151);
			/* RW u1 P0-3 R17 h2010 */
			miiphy_write("eth0", i, 0x11, 0x2010);
			/* RW u1 P0-3 R16 h2152 */
			miiphy_write("eth0", i, 0x10, 0x2152);
			/* RW u1 P0-3 R17 h99EB */
			miiphy_write("eth0", i, 0x11, 0x99eb);
			/* RW u1 P0-3 R16 h2153 */
			miiphy_write("eth0", i, 0x10, 0x2153);
			/* RW u1 P0-3 R17 h2f3B */
			miiphy_write("eth0", i, 0x11, 0x2f3b);
			/* RW u1 P0-3 R16 h2154 */
			miiphy_write("eth0", i, 0x10, 0x2154);
			/* RW u1 P0-3 R17 h584E */
			miiphy_write("eth0", i, 0x11, 0x584e);
			/* RW u1 P0-3 R16 h2156 */
			miiphy_write("eth0", i, 0x10, 0x2156);
			/* RW u1 P0-3 R17 h1223 */
			miiphy_write("eth0", i, 0x11, 0x1223);
			/* RW u1 P0-3 R16 h2158 */
			miiphy_write("eth0", i, 0x10, 0x2158);
			/* RW u1 P0-3 R22 h0000 */
			miiphy_write("eth0", i, 0x16, 0x0000);

			/* wire mac: enable large frames */
			val = aruba_linkcrypt_read(i, 0x40);
			val &= ~0xfffc; /* 0x3fff << 2 */
			val |= (MAX_FRAME_SIZE << 2);
			aruba_linkcrypt_write(i, 0x40, val);

			/* system mac: enable large frames */
			val = aruba_linkcrypt_read(i, 0x50);
			val &= ~0xfffc; /* 0x3fff << 2 */
			val |= (MAX_FRAME_SIZE << 2);
			aruba_linkcrypt_write(i, 0x50, val);
#if 1
			/* per Marvell, this is needed for EEE (802.1az) */
			/* --------------------------------------- */
			/* Soft-Reset */
			/* --------------------------------------- */
			/* RW u1 P0-3 R22 H0000 */
			miiphy_write("eth0", i, 0x16, 0x0000);
			/* RW u1 P0-3 R0 H9140 */
			miiphy_write("eth0", i, 0x0, 0x9140);
			udelay(10000);
#endif
			break;
		case A1:
		default:
			miiphy_write("eth0", i, 0x16, 0x0000);
			/* --------------------------------------- */
			/* MACSec Initialization */
			/* --------------------------------------- */
			/* Disable drop_bad_tag bit for Port 0-3 */
			/* RW u1 P0 R22 H0010 */
			miiphy_write("eth0", i, 0x16, 0x0010);
			/* RW u1 P0 R1 H000b */
			miiphy_write("eth0", i, 0x1, 0x000b);
			/* RW u1 P0 R2 h0000 */
			miiphy_write("eth0", i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			miiphy_write("eth0", i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H080b */
			miiphy_write("eth0", i, 0x1, 0x080b);
			/* RW u1 P0 R2 h0000 */
			miiphy_write("eth0", i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			miiphy_write("eth0", i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H100b */
			miiphy_write("eth0", i, 0x1, 0x100b);
			/* RW u1 P0 R2 h0000 */
			miiphy_write("eth0", i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			miiphy_write("eth0", i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H180b */
			miiphy_write("eth0", i, 0x1, 0x180b);
			/* RW u1 P0 R2 h0000 */
			miiphy_write("eth0", i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			miiphy_write("eth0", i, 0x3, 0x0fb4);
			/* RW u1 P0 R22 H0000 */
			miiphy_write("eth0", i, 0x16, 0x0000);

			/* from rev A1 release notes dated 2012/07/11 */
			miiphy_write("eth0", i, 0x16, 0x00ff);
			miiphy_write("eth0", i, 0x11, 0x2148);
			miiphy_write("eth0", i, 0x10, 0x2144);
			miiphy_write("eth0", i, 0x11, 0xdc0c);
			miiphy_write("eth0", i, 0x10, 0x2159);

			miiphy_write("eth0", i, 0x16, 0x0000);

			/* wire mac: enable large frames */
			val = aruba_linkcrypt_read(i, 0x40);
			val &= ~0xfffc; /* 0x3fff << 2 */
			val |= (MAX_FRAME_SIZE << 2);
			aruba_linkcrypt_write(i, 0x40, val);

			/* system mac: enable large frames */
			val = aruba_linkcrypt_read(i, 0x50);
			val &= ~0xfffc; /* 0x3fff << 2 */
			val |= (MAX_FRAME_SIZE << 2);
			aruba_linkcrypt_write(i, 0x50, val);
#if 1
			/* per Marvell, this is needed for EEE (802.1az) */
			/* --------------------------------------- */
			/* Soft-Reset */
			/* --------------------------------------- */
			/* RW u1 P0-3 R22 H0000 */
			miiphy_write("eth0", i, 0x16, 0x0000);
			/* RW u1 P0-3 R0 H9140 */
			miiphy_write("eth0", i, 0x0, 0x9140);
			udelay(10000);
#endif
			break;
		}
#ifdef CONFIG_APBOOT_notyet
		printf(".");
#endif
	}
	/*  Sleep 3000 */
	udelay(300000);
#ifdef CONFIG_APBOOT_notyet
	printf("\b\b\b\bdone\n");
#endif
}
#endif	/* MV154x */

/*
 * Initialize all the TSEC devices
 *
 * Returns the number of TSEC devices that were initialized
 */
int tsec_eth_init(bd_t *bis, struct tsec_info_struct *tsecs, int num)
{
	int i;
	int ret, count = 0;

	for (i = 0; i < num; i++) {
		ret = tsec_initialize(bis, &tsecs[i]);
		if (ret > 0)
			count += ret;
	}
#ifdef CONFIG_PHY_MV154x
	tsec_154x_init(1);
#endif

	return count;
}

int tsec_standard_init(bd_t *bis)
{
	return tsec_eth_init(bis, tsec_info, ARRAY_SIZE(tsec_info));
}

/* Initialize device structure. Returns success if PHY
 * initialization succeeded (i.e. if it recognizes the PHY)
 */
static int tsec_initialize(bd_t * bis, struct tsec_info_struct *tsec_info)
{
	struct eth_device *dev;
	int i;
	struct tsec_private *priv;
#ifndef CONFIG_TOMATIN
	bd_t *bd = gd->bd;
#endif

	dev = (struct eth_device *)malloc(sizeof *dev);

	if (NULL == dev)
		return 0;

	memset(dev, 0, sizeof *dev);

	priv = (struct tsec_private *)malloc(sizeof(*priv));

	if (NULL == priv)
		return 0;

	privlist[num_tsecs++] = priv;
	priv->regs = tsec_info->regs;
	priv->phyregs = tsec_info->miiregs;
	priv->phyregs_sgmii = tsec_info->miiregs_sgmii;

	priv->phyaddr = tsec_info->phyaddr;
	priv->flags = tsec_info->flags;

	sprintf(dev->name, tsec_info->devname);
	dev->iobase = 0;
	dev->priv = priv;
	dev->init = tsec_init;
	dev->halt = tsec_halt;
	dev->send = tsec_send;
	dev->recv = tsec_recv;
#ifdef CONFIG_MCAST_TFTP
	dev->mcast = tsec_mcast_addr;
#endif

#ifndef CONFIG_APBOOT
	/* Tell u-boot to get the addr from the env */
	for (i = 0; i < 6; i++)
		dev->enetaddr[i] = 0;
#else
	{
		char *addr;
		char *tmp;
		char *end;

		addr = getenv("ethaddr");

		tmp = addr;
		for (i=0; i<6; i++) {
			dev->enetaddr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
			if (tmp) {
				tmp = (*end) ? end + 1 : end;
			}
		}

#ifndef CONFIG_TOMATIN
		if (strcmp(dev->name, "eth1") == 0) {
			/*
			 * this assumes that manufacturing always assigns an 
			 * even base MAC, which currently has to be true or 
			 * the radio driver has issues
			 *
			 * if we have more than two ports, we'll just ping-pong
			 * to avoid incrementing the rest of the address; 
			 * I don't think this will happen anytime soon
			 */
			dev->enetaddr[5] += 1;
			/* this will be given to Linux via the device tree */
			memcpy(bd->bi_enet1addr, dev->enetaddr, 6);
		}
#endif
	}
#if 0
printf("NAME %s, addr %02x:%02x:%02x:%02x:%02x:%02x\n",
dev->name, dev->enetaddr[0],
dev->enetaddr[1],
dev->enetaddr[2],
dev->enetaddr[3],
dev->enetaddr[4],
dev->enetaddr[5]);
#endif

#endif
#ifdef TSEC_HIDE_ETH2
	/* we don't register TSEC1 since we don't use it */
	if (strcmp(dev->name, "eth2") != 0)
#endif
	eth_register(dev);

	/* Reset the MAC */
	priv->regs->maccfg1 |= MACCFG1_SOFT_RESET;
	udelay(2);  /* Soft Reset must be asserted for 3 TX clocks */
	priv->regs->maccfg1 &= ~(MACCFG1_SOFT_RESET);

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII) \
	&& !defined(BITBANGMII)
#ifdef TSEC_HIDE_ETH2
	/* we don't register TSEC1 since we don't use it */
	if (strcmp(dev->name, "eth2") != 0)
#endif
	miiphy_register(dev->name, tsec_miiphy_read, tsec_miiphy_write);
#endif

	/* Try to initialize PHY here, and return */
	return init_phy(dev);
}

/* Initializes data structures and registers for the controller,
 * and brings the interface up.	 Returns the link status, meaning
 * that it returns success if the link is up, failure otherwise.
 * This allows u-boot to find the first active controller.
 */
static int tsec_init(struct eth_device *dev, bd_t * bd)
{
	uint tempval;
	char tmpbuf[MAC_ADDR_LEN];
	int i;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	/* Make sure the controller is stopped */
	tsec_halt(dev);

	/* Init MACCFG2.  Defaults to GMII */
	regs->maccfg2 = MACCFG2_INIT_SETTINGS;

	/* Init ECNTRL */
	regs->ecntrl = ECNTRL_INIT_SETTINGS;

	/* Copy the station address into the address registers.
	 * Backwards, because little endian MACS are dumb */
	for (i = 0; i < MAC_ADDR_LEN; i++) {
		tmpbuf[MAC_ADDR_LEN - 1 - i] = dev->enetaddr[i];
	}
	tempval = (tmpbuf[0] << 24) | (tmpbuf[1] << 16) | (tmpbuf[2] << 8) |
		  tmpbuf[3];

	regs->macstnaddr1 = tempval;

	tempval = *((uint *) (tmpbuf + 4));

	regs->macstnaddr2 = tempval;

	/* Clear out (for the most part) the other registers */
	init_registers(regs);

	/* Ready the device for tx/rx */
	startup_tsec(dev);

	/* If there's no link, fail */
	return (priv->link ? 1 : 0);	/* test is inverted from 2010.12 */
}

/* Writes the given phy's reg with value, using the specified MDIO regs */
static void tsec_local_mdio_write(volatile tsec_mdio_t *phyregs, uint addr,
		uint reg, uint value)
{
	int timeout = 1000000;

	phyregs->miimadd = (addr << 8) | reg;
	phyregs->miimcon = value;
	asm("sync");

	timeout = 1000000;
	while ((phyregs->miimind & MIIMIND_BUSY) && timeout--) ;
}


/* Provide the default behavior of writing the PHY of this ethernet device */
#define write_phy_reg(priv, regnum, value) \
	tsec_local_mdio_write(priv->phyregs,priv->phyaddr,regnum,value)

/* Reads register regnum on the device's PHY through the
 * specified registers.	 It lowers and raises the read
 * command, and waits for the data to become valid (miimind
 * notvalid bit cleared), and the bus to cease activity (miimind
 * busy bit cleared), and then returns the value
 */
static uint tsec_local_mdio_read(volatile tsec_mdio_t *phyregs,
				uint phyid, uint regnum)
{
	uint value;

	/* Put the address of the phy, and the register
	 * number into MIIMADD */
	phyregs->miimadd = (phyid << 8) | regnum;

	/* Clear the command register, and wait */
	phyregs->miimcom = 0;
	asm("sync");

	/* Initiate a read command, and wait */
	phyregs->miimcom = MIIM_READ_COMMAND;
	asm("sync");

	/* Wait for the the indication that the read is done */
	while ((phyregs->miimind & (MIIMIND_NOTVALID | MIIMIND_BUSY))) ;

	/* Grab the value read from the PHY */
	value = phyregs->miimstat;

	return value;
}

/* #define to provide old read_phy_reg functionality without duplicating code */
#define read_phy_reg(priv,regnum) \
	tsec_local_mdio_read(priv->phyregs,priv->phyaddr,regnum)

#ifndef CONFIG_SYS_TBIANA_SETTINGS
#define TBIANA_SETTINGS ( \
		TBIANA_ASYMMETRIC_PAUSE \
		| TBIANA_SYMMETRIC_PAUSE \
		| TBIANA_FULL_DUPLEX \
		)
#else
#define TBIANA_SETTINGS CONFIG_SYS_TBIANA_SETTINGS
#endif

/* By default force the TBI PHY into 1000Mbps full duplex when in SGMII mode */
#ifndef CONFIG_TSEC_TBICR_SETTINGS
#define CONFIG_TSEC_TBICR_SETTINGS ( \
		TBICR_PHY_RESET \
		| TBICR_ANEG_ENABLE \
		| TBICR_FULL_DUPLEX \
		| TBICR_SPEED1_SET \
		)
#endif /* CONFIG_TSEC_TBICR_SETTINGS */

#ifndef CONFIG_TOMATIN
/* Configure the TBI for SGMII operation */
static void tsec_configure_serdes(struct tsec_private *priv)
{
	/* Access TBI PHY registers at given TSEC register offset as opposed
	 * to the register offset used for external PHY accesses */
	tsec_local_mdio_write(priv->phyregs_sgmii, priv->regs->tbipa, TBI_ANA,
			TBIANA_SETTINGS);
	tsec_local_mdio_write(priv->phyregs_sgmii, priv->regs->tbipa, TBI_TBICON,
			TBICON_CLK_SELECT);
	tsec_local_mdio_write(priv->phyregs_sgmii, priv->regs->tbipa, TBI_CR,
			CONFIG_TSEC_TBICR_SETTINGS);
}
#endif

/* Discover which PHY is attached to the device, and configure it
 * properly.  If the PHY is not recognized, then return 0
 * (failure).  Otherwise, return 1
 */
static int init_phy(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	struct phy_info *curphy;
	volatile tsec_t *regs = priv->regs;

	/* Assign a Physical address to the TBI */
	regs->tbipa = CONFIG_SYS_TBIPA_VALUE;
	asm("sync");

	/* Reset MII (due to new addresses) */
	priv->phyregs->miimcfg = MIIMCFG_RESET;
	asm("sync");
	priv->phyregs->miimcfg = MIIMCFG_INIT_VALUE;
	asm("sync");
	while (priv->phyregs->miimind & MIIMIND_BUSY) ;

	if (0 == relocated)
		relocate_cmds();

	/* Get the cmd structure corresponding to the attached
	 * PHY */
	curphy = get_phy_info(dev);

	if (curphy == NULL) {
		priv->phyinfo = NULL;
		printf("%s: No PHY found\n", dev->name);

		return 0;
	}

#ifndef CONFIG_TOMATIN
	if (regs->ecntrl & ECNTRL_SGMII_MODE)
		tsec_configure_serdes(priv);
#endif

	priv->phyinfo = curphy;

	phy_run_commands(priv, priv->phyinfo->config);

	return 1;
}

/*
 * Returns which value to write to the control register.
 * For 10/100, the value is slightly different
 */
static uint mii_cr_init(uint mii_reg, struct tsec_private * priv)
{
	if (priv->flags & TSEC_GIGABIT)
		return MIIM_CONTROL_INIT;
	else
		return MIIM_CR_INIT;
}

/*
 * Wait for auto-negotiation to complete, then determine link
 */
static uint mii_parse_sr(uint mii_reg, struct tsec_private * priv)
{
#ifndef CONFIG_APBOOT
	/*
	 * Wait if the link is up, and autonegotiation is in progress
	 * (ie - we're capable and it's not done)
	 */
	mii_reg = read_phy_reg(priv, MIIM_STATUS);
	if ((mii_reg & PHY_BMSR_AUTN_ABLE) && !(mii_reg & PHY_BMSR_AUTN_COMP)) {
		int i = 0;

		puts("Waiting for PHY auto negotiation to complete");
		while (!(mii_reg & PHY_BMSR_AUTN_COMP)) {
			/*
			 * Timeout reached ?
			 */
			if (i > PHY_AUTONEGOTIATE_TIMEOUT) {
				puts(" TIMEOUT !\n");
				priv->link = 0;
				return 0;
			}

			if (ctrlc()) {
				puts("user interrupt!\n");
				priv->link = 0;
				return -EINTR;
			}

			if ((i++ % 1000) == 0) {
				putc('.');
			}
			udelay(1000);	/* 1 ms */
			mii_reg = read_phy_reg(priv, MIIM_STATUS);
		}
		puts(" done\n");

		/* Link status bit is latched low, read it again */
		mii_reg = read_phy_reg(priv, MIIM_STATUS);

		udelay(500000);	/* another 500 ms (results in faster booting) */
	}

	priv->link = mii_reg & MIIM_STATUS_LINK ? 1 : 0;

	return 0;
#else
	/* read twice to clear latched value */
	mii_reg = read_phy_reg(priv, MIIM_STATUS);
	mii_reg = read_phy_reg(priv, MIIM_STATUS);
	priv->link = mii_reg & MIIM_STATUS_LINK ? 1 : 0;
	return 0;
#endif
}

/* Generic function which updates the speed and duplex.  If
 * autonegotiation is enabled, it uses the AND of the link
 * partner's advertised capabilities and our advertised
 * capabilities.  If autonegotiation is disabled, we use the
 * appropriate bits in the control register.
 *
 * Stolen from Linux's mii.c and phy_device.c
 */
static uint mii_parse_link(uint mii_reg, struct tsec_private *priv)
{
	/* We're using autonegotiation */
	if (mii_reg & PHY_BMSR_AUTN_ABLE) {
		uint lpa = 0;
		uint gblpa = 0;

		/* Check for gigabit capability */
		if (mii_reg & PHY_BMSR_EXT) {
			/* We want a list of states supported by
			 * both PHYs in the link
			 */
			gblpa = read_phy_reg(priv, PHY_1000BTSR);
			gblpa &= read_phy_reg(priv, PHY_1000BTCR) << 2;
		}

		/* Set the baseline so we only have to set them
		 * if they're different
		 */
		priv->speed = 10;
		priv->duplexity = 0;

		/* Check the gigabit fields */
		if (gblpa & (PHY_1000BTSR_1000FD | PHY_1000BTSR_1000HD)) {
			priv->speed = 1000;

			if (gblpa & PHY_1000BTSR_1000FD)
				priv->duplexity = 1;

			/* We're done! */
			return 0;
		}

		lpa = read_phy_reg(priv, PHY_ANAR);
		lpa &= read_phy_reg(priv, PHY_ANLPAR);

		if (lpa & (PHY_ANLPAR_TXFD | PHY_ANLPAR_TX)) {
			priv->speed = 100;

			if (lpa & PHY_ANLPAR_TXFD)
				priv->duplexity = 1;

		} else if (lpa & PHY_ANLPAR_10FD)
			priv->duplexity = 1;
	} else {
		uint bmcr = read_phy_reg(priv, PHY_BMCR);

		priv->speed = 10;
		priv->duplexity = 0;

		if (bmcr & PHY_BMCR_DPLX)
			priv->duplexity = 1;

		if (bmcr & PHY_BMCR_1000_MBPS)
			priv->speed = 1000;
		else if (bmcr & PHY_BMCR_100_MBPS)
			priv->speed = 100;
	}

	return 0;
}

/*
 * "Ethernet@Wirespeed" needs to be enabled to achieve link in certain
 * circumstances.  eg a gigabit TSEC connected to a gigabit switch with
 * a 4-wire ethernet cable.  Both ends advertise gigabit, but can't
 * link.  "Ethernet@Wirespeed" reduces advertised speed until link
 * can be achieved.
 */
static uint mii_BCM54xx_wirespeed(uint mii_reg, struct tsec_private *priv)
{
	return (read_phy_reg(priv, mii_reg) & 0x8FFF) | 0x8010;
}

/*
 * Parse the BCM54xx status register for speed and duplex information.
 * The linux sungem_phy has this information, but in a table format.
 */
static uint mii_parse_BCM54xx_sr(uint mii_reg, struct tsec_private *priv)
{
	/* If there is no link, speed and duplex don't matter */
	if (!priv->link)
		return 0;

	switch ((mii_reg & MIIM_BCM54xx_AUXSTATUS_LINKMODE_MASK) >>
		MIIM_BCM54xx_AUXSTATUS_LINKMODE_SHIFT) {
	case 1:
		priv->duplexity = 0;
		priv->speed = 10;
		break;
	case 2:
		priv->duplexity = 1;
		priv->speed = 10;
		break;
	case 3:
		priv->duplexity = 0;
		priv->speed = 100;
		break;
	case 5:
		priv->duplexity = 1;
		priv->speed = 100;
		break;
	case 6:
		priv->duplexity = 0;
		priv->speed = 1000;
		break;
	case 7:
		priv->duplexity = 1;
		priv->speed = 1000;
		break;
	default:
		printf("Auto-neg error, defaulting to 10BT/HD\n");
		priv->duplexity = 0;
		priv->speed = 10;
		break;
	}

	return 0;
}

/*
 * Find out if PHY is in copper or serdes mode by looking at Expansion Reg
 * 0x42 - "Operating Mode Status Register"
 */
static int BCM8482_is_serdes(struct tsec_private *priv)
{
	u16 val;
	int serdes = 0;

	write_phy_reg(priv, MIIM_BCM54XX_EXP_SEL, MIIM_BCM54XX_EXP_SEL_ER | 0x42);
	val = read_phy_reg(priv, MIIM_BCM54XX_EXP_DATA);

	switch (val & 0x1f) {
	case 0x0d:	/* RGMII-to-100Base-FX */
	case 0x0e:	/* RGMII-to-SGMII */
	case 0x0f:	/* RGMII-to-SerDes */
	case 0x12:	/* SGMII-to-SerDes */
	case 0x13:	/* SGMII-to-100Base-FX */
	case 0x16:	/* SerDes-to-Serdes */
		serdes = 1;
		break;
	case 0x6:	/* RGMII-to-Copper */
	case 0x14:	/* SGMII-to-Copper */
	case 0x17:	/* SerDes-to-Copper */
		break;
	default:
		printf("ERROR, invalid PHY mode (0x%x\n)", val);
		break;
	}

	return serdes;
}

/*
 * Determine SerDes link speed and duplex from Expansion reg 0x42 "Operating
 * Mode Status Register"
 */
uint mii_parse_BCM5482_serdes_sr(struct tsec_private *priv)
{
	u16 val;
	int i = 0;

	/* Wait 1s for link - Clause 37 autonegotiation happens very fast */
	while (1) {
		write_phy_reg(priv, MIIM_BCM54XX_EXP_SEL,
				MIIM_BCM54XX_EXP_SEL_ER | 0x42);
		val = read_phy_reg(priv, MIIM_BCM54XX_EXP_DATA);

		if (val & 0x8000)
			break;

		if (i++ > 1000) {
			priv->link = 0;
			return 1;
		}

		udelay(1000);	/* 1 ms */
	}

	priv->link = 1;
	switch ((val >> 13) & 0x3) {
	case (0x00):
		priv->speed = 10;
		break;
	case (0x01):
		priv->speed = 100;
		break;
	case (0x02):
		priv->speed = 1000;
		break;
	}

	priv->duplexity = (val & 0x1000) == 0x1000;

	return 0;
}

/*
 * Figure out if BCM5482 is in serdes or copper mode and determine link
 * configuration accordingly
 */
static uint mii_parse_BCM5482_sr(uint mii_reg, struct tsec_private *priv)
{
	if (BCM8482_is_serdes(priv)) {
		mii_parse_BCM5482_serdes_sr(priv);
		priv->flags |= TSEC_FIBER;
	} else {
		/* Wait for auto-negotiation to complete or fail */
		mii_parse_sr(mii_reg, priv);

		/* Parse BCM54xx copper aux status register */
		mii_reg = read_phy_reg(priv, MIIM_BCM54xx_AUXSTATUS);
		mii_parse_BCM54xx_sr(mii_reg, priv);
	}

	return 0;
}

/* Parse the 88E1011's status register for speed and duplex
 * information
 */
static uint mii_parse_88E1011_psr(uint mii_reg, struct tsec_private * priv)
{
	uint speed;

	mii_reg = read_phy_reg(priv, MIIM_88E1011_PHY_STATUS);

	if ((mii_reg & MIIM_88E1011_PHYSTAT_LINK) &&
		!(mii_reg & MIIM_88E1011_PHYSTAT_SPDDONE)) {
		int i = 0;

		puts("Waiting for PHY realtime link");
		while (!(mii_reg & MIIM_88E1011_PHYSTAT_SPDDONE)) {
			/* Timeout reached ? */
			if (i > PHY_AUTONEGOTIATE_TIMEOUT) {
				puts(" TIMEOUT !\n");
				priv->link = 0;
				break;
			}

			if ((i++ % 1000) == 0) {
				putc('.');
			}
			udelay(1000);	/* 1 ms */
			mii_reg = read_phy_reg(priv, MIIM_88E1011_PHY_STATUS);
		}
		puts(" done\n");
		udelay(500000);	/* another 500 ms (results in faster booting) */
	} else {
		if (mii_reg & MIIM_88E1011_PHYSTAT_LINK)
			priv->link = 1;
		else
			priv->link = 0;
	}

	if (mii_reg & MIIM_88E1011_PHYSTAT_DUPLEX)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	speed = (mii_reg & MIIM_88E1011_PHYSTAT_SPEED);

	switch (speed) {
	case MIIM_88E1011_PHYSTAT_GBIT:
		priv->speed = 1000;
		break;
	case MIIM_88E1011_PHYSTAT_100:
		priv->speed = 100;
		break;
	default:
		priv->speed = 10;
	}

	return 0;
}

/* Parse the RTL8211B's status register for speed and duplex
 * information
 */
static uint mii_parse_RTL8211B_sr(uint mii_reg, struct tsec_private * priv)
{
	uint speed;

	mii_reg = read_phy_reg(priv, MIIM_RTL8211B_PHY_STATUS);
	if (!(mii_reg & MIIM_RTL8211B_PHYSTAT_SPDDONE)) {
		int i = 0;

		/* in case of timeout ->link is cleared */
		priv->link = 1;
		puts("Waiting for PHY realtime link");
		while (!(mii_reg & MIIM_RTL8211B_PHYSTAT_SPDDONE)) {
			/* Timeout reached ? */
			if (i > PHY_AUTONEGOTIATE_TIMEOUT) {
				puts(" TIMEOUT !\n");
				priv->link = 0;
				break;
			}

			if ((i++ % 1000) == 0) {
				putc('.');
			}
			udelay(1000);	/* 1 ms */
			mii_reg = read_phy_reg(priv, MIIM_RTL8211B_PHY_STATUS);
		}
		puts(" done\n");
		udelay(500000);	/* another 500 ms (results in faster booting) */
	} else {
		if (mii_reg & MIIM_RTL8211B_PHYSTAT_LINK)
			priv->link = 1;
		else
			priv->link = 0;
	}

	if (mii_reg & MIIM_RTL8211B_PHYSTAT_DUPLEX)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	speed = (mii_reg & MIIM_RTL8211B_PHYSTAT_SPEED);

	switch (speed) {
	case MIIM_RTL8211B_PHYSTAT_GBIT:
		priv->speed = 1000;
		break;
	case MIIM_RTL8211B_PHYSTAT_100:
		priv->speed = 100;
		break;
	default:
		priv->speed = 10;
	}

	return 0;
}

/* Parse the cis8201's status register for speed and duplex
 * information
 */
static uint mii_parse_cis8201(uint mii_reg, struct tsec_private * priv)
{
	uint speed;

	if (mii_reg & MIIM_CIS8201_AUXCONSTAT_DUPLEX)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	speed = mii_reg & MIIM_CIS8201_AUXCONSTAT_SPEED;
	switch (speed) {
	case MIIM_CIS8201_AUXCONSTAT_GBIT:
		priv->speed = 1000;
		break;
	case MIIM_CIS8201_AUXCONSTAT_100:
		priv->speed = 100;
		break;
	default:
		priv->speed = 10;
		break;
	}

	return 0;
}

/* Parse the vsc8244's status register for speed and duplex
 * information
 */
static uint mii_parse_vsc8244(uint mii_reg, struct tsec_private * priv)
{
	uint speed;

	if (mii_reg & MIIM_VSC8244_AUXCONSTAT_DUPLEX)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	speed = mii_reg & MIIM_VSC8244_AUXCONSTAT_SPEED;
	switch (speed) {
	case MIIM_VSC8244_AUXCONSTAT_GBIT:
		priv->speed = 1000;
		break;
	case MIIM_VSC8244_AUXCONSTAT_100:
		priv->speed = 100;
		break;
	default:
		priv->speed = 10;
		break;
	}

	return 0;
}

/* Parse the DM9161's status register for speed and duplex
 * information
 */
static uint mii_parse_dm9161_scsr(uint mii_reg, struct tsec_private * priv)
{
	if (mii_reg & (MIIM_DM9161_SCSR_100F | MIIM_DM9161_SCSR_100H))
		priv->speed = 100;
	else
		priv->speed = 10;

	if (mii_reg & (MIIM_DM9161_SCSR_100F | MIIM_DM9161_SCSR_10F))
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	return 0;
}

/* Parse the 88E1512's status register for speed and duplex
 * information
 */
static uint mii_parse_88E1512_psr(uint mii_reg, struct tsec_private * priv)
{
	uint speed;

	mii_reg = read_phy_reg(priv, MIIM_88E1512_PHY_STATUS);

	if ((mii_reg & MIIM_88E1512_PHYSTAT_LINK) &&
		!(mii_reg & MIIM_88E1512_PHYSTAT_SPDDONE)) {
		int i = 0;

		puts("Waiting for PHY realtime link");
		while (!(mii_reg & MIIM_88E1512_PHYSTAT_SPDDONE)) {
			/* Timeout reached ? */
			if (i > PHY_AUTONEGOTIATE_TIMEOUT) {
				puts(" TIMEOUT !\n");
				priv->link = 0;
				break;
			}

			if ((i++ % 1000) == 0) {
				putc('.');
			}
			udelay(1000);	/* 1 ms */
			mii_reg = read_phy_reg(priv, MIIM_88E1512_PHY_STATUS);
		}
		puts(" done\n");
		udelay(500000);	/* another 500 ms (results in faster booting) */
	} else {
		if (mii_reg & MIIM_88E1512_PHYSTAT_LINK)
			priv->link = 1;
		else
			priv->link = 0;
	}

	if (mii_reg & MIIM_88E1512_PHYSTAT_DUPLEX)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	speed = (mii_reg & MIIM_88E1512_PHYSTAT_SPEED);

	switch (speed) {
	case MIIM_88E1512_PHYSTAT_GBIT:
		priv->speed = 1000;
		break;
	case MIIM_88E1512_PHYSTAT_100:
		priv->speed = 100;
		break;
	default:
		priv->speed = 10;
	}

	return 0;
}

/*
 * Hack to write all 4 PHYs with the LED values
 */
static uint mii_cis8204_fixled(uint mii_reg, struct tsec_private * priv)
{
	uint phyid;
	volatile tsec_mdio_t *regbase = priv->phyregs;
	int timeout = 1000000;

	for (phyid = 0; phyid < 4; phyid++) {
		regbase->miimadd = (phyid << 8) | mii_reg;
		regbase->miimcon = MIIM_CIS8204_SLEDCON_INIT;
		asm("sync");

		timeout = 1000000;
		while ((regbase->miimind & MIIMIND_BUSY) && timeout--) ;
	}

	return MIIM_CIS8204_SLEDCON_INIT;
}

static uint mii_cis8204_setmode(uint mii_reg, struct tsec_private * priv)
{
	if (priv->flags & TSEC_REDUCED)
		return MIIM_CIS8204_EPHYCON_INIT | MIIM_CIS8204_EPHYCON_RGMII;
	else
		return MIIM_CIS8204_EPHYCON_INIT;
}

static uint mii_m88e1111s_setmode(uint mii_reg, struct tsec_private *priv)
{
	uint mii_data = read_phy_reg(priv, mii_reg);

	if (priv->flags & TSEC_REDUCED)
		mii_data = (mii_data & 0xfff0) | 0x000b;
	return mii_data;
}

/* Initialized required registers to appropriate values, zeroing
 * those we don't care about (unless zero is bad, in which case,
 * choose a more appropriate value)
 */
static void init_registers(volatile tsec_t * regs)
{
	/* Clear IEVENT */
	regs->ievent = IEVENT_INIT_CLEAR;

	regs->imask = IMASK_INIT_CLEAR;

	regs->hash.iaddr0 = 0;
	regs->hash.iaddr1 = 0;
	regs->hash.iaddr2 = 0;
	regs->hash.iaddr3 = 0;
	regs->hash.iaddr4 = 0;
	regs->hash.iaddr5 = 0;
	regs->hash.iaddr6 = 0;
	regs->hash.iaddr7 = 0;

	regs->hash.gaddr0 = 0;
	regs->hash.gaddr1 = 0;
	regs->hash.gaddr2 = 0;
	regs->hash.gaddr3 = 0;
	regs->hash.gaddr4 = 0;
	regs->hash.gaddr5 = 0;
	regs->hash.gaddr6 = 0;
	regs->hash.gaddr7 = 0;

	regs->rctrl = 0x00000000;

	/* Init RMON mib registers */
	memset((void *)&(regs->rmon), 0, sizeof(rmon_mib_t));

	regs->rmon.cam1 = 0xffffffff;
	regs->rmon.cam2 = 0xffffffff;

	regs->mrblr = MRBLR_INIT_SETTINGS;

	regs->minflr = MINFLR_INIT_SETTINGS;

	regs->attr = ATTR_INIT_SETTINGS;
	regs->attreli = ATTRELI_INIT_SETTINGS;

}

/* Configure maccfg2 based on negotiated speed and duplex
 * reported by PHY handling code
 */
static void adjust_link(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
	char *speedstr = "unk";

	if (priv->link) {
		if (priv->duplexity != 0)
			regs->maccfg2 |= MACCFG2_FULL_DUPLEX;
		else
			regs->maccfg2 &= ~(MACCFG2_FULL_DUPLEX);

		switch (priv->speed) {
		case 1000:
			regs->maccfg2 = ((regs->maccfg2 & ~(MACCFG2_IF))
					 | MACCFG2_GMII);
			speedstr = "1 Gb/s";
			break;
		case 100:
		case 10:
			regs->maccfg2 = ((regs->maccfg2 & ~(MACCFG2_IF))
					 | MACCFG2_MII);

			/* Set R100 bit in all modes although
			 * it is only used in RGMII mode
			 */
			if (priv->speed == 100) {
				speedstr = "100 Mb/s";
				regs->ecntrl |= ECNTRL_R100;
			} else {
				speedstr = "10 Mb/s";
				regs->ecntrl &= ~(ECNTRL_R100);
			}
			break;
		default:
			printf("%s: Speed was bad\n", dev->name);
			break;
		}

		printf("%s: link up, speed %s, %s duplex%s\n", 
			dev->name, speedstr,
		       (priv->duplexity) ? "full" : "half",
		       (priv->flags & TSEC_FIBER) ? ", fiber mode" : "");

	} else {
		printf("%s: link down.\n", dev->name);
	}
}

#ifdef CONFIG_SYS_FSL_ERRATUM_NMG_ETSEC129
/*
 * When MACCFG1[Rx_EN] is enabled during system boot as part
 * of the eTSEC port initialization sequence,
 * the eTSEC Rx logic may not be properly initialized.
 */
void redundant_init(struct eth_device *dev)
{
	struct tsec_private *priv = dev->priv;
	tsec_t *regs = (tsec_t *)priv->regs;
	uint t, count = 0;
	int fail = 1;
	static const u8 pkt[] = {
		0x00, 0x1e, 0x4f, 0x12, 0xcb, 0x2c, 0x00, 0x25,
		0x64, 0xbb, 0xd1, 0xab, 0x08, 0x00, 0x45, 0x00,
		0x00, 0x5c, 0xdd, 0x22, 0x00, 0x00, 0x80, 0x01,
		0x1f, 0x71, 0x0a, 0xc1, 0x14, 0x22, 0x0a, 0xc1,
		0x14, 0x6a, 0x08, 0x00, 0xef, 0x7e, 0x02, 0x00,
		0x94, 0x05, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
		0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e,
		0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
		0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
		0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
		0x71, 0x72};

	/* Enable promiscuous mode */
	setbits_be32(&regs->rctrl, 0x8);
	/* Enable loopback mode */
	setbits_be32(&regs->maccfg1, MACCFG1_LOOPBACK);
	/* Enable transmit and receive */
	setbits_be32(&regs->maccfg1, MACCFG1_RX_EN | MACCFG1_TX_EN);

	/* Tell the DMA it is clear to go */
	setbits_be32(&regs->dmactrl, DMACTRL_INIT_SETTINGS);
	out_be32(&regs->tstat, TSTAT_CLEAR_THALT);
	out_be32(&regs->rstat, RSTAT_CLEAR_RHALT);
	clrbits_be32(&regs->dmactrl, DMACTRL_GRS | DMACTRL_GTS);

	do {
		tsec_send(dev, (void *)pkt, sizeof(pkt));

		/* Wait for buffer to be received */
		for (t = 0; priv->rxbd[priv->rx_idx].status & RXBD_EMPTY; t++) {
			if (t >= 10 * TOUT_LOOP) {
				printf("%s: tsec: rx error\n", dev->name);
				break;
			}
		}

		if (!memcmp(pkt, (void *)NetRxPackets[priv->rx_idx], sizeof(pkt)))
			fail = 0;

		priv->rxbd[priv->rx_idx].length = 0;
		priv->rxbd[priv->rx_idx].status =
		    RXBD_EMPTY | (((priv->rx_idx + 1) == PKTBUFSRX) ? RXBD_WRAP : 0);
		priv->rx_idx = (priv->rx_idx + 1) % PKTBUFSRX;

		if (in_be32(&regs->ievent) & IEVENT_BSY) {
			out_be32(&regs->ievent, IEVENT_BSY);
			out_be32(&regs->rstat, RSTAT_CLEAR_RHALT);
		}
		if (fail) {
			printf("loopback recv packet error!\n");
			clrbits_be32(&regs->maccfg1, MACCFG1_RX_EN);
			udelay(1000);
			setbits_be32(&regs->maccfg1, MACCFG1_RX_EN);
		}
	} while ((count++ < 4) && (fail == 1));

	if (fail)
		panic("eTSEC init fail!\n");
	/* Disable promiscuous mode */
	clrbits_be32(&regs->rctrl, 0x8);
	/* Disable loopback mode */
	clrbits_be32(&regs->maccfg1, MACCFG1_LOOPBACK);
}
#endif

/* Set up the buffers and their descriptors, and bring up the
 * interface
 */
static void startup_tsec(struct eth_device *dev)
{
	int i;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
#ifdef CONFIG_SYS_FSL_ERRATUM_NMG_ETSEC129
	uint svr;
#endif

	/* reset the indices to zero */
	priv->rx_idx = 0;
	priv->tx_idx = 0;

	/* Point to the buffer descriptors */
	regs->tbase = (unsigned int)(&priv->txbd[priv->tx_idx]);
	regs->rbase = (unsigned int)(&priv->rxbd[priv->rx_idx]);

	/* Initialize the Rx Buffer descriptors */
	for (i = 0; i < PKTBUFSRX; i++) {
		priv->rxbd[i].status = RXBD_EMPTY;
		priv->rxbd[i].length = 0;
		priv->rxbd[i].bufPtr = (uint) NetRxPackets[i];
	}
	priv->rxbd[PKTBUFSRX - 1].status |= RXBD_WRAP;

	/* Initialize the TX Buffer Descriptors */
	for (i = 0; i < TX_BUF_CNT; i++) {
		priv->txbd[i].status = 0;
		priv->txbd[i].length = 0;
		priv->txbd[i].bufPtr = 0;
	}
	priv->txbd[TX_BUF_CNT - 1].status |= TXBD_WRAP;

	/* Start up the PHY */
	if(priv->phyinfo)
		phy_run_commands(priv, priv->phyinfo->startup);

	adjust_link(dev);

#ifdef CONFIG_SYS_FSL_ERRATUM_NMG_ETSEC129
	svr = get_svr();
	if ((SVR_MAJ(svr) == 1) || IS_SVR_REV(svr, 2, 0))
		redundant_init(dev);
#endif
	/* Enable Transmit and Receive */
	regs->maccfg1 |= (MACCFG1_RX_EN | MACCFG1_TX_EN);

	/* Tell the DMA it is clear to go */
	regs->dmactrl |= DMACTRL_INIT_SETTINGS;
	regs->tstat = TSTAT_CLEAR_THALT;
	regs->rstat = RSTAT_CLEAR_RHALT;
	regs->dmactrl &= ~(DMACTRL_GRS | DMACTRL_GTS);
}

/* This returns the status bits of the device.	The return value
 * is never checked, and this is what the 8260 driver did, so we
 * do the same.	 Presumably, this would be zero if there were no
 * errors
 */
int tsec_send(struct eth_device *dev, volatile void *packet, int length)
{
	int i;
	int result = 0;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;
#ifdef CONFIG_APBOOT
	int link, duplexity, speed;
#endif

#ifdef CONFIG_APBOOT
	link = priv->link;
	duplexity = priv->duplexity;
	speed = priv->speed;

	if(priv->phyinfo)
		phy_run_commands(priv, priv->phyinfo->startup);
	if (priv->link != link || priv->speed != speed || priv->duplexity != duplexity) {
		adjust_link(dev);
	}
#endif
	/* Find an empty buffer descriptor */
	for (i = 0; priv->txbd[priv->tx_idx].status & TXBD_READY; i++) {
		if (i >= TOUT_LOOP) {
			debug("%s: tsec: tx buffers full\n", dev->name);
			return result;
		}
	}

	priv->txbd[priv->tx_idx].bufPtr = (uint) packet;
	priv->txbd[priv->tx_idx].length = length;
	priv->txbd[priv->tx_idx].status |=
	    (TXBD_READY | TXBD_LAST | TXBD_CRC | TXBD_INTERRUPT);

	/* Tell the DMA to go */
	regs->tstat = TSTAT_CLEAR_THALT;

	/* Wait for buffer to be transmitted */
	for (i = 0; priv->txbd[priv->tx_idx].status & TXBD_READY; i++) {
		if (i >= TOUT_LOOP) {
			debug("%s: tsec: tx error\n", dev->name);
			return result;
		}
	}

	priv->tx_idx = (priv->tx_idx + 1) % TX_BUF_CNT;
	result = priv->txbd[priv->tx_idx].status & TXBD_STATS;

	return result;
}

static int tsec_recv(struct eth_device *dev)
{
	int length;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	while (!(priv->rxbd[priv->rx_idx].status & RXBD_EMPTY)) {

		length = priv->rxbd[priv->rx_idx].length;

		/* Send the packet up if there were no errors */
		if (!(priv->rxbd[priv->rx_idx].status & RXBD_STATS)) {
			NetReceive(NetRxPackets[priv->rx_idx], length - 4);
		} else {
			printf("Got error %x\n",
			       (priv->rxbd[priv->rx_idx].status & RXBD_STATS));
		}

		priv->rxbd[priv->rx_idx].length = 0;

		/* Set the wrap bit if this is the last element in the list */
		priv->rxbd[priv->rx_idx].status =
		    RXBD_EMPTY | (((priv->rx_idx + 1) == PKTBUFSRX) ? RXBD_WRAP : 0);

		priv->rx_idx = (priv->rx_idx + 1) % PKTBUFSRX;
	}

	if (regs->ievent & IEVENT_BSY) {
		regs->ievent = IEVENT_BSY;
		regs->rstat = RSTAT_CLEAR_RHALT;
	}

	return -1;

}

/* XXX fix me */
int tsec_recv_test(struct eth_device *dev, uchar *pkt)
{
	int length;
	int ret = 0;
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	while (!(priv->rxbd[priv->rx_idx].status & RXBD_EMPTY)) {

		length = priv->rxbd[priv->rx_idx].length;

		/* Send the packet up if there were no errors */
		if (!(priv->rxbd[priv->rx_idx].status & RXBD_STATS)) {
//			NetReceive(NetRxPackets[priv->rx_idx], length - 4);
			memcpy(pkt, (void *)NetRxPackets[priv->rx_idx], length - 4);
			ret = length - 4;
		} else {
			printf("Got error %x\n",
			       (priv->rxbd[priv->rx_idx].status & RXBD_STATS));
			ret = 0;
		}

		priv->rxbd[priv->rx_idx].length = 0;

		/* Set the wrap bit if this is the last element in the list */
		priv->rxbd[priv->rx_idx].status =
		    RXBD_EMPTY | (((priv->rx_idx + 1) == PKTBUFSRX) ? RXBD_WRAP : 0);

		priv->rx_idx = (priv->rx_idx + 1) % PKTBUFSRX;
	}

	if (regs->ievent & IEVENT_BSY) {
		regs->ievent = IEVENT_BSY;
		regs->rstat = RSTAT_CLEAR_RHALT;
	}

	return ret;

}

/* Stop the interface */
static void tsec_halt(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	volatile tsec_t *regs = priv->regs;

	regs->dmactrl &= ~(DMACTRL_GRS | DMACTRL_GTS);
	regs->dmactrl |= (DMACTRL_GRS | DMACTRL_GTS);

	while ((regs->ievent & (IEVENT_GRSC | IEVENT_GTSC))
		!= (IEVENT_GRSC | IEVENT_GTSC)) ;

	regs->maccfg1 &= ~(MACCFG1_TX_EN | MACCFG1_RX_EN);

	/* Shut down the PHY, as needed */
	if(priv->phyinfo)
		phy_run_commands(priv, priv->phyinfo->shutdown);
}

static struct phy_info phy_info_M88E1149S = {
	0x1410ca,
	"Marvell 88E1149S",
	4,
	(struct phy_cmd[]) {     /* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{0x1d, 0x1f, NULL},
		{0x1e, 0x200c, NULL},
		{0x1d, 0x5, NULL},
		{0x1e, 0x0, NULL},
		{0x1e, 0x100, NULL},
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {     /* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_88E1011_PHY_STATUS, miim_read, &mii_parse_88E1011_psr},
		{miim_end,}
	},
	(struct phy_cmd[]) {     /* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_M88E154x_A0 = {
	0x1410eb0,
	"Marvell 88E154x A0",
	0,
	(struct phy_cmd[]) {     /* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
#if 0
		{0x1d, 0x1f, NULL},
		{0x1e, 0x200c, NULL},
		{0x1d, 0x5, NULL},
		{0x1e, 0x0, NULL},
		{0x1e, 0x100, NULL},
#endif
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {     /* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_88E1011_PHY_STATUS, miim_read, &mii_parse_88E1011_psr},
		{miim_end,}
	},
	(struct phy_cmd[]) {     /* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_M88E154x_A1 = {
	0x1410eb1,
	"Marvell 88E154x A1",
	0,
	(struct phy_cmd[]) {     /* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
#if 0
		{0x1d, 0x1f, NULL},
		{0x1e, 0x200c, NULL},
		{0x1d, 0x5, NULL},
		{0x1e, 0x0, NULL},
		{0x1e, 0x100, NULL},
#endif
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {     /* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_88E1011_PHY_STATUS, miim_read, &mii_parse_88E1011_psr},
		{miim_end,}
	},
	(struct phy_cmd[]) {     /* shutdown */
		{miim_end,}
	},
};

/* The 5411 id is 0x206070, the 5421 is 0x2060e0 */
static struct phy_info phy_info_BCM5461S = {
	0x02060c1,	/* 5461 ID */
	"Broadcom BCM5461S",
	0, /* not clear to me what minor revisions we can shift away */
	(struct phy_cmd[]) { /* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_BCM54xx_AUXSTATUS, miim_read, &mii_parse_BCM54xx_sr},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_BCM5464S = {
	0x02060b1,	/* 5464 ID */
	"Broadcom BCM5464S",
	0, /* not clear to me what minor revisions we can shift away */
	(struct phy_cmd[]) { /* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_BCM54xx_AUXSTATUS, miim_read, &mii_parse_BCM54xx_sr},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_BCM5482S =  {
	0x0143bcb,
	"Broadcom BCM5482S",
	4,
	(struct phy_cmd[]) { /* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		/* Setup read from auxilary control shadow register 7 */
		{MIIM_BCM54xx_AUXCNTL, MIIM_BCM54xx_AUXCNTL_ENCODE(7), NULL},
		/* Read Misc Control register and or in Ethernet@Wirespeed */
		{MIIM_BCM54xx_AUXCNTL, 0, &mii_BCM54xx_wirespeed},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		/* Initial config/enable of secondary SerDes interface */
		{MIIM_BCM54XX_SHD, MIIM_BCM54XX_SHD_WR_ENCODE(0x14, 0xf), NULL},
		/* Write intial value to secondary SerDes Contol */
		{MIIM_BCM54XX_EXP_SEL, MIIM_BCM54XX_EXP_SEL_SSD | 0, NULL},
		{MIIM_BCM54XX_EXP_DATA, MIIM_CONTROL_RESTART, NULL},
		/* Enable copper/fiber auto-detect */
		{MIIM_BCM54XX_SHD, MIIM_BCM54XX_SHD_WR_ENCODE(0x1e, 0x201)},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Determine copper/fiber, auto-negotiate, and read the result */
		{MIIM_STATUS, miim_read, &mii_parse_BCM5482_sr},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_M88E1011S = {
	0x01410c6,
	"Marvell 88E1011S",
	4,
	(struct phy_cmd[]) {	/* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{0x1d, 0x1f, NULL},
		{0x1e, 0x200c, NULL},
		{0x1d, 0x5, NULL},
		{0x1e, 0x0, NULL},
		{0x1e, 0x100, NULL},
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_88E1011_PHY_STATUS, miim_read, &mii_parse_88E1011_psr},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_M88E1111S = {
	0x01410cc,
	"Marvell 88E1111S",
	4,
	(struct phy_cmd[]) {	/* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{0x1b, 0x848f, &mii_m88e1111s_setmode},
		{0x14, 0x0cd2, NULL}, /* Delay RGMII TX and RX */
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_88E1011_PHY_STATUS, miim_read, &mii_parse_88E1011_psr},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_M88E1118 = {
	0x01410e1,
	"Marvell 88E1118",
	4,
	(struct phy_cmd[]) {	/* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{0x16, 0x0002, NULL}, /* Change Page Number */
		{0x15, 0x1070, NULL}, /* Delay RGMII TX and RX */
		{0x16, 0x0003, NULL}, /* Change Page Number */
		{0x10, 0x021e, NULL}, /* Adjust LED control */
		{0x16, 0x0000, NULL}, /* Change Page Number */
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		{0x16, 0x0000, NULL}, /* Change Page Number */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_88E1011_PHY_STATUS, miim_read,
		 &mii_parse_88E1011_psr},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

/*
 *  Since to access LED register we need do switch the page, we
 * do LED configuring in the miim_read-like function as follows
 */
static uint mii_88E1121_set_led (uint mii_reg, struct tsec_private *priv)
{
	uint pg;

	/* Switch the page to access the led register */
	pg = read_phy_reg(priv, MIIM_88E1121_PHY_PAGE);
	write_phy_reg(priv, MIIM_88E1121_PHY_PAGE, MIIM_88E1121_PHY_LED_PAGE);

	/* Configure leds */
	write_phy_reg(priv, MIIM_88E1121_PHY_LED_CTRL,
		      MIIM_88E1121_PHY_LED_DEF);

	/* Restore the page pointer */
	write_phy_reg(priv, MIIM_88E1121_PHY_PAGE, pg);
	return 0;
}

static struct phy_info phy_info_M88E1121R = {
	0x01410cb,
	"Marvell 88E1121R",
	4,
	(struct phy_cmd[]) {	/* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		/* Configure leds */
		{MIIM_88E1121_PHY_LED_CTRL, miim_read, &mii_88E1121_set_led},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		/* Disable IRQs and de-assert interrupt */
		{MIIM_88E1121_PHY_IRQ_EN, 0, NULL},
		{MIIM_88E1121_PHY_IRQ_STATUS, miim_read, NULL},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		{MIIM_STATUS, miim_read, &mii_parse_link},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

static unsigned int m88e1145_setmode(uint mii_reg, struct tsec_private *priv)
{
	uint mii_data = read_phy_reg(priv, mii_reg);

	/* Setting MIIM_88E1145_PHY_EXT_CR */
	if (priv->flags & TSEC_REDUCED)
		return mii_data |
		    MIIM_M88E1145_RGMII_RX_DELAY | MIIM_M88E1145_RGMII_TX_DELAY;
	else
		return mii_data;
}

static struct phy_info phy_info_M88E1145 = {
	0x01410cd,
	"Marvell 88E1145",
	4,
	(struct phy_cmd[]) {	/* config */
		/* Reset the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},

		/* Errata E0, E1 */
		{29, 0x001b, NULL},
		{30, 0x418f, NULL},
		{29, 0x0016, NULL},
		{30, 0xa2da, NULL},

		/* Configure the PHY */
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_88E1011_PHY_SCR, MIIM_88E1011_PHY_MDI_X_AUTO, NULL},
		{MIIM_88E1145_PHY_EXT_CR, 0, &m88e1145_setmode},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, NULL},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		{MIIM_88E1111_PHY_LED_CONTROL, MIIM_88E1111_PHY_LED_DIRECT, NULL},
		/* Read the Status */
		{MIIM_88E1011_PHY_STATUS, miim_read, &mii_parse_88E1011_psr},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_M88E1512 = {
	0x1410dd,
	"Marvell 88E1512",
	4,
	(struct phy_cmd[]) {     /* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
#ifndef CONFIG_TOMATIN
                /* config SGMII mode */
		{0x16, 0x12, NULL},
		{0x14, 0x8001, NULL},
		{0x16, 0x0, NULL},
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
#endif
		{miim_end,}
	},
	(struct phy_cmd[]) {     /* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_88E1512_PHY_STATUS, miim_read, &mii_parse_88E1512_psr},
		{miim_end,}
	},
	(struct phy_cmd[]) {     /* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_cis8204 = {
	0x3f11,
	"Cicada Cis8204",
	6,
	(struct phy_cmd[]) {	/* config */
		/* Override PHY config settings */
		{MIIM_CIS8201_AUX_CONSTAT, MIIM_CIS8201_AUXCONSTAT_INIT, NULL},
		/* Configure some basic stuff */
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{MIIM_CIS8204_SLED_CON, MIIM_CIS8204_SLEDCON_INIT,
		 &mii_cis8204_fixled},
		{MIIM_CIS8204_EPHY_CON, MIIM_CIS8204_EPHYCON_INIT,
		 &mii_cis8204_setmode},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Read the Status (2x to make sure link is right) */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_CIS8201_AUX_CONSTAT, miim_read, &mii_parse_cis8201},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

/* Cicada 8201 */
static struct phy_info phy_info_cis8201 = {
	0xfc41,
	"CIS8201",
	4,
	(struct phy_cmd[]) {	/* config */
		/* Override PHY config settings */
		{MIIM_CIS8201_AUX_CONSTAT, MIIM_CIS8201_AUXCONSTAT_INIT, NULL},
		/* Set up the interface mode */
		{MIIM_CIS8201_EXT_CON1, MIIM_CIS8201_EXTCON1_INIT, NULL},
		/* Configure some basic stuff */
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Read the Status (2x to make sure link is right) */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_CIS8201_AUX_CONSTAT, miim_read, &mii_parse_cis8201},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_VSC8211 = {
	0xfc4b,
	"Vitesse VSC8211",
	4,
	(struct phy_cmd[]) { /* config */
		/* Override PHY config settings */
		{MIIM_CIS8201_AUX_CONSTAT, MIIM_CIS8201_AUXCONSTAT_INIT, NULL},
		/* Set up the interface mode */
		{MIIM_CIS8201_EXT_CON1, MIIM_CIS8201_EXTCON1_INIT, NULL},
		/* Configure some basic stuff */
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* startup */
		/* Read the Status (2x to make sure link is right) */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_CIS8201_AUX_CONSTAT, miim_read, &mii_parse_cis8201},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_VSC8244 = {
	0x3f1b,
	"Vitesse VSC8244",
	6,
	(struct phy_cmd[]) {	/* config */
		/* Override PHY config settings */
		/* Configure some basic stuff */
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Read the Status (2x to make sure link is right) */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_VSC8244_AUX_CONSTAT, miim_read, &mii_parse_vsc8244},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_VSC8641 = {
	0x7043,
	"Vitesse VSC8641",
	4,
	(struct phy_cmd[]) {	/* config */
		/* Configure some basic stuff */
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Read the Status (2x to make sure link is right) */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_VSC8244_AUX_CONSTAT, miim_read, &mii_parse_vsc8244},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_VSC8221 = {
	0xfc55,
	"Vitesse VSC8221",
	4,
	(struct phy_cmd[]) {	/* config */
		/* Configure some basic stuff */
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Read the Status (2x to make sure link is right) */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_VSC8244_AUX_CONSTAT, miim_read, &mii_parse_vsc8244},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_VSC8601 = {
	0x00007042,
	"Vitesse VSC8601",
	4,
	(struct phy_cmd[]) {     /* config */
		/* Override PHY config settings */
		/* Configure some basic stuff */
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
#ifdef CONFIG_SYS_VSC8601_SKEWFIX
		{MIIM_VSC8601_EPHY_CON,MIIM_VSC8601_EPHY_CON_INIT_SKEW,NULL},
#if defined(CONFIG_SYS_VSC8601_SKEW_TX) && defined(CONFIG_SYS_VSC8601_SKEW_RX)
		{MIIM_EXT_PAGE_ACCESS,1,NULL},
#define VSC8101_SKEW \
	(CONFIG_SYS_VSC8601_SKEW_TX << 14) | (CONFIG_SYS_VSC8601_SKEW_RX << 12)
		{MIIM_VSC8601_SKEW_CTRL,VSC8101_SKEW,NULL},
		{MIIM_EXT_PAGE_ACCESS,0,NULL},
#endif
#endif
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESTART, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {     /* startup */
		/* Read the Status (2x to make sure link is right) */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_VSC8244_AUX_CONSTAT, miim_read, &mii_parse_vsc8244},
		{miim_end,}
	},
	(struct phy_cmd[]) {     /* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_dm9161 = {
	0x0181b88,
	"Davicom DM9161E",
	4,
	(struct phy_cmd[]) {	/* config */
		{MIIM_CONTROL, MIIM_DM9161_CR_STOP, NULL},
		/* Do not bypass the scrambler/descrambler */
		{MIIM_DM9161_SCR, MIIM_DM9161_SCR_INIT, NULL},
		/* Clear 10BTCSR to default */
		{MIIM_DM9161_10BTCSR, MIIM_DM9161_10BTCSR_INIT, NULL},
		/* Configure some basic stuff */
		{MIIM_CONTROL, MIIM_CR_INIT, NULL},
		/* Restart Auto Negotiation */
		{MIIM_CONTROL, MIIM_DM9161_CR_RSTAN, NULL},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_DM9161_SCSR, miim_read, &mii_parse_dm9161_scsr},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

/* micrel KSZ804  */
static struct phy_info phy_info_ksz804 =  {
	0x0022151,
	"Micrel KSZ804 PHY",
	4,
	(struct phy_cmd[]) { /* config */
		{PHY_BMCR, PHY_BMCR_RESET, NULL},
		{PHY_BMCR, PHY_BMCR_AUTON|PHY_BMCR_RST_NEG, NULL},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* startup */
		{PHY_BMSR, miim_read, NULL},
		{PHY_BMSR, miim_read, &mii_parse_sr},
		{PHY_BMSR, miim_read, &mii_parse_link},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* shutdown */
		{miim_end,}
	}
};

/* a generic flavor.  */
static struct phy_info phy_info_generic =  {
	0,
	"Unknown/Generic PHY",
	32,
	(struct phy_cmd[]) { /* config */
		{PHY_BMCR, PHY_BMCR_RESET, NULL},
		{PHY_BMCR, PHY_BMCR_AUTON|PHY_BMCR_RST_NEG, NULL},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* startup */
		{PHY_BMSR, miim_read, NULL},
		{PHY_BMSR, miim_read, &mii_parse_sr},
		{PHY_BMSR, miim_read, &mii_parse_link},
		{miim_end,}
	},
	(struct phy_cmd[]) { /* shutdown */
		{miim_end,}
	}
};

static uint mii_parse_lxt971_sr2(uint mii_reg, struct tsec_private *priv)
{
	unsigned int speed;
	if (priv->link) {
		speed = mii_reg & MIIM_LXT971_SR2_SPEED_MASK;

		switch (speed) {
		case MIIM_LXT971_SR2_10HDX:
			priv->speed = 10;
			priv->duplexity = 0;
			break;
		case MIIM_LXT971_SR2_10FDX:
			priv->speed = 10;
			priv->duplexity = 1;
			break;
		case MIIM_LXT971_SR2_100HDX:
			priv->speed = 100;
			priv->duplexity = 0;
			break;
		default:
			priv->speed = 100;
			priv->duplexity = 1;
		}
	} else {
		priv->speed = 0;
		priv->duplexity = 0;
	}

	return 0;
}

static struct phy_info phy_info_lxt971 = {
	0x0001378e,
	"LXT971",
	4,
	(struct phy_cmd[]) {	/* config */
		{MIIM_CR, MIIM_CR_INIT, mii_cr_init},	/* autonegotiate */
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup - enable interrupts */
		/* { 0x12, 0x00f2, NULL }, */
		{MIIM_STATUS, miim_read, NULL},
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		{MIIM_LXT971_SR2, miim_read, &mii_parse_lxt971_sr2},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown - disable interrupts */
		{miim_end,}
	},
};

/* Parse the DP83865's link and auto-neg status register for speed and duplex
 * information
 */
static uint mii_parse_dp83865_lanr(uint mii_reg, struct tsec_private *priv)
{
	switch (mii_reg & MIIM_DP83865_SPD_MASK) {

	case MIIM_DP83865_SPD_1000:
		priv->speed = 1000;
		break;

	case MIIM_DP83865_SPD_100:
		priv->speed = 100;
		break;

	default:
		priv->speed = 10;
		break;

	}

	if (mii_reg & MIIM_DP83865_DPX_FULL)
		priv->duplexity = 1;
	else
		priv->duplexity = 0;

	return 0;
}

static struct phy_info phy_info_dp83865 = {
	0x20005c7,
	"NatSemi DP83865",
	4,
	(struct phy_cmd[]) {	/* config */
		{MIIM_CONTROL, MIIM_DP83865_CR_INIT, NULL},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the link and auto-neg status */
		{MIIM_DP83865_LANR, miim_read, &mii_parse_dp83865_lanr},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

static struct phy_info phy_info_rtl8211b = {
	0x001cc91,
	"RealTek RTL8211B",
	4,
	(struct phy_cmd[]) {	/* config */
		/* Reset and configure the PHY */
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_GBIT_CONTROL, MIIM_GBIT_CONTROL_INIT, NULL},
		{MIIM_ANAR, MIIM_ANAR_INIT, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_RESET, NULL},
		{MIIM_CONTROL, MIIM_CONTROL_INIT, &mii_cr_init},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* startup */
		/* Status is read once to clear old link state */
		{MIIM_STATUS, miim_read, NULL},
		/* Auto-negotiate */
		{MIIM_STATUS, miim_read, &mii_parse_sr},
		/* Read the status */
		{MIIM_RTL8211B_PHY_STATUS, miim_read, &mii_parse_RTL8211B_sr},
		{miim_end,}
	},
	(struct phy_cmd[]) {	/* shutdown */
		{miim_end,}
	},
};

struct phy_info phy_info_AR8021 =  {
        0x4dd04,
        "AR8021",
        4,
        (struct phy_cmd[]) { /* config */
                {PHY_BMCR, PHY_BMCR_RESET, NULL},
                {PHY_BMCR, PHY_BMCR_AUTON|PHY_BMCR_RST_NEG, NULL},
                {0x1d, 0x05, NULL},
                {0x1e, 0x3D47, NULL},
                {miim_end,}
        },
        (struct phy_cmd[]) { /* startup */
                {PHY_BMSR, miim_read, NULL},
                {PHY_BMSR, miim_read, &mii_parse_sr},
                {PHY_BMSR, miim_read, &mii_parse_link},
                {miim_end,}
        },
        (struct phy_cmd[]) { /* shutdown */
                {miim_end,}
        }
};

static struct phy_info *phy_info[] = {
	&phy_info_cis8204,
	&phy_info_cis8201,
	&phy_info_BCM5461S,
	&phy_info_BCM5464S,
	&phy_info_BCM5482S,
	&phy_info_M88E1011S,
	&phy_info_M88E1111S,
	&phy_info_M88E1118,
	&phy_info_M88E1121R,
	&phy_info_M88E1145,
	&phy_info_M88E1149S,
	&phy_info_M88E154x_A0,
	&phy_info_M88E154x_A1,
        &phy_info_M88E1512,
	&phy_info_dm9161,
	&phy_info_ksz804,
	&phy_info_lxt971,
	&phy_info_VSC8211,
	&phy_info_VSC8244,
	&phy_info_VSC8601,
	&phy_info_VSC8641,
	&phy_info_VSC8221,
	&phy_info_dp83865,
	&phy_info_rtl8211b,
	&phy_info_AR8021,
	&phy_info_generic,	/* must be last; has ID 0 and 32 bit mask */
	NULL
};

/* Grab the identifier of the device's PHY, and search through
 * all of the known PHYs to see if one matches.	 If so, return
 * it, if not, return NULL
 */
static struct phy_info *get_phy_info(struct eth_device *dev)
{
	struct tsec_private *priv = (struct tsec_private *)dev->priv;
	uint phy_reg, phy_ID;
	int i;
	struct phy_info *theInfo = NULL;

	/* Grab the bits from PHYIR1, and put them in the upper half */
	phy_reg = read_phy_reg(priv, MIIM_PHYIR1);
	phy_ID = (phy_reg & 0xffff) << 16;

	/* Grab the bits from PHYIR2, and put them in the lower half */
	phy_reg = read_phy_reg(priv, MIIM_PHYIR2);
	phy_ID |= (phy_reg & 0xffff);

	/* loop through all the known PHY types, and find one that */
	/* matches the ID we read from the PHY. */
	for (i = 0; phy_info[i]; i++) {
		if (phy_info[i]->id == (phy_ID >> phy_info[i]->shift)) {
			theInfo = phy_info[i];
			break;
		}
	}

	if (theInfo == &phy_info_generic) {
		debug("%s: No support for PHY id %x; assuming generic\n",
			dev->name, phy_ID);
	} else {
		debug("%s: PHY is %s (%x)\n", dev->name, theInfo->name, phy_ID);
	}

	return theInfo;
}

/* Execute the given series of commands on the given device's
 * PHY, running functions as necessary
 */
static void phy_run_commands(struct tsec_private *priv, struct phy_cmd *cmd)
{
	int i;
	uint result;
	volatile tsec_mdio_t *phyregs = priv->phyregs;

	phyregs->miimcfg = MIIMCFG_RESET;

	phyregs->miimcfg = MIIMCFG_INIT_VALUE;

	while (phyregs->miimind & MIIMIND_BUSY) ;

	for (i = 0; cmd->mii_reg != miim_end; i++) {
		if (cmd->mii_data == miim_read) {
			result = read_phy_reg(priv, cmd->mii_reg);

			if (cmd->funct != NULL)
				(*(cmd->funct)) (result, priv);

		} else {
			if (cmd->funct != NULL)
				result = (*(cmd->funct)) (cmd->mii_reg, priv);
			else
				result = cmd->mii_data;

			write_phy_reg(priv, cmd->mii_reg, result);

		}
		cmd++;
	}
}

/* Relocate the function pointers in the phy cmd lists */
static void relocate_cmds(void)
{
	struct phy_cmd **cmdlistptr;
	struct phy_cmd *cmd;
	int i, j, k;

	for (i = 0; phy_info[i]; i++) {
		/* First thing's first: relocate the pointers to the
		 * PHY command structures (the structs were done) */
		phy_info[i] = (struct phy_info *)((uint) phy_info[i]
						  + gd->reloc_off);
		phy_info[i]->name += gd->reloc_off;
		phy_info[i]->config =
		    (struct phy_cmd *)((uint) phy_info[i]->config
				       + gd->reloc_off);
		phy_info[i]->startup =
		    (struct phy_cmd *)((uint) phy_info[i]->startup
				       + gd->reloc_off);
		phy_info[i]->shutdown =
		    (struct phy_cmd *)((uint) phy_info[i]->shutdown
				       + gd->reloc_off);

		cmdlistptr = &phy_info[i]->config;
		j = 0;
		for (; cmdlistptr <= &phy_info[i]->shutdown; cmdlistptr++) {
			k = 0;
			for (cmd = *cmdlistptr;
			     cmd->mii_reg != miim_end;
			     cmd++) {
				/* Only relocate non-NULL pointers */
				if (cmd->funct)
					cmd->funct += gd->reloc_off;

				k++;
			}
			j++;
		}
	}

	relocated = 1;
}

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII) \
	&& !defined(BITBANGMII)

/*
 * Read a MII PHY register.
 *
 * Returns:
 *  0 on success
 */
static int tsec_miiphy_read(char *devname, unsigned char addr,
			    unsigned char reg, unsigned short *value)
{
	unsigned short ret;
	struct tsec_private *priv = privlist[0];

	if (NULL == priv) {
		printf("Can't read PHY at address %d\n", addr);
		return -1;
	}

	ret = (unsigned short)tsec_local_mdio_read(priv->phyregs, addr, reg);
	*value = ret;

	return 0;
}

/*
 * Write a MII PHY register.
 *
 * Returns:
 *  0 on success
 */
static int tsec_miiphy_write(char *devname, unsigned char addr,
			     unsigned char reg, unsigned short value)
{
	struct tsec_private *priv = privlist[0];

	if (NULL == priv) {
		printf("Can't write PHY at address %d\n", addr);
		return -1;
	}

	tsec_local_mdio_write(priv->phyregs, addr, reg, value);

	return 0;
}

#endif

#ifdef CONFIG_MCAST_TFTP

/* CREDITS: linux gianfar driver, slightly adjusted... thanx. */

/* Set the appropriate hash bit for the given addr */

/* The algorithm works like so:
 * 1) Take the Destination Address (ie the multicast address), and
 * do a CRC on it (little endian), and reverse the bits of the
 * result.
 * 2) Use the 8 most significant bits as a hash into a 256-entry
 * table.  The table is controlled through 8 32-bit registers:
 * gaddr0-7.  gaddr0's MSB is entry 0, and gaddr7's LSB is
 * gaddr7.  This means that the 3 most significant bits in the
 * hash index which gaddr register to use, and the 5 other bits
 * indicate which bit (assuming an IBM numbering scheme, which
 * for PowerPC (tm) is usually the case) in the tregister holds
 * the entry. */
static int
tsec_mcast_addr (struct eth_device *dev, u8 mcast_mac, u8 set)
{
	struct tsec_private *priv = privlist[1];
	volatile tsec_t *regs = priv->regs;
	volatile u32  *reg_array, value;
	u8 result, whichbit, whichreg;

	result = (u8)((ether_crc(MAC_ADDR_LEN,mcast_mac) >> 24) & 0xff);
	whichbit = result & 0x1f;	/* the 5 LSB = which bit to set */
	whichreg = result >> 5;		/* the 3 MSB = which reg to set it in */
	value = (1 << (31-whichbit));

	reg_array = &(regs->hash.gaddr0);

	if (set) {
		reg_array[whichreg] |= value;
	} else {
		reg_array[whichreg] &= ~value;
	}
	return 0;
}
#endif /* Multicast TFTP ? */

int do_tsecdiag(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	unsigned short val;

	printf("TSEC2\n");
	for (i = 0; i < 0x12; i++) {
		val = tsec_local_mdio_read((volatile tsec_mdio_t *)0xffe25000, 0x1f, i);
		val = tsec_local_mdio_read((volatile tsec_mdio_t *)0xffe25000, 0x1f, i);
		printf("reg[0x%x] = 0x%x\n", i, val);
	}

	printf("PHY 0\n");
	miiphy_write("eth0", 0, 0x16, 0x1);
	for (i = 0; i < 0x1f; i++) {
		miiphy_read("eth0", 0, i, &val);
		miiphy_read("eth0", 0, i, &val);
		printf("reg[0x%x] = 0x%x\n", i, val);
	}
	miiphy_write("eth0", 0, 0x16, 0x0);

	printf("TSEC3\n");
	for (i = 0; i < 0x12; i++) {
		val = tsec_local_mdio_read((volatile tsec_mdio_t *)0xffe26000, 0x1f, i);
		val = tsec_local_mdio_read((volatile tsec_mdio_t *)0xffe26000, 0x1f, i);
		printf("reg[0x%x] = 0x%x\n", i, val);
	}

	printf("PHY 1\n");
	miiphy_write("eth0", 1, 0x16, 0x1);
	for (i = 0; i < 0x1f; i++) {
		miiphy_read("eth0", 1, i, &val);
		miiphy_read("eth0", 1, i, &val);
		printf("reg[0x%x] = 0x%x\n", i, val);
	}
	miiphy_write("eth0", 1, 0x16, 0x0);

	return 0;
}

U_BOOT_CMD(
	tsecdiag,	1,	1,	do_tsecdiag,
	"tsecdiag",
	"tsecdiag"
);
#endif /* powerpc */
