/*
 * Broadcom 53xx RoboSwitch device driver.
 *
 * $Copyright Open Broadcom Corporation$
 *
 * $Id: bcmrobo.c 312061 2012-02-01 06:59:04Z kenlo $
 */


#include <config.h>
#ifdef CONFIG_IPROC
#include <common.h>
#include <asm/arch/iproc_regs.h>
#include <asm/arch/bcmrobo.h>
#include <asm/iproc/reg_utils.h>
#include <asm/arch/bcmutils.h>
#include "bcmiproc_robo_serdes.h"

/* debug/trace */
//#define BCMDBG
#define BCMDBG_ERR
#ifdef	BCMDBG
#define	ET_ERROR(args) printf args
#define	ET_TRACE(args) printf args
#elif defined(BCMDBG_ERR)
#define	ET_ERROR(args) printf args
#define ET_TRACE(args)
#else
#define	ET_ERROR(args)
#define	ET_TRACE(args)
#endif	/* BCMDBG */

#define VARG(var, len) (((len) == 1) ? *((uint8 *)(var)) : \
		        ((len) == 2) ? *((uint16 *)(var)) : \
		        *((uint32 *)(var)))

#define ETH_ADDR_LEN	6

//#define SERDES_LOOPBACK 1

/*
 * Switch can be programmed through SPI interface, which
 * has a rreg and a wreg functions to read from and write to
 * registers.
 */

/* MII access registers */
#define PSEUDO_PHYAD	0x1E	/* MII Pseudo PHY address */
#define REG_MII_CTRL    0x00    /* 53115 MII control register */
#define REG_MII_PAGE	0x10	/* MII Page register */
#define REG_MII_ADDR	0x11	/* MII Address register */
#define REG_MII_DATA0	0x18	/* MII Data register 0 */
#define REG_MII_DATA1	0x19	/* MII Data register 1 */
#define REG_MII_DATA2	0x1a	/* MII Data register 2 */
#define REG_MII_DATA3	0x1b	/* MII Data register 3 */
#define REG_MII_AUX_STATUS2	0x1b	/* Auxiliary status 2 register */
#define REG_MII_AUTO_PWRDOWN	0x1c	/* 53115 Auto power down register */
#define REG_MII_BRCM_TEST	0x1f	/* Broadcom test register */

/* Page numbers */
#define PAGE_CTRL	0x00	/* Control page */
#define PAGE_STATUS	0x01	/* Status page */
#define PAGE_MMR	0x02	/* 5397 Management/Mirroring page */
#define PAGE_VTBL	0x05	/* ARL/VLAN Table access page */
#define PAGE_PHY	0x10	/* GPHY register page */
#define PAGE_VLAN	0x34	/* VLAN page */

/* Control page registers */
#define REG_CTRL_PORT0	0x00	/* Port 0 traffic control register */
#define REG_CTRL_PORT1	0x01	/* Port 1 traffic control register */
#define REG_CTRL_PORT2	0x02	/* Port 2 traffic control register */
#define REG_CTRL_PORT3	0x03	/* Port 3 traffic control register */
#define REG_CTRL_PORT4	0x04	/* Port 4 traffic control register */
#define REG_CTRL_PORT5	0x05	/* Port 5 traffic control register */
#define REG_CTRL_PORT6	0x06	/* Port 6 traffic control register */
#define REG_CTRL_PORT7	0x07	/* Port 7 traffic control register */
#define REG_CTRL_IMP	0x08	/* IMP port traffic control register */
#define REG_CTRL_MODE	0x0B	/* Switch Mode register */
#define REG_CTRL_MIIPO	0x0E	/* 5325: MII Port Override register */
#define REG_CTRL_PWRDOWN 0x0F   /* 5325: Power Down Mode register */
#define REG_CTRL_PPORT	0x24	/* Protected port register */
#define REG_CTRL_SRST	0x79	/* Software reset control register */

/* Status Page Registers */
#define REG_STATUS_LINK	0x00	/* Link Status Summary */
#define REG_STATUS_REV	0x50	/* Revision Register */

#define REG_DEVICE_ID	0x30	/* 539x Device id: */

/* VLAN page registers */
#define REG_VLAN_CTRL0	0x00	/* VLAN Control 0 register */
#define REG_VLAN_CTRL1	0x01	/* VLAN Control 1 register */
#define REG_VLAN_CTRL4	0x04	/* VLAN Control 4 register */
#define REG_VLAN_CTRL5	0x05	/* VLAN Control 5 register */
#define REG_VLAN_ACCESS	0x06	/* VLAN Table Access register */
#define REG_VLAN_WRITE	0x08	/* VLAN Write register */
#define REG_VLAN_READ	0x0C	/* VLAN Read register */
#define REG_VLAN_PTAG0	0x10	/* VLAN Default Port Tag register - port 0 */
#define REG_VLAN_PTAG1	0x12	/* VLAN Default Port Tag register - port 1 */
#define REG_VLAN_PTAG2	0x14	/* VLAN Default Port Tag register - port 2 */
#define REG_VLAN_PTAG3	0x16	/* VLAN Default Port Tag register - port 3 */
#define REG_VLAN_PTAG4	0x18	/* VLAN Default Port Tag register - port 4 */
#define REG_VLAN_PTAG5	0x1a	/* VLAN Default Port Tag register - port 5 */
#define REG_VLAN_PTAG6	0x1c	/* VLAN Default Port Tag register - port 6 */
#define REG_VLAN_PTAG7	0x1e	/* VLAN Default Port Tag register - port 7 */
#define REG_VLAN_PTAG8	0x20	/* 539x: VLAN Default Port Tag register - IMP port */
#define REG_VLAN_PMAP	0x20	/* 5325: VLAN Priority Re-map register */

#define VLAN_NUMVLANS	16	/* # of VLANs */
#define VLAN_MAXVID		15	/* Max. VLAN ID supported/allowed */


/* ARL/VLAN Table Access page registers */
#define REG_VTBL_CTRL		0x00	/* ARL Read/Write Control */
#define REG_VTBL_MINDX		0x02	/* MAC Address Index */
#define REG_VTBL_VINDX		0x08	/* VID Table Index */
#define REG_VTBL_ARL_E0		0x10	/* ARL Entry 0 */
#define REG_VTBL_ARL_E1		0x18	/* ARL Entry 1 */
#define REG_VTBL_DAT_E0		0x18	/* ARL Table Data Entry 0 */
#define REG_VTBL_SCTRL		0x20	/* ARL Search Control */
#define REG_VTBL_SADDR		0x22	/* ARL Search Address */
#define REG_VTBL_SRES		0x24	/* ARL Search Result */
#define REG_VTBL_SREXT		0x2c	/* ARL Search Result */
#define REG_VTBL_VID_E0		0x30	/* VID Entry 0 */
#define REG_VTBL_VID_E1		0x32	/* VID Entry 1 */
#define REG_VTBL_PREG		0xFF	/* Page Register */
#define REG_VTBL_ACCESS		0x60	/* VLAN table access register */
#define REG_VTBL_INDX		0x61	/* VLAN table address index register */
#define REG_VTBL_ENTRY		0x63	/* VLAN table entry register */
#define REG_VTBL_ACCESS_5395	0x80	/* VLAN table access register */
#define REG_VTBL_INDX_5395	0x81	/* VLAN table address index register */
#define REG_VTBL_ENTRY_5395	0x83	/* VLAN table entry register */

/////////////////////+SRAB+/////////////////////
/* Access switch registers through SRAB (Switch Register Access Bridge) */
#define REG_VERSION_ID		0x40
#define REG_CTRL_PORT0_GMIIPO	0x58	/* 53012: GMII Port0 Override register */
#define REG_CTRL_PORT1_GMIIPO	0x59	/* 53012: GMII Port1 Override register */
#define REG_CTRL_PORT2_GMIIPO	0x5a	/* 53012: GMII Port2 Override register */
#define REG_CTRL_PORT3_GMIIPO	0x5b	/* 53012: GMII Port3 Override register */
#define REG_CTRL_PORT4_GMIIPO	0x5c	/* 53012: GMII Port4 Override register */
#define REG_CTRL_PORT5_GMIIPO	0x5d	/* 53012: GMII Port5 Override register */
#define REG_CTRL_PORT7_GMIIPO	0x5f	/* 53012: GMII Port7 Override register */

#define CHIPCB_SRAB_BASE		IPROC_CCB_SRAU_REG_BASE /*(0x18007000)*/

/* Command and status register of the SRAB */
#define CFG_R_SRAB_CMDSTAT_MEMADDR	(CHIPCB_SRAB_BASE + 0x2c)
#define CFG_F_sra_rst_MASK		(1 << 2)
#define CFG_F_sra_write_MASK		(1 << 1)
#define CFG_F_sra_gordyn_MASK		(1 << 0)
#define CFG_F_sra_page_R		24
#define CFG_F_sra_offset_R		16

/* High order word of write data to switch register */
#define CFG_R_SRAB_WDH_MEMADDR		(CHIPCB_SRAB_BASE + 0x30)

/* Low order word of write data to switch register */
#define CFG_R_SRAB_WDL_MEMADDR		(CHIPCB_SRAB_BASE + 0x34)

/* High order word of read data from switch register */
#define CFG_R_SRAB_RDH_MEMADDR		(CHIPCB_SRAB_BASE + 0x38)

/* Low order word of read data from switch register */
#define CFG_R_SRAB_RDL_MEMADDR		(CHIPCB_SRAB_BASE + 0x3c)

/* Switch interface controls */
#define CFG_R_SW_IF_MEMADDR		(CHIPCB_SRAB_BASE + 0x40)
#define CFG_F_sw_init_done_MASK		(1 << 6)
#define CFG_F_rcareq_MASK		(1 << 3)
#define CFG_F_rcagnt_MASK		(1 << 4)
/////////////////////-SRAB-/////////////////////

/////////////////////+SRAB+/////////////////////
#define readl(a) ({ \
	uint32 *__osl_reg = (uint32 *)(a); \
	reg32_read(__osl_reg); \
})

#define writel(a, b) ({ \
	uint32 *__osl_reg = (uint32 *)(a); \
	reg32_write(__osl_reg, b); \
})

#define SRAB_MAX_RETRY		1000

#ifndef ASSERT
#define ASSERT(exp)
#endif

#if 0
static int
srab_request_grant(void)
{
	int i, ret = 0;
	uint32 val32;

	val32 = readl(CFG_R_SW_IF_MEMADDR);
	val32 |= CFG_F_rcareq_MASK;
	writel(CFG_R_SW_IF_MEMADDR, val32);

	/* Wait for command complete */
	for (i = SRAB_MAX_RETRY * 10; i > 0; i --) {
		val32 = readl(CFG_R_SW_IF_MEMADDR);
		if ((val32 & CFG_F_rcagnt_MASK))
			break;
	}

	/* timed out */
	if (!i) {
		ET_ERROR(("srab_request_grant: timeout"));
		ret = -1;
	}

	return ret;
}

static void
srab_release_grant(void)
{
	uint32 val32;

	val32 = readl(CFG_R_SW_IF_MEMADDR);
	val32 &= ~CFG_F_rcareq_MASK;
	writel(CFG_R_SW_IF_MEMADDR, val32);
}
#endif

static int
srab_interface_reset(void)
{
	int i, ret = 0;
	uint32 val32;

#if 0
	/* Wait for switch initialization complete */
	for (i = SRAB_MAX_RETRY * 10; i > 0; i --) {
		val32 = readl(CFG_R_SW_IF_MEMADDR);
		if ((val32 & CFG_F_sw_init_done_MASK))
			break;
	}

	/* timed out */
	if (!i) {
		ET_ERROR(("srab_interface_reset: timeout sw_init_done"));
		ret = -1;
	}
#endif

	/* Set the SRAU reset bit */
	writel(CFG_R_SRAB_CMDSTAT_MEMADDR, CFG_F_sra_rst_MASK);

	/* Wait for it to auto-clear */
	for (i = SRAB_MAX_RETRY * 10; i > 0; i --) {
		val32 = readl(CFG_R_SRAB_CMDSTAT_MEMADDR);
		if ((val32 & CFG_F_sra_rst_MASK) == 0)
			break;
	}

	/* timed out */
	if (!i) {
		ET_ERROR(("srab_interface_reset: timeout sra_rst"));
		ret |= -2;
	}

	return ret;
}

int
srab_wreg(robo_info_t *robo, uint8 page, uint8 reg, void *val, int len)
{
	uint16 val16;
	uint32 val32;
	uint32 val_h = 0, val_l = 0;
	//void *h = robo->h;
	int i, ret = 0;
	uint8 *ptr = (uint8 *)val;

	/* validate value length and buffer address */
	ASSERT(len == 1 || len == 6 || len == 8 ||
	       ((len == 2) && !((int)val & 1)) || ((len == 4) && !((int)val & 3)));

	ET_TRACE(("%s: [0x%x-0x%x] := 0x%x (len %d)\n", __FUNCTION__, page, reg,
	       VARG(val, len), len));

	//srab_request_grant();

	/* Load the value to write */
	switch (len) {
	case 8:
		val16 = ptr[7];
		val16 = ((val16 << 8) | ptr[6]);
		val_h = val16 << 16;
		/* FALLTHRU */

	case 6:
		val16 = ptr[5];
		val16 = ((val16 << 8) | ptr[4]);
		val_h |= val16;

		val16 = ptr[3];
		val16 = ((val16 << 8) | ptr[2]);
		val_l = val16 << 16;
		val16 = ptr[1];
		val16 = ((val16 << 8) | ptr[0]);
		val_l |= val16;
		break;

	case 4:
		val_l = *(uint32 *)val;
		break;

	case 2:
		val_l = *(uint16 *)val;
		break;

	case 1:
		val_l = *(uint8 *)val;
		break;
	}
	writel(CFG_R_SRAB_WDH_MEMADDR, val_h);
	writel(CFG_R_SRAB_WDL_MEMADDR, val_l);

#ifndef CONFIG_APBOOT
	/* We don't need this variable */
	if (robo->page != page)
		robo->page = page;
#endif

	/* Issue the write command */
	val32 = ((page << CFG_F_sra_page_R)
		| (reg << CFG_F_sra_offset_R)
		| CFG_F_sra_gordyn_MASK
		| CFG_F_sra_write_MASK);
	writel(CFG_R_SRAB_CMDSTAT_MEMADDR, val32);

	/* Wait for command complete */
	for (i = SRAB_MAX_RETRY; i > 0; i --) {
		val32 = readl(CFG_R_SRAB_CMDSTAT_MEMADDR);
		if ((val32 & CFG_F_sra_gordyn_MASK) == 0)
			break;
	}

	/* timed out */
	if (!i) {
		ET_ERROR(("srab_wreg: timeout"));
		srab_interface_reset();
		ret = -1;
	}

	//srab_release_grant();

	return ret;
}

int
srab_rreg(robo_info_t *robo, uint8 page, uint8 reg, void *val, int len)
{
	uint32 val32;
	uint32 val_h = 0, val_l = 0;
	//void *h = robo->h;
	int i, ret = 0;
	uint8 *ptr = (uint8 *)val;

	/* validate value length and buffer address */
	ASSERT(len == 1 || len == 6 || len == 8 ||
	       ((len == 2) && !((int)val & 1)) || ((len == 4) && !((int)val & 3)));

	//srab_request_grant();

#ifndef CONFIG_APBOOT
	/* We don't need this variable */
	if (robo->page != page)
		robo->page = page;
#endif

	/* Assemble read command */
	//srab_request_grant();

	val32 = ((page << CFG_F_sra_page_R)
		| (reg << CFG_F_sra_offset_R)
		| CFG_F_sra_gordyn_MASK);
	writel(CFG_R_SRAB_CMDSTAT_MEMADDR, val32);

	/* is operation finished? */
	for (i = SRAB_MAX_RETRY; i > 0; i --) {
		val32 = readl(CFG_R_SRAB_CMDSTAT_MEMADDR);
		if ((val32 & CFG_F_sra_gordyn_MASK) == 0)
			break;
	}

	/* timed out */
	if (!i) {
		ET_ERROR(("srab_read: timeout"));
		srab_interface_reset();
		ret = -1;
		goto err;
	}

	/* Didn't time out, read and return the value */
	val_h = readl(CFG_R_SRAB_RDH_MEMADDR);
	val_l = readl(CFG_R_SRAB_RDL_MEMADDR);

	switch (len) {
	case 8:
		ptr[7] = (val_h >> 24);
		ptr[6] = ((val_h >> 16) & 0xff);
		/* FALLTHRU */

	case 6:
		ptr[5] = ((val_h >> 8) & 0xff);
		ptr[4] = (val_h & 0xff);
		ptr[3] = (val_l >> 24);
		ptr[2] = ((val_l >> 16) & 0xff);
		ptr[1] = ((val_l >> 8) & 0xff);
		ptr[0] = (val_l & 0xff);
		break;

	case 4:
		*(uint32 *)val = val_l;
		break;

	case 2:
		*(uint16 *)val = (uint16)(val_l & 0xffff);
		break;

	case 1:
		*(uint8 *)val = (uint8)(val_l & 0xff);
		break;
	}

	ET_TRACE(("%s: [0x%x-0x%x] => 0x%x (len %d)\n", __FUNCTION__, page, reg,
	       VARG(val, len), len));

err:
	//srab_release_grant();

	return ret;
}

/* SRAB interface functions */
static dev_ops_t srab = {
	NULL,
	NULL,
	srab_wreg,
	srab_rreg,
	"SRAB"
};

#if (defined(CONFIG_NS_PLUS))
void
srab_sgmii_set_blk(robo_info_t *robo, uint page, uint blk)
{
	uint16 blkaddr;
	uint16 destblk = (uint16)blk;

	/* printf("%s page(0x%x) blk(0x%x)\n", __FUNCTION__, page, blk); */
	/* check if need to update blk addr */
	robo->ops->read_reg(robo, page, REG_SGMII_BLK_ADDR, &blkaddr, sizeof(blkaddr));
	if (blkaddr!=destblk) {
		/* write block address */
		robo->ops->write_reg(robo, page, REG_SGMII_BLK_ADDR, &destblk, sizeof(destblk));
	}
}

int
srab_sgmii_rreg(robo_info_t *robo, uint8 page, uint16 reg, uint16 *val)
{
	uint blk = reg&0xfff0;
	uint8 off = reg&0x000f;
	uint16 data;

	if (reg&0x8000)
		off|=0x10;

	/* spi offset is only even (multiple of 2) */
	off = off*2;

	/* check block addr */
	srab_sgmii_set_blk(robo, page, blk);

	/* read offset register */
	robo->ops->read_reg(robo, page, off, &data, sizeof(data));
	//printf("%s page(0x%x) blk(0x%x) offset(0x%x) value(0x%x)\n", __FUNCTION__, page, blk, off, data);
	*val = data;

	return 0;
}

int
srab_sgmii_wreg(robo_info_t *robo, uint8 page, uint16 reg, uint16 *val)
{
	uint blk = reg&0xfff0;
	uint8 off = reg&0x000f;
	uint16 data=*val;

	if (reg&0x8000)
		off|=0x10;

	/* spi offset is only even (multiple of 2) */
	off = off*2;

	/* check block addr */
	srab_sgmii_set_blk(robo, page, blk);

	/* write offset register */
	robo->ops->write_reg(robo, page, off, &data, sizeof(data));
	//printf("%s page(0x%x) blk(0x%x) offset(0x%x) value(0x%x)\n", __FUNCTION__, page, blk, off, data);

	return 0;
}
#endif //(defined(CONFIG_NS_PLUS))
/////////////////////-SRAB-/////////////////////

/* High level switch configuration functions. */

/* Get access to the RoboSwitch */
robo_info_t *
bcm_robo_attach(si_t *sih, void *h, char *vars, miird_f miird, miiwr_f miiwr)
{
	robo_info_t *robo;
#if 0
	uint32 idx;
#endif
	int rc;

	ET_TRACE(("%s: enter\n", __FUNCTION__));

	/* Allocate and init private state */
	if (!(robo = MALLOC(sih, sizeof(robo_info_t)))) {
		ET_ERROR(("robo_attach: out of memory, malloced %d bytes", MALLOCED(sih)));
		return NULL;
	}
	memset(robo, 0, sizeof(robo_info_t));

	robo->h = h;
	robo->sih = sih;
	robo->vars = vars;
	robo->miird = miird;
	robo->miiwr = miiwr;
	robo->page = -1;

#if 0
	/* In case we need it */
	idx = si_coreidx(sih);
#endif

	srab_interface_reset();
	srab_rreg(robo, PAGE_MMR, REG_VERSION_ID, &robo->corerev, 1);

#if 0
	si_setcoreidx(sih, idx);
#endif

	ET_TRACE(("%s: Internal robo rev %d\n", __FUNCTION__, robo->corerev));

	/* force mii to null */
	robo->miird = NULL;
	robo->miiwr = NULL;

	rc = srab_rreg(robo, PAGE_MMR, REG_DEVICE_ID, &robo->devid32, sizeof(uint32));
	ET_TRACE(("%s: devid read %ssuccesfully via srab: 0x%x\n",
			__FUNCTION__, rc ? "un" : "", robo->devid32));
	ET_TRACE(("%s: srab access to switch works\n", __FUNCTION__));
			robo->ops = &srab;
	if ((rc != 0) || (robo->devid32 == 0)) {
		ET_ERROR(("%s: error reading devid\n", __FUNCTION__));
		MFREE(si_osh(robo->sih), robo, sizeof(robo_info_t));
		return NULL;
	}

	if (!ROBO_IS_BCM5301X(robo->devid32)) {
		ET_ERROR(("%s: ERROR - Not BCM5301x switch\n", __FUNCTION__));
		MFREE(si_osh(robo->sih), robo, sizeof(robo_info_t));
		return NULL;
	}

	ET_TRACE(("%s: devid: 0x%x\n", __FUNCTION__, robo->devid32));
#ifdef BCMDBG
	printf("%s: devid: 0x%x\n", __FUNCTION__, robo->devid32);
#endif

	/* sanity check */
	ASSERT(robo->ops);
	ASSERT(robo->ops->write_reg);
	ASSERT(robo->ops->read_reg);
	ASSERT(ROBO_IS_BCM5301X(robo->devid32));

	return robo;
}

/* Release access to the RoboSwitch */
void
bcm_robo_detach(robo_info_t *robo)
{
	ET_TRACE(("%s: enter\n", __FUNCTION__));

	MFREE(robo->sih, robo, sizeof(robo_info_t));
}

/* Enable the device and set it to a known good state */
int
bcm_robo_enable_device(robo_info_t *robo)
{
	ET_TRACE(("%s: enter\n", __FUNCTION__));

	return 0;
}

/* Port flags */
#define FLAG_TAGGED	't'	/* output tagged (external ports only) */
#define FLAG_UNTAG	'u'	/* input & output untagged (CPU port only, for OS (linux, ...) */
#define FLAG_LAN	'*'	/* input & output untagged (CPU port only, for CFE */

/* port descriptor */
typedef	struct {
	uint32 untag;	/* untag enable bit (Page 0x05 Address 0x63-0x66 Bit[17:9]) */
	uint32 member;	/* vlan member bit (Page 0x05 Address 0x63-0x66 Bit[7:0]) */
	uint8 ptagr;	/* port tag register address (Page 0x34 Address 0x10-0x1F) */
	uint8 cpu;	/* is this cpu port? */
} pdesc_t;

pdesc_t pdesc97[] = {
	/* 5395/5397/5398/53115S is 0 ~ 7.  port 8 is IMP port. */
	/* port 0 */ {1 << 9, 1 << 0, REG_VLAN_PTAG0, 0},
	/* port 1 */ {1 << 10, 1 << 1, REG_VLAN_PTAG1, 0},
	/* port 2 */ {1 << 11, 1 << 2, REG_VLAN_PTAG2, 0},
	/* port 3 */ {1 << 12, 1 << 3, REG_VLAN_PTAG3, 0},
	/* port 4 */ {1 << 13, 1 << 4, REG_VLAN_PTAG4, 0},
	/* port 5 */ {1 << 14, 1 << 5, REG_VLAN_PTAG5, 0},
	/* port 6 */ {1 << 15, 1 << 6, REG_VLAN_PTAG6, 1},
	/* port 7 */ {1 << 16, 1 << 7, REG_VLAN_PTAG7, 1},
	/* mii port */ {1 << 17, 1 << 8, REG_VLAN_PTAG8, 1},
};

/* Configure the VLANs */
int
bcm_robo_config_vlan(robo_info_t *robo, uint8 *mac_addr)
{
	uint8 val8;
	uint16 val16;
	uint32 val32;
	pdesc_t *pdesc;
	int pdescsz;
	uint16 vid;
	uint8 arl_entry[8] = { 0 };
/* only enable lan ports */
#ifdef CONFIG_FOUR_PORTS
	char vlan1ports[] = "0 1 2 7*";
#else
	char vlan1ports[] = "0 1 2 3 4 5 7*";
#endif
	uint8 vtble, vtbli, vtbla;

	ET_TRACE(("%s: enter\n", __FUNCTION__));

	/* setup global vlan configuration */
	/* VLAN Control 0 Register (Page 0x34, Address 0) */
	robo->ops->read_reg(robo, PAGE_VLAN, REG_VLAN_CTRL0, &val8, sizeof(val8));
	val8 |= ((1 << 7) |		/* enable 802.1Q VLAN */
	         (3 << 5));		/* individual VLAN learning mode */
	robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_CTRL0, &val8, sizeof(val8));
	/* VLAN Control 1 Register (Page 0x34, Address 1) */
	robo->ops->read_reg(robo, PAGE_VLAN, REG_VLAN_CTRL1, &val8, sizeof(val8));
	val8 |= ((1 << 2) |		/* enable RSV multicast V Fwdmap */
		 (1 << 3));		/* enable RSV multicast V Untagmap */
	robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_CTRL1, &val8, sizeof(val8));

	arl_entry[0] = mac_addr[5];
	arl_entry[1] = mac_addr[4];
	arl_entry[2] = mac_addr[3];
	arl_entry[3] = mac_addr[2];
	arl_entry[4] = mac_addr[1];
	arl_entry[5] = mac_addr[0];

	/* Initialize the MAC Addr Index Register */
	robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_MINDX,
		                     arl_entry, ETH_ADDR_LEN);

	pdesc = pdesc97;
	pdescsz = sizeof(pdesc97) / sizeof(pdesc_t);

	//Port 5 is already configured as non-CPU port
	//in pdesc97 table above
#if 0
	if (ROBO_IS_BCM5301X(robo->devid32)) {
		/* 5301x use port 5 as mii port connect to GMAC0 */
		pdesc[5].cpu = 1;
		pdescsz -= 3;
	}
#endif

	/* setup each vlan. max. 16 vlans. */
	/* force vlan id to be equal to vlan number */
	for (vid = 0; vid < VLAN_NUMVLANS; vid ++) {
		/*
			this data taken from the 53010 config file:
			# Robo switch config (vlan1:LAN, vlan2:WAN)
			vlan1hwname=et0
			vlan1ports=0 1 2 3 5*
			vlan2hwname=et0
			vlan2ports=4 5u
		*/
		char port[] = "XXXX", *next;
		const char *ports, *cur;
		uint32 untag = 0;
		uint32 member = 0;
		int pid, len;

		/* no members if VLAN id is out of limitation */
		if (vid > VLAN_MAXVID)
			goto vlan_setup;

		/* get vlan member ports from nvram */
		/* sprintf(vlanports, "vlan%dports", vid); */
		/* char vlan1ports[] = "0 1 2 3 5*"; */
		/* char vlan2ports[] = "4 5u" */
		if (vid == 1)
			ports = vlan1ports;
		else
			ports = NULL;

		/* disable this vlan if not defined */
		if (!ports)
			goto vlan_setup;

		/*
		 * setup each port in the vlan. cpu port needs special handing
		 * (with or without output tagging) to support linux/pmon/cfe.
		 */
		for (cur = ports; cur; cur = next) {
			/* tokenize the port list */
			while (*cur == ' ')
				cur ++;
			next = strstr(cur, " ");
			len = next ? next - cur : strlen(cur);
			if (!len)
				break;
			if (len > sizeof(port) - 1)
				len = sizeof(port) - 1;
			strncpy(port, cur, len);
			port[len] = 0;

			/* make sure port # is within the range */
			/* pid = atoi(port); */
			/* no atoi so do it manually */
			/* assume port number is only one digit */
			pid = port[0]-'0';
			ET_TRACE(("%s: processing port %d in vlan%dports \n",
						__FUNCTION__, pid, vid));
			if (pid >= pdescsz) {
				ET_ERROR(("%s: port %d in vlan%dports is out of range[0-%d]\n",
						__FUNCTION__, pid, vid, pdescsz));
				continue;
			}

			/* build VLAN registers values */
			untag |= pdesc[pid].untag;

			member |= pdesc[pid].member;

			/* set port tag - applies to untagged ingress frames */
			/* Default Port Tag Register (Page 0x34, Address 0x10-0x1D) */
#define	FL	FLAG_LAN
			if (!pdesc[pid].cpu || strchr(port, FL)) {
				val16 = ((0 << 13) |		/* priority - always 0 */
				         vid);			/* vlan id */
				robo->ops->write_reg(robo, PAGE_VLAN, pdesc[pid].ptagr,
				                     &val16, sizeof(val16));
			}
		}

		/* Set the VLAN Id in VLAN ID Index Register */
		val8 = vid;
		robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_VINDX,
		                     &val8, sizeof(val8));

		/* Set the MAC addr and VLAN Id in ARL Table MAC/VID Entry 0
		 * Register.
		 */
		arl_entry[6] = vid;
		arl_entry[7] = 0x0;
		robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_ARL_E0,
		                     arl_entry, sizeof(arl_entry));

		/* Set the Static bit , Valid bit and Port ID fields in
		 * ARL Table Data Entry 0 Register
		 */
		val16 = 0xc008;
		robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_DAT_E0,
		                     &val16, sizeof(val16));

		/* Clear the ARL_R/W bit and set the START/DONE bit in
		 * the ARL Read/Write Control Register.
		 */
		val8 = 0x80;
		robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_CTRL,
		                     &val8, sizeof(val8));
		/* Wait for write to complete */
		SPINWAIT((robo->ops->read_reg(robo, PAGE_VTBL, REG_VTBL_CTRL,
		         &val8, sizeof(val8)), ((val8 & 0x80) != 0)),
		         100 /* usec */);

vlan_setup:
		/* setup VLAN ID and VLAN memberships */

		val32 = (untag |			/* untag enable */
		         member);			/* vlan members */

		vtble = REG_VTBL_ENTRY_5395;
		vtbli = REG_VTBL_INDX_5395;
		vtbla = REG_VTBL_ACCESS_5395;

		/* VLAN Table Entry Register (Page 0x05, Address 0x63-0x66/0x83-0x86) */
		robo->ops->write_reg(robo, PAGE_VTBL, vtble, &val32,
		                     sizeof(val32));
		/* VLAN Table Address Index Reg (Page 0x05, Address 0x61-0x62/0x81-0x82) */
		val16 = vid;        /* vlan id */
		robo->ops->write_reg(robo, PAGE_VTBL, vtbli, &val16,
		                     sizeof(val16));

		/* VLAN Table Access Register (Page 0x34, Address 0x60/0x80) */
		val8 = ((1 << 7) | 	/* start command */
		        0);	        /* write */
		robo->ops->write_reg(robo, PAGE_VTBL, vtbla, &val8,
		                     sizeof(val8));
	}

	return 0;
}

/* Enable switching/forwarding */
int
bcm_robo_enable_switch(robo_info_t *robo)
{
	int i, max_port_ind, ret = 0;
	uint8 val8;
	uint16 val16;

	ET_TRACE(("%s: enter\n", __FUNCTION__));

	/* Switch Mode register (Page 0, Address 0x0B) */
	robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_MODE, &val8, sizeof(val8));

	/* Bit 1 enables switching/forwarding */
	if (!(val8 & (1 << 1))) {
		/* Set unmanaged mode */
		val8 &= (~(1 << 0));

		/* Enable forwarding */
		val8 |= (1 << 1);
		robo->ops->write_reg(robo, PAGE_CTRL, REG_CTRL_MODE, &val8, sizeof(val8));

		/* Read back */
		robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_MODE, &val8, sizeof(val8));
		if (!(val8 & (1 << 1))) {
			ET_ERROR(("robo_enable_switch: enabling forwarding failed\n"));
			ret = -1;
		}

		/* No spanning tree for unmanaged mode */
		val8 = 0;
		max_port_ind = REG_CTRL_PORT7;

		for (i = REG_CTRL_PORT0; i <= max_port_ind; i++) {
			if (ROBO_IS_BCM5301X(robo->devid32) && i == REG_CTRL_PORT6)
				continue;
			robo->ops->write_reg(robo, PAGE_CTRL, i, &val8, sizeof(val8));
		}

		/* No spanning tree on IMP port too */
		robo->ops->write_reg(robo, PAGE_CTRL, REG_CTRL_IMP, &val8, sizeof(val8));
	}

	if (ROBO_IS_BCM5301X(robo->devid32)) {
		/*
		 * Port N GMII Port States Override Register (Page 0x00 , address Offset: 0x0e , 0x58-0x5d and 0x5f )
		 * SPEED/ DUPLEX_MODE/ LINK_STS
		 */
#ifdef CFG_SIM
		/* Over ride Port0 ~ Port4 status to make it link by default */
		/* (Port0 ~ Port4) LINK_STS bit default is 0x1(link up), do it anyway */
		for (i = REG_CTRL_PORT0_GMIIPO; i <= REG_CTRL_PORT4_GMIIPO; i++) {
			val8 = 0;
			robo->ops->read_reg(robo, PAGE_CTRL, i, &val8, sizeof(val8));
			val8 |= 0x71;	/* Make it link by default. */
			robo->ops->write_reg(robo, PAGE_CTRL, i, &val8, sizeof(val8));
		}
#endif
		/* Over ride GMAC0 Port5 status to make it link by default */
		val8 = 0;
		robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_PORT5_GMIIPO, &val8, sizeof(val8));
		val8 |= 0x71;	/* Make Link pass and override it. */
		robo->ops->write_reg(robo, PAGE_CTRL, REG_CTRL_PORT5_GMIIPO, &val8, sizeof(val8));

		/* Over ride GMAC1 Port7 status to make it link by default */
		val8 = 0;
		robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_PORT7_GMIIPO, &val8, sizeof(val8));
		val8 |= 0x71;	/* Make Link pass and override it. */
		robo->ops->write_reg(robo, PAGE_CTRL, REG_CTRL_PORT7_GMIIPO, &val8, sizeof(val8));

#if 0
		/* Over ride GMAC2 IMP(Port8) status to make it link by default */
		val8 = 0;
		robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_MIIPO, &val8, sizeof(val8));
		val8 |= 0x71;	/* Make Link pass and override it. */
		robo->ops->write_reg(robo, PAGE_CTRL, REG_CTRL_MIIPO, &val8, sizeof(val8));
#endif
	}

	/* put external ports into protected mode (Page 0, Address 0x24) */
	robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_PPORT, &val16, sizeof(val16));
	/* set external ports */
	val16 |= 0x1f;	/* ports 0 - 4 */
	robo->ops->write_reg(robo, PAGE_CTRL, REG_CTRL_PPORT, &val16, sizeof(val16));


#if (defined(CONFIG_NS_PLUS))
	if ( robo_is_port_cfg(5, PORTCFG_SGMII)
		 	|| robo_is_port_cfg(4, PORTCFG_SGMII) ) {
		/* either port5 or port4 SGMII enabled */
		/* enable serdes */
		robo_serdes_reset_core(robo, PAGE_P5_SGMII);
		if (robo_is_port_cfg(5, PORTCFG_SGMII)) {
			/* enable port5 sgmii */


			robo_serdes_init(robo, PAGE_P5_SGMII);

#ifdef SERDES_LOOPBACK

			//printf("Set loop back port 5\n");
			robo_serdes_lb_set(robo, PAGE_P5_SGMII, 1);
#else
			//printf("\n\n\n---No Loop back set in serdes on port 5");
#endif
		}
		if (robo_is_port_cfg(4, PORTCFG_SGMII)) {
			/* enable port4 sgmii */
			robo_serdes_init(robo, PAGE_P4_SGMII);
#ifdef SERDES_LOOPBACK
			printf("Set loop back port 4\n");
			robo_serdes_lb_set(robo, PAGE_P4_SGMII, 1);
#else
			//printf("\n\n\n---No Loop back set in serdes on port 4");
#endif
		}


		//swang ??? how about port 4????
		/* start serdes pll */
		robo_serdes_start_pll(robo, PAGE_P5_SGMII);
		//robo_serdes_start_pll(robo, PAGE_P4_SGMII);
	}
#endif //defined(CONFIG_NS_PLUS))

	return ret;
}


#if (defined(CONFIG_NS_PLUS))
int
robo_is_port_cfg(int port, char *cfg)
{
	char name[16];
	char *var;

	/* get port5 config */
	sprintf(name, PORTCFG, port);
	var = getenv(name);
	if (var == NULL) {
		/* if no port config then normal port config */
		return 0;
	}

	if (strcmp(var, cfg)==0) {
		/* the port is the configuration we are looing for */
		return 1;
	}

	/* not config we are looking for */
	return 0;
}
#endif /* (defined(CONFIG_NS_PLUS)) */

int
do_robo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int read = 0;
    int args;
    int page, reg, len;
    int val;
    unsigned char val8;
    unsigned short val16;
    unsigned val32;


    if (argc < 2) {
usage:
        printf("Usage: \n%s\n", cmdtp->usage);
        return 1;
    }

    if (!strcmp(argv[1], "read")) {
    	read = 1;
	args = 5;
    } else if (!strcmp(argv[1], "write")) {
         args = 6;
    } else {
         goto usage;
    }

    if (argc < args) {
    	goto usage;
    }

    page = simple_strtoul(argv[2], (char **)0, 0x16);
    reg = simple_strtoul(argv[3], (char **)0, 0x16);
    len = simple_strtoul(argv[4], (char **)0, 0x16);

    if (len != 1 && len != 2 && len != 4) {
    	printf("Incorrect length: %d\n", len);
	return 1;
    }

    if (!read) {
        val = simple_strtoul(argv[5], (char **)0, 0x16);

	switch (len) {
	case 1:
		val8 = val;
		srab_wreg(0, page, reg, &val8, len);
		break;
	case 2:
		val16 = val;
		srab_wreg(0, page, reg, &val16, len);
		break;
	case 4:
		val32 = val;
		srab_wreg(0, page, reg, &val32, len);
		break;
	}
     }
     /* FALL THROUGH */
    switch (len) {
    case 1:
	    srab_rreg(0, page, reg, &val8, len);
	    printf("Value at page %x, reg %x, is 0x%x\n", page, reg, val8);
	    break;
    case 2:
	    srab_rreg(0, page, reg, &val16, len);
	    printf("Value at page %x, reg %x, is 0x%x\n", page, reg, val16);
	    break;
    case 4:
	    srab_rreg(0, page, reg, &val32, len);
	    printf("Value at page %x, reg %x, is 0x%x\n", page, reg, val32);
	    break;
    }


    return 0;
}

U_BOOT_CMD(
    robo,    6,    1,     do_robo,
    "robo    - read/write robo registers\n",                   
    " Usage: robo read|write page reg 1|2|4 [val]\n"
);
#ifdef BCMDBG
void
robo_dump_regs(robo_info_t *robo)
{
	uint8 val8;
	uint16 val16;
	uint32 val32;
	pdesc_t *pdesc;
	int pdescsz;
	int i;
	uint8 vtble, vtbli, vtbla;

	printf("%s %s:\n", __FUNCTION__, robo->ops->desc);

	/* Dump registers interested */
	robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_MODE, &val8, sizeof(val8));
	printf("(0x00,0x0B)Switch mode regsiter: 0x%02x\n", val8);

	pdesc = pdesc97;
	pdescsz = sizeof(pdesc97) / sizeof(pdesc_t);

#if 0
	if (ROBO_IS_BCM5301X(robo->devid32)) {
		/* 5301x use port 5 as mii port connect to GMAC0 */
		pdesc[5].cpu = 1;
		pdescsz -= 3;
	}
#endif

	robo->ops->read_reg(robo, PAGE_VLAN, REG_VLAN_CTRL0, &val8, sizeof(val8));
	printf("(0x34,0x00)VLAN control 0 register: 0x%02x\n", val8);
	robo->ops->read_reg(robo, PAGE_VLAN, REG_VLAN_CTRL1, &val8, sizeof(val8));
	printf("(0x34,0x01)VLAN control 1 register: 0x%02x\n", val8);
	robo->ops->read_reg(robo, PAGE_VLAN, REG_VLAN_CTRL4, &val8, sizeof(val8));

	vtble = REG_VTBL_ENTRY_5395;
	vtbli = REG_VTBL_INDX_5395;
	vtbla = REG_VTBL_ACCESS_5395;

	/* XXX: Show the VLAN Table from 539x */
	for (i = 0; i <= VLAN_MAXVID; i++) {
		/* VLAN Table Address Index Register (Page 0x05, Address 0x61-0x62/0x81-0x82) */
		val16 = i;		/* vlan id */
		robo->ops->write_reg(robo, PAGE_VTBL, vtbli, &val16,
		                     sizeof(val16));
		/* VLAN Table Access Register (Page 0x34, Address 0x60/0x80) */
		val8 = ((1 << 7) | 	/* start command */
		        1);		/* read */
		robo->ops->write_reg(robo, PAGE_VTBL, vtbla, &val8,
		                     sizeof(val8));
		/* VLAN Table Entry Register (Page 0x05, Address 0x63-0x66/0x83-0x86) */
		robo->ops->read_reg(robo, PAGE_VTBL, vtble, &val32,
		                    sizeof(val32));
		printf("VLAN %d untag bits: 0x%02x member bits: 0x%02x\n",
		            i, (val32 & 0x3fe00) >> 9, (val32 & 0x1ff));
	}

	for (i = 0; i < pdescsz; i++) {
		robo->ops->read_reg(robo, PAGE_VLAN, pdesc[i].ptagr, &val16, sizeof(val16));
		printf("(0x34,0x%02x)Port %d Tag: 0x%04x\n", pdesc[i].ptagr, i, val16);
	}
}
#endif /* BCMDBG */
#endif
