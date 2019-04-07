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
*  @file    bcmiproc_serdes.c
*
*  @brief   serdes routines
*
*  @note
*
*   These routines provide access to the serdes
*/
/****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */
#include <config.h>
#ifdef CONFIG_IPROC
#include "bcmiproc_serdes.h"
#include "bcmiproc_serdes_def.h"

/* ---- External Variable Declarations ----------------------------------- */
/* ---- External Function Prototypes ------------------------------------- */
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */

/* debug/trace */
//#define BCMDBG
#define BCMDBG_ERR
#ifdef BCMDBG
#define	NET_ERROR(args) printf args
#define	NET_TRACE(args) printf args
#elif defined(BCMDBG_ERR)
#define	NET_ERROR(args) printf args
#define NET_TRACE(args)
#else
#define	NET_ERROR(args)
#define	NET_TRACE(args)
#endif /* BCMDBG */
#define	NET_REG_TRACE(args)


#ifndef ASSERT
#define ASSERT(exp)
#endif

#define	SERDES_INTERNAL	0

extern void chip_phy_wr(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg, uint16_t v);
extern uint16_t chip_phy_rd(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg);


/* ==== Public Functions ================================================= */

void
serdes_set_blk(bcm_eth_t *eth_data, uint phyaddr, uint blk)
{
	uint blkaddr;
	uint destblk = blk;

	NET_TRACE(("%s enter\n", __FUNCTION__));

	NET_REG_TRACE(("%s phyaddr(0x%x) blk(0x%x)\n",
		 __FUNCTION__, phyaddr, blk));

	/* check if need to update blk addr */
	blkaddr = chip_phy_rd(eth_data, SERDES_INTERNAL, phyaddr, PHY_REG_BLK_ADDR) & 0xfff0;
	if (blkaddr!=destblk) {
		/* write block address */
		NET_REG_TRACE(("%s write block address (0x%x)\n",
			 __FUNCTION__, destblk));
		chip_phy_wr(eth_data, SERDES_INTERNAL, phyaddr, PHY_REG_BLK_ADDR, destblk);
		NET_REG_TRACE(("%s write block address (0x%x) check\n",
			 __FUNCTION__, destblk));
		chip_phy_rd(eth_data, SERDES_INTERNAL, phyaddr, PHY_REG_BLK_ADDR);
	}
}


void
serdes_wr_reg(bcm_eth_t *eth_data, uint phyaddr, uint reg, uint data)
{
	uint blk = reg&0x7ff0;
	uint off = reg&0x000f;

	NET_TRACE(("%s enter\n", __FUNCTION__));

	if (reg&0x8000)
		off|=0x10;

	NET_REG_TRACE(("%s phyaddr(0x%x) reg(0x%x) data(0x%x)\n",
		 __FUNCTION__, phyaddr, reg, data));
	//printf("%s write(0x%x) to phyaddr(0x%x) reg(0x%x)\n",
	//	 __FUNCTION__, data, phyaddr, reg);

	/* set block address */
	serdes_set_blk(eth_data, phyaddr, blk);

	/* write register */
	chip_phy_wr(eth_data, SERDES_INTERNAL, phyaddr, off, data);
}


uint16
serdes_rd_reg(bcm_eth_t *eth_data, uint phyaddr, uint reg)
{
	uint blk = reg&0x7ff0;
	uint off = reg&0x000f;
	uint16	data;

	NET_TRACE(("%s enter\n", __FUNCTION__));

	if (reg&0x8000)
		off|=0x10;

	NET_REG_TRACE(("%s phyaddr(0x%x) reg(0x%x)\n",
		 __FUNCTION__, phyaddr, reg));

	/* set block address */
	serdes_set_blk(eth_data, phyaddr, blk);

	/* read register */
	data = chip_phy_rd(eth_data, SERDES_INTERNAL, phyaddr, off);
	NET_REG_TRACE(("%s phyaddr(0x%x) reg(0x%x) data(0x%x)\n",
		 __FUNCTION__, phyaddr, reg, data));
	//printf("%s read(0x%x) from phyaddr(0x%x) reg(0x%x)\n",
	//	 __FUNCTION__, data, phyaddr, reg);

	return data;
}


uint16
serdes_get_id(bcm_eth_t *eth_data, uint phyaddr, uint off)
{

	ASSERT(phyaddr < MAXEPHY);

	/* read the id high */
	return serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESID_SERDESID0r+off);
}


void
serdes_reset(bcm_eth_t *eth_data, uint phyaddr)
{
	uint ctrl;

	ASSERT(phyaddr < MAXEPHY);

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	/* set reset flag */
	ctrl = serdes_rd_reg(eth_data, phyaddr, XGXS16G_IEEE0BLK_IEEECONTROL0r);
	ctrl |= IEEE0BLK_IEEECONTROL0_RST_HW_MASK;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_IEEE0BLK_IEEECONTROL0r, ctrl);
	udelay(100);
	/* check if out of reset */
	if (serdes_rd_reg(eth_data, phyaddr, XGXS16G_IEEE0BLK_IEEECONTROL0r) & IEEE0BLK_IEEECONTROL0_RST_HW_MASK) {
		NET_ERROR(("et%d: %s reset not complete\n", eth_data->unit, __FUNCTION__));
	}
}


int
serdes_reset_core(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16		data16;
	uint16		serdes_id2;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	/* get serdes id */
	serdes_id2 = serdes_get_id(eth_data, phyaddr, 2);
	printf("%s pbyaddr(0x%x) id2(0x%x)\n", __FUNCTION__, phyaddr, serdes_id2);

	/* unlock lane */
	data16 = serdes_rd_reg(eth_data, phyaddr, 0x833c);
	data16 &= ~(0x0040);
	serdes_wr_reg(eth_data, phyaddr, 0x833c, data16);

	/* Reset the core */
	if ( phyaddr == 1 ) {
		/* Stop PLL Sequencer and configure the core into correct mode */
		data16 = (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane <<
					XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT) |
				XGXSBLK0_XGXSCONTROL_HSTL_MASK |
				XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
				XGXSBLK0_XGXSCONTROL_EDEN_MASK |
				XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK |
				XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK;
		serdes_wr_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSCONTROLr, data16);

		/* Disable IEEE block select auto-detect. 
		* The driver will select desired block as necessary.
		* By default, the driver keeps the XAUI block in
		* IEEE address space.
		*/
		data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_MISCCONTROL1r);
		if (XGXS16G_2p5G_ID(serdes_id2)) {
			data16 &= ~(	XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
							XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK);
		} else {
			data16 &= ~(	XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
							XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK);
			data16 |= XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK;
		}
		serdes_wr_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_MISCCONTROL1r, data16);

		/* disable in-band MDIO. PHY-443 */
		data16 = serdes_rd_reg(eth_data, phyaddr, 0x8111);
		/* rx_inBandMdio_rst */
		data16 |= 1 << 3;
		serdes_wr_reg(eth_data, phyaddr, 0x8111, data16);
	}
	return 0;
}


int
serdes_start_pll(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16		data16;

	if ( phyaddr == 1 ) {
		uint32		count=250;
		 /* Start PLL Sequencer and wait for PLL to lock */
		data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSCONTROLr);
		data16 |= XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK;
		serdes_wr_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSCONTROLr, data16);

		/* wait for PLL to lock */
		while (count!=0) {
			data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSSTATUSr);
			if ( data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK ) {
	            break;
			}
			/* wait 1 usec then dec counter */
			udelay(10);
			count--;
		}
		if (count == 0) {
			NET_ERROR(("%s TXPLL did not lock\n", __FUNCTION__));
		}
	}

	return 0;
}


/*
 * Function:
 *      serdes_init
 * Purpose:
 *      Initialize xgxs6 phys
 * Parameters:
 *      eth_data - ethernet data
 *      phyaddr - physical address
 * Returns:
 *      0
 */
int
serdes_init(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16		data16;
	uint16		serdes_id0, serdes_id1, serdes_id2;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	/* get serdes id */
	serdes_id0 = serdes_get_id(eth_data, phyaddr, 0);
	serdes_id1 = serdes_get_id(eth_data, phyaddr, 1);
	serdes_id2 = serdes_get_id(eth_data, phyaddr, 2);
	//printf("%s pbyaddr(0x%x) id0(0x%x) id1(0x%x) id2(0x%x)\n", __FUNCTION__, phyaddr, serdes_id0, serdes_id1, serdes_id2);

	/* get more ids */
	serdes_id0 = serdes_rd_reg(eth_data, phyaddr, 2);
	serdes_id1 = serdes_rd_reg(eth_data, phyaddr, 3);
	//printf("%s pbyaddr(0x%x) SERDES PhyID_MS(0x%x) PhyID_LS(0x%x)\n", __FUNCTION__, phyaddr, serdes_id0, serdes_id1);

	/* unlock lane */
	data16 = serdes_rd_reg(eth_data, phyaddr, 0x833c);
	data16 &= ~(0x0040);
	serdes_wr_reg(eth_data, phyaddr, 0x833c, data16);

#if 0
	/* Reset the core */
	if ( phyaddr == 1 ) {
		/* Stop PLL Sequencer and configure the core into correct mode */
		data16 = (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane <<
					XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT) |
				XGXSBLK0_XGXSCONTROL_HSTL_MASK |
				XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
				XGXSBLK0_XGXSCONTROL_EDEN_MASK |
				XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK |
				XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK;
		serdes_wr_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSCONTROLr, data16);

		/* Disable IEEE block select auto-detect. 
		* The driver will select desired block as necessary.
		* By default, the driver keeps the XAUI block in
		* IEEE address space.
		*/
		data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_MISCCONTROL1r);
		if (XGXS16G_2p5G_ID(serdes_id2)) {
			data16 &= ~(	XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
							XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK);
		} else {
			data16 &= ~(	XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
							XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK);
			data16 |= XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK;
		}
		serdes_wr_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_MISCCONTROL1r, data16);

		/* disable in-band MDIO. PHY-443 */
		data16 = serdes_rd_reg(eth_data, phyaddr, 0x8111);
		/* rx_inBandMdio_rst */
		data16 |= 1 << 3;
		serdes_wr_reg(eth_data, phyaddr, 0x8111, data16);
	}
#endif

	/* disable CL73 BAM */
	data16 = serdes_rd_reg(eth_data, phyaddr, 0x8372);
	data16 &= ~(CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK);
	serdes_wr_reg(eth_data, phyaddr, 0x8372, data16);

	/* Set Local Advertising Configuration */
	data16 = MII_ANA_C37_FD | MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE | MII_ANA_C37_HD;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_COMBO_IEEE0_AUTONEGADVr, data16);

	/* Disable BAM in Independent Lane mode. Over1G AN not supported  */
	data16 = 0;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr, data16);
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_BAM_NEXTPAGE_UD_FIELDr, data16);

	data16 = SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
				SERDESDIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK;
	/*
	* Put the Serdes in SGMII mode
	* bit0 = 0; in SGMII mode
	*/
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_CONTROL1000X1r, data16);

	/* Initialialize 1G and fullduplex */
	data16 = MII_CTRL_FD | MII_CTRL_SS_1000;
	/* set autoneg */
	data16 |= MII_CTRL_AE | MII_CTRL_RAN;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_COMBO_IEEE0_MIICNTLr, data16);

	/* Disable 10G parallel detect */
	data16 = 0;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_AN73_PDET_PARDET10GCONTROLr, data16);

	/* Disable BAM mode and Teton mode */
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr, data16);

	/* Enable lanes */
	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK1_LANECTRL0r);
	data16 |= XGXSBLK1_LANECTRL0_CL36_PCS_EN_RX_MASK |
				XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_XGXSBLK1_LANECTRL0r, data16);

    /* set elasticity fifo size to 13.5k to support 12k jumbo pkt size*/
	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_CONTROL1000X3r);
	data16 &= SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_RX_MASK;
	data16 |= (1 << 2);
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, data16);

    /* Enabble LPI passthru' for native mode EEE */
	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_REMOTEPHY_MISC5r);
	data16 |= 0xc000;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_REMOTEPHY_MISC5r, data16);
	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK7_EEECONTROLr);
	data16 |= 0x0007;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_XGXSBLK7_EEECONTROLr, data16);

#if 0
	if ( phyaddr == 1 ) {
		uint32		count=250;
		 /* Start PLL Sequencer and wait for PLL to lock */
		data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSCONTROLr);
		data16 |= XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK;
		serdes_wr_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSCONTROLr, data16);

		/* wait for PLL to lock */
		while (count!=0) {
			data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSSTATUSr);
			if ( data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK ) {
	            break;
			}
			/* wait 1 usec then dec counter */
			udelay(10);
			count--;
		}
		if (count == 0) {
			NET_ERROR(("%s TXPLL did not lock\n", __FUNCTION__));
		}
	}
#endif

	return 0;
}


/*
 * Function:
 *      serdes_enable_set
 * Purpose:
 *      Enable/Disable phy
 * Parameters:
 *      eth_data - ethernet data
 *      phyaddr - physical address
 *      enable - on/off state to set
 * Returns:
 *      0
 */
int
serdes_enable_set(bcm_eth_t *eth_data, uint phyaddr, int enable)
{
	uint16	data16, mask16;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK1_LANECTRL3r);
	mask16 = (1 << (phyaddr-1));	/* rx lane */
	mask16 |= (mask16 << 4);	/* add tx lane */
	mask16 |= 0x800;
	if (enable) {
		data16 &= ~(mask16);
	} else {
		data16 &= ~(mask16);
		data16 |= mask16;
	}
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_XGXSBLK1_LANECTRL3r, data16);

	return 0;
}


/*
 * Function:
 *      serdes_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      eth_data - ethernet data
 *      phyaddr - physical address
 *      speed - link speed in Mbps
 * Returns:
 *      0
 */
int
serdes_speed_set(bcm_eth_t *eth_data, uint phyaddr, int speed)
{
	uint16		speed_val, mask;
	uint16		data16;
	uint16		speed_mii;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	if (speed > 1000) {
		return -1;
	}

	speed_val = 0;
	speed_mii = 0;
	mask      = SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK |
				SERDESDIGITAL_MISC1_FORCE_SPEED_MASK;

	switch (speed) {
	case 0:
		/* Do not change speed */
		return 0;
	case 10:
		speed_mii = MII_CTRL_SS_10;
		break;
	case 100:
		speed_mii = MII_CTRL_SS_100;
		break;
	case 1000:
		speed_mii = MII_CTRL_SS_1000;
		break;
	default:
		return -1;
	}

	/* Hold rxSeqStart */
	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_RX0_RX_CONTROLr);
	data16 |= DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_RX0_RX_CONTROLr, data16);

	/* hold TX FIFO in reset */
	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_CONTROL1000X3r);
	data16 |= SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, data16);

	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_MISC1r);
	data16 &= ~(mask);
	data16 |= speed_val;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_MISC1r, data16);

	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_COMBO_IEEE0_MIICNTLr);
	data16 &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
	data16 |= speed_mii;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_COMBO_IEEE0_MIICNTLr, data16);

	/* release rxSeqStart */
	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_RX0_RX_CONTROLr);
	data16 &= ~(DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK);
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_RX0_RX_CONTROLr, data16);
                                                                               
	/* release TX FIFO reset */
	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_CONTROL1000X3r);
	data16 &= ~(SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK);
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, data16);

	return 0;
}


/*
 * Function:
 *      serdes_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      eth_data - ethernet data
 *      phyaddr - physical address
 *      speed - current link speed in Mbps
 * Returns:
 *      0
 */
int
serdes_speed_get(bcm_eth_t *eth_data, uint phyaddr, int *speed)
{
	uint16	data16;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_STATUS1000X1r);

	data16 &= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_MASK;
	data16 >>= SERDESDIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT;

	if (data16 == 3) {
		*speed= 2500;
	} else if (data16 == 2) {
		*speed= 1000;
	} else if (data16 == 1) {
		*speed= 100;
	} else {
		*speed= 10;
	}

	return 0;
}


/*
 * Function:
 *      phy_xgxs16g1l_lb_set
 * Purpose:
 *      Put XGXS6/FusionCore in PHY loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:
 *      0
 */
int
serdes_lb_set(bcm_eth_t *eth_data, uint phyaddr, int enable)
{
	uint16      misc_ctrl, data16;
	uint16      lb_bit;
	uint16      lb_mask;

	/* Configure Loopback in XAUI */
	misc_ctrl = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_MISCCONTROL1r);
	if (misc_ctrl & XGXSBLK0_MISCCONTROL1_PCS_DEV_EN_OVERRIDE_MASK) {
		/* PCS */
		lb_bit  = (enable) ? IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK : 0;
		lb_mask = IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK;
	} else if (misc_ctrl & XGXSBLK0_MISCCONTROL1_PMD_DEV_EN_OVERRIDE_MASK) {
		/* PMA/PMD */
		lb_bit  = (enable) ? 1 : 0;
		lb_mask = 1;
	} else {
		/* PHY XS, DTE XS */
		lb_bit  = (enable) ? IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK : 0;
		lb_mask = IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK;
	}

	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_IEEE0BLK_IEEECONTROL0r);
	data16 &= ~(lb_mask);
	data16 |= lb_bit;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_IEEE0BLK_IEEECONTROL0r, data16);

	/* Configure Loopback in SerDes */
	lb_bit  = (enable) ? MII_CTRL_LE : 0;
	lb_mask = MII_CTRL_LE;
	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_COMBO_IEEE0_MIICNTLr);
	data16 &= ~(lb_mask);
	data16 |= lb_bit;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_COMBO_IEEE0_MIICNTLr, data16);

	data16 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSCONTROLr);
	data16 |= 0x10;
	serdes_wr_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSCONTROLr, data16);

	data16 = serdes_rd_reg(eth_data, phyaddr, 0x8017);
	data16 = 0xff0f;
	serdes_wr_reg(eth_data, phyaddr, 0x8017, data16);

    return 0;
}

void
serdes_disp_status(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16		tmp0, tmp1, tmp2, tmp3;

	printf("et%d: %s: phyaddr:%d\n", eth_data->unit, __FUNCTION__, phyaddr);

	tmp0 = serdes_get_id(eth_data, phyaddr, 0);
	tmp1 = serdes_get_id(eth_data, phyaddr, 1);
	tmp2 = serdes_get_id(eth_data, phyaddr, 2);
	printf("  id0(0x%x) id1(0x%x) id2(0x%x)\n", tmp0, tmp1, tmp2);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_IEEE0BLK_IEEECONTROL0r);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_IEEE0BLK_IEEECONTROL0r+1);
	printf("  MII-Control(0): 0x%x; MII-Status(1): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, 2);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, 3);
	printf("  Phy ChipID(2:3): 0x%04x:0x%04x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, 4);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, 5);
	tmp2 = serdes_rd_reg(eth_data, phyaddr, 0xf);
	printf("  AN AD(4): 0x%x; AN LNK PARTNER(5): 0x%x; EXT STAT(f): 0x%x\n", tmp0, tmp1, tmp2);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSCONTROLr);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_XGXSSTATUSr);
	printf("  XGXS-Control(8000): 0x%x; XGXS-Status(8001): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_MMDSELECTr);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK0_MISCCONTROL1r);
	printf("  XGXS BLK0 MMD Select(800d): 0x%x; XGXS BLK0 MISC CTRL(800e): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK1_LANECTRL0r);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK1_LANECTRL3r);
	printf("  XGXS BLK1 LNCTRL0(8015): 0x%x; XGXS BLK1_LNCTRL3(8018): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_RX0_RX_CONTROLr);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_AN73_PDET_PARDET10GCONTROLr);
	tmp2 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_XGXSBLK7_EEECONTROLr);
	printf("  XGXS RX0 CTRL(80b1): 0x%x; XGXS AN73 PARDET CTRL(8131): 0x%x; XGXS BLK7 EEECTRL(8150): 0x%x\n", tmp0, tmp1, tmp2);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, 0x8111);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, 0x8372);
	printf("  (8111): 0x%x; (8372): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_CONTROL1000X1r);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_CONTROL1000X2r);
	tmp2 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_CONTROL1000X3r);
	printf("  XGXS SERDES DIG CTRL 1000X1(8300): 0x%x; XGXS SERDES DIG CTRL 1000X2(8301): 0x%x; XGXS SERDES DIGITAL CTRL 1000X3r(8302): 0x%x\n", tmp0, tmp1, tmp2);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_STATUS1000X1r);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESDIGITAL_MISC1r);
	printf("  XGXS SERDES DIG STATUS 1000X1(8304): 0x%x; XGXS SERDES DIG MISC1(8308): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESID_SERDESID0r);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESID_SERDESID1r);
	printf("  XGXS SERDESID0(8310): 0x%x; XGXS SERDESID1(8311): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESID_SERDESID2r);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_SERDESID_SERDESID3r);
	printf("  XGXS SERDESID0(8312): 0x%x; XGXS SERDESID1(8313): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_REMOTEPHY_MISC3r);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_REMOTEPHY_MISC5r);
	printf("  XGXS REMOTEPHY MISC3(833c): 0x%x; XGXS REMOTEPHY MISC5(833e): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_BAM_NEXTPAGE_UD_FIELDr);
	printf("  XGXS BAM MP5_NEXTPAGECTRL(8350): 0x%x; XGXS BAM NP UD FIELDr(8357): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_COMBO_IEEE0_MIICNTLr);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, XGXS16G_COMBO_IEEE0_AUTONEGADVr);
	printf("  XGXS COMBO IEEE0 MIICNTL(ffe0): 0x%x; XGXS COMBO IEEE0 AUTONEGADVr(ffe4): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, 0x8050);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, 0x8122);
	printf("  (8050): 0x%x; (8122): 0x%x\n", tmp0, tmp1);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, 0x80b0);
	tmp1 = serdes_rd_reg(eth_data, phyaddr, 0x80c0);
	tmp2 = serdes_rd_reg(eth_data, phyaddr, 0x80d0);
	tmp3 = serdes_rd_reg(eth_data, phyaddr, 0x80e0);
	printf("  (80b0): 0x%x; (80c0): 0x%x; (80d0): 0x%x, (80e0): 0x%x\n", tmp0, tmp1, tmp2, tmp3);

	tmp0 = serdes_rd_reg(eth_data, phyaddr, 0xffe1);
	printf("  (ffe1): 0x%x\n", tmp0);

}
#endif
