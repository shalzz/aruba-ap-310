/******************************************************************************/
/*                                                                            */
/*  Copyright 2011  Broadcom Corporation                                      */
/*                                                                            */
/*     Unless you and Broadcom execute a separate written software license    */
/*     agreement governing  use of this software, this software is licensed   */
/*     to you under the terms of the GNU General Public License version 2     */
/*     (the GPL), available at                                                */
/*                                                                            */
/*          http://www.broadcom.com/licenses/GPLv2.php                        */
/*                                                                            */
/*     with the following added to such license:                              */
/*                                                                            */
/*     As a special exception, the copyright holders of this software give    */
/*     you permission to link this software with independent modules, and to  */
/*     copy and distribute the resulting executable under terms of your       */
/*     choice, provided that you also meet, for each linked independent       */
/*     module, the terms and conditions of the license of that module.        */
/*     An independent module is a module which is not derived from this       */
/*     software.  The special exception does not apply to any modifications   */
/*     of the software.                                                       */
/*     Notwithstanding the above, under no circumstances may you combine      */
/*     this software in any way with any other Broadcom software provided     */
/*     under a license other than the GPL, without Broadcom's express prior   */
/*     written consent.                                                       */
/*                                                                            */
/******************************************************************************/

/* debug/trace */
//#define BCMDBG
#define BCMDBG_ERR
//#define BCMDBG
#ifdef BCMDBG
#define	ET_ERROR(args) printf args
#define	ET_TRACE(args) printf args
#define BCMIPROC_ETH_DEBUG
#elif defined(BCMDBG_ERR)
#define	ET_ERROR(args) printf args
#define ET_TRACE(args)
#undef BCMIPROC_ETH_DEBUG
#else
#define	ET_ERROR(args)
#define	ET_TRACE(args)
#undef BCMIPROC_ETH_DEBUG
#endif /* BCMDBG */

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <config.h>
#ifdef CONFIG_IPROC
#include <miiphy.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/ethHw_dma.h>

#ifdef CONFIG_APBOOT
#define BCMIPROC_ETH_DEV_NAME          "eth"
#else
#define BCMIPROC_ETH_DEV_NAME          "bcmiproc_eth"
#endif

#define BCM_NET_MODULE_DESCRIPTION    "Broadcom BCM IPROC Ethernet driver"
#define BCM_NET_MODULE_VERSION        "0.1"

static const char banner[] = BCM_NET_MODULE_DESCRIPTION " " BCM_NET_MODULE_VERSION "\n";
extern int ethHw_miiphy_read(unsigned int const phyaddr, 
		   unsigned int const reg, unsigned short *const value);
extern int ethHw_miiphy_write(unsigned int const phyaddr, 
		   unsigned int const reg, unsigned short *const value);


/******************************************************************
 * u-boot net functions
 */
static int
bcmiproc_eth_send(struct eth_device *dev, volatile void *packet, int length)
{
	uint8_t *buf = (uint8_t *) packet;
	int rc;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* load buf and start transmit */
	rc = ethHw_dmaTx( length, buf );
	if (rc) {
		ET_ERROR(("ERROR - Tx failed\n"));
		return rc;
	}

	rc = ethHw_dmaTxWait();
	if (rc) {
#ifndef CONFIG_APBOOT
		ET_ERROR(("ERROR - no Tx notice\n"));
#endif
		return rc;
	}

	ET_TRACE(("%s exit rc(0x%x)\n", __FUNCTION__, rc));
	return rc;
}

static int
bcmiproc_eth_rcv(struct eth_device *dev)
{
	int rc;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	rc = ethHw_dmaRx();

	ET_TRACE(("%s exit rc(0x%x)\n", __FUNCTION__, rc));
	return rc;
}


static int
bcmiproc_eth_write_hwaddr(struct eth_device* dev)
{
	int rc=0;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	ET_TRACE(("%s Not going to change MAC address\n", __FUNCTION__));
#if 0
	printf("\nMAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		dev->enetaddr[0], dev->enetaddr[1], dev->enetaddr[2],
		dev->enetaddr[3], dev->enetaddr[4], dev->enetaddr[5]);

	rc = ethHw_arlEntrySet(dev);
	if (rc != ETHHW_RC_NONE) {
		ET_ERROR(("ERROR setting MAC\n"));
		return -1;
	}
#endif

	ET_TRACE(("%s exit rc(0x%x)\n", __FUNCTION__, rc));
	return rc;
}


/* note that we have inverted these return codes due to older infrastructure */
static int
bcmiproc_eth_open( struct eth_device *dev, bd_t * bt )
{
	extern void ethHw_checkPortSpeed(void);
	extern int ethHw_portLinkUp(void);
	int rc=1;
	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* Enable forwarding to internal port */
	ethHw_macEnableSet(ETHHW_PORT_INT, 1);

	/* enable tx and rx DMA */
	ethHw_dmaEnable(DMA_RX);
	ethHw_dmaEnable(DMA_TX);

#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_KATANA2)) || (defined(CONFIG_NORTHSTAR) && defined(CONFIG_APBOOT))
	/* Check which port is connected and take PORT0 with priority */
	if ( !ethHw_portLinkUp() ) {
//		error("Ethernet external port not connected");
//		printf("%s:%d %s: link down\n", __FUNCTION__, __LINE__, dev->name);
		return 0;//-1;
	}
	ethHw_checkPortSpeed();
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_KATANA2)) */

	ET_TRACE(("%s exit rc(0x%x)\n", __FUNCTION__, rc));
	return rc;
}


static void
bcmiproc_eth_close(struct eth_device *dev)
{
	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* disable DMA */
	ethHw_dmaDisable(DMA_RX);
	ethHw_dmaDisable(DMA_TX);

	/* Disable forwarding to internal port */
	ethHw_macEnableSet(ETHHW_PORT_INT, 0);

	ET_TRACE(("%s exit\n", __FUNCTION__));
}


int
bcmiproc_miiphy_read(char *devname, unsigned char addr, 
   unsigned char reg, unsigned short *value)
{
	//printf("%s devname(%s) addr(0x%x) reg(0x%x)\n", __FUNCTION__, devname, addr, reg);
	return ethHw_miiphy_read(addr, reg, value);
}

int
bcmiproc_miiphy_write(char *devname, unsigned char addr, 
   unsigned char reg, unsigned short value)
{
	//printf("%s devname(%s) addr(0x%x) reg(0x%x)\n", __FUNCTION__, devname, addr, reg);
	return ethHw_miiphy_write(addr, reg, &value);
}

/*
 * GPHY register accesses on Northstar are through SPI pages starting with
 * 0x10. We use the SRAB registers in chipcommonB to access them
 * Note that the register offsets are 0, 2, 4, etc..., not 0, 1, 2, as is
 * usually the case
 */
extern int srab_rreg(void *robo, uint8 page, uint8 reg, void *val, int len);
extern int srab_wreg(void *robo, uint8 page, uint8 reg, void *val, int len);

#define PAGE_PHY 0x10  /* first GPHY page */

int
bcmiproc_miiphy_read_wrap(char *devname, unsigned char addr, 
   unsigned char reg, unsigned short *value)
{
    //printf("%s devname(%s) addr(0x%x) reg(0x%x) value %04x\n", __FUNCTION__, devname, (addr + PAGE_PHY), reg, *value);
    return srab_rreg(NULL, addr + PAGE_PHY, reg << 1, value, 2);
}

int
bcmiproc_miiphy_write_wrap(char *devname, unsigned char addr, 
   unsigned char reg, unsigned short value)
{
	//printf("%s devname(%s) addr(0x%x) reg(0x%x) value %04x\n", __FUNCTION__, devname, (addr+ PAGE_PHY), reg, value);
        return srab_wreg(NULL, addr + PAGE_PHY, reg << 1, &value, 2);
}


int
bcmiproc_eth_register(u8 dev_num)
{
	struct eth_device *dev;
	int rc;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	dev = (struct eth_device *) malloc(sizeof(struct eth_device));
	if (dev == NULL) {
		return -1;
	}
	memset(dev, 0, sizeof(*dev));
#ifdef CONFIG_APBOOT
	sprintf(dev->name, "%s%hu", BCMIPROC_ETH_DEV_NAME, dev_num);
#else
	sprintf(dev->name, "%s-%hu", BCMIPROC_ETH_DEV_NAME, dev_num);

	printf(banner);
#endif

	/* Initialization */
	ET_TRACE(("Ethernet initialization...\n"));
	rc = ethHw_Init();
	ET_TRACE(("Ethernet initialization %s (rc=%i)\n",
			(rc>=0) ? "successful" : "failed", rc));

	dev->iobase = 0;

	dev->init = bcmiproc_eth_open;
	dev->halt = bcmiproc_eth_close;
	dev->send = bcmiproc_eth_send;
	dev->recv = bcmiproc_eth_rcv;
	dev->write_hwaddr = bcmiproc_eth_write_hwaddr;

	eth_register(dev);

	ET_TRACE(("Ethernet Driver registered...\n"));

#ifdef CONFIG_CMD_MII
#ifdef CONFIG_APBOOT
   miiphy_register(dev->name, bcmiproc_miiphy_read_wrap, bcmiproc_miiphy_write_wrap);
#else
   miiphy_register(dev->name, bcmiproc_miiphy_read, bcmiproc_miiphy_write);
#endif
#endif

	ET_TRACE(("%s exit\n", __FUNCTION__));
	return 1;
}
#endif /* CONFIG_IPROC */
