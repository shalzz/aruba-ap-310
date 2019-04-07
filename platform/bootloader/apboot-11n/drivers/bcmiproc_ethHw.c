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
/****************************************************************************/
/**
*  @file    ethHw.c
*
*  @brief   Low level ETH driver routines
*
*  @note
*
*   These routines provide basic ETH functionality only. Intended for use
*   with boot and other simple applications.
*/
/****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */
#include <config.h>
#ifdef CONFIG_IPROC
#include <common.h>
#include <net.h>
#include <asm/arch/bcmenetphy.h>
#include <asm/arch/iproc_regs.h>
#include <asm/arch/iproc_gmac_regs.h>
#include <asm/iproc/reg_utils.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/ethHw_dma.h>
#include <asm/arch/bcmgmacmib.h>
#include <asm/arch/bcmrobo.h>
#include <asm/arch/bcmutils.h>
#include <asm/armv7.h>
#include <asm/iproc/iproc_common.h>
#include "ethHw_data.h"
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
#include "bcmiproc_serdes.h"
#include "bcmiproc_phy5461s.h"
#endif /*(defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))*/
#if defined(CONFIG_HURRICANE2)
#include "bcmiproc_phy5221.h"
#endif
#if (defined(CONFIG_NS_PLUS))
#include "bcmiproc_robo_serdes.h"
#endif
#ifdef CONFIG_APBOOT
#include <miiphy.h>
#endif

/* ---- External Variable Declarations ----------------------------------- */
/* ---- External Function Prototypes ------------------------------------- */
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */
/* debug/trace */
//#define BCMDBG
#define BCMDBG_ERR
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

#define CHIP_HAS_ROBO(id) ( ((id) >= CHIP_ID_NS_10 && (id) <= CHIP_ID_CoNS_19) || ((id) == CHIP_ID_NSP_22) || ((id) == CHIP_ID_NSP_23) || ((id) == CHIP_ID_NSP_25))

uint32_t reg_debug = 0;

bcm_eth_t g_eth_data;

uint32_t rxDescBuf = 0;
uint32_t rxDescAlignedBuf = 0;
uint32_t rxDataBuf = 0;
uint32_t txDescBuf = 0;
uint32_t txDescAlignedBuf = 0;
uint32_t txDataBuf = 0;

#ifndef ASSERT
#define ASSERT(exp)
#endif

/* protypes */
int ethHw_chipAttach(bcm_eth_t *eth_data);
void ethHw_chipDetach(bcm_eth_t *eth_data);
int ethHw_dmaInit(bcm_eth_t *eth_data);
int ethHw_dmaRxInit(bcm_eth_t *eth_data);
int ethHw_dmaTxInit(bcm_eth_t *eth_data);
int ethHw_dmaAttach(bcm_eth_t *eth_data);
int ethHw_dmaDetach(dma_info_t *di);

#ifdef BCMIPROC_ETH_DEBUG
static void txDump(uint8_t *buf, int len);
static void dmaTxDump(bcm_eth_t *eth_data);
static void dmaRxDump(bcm_eth_t *eth_data);
static void gmacRegDump(bcm_eth_t *eth_data);
static void gmac_mibTxDump(bcm_eth_t *eth_data);
static void gmac_mibRxDump(bcm_eth_t *eth_data);
#endif
static uint dma_ctrlflags(dma_info_t *di, uint mask, uint flags);
static int dma_rxenable(dma_info_t *di);
static void dma_txinit(dma_info_t *di);
static void dma_rxinit(dma_info_t *di);
static bool dma_txreset(dma_info_t *di);
static bool dma_rxreset(dma_info_t *di);
static int dma_txload(int index, size_t len, uint8_t * tx_buf);
static void *dma_getnextrxp(dma_info_t *di, int *index, size_t *len,
		uint32_t *stat0, uint32_t *stat1);
static void dma_rxrefill(dma_info_t *di, int index);
static void gmac_init_reset(bcm_eth_t *eth_data);
static void gmac_clear_reset(bcm_eth_t *eth_data);
static void gmac_loopback(bcm_eth_t *eth_data, bool on);
static void gmac_reset(bcm_eth_t *eth_data);
static void gmac_clearmib(bcm_eth_t *eth_data);
static int gmac_speed(bcm_eth_t *eth_data, uint32_t speed);
static void gmac_tx_flowcontrol(bcm_eth_t *eth_data, bool on);
static void gmac_promisc(bcm_eth_t *eth_data, bool mode);
static void gmac_enable(bcm_eth_t *eth_data, bool en);
static void gmac_core_reset(void);
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
static void gmac_serdes_init(bcm_eth_t *eth_data);
#endif /*(defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))*/
void chip_phy_wr(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg, uint16_t v);
uint16_t chip_phy_rd(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg);
static void chip_reset(bcm_eth_t *eth_data);
static void chip_init(bcm_eth_t *eth_data, uint options);
static uint32_t chip_getintr_events(bcm_eth_t *eth_data, bool in_isr);

#if (defined(CONFIG_NS_PLUS))
static u32 ethHw_readl(u32 addr);
static void ethHw_writel(u32 val,u32 addr);
#endif

#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
void gmac_set_amac_mdio(int en)
{
	u32 tmp;
	tmp = *(volatile unsigned int *)IPROC_WRAP_MISC_CONTROL;
	//printf("%s read 0x%x from IPROC_WRAP_MISC_CONTROL(0x%x)\n", __FUNCTION__, tmp, IPROC_WRAP_MISC_CONTROL);
	if (en) {
		/* set bits 3&2 so AMAC can access the Serdes and Phy */
#if defined(CONFIG_HELIX4)
		tmp |= ((1<<IPROC_WRAP_MISC_CONTROL__QUAD_SERDES_MDIO_SEL)|(1<<IPROC_WRAP_MISC_CONTROL__QUAD_SERDES_CTRL_SEL)|
#else
		tmp |= ((1<<IPROC_WRAP_MISC_CONTROL__UNICORE_SERDES_MDIO_SEL)|(1<<IPROC_WRAP_MISC_CONTROL__UNICORE_SERDES_CTRL_SEL)|
#endif /*defined(CONFIG_HELIX4)*/
				(1<<IPROC_WRAP_MISC_CONTROL__IPROC_MDIO_SEL));
	} else {
		/* clear bits 3&2 so CMIC can access the Serdes and Phy */
#if defined(CONFIG_HELIX4)
		tmp &= ~((1<<IPROC_WRAP_MISC_CONTROL__QUAD_SERDES_MDIO_SEL)|(1<<IPROC_WRAP_MISC_CONTROL__QUAD_SERDES_CTRL_SEL)|
#else
		tmp &= ~((1<<IPROC_WRAP_MISC_CONTROL__UNICORE_SERDES_MDIO_SEL)|(1<<IPROC_WRAP_MISC_CONTROL__UNICORE_SERDES_CTRL_SEL)|
#endif /*defined(CONFIG_HELIX4)*/
				(1<<IPROC_WRAP_MISC_CONTROL__IPROC_MDIO_SEL));
	}
	//printf("%s write 0x%x to IPROC_WRAP_MISC_CONTROL(0x%x)\n", __FUNCTION__, tmp, IPROC_WRAP_MISC_CONTROL);
	*(volatile unsigned int *)IPROC_WRAP_MISC_CONTROL = tmp;

	//tmp = *(volatile unsigned int *)IPROC_WRAP_MISC_CONTROL;
	//printf("%s read 0x%x from IPROC_WRAP_MISC_CONTROL(0x%x)\n", __FUNCTION__, tmp, IPROC_WRAP_MISC_CONTROL);
}
#endif /*(defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))*/


#define LINE_SIZE	32
void gmac_cache_flush(u32 start, u32 len)
{
	u32 beg=start&(~(LINE_SIZE - 1));
	u32 linelen = (len+(LINE_SIZE - 1))&(~(LINE_SIZE - 1));
	u32 end=beg+linelen;

/* 	printf("flush: start(0x%x) len(0x%x) beg(0x%x) linelen(0x%x) end(0x%x)\n", */
/* 			start, len, beg, linelen, end);	                                   */
	if (iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2) {
		/* printf("flush: beg(0x%x) end(0x%x)\n", beg, end); */
		v7_outer_cache_flush_range(beg, end);
	}
}


void gmac_cache_inval(u32 start, u32 len)
{
	u32 beg=start&(~(LINE_SIZE - 1));
	u32 linelen = (len+(LINE_SIZE - 1))&(~(LINE_SIZE - 1));
	u32 end=beg+linelen;

/* 	printf("inval: start(0x%x) len(0x%x) beg(0x%x) linelen(0x%x) end(0x%x)\n", */
/* 			start, len, beg, linelen, end);                                    */
	if (iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2) {
		/* printf("inval: beg(0x%x) end(0x%x)\n", beg, end); */
		v7_outer_cache_inval_range(beg, end);
	}
}


/* ==== Public Functions ================================================= */

/*****************************************************************************
* See ethHw.h for API documentation
*****************************************************************************/


int
ethHw_Init(void)
{
	bcm_eth_t *eth_data = &g_eth_data;
	int stat;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* clear out g_eth_data */
	memset(&g_eth_data, 0, sizeof(bcm_eth_t));

	/* load mac number  */
	eth_data->mac = CONFIG_GMAC_NUM;

	/* load mac address */
	switch (eth_data->mac) {
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
		case (ETHHW_MAC_0):
			eth_data->regs = (gmacregs_t *)IPROC_GMAC0_REG_BASE;
			break;
		case (ETHHW_MAC_1):
			eth_data->regs = (gmacregs_t *)IPROC_GMAC1_REG_BASE;
			break;
		case (ETHHW_MAC_2):
			eth_data->regs = (gmacregs_t *)IPROC_GMAC2_REG_BASE;
			break;
#elif (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
		case (ETHHW_MAC_0):
			eth_data->regs = (gmacregs_t *)IPROC_GMAC0_REG_BASE;
			break;
		case (ETHHW_MAC_1):
			eth_data->regs = (gmacregs_t *)IPROC_GMAC1_REG_BASE;
			break;
#elif defined(CONFIG_HURRICANE2)
		case (ETHHW_MAC_0):
			eth_data->regs = (gmacregs_t *)IPROC_GMAC0_REG_BASE;
			break;
#endif
		default:
			ET_ERROR(("ERROR: invalid GMAC specified\n"));
	}

#ifdef BCMDBG
	printf ("Using GMAC%d (0x%x)\n", eth_data->mac, (unsigned int)eth_data->regs);
#endif

	/* load options */
	eth_data->loopback = false;

	/* copy mac addr */
	if (getenv ("ethaddr")) {
		if (!eth_getenv_enetaddr("ethaddr", eth_data->enetaddr)) {
			ET_ERROR(("ERROR: could not get env ethaddr\n"));
		}
	} else {
		ET_ERROR(("ERROR: could not get env ethaddr\n"));
	}

	if (ethHw_dmaInit(eth_data)<0)
		return -1;

	/* reset cores */
	gmac_core_reset();

	stat = ethHw_chipAttach(eth_data);
	if (stat)
		return -1;

	/* init chip  */
	chip_init(eth_data, ET_INIT_FULL);

#ifdef BCMIPROC_ETH_DEBUG
	dmaTxDump(eth_data);
	dmaRxDump(eth_data);
	gmacRegDump(eth_data);
#endif

	return ETHHW_RC_NONE;
}


int
ethHw_Exit(void)
{
	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* free rx descriptors buffer */
	if (rxDescBuf) {
		MFREE(0,(void*)rxDescBuf,0);
		rxDescBuf = 0;
		rxDescAlignedBuf = 0;
	}

	/* allocate rx data buffer */
	if (rxDataBuf) {
#if 0	//defined(CONFIG_NORTHSTAR)
		if(!(iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2))
			rxDataBuf -= 0x80000000;
#endif
		MFREE(0,(void*)rxDataBuf,0);
		rxDataBuf = 0;
	}

	/* free tx descriptors buffer */
	if (txDescBuf) {
		MFREE(0,(void*)txDescBuf,0);
		txDescBuf = 0;
		txDescAlignedBuf = 0;
	}

	/* allocate tx data buffer */
	if (txDataBuf) {
#if 0	//defined(CONFIG_NORTHSTAR)
		if(!(iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2))
			txDataBuf -= 0x80000000;
#endif
		MFREE(0,(void*)txDataBuf,0);
		txDataBuf = 0;
	}

	/* Application code will normally control shutdown of the driver */
	return ETHHW_RC_NONE;
}


int
ethHw_arlEntrySet(struct eth_device *dev)
{
	bcm_eth_t *eth_data = (bcm_eth_t *)dev->priv;
	gmacregs_t *regs = eth_data->regs;
	int i, rc=ETHHW_RC_NONE;

	/* copy mac addr */
	for (i=0; i<ETH_ADDR_LEN; i++)
		eth_data->enetaddr[i] = dev->enetaddr[i];

	/* put mac in reset */
	gmac_init_reset(eth_data);

	reg32_write(&regs->mac_addr_high, htonl(*(uint32_t *)&eth_data->enetaddr[0]));
	reg32_write(&regs->mac_addr_low,  htons(*(uint32_t *)&eth_data->enetaddr[4]));

	/* bring mac out of reset */
	gmac_clear_reset(eth_data);

	return rc;
}


int
ethHw_macEnableSet(int port, int en)
{
	bcm_eth_t *eth_data = &g_eth_data;
	gmacregs_t *regs = eth_data->regs;

	gmac_enable(eth_data, en);

	if (en) {
		/* clear interrupts */
		reg32_write(&regs->int_status, I_INTMASK);
	}

	/* reset phy: reset it once now */

   return ETHHW_RC_NONE;
}


int
ethHw_macEnableGet(int port, int *txp, int *rxp)
{
	bcm_eth_t *eth_data = &g_eth_data;
	gmacregs_t *regs = eth_data->regs;
	uint32_t cmdcfg;

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	*txp = ((cmdcfg | CC_TE) ? 1 : 0);
	*rxp = ((cmdcfg | CC_RE) ? 1 : 0);

	return ETHHW_RC_NONE;
}
#if (defined(CONFIG_NS_PLUS))
static void ethHw_writel(u32 val,u32 addr)
{
	debug("Write [0x%08x] = 0x%08x\n", (u32)addr, val);
	*(volatile u32 *)(addr) = (volatile u32)val;
}

static u32 ethHw_readl(u32 addr)
{
	volatile u32 val = *(volatile u32 *)(addr);
	debug("Read  [0x%08x] = 0x%08x\n", (u32)addr, val);
	return (u32)val;
}
#endif

int
ethHw_chipAttach(bcm_eth_t *eth_data)
{
	bcmgmac_t *ch = &eth_data->bcmgmac;
	int stat;
	int chipid;
	char name[16];
	char *strptr;
//	u32 GPIO_OUTENABLE =0x18000068;//GPIO OUT ENABLE

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	BZERO_SM((char *)ch, sizeof(eth_data->bcmgmac));

	/* get our phyaddr value */
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
	ch->phyaddr = PHY_ADDR;
#else
	ch->phyaddr = CONFIG_GMAC_NUM+1;
#endif
#ifdef CONFIG_APBOOT
	ch->phyaddr_port = IPROC_PHY_ADDR;
#endif
	/* get our phyaddr value */
	sprintf(name, "et%dphyaddr", eth_data->unit);
	if ((strptr = getenv (name)) != NULL) {
		ch->phyaddr = simple_strtoul(strptr, &strptr, 10) & 0x1f;
	}
	//printf("et%d: %s phyaddr (0x%x)\n", eth_data->unit, __FUNCTION__, ch->phyaddr);

	stat = ethHw_dmaAttach(eth_data);
	if (stat) {
		ET_ERROR(("et%d: ethHw_dmaAttach failed\n", eth_data->unit));
		goto fail;
	}

	/* reset the gmac core */
	chip_reset(eth_data);

	ethHw_dmaTxInit(eth_data);
	ethHw_dmaRxInit(eth_data);

	/* set default sofware intmask */
	ch->def_intmask = DEF_INTMASK;

	ch->intmask = ch->def_intmask;

	/* reset phy: reset it once now */
	chipid = iproc_get_chipid();
#ifdef BCMDBG
	printf("et%d: %s: Chip ID: 0x%x; phyaddr: 0x%x\n", eth_data->unit, __FUNCTION__, chipid, eth_data->bcmgmac.phyaddr);
#endif
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	/* flip switch so AMAC can access serdes */
	gmac_set_amac_mdio(1);

	/* if using GMAC1 must enable GMAC0 for GMAC1 to work */
	if (ch->phyaddr!=1) {
		/* using GMAC1, initialize GMAC0 serdes control */
		eth_data->regs = (gmacregs_t *)IPROC_GMAC0_REG_BASE;
		gmac_serdes_init(eth_data);
		eth_data->regs = (gmacregs_t *)IPROC_GMAC1_REG_BASE;
	}
	gmac_serdes_init(eth_data);

	phy5461_init(eth_data, ch->phyaddr);

	serdes_reset(eth_data, ch->phyaddr);
	/* reset core from lane 0  */
	serdes_reset_core(eth_data, 1);
	serdes_init(eth_data, ch->phyaddr);
	/* start pll from lane 0  */
	serdes_start_pll(eth_data, 1);
#endif /*(defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))*/
#if defined(CONFIG_HURRICANE2)
	phy5221_init(eth_data, ch->phyaddr);
	phy5221_enable_set(eth_data, ch->phyaddr, 1);
#endif



#if (defined(CONFIG_NS_PLUS))
#define GPIO_SFP0_TXDIS		26
#define GPIO_SFP1_TXDIS		27
	if ( robo_is_port_cfg(5, PORTCFG_SGMII)
			|| robo_is_port_cfg(4, PORTCFG_SGMII) )
	{
		void *cruresetaddr, *plllockaddr, *lcplladdr, *sgmiicfgaddr;
		uint32 tmp;
		uint	idx;

		//printf("%s Going to initialize SGMII ports\n", __FUNCTION__);
		/* take the LCPLL2 out of powerdown and reset */
		/* unlock access to LCPLL registers */
		plllockaddr = CRU_CLKSET_KEY_OFFSET;
		ethHw_writel(0xea68, plllockaddr);

		/* CRU_LCPLL2_CONTROL0 pwrdwn=0; resetb=1 */
		lcplladdr = CRU_LCPLL2_CONTROL0;
		tmp = ethHw_readl(lcplladdr);
		tmp &= ~(1<<CRU_LCPLL2_CONTROL0__PWRDWN);
		tmp |= (1<<CRU_LCPLL2_CONTROL0__RESETB);
		ethHw_writel(tmp, lcplladdr);
		/* wait for PLL to lock */
		for (idx=0; idx<1000; idx++) {
			tmp = ethHw_readl((lcplladdr+0x18));
			//printf("%s waiting for pll to lock 0x%x\n", __FUNCTION__, tmp);
			if (tmp & (1<<CRU_LCPLL2_STATUS__LOCK)) {
				//printf("%s pll locked\n", __FUNCTION__);
				break;
			}
		}
		if (idx>=1000) {
			printf("%s ERROR: PLL failed to lock\n", __FUNCTION__);
		}
		/* CRU_LCPLL2_CONTROL0 post_resetb=1 */
		tmp = ethHw_readl(lcplladdr);
		tmp |= (1<<CRU_LCPLL2_CONTROL0__POST_RESETB);
		ethHw_writel(tmp, lcplladdr);

		/* Lock access to LCPLL registers */
		ethHw_writel(0, plllockaddr);
		//iounmap(plllockaddr);
		//iounmap(lcplladdr);

		/* take SGMII out of reset */
		cruresetaddr = CRU_RESET;
		tmp = ethHw_readl(cruresetaddr);
		tmp |= (1<<CRU_RESET__SGMII_RESET_N);
		//printf("%s write CRU_RESET: 0x%x\n", __FUNCTION__, tmp);
		ethHw_writel(tmp, cruresetaddr);
		//iounmap(cruresetaddr);

		/* take PLL and MDIOREGS out of reset */
		sgmiicfgaddr = SGMII_CONFIG;
		tmp = ethHw_readl(sgmiicfgaddr);
		//printf("%s read SGMII_CONFIG: 0x%x\n", __FUNCTION__, tmp);
		tmp |= (1<<SGMII_CONFIG__RSTB_PLL);
		tmp |= (1<<SGMII_CONFIG__RSTB_MDIOREGS);
		//tmp |= (1<<SGMII_CONFIG__TXD10G_FIFO_RSTB);
		/* take all 4 1G lines out of reset */
		tmp |= (((1<<SGMII_CONFIG__TXD1G_FIFO_RSTB_WIDTH)-1)<<SGMII_CONFIG__TXD1G_FIFO_RSTB_R);
		//printf("%s write SGMII_CONFIG: 0x%x\n", __FUNCTION__, tmp);
		ethHw_writel(tmp, sgmiicfgaddr);
		//iounmap(sgmiicfgaddr);
	}

	if ( robo_is_port_cfg(5, PORTCFG_SGMII) )
	{
		void *p5muxaddr;
		uint32 tmp, msk;

		/* Get register base address */
		p5muxaddr = (P5_MUX_CONFIG);
		tmp = ethHw_readl(p5muxaddr);
		msk = (1<<P5_MUX_CONFIG__P5_MODE_WIDTH)-1;
		tmp &= ~(msk<<P5_MUX_CONFIG__P5_MODE_R);
		tmp |= (P5_MUX_CONFIG__P5_MODE_SGMII<<P5_MUX_CONFIG__P5_MODE_R);
		//printf("%s write P5_MUX_CONFIG: 0x%x to reg 0x%x\n", __FUNCTION__, tmp, P5_MUX_CONFIG);
		ethHw_writel(tmp, p5muxaddr);
		//printf("%s Read P5_MUX_CONFIG: 0x%x at reg 0x%x\n", __FUNCTION__, ethHw_readl(p5muxaddr), P5_MUX_CONFIG);
		//iounmap(p5muxaddr);

		/* turnoff tx disable SFP0_TXDIS - GPIO 26*/
		//printf("%s set GPIO26 output 0\n", __FUNCTION__);

		tmp = ethHw_readl(GPIO_OUTENABLE);
		tmp |= ( 1<< 26 );
		ethHw_writel(tmp, GPIO_OUTENABLE);



	}
	else if ( robo_is_port_cfg(5, PORTCFG_GPHY) )
	{
		void *p5muxaddr;
		uint32 tmp, msk;

		/* Get register base address */
		p5muxaddr = P5_MUX_CONFIG;
		tmp = ethHw_readl(p5muxaddr);
		msk = (1<<P5_MUX_CONFIG__P5_MODE_WIDTH)-1;
		tmp &= ~(msk<<P5_MUX_CONFIG__P5_MODE_R);
		tmp |= (P5_MUX_CONFIG__P5_MODE_GPHY3<<P5_MUX_CONFIG__P5_MODE_R);
		//printf("%s write P5_MUX_CONFIG: 0x%x\n", __FUNCTION__, tmp);
		ethHw_writel(tmp, p5muxaddr);
		//iounmap(p5muxaddr);
	}

	if ( robo_is_port_cfg(4, PORTCFG_SGMII) )
	{
		void *p4muxaddr;
		uint32 tmp, msk;

		/* Configure P4 for SGMII */
		p4muxaddr = (P4_MUX_CONFIG);
		tmp = ethHw_readl(p4muxaddr);
		msk = (1<<P4_MUX_CONFIG__P4_MODE_WIDTH)-1;
		tmp &= ~(msk<<P4_MUX_CONFIG__P4_MODE_R);
		tmp |= (P4_MUX_CONFIG__P4_MODE_SGMII<<P4_MUX_CONFIG__P4_MODE_R);
		//printf("%s write P4_MUX_CONFIG: 0x%x,reg 0x%x\n", __FUNCTION__, tmp,p4muxaddr);
		ethHw_writel(tmp, p4muxaddr);
		//printf("%s Read P4_MUX_CONFIG: 0x%x at reg 0x%x\n", __FUNCTION__, ethHw_readl(p4muxaddr), P4_MUX_CONFIG);
		//iounmap(p4muxaddr);

		/* turnoff tx disable SFP1_TXDIS - GPIO 27*/
		//printf("%s set GPIO27 output 0\n", __FUNCTION__);

		tmp = ethHw_readl(GPIO_OUTENABLE);
		tmp |= ( 1<< 27 );
		ethHw_writel(tmp, GPIO_OUTENABLE);
	}
#endif //defined(CONFIG_NS_PLUS))



	if (CHIP_HAS_ROBO(chipid)) {
		/*
		 * Broadcom Robo ethernet switch.
		 */
		/* Attach to the switch */
		if (!(ch->robo = bcm_robo_attach(ch->sih, ch, ch->vars,
		                                  NULL,
		                                  NULL))) {
			ET_ERROR(("et%d: chipattach: robo_attach failed\n", eth_data->unit));
			goto fail;
		}
		/* Enable the switch and set it to a known good state */
		if (bcm_robo_enable_device(ch->robo)) {
			ET_ERROR(("et%d: chipattach: robo_enable_device failed\n", eth_data->unit));
			goto fail;
		}
		/* Configure the switch to do VLAN */
		if (bcm_robo_config_vlan(ch->robo, eth_data->enetaddr)) {
			ET_ERROR(("et%d: chipattach: robo_config_vlan failed\n", eth_data->unit));
			goto fail;
		}
		/* Enable switching/forwarding */
		if (bcm_robo_enable_switch(ch->robo)) {
			ET_ERROR(("et%d: chipattach: robo_enable_switch failed\n", eth_data->unit));
			goto fail;
		}
	}

#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_KATANA2))
	/* Check which port is connected and take PORT0 with priority */
	if ( !ethHw_portLinkUp() ) {
		//error("Ethernet external port not connected");
		return -1;
	}
	ethHw_checkPortSpeed();
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_KATANA2)) */

	return 0;

fail:
	return -1;
}


void
ethHw_chipDetach(bcm_eth_t *eth_data)
{
	bcmgmac_t *ch = &eth_data->bcmgmac;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	/* free robo state */
	if (ch->robo)
		bcm_robo_detach((robo_info_t*)ch->robo);

	ethHw_dmaDetach(ch->di[0]);
	ch->di[0] = NULL;
}

int
ethHw_dmaInit(bcm_eth_t *eth_data)
{
	int buflen;
	uint32_t alloc_ptr = IPROC_ETH_MALLOC_BASE;

	/* allocate rx descriptors buffer */
	buflen = RX_DESC_LEN+0x10;
	
	//rxDescBuf = (uint32_t)MALLOC(0, buflen);
	rxDescBuf = alloc_ptr;
	alloc_ptr += buflen;
	if (rxDescBuf == 0) {
		ET_ERROR(("%s: Failed to allocate RX Descriptor memory\n", __FUNCTION__));
		return -1;
	}
	rxDescAlignedBuf = rxDescBuf;
	ET_TRACE(("RX Descriptor Buffer: 0x%x; length: 0x%x\n", rxDescBuf, buflen));
	//printf("RX Descriptor Buffer: 0x%x; length: 0x%x\n", rxDescBuf, buflen);
	/* check if need to align buffer */
	if (rxDescAlignedBuf&0x0f) {
		/* align buffer */
		rxDescAlignedBuf = (rxDescBuf+0x10)&0xfffffff0;
	}
#if 0	//defined(CONFIG_NORTHSTAR)
	if(!(iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2))
		rxDescAlignedBuf += 0x80000000; /* Move to ACP region */
#endif
	ET_TRACE(("RX Descriptor Buffer (aligned): 0x%x; length: 0x%x\n", rxDescAlignedBuf, RX_DESC_LEN));
	//printf("RX Descriptor Buffer (aligned): 0x%x; length: 0x%x\n", rxDescAlignedBuf, RX_DESC_LEN);

	/* allocate rx data buffer */
	buflen = RX_BUF_LEN;
	//rxDataBuf = (uint32_t)MALLOC(0, buflen);
	rxDataBuf = alloc_ptr;
	alloc_ptr += buflen;
	if (rxDataBuf == 0) {
		ET_ERROR(("%s: Failed to allocate RX Data Buffer memory\n", __FUNCTION__));
		return -1;
	}
#if 0	//defined(CONFIG_NORTHSTAR)
	if(!(iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2))
		rxDataBuf += 0x80000000; /* Move to ACP region */
#endif
	ET_TRACE(("RX Data Buffer: 0x%x; length: 0x%x\n", rxDataBuf, buflen));
	//printf("RX Data Buffer: 0x%x; length: 0x%x\n", rxDataBuf, buflen);

	/* allocate tx descriptors buffer */
	buflen = TX_DESC_LEN+0x10;
	//txDescBuf = (uint32_t)MALLOC(0, buflen);
	txDescBuf = alloc_ptr;
	alloc_ptr += buflen;
	if (txDescBuf == 0) {
		ET_ERROR(("%s: Failed to allocate TX Descriptor memory\n", __FUNCTION__));
		return -1;
	}
	txDescAlignedBuf = txDescBuf;
	ET_TRACE(("TX Descriptor Buffer: 0x%x; length: 0x%x\n", txDescBuf, buflen));
	//printf("TX Descriptor Buffer: 0x%x; length: 0x%x\n", txDescBuf, buflen);
	/* check if need to align buffer */
	if (txDescAlignedBuf&0x0f) {
		/* align buffer */
		txDescAlignedBuf = (txDescBuf+0x10)&0xfffffff0;
	}
#if 0	//defined(CONFIG_NORTHSTAR)
	if(!(iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2))
		txDescAlignedBuf += 0x80000000; /* Move to ACP region */
#endif
	ET_TRACE(("TX Descriptor Buffer (aligned): 0x%x; length: 0x%x\n", txDescAlignedBuf, TX_DESC_LEN));
	//printf("TX Descriptor Buffer (aligned): 0x%x; length: 0x%x\n", txDescAlignedBuf, TX_DESC_LEN);

	/* allocate tx data buffer */
	buflen = TX_BUF_LEN;
	//txDataBuf = (uint32_t)MALLOC(0, buflen);
	txDataBuf = alloc_ptr;
	alloc_ptr += buflen;
	if (txDataBuf == 0) {
		ET_ERROR(("%s: Failed to allocate TX Data Buffer memory\n", __FUNCTION__));
		return -1;
	}
#if 0	//defined(CONFIG_NORTHSTAR)
	if(!(iproc_get_chipid() == 53010 && iproc_get_chiprev() < CHIP_REV_A2))
		txDataBuf += 0x80000000; /* Move to ACP region */
#endif
	ET_TRACE(("TX Data Buffer: 0x%x; length: 0x%x\n", txDataBuf, buflen));
	//printf("TX Data Buffer: 0x%x; length: 0x%x\n", txDataBuf, buflen);

	return 0;
}


int
ethHw_dmaRxInit(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	uint32_t lastDscr;
	dma64dd_t *descp = NULL;
	uint8_t *bufp;
	int i;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	/* clear descriptor memory */
	BZERO_SM((void*)RX_DESC_BASE, RX_DESC_LEN);
	/* clear buffer memory */
	BZERO_SM((void*)RX_BUF_BASE, RX_BUF_LEN);

	/* Initialize RX DMA descriptor table */
	for (i = 0; i < RX_BUF_NUM; i++) {
		uint32_t ctrl;
		bufp = (uint8_t *) RX_BUF(i);
		descp = (dma64dd_t *) RX_DESC(i);
		ctrl = 0;
		/* if last descr set endOfTable */
		if (i==RX_BUF_NUM-1)
			ctrl = D64_CTRL1_EOT;
		descp->ctrl1 = ctrl;
		descp->ctrl2 = RX_BUF_SIZE;
		descp->addrlow = (uint32_t)bufp;
		descp->addrhigh = 0;
		/* flush descriptor */
		//gmac_cache_flush((u32)descp, sizeof(dma64dd_t));

		lastDscr = ((uint32_t)(descp) & D64_XP_LD_MASK) + sizeof(dma64dd_t);
	}

	/* initailize the DMA channel */
	reg32_write(&dma->d64rxregs->addrlow, RX_DESC_BASE);
	reg32_write(&dma->d64rxregs->addrhigh, 0);

	/* now update the dma last descriptor */
	reg32_write(&dma->d64rxregs->ptr, lastDscr);

	dma->rcvptrbase = RX_DESC_BASE;

	return 0;
}

int
ethHw_dmaTxInit(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	dma64dd_t *descp = NULL;
	uint8_t *bufp;
	int i;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	/* clear descriptor memory */
	BZERO_SM((void*)TX_DESC_BASE, TX_DESC_LEN);
	/* clear buffer memory */
	BZERO_SM((void*)TX_BUF_BASE, TX_BUF_LEN);

	/* Initialize TX DMA descriptor table */
	for (i = 0; i < TX_BUF_NUM; i++) {
		uint32_t ctrl;
		bufp = (uint8_t *) TX_BUF(i);
		descp = (dma64dd_t *) TX_DESC(i);
		ctrl = 0;
		/* if last descr set endOfTable */
		if (i==TX_BUF_NUM-1)
			ctrl = D64_CTRL1_EOT;
		descp->ctrl1 = ctrl;
		descp->ctrl2 = 0;
		descp->addrlow = (uint32_t)bufp;
		descp->addrhigh = 0;
		/* flush descriptor */
		//gmac_cache_flush((u32)descp, sizeof(dma64dd_t));
	}

	/* initailize the DMA channel */
	reg32_write(&dma->d64txregs->addrlow, TX_DESC_BASE);
	reg32_write(&dma->d64txregs->addrhigh, 0);

	dma->xmtptrbase = TX_DESC_BASE;

	/* now update the dma last descriptor */
	reg32_write(&dma->d64txregs->ptr, TX_DESC_BASE&D64_XP_LD_MASK);

	return 0;
}


int
ethHw_dmaTx(size_t len, uint8_t * tx_buf)
{
	/* kick off the dma */
	bcm_eth_t *eth_data = &g_eth_data;
	dma_info_t *di = eth_data->bcmgmac.di[0];
	int txout = di->txout;
	int ntxd = di->ntxd;
	uint32_t flags;
	dma64dd_t *descp = NULL;
	uint8_t *bufp;
	uint32_t ctrl2;
	uint32_t lastDscr = ((uint32_t)(TX_DESC(1)) & D64_XP_LD_MASK);
	size_t buflen;

	REG_DEBUG(1);
	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	if (txout!=0 && txout!=1) {
		ET_ERROR(("%s: ERROR - invalid txout 0x%x\n", __FUNCTION__, txout));
		REG_DEBUG(0);
		return -1;
	}

	/* load the buffer */
	buflen = dma_txload(txout, len, tx_buf);

	ctrl2 = (buflen & D64_CTRL2_BC_MASK);

	/* the transmit will only be one frame or set SOF, EOF */
	/* also set int on completion */
	flags = D64_CTRL1_SOF | D64_CTRL1_IOC | D64_CTRL1_EOF;

	/* txout points to the descriptor to uset */
	/* if last descriptor then set EOT */
	if (txout == (ntxd - 1)) {
		flags |= D64_CTRL1_EOT;
		lastDscr = ((uint32_t)(TX_DESC(0)) & D64_XP_LD_MASK);
	}

	/* write the descriptor */
	bufp = (uint8_t *) TX_BUF(txout);
	descp = (dma64dd_t *) TX_DESC(txout);
	descp->addrlow = (uint32_t)bufp;
	descp->addrhigh = 0;
	descp->ctrl1 = flags;
	descp->ctrl2 = ctrl2;

	/* flush descriptor and buffer */
	//gmac_cache_flush((u32)descp, sizeof(dma64dd_t));
	//gmac_cache_flush((u32)bufp, buflen);

	/* now update the dma last descriptor */
	reg32_write(&di->d64txregs->ptr, lastDscr);

	/* tx dma should be enabled so packet should go out */

	/* update txout */
	di->txout = NEXTTXD(txout);
	REG_DEBUG(0);

#if 0 //defined(CONFIG_HELIX4)
	{
	bcmgmac_t *ch = &eth_data->bcmgmac;
	serdes_disp_status(eth_data, ch->phyaddr);
	phy5461_disp_status(eth_data, ch->phyaddr);
	}
#endif
#if defined(CONFIG_HURRICANE2)
#if defined(CHK_ETH_ERRS)
	{
	bcmgmac_t *ch = &eth_data->bcmgmac;
	phy5221_chk_err(eth_data, ch->phyaddr);
	//phy5221_disp_status(eth_data, ch->phyaddr);
	}
#endif
#endif
	return 0;
}


int
ethHw_dmaTxWait(void)
{
	/* wait for tx to complete */
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	uint32_t intstatus;
	int xfrdone=false;
	int i=0;
#if defined(CHK_ETH_ERRS)
	dma_info_t *di = ch->di[0];
	uint32_t stat1
#endif

	//REG_DEBUG(1);
	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	/* clear stored intstatus */
	ch->intstatus = 0;

	intstatus = chip_getintr_events(eth_data, true);
	ET_TRACE(("int(0x%x)\n", intstatus));
	if (intstatus & (I_XI0 | I_XI1 | I_XI2 | I_XI3))
		xfrdone=true;

	//printf("Waiting for TX to complete");

#if defined(CHK_ETH_ERRS)
	stat1 = reg32_read(&di->d64txregs->status1);
	if (stat1&0xf0000000)
		printf("%s stat1 (0x%x)\n", __FUNCTION__, stat1);
#endif

	while (!xfrdone) {
		udelay(100);
		//printf(".");
		i++;
		if (i > 20) {
#ifndef CONFIG_APBOOT
			printf("\nbcm iproc ethernet Tx failure! Already retried 20 times\n");
#endif
			//REG_DEBUG(0);
			return -1;
		}
		intstatus = chip_getintr_events(eth_data, true);
		ET_TRACE(("int(0x%x)", intstatus));
		if (intstatus & (I_XI0 | I_XI1 | I_XI2 | I_XI3))
			xfrdone=true;
		else if (intstatus) {
#ifndef CONFIG_APBOOT
			printf("int(0x%x)", intstatus);
#endif
		}
	}

	//printf("\n");
	//REG_DEBUG(0);

#ifdef BCMIPROC_ETH_DEBUG
	dmaTxDump(eth_data);
	gmacRegDump(eth_data);
	gmac_mibTxDump(eth_data);
#endif

	return 0;
}


#if defined(CHK_ETH_ERRS)
/* get current and pending interrupt events */
static void
check_errs(gmacregs_t *regs)
{
	static uint32 crserrs=0;
	uint32 err;

	err = reg32_read(&regs->mib.tx_jabber_pkts);
	if (err)
		printf("%s tx_jabber_pkts (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.tx_oversize_pkts);
	if (err)
		printf("%s tx_oversize_pkts (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.tx_fragment_pkts);
	if (err)
		printf("%s tx_fragment_pkts (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.tx_underruns);
	if (err)
		printf("%s tx_underruns (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.tx_total_cols);
	if (err)
		printf("%s tx_total_cols (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.tx_single_cols);
	if (err)
		printf("%s tx_single_cols (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.tx_multiple_cols);
	if (err)
		printf("%s tx_multiple_cols (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.tx_excessive_cols);
	if (err)
		printf("%s tx_excessive_cols (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.tx_late_cols);
	if (err)
		printf("%s tx_late_cols (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.tx_defered);
	if (err)
		printf("%s tx_defered (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.tx_carrier_lost);
	crserrs+=err;
//	if (err)
//		printf("%s tx_carrier_lost (0x%x)\n", __FUNCTION__, err);
//		udelay(2000);
//	if (crserrs>100) {
//		printf("%s tx_carrier_lost crserrs(0x%x)\n", __FUNCTION__, crserrs);
//		crserrs=0;
//	}
	err = reg32_read(&regs->mib.tx_pause_pkts);
	if (err)
		printf("%s tx_pause_pkts (0x%x)\n", __FUNCTION__, err);

	err = reg32_read(&regs->mib.rx_jabber_pkts);
	if (err)
		printf("%s rx_jabber_pkts (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_oversize_pkts);
	if (err)
		printf("%s rx_oversize_pkts (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_fragment_pkts);
	if (err)
		printf("%s rx_fragment_pkts (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_missed_pkts);
	if (err)
		printf("%s rx_missed_pkts (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_crc_align_errs);
	if (err)
		printf("%s rx_crc_align_errs (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_undersize);
	if (err)
		printf("%s rx_undersize (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_crc_errs);
	if (err)
		printf("%s rx_crc_errs (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_align_errs);
	if (err)
		printf("%s rx_align_errs (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_symbol_errs);
	if (err)
		printf("%s rx_symbol_errs (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_pause_pkts);
	if (err)
		printf("%s rx_pause_pkts (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_nonpause_pkts);
	if (err)
		printf("%s rx_nonpause_pkts (0x%x)\n", __FUNCTION__, err);
	err = reg32_read(&regs->mib.rx_sachanges);
	if (err)
		printf("%s rx_sachanges (0x%x)\n", __FUNCTION__, err);
}
#endif


int
ethHw_dmaRx(void)
{
	uint8_t *buf = (uint8_t *) NetRxPackets[0];
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	dma_info_t *di = ch->di[0];
	void *bufp, *datap;
	int index;
	size_t rcvlen, buflen;
	uint32_t stat0, stat1;
	bool rxdata=false;
	int rc=0;
	uint32_t control, offset;
	uint8_t statbuf[HWRXOFF*2];

	//REG_DEBUG(1);
	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	udelay(50);

#if defined(CHK_ETH_ERRS)
	check_errs(eth_data->regs);
#endif

	while(1) {
		bufp = dma_getnextrxp(ch->di[0], &index, &buflen, &stat0, &stat1);
#if defined(CHK_ETH_ERRS)
		if (stat1&0xf0000000)
			printf("%s stat1 (0x%x)\n", __FUNCTION__, stat1);
#endif
		if (bufp) {
			ET_TRACE(("received packet\n"));
			ET_TRACE(("bufp(0x%x) index(0x%x) buflen(0x%x) stat0(0x%x) stat1(0x%x)\n", (uint32_t)bufp, index, buflen, stat0, stat1));


			// get buffer offset
			control = reg32_read(&di->d64rxregs->control);
			offset = (control&D64_RC_RO_MASK)>>D64_RC_RO_SHIFT;
			rcvlen = ltoh16(*(uint16 *)bufp);

			if ((rcvlen == 0) || (rcvlen > RX_BUF_SIZE)) {
				ET_ERROR(("Wrong RX packet size 0x%x drop it\n", rcvlen));
				/* refill buffre & descriptor */
				dma_rxrefill(ch->di[0], index);
				break;
			}

			ET_TRACE(("Received %d bytes\n", rcvlen));
			/* copy status into temp buf (need to copy all data out of buffer) */
			memcpy(statbuf, bufp, offset);
			datap = (void*)((uint32_t)bufp+offset);
			memcpy(buf, datap, rcvlen);

#ifdef BCMIPROC_ETH_DEBUG
			printf("Rx Buf: 0x%x\n", rcvlen);
			int i;
			for (i = 0; i < 0x40; i++) {
				printf("%02X ", buf[i]);
			}
			printf("\n");
#endif

#ifdef UBOOT_MDK
            /* Send to MDK handler instead */
            mdk_rcv_handler(buf, rcvlen);
#else
            /* A packet has been received, so forward to uboot network handler */
            NetReceive(buf, rcvlen);
#endif

			udelay(50);

			/* refill buffre & descriptor */
			dma_rxrefill(ch->di[0], index);
			rxdata = true;
			break;
		} else {
			if (!rxdata) {
				/* Tell caller that no packet was received when Rx queue was polled */
				rc = -1;
				break;
			}
			/* at leasted received one packet */
			break;
		}
	}
	//REG_DEBUG(0);

#ifdef BCMIPROC_ETH_DEBUG
	dmaRxDump(eth_data);
	gmacRegDump(eth_data);
	gmac_mibRxDump(eth_data);
#endif

	return rc;
}


int
ethHw_dmaAttach(bcm_eth_t *eth_data)
{
	bcmgmac_t *ch = &eth_data->bcmgmac;
	dma_info_t *di=NULL;
	char name[16];

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	/* allocate private info structure */
	if ((di = MALLOC(0, sizeof (dma_info_t))) == NULL) {
		ET_ERROR(("%s: out of memory, malloced %d bytes\n", __FUNCTION__, MALLOCED(0)));
		return -1;
	}

	BZERO_SM(di, sizeof(dma_info_t));

	di->dma64 = 1;
	di->d64txregs = (dma64regs_t *)&eth_data->regs->dma_regs[TX_Q0].dmaxmt;
	di->d64rxregs = (dma64regs_t *)&eth_data->regs->dma_regs[RX_Q0].dmarcv;
	/* Default flags: For backwards compatibility both Rx Overflow Continue
	 * and Parity are DISABLED.
	 * supports it.
	 */
	dma_ctrlflags(di, DMA_CTRL_ROC | DMA_CTRL_PEN, 0);

	di->rxburstlen = (reg32_read(&di->d64rxregs->control) & D64_RC_BL_MASK) >> D64_RC_BL_SHIFT;
	di->txburstlen = (reg32_read(&di->d64txregs->control) & D64_XC_BL_MASK) >> D64_XC_BL_SHIFT;
	ET_TRACE(("rx burst len 0x%x\n", di->rxburstlen));
	ET_TRACE(("tx burst len 0x%x\n", di->txburstlen));

	di->ntxd = NTXD;
	di->nrxd = NRXD;

	di->rxbufsize = RX_BUF_SIZE;

	di->nrxpost = NRXBUFPOST;
	di->rxoffset = HWRXOFF;

	sprintf(name, "et%d", eth_data->unit);
	strncpy(di->name, name, MAXNAMEL);

	/* load dma struc addr */
	ch->di[0] = di;

	return 0;
}

int
ethHw_dmaDetach(dma_info_t *di)
{

	ET_TRACE(("%s enter\n", __FUNCTION__));

	if ( di ) {
		/* free our private info structure */
		MFREE(0, (void *)di, sizeof(dma_info_t));
	}

	return 0;
}


int
ethHw_dmaDisable(int dir)
{
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	int stat;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	if (dir == DMA_TX)
		stat = dma_txreset(ch->di[0]);
	else
		stat = dma_rxreset(ch->di[0]);

	return stat;
}

int
ethHw_dmaEnable(int dir)
{
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __FUNCTION__));

	if (dir == DMA_TX)
		dma_txinit(ch->di[0]);
	else
		dma_rxinit(ch->di[0]);

	return 0;
}


int
ethHw_miiphy_read(unsigned int const phyaddr, 
   unsigned int const reg, unsigned short *const value)
{
	uint16 tmp16 = 0;
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
	printf("%s NOT IMPLEMENTED phyaddr(0x%x) reg(0x%x)\n", __FUNCTION__, phyaddr, reg);
#elif (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	uint32 addr, ext, bank, flags;

	printf("%s (addr&f):phyaddr (addr&f0):ext (addr&1f00):bank (addr&10000):flags\n", __FUNCTION__);
	printf("  ext:0(serdes)/1(ext PHY)\n");
	//printf("%s phyaddr(0x%x) reg(0x%x)\n", __FUNCTION__, phyaddr, reg);
	addr = phyaddr&0xf;
	ext = phyaddr&0xf0;
	bank = (phyaddr&0x1f00)>>8;
	flags = (phyaddr&0x10000)?SOC_PHY_REG_1000X:0;

	if (!ext) {
		/* internal serdes */
		tmp16 = serdes_rd_reg(&g_eth_data, addr, reg);
	} else {
		/* external phy */
		phy5461_rd_reg(&g_eth_data, addr, flags, bank, reg, &tmp16);
	}
	printf("%s phyaddr(0x%x) ext(0x%x) bank(0x%x) flags(0x%x) reg(0x%x) data(0x%x)\n",
			 __FUNCTION__, addr, ext, bank, flags, reg, tmp16);
#elif defined(CONFIG_HURRICANE2)
	uint32 addr, bank;

	printf("%s (addr&f):phyaddr (addr&1f00):bank\n", __FUNCTION__);
	//printf("%s phyaddr(0x%x) reg(0x%x)\n", __FUNCTION__, phyaddr, reg);
	addr = phyaddr&0xf;
	bank = (phyaddr&0x1f00)>>8;

	phy5221_rd_reg(&g_eth_data, addr, bank, reg, &tmp16);
	printf("%s phyaddr(0x%x) bank(0x%x) reg(0x%x) data(0x%x)\n", __FUNCTION__, addr, bank, reg, tmp16);
#endif
	*value = tmp16;
	return 0;
}


int
ethHw_miiphy_write(unsigned int const phyaddr, 
   unsigned int const reg, unsigned short *const value)
{
//	uint16 tmp16 = *value;
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
	printf("%s NOT IMPLEMENTED phyaddr(0x%x) reg(0x%x) *value(0x%x)\n", __FUNCTION__, phyaddr, reg, *value);
#elif (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	uint32 addr, ext, bank, flags;

	printf("%s (addr&f):phyaddr (addr&f0):ext (addr&1f00):bank (addr&10000):flags\n", __FUNCTION__);
	//printf("%s phyaddr(0x%x) reg(0x%x) *value(0x%x)\n", __FUNCTION__, phyaddr, reg, *value);
	addr = phyaddr&0xf;
	ext = phyaddr&0xf0;
	bank = (phyaddr&0x1f00)>>8;
	flags = (phyaddr&0x10000)?SOC_PHY_REG_1000X:0;

	printf("%s phyaddr(0x%x) ext(0x%x) bank(0x%x) flags(0x%x) reg(0x%x) data(0x%x)\n",
			 __FUNCTION__, addr, ext, bank, flags, reg, *value);
	if (!ext) {
		/* internal serdes */
		serdes_wr_reg(&g_eth_data, addr, reg, tmp16);
	} else {
		/* external phy */
		phy5461_wr_reg(&g_eth_data, addr, flags, bank, reg, &tmp16);
	}
#elif defined(CONFIG_HURRICANE2)
	uint32 addr, bank;

	printf("%s (addr&f):phyaddr (addr&1f00):bank\n", __FUNCTION__);
	//printf("%s phyaddr(0x%x) reg(0x%x) *value(0x%x)\n", __FUNCTION__, phyaddr, reg, *value);
	addr = phyaddr&0xf;
	bank = (phyaddr&0x1f00)>>8;

	printf("%s phyaddr(0x%x) bank(0x%x) reg(0x%x) data(0x%x)\n", __FUNCTION__, addr, bank, reg, *value);
	phy5221_wr_reg(&g_eth_data, addr, bank, reg, &tmp16);
#endif
	return 0;
}

int
ethHw_portLinkUp(void)
{
	int link=0;
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_HURRICANE2) || (defined(CONFIG_NORTHSTAR) && defined(CONFIG_APBOOT)))
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_HURRICANE2)) */

#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	phy5461_link_get(eth_data, ch->phyaddr, &link);
#elif defined(CONFIG_HURRICANE2)
	phy5221_link_get(eth_data, ch->phyaddr, &link);
#elif (defined(CONFIG_NORTHSTAR) && defined(CONFIG_APBOOT))
	link = miiphy_link("eth0", ch->phyaddr_port);
        if(!link) {
            printf("eth0: link down.\n");
        } else {
            printf("eth0: link up.\n");
        }

#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2)) */
	/* printf("%s link: %d\n", __FUNCTION__, link); */
	return link;
}

#ifdef CONFIG_APBOOT
static char *
speed_to_str(int speed)
{
    switch (speed) {
    case _1000BASET:
        return "1 Gb/s";
    case _100BASET:
        return "100 Mb/s";
    case _10BASET:
        return "10 Mb/s";
    default:
        return "unknown";
    }
}

static char *
duplex_to_str(int duplex)
{
    switch (duplex) {
    case 1:
        return "full";
    case 0:
        return "half";
    default:
        return "unknown";
    }
}
#endif

void
ethHw_checkPortSpeed(void)
{
	int speed=0, duplex=0, speedcfg;
	bcm_eth_t *eth_data = &g_eth_data;
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_HURRICANE2)) || (defined(CONFIG_NORTHSTAR) && defined(CONFIG_APBOOT))
	bcmgmac_t *ch = &eth_data->bcmgmac;
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2) || defined(CONFIG_HURRICANE2)) */
	static int orgspd, orgdpx;
#ifdef CONFIG_APBOOT
	int link;
	static int origlink;
#endif

#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	phy5461_speed_get(eth_data, ch->phyaddr, &speed, &duplex);
#elif defined(CONFIG_HURRICANE2)
	phy5221_speed_get(eth_data, ch->phyaddr, &speed, &duplex);
#elif defined(CONFIG_APBOOT)
	link = ethHw_portLinkUp();
	speed  = miiphy_speed("eth0", ch->phyaddr_port);
	duplex = (miiphy_duplex("eth0", ch->phyaddr_port) == FULL);

	if (link != origlink) {
		if (!link) {
			printf("eth%u: link down\n", eth_data->unit);
		}
		origlink = link;
	}

#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2)) */
	if (speed) {
		if (speed == 1000) {
			if (duplex)
				speedcfg = ET_1000FULL;
			else
				speedcfg = ET_1000HALF;
		}
		else if (speed == 100) {
			if (duplex)
				speedcfg = ET_100FULL;
			else
				speedcfg = ET_100HALF;
		}
		else if (speed == 10) {
			if (duplex)
				speedcfg = ET_10FULL;
			else
				speedcfg = ET_10HALF;
		}
		else {
			printf("ERROR: unknown speed %d\n", speed);
			return;
		}
		/* printf("%s setting speed: %d\n", __FUNCTION__, speedcfg); */
		gmac_speed(eth_data, speedcfg);
	}
	if ( orgspd!=speed || orgdpx!=duplex ) {
#ifndef CONFIG_APBOOT
		printf("ETH LINK UP: %d%s\n", speed, duplex?"FD":"HD");
#else
		printf("eth%u up: %s %s duplex\n",eth_data->unit, speed_to_str(speed), duplex_to_str(duplex));
#endif
		orgspd = speed;
		orgdpx = duplex;
	}
}


/* ==== Private Functions ================================================ */

#ifdef BCMIPROC_ETH_DEBUG
static void
txDump(uint8_t *buf, int len)
{
   uint8_t *bufp;
   int i;

   bufp = (uint8_t *) buf;

   printf("Tx Buf: 0x%x, %d\n", (uint32_t)bufp, len);
   for (i = 0; i < len; i++) {
      printf("%02X ", bufp[i]);
   }
   printf("\n");
}


static void
dmaTxDump(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	dma64dd_t *descp = NULL;
	uint8_t *bufp;
	int i;

	printf("TX DMA Register:\n");
	printf("control:0x%x; ptr:0x%x; addrl:0x%x; addrh:0x%x; stat0:0x%x, stat1:0x%x\n",
			reg32_read(&dma->d64txregs->control),
			reg32_read(&dma->d64txregs->ptr),
			reg32_read(&dma->d64txregs->addrlow),
			reg32_read(&dma->d64txregs->addrhigh),
			reg32_read(&dma->d64txregs->status0),
			reg32_read(&dma->d64txregs->status1));

	printf("TX Descriptors:\n");
	for (i = 0; i < TX_BUF_NUM; i++) {
		descp = (dma64dd_t *) TX_DESC(i);
		printf("ctrl1:0x%08x; ctrl2:0x%08x; addrhigh:0x%x; addrlow:0x%08x\n",
				descp->ctrl1, descp->ctrl2, descp->addrhigh, descp->addrlow);
	}

	printf("TX Buffers:\n");
	/* Initialize TX DMA descriptor table */
	for (i = 0; i < TX_BUF_NUM; i++) {
		bufp = (uint8_t *) TX_BUF(i);
		printf("buf%d:0x%x; ", i, (uint32_t)bufp);
	}
	printf("\n");
}


static void
dmaRxDump(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	dma64dd_t *descp = NULL;
	uint8_t *bufp;
	int i;

	printf("RX DMA Register:\n");
	printf("control:0x%x; ptr:0x%x; addrl:0x%x; addrh:0x%x; stat0:0x%x, stat1:0x%x\n",
			reg32_read(&dma->d64rxregs->control),
			reg32_read(&dma->d64rxregs->ptr),
			reg32_read(&dma->d64rxregs->addrlow),
			reg32_read(&dma->d64rxregs->addrhigh),
			reg32_read(&dma->d64rxregs->status0),
			reg32_read(&dma->d64rxregs->status1));

	printf("RX Descriptors:\n");
	for (i = 0; i < RX_BUF_NUM; i++) {
		descp = (dma64dd_t *) RX_DESC(i);
		printf("ctrl1:0x%08x; ctrl2:0x%08x; addrhigh:0x%x; addrlow:0x%08x\n",
				descp->ctrl1, descp->ctrl2, descp->addrhigh, descp->addrlow);
	}

	printf("RX Buffers:\n");
	for (i = 0; i < RX_BUF_NUM; i++) {
		bufp = (uint8_t *) RX_BUF(i);
		printf("buf%d:0x%x; ", i, (uint32_t)bufp);
	}
	printf("\n");
}


static void
gmacRegDump(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	printf("GMAC Registers:\n");
	printf("dev_ctl:0x%x; dev_status:0x%x; int_status:0x%x; int_mask:0x%x; int_recv_lazy:0x%x\n",
			reg32_read(&regs->dev_ctl),
			reg32_read(&regs->dev_status),
			reg32_read(&regs->int_status),
			reg32_read(&regs->int_mask),
			reg32_read(&regs->int_recv_lazy));
	printf("UNIMAC Registers:\n");
	printf("cmd_cfg:0x%x; mac_h:0x%x; mac_l:0x%x; rx_max_len:0x%x\n",
			reg32_read(&regs->cmd_cfg),
			reg32_read(&regs->mac_addr_high),
			reg32_read(&regs->mac_addr_low),
			reg32_read(&regs->rx_max_length));
}

static void
gmac_mibTxDump(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	volatile uint32_t *ptr;
	uint32_t tmp;

	printf("GMAC TX MIB:\n");

	for (ptr = &regs->mib.tx_good_octets; ptr <= &regs->mib.tx_q3_octets_high; ptr++) {
		tmp = reg32_read(ptr);
		printf("0x%x:0x%x; ", (uint32_t)ptr, tmp);
	}
	printf("\n");

	return;
}


static void
gmac_mibRxDump(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	volatile uint32_t *ptr;
	uint32_t tmp;

	printf("GMAC RX MIB:\n");

	for (ptr = &regs->mib.rx_good_octets; ptr <= &regs->mib.rx_uni_pkts; ptr++) {
		tmp = reg32_read(ptr);
		printf("0x%x:0x%x; ", (uint32_t)ptr, tmp);
	}
	printf("\n");

	return;
}
#endif


static uint
dma_ctrlflags(dma_info_t *di, uint mask, uint flags)
{
	uint dmactrlflags;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	dmactrlflags = di->hnddma.dmactrlflags;
	ASSERT((flags & ~mask) == 0);

	dmactrlflags &= ~mask;
	dmactrlflags |= flags;

	/* If trying to enable parity, check if parity is actually supported */
	if (dmactrlflags & DMA_CTRL_PEN) {
		uint32 control;

		control = reg32_read(&di->d64txregs->control);
		reg32_write(&di->d64txregs->control, control | D64_XC_PD);
		if (reg32_read(&di->d64txregs->control) & D64_XC_PD) {
			/* We *can* disable it so it is supported,
			 * restore control register
			 */
			reg32_write(&di->d64txregs->control, control);
		} else {
			/* Not supported, don't allow it to be enabled */
			dmactrlflags &= ~DMA_CTRL_PEN;
		}
	}

	di->hnddma.dmactrlflags = dmactrlflags;

	return (dmactrlflags);
}


static int
dma_rxenable(dma_info_t *di)
{
	uint32_t dmactrlflags = di->hnddma.dmactrlflags;
	uint32_t rxoffset = di->rxoffset;
	uint32_t rxburstlen = di->rxburstlen;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	uint32 control = (reg32_read(&di->d64rxregs->control) & D64_RC_AE) | D64_RC_RE;

	if ((dmactrlflags & DMA_CTRL_PEN) == 0)
		control |= D64_RC_PD;

	if (dmactrlflags & DMA_CTRL_ROC)
		control |= D64_RC_OC;

	/* These bits 20:18 (burstLen) of control register can be written but will take
	 * effect only if these bits are valid. So this will not affect previous versions
	 * of the DMA. They will continue to have those bits set to 0.
	 */
	control &= ~D64_RC_BL_MASK;
	control |= (rxburstlen << D64_RC_BL_SHIFT);
	control |= (rxoffset << D64_RC_RO_SHIFT);

	reg32_write(&di->d64rxregs->control, control);
	return 0;
}


static void
dma_txinit(dma_info_t *di)
{
	uint32 control;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	if (di->ntxd == 0)
		return;

	di->txin = di->txout = 0;
	di->hnddma.txavail = di->ntxd - 1;

	/* These bits 20:18 (burstLen) of control register can be written but will take
	 * effect only if these bits are valid. So this will not affect previous versions
	 * of the DMA. They will continue to have those bits set to 0.
	 */
	control = reg32_read(&di->d64txregs->control);

	control |= D64_XC_XE;
	if ((di->hnddma.dmactrlflags & DMA_CTRL_PEN) == 0)
		control |= D64_XC_PD;

	reg32_write(&di->d64txregs->control, control);

	/* initailize the DMA channel */
	reg32_write(&di->d64txregs->addrlow, TX_DESC_BASE);
	reg32_write(&di->d64txregs->addrhigh, 0);
}


static void
dma_rxinit(dma_info_t *di)
{
	ET_TRACE(("%s enter\n", __FUNCTION__));

	di->rxin = di->rxout = 0;

	dma_rxenable(di);

	/* the rx descriptor ring should have the addresses set properly */
	/* set the lastdscr for the rx ring */
	reg32_write(&di->d64rxregs->ptr, ((uint32_t)RX_DESC(RX_BUF_NUM)&D64_XP_LD_MASK));

}


static bool
dma_txreset(dma_info_t *di)
{
	uint32 status;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	if (di->ntxd == 0)
		return TRUE;

	/* address PR8249/PR7577 issue */
	/* suspend tx DMA first */
	reg32_write(&di->d64txregs->control, D64_XC_SE);
	SPINWAIT(((status = (reg32_read(&di->d64txregs->status0) & D64_XS0_XS_MASK)) !=
	          D64_XS0_XS_DISABLED) &&
	         (status != D64_XS0_XS_IDLE) &&
	         (status != D64_XS0_XS_STOPPED),
	         10000);

	/* PR2414 WAR: DMA engines are not disabled until transfer finishes */
	reg32_write(&di->d64txregs->control, 0);
	SPINWAIT(((status = (reg32_read(&di->d64txregs->status0) & D64_XS0_XS_MASK)) !=
	          D64_XS0_XS_DISABLED),
	         10000);

	/* wait for the last transaction to complete */
	udelay(300);

	return (status == D64_XS0_XS_DISABLED);
}


static bool
dma_rxreset(dma_info_t *di)
{
	uint32 status;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	if (di->nrxd == 0)
		return TRUE;

	/* PR2414 WAR: DMA engines are not disabled until transfer finishes */
	reg32_write(&di->d64rxregs->control, 0);
	SPINWAIT(((status = (reg32_read(&di->d64rxregs->status0) & D64_RS0_RS_MASK)) !=
	          D64_RS0_RS_DISABLED), 10000);

	return (status == D64_RS0_RS_DISABLED);
}


static int
dma_txload(int index, size_t len, uint8_t * tx_buf)
{
#ifdef BCMIPROC_ETH_DEBUG
	int tlen = len;
#endif

	ET_TRACE(("%s enter\n", __FUNCTION__));

#ifdef BCMIPROC_ETH_DEBUG
	printf("TX IN buf:0x%x; len:%d\n", (uint32_t)tx_buf, len);
	if (tlen>64)
		tlen=64;
	txDump(tx_buf, tlen);
#endif

	/* copy buffer */
	memcpy((uint8_t *) TX_BUF(index), tx_buf, len);

#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
	/* The Ethernet packet has to be >= 64 bytes required by switch 
	* padding it with zeros
	*/
#ifdef CONFIG_APBOOT /* GMAC will add FCS, the min len should be 60 here */
	if (len < 60) {
		memset((uint8_t *) TX_BUF(index) + len, 0, 60 - len);
		len = 60;
	}
#else
	if (len < 64) {
		memset((uint8_t *) TX_BUF(index) + len, 0, 64 - len);
		len = 64;
	}
#endif
#endif

#ifdef UBOOT_MDK
        /* MDK tx size already included FCS  */
#else
	/* Add 4 bytes for Ethernet FCS/CRC */
#ifndef CONFIG_APBOOT /* GMAC will add FCS */
	len += 4;
#endif
#endif

	//Flush data, config, and descriptors to external memory
	TX_FLUSH_CACHE();

#ifdef BCMIPROC_ETH_DEBUG
	printf("TX DMA buf:0x%x; len:%d\n", (uint32_t)TX_BUF(index), len);
	txDump((uint8_t *)TX_BUF(index), tlen);
#endif

	return len;
}


/* this api will check if a packet has been received.  If so it will return the
   address of the buffer and enter a bogus address into the descriptor table
   rxin will be incremented to the next descriptor.
   Once done with the frame the buffer should be added back onto the descriptor
   and the lastdscr should be updated to this descriptor. */
static void *
dma_getnextrxp(dma_info_t *di, int *index, size_t *len, uint32_t *stat0, uint32_t *stat1)
{
	dma64dd_t *descp = NULL;
	uint i, curr, active;
	void *rxp;

	/* initialize return parameters */
	*index = 0;
	*len = 0;
	*stat0 = 0;
	*stat1 = 0;

	i = di->rxin;

	curr = B2I(((reg32_read(&di->d64rxregs->status0) & D64_RS0_CD_MASK) -
		di->rcvptrbase) & D64_RS0_CD_MASK, dma64dd_t);
	active = B2I(((reg32_read(&di->d64rxregs->status1) & D64_RS0_CD_MASK) -
		di->rcvptrbase) & D64_RS0_CD_MASK, dma64dd_t);

	/* check if any frame */
	if (i == curr)
		return (NULL);

	ET_TRACE(("rxin(0x%x) curr(0x%x) active(0x%x)\n", i, curr, active));
	/* remove warning */
	if (i == active);

	/* get the packet pointer that corresponds to the rx descriptor */
	rxp = (void*)RX_BUF(i);

	descp = (dma64dd_t *)RX_DESC(i);
	/* invalidate buffer */
	//gmac_cache_inval((u32)rxp, RX_BUF_LEN);
	//gmac_cache_inval((u32)descp, sizeof(dma64dd_t));

	descp->addrlow = 0xdeadbeef;
	descp->addrhigh = 0xdeadbeef;

	*index = i;
	*len = (descp->ctrl2&D64_CTRL2_BC_MASK);
	*stat0 = reg32_read(&di->d64rxregs->status0);
	*stat1 = reg32_read(&di->d64rxregs->status1);

	di->rxin = NEXTRXD(i);

	return (rxp);
}


/* Restore the buffer back on to the descriptor and
   then lastdscr should be updated to this descriptor. */
static void
dma_rxrefill(dma_info_t *di, int index)
{
	dma64dd_t *descp = NULL;
	void *bufp;

	/* get the packet pointer that corresponds to the rx descriptor */
	bufp = (void*)RX_BUF(index);
	descp = (dma64dd_t *)RX_DESC(index);

	/* update descriptor that is being added back on ring */
	descp->ctrl2 = RX_BUF_SIZE;
	descp->addrlow = (uint32_t)bufp;
	descp->addrhigh = 0;
	/* flush descriptor */
	//gmac_cache_flush((u32)descp, sizeof(dma64dd_t));
}


/* reset all 4 GMAC Cores */
static void
gmac_core_reset(void)
{
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
	reg32_write((uint32_t*)(AMAC_IDM0_IDM_RESET_CONTROL),0);
	reg32_write((uint32_t*)(AMAC_IDM1_IDM_RESET_CONTROL),0);
	reg32_write((uint32_t*)(AMAC_IDM2_IDM_RESET_CONTROL),0);
	reg32_write((uint32_t*)(AMAC_IDM3_IDM_RESET_CONTROL),0);
#endif
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))

	uint32 tmp;
	/* write 0 to core reset control reg */
	/* write command config reg */
	//tmp = reg32_read((uint32_t*)(AMAC_IDM0_IDM_RESET_CONTROL));
	//printf("%s read 0x%x from AMAC_IDM0_IDM_RESET_CONTROL\n", __FUNCTION__, tmp);
	//printf("%s write 0 to AMAC_IDM0_IDM_RESET_CONTROL\n", __FUNCTION__);
	reg32_write((uint32_t*)(AMAC_IDM0_IDM_RESET_CONTROL),0);
	//tmp = reg32_read((uint32_t*)(AMAC_IDM1_IDM_RESET_CONTROL));
	//printf("%s read 0x%x from AMAC_IDM1_IDM_RESET_CONTROL\n", __FUNCTION__, tmp);
	//printf("%s write 0 to AMAC_IDM1_IDM_RESET_CONTROL\n", __FUNCTION__);
	reg32_write((uint32_t*)(AMAC_IDM1_IDM_RESET_CONTROL),0);
	tmp = reg32_read((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT));
	//printf("%s read 0x%x from AMAC_IDM0_IO_CONTROL_DIRECT\n", __FUNCTION__, tmp);
	tmp &= ~(1<<AMAC_IDM0_IO_CONTROL_DIRECT__CLK_250_SEL);
	tmp |= (1<<AMAC_IDM0_IO_CONTROL_DIRECT__DIRECT_GMII_MODE);
	tmp |= (1<<AMAC_IDM0_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	//printf("%s write 0x%x to AMAC_IDM0_IO_CONTROL_DIRECT\n", __FUNCTION__, tmp);
	reg32_write((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT),tmp);
	tmp = reg32_read((uint32_t*)(AMAC_IDM1_IO_CONTROL_DIRECT));
	//printf("%s read 0x%x from AMAC_IDM1_IO_CONTROL_DIRECT\n", __FUNCTION__, tmp);
	tmp &= ~(1<<AMAC_IDM1_IO_CONTROL_DIRECT__CLK_250_SEL);
	tmp |= (1<<AMAC_IDM1_IO_CONTROL_DIRECT__DIRECT_GMII_MODE);
	tmp |= (1<<AMAC_IDM1_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	//printf("%s write 0x%x to AMAC_IDM1_IO_CONTROL_DIRECT\n", __FUNCTION__, tmp);
	reg32_write((uint32_t*)(AMAC_IDM1_IO_CONTROL_DIRECT),tmp);
#endif /*(defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))*/
#if defined(CONFIG_HURRICANE2)
	uint32 tmp;
	/* write 0 to core reset control reg */
	/* write command config reg */
	reg32_write((uint32_t*)(AMAC_IDM0_IDM_RESET_CONTROL),0);
	tmp = reg32_read((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT));
	tmp &= ~(1<<AMAC_IDM0_IO_CONTROL_DIRECT__CLK_250_SEL);
	tmp |= (1<<AMAC_IDM0_IO_CONTROL_DIRECT__DIRECT_GMII_MODE);
	tmp |= (1<<AMAC_IDM0_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	reg32_write((uint32_t*)(AMAC_IDM0_IO_CONTROL_DIRECT),tmp);
#endif
}


static void
gmac_init_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* set command config reg CC_SR */
	reg32_set_bits( &regs->cmd_cfg, CC_SR );
	udelay(GMAC_RESET_DELAY);
}


static void
gmac_clear_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* clear command config reg CC_SR */
	reg32_clear_bits(&regs->cmd_cfg, CC_SR);
	udelay(GMAC_RESET_DELAY);
}


static void
gmac_loopback(bcm_eth_t *eth_data, bool on)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t ocmdcfg, cmdcfg;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* set/clear the mac loopback mode */
	if (on)
		cmdcfg |= CC_ML;
	else
		cmdcfg &= ~CC_ML;

	/* check if need to write register back */
	if (cmdcfg != ocmdcfg) {
		/* put mac in reset */
		gmac_init_reset(eth_data);
		/* write register */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}


static void
gmac_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t ocmdcfg, cmdcfg;

 	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	cmdcfg &= ~(CC_TE | CC_RE | CC_RPI | CC_TAI | CC_HD | CC_ML |
			CC_CFE | CC_RL | CC_RED | CC_PE | CC_TPI | CC_PAD_EN | CC_PF);
	cmdcfg |= (CC_PROM | CC_NLC | CC_CFE);

	/* check if need to write register back */
	if (cmdcfg != ocmdcfg) {
		/* put mac in reset */
		gmac_init_reset(eth_data);
		/* write register */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}


static void
gmac_clearmib(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	volatile uint32_t *ptr;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* enable clear on read */
	reg32_set_bits( &regs->dev_ctl, DC_MROR );

	for (ptr = &regs->mib.tx_good_octets; ptr <= &regs->mib.rx_uni_pkts; ptr++) {
		reg32_read(ptr);
		if (ptr == &regs->mib.tx_q3_octets_high)
			ptr++;
	}

	return;
}


static int
gmac_speed(bcm_eth_t *eth_data, uint32_t speed)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t cmdcfg, ocmdcfg;
	uint32_t hd_ena = 0;

	ET_TRACE(("%s enter\n", __FUNCTION__));
        
#ifdef CONFIG_APBOOT
/* 
 * bug where if we set the speed in mac to 100 or 10 
 * cant send any traffic so lets keep the speed as
 * always 1G irrespective of speed on phy.
 */
	speed = ET_1000FULL;
#endif
	switch (speed) {
	case ET_10HALF:
		hd_ena = CC_HD;
		/* FALLTHRU */

	case ET_10FULL:
		speed = 0;
		break;

	case ET_100HALF:
		hd_ena = CC_HD;
		/* FALLTHRU */

	case ET_100FULL:
		speed = 1;
		break;

	case ET_1000FULL:
		speed = 2;
		break;

	case ET_1000HALF:
		ET_ERROR(("et%d: gmac_speed: supports 1000 mbps full duplex only\n",
				eth_data->unit));
		return (FAILURE);

	default:
		ET_ERROR(("et%d: gmac_speed: speed %d not supported\n",
				eth_data->unit, speed));
		return (FAILURE);
	}

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* set the speed */
	cmdcfg &= ~(CC_ES_MASK | CC_HD);
	cmdcfg |= ((speed << CC_ES_SHIFT) | hd_ena);
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	cmdcfg |= CC_AE;
#endif

	if (cmdcfg!=ocmdcfg) {
		/* put mac in reset */
		gmac_init_reset(eth_data);
		/* write command config reg */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* bring mac out of reset */
		gmac_clear_reset(eth_data);
	}

	return (SUCCESS);
}


static void
gmac_tx_flowcontrol(bcm_eth_t *eth_data, bool on)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t cmdcfg, ocmdcfg;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* to enable tx flow control clear the rx pause ignore bit */
	if (on)
		cmdcfg &= ~CC_RPI;
	else
		cmdcfg |= CC_RPI;

	if (cmdcfg!=ocmdcfg) {
		/* put the mac in reset */
		gmac_init_reset(eth_data);
		/* write command config reg */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}


static void
gmac_promisc(bcm_eth_t *eth_data, bool mode)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t cmdcfg, ocmdcfg;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* enable or disable promiscuous mode */
	if (mode)
		cmdcfg |= CC_PROM;
	else
		cmdcfg &= ~CC_PROM;

	if (cmdcfg!=ocmdcfg) {
		/* put the mac in reset */
		gmac_init_reset(eth_data);
		/* write command config reg */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}


static void
gmac_enable(bcm_eth_t *eth_data, bool en)
{
	gmacregs_t *regs = eth_data->regs;
	uint32_t cmdcfg;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);

	/* put mac in reset */
	gmac_init_reset(eth_data);

	cmdcfg |= CC_SR;

	/* first deassert rx_ena and tx_ena while in reset */
	cmdcfg &= ~(CC_RE | CC_TE);
	/* write command config reg */
	reg32_write(&regs->cmd_cfg, cmdcfg);

	/* bring mac out of reset */
	gmac_clear_reset(eth_data);

	/* if not enable exit now */
	if (!en)
		return;

	/* enable the mac transmit and receive paths now */
	udelay(2);
	cmdcfg &= ~CC_SR;
	cmdcfg |= (CC_RE | CC_TE);

	/* assert rx_ena and tx_ena when out of reset to enable the mac */
	reg32_write(&regs->cmd_cfg, cmdcfg);

	return;
}


#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
#if 0
static void
gmac_serdes_init(bcm_eth_t *eth_data)
{
	/* bring serdes out of reset */
	gmacregs_t *regs = eth_data->regs;
	uint32_t sdctl, sdstat0, sdstat1;

	ET_TRACE(("%s enter\n", __FUNCTION__));
	//printf("et%d: %s enter\n", eth_data->unit, __FUNCTION__);

	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdstat0 = reg32_read(&regs->serdes_status0);
	sdstat1 = reg32_read(&regs->serdes_status1);
	//printf("et%d: %s read sdstat0(0x%x); sdstat1(0x%x)\n", eth_data->unit, __FUNCTION__, sdstat0, sdstat1);

    /*
     * Bring up both digital and analog clocks
     *
     * NOTE: Many MAC registers are not accessible until the PLL is locked.
     * An S-Channel timeout will occur before that.
     */

	/* read serdes control reg */
	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl &= ~(SC_PWR_DOWN_MASK|SC_IDDQ_MASK);
	sdctl |= (SC_LCREF_EN_MASK|SC_RSTB_HW_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl &= ~(SC_RSTB_HW_MASK|SC_TX1G_FIFO_RST_MASK);
	sdctl |= (SC_IDDQ_MASK|SC_PWR_DOWN_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

	/* set reference clock, divisor & termination */
	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl &= ~(SC_REFSEL_MASK|SC_REFDIV_MASK);
	sdctl |= (SC_REF_TERM_SEL_MASK|SC_REFSEL_VAL|SC_REFDIV_VAL);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

	/* remove power down */
	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl &= ~(SC_PWR_DOWN_MASK|SC_IDDQ_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

    /* Bring hardware out of reset */
	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl |= (SC_RSTB_HW_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

    /* Bring MDIOREGS out of reset */
	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl |= (SC_RSTB_MDIOREGS_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

    /* Bring PLL out of reset */
	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl |= (SC_RSTB_PLL_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

    /* Bring Tx FIFO out of reset */
	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	/* set fifo to f than 1 */
	sdctl |= (SC_TX1G_FIFO_RST_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdstat0 = reg32_read(&regs->serdes_status0);
	sdstat1 = reg32_read(&regs->serdes_status1);
	//printf("et%d: %s read sdstat0(0x%x); sdstat1(0x%x)\n", eth_data->unit, __FUNCTION__, sdstat0, sdstat1);

	return;
}
#endif /*(defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))*/
static void
gmac_serdes_init(bcm_eth_t *eth_data)
{
	/* bring serdes out of reset */
	gmacregs_t *regs =	eth_data->regs;
	uint32_t sdctl, sdstat0, sdstat1;

	ET_TRACE(("%s enter\n", __FUNCTION__));
	//printf("et%d: %s enter\n", eth_data->unit, __FUNCTION__);

	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdstat0 = reg32_read(&regs->serdes_status0);
	sdstat1 = reg32_read(&regs->serdes_status1);
	//printf("et%d: %s read sdstat0(0x%x); sdstat1(0x%x)\n", eth_data->unit, __FUNCTION__, sdstat0, sdstat1);

    /*
     * Bring up both digital and analog clocks
     *
     * NOTE: Many MAC registers are not accessible until the PLL is locked.
     * An S-Channel timeout will occur before that.
     */

	sdctl = 0;
	sdctl |= (SC_TX1G_FIFO_RST_VAL|SC_FORCE_SPD_STRAP_VAL|SC_REFSEL_VAL);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl |= (SC_IDDQ_MASK|SC_PWR_DOWN_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl &= ~(SC_IDDQ_MASK|SC_PWR_DOWN_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

    /* Bring hardware out of reset */
	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl |= (SC_RSTB_HW_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

    /* Bring MDIOREGS out of reset */
	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl |= (SC_RSTB_MDIOREGS_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

    /* Bring PLL out of reset */
	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdctl |= (SC_RSTB_PLL_MASK);
	//printf("et%d: %s write sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	reg32_write(&regs->serdes_ctl, sdctl);

	udelay(1000);

	sdctl = reg32_read(&regs->serdes_ctl);
	//printf("et%d: %s read sdctl(0x%x)\n", eth_data->unit, __FUNCTION__, sdctl);
	sdstat0 = reg32_read(&regs->serdes_status0);
	sdstat1 = reg32_read(&regs->serdes_status1);
	//printf("et%d: %s read sdstat0(0x%x); sdstat1(0x%x)\n", eth_data->unit, __FUNCTION__, sdstat0, sdstat1);

	return;
}
#endif


void
chip_phy_wr(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg, uint16_t v)
{
	uint32_t tmp;
	volatile uint32_t *phy_ctrl = (uint32_t *)ChipcommonB_MII_Management_Control;
	volatile uint32_t *phy_data = (uint32_t *)ChipcommonB_MII_Management_Command_Data;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	ASSERT(phyaddr < MAXEPHY);
	ASSERT(reg < MAXPHYREG);

	/* wait until Mii mgt interface not busy */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("et%d: %s: busy\n", eth_data->unit, __FUNCTION__));
	}

	/* set preamble and MDCDIV */
	tmp = 	(1<<ChipcommonB_MII_Management_Control__PRE) |			/* PRE */
			0x1a;													/* MDCDIV */
	/* set int/ext phy */
	if (ext) {
		/* ext phy */
		tmp |= (1<<ChipcommonB_MII_Management_Control__EXT);
	}
	reg32_write(phy_ctrl, tmp);
	//printf("%s wrt phy_ctrl: 0x%x\n", __FUNCTION__, tmp);

	/* wait for it to complete */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("et%d: %s ChipcommonB_MII_Management_Control did not complete\n", eth_data->unit, __FUNCTION__));
	}

	/* issue the write */
	/* set start bit, write op, phy addr, phy reg & data */
	tmp = 	(1 << ChipcommonB_MII_Management_Command_Data__SB_R) |			/* SB */
			(1 << ChipcommonB_MII_Management_Command_Data__OP_R) |			/* OP - wrt */
			(phyaddr << ChipcommonB_MII_Management_Command_Data__PA_R) |	/* PA */
			(reg << ChipcommonB_MII_Management_Command_Data__RA_R) |		/* RA */
			(2 << ChipcommonB_MII_Management_Command_Data__TA_R) |			/* TA */
			v;																/* Data */
	//printf("%s wrt phy_data: 0x%x phyaddr(0x%x) reg(0x%x) val(0x%x)\n",
	//		__FUNCTION__, tmp, phyaddr, reg, v);
	reg32_write(phy_data, tmp);

	/* wait for it to complete */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("et%d: %s ChipcommonB_MII_Management_Command_Data did not complete\n", eth_data->unit, __FUNCTION__));
	}
}


uint16_t
chip_phy_rd(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg)
{
	uint32_t tmp;
	volatile uint32_t *phy_ctrl = (uint32_t *)ChipcommonB_MII_Management_Control;
	volatile uint32_t *phy_data = (uint32_t *)ChipcommonB_MII_Management_Command_Data;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	ASSERT(phyaddr < MAXEPHY);
	ASSERT(reg < MAXPHYREG);

	/* wait until Mii mgt interface not busy */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("et%d: %s: busy\n", eth_data->unit, __FUNCTION__));
	}

	/* set preamble and MDCDIV */
	tmp = 	(1<<ChipcommonB_MII_Management_Control__PRE) |			/* PRE */
			0x1a;													/* MDCDIV */
	/* set int/ext phy */
	if (ext) {
		/* ext phy */
		tmp |= (1<<ChipcommonB_MII_Management_Control__EXT);
	}
	reg32_write(phy_ctrl, tmp);
	//printf("%s wrt phy_ctrl: 0x%x\n", __FUNCTION__, tmp);

	/* wait for it to complete */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("et%d: %s ChipcommonB_MII_Management_Control did not complete\n", eth_data->unit, __FUNCTION__));
	}

	/* issue the read */
	/* set start bit, write op, phy addr, phy reg & data */
	tmp = 	(1 << ChipcommonB_MII_Management_Command_Data__SB_R) |			/* SB */
			(2 << ChipcommonB_MII_Management_Command_Data__OP_R) |			/* OP - rd*/
			(phyaddr << ChipcommonB_MII_Management_Command_Data__PA_R) |	/* PA */
			(reg << ChipcommonB_MII_Management_Command_Data__RA_R) |		/* RA */
			(2 << ChipcommonB_MII_Management_Command_Data__TA_R);			/* TA */
	//printf("%s wrt phy_data:0x%x phyaddr(0x%x) reg(0x%x)\n",
	//		__FUNCTION__, tmp, phyaddr, reg);
	reg32_write(phy_data, tmp);

	/* wait for it to complete */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		ET_ERROR(("et%d: %s ChipcommonB_MII_Management_Command_Data did not complete\n", eth_data->unit, __FUNCTION__));
	}

	/* read data */
	tmp = reg32_read(phy_data);
	//printf("%s rd phyaddr(0x%x) reg(0x%x) data: 0x%x\n", __FUNCTION__, phyaddr, reg, tmp);

	return (tmp & 0xffff);
}


static void
chip_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	int speed = ET_1000FULL;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* reset the tx dma engines */
	dma_txreset(ch->di[TX_Q0]);

	/* set eth_data into loopback mode to ensure no rx traffic */
	gmac_loopback(eth_data, TRUE);
	udelay(1);

	/* reset the rx dma engine */
	dma_rxreset(ch->di[RX_Q0]);

	/* reset gmac */
	gmac_reset(eth_data);

	/* clear mib */
	gmac_clearmib(eth_data);

	/* set smi_master to drive mdc_clk */
	reg32_set_bits(&eth_data->regs->phy_ctl, PC_MTE);

	/* set gmac speed */
#if defined(CONFIG_HURRICANE2)
	speed = ET_100FULL;
#endif
	gmac_speed(eth_data, speed);

	/* clear persistent sw intstatus */
	reg32_write(&regs->int_status, 0);
}


/*
 * Initialize all the chip registers.  If dma mode, init tx and rx dma engines
 * but leave the devcontrol tx and rx (fifos) disabled.
 */
static void
chip_init(bcm_eth_t *eth_data, uint options)
{
	gmacregs_t *regs = eth_data->regs;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	int speed = ET_1000FULL;

	ET_TRACE(("%s enter\n", __FUNCTION__));

	/* enable one rx interrupt per received frame */
	reg32_write(&regs->int_recv_lazy, (1 << IRL_FC_SHIFT));

	/* enable 802.3x tx flow control (honor received PAUSE frames) */
	gmac_tx_flowcontrol(eth_data, TRUE);

	/* disable promiscuous mode */
	gmac_promisc(eth_data, TRUE);

	/* set our local address */
	ET_TRACE(("\nMAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
			eth_data->enetaddr[0], eth_data->enetaddr[1], eth_data->enetaddr[2],
			eth_data->enetaddr[3], eth_data->enetaddr[4], eth_data->enetaddr[5]));
	reg32_write(&regs->mac_addr_high, htonl(*(uint32_t *)&eth_data->enetaddr[0]));
	reg32_write(&regs->mac_addr_low,  htons(*(uint32_t *)&eth_data->enetaddr[4]));

	/* optionally enable mac-level loopback */
	gmac_loopback(eth_data, eth_data->loopback);

	/* set max frame lengths - account for possible vlan tag */
	reg32_write(&regs->rx_max_length, PKTSIZE + 32);

	/* set phy speed/duplex */
#if defined(CONFIG_HURRICANE2)
	speed = ET_100FULL;
#endif
	gmac_speed(eth_data, speed);

	/* enable the overflow continue feature and disable parity */
	dma_ctrlflags(ch->di[0], DMA_CTRL_ROC | DMA_CTRL_PEN /* mask */,
	              DMA_CTRL_ROC /* value */);
}


/* get current and pending interrupt events */
static uint32_t
chip_getintr_events(bcm_eth_t *eth_data, bool in_isr)
{
	gmacregs_t *regs = eth_data->regs;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	uint32_t intstatus;

	/* read the interrupt status register */
	intstatus = reg32_read(&regs->int_status);
	/* clear the int bits */
	reg32_write(&regs->int_status, intstatus);
	reg32_read(&regs->int_status);

	/* defer unsolicited interrupts */
	intstatus &= (in_isr ? ch->intmask : ch->def_intmask);

	/* or new bits into persistent intstatus */
	intstatus = (ch->intstatus |= intstatus);

	/* return intstatus */
	return intstatus;
}
#endif
