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
*  @file    bcmiproc_robo_serdes.c
*
*  @brief   serdes routines
*
*  @note
*
*   These routines provide access to the serdes
*/
/****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */
//#include <bcmutils.h>
//#include <bcmenetphy.h>
//#include <siutils.h>
#include <asm/arch/bcmrobo.h>
#include "bcmiproc_robo_serdes.h"
#include "bcmiproc_serdes_def.h"

/* ---- External Variable Declarations ----------------------------------- */
/* ---- External Function Prototypes ------------------------------------- */
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */

/* debug/trace */
//#define BCMDBG
//#define BCMDBG_ERR
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

/* ==== Public Functions ================================================= */

uint16
robo_serdes_get_id(robo_info_t *robo, uint page, uint off)
{
	uint16 data;

	/* read the id high */
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESID_SERDESID0r+off, &data);
	return data;
}


void
robo_serdes_reset(robo_info_t *robo, uint page)
{
	uint16 ctrl;

	/* set reset flag */
	srab_sgmii_rreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &ctrl);
	ctrl |= IEEE0BLK_IEEECONTROL0_RST_HW_MASK;
	srab_sgmii_wreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &ctrl);
	udelay(100);
	/* check if out of reset */
	srab_sgmii_rreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &ctrl);
	if (ctrl & IEEE0BLK_IEEECONTROL0_RST_HW_MASK) {
		NET_ERROR(("%s page (0x%x) reset not complete\n", __FUNCTION__, page));
	}
}


int
robo_serdes_reset_core(robo_info_t *robo, uint page)
{
	uint16		data16;
	uint16		serdes_id2;

	/* get serdes id */
	serdes_id2 = robo_serdes_get_id(robo, page, 2);
	printf("%s page(0x%x) id2(0x%x)\n", __FUNCTION__, page, serdes_id2);

	/* unlock lane */
	srab_sgmii_rreg(robo, page, 0x833c, &data16);
	data16 &= ~(0x0040);
	srab_sgmii_wreg(robo, page, 0x833c, &data16);

	if ( page == PAGE_P5_SGMII ) {
		/* Reset the core */
		/* Stop PLL Sequencer and configure the core into correct mode */
		data16 = (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane <<
					XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT) |
				XGXSBLK0_XGXSCONTROL_HSTL_MASK |
				XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
				XGXSBLK0_XGXSCONTROL_EDEN_MASK |
				XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK |
				XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK;
		srab_sgmii_wreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &data16);

		/* Disable IEEE block select auto-detect. 
		* The driver will select desired block as necessary.
		* By default, the driver keeps the XAUI block in
		* IEEE address space.
		*/
		srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_MISCCONTROL1r, &data16);
		if (XGXS16G_2p5G_ID(serdes_id2)) {
			data16 &= ~(	XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
							XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK);
		} else {
			data16 &= ~(	XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
							XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK);
			data16 |= XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK;
		}
		srab_sgmii_wreg(robo, page, XGXS16G_XGXSBLK0_MISCCONTROL1r, &data16);

#if 0 //dgb
		/* disable in-band MDIO. PHY-443 */
		srab_sgmii_rreg(robo, page, 0x8111, &data16);
		/* rx_inBandMdio_rst */
		data16 |= 1 << 3;
		srab_sgmii_wreg(robo, page, 0x8111, &data16);
#endif
	}
	return 0;
}


int
robo_serdes_start_pll(robo_info_t *robo, uint page)
{
	uint16		data16;

	if ( page == PAGE_P5_SGMII ) {
		uint32		count=250;
		 /* Start PLL Sequencer and wait for PLL to lock */
		srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &data16);
		data16 |= XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK;
		srab_sgmii_wreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &data16);

		/* wait for PLL to lock */
		while (count!=0) {
			srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_XGXSSTATUSr, &data16);
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
 *      robo_serdes_init
 * Purpose:
 *      Initialize xgxs6 phys
 * Parameters:
 *      robo - robo handle
 *      page - port page
 * Returns:
 *      0
 */
int
robo_serdes_init(robo_info_t *robo, uint page)
{
	uint16		data16;
	uint16		serdes_id0, serdes_id1, serdes_id2;
	uint		locpage=page;

	if ( page == PAGE_P4_SGMII ) {
		printf("%s access page4 via page5 lane1\n", __FUNCTION__);
		locpage = PAGE_P5_SGMII;
		// try to access p4 registers via lane1
		data16 = 1;
		srab_sgmii_wreg(robo, locpage, 0xffde, &data16);
	}

	/* get serdes id */
	serdes_id0 = robo_serdes_get_id(robo, locpage, 0);
	serdes_id1 = robo_serdes_get_id(robo, locpage, 1);
	serdes_id2 = robo_serdes_get_id(robo, locpage, 2);
	printf("%s page(0x%x) id0(0x%x) id1(0x%x) id2(0x%x)\n", __FUNCTION__, locpage, serdes_id0, serdes_id1, serdes_id2);

	/* get more ids */
	srab_sgmii_rreg(robo, locpage, 2, &serdes_id0);
	srab_sgmii_rreg(robo, locpage, 3, &serdes_id1);
	//printf("%s locpage(0x%x) SERDES PhyID_MS(0x%x) PhyID_LS(0x%x)\n", __FUNCTION__, locpage, serdes_id0, serdes_id1);

	/* unlock lane */
	srab_sgmii_rreg(robo, locpage, 0x833c, &data16);
	data16 &= ~(0x0040);
	srab_sgmii_wreg(robo, locpage, 0x833c, &data16);

	/* Must Enable TX polarity flip */
	srab_sgmii_rreg(robo, locpage, XGXS16G_TX0_TX_ACONTROL0r, &data16);
	data16 |= TX0_TX_ACONTROL0_TXPOL_FLIP_MASK;
	srab_sgmii_wreg(robo, locpage, XGXS16G_TX0_TX_ACONTROL0r, &data16);

	/* disable CL73 BAM */
	srab_sgmii_rreg(robo, locpage, 0x8372, &data16);
	data16 &= ~(CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK);
	srab_sgmii_wreg(robo, locpage, 0x8372, &data16);

	/* Set Local Advertising Configuration */
	data16 = MII_ANA_C37_FD | MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE;
	srab_sgmii_wreg(robo, locpage, XGXS16G_COMBO_IEEE0_AUTONEGADVr, &data16);

	/* Disable BAM in Independent Lane mode. Over1G AN not supported  */
	data16 = 0;
	srab_sgmii_wreg(robo, locpage, XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr, &data16);
	srab_sgmii_wreg(robo, locpage, XGXS16G_BAM_NEXTPAGE_UD_FIELDr, &data16);

	data16 = SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
				SERDESDIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK |
				SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;

//	data16 |= SERDESDIGITAL_CONTROL1000X1_REMOTE_LOOPBACK_MASK;
	/*
	* Put the Serdes in SGMII mode
	* bit0 = 0; in SGMII mode
	*/
	srab_sgmii_wreg(robo, locpage, XGXS16G_SERDESDIGITAL_CONTROL1000X1r, &data16);

	/* set autoneg */
	data16 = MII_CTRL_AE | MII_CTRL_RAN;
//	data16 = MII_CTRL_SS_MSB | MII_CTRL_FD;
	srab_sgmii_wreg(robo, locpage, XGXS16G_COMBO_IEEE0_MIICNTLr, &data16);

	/* Disable 10G parallel detect */
	data16 = 0;
	srab_sgmii_wreg(robo, locpage, XGXS16G_AN73_PDET_PARDET10GCONTROLr, &data16);

	/* Disable BAM mode and Teton mode */
	srab_sgmii_wreg(robo, locpage, XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr, &data16);

	/* Enable lanes */
	srab_sgmii_rreg(robo, locpage, XGXS16G_XGXSBLK1_LANECTRL0r, &data16);
	data16 |= XGXSBLK1_LANECTRL0_CL36_PCS_EN_RX_MASK |
				XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK;
	srab_sgmii_wreg(robo, locpage, XGXS16G_XGXSBLK1_LANECTRL0r, &data16);

    /* set elasticity fifo size to 13.5k to support 12k jumbo pkt size*/
	srab_sgmii_rreg(robo, locpage, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);
	data16 &= SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_RX_MASK;
	data16 |= (1 << 2);
	srab_sgmii_wreg(robo, locpage, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);

    /* Enabble LPI passthru' for native mode EEE */
	srab_sgmii_rreg(robo, locpage, XGXS16G_REMOTEPHY_MISC5r, &data16);
	data16 |= 0xc000;
	srab_sgmii_wreg(robo, locpage, XGXS16G_REMOTEPHY_MISC5r, &data16);
	srab_sgmii_rreg(robo, locpage, XGXS16G_XGXSBLK7_EEECONTROLr, &data16);
	data16 |= 0x0007;
	srab_sgmii_wreg(robo, locpage, XGXS16G_XGXSBLK7_EEECONTROLr, &data16);

	if ( page == PAGE_P4_SGMII ) {
		printf("%s set back to lane0\n", __FUNCTION__);
		// set back to lane 0
		data16 = 0;
		srab_sgmii_wreg(robo, locpage, 0xffde, &data16);
	}

	return 0;
}


/*
 * Function:
 *      robo_serdes_enable_set
 * Purpose:
 *      Enable/Disable phy
 * Parameters:
 *      robo - robo handle
 *      page - port page
 *      enable - on/off state to set
 * Returns:
 *      0
 */
int
robo_serdes_enable_set(robo_info_t *robo, uint page, int enable)
{
	uint16	data16, mask16;

	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK1_LANECTRL3r, &data16);
	if (page == PAGE_P5_SGMII)
		/* lane 0 */
		mask16 = 1;		/* rx lane */
	else
		/* lane 1 */
		mask16 = 2;		/* rx lane */
	mask16 |= (mask16 << 4);	/* add tx lane */
	mask16 |= 0x800;
	if (enable) {
		data16 &= ~(mask16);
	} else {
		data16 &= ~(mask16);
		data16 |= mask16;
	}
	srab_sgmii_wreg(robo, page, XGXS16G_XGXSBLK1_LANECTRL3r, &data16);

	return 0;
}


/*
 * Function:
 *      robo_serdes_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      eth_num - ethernet data
 *      phyaddr - physical address
 *      speed - link speed in Mbps
 * Returns:
 *      0
 */
int
robo_serdes_speed_set(robo_info_t *robo, uint page, int speed)
{
	uint16		speed_val, mask;
	uint16		data16;
	uint16		speed_mii;

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
	srab_sgmii_rreg(robo, page, XGXS16G_RX0_RX_CONTROLr, &data16);
	data16 |= DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK;
	srab_sgmii_wreg(robo, page, XGXS16G_RX0_RX_CONTROLr, &data16);

	/* hold TX FIFO in reset */
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);
	data16 |= SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK;
	srab_sgmii_wreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_MISC1r, &data16);
	data16 &= ~(mask);
	data16 |= speed_val;
	srab_sgmii_wreg(robo, page, XGXS16G_SERDESDIGITAL_MISC1r, &data16);

	srab_sgmii_rreg(robo, page, XGXS16G_COMBO_IEEE0_MIICNTLr, &data16);
	data16 &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
	data16 |= speed_mii;
	srab_sgmii_wreg(robo, page, XGXS16G_COMBO_IEEE0_MIICNTLr, &data16);

	/* release rxSeqStart */
	srab_sgmii_rreg(robo, page, XGXS16G_RX0_RX_CONTROLr, &data16);
	data16 &= ~(DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK);
	srab_sgmii_wreg(robo, page, XGXS16G_RX0_RX_CONTROLr, &data16);
                                                                               
	/* release TX FIFO reset */
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);
	data16 &= ~(SERDESDIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK);
	srab_sgmii_wreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &data16);

	return 0;
}


/*
 * Function:
 *      robo_serdes_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      robo - robo handle
 *      page - port page
 *      speed - current link speed in Mbps
 * Returns:
 *      0
 */
int
robo_serdes_speed_get(robo_info_t *robo, uint page, int *speed)
{
	uint16	data16;

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_STATUS1000X1r, &data16);

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
 *      robo_serdes_lb_set
 * Purpose:
 *      Put XGXS6/FusionCore in PHY loopback
 * Parameters:
 *      robo - robo handle
 *      page - port page
 *      enable - binary value for on/off (1/0)
 * Returns:
 *      0
 */
int
robo_serdes_lb_set(robo_info_t *robo, uint gpage, int enable)
{
	uint16      misc_ctrl, data16;
	uint16      lb_bit;
	uint16      lb_mask;


	uint		page=gpage;

	if ( page == PAGE_P4_SGMII ) {
		printf("%s access page4 via page5 lane1\n", __FUNCTION__);
		page = PAGE_P5_SGMII;
		// try to access p4 registers via lane1
		data16 = 1;
		srab_sgmii_wreg(robo, page, 0xffde, &data16);
	}


	/* Configure Loopback in XAUI */
	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_MISCCONTROL1r, &misc_ctrl);
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

	srab_sgmii_rreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &data16);
	data16 &= ~(lb_mask);
	data16 |= lb_bit;
	srab_sgmii_wreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &data16);

	/* Configure Loopback in SerDes */
	lb_bit  = (enable) ? MII_CTRL_LE : 0;
	lb_mask = MII_CTRL_LE;
	srab_sgmii_rreg(robo, page, XGXS16G_COMBO_IEEE0_MIICNTLr, &data16);
	data16 &= ~(lb_mask);
	data16 |= lb_bit;
	srab_sgmii_wreg(robo, page, XGXS16G_COMBO_IEEE0_MIICNTLr, &data16);

	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &data16);
//	data16 |= 0x10;
	data16 |= 0x50;
	srab_sgmii_wreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &data16);

	srab_sgmii_rreg(robo, page, 0x8017, &data16);
	data16 = 0xff0f;
	srab_sgmii_wreg(robo, page, 0x8017, &data16);

    return 0;
}

void
robo_serdes_disp_status(robo_info_t *robo, uint page)
{
	uint16		tmp0, tmp1, tmp2, tmp3;

	printf("%s: page:%d\n", __FUNCTION__, page);

	tmp0 = robo_serdes_get_id(robo, page, 0);
	tmp1 = robo_serdes_get_id(robo, page, 1);
	tmp2 = robo_serdes_get_id(robo, page, 2);
	printf("  id0(0x%x) id1(0x%x) id2(0x%x)\n", tmp0, tmp1, tmp2);

	srab_sgmii_rreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_IEEE0BLK_IEEECONTROL0r+1, &tmp1);
	printf("  MII-Control(0): 0x%x; MII-Status(1): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, 2, &tmp0);
	srab_sgmii_rreg(robo, page, 3, &tmp1);
	printf("  Phy ChipID(2:3): 0x%04x:0x%04x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, 4, &tmp0);
	srab_sgmii_rreg(robo, page, 5, &tmp1);
	srab_sgmii_rreg(robo, page, 0xf, &tmp2);
	printf("  AN AD(4): 0x%x; AN LNK PARTNER(5): 0x%x; EXT STAT(f): 0x%x\n", tmp0, tmp1, tmp2);

	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_XGXSCONTROLr, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_XGXSSTATUSr, &tmp1);
	printf("  XGXS-Control(8000): 0x%x; XGXS-Status(8001): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_MMDSELECTr, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK0_MISCCONTROL1r, &tmp1);
	printf("  XGXS BLK0 MMD Select(800d): 0x%x; XGXS BLK0 MISC CTRL(800e): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK1_LANECTRL0r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK1_LANECTRL3r, &tmp1);
	printf("  XGXS BLK1 LNCTRL0(8015): 0x%x; XGXS BLK1_LNCTRL3(8018): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_RX0_RX_CONTROLr, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_AN73_PDET_PARDET10GCONTROLr, &tmp1);
	srab_sgmii_rreg(robo, page, XGXS16G_XGXSBLK7_EEECONTROLr, &tmp2);
	printf("  XGXS RX0 CTRL(80b1): 0x%x; XGXS AN73 PARDET CTRL(8131): 0x%x; XGXS BLK7 EEECTRL(8150): 0x%x\n", tmp0, tmp1, tmp2);

	srab_sgmii_rreg(robo, page, 0x8111, &tmp0);
	srab_sgmii_rreg(robo, page, 0x8372, &tmp1);
	printf("  (8111): 0x%x; (8372): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X1r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X2r, &tmp1);
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_CONTROL1000X3r, &tmp2);
	printf("  XGXS SERDES DIG CTRL 1000X1(8300): 0x%x; XGXS SERDES DIG CTRL 1000X2(8301): 0x%x; XGXS SERDES DIGITAL CTRL 1000X3r(8302): 0x%x\n", tmp0, tmp1, tmp2);

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_STATUS1000X1r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESDIGITAL_MISC1r, &tmp1);
	printf("  XGXS SERDES DIG STATUS 1000X1(8304): 0x%x; XGXS SERDES DIG MISC1(8308): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESID_SERDESID0r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESID_SERDESID1r, &tmp1);
	printf("  XGXS SERDESID0(8310): 0x%x; XGXS SERDESID1(8311): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_SERDESID_SERDESID2r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_SERDESID_SERDESID3r, &tmp1);
	printf("  XGXS SERDESID0(8312): 0x%x; XGXS SERDESID1(8313): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_REMOTEPHY_MISC3r, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_REMOTEPHY_MISC5r, &tmp1);
	printf("  XGXS REMOTEPHY MISC3(833c): 0x%x; XGXS REMOTEPHY MISC5(833e): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_BAM_NEXTPAGE_UD_FIELDr, &tmp1);
	printf("  XGXS BAM MP5_NEXTPAGECTRL(8350): 0x%x; XGXS BAM NP UD FIELDr(8357): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, XGXS16G_COMBO_IEEE0_MIICNTLr, &tmp0);
	srab_sgmii_rreg(robo, page, XGXS16G_COMBO_IEEE0_AUTONEGADVr, &tmp1);
	printf("  XGXS COMBO IEEE0 MIICNTL(ffe0): 0x%x; XGXS COMBO IEEE0 AUTONEGADVr(ffe4): 0x%x\n", tmp0, tmp1);

   srab_sgmii_rreg(robo, page, 0x8050, &tmp0);
   srab_sgmii_rreg(robo, page, 0x8122, &tmp1);
	printf("  (8050): 0x%x; (8122): 0x%x\n", tmp0, tmp1);

	srab_sgmii_rreg(robo, page, 0x80b0, &tmp0);
	srab_sgmii_rreg(robo, page, 0x80c0, &tmp1);
	srab_sgmii_rreg(robo, page, 0x80d0, &tmp2);
	srab_sgmii_rreg(robo, page, 0x80e0, &tmp3);
	printf("  (80b0): 0x%x; (80c0): 0x%x; (80d0): 0x%x, (80e0): 0x%x\n", tmp0, tmp1, tmp2, tmp3);

	srab_sgmii_rreg(robo, page, 0xffe1, &tmp0);
	printf("  (ffe1): 0x%x\n", tmp0);

}


#if 0
int wcmod_set_port_mode(wcmod_st* pc)           /* SET_PORT_MODE */
{
  uint16 rv, mask, data, maskr, datar, laneValue;
  int tmp_lane_select;

  int laneSwap = pc->lane_swap;
  int unit = pc->unit;
  wcmod_os_type os = pc->os_type;

  if (pc->wc_touched == 0) {
    pc->wc_touched = 1;
  } else {
    if (pc->verbosity > 0) printf("%-22s: Skipping multiple inits\n",__func__);
    return SOC_E_NONE;
  }

  SOC_IF_ERROR_RETURN(_wcmod_getRevDetails(pc));

  pc->lane_num_ignore = 1;
  /***************  COMBO/INDEP SINGLE/DUAL/QUAD PORT *****************/
  /* fix OS Type for XENIA */
  if ((pc->model_type == WCMOD_XN) || (pc->model_type == WCMOD_QS)) {
    pc->os_type = os = WCMOD_OS_IND_LANE;
    if (pc->verbosity > 0)
      printf("%-22s: OS fixed to %d for Xenia\n",__func__, pc->os_type);
  }
  /***** disable laneDisable and cl49 for TR3 *****/
  pc->lane_num_ignore = 0;
  tmp_lane_select     = pc->lane_select;        /* save this */
  pc->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc, 0,
                                           DIGITAL4_MISC3_LANEDISABLE_MASK));
  /* disable cl72 */
  mask = CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK |
         CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK;
  MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(pc->unit, pc, 
         CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK, mask);

  /*set all the sync word and mask for all lanes */
  WRITE_WC40_RX66_SCW0r(pc->unit, pc, 0xE070);
  WRITE_WC40_RX66_SCW1r(pc->unit, pc, 0xC0D0);
  WRITE_WC40_RX66_SCW2r(pc->unit, pc, 0xA0B0);
  WRITE_WC40_RX66_SCW3r(pc->unit, pc, 0x8090);
  WRITE_WC40_RX66_SCW0_MASKr(pc->unit, pc, 0xF0F0);
  WRITE_WC40_RX66_SCW1_MASKr(pc->unit, pc, 0xF0F0);
  WRITE_WC40_RX66_SCW2_MASKr(pc->unit, pc, 0xF0F0);
  WRITE_WC40_RX66_SCW3_MASKr(pc->unit, pc, 0xF0F0);

  SOC_IF_ERROR_RETURN(MODIFY_WC40_RX66B1_BRCM_CL49_CONTROLr(pc->unit, pc,
                        RX66B1_BRCM_CL49_CONTROL_MDIO_BRCM6466_CL49_EN_MASK,
                        RX66B1_BRCM_CL49_CONTROL_MDIO_BRCM6466_CL49_EN_MASK));
  pc->lane_select        = tmp_lane_select;     /* restore */
  pc->lane_num_ignore    = 1;                   /* enable again */

  if (pc->port_type == WCMOD_COMBO) {
    if (pc->verbosity > 0)
      printf("%-22s: Combo Mode\n",__func__);

    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x0,
                              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK));

    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                 (XGXSBLK0_XGXSCONTROL_MODE_10G_ComboCoreMode <<
                     XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                     XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));
    
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc, 
                     XGXSBLK6_XGXSX2CONTROL2_MAC_INF_TXCK_SEL_MASK,
                     XGXSBLK6_XGXSX2CONTROL2_MAC_INF_TXCK_SEL_MASK)) ;

  } else if (pc->port_type == WCMOD_INDEPENDENT) {
    if( pc->dual_type ) {
      pc->dxgxs = 0 ; 
      pc->lane_select = WCMOD_LANE_BCST;
      SOC_IF_ERROR_RETURN
          (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc, 
              (0x9 <<  XGXSBLK6_XGXSX2CONTROL2_MAC_INF_TXCK_SEL_SHIFT),
                       XGXSBLK6_XGXSX2CONTROL2_MAC_INF_TXCK_SEL_MASK)) ;
      pc->dxgxs = pc->dual_type ;
      pc->lane_select        = tmp_lane_select;     /* restore */
    } else {
      SOC_IF_ERROR_RETURN
          (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc, 
                    0, XGXSBLK6_XGXSX2CONTROL2_MAC_INF_TXCK_SEL_MASK)) ;
    }

    if (pc->verbosity > 0)
      printf("%-22s: Independent Mode\n",__func__);
    switch(pc->refclk) {
      case 25 : datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_25MHz;     break;
      case 50 : datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_50Mhz;     break;
      case 100: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_100MHz;    break;
      case 106: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_106p25Mhz; break;
      case 125: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz;    break;
      case 156: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz; break;
      case 161: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_161p25Mhz; break;
      case 187: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_187p5MHz;  break;
      default : datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz; 
    }
    maskr = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK;
    datar = datar << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT;

    /* os5/os8 (for indep. only) */
    if        (os == WCMOD_OS5) {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc,
                   (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS5 <<
                    XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                    XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

      data = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |  /* PP */
             (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 <<
              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | datar;
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK | maskr;

      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit,pc,data, mask));
    } else if (os == WCMOD_OS_IND_LANE) { /* Xenia OS IndLane neither OS5/OS8 */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc,
               (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_XENIA <<
                XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
               XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

      if (pc->refclk == 125) { /* For ESM_SERDES  core (redstone) */
        data = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz
             << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
        (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div20
         << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
    mask = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
           SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
           SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
      } else {
    data = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK | 
           (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div16_XENIA
        << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) |
        datar;
    mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
           SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
           maskr;
      }
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit,pc,data, mask));
    } else if (os == WCMOD_OS8) {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc,
               (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS8 <<
                XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
               XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

      data = (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK | 
              (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66
              << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) |
              datar);
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
             maskr;

      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit,pc,data, mask));
    } else {
      if (pc->model_type == WCMOD_WL_A0 || pc->model_type == WCMOD_WC_C0) {
        SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x0,
                              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK));
        /* QSGMII Mode for dependentl lanes is 6 not 5 */
      } else if ((pc->model_type == WCMOD_QS_A0) && (os == WCMOD_OS6)) {
        SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                                               (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS6 <<
                                                                XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                                                               XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));
        
      } else {
        printf ("%-22s Error: Bad OSTYPE in independant mode:%d\n",__func__,os);
        return SOC_E_ERROR;
      }
    }
  } else {
    printf ("%-22s Error: Bad combo/ind mode:%d\n",__func__,pc->port_type);
    return SOC_E_ERROR;
  }

  /* configure the PMA/PMD mux/demux */
  mask = 0x00ff;
  /* Mapping 16 bit value to 8 bit positions */
  if ((laneSwap & 0xffff) == 0x3210) {
    if (pc->verbosity > 0)
      printf ("%-22s: Tx Lanes not swapped\n", __func__);
  } else {
    laneValue =(((laneSwap & 0x3000) >> 6) | ((laneSwap & 0x0300) >> 4) |
                ((laneSwap & 0x0030) >> 2) | ((laneSwap & 0x0003) >> 0));
    SOC_IF_ERROR_RETURN
          (MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(unit, pc, (laneValue & 0xff), mask));
    if (pc->verbosity > 0) printf ("%-22s: Tx lanes swapped: %08d->%04d\n", __func__,
                                              (laneSwap & 0xffff), laneValue);
  }
  laneValue =(((laneSwap & 0x30000000) >> 22) | ((laneSwap & 0x03000000) >> 20) |
              ((laneSwap & 0x00300000) >> 18) | ((laneSwap & 0x00030000) >> 16));
  if ((laneSwap & 0xffff0000)== 0x32100000) {
    if (pc->verbosity > 0)
      printf ("%-22s: Rx Lanes not swapped\n", __func__);
  } else {
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_XGXSBLK8_RXLNSWAP1r(unit, pc, laneValue, mask));
    if (pc->verbosity > 0) printf ("%-22s: Rx lanes swapped: %08d->%04d\n", __func__,
                                            (laneSwap&0xffff0000), laneValue);
  }

  /* VCO rate, set only for indep mode. Automatically done in combo mode. */
  if (pc->port_type == WCMOD_INDEPENDENT) {
    tmp_lane_select =  pc->lane_select; /* save. Next writes are broadcast */
    pc->lane_select = WCMOD_LANE_BCST;

    if (pc->refclk == 125) {
      data =((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz
              << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div20
              << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
     mask = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
    } else {
      data =(SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz
             << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT));
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK;
    }

    /* Set to 1G mode to avoid conflicting default setting like R2 */
    /*
    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x0,
                                         SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, 0x0,
                                           DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));
    if (pc->model_type == WCMOD_XN) {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_IEEE0BLK_MIICNTLr(unit, pc,
                                     IEEE0BLK_MIICNTL_MANUAL_SPEED1_MASK,
                                     (IEEE0BLK_MIICNTL_MANUAL_SPEED1_MASK |
                                      IEEE0BLK_MIICNTL_MANUAL_SPEED0_MASK)));
    } else {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc,
                                     COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK,
                                     (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                      COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK)));
    }
    */
    pc->lane_select = tmp_lane_select; /* restore. End of broadcast */
  }

  /* configure MAC interface: make a seperate process
  if ((pc->port_type==WCMOD_INDEPENDENT) &
      (  (spd_intf==WCMOD_SPD_R2_12000)
       || (spd_intf==WCMOD_SPD_10000_XFI)
       || (spd_intf==SCMOD_SPD_15750_HI_DXGXS)){
    mask = XGXSBLK6_XGXSX2CONTROL2_MAC_INF_RXCK_OVERRIDE_MASK;
    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc,mask,mask);
    if (pc->verbosity > 0)
      printf ("%-22s. mac_inf_rxck_override bit is set\n",__func__);
  }

  if ((pc->port_type == WCMOD_INDEPENDENT) & (spd_intf == WCMOD_SPD_10000_XFI)) {
    mask = 1 << 13;
    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc,mask,mask);
    if (pc->verbosity > 0)
      printf ("%-22s txckout33_override bit is set\n",__func__);
  }
  */
  
  /* load firmware */
  rv = wcmod_firmware_set (pc);
  pc->lane_num_ignore = 0;
  return rv;
} /* wcmod_set_port_mode(wcmod_st* pc) */

int wcmod_pll_sequencer_control(wcmod_st* pc)   /* PLL_SEQUENCER_CONTROL */
{
  int cntl = pc->per_lane_control;
  int tmplane_num_ignore;

  tmplane_num_ignore = pc->lane_num_ignore;
  pc->lane_num_ignore = 1; /* write only to lane 1. */

  if (cntl == WCMOD_ENABLE) {
    /* enable PLL sequencer */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                          XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                          XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
    /* implement wait. Wait for 2 micro seconds... for PLL to lock */
    wcmod_pll_lock_wait(pc);
  } else if (cntl == WCMOD_PLL_ENABLE_NO_WAIT) {
    /* enable PLL sequencer without wait for the PLL lock */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                          XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                          XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
  } else {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc, 0,
                                    XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
  }
  pc->lane_num_ignore = tmplane_num_ignore;
  return SOC_E_MULT_REG;  /* No errors. Just no need to call multiple times */
}


int wcmod_rx_seq_control(wcmod_st* pc)         /* START_RX_SEQ or STOP_RX_SEQ */
{
  uint16 data, prevStat;

  int cntl = pc->per_lane_control;

  if (cntl & 0x10) {
    if        (pc->this_lane == 0) { /*************** LANE 0 *****************/
      SOC_IF_ERROR_RETURN(READ_WC40_RX0_ANARXCONTROLr(pc->unit,pc,&prevStat));

      /* enable sync status reporting in the status reg.*/
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX0_ANARXCONTROLr(pc->unit, pc,
                                     RX0_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                     RX0_ANARXCONTROL_STATUS_SEL_MASK));
      SOC_IF_ERROR_RETURN(READ_WC40_RX0_ANARXSTATUSr(pc->unit, pc, &data));
      if (data & RX0_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
        pc->accData = 1;
      } else {
        pc->accData = 0;
      }
      /* restore original contents for AnaRxStatus */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX0_ANARXCONTROLr(pc->unit, pc, prevStat,
                                       RX0_ANARXCONTROL_STATUS_SEL_MASK));
    } else if (pc->this_lane == 1) { /*************** LANE 1 *****************/
      SOC_IF_ERROR_RETURN(READ_WC40_RX1_ANARXCONTROLr(pc->unit,pc,&prevStat));

      /* enable sync status reporting in the status reg.*/
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX1_ANARXCONTROLr(pc->unit, pc,
                                     RX1_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                     RX1_ANARXCONTROL_STATUS_SEL_MASK));
      SOC_IF_ERROR_RETURN(READ_WC40_RX1_ANARXSTATUSr(pc->unit, pc, &data));
      if (data & RX1_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
        pc->accData = 1;
      } else {
        pc->accData = 0;
      }
      /* restore original contents for AnaRxStatus */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX1_ANARXCONTROLr(pc->unit, pc, prevStat,
                                       RX1_ANARXCONTROL_STATUS_SEL_MASK));
    } else if (pc->this_lane == 2) { /*************** LANE 2 *****************/
      SOC_IF_ERROR_RETURN(READ_WC40_RX2_ANARXCONTROLr(pc->unit,pc,&prevStat));

      /* enable sync status reporting in the status reg.*/
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX2_ANARXCONTROLr(pc->unit, pc,
                                     RX2_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                     RX2_ANARXCONTROL_STATUS_SEL_MASK));
      SOC_IF_ERROR_RETURN(READ_WC40_RX2_ANARXSTATUSr(pc->unit, pc, &data));
      if (data & RX2_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
        pc->accData = 1;
      } else {
        pc->accData = 0;
      }
      /* restore original contents for AnaRxStatus */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX2_ANARXCONTROLr(pc->unit, pc, prevStat,
                                       RX2_ANARXCONTROL_STATUS_SEL_MASK));
    } else if (pc->this_lane == 3) { /*************** LANE 3 *****************/
      SOC_IF_ERROR_RETURN(READ_WC40_RX3_ANARXCONTROLr(pc->unit,pc,&prevStat));

      /* enable sync status reporting in the status reg.*/
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX3_ANARXCONTROLr(pc->unit, pc,
                                     RX3_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                     RX3_ANARXCONTROL_STATUS_SEL_MASK));
      SOC_IF_ERROR_RETURN(READ_WC40_RX3_ANARXSTATUSr(pc->unit, pc, &data));
      if (data & RX3_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
        pc->accData = 1;
      } else {
        pc->accData = 0;
      }
      /* restore original contents for AnaRxStatus */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX3_ANARXCONTROLr(pc->unit, pc, prevStat,
                                       RX3_ANARXCONTROL_STATUS_SEL_MASK));
    }
  } else if (cntl & 0x1) { 
    SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc, 0,
                                        DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
  } else if (cntl == 0x0) {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc,
                                        DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK,
                                        DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
  } else {
    printf("%-22s: Error: invalid per_lane_control %d\n",__func__, cntl);
    return SOC_E_ERROR;
  }
  return SOC_E_NONE;
}


int wcmod_clause72_control(wcmod_st* pc)                /* CLAUSE_72_CONTROL */
{
  int blkCntl, uCntl;
  uint16 mask, data;
  int timeout=0;
  int lane_select ;
  const int UCODE_CMDREAD_TIMEOUT = 100;
  /*wcmod_spd_intfc_set spd_intf = pc->spd_intf; */

  blkCntl = pc->per_lane_control & 0x1;
  uCntl  = pc->per_lane_control & 0x10;


  /* first check if 6 canned speed */
  if ((pc->spd_intf == WCMOD_SPD_PCSBYP_10P3125) || (pc->spd_intf == WCMOD_SPD_PCSBYP_10P9375)
      || (pc->spd_intf == WCMOD_SPD_PCSBYP_11P5) || (pc->spd_intf == WCMOD_SPD_PCSBYP_12P5)
      || (pc->spd_intf == WCMOD_SPD_CUSTOM_BYPASS) || (pc->spd_intf == WCMOD_SPD_PCSBYP_6P25G)) {
    /* if enable, then set the firmware_mode to be cl72_woAN */
    /* Wait if it is not ready to take a cmd. */
    do {
      SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, &data));
    } while (!(data & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)
            && (++timeout < UCODE_CMDREAD_TIMEOUT));

    /* clear error bit */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0,
                                          (DSC1B0_UC_CTRL_ERROR_FOUND_MASK)));

    mask = (UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK       << (pc->this_lane * 4));
    data = (UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_CL72_NO_AN << (pc->this_lane * 4));
    if (blkCntl || uCntl) {
         MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(pc->unit, pc, data, mask);
    } else {
        /* restore the original firmware mode */
        data = (pc->firmware_mode) << (pc->this_lane * 4) ; 
        MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(pc->unit, pc, data, mask);
    }
  }

  lane_select = pc->lane_select ;
  if( pc->port_type == WCMOD_COMBO ) {
      /* for combo mode, do BCST */
      pc->lane_select = WCMOD_LANE_BCST ;
  } else {
      /* for dual port mode, do dual bcst as well */
      if( pc->dual_type ) {
          /* may not be needed */
      }
  }

  /* Block Control. Turn it on if requested */
  if (blkCntl || uCntl) {
    MODIFY_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(pc->unit, pc,
       PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK,
       PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK);
    data = CL72_USERB0_CL72_MISC2_CONTROL_DIS_MAX_WAIT_TIMER_FORCEVAL_MASK | 
           CL72_USERB0_CL72_MISC2_CONTROL_DIS_MAX_WAIT_TIMER_FORCE_MASK; 
    MODIFY_WC40_CL72_USERB0_CL72_MISC2_CONTROLr(pc->unit,pc,data, data);
    /* disable TXFIR force bit */
    MODIFY_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(pc->unit, pc, 0, 
        CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK);
  } else if (!blkCntl && !uCntl) {
    MODIFY_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(pc->unit,pc,0,
        PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK);
  }

  mask = CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK |
           CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK;

  if (blkCntl || uCntl) {
    MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(pc->unit, pc, mask, mask);
  } else if (!blkCntl && !uCntl) {
    MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(pc->unit, pc, 0, mask);
  }

  /* wait for cmd to be read by micro. i.e it is ready to take next cmd */
  timeout = 0;
  do {
    SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, &data));
  } while (!(data & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)
            && (++timeout < UCODE_CMDREAD_TIMEOUT));
  
  /* check for error */
  SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, &data));
  if (data & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) {
    printf("%-22s: Micro reported error\n", __func__);
  }

  pc->lane_select = lane_select ;
 
#ifdef _SDK_WCMOD_
    sal_usleep(1000);  /* 1ms */
#endif

  return SOC_E_NONE;
}


int wcmod_autoneg_control(wcmod_st* pc)
{
  uint16 data16 = 0, mask16 = 0;
  int cntl = (pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;
  
  if (pc->verbosity > 0)
      printf("%-22s: p=%0d cntl=%x per_lane_cntl=%x lane=%0d select=%x\n",__func__, 
             pc->port, cntl, pc->per_lane_control, pc->this_lane, pc->lane_select);

  if ((cntl & USE_CLAUSE_37_AN) || (cntl & USE_CLAUSE_37_AN_WITH_BAM)) {
    if (pc->verbosity > 0)
        printf("%-22s: p=%d cl37 autoneg_enable\n",__func__, pc->port);
    if((pc->model_type == WCMOD_XN) || (pc->model_type == WCMOD_QS_A0) || (pc->model_type == WCMOD_QS)) {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_IEEE0BLK_MIICNTLr(pc->unit, pc,
                                     IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK,
                                     IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK));
    } else {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                                    COMBO_IEEE0_MIICNTL_AUTONEG_ENABLE_MASK,
                                    COMBO_IEEE0_MIICNTL_AUTONEG_ENABLE_MASK));
    }
  } else {
    /* disable cl37 an. */
    if (pc->verbosity > 0)
        printf("%-22s: p=%0d cl37 autoneg_disable\n",__func__, pc->port);
    if((pc->model_type == WCMOD_XN) || (pc->model_type == WCMOD_QS_A0) || (pc->model_type == WCMOD_QS)) {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_IEEE0BLK_MIICNTLr(pc->unit, pc,
                            0, COMBO_IEEE0_MIICNTL_AUTONEG_ENABLE_MASK));
    } else  {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                                  0, COMBO_IEEE0_MIICNTL_AUTONEG_ENABLE_MASK));
    }
  }
  if ((cntl & USE_CLAUSE_73_AN) || (cntl & USE_CLAUSE_73_AN_WITH_BAM)) {
    if (pc->verbosity > 0)
        printf("%-22s: p=%0d cl73 autoneg_enable\n",__func__, pc->port);
    SOC_IF_ERROR_RETURN((MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(pc->unit, pc,
                AN_IEEE0BLK_AN_IEEECONTROL1_AN_ENABLE_MASK,
                AN_IEEE0BLK_AN_IEEECONTROL1_AN_ENABLE_MASK)));
    mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK; 
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(pc->unit, pc, mask16, mask16));  


    /* restart is called at the end of this function 
    SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(pc->unit, pc,
                AN_IEEE0BLK_AN_IEEECONTROL1_RESTARTAN_MASK,
                AN_IEEE0BLK_AN_IEEECONTROL1_RESTARTAN_MASK));
    */
  } else {
    /* disable cl73 an */
    if (pc->verbosity > 0)
        printf("%-22s: p=%0d cl73 autoneg_disable\n",__func__, pc->port);
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(pc->unit, pc,0,
                AN_IEEE0BLK_AN_IEEECONTROL1_AN_ENABLE_MASK));
    mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK; 
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(pc->unit, pc, 0, mask16)); 

  }

  if (cntl & USE_CLAUSE_73_AN_WITH_BAM) {
    if (pc->verbosity > 0)
        printf ("%-22s p=%0d Enabling cl73 with BAM\n",__func__, pc->port);
    data16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK |
             CL73_USERB0_CL73_BAMCTRL1_CL73_BAM_STATION_MNGR_EN_MASK;
    mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK |
             CL73_USERB0_CL73_BAMCTRL1_CL73_BAM_STATION_MNGR_EN_MASK;

    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(pc->unit, pc, data16, mask16));

  } else {
    if (pc->verbosity > 0)
        printf ("%-22s p=%0d Disabling cl73 with BAM.\n",__func__, pc->port);
    mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMNP_AFTER_BP_EN_MASK |
             CL73_USERB0_CL73_BAMCTRL1_CL73_BAM_STATION_MNGR_EN_MASK;
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(pc->unit, pc, mask16, mask16));
  }

  if (cntl & USE_CLAUSE_37_AN_WITH_BAM) {
    if (pc->verbosity > 0)
        printf ("%-22s p=%0d Enabling cl37+BAM\n",__func__, pc->port);
  data16 = DIGITAL6_MP5_NEXTPAGECTRL_BAM_MODE_MASK |
           DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_MASK |
           DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_UP3_EN_MASK;
  mask16 = DIGITAL6_MP5_NEXTPAGECTRL_BAM_MODE_MASK |
           DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_MASK |
           DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_UP3_EN_MASK;
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(pc->unit, pc, data16, mask16));
  } else {
    if (pc->verbosity > 0)
        printf ("%-22s p=%0d Disabling cl37+BAM\n",__func__, pc->port);
    mask16 = DIGITAL6_MP5_NEXTPAGECTRL_BAM_MODE_MASK   |
             DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_MASK |
             DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_UP3_EN_MASK;
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(pc->unit, pc, 0x0, mask16));
  }
/* if 73BAM and 37BAM is enabled together restart 37 AN only */
  if ((cntl & USE_CLAUSE_73_AN_WITH_BAM) && (cntl & USE_CLAUSE_37_AN_WITH_BAM)) {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                         COMBO_IEEE0_MIICNTL_RESTART_AUTONEG_MASK,
                         COMBO_IEEE0_MIICNTL_RESTART_AUTONEG_MASK));
  } else if ((cntl & USE_CLAUSE_73_AN) || (cntl & USE_CLAUSE_73_AN_WITH_BAM)) {
        SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(pc->unit, pc,
                    AN_IEEE0BLK_AN_IEEECONTROL1_RESTARTAN_MASK,
                    AN_IEEE0BLK_AN_IEEECONTROL1_RESTARTAN_MASK));
  } else if ((cntl & USE_CLAUSE_37_AN) || (cntl & USE_CLAUSE_37_AN_WITH_BAM)) {
        SOC_IF_ERROR_RETURN (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                             COMBO_IEEE0_MIICNTL_RESTART_AUTONEG_MASK,
                             COMBO_IEEE0_MIICNTL_RESTART_AUTONEG_MASK));
  }

  return SOC_E_NONE;
} /* wcmod_autoneg_control(wcmod_st* pc) */


int wcmod_parallel_detect_control(wcmod_st* pc)    /* PARALLEL_DETECT_CONTROL */
{
  uint16 data16, mask16;
  int cntl = pc->per_lane_control;

  if (cntl & 0x1) {
    if (pc->verbosity > 0) printf("%-22s: 1G parallel_detect_enable\n",__func__);
    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(pc->unit, pc,
                SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK,
                SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));
  } else {
    if (pc->verbosity > 0) printf("%-22s: 1G parallel_detect_disable\n",__func__);
    data16 = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_ALIGN <<
                            SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_SHIFT |
                            SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_BITS;
    mask16 = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc,
                                                               data16, mask16));
    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc,
                SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_ALIGN <<
                SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_SHIFT,
                SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK));

    data16 = SERDESDIGITAL_CONTROL1000X2_AUTONEG_FAST_TIMERS_BITS <<
                SERDESDIGITAL_CONTROL1000X2_AUTONEG_FAST_TIMERS_SHIFT |
                SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT;

    data16 &= ~SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;

    mask16= data16 | SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;

    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(pc->unit, pc, data16, mask16));
  }

  if (cntl & 0x2) {
    if (pc->verbosity > 0) printf("%-22s: 10G parallel_detect_enable\n",__func__);
    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(pc->unit, pc,
                XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_BITS,
                XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK));

  } else {
    if (pc->verbosity > 0) printf("%-22s: 10G parallel_detect_disable\n",__func__);

    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(pc->unit, pc,
                XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_ALIGN,
                XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK));
  }
  return SOC_E_NONE;
}

int wcmod_100fx_control (wcmod_st* pc)
{
  uint16 data16, mask16;

  int cntl=(pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;
  if (cntl) {
    if (pc->verbosity > 0) printf ("%-22s: 100fx set\n", __func__);
      data16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;

      MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, data16,
                            SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK);

      mask16 = FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
               FX100_CONTROL1_FULL_DUPLEX_MASK |
               FX100_CONTROL1_ENABLE_MASK;
      data16 = mask16;
      MODIFY_WC40_FX100_CONTROL1r(pc->unit, pc, data16, mask16);

  } else {
    /* disable 100FX and 100FX auto-detect */
    SOC_IF_ERROR_RETURN
     (MODIFY_WC40_FX100_CONTROL1r(pc->unit, pc, 0, FX100_CONTROL1_ENABLE_MASK));
    /* disable 100FX idle detect */
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_FX100_CONTROL3r(pc->unit, pc,
                   FX100_CONTROL3_CORRELATOR_DISABLE_MASK,
                   FX100_CONTROL3_CORRELATOR_DISABLE_MASK));
  }
  return SOC_E_NONE;
}

int wcmod_speed_intf_set(wcmod_st* pc)                  /* SPEED_INTF_SET */
{
  uint16   data16 = 0, mask16;
  uint16   speed_val, speed_mii, mask, this_lane, lane_num_ignore;
  uint16   b6, b5 ;  /* force_speed b5 and b6 */
  wcmod_spd_intfc_set spd_intf;

  spd_intf    = pc->spd_intf;
  this_lane   = pc->this_lane;
  speed_val   = 0;
  speed_mii   = 0;
  mask        =  SERDESDIGITAL_MISC1_FORCE_SPEED_MASK;
  b6          = 0x40 ;
  b5          = 0x20 ;

/* 1: Set ieee_blksel_val = miscControl1B[0] = 1'b1 (miscControl1_A == 16'h0001)
 * 1 not needed for 1000 fiber, but needed later on.
 * 2. Set force_speed[5:0] reg to desired value
 */

  /* this only works for combo mode right now. */
  if (pc->spd_intf > WCMOD_SPD_ILLEGAL) {
    printf("%-22s Error: Bad spd-intf: %d > WCMOD_SPD_ILLEGAL\n",
                                                        __func__, pc->spd_intf);
    return SOC_E_ERROR;
  } else {
     if (pc->verbosity > 0) printf("%-22s: %s[%d]\n", __func__, e2s_wcmod_spd_intfc_set[pc->spd_intf],
                                        e2n_wcmod_spd_intfc_set[pc->spd_intf]);
  }

  /* first disable pll force bit */
  SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0x0,
              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK));   

  if (pc->accAddr != PMA_PMD_SPEED_ENFORCE) {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0x0,
                  SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK)); 
  }
  /* this is only for CD3 */
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc, 0,
                                           DIGITAL4_MISC3_LANEDISABLE_MASK));

  /* disable 100FX auto-detect */
  MODIFY_WC40_FX100_CONTROL1r(pc->unit,pc,0,
                              FX100_CONTROL1_AUTO_DETECT_FX_MODE_MASK |
                              FX100_CONTROL1_ENABLE_MASK);

  /* for warpLite clear force os enable bit */
  if ((pc->model_type == WCMOD_WL_A0) || (pc->model_type == WCMOD_WL_B0)) {
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL5_MISC7r(pc->unit, pc, 0, 
                                                    DIGITAL5_MISC7_FORCE_OSCDR_MODE_MASK)); 
  }

  if        (spd_intf == WCMOD_SPD_10_SGMII)   {
      speed_mii = 0; /*MII_CTRL_SS_10; */
  } else if (spd_intf == WCMOD_SPD_100_SGMII)  {
      speed_mii = IEEE0BLK_MIICNTL_MANUAL_SPEED0_BITS <<
          IEEE0BLK_MIICNTL_MANUAL_SPEED0_SHIFT; /* MII_CTRL_SS_100; */
  } else if (spd_intf == WCMOD_SPD_100_FX)     {
      speed_mii = 0; /* MII_CTRL_SS_10 */
  } else if (spd_intf == WCMOD_SPD_1000_SGMII) {
      speed_mii = IEEE0BLK_MIICNTL_MANUAL_SPEED1_BITS <<
          IEEE0BLK_MIICNTL_MANUAL_SPEED1_SHIFT; /* MII_CTRL_SS_1000; */
  } else if (spd_intf == WCMOD_SPD_1000_FIBER) {
      speed_mii = PMD_IEEE0BLK_PMD_IEEECONTROL1_SPEEDSELECTION1_MASK;
  } else if (spd_intf == WCMOD_SPD_2500) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_2500BRCM_X1;
  } else if (spd_intf == WCMOD_SPD_5000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_5000BRCM_X4;
  } else if (spd_intf == WCMOD_SPD_6000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_6000BRCM_X4;
  } else if (spd_intf == WCMOD_SPD_10000_HI) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_10000) {
       if (pc->model_type == WCMOD_XN) {
          speed_val = 0x14;
       } else {
          speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10GBASE_CX4;
       }
  } else if (spd_intf == WCMOD_SPD_12000_HI) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_12500) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p5GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_13000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_13GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_15000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_15GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_16000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_16GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_20000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_10000_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_DXGXS ;
  } else if (spd_intf == WCMOD_SPD_10000_DXGXS_SCR) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_DXGXS_SCR ;
  } else if (spd_intf == WCMOD_SPD_10500_HI_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p5G_HiG_DXGXS ;
  } else if (spd_intf == WCMOD_SPD_10500_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p5G_DXGXS;
  } else if (spd_intf == WCMOD_SPD_12773_HI_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p773G_HiG_DXGXS ;
  } else if (spd_intf == WCMOD_SPD_12773_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p773G_DXGXS ;
  } else if ((spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5))  {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_XFI ;
  } else if (spd_intf == WCMOD_SPD_10000_SFI) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_SFI ;
  } else if (spd_intf == WCMOD_SPD_10600_XFI_HG) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_XFI ;  
      /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_XFI instead */ 
  } else if (spd_intf == WCMOD_SPD_40G) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_X4 ;
  } else if (spd_intf == WCMOD_SPD_42G) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_X4 ;   
      /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_X4 instead */ 
  } else if (spd_intf == WCMOD_SPD_20G_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_DXGXS ;
  } else if (spd_intf == WCMOD_SPD_31500) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_31p5G ;
  } else if (spd_intf == WCMOD_SPD_20000_SCR) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_SCR ;
  } else if (spd_intf == WCMOD_SPD_10000_HI_DXGXS_SCR) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_HiG_DXGXS_SCR ;
  } else if (spd_intf == WCMOD_SPD_R2_12000)  {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12G_R2 ;
  } else if (spd_intf == WCMOD_SPD_X2_10000)  {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_X2 ;
  } else if (spd_intf == WCMOD_SPD_X2_23000)  {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_X2 ;
  } else if((spd_intf == WCMOD_SPD_40G_KR4)  || (spd_intf == WCMOD_SPD_42G_KR4)
          ||(spd_intf == WCMOD_SPD_40G_XLAUI) ||(spd_intf == WCMOD_SPD_42G_XLAUI)) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_KR4 ; 
      /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_KR4 for all of those */
  } else if (spd_intf == WCMOD_SPD_40G_CR4) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_CR4 ;
  } else if (spd_intf == WCMOD_SPD_10000_HI_DXGXS) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_HiG_DXGXS;
  } else if (spd_intf == WCMOD_SPD_15750_HI_DXGXS)  {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_15p75_HiG_DXGXS;
  } else if (spd_intf == WCMOD_SPD_21000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_21GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_25455) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_25p45GHiGig_X4;
  } else if ((spd_intf == WCMOD_SPD_20000_HI_DXGXS)
          || (spd_intf == WCMOD_SPD_21G_HI_DXGXS)) {
      /* 21G is same as 20G, plldiv=70 */   /* 20G MLD */ 
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_HiG_DXGXS ; 
      /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_HiG_DXGXS for both */
  } else if ( (spd_intf == WCMOD_SPD_20G_KR2)) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_KR2 ;
  } else if ( (spd_intf == WCMOD_SPD_20G_CR2)) {
      speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_CR2 ;
  } else if (spd_intf == WCMOD_SPD_PCSBYP_3P125G)  {
    if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
        speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_3p125G ;
    } else {
      printf("%-22s: Error: This speed allowed only for C0/WL model type %d\n", __func__, pc->model_type);
      return SOC_E_ERROR;
    }
  } else if (spd_intf == WCMOD_SPD_PCSBYP_6P25G)  {
    if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
      /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
      MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
      MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
        SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
      speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_6p25G ;
    } else {
      printf("%-22s: Error: This speed allowed only for C0/WL\n", __func__);
      return SOC_E_ERROR;
    }
  } else if (spd_intf == WCMOD_SPD_PCSBYP_10P3125)  {
    if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
      /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
      MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
      MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
        SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
      /* this set of writes added for 100G only.*/ /* PP */
      WRITE_WC40_XGXSBLK1_LANECTRL0r(pc->unit, pc, 0x0);
      WRITE_WC40_XGXSBLK1_LANECTRL2r(pc->unit, pc, 0x0);
      data16 = XGXSBLK8_TXLNSWAP1_FORCE_GB_BYPASS_EN_MASK;
      mask16 = XGXSBLK8_TXLNSWAP1_FORCE_GB_BYPASS_EN_MASK;
      MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(pc->unit, pc, data16, mask16);
      data16 = (CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_QUAD_MODE << 
                CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_MODE_SEL_SHIFT) |
               (CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_16_QUAD_MODE <<
                CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_16_MODE_SEL_SHIFT) |
               (CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK33_QUAD_MODE <<
                CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK33_MODE_SEL_SHIFT);
      mask16 = CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_MODE_SEL_MASK |
               CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_16_MODE_SEL_MASK |
               CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK33_MODE_SEL_MASK;
      MODIFY_WC40_CL72_USERB0_CL72_MISC4_CONTROLr(pc->unit,pc,data16,mask16);
      data16 = XGXSBLK6_XGXSX2CONTROL2_100G_BIT_MASK;
      mask16 = XGXSBLK6_XGXSX2CONTROL2_100G_BIT_MASK;
      MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(pc->unit, pc, data16, mask16);
      speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p3125G ;
    }
  } else if (spd_intf == WCMOD_SPD_PCSBYP_10P9375)  {
      if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
          /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
          MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
          MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
            SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
          speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p9375G ;
      } else {
          printf("%-22s: Error: This speed allowed only for C0/WL\n", __func__);
          return SOC_E_ERROR;
      }
  } else if ((spd_intf==WCMOD_SPD_PCSBYP_11P5) ||
             (spd_intf==WCMOD_SPD_PCSBYP_5P75G))  {
      if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
          /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
          MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
          MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
            SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
          speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_11p5G ;   
          /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_11p5G for both speed */
      } else {
          printf("%-22s: Error: This speed allowed only for C0/WL\n", __func__);
          return SOC_E_ERROR;
      }
  } else if (spd_intf==WCMOD_SPD_PCSBYP_12P5) {
      if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
          /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
          MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
          MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
            SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
          speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p5G ;
      } else {
          printf("%-22s: Error: This speed allowed only for C0/WL\n", __func__);
          return SOC_E_ERROR;
      }
  } else if ((spd_intf==WCMOD_SPD_PCSBYP_11P5) ||
             (spd_intf==WCMOD_SPD_PCSBYP_5P75G))  {
      if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
          /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
          MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
          MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
            SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
          speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_11p5G ;
          /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_11p5G for both speed */
      } else {
       printf("%-22s: Error: This speed allowed only for C0/WL\n", __func__);
       return SOC_E_ERROR;
      }
  } else if (spd_intf == WCMOD_SPD_CUSTOM_BYPASS){
      if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
          speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_pcs_bypass ;
      } else {
          printf("%-22s: Error: This speed allowed only for C0/WL\n", __func__);
          return SOC_E_ERROR;
      }
  }
  if (spd_intf == WCMOD_SPD_1000_FIBER) {
    data16 = SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    mask16 = SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK | 
             SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, data16, mask16);
  } else if ((spd_intf==WCMOD_SPD_10_SGMII) ||
             (spd_intf == WCMOD_SPD_100_SGMII) ||
             (spd_intf == WCMOD_SPD_1000_SGMII)) {
    data16 = SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK;
    mask16 = SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK | 
             SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK;
    MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, data16, mask16);
    data16 = 0;
    mask16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, data16, mask16);
  }
  if (spd_intf == WCMOD_SPD_100_FX) {

    /* disable 100FX auto-detect */
    MODIFY_WC40_FX100_CONTROL1r(pc->unit,pc,0,
                              FX100_CONTROL1_AUTO_DETECT_FX_MODE_MASK);

    /* disable 100FX idle detect */
    SOC_IF_ERROR_RETURN
            (MODIFY_WC40_FX100_CONTROL3r(pc->unit,pc,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK));

    /* fiber mode 100fx, enable */
    MODIFY_WC40_FX100_CONTROL1r(pc->unit,pc,
                      FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                      FX100_CONTROL1_ENABLE_MASK,
                      FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                      FX100_CONTROL1_ENABLE_MASK);

    /* enable 100fx extended packet size */
    MODIFY_WC40_FX100_CONTROL2r(pc->unit,pc,
                    FX100_CONTROL2_EXTEND_PKT_SIZE_MASK,
                    FX100_CONTROL2_EXTEND_PKT_SIZE_MASK);
  }

  if (pc->port_type == WCMOD_COMBO) { /*************** COMBO *****************/
    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit,pc,speed_val,
                                        SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));

    if (pc->model_type != WCMOD_XN) {
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,
                      (speed_val & 0x20)?  DIGITAL4_MISC3_FORCE_SPEED_B5_MASK:0,
                      DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));
    }

    /* if (pc->model_type == WCMOD_XN) {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_IEEE0BLK_MIICNTLr(pc->unit, pc, speed_mii,
                                  (IEEE0BLK_MIICNTL_MANUAL_SPEED1_MASK |
                                   IEEE0BLK_MIICNTL_MANUAL_SPEED0_MASK)));
    } else */{
     SOC_IF_ERROR_RETURN(MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit,pc,speed_mii,
                                  (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                   COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK)));
    }
    /* set sgmii mode */
    if ((spd_intf == WCMOD_SPD_10_SGMII) || (spd_intf == WCMOD_SPD_100_SGMII) ||
        (spd_intf == WCMOD_SPD_1000_SGMII)) {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit,pc,
                         0, SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
    }

    /*added the support for 2.5G QSGMII support */
    if ((spd_intf == WCMOD_SPD_2500) && ((pc->model_type == WCMOD_QS_A0) ||(pc->model_type == WCMOD_QS_B0))) {
        /*use the combo mode for OS mode */
      if ((pc->this_lane == 0) || (pc->this_lane == 4))  {
          SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                                                                 (XGXSBLK0_XGXSCONTROL_MODE_10G_ComboCoreMode <<
                                                                  XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                                                                 XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));
        /*disable the QSGMII mode for both tx/rx only lane 0 or lane 4 is valid for this write*/
          SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK1_LANECTRL0r(pc->unit, pc, 0x0, 0xff00));
      }
      /* disbale the fiber mode also */
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, 0x1,
                                                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));   
    }

    if ((spd_intf == WCMOD_SPD_1000_SGMII) || (spd_intf == WCMOD_SPD_1000_FIBER)){
      if (pc->model_type == WCMOD_XN) {
    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_BITS <<
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_SHIFT |
                 SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                 SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div16_XENIA <<
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
      } else {
    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_BITS <<
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_SHIFT |
                 SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                 SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 <<
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
      }
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                data16, mask));
    } /* 1G SGMII or fiber */

    /* Force pll mode = a for all lanes in XLAUI mode */
    if (   (spd_intf == WCMOD_SPD_42G_XLAUI) || (spd_intf == WCMOD_SPD_40G_KR4)
        || (spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_40G)) {
      lane_num_ignore = pc->lane_num_ignore; /* save and restore it below */
      pc->lane_num_ignore = 1;

      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                data16, mask));
      pc->lane_num_ignore = lane_num_ignore; /* restore */
    }

    if ((spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_42G_XLAUI)) {
      data16 = UC_INFO_B1_FIRMWARE_MODE_LN3_MODE_XLAUI << UC_INFO_B1_FIRMWARE_MODE_LN3_MODE_SHIFT | 
               UC_INFO_B1_FIRMWARE_MODE_LN2_MODE_XLAUI << UC_INFO_B1_FIRMWARE_MODE_LN2_MODE_SHIFT | 
               UC_INFO_B1_FIRMWARE_MODE_LN1_MODE_XLAUI << UC_INFO_B1_FIRMWARE_MODE_LN1_MODE_SHIFT | 
               UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_XLAUI;
      WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(pc->unit, pc, data16);
    }

    /* set divider for 42G kr4 and 42G XLAUI */
    if ((spd_intf == WCMOD_SPD_42G_KR4) || (spd_intf == WCMOD_SPD_42G_XLAUI) ||
        (spd_intf == WCMOD_SPD_42G)) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                data16, mask));
    }
    /* set divider for 48G XLAUI */
    if (spd_intf == WCMOD_SPD_48G) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div80 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                data16, mask));
    }
    if (spd_intf == WCMOD_SPD_10600_XFI_HG) { /* PP, need for combo mode */
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    }

    /* in XFI mode set 40-bit interface */
    if ((spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_10600_XFI_HG) ||
       (spd_intf == WCMOD_SPD_10000_SFI) || (spd_intf == WCMOD_SPD_11P5) ||
       (spd_intf == WCMOD_SPD_12P5)) {
      MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,DIGITAL4_MISC3_IND_40BITIF_MASK,
                                               DIGITAL4_MISC3_IND_40BITIF_MASK);
    }

    /* When speed is speed_12000_hi, speed_25455, speed_31520, speed_40g the */
    /* txcko_div is set to 1'b0 (i.e. xgxsControl[0] = 1'b0) */

    if ((spd_intf == WCMOD_SPD_10000_HI) || (spd_intf == WCMOD_SPD_10000) ||
        (spd_intf == WCMOD_SPD_12000_HI) || (spd_intf == WCMOD_SPD_13000) ||
        (spd_intf == WCMOD_SPD_16000) || (spd_intf == WCMOD_SPD_20000)) {
      /* Xenia needs to have clock divided by 2 for 10G Xaui */
      if ((spd_intf == WCMOD_SPD_10000) &&
          (getGenericModelType(pc->model_type) == WCMOD_XN)) {
        /* SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                                          XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK,
            XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK)); */
      } else {
        SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,0,
                                        XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK));
      }
    }

    /*--------------------------------------------------------------------------
     * The following code configures the 64B/66B decoder when speed is forced
     * to speed_25455 or speed_12773_dxgxs or speed_31500 or speed_40g.
     *------------------------------------------------------------------------*/

    if ((spd_intf == WCMOD_SPD_25455) || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_40G)  || (spd_intf == WCMOD_SPD_20000_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_20G_DXGXS ) ||
        (spd_intf == WCMOD_SPD_42G)  || 
        (spd_intf == WCMOD_SPD_12773_HI_DXGXS) || (spd_intf == WCMOD_SPD_12773_DXGXS) || 
        (spd_intf == WCMOD_SPD_31500) || (spd_intf == WCMOD_SPD_21G_HI_DXGXS)) {

#ifdef _SDK_WCMOD_
      int i;
      /* temp work around for writing to all lanes */
      for (i=0; i<4; i++) {
        pc->this_lane = i;
        WRITE_WC40_RX66_SCW0r(pc->unit, pc, 0xE070);
        WRITE_WC40_RX66_SCW1r(pc->unit, pc, 0xC0D0);
        WRITE_WC40_RX66_SCW2r(pc->unit, pc, 0xA0B0);
        WRITE_WC40_RX66_SCW3r(pc->unit, pc, 0x8090);
        WRITE_WC40_RX66_SCW0_MASKr(pc->unit, pc, 0xF0F0);
        WRITE_WC40_RX66_SCW1_MASKr(pc->unit, pc, 0xF0F0);
        WRITE_WC40_RX66_SCW2_MASKr(pc->unit, pc, 0xF0F0);
        WRITE_WC40_RX66_SCW3_MASKr(pc->unit, pc, 0xF0F0);
      }
      pc->this_lane = this_lane ; 
#else

      SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(pc->unit, pc, pc->aer_bcst_ofs_strap));

      WRITE_WC40_RX66_SCW0r(pc->unit, pc, 0xE070);
      WRITE_WC40_RX66_SCW1r(pc->unit, pc, 0xC0D0);
      WRITE_WC40_RX66_SCW2r(pc->unit, pc, 0xA0B0);
      WRITE_WC40_RX66_SCW3r(pc->unit, pc, 0x8090);
      WRITE_WC40_RX66_SCW0_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW1_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW2_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW3_MASKr(pc->unit, pc, 0xF0F0);
#endif

      SOC_IF_ERROR_RETURN (WRITE_WC40_AERBLK_AERr(pc->unit, pc, 0));
    }
    /* set BRCM 31G control */
    if (spd_intf == WCMOD_SPD_31500) {
#ifdef _SDK_WCMOD_
        int i;
        /* temp work around for writing to all lanes */
        for (i=0; i<4; i++) {
            pc->this_lane = i;
            SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_DIGITAL5_MISC6r(pc->unit, pc, 
                           DIGITAL5_MISC6_USE_BRCM6466_31500_CYA_MASK, 
                           DIGITAL5_MISC6_USE_BRCM6466_31500_CYA_MASK));
        }
        pc->this_lane = this_lane ;  
#endif
    }
    /* set the fiber mode (bit 0) in CONTROL11000X1 reg. */
  } else if (pc->port_type == WCMOD_INDEPENDENT) { /******* INDEPENDENT *******/

    if ( pc->model_type == WCMOD_XN &&  _wcmod_int_st.asymmetric_mode) {
      _wcmod_asymmetric_mode(pc);
    }

    /* clear the force pll mode for XENIA core */
    if ((pc->model_type == WCMOD_XN)) {
        /* first program the 0x8308 to 0x7110 */
        MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0, 
                            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK); 
        /* next disable the asymmetric bit in 0x8169 */
        MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(pc->unit, pc, 0, 
                            XGXSBLK8_TXLNSWAP1_ASYMMETRIC_MODE_EN_MASK);
    }

    /*work around to fix Arad odd lane link flapping */
    if ((spd_intf == WCMOD_SPD_1000_SGMII) || (spd_intf == WCMOD_SPD_1000_FIBER) || 
          (spd_intf == WCMOD_SPD_2500)) {
        if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
          SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC5r(pc->unit, pc,
                                                       (speed_val & 0xc0), 0xc0));
        }  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,
                          (speed_val & 0x20)?  DIGITAL4_MISC3_FORCE_SPEED_B5_MASK:0,
                           DIGITAL4_MISC3_FORCE_SPEED_B5_MASK)); 
        SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                speed_val, SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
    } else {
        SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                speed_val, SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));

        SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,
                          (speed_val & 0x20)?  DIGITAL4_MISC3_FORCE_SPEED_B5_MASK:0,
                           DIGITAL4_MISC3_FORCE_SPEED_B5_MASK)); 
        if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
          SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC5r(pc->unit, pc,
                                                       (speed_val & 0xc0), 0xc0));
        } 
    }  

    /*added the support of HX4 QUAD SGMII 1G  mode */
    if (pc->model_type == WCMOD_QS) {
      /* disbale the fiber mode also */
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, 0x0,
                                                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));   
    }

    /*added the support of HX4 QSGMII 1G  mode */
    if ((spd_intf == WCMOD_SPD_1000_SGMII) && ((pc->model_type == WCMOD_QS_A0) ||(pc->model_type == WCMOD_QS_B0))) {
      if ((pc->this_lane == 0) || (pc->this_lane == 4))  {
        /*use the independent mode for OS mode */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                                                             (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS6 <<
                                                              XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                                                             XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));
        /*enable the QSGMII mode for both tx/rx  only lane 0 or lane 4 is valid*/
        SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK1_LANECTRL0r(pc->unit, pc, 0xff00, 0xff00));
      }
      /* disbale the fiber mode also */
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, 0x0,
                                                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));   
    }

    /*QSGMII block has not analog, just PCS layer  */
    if (spd_intf == WCMOD_SPD_1G_QSGMII) {
      /* Speed added to hit each QSGMII since its register for broadcasting is different. */

      data16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_BITS  << SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_ALIGN |
        SERDESDIGITAL_CONTROL1000X1_DISABLE_SIGNAL_DETECT_FILTER_BITS  << SERDESDIGITAL_CONTROL1000X1_DISABLE_SIGNAL_DETECT_FILTER_SHIFT |
        0 << SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_SHIFT;
      mask16 =  SERDESDIGITAL_CONTROL1000X1_DISABLE_SIGNAL_DETECT_FILTER_MASK | SERDESDIGITAL_CONTROL1000X1_DISABLE_SIGNAL_DETECT_FILTER_MASK |
        SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK;      
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, data16, mask16));

      data16 = SERDESDIGITAL_CONTROL1000X2_AUTONEG_FAST_TIMERS_BITS << SERDESDIGITAL_CONTROL1000X2_AUTONEG_FAST_TIMERS_SHIFT;
      mask16 = SERDESDIGITAL_CONTROL1000X2_AUTONEG_FAST_TIMERS_MASK;
      SOC_IF_ERROR_RETURN
        (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(pc->unit, pc, data16, mask16));

      data16 = IEEE0BLK_AUTONEGADV_FULL_DUPLEX_BITS << IEEE0BLK_AUTONEGADV_FULL_DUPLEX_SHIFT;
      mask16 = IEEE0BLK_AUTONEGADV_FULL_DUPLEX_MASK;  
      SOC_IF_ERROR_RETURN
        (MODIFY_WC40_IEEE0BLK_AUTONEGADVr(pc->unit, pc, data16, mask16));

      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                                                             (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS6 <<
                                                              XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                                                             XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

      SOC_IF_ERROR_RETURN(MODIFY_WC40_IEEE0BLK_MIICNTLr(pc->unit, pc, 
                                                        IEEE0BLK_MIICNTL_MANUAL_SPEED1_BITS <<  IEEE0BLK_MIICNTL_MANUAL_SPEED1_SHIFT |
                                                        IEEE0BLK_MIICNTL_FULL_DUPLEX_BITS  << IEEE0BLK_MIICNTL_FULL_DUPLEX_SHIFT |
                                                        IEEE0BLK_MIICNTL_AUTONEG_ENABLE_BITS << IEEE0BLK_MIICNTL_PWRDWN_SW_SHIFT |
                                                        0 << IEEE0BLK_MIICNTL_MANUAL_SPEED0_SHIFT ,
                                                        (IEEE0BLK_MIICNTL_MANUAL_SPEED1_MASK | IEEE0BLK_MIICNTL_FULL_DUPLEX_MASK |
                                                         IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK | IEEE0BLK_MIICNTL_MANUAL_SPEED0_MASK)));
    }

    if ((pc->model_type == WCMOD_XN) || (pc->model_type == WCMOD_QS)) {
        SOC_IF_ERROR_RETURN(MODIFY_WC40_IEEE0BLK_MIICNTLr(pc->unit, pc, speed_mii,
                                  (IEEE0BLK_MIICNTL_MANUAL_SPEED1_MASK |
                                    IEEE0BLK_MIICNTL_MANUAL_SPEED1_MASK)));
    } else {
        MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc, speed_mii,
                                    (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                      COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK));
    }

    /* turn on cl-36 pcs (for xenia, may be implemented via straps in WC) */
    if ((spd_intf == WCMOD_SPD_1000_SGMII) || (spd_intf == WCMOD_SPD_1000_FIBER) ||
        (spd_intf == WCMOD_SPD_2500)) {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK1_LANECTRL0r(pc->unit, pc,
                 XGXSBLK1_LANECTRL0_CL36_PCS_EN_RX_MASK |
                 XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK,
                 XGXSBLK1_LANECTRL0_CL36_PCS_EN_RX_MASK |
                 XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK));

     /* set up rx mac clock frequency */
     /*data16 = 0;
     mask = 0;
     switch(pc->this_lane) {
     case 0:
       mask |=   XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_MASK
           | XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK;
       data16 |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SWSDR_div1
               << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT;
       break;
     case 1:
       mask |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_MASK |
           XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_MASK;
       data16 |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SWSDR_div1
                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT;    
       break;
     case 2:
       mask |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_MASK
            | XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_MASK;
       data16 |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SWSDR_div1
                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT;
       break;
     case 3:
       mask |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_MASK
                | XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_MASK;
       data16 |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SWSDR_div1
                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT;
       break;
     }
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16, mask);

     MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                      SERDESDIGITAL_MISC1_FORCE_LN_MODE_MASK,
                      SERDESDIGITAL_MISC1_FORCE_LN_MODE_MASK);*/
    }
    if((pc->model_type == WCMOD_QS_A0) && ( pc->spd_intf == WCMOD_SPD_1G_QSGMII)) {
      MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(pc->unit, pc, 0,
                          SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_MASK |
                          SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_MASK);
      /* enable full duplex */
      MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                        IEEE0BLK_MIICNTL_FULL_DUPLEX_MASK,
                        IEEE0BLK_MIICNTL_FULL_DUPLEX_MASK);
      /* disable lane disable */
      MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,0,
                                              DIGITAL4_MISC3_LANEDISABLE_MASK);
    }

    if (spd_intf == WCMOD_SPD_PCSBYP_5P75G) {
      /* SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div32 is no longer 32. It is 46.
       * By setting the clkmul to 46, the 20 bit interface is also setup */
      data16 = ((SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_BITS
              << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_SHIFT) |
               (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz
            << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
               (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div32
            << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    }
    /* for 12.5G XFI with PCS, we need to force the pll divider */
    if (spd_intf == WCMOD_SPD_12P5) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div80 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
      /* need to set the PLL rate */
      data16 = 28;
      SOC_IF_ERROR_RETURN(MODIFY_WC40_UC_INFO_B1_REGBr(pc->unit, pc, data16 <<
                      UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_SHIFT, UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_MASK));

      /* use firmware mode OSDFE */
      data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_LONG_CH_6G;
      data16  <<= (pc->this_lane * 4);
      mask16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK << (pc->this_lane * 4);
      SOC_IF_ERROR_RETURN
          (MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(pc->unit,pc,data16,mask16));

    }

    /* for 11.5G XFI with PCS at 125M, we need to force the pll divider */
    if (spd_intf == WCMOD_SPD_11P5) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div92 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
      /* need to set the PLL rate */
      data16 = 24;
      SOC_IF_ERROR_RETURN(MODIFY_WC40_UC_INFO_B1_REGBr(pc->unit, pc, data16 <<
                      UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_SHIFT, UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_MASK));

      /* use firmware mode OSDFE */
      data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_LONG_CH_6G;
      data16  <<= (pc->this_lane * 4);
      mask16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK << (pc->this_lane * 4);
      SOC_IF_ERROR_RETURN
          (MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(pc->unit,pc,data16,mask16));
    }

    if (spd_intf == WCMOD_SPD_10600_XFI_HG) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    } else if (spd_intf == WCMOD_SPD_15750_HI_DXGXS) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                       SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
               (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div52 <<
                                SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT);
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    } else if (spd_intf == WCMOD_SPD_21G_HI_DXGXS) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
               (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT);
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    }

    if((spd_intf == WCMOD_SPD_2500)       || (spd_intf == WCMOD_SPD_10000_XFI)    ||
       (spd_intf == WCMOD_SPD_10000_SFI)  || (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
       (spd_intf == WCMOD_SPD_R2_12000)   || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
       (spd_intf == WCMOD_SPD_10600_XFI_HG) || (spd_intf == WCMOD_SPD_21G_HI_DXGXS) ||
       (spd_intf == WCMOD_SPD_20G_DXGXS)  || (spd_intf == WCMOD_SPD_20000_HI_DXGXS)){
      MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, 
                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK,
                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK);
      /* set full duplex */
      MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                        IEEE0BLK_MIICNTL_FULL_DUPLEX_MASK,
                        IEEE0BLK_MIICNTL_FULL_DUPLEX_MASK);

      mask = XGXSBLK6_XGXSX2CONTROL2_TXCKOUT33_OVERRIDE_MASK;
      MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(pc->unit, pc, mask, mask);
      if (pc->verbosity > 0) printf ("%-22s: txckout33_override bit is set\n",__func__);
    }

    /* 20G dual port mode PLL div and clk156p25 selected to ensure plldiv value  */
    if((spd_intf == WCMOD_SPD_20000_HI_DXGXS) || (spd_intf == WCMOD_SPD_20G_KR2) || 
       (spd_intf == WCMOD_SPD_20G_CR2) || (spd_intf == WCMOD_SPD_20G_DXGXS)) {
        
       data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
          (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
           SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
          (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66 <<
           SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT);
       mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
          SERDESDIGITAL_MISC1_REFCLK_SEL_MASK | 
          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
       
       SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                             data16, mask));
    }
    
    /*--------------------------------------------------------------------------
     * The following code configures the 64B/66B decoder when speed is forced
     * to speed_25455 or speed_12773_dxgxs or speed_31500 or speed_40g.
     *------------------------------------------------------------------------*/

    if ((spd_intf == WCMOD_SPD_25455) || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_40G)  || (spd_intf == WCMOD_SPD_20000_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_42G)  ||
        (spd_intf == WCMOD_SPD_31500) || (spd_intf == WCMOD_SPD_21G_HI_DXGXS)) {
 
      WRITE_WC40_RX66_SCW0r(pc->unit, pc, 0xE070);
      WRITE_WC40_RX66_SCW1r(pc->unit, pc, 0xC0D0);
      WRITE_WC40_RX66_SCW2r(pc->unit, pc, 0xA0B0);
      WRITE_WC40_RX66_SCW3r(pc->unit, pc, 0x8090);
      WRITE_WC40_RX66_SCW0_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW1_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW2_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW3_MASKr(pc->unit, pc, 0xF0F0);
    }

    /* in XFI mode set 40-bit interface */
    if ((spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_10600_XFI_HG) ||
        (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) || 
        (spd_intf == WCMOD_SPD_10000_SFI)) {
      MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,DIGITAL4_MISC3_IND_40BITIF_MASK,
                                               DIGITAL4_MISC3_IND_40BITIF_MASK);
    }
    /* CONFIGURE MAC INTERFACE: MAKE A SEPERATE PROCESS */
    /* set mac_int_rxck_override */
    if ((pc->port_type == WCMOD_INDEPENDENT) && ((spd_intf == WCMOD_SPD_R2_12000) ||
        (spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_10600_XFI_HG) || (spd_intf == WCMOD_SPD_10000_SFI) ||
        (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
        (spd_intf == WCMOD_SPD_20000_HI_DXGXS) || (spd_intf == WCMOD_SPD_20G_DXGXS) ||
        (spd_intf == WCMOD_SPD_21G_HI_DXGXS))){
      mask = XGXSBLK6_XGXSX2CONTROL2_MAC_INF_RXCK_OVERRIDE_MASK;
      MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(pc->unit, pc, mask, mask);
      if (pc->verbosity > 0) 
        printf ("%-22s: mac_inf_rxck_override bit is set\n",__func__);
    }

    if ((pc->port_type == WCMOD_INDEPENDENT) & (spd_intf == WCMOD_SPD_R2_12000)) {
    }

    if ((pc->os_type == WCMOD_OS8) && (  (spd_intf == WCMOD_SPD_10000_XFI) ||
        (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
        (spd_intf == WCMOD_SPD_10000_SFI) || (spd_intf == WCMOD_SPD_10600_XFI_HG))) {
      lane_num_ignore = pc->lane_num_ignore; /* save and restore it below */
      pc->lane_num_ignore = 1;

      switch(this_lane) {
        case 0:
     data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_DWSDR_div1
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_SHIFT) |
            (XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_DWSDR_div1
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT);
    
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_MASK);
       break;
       case 1:
     data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_SHIFT) |
              (XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT);
    
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_MASK);
       break;
       case 2:
     data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_SHIFT) |
              (XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT);
    
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_MASK);
       break;
       case 3:
     data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_SHIFT) |
              (XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT);
    
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_MASK);
     break;
       default:
         printf("%-22s: Error: invalid lane number %d\n",__func__,this_lane);
         return SOC_E_ERROR;
         break;
       }
       pc->lane_num_ignore = lane_num_ignore;
     }

     if ((pc->os_type == WCMOD_OS5) && ((spd_intf == WCMOD_SPD_10000_XFI) ||
         (spd_intf == WCMOD_SPD_10000_SFI) || (spd_intf == WCMOD_SPD_10600_XFI_HG))) {
       MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                              (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66
                            << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) |
                               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK,
                               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
                               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
     }

     if( (spd_intf == WCMOD_SPD_PCSBYP_3P125G) ||
         (spd_intf == WCMOD_SPD_PCSBYP_6P25G) ||
         (spd_intf == WCMOD_SPD_PCSBYP_10P3125)) {
       lane_num_ignore = pc->lane_num_ignore; /* save and restore it below */
       pc->lane_num_ignore = 1;

       if (pc->model_type == WCMOD_WC_B0) {
         switch(this_lane) {
         case 0:
     if (spd_intf == WCMOD_SPD_PCSBYP_3P125G)
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_DWSDR_div2 
                                    << XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_DWSDR_div2 
                                    << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT);
     else if ((spd_intf == WCMOD_SPD_PCSBYP_6P25G)
               || (spd_intf == WCMOD_SPD_PCSBYP_10P3125))
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT);
     else
       data16 = 0;

     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_MASK);
     break;
         case 1:
     if (spd_intf == WCMOD_SPD_PCSBYP_3P125G)
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT);
     else if ((spd_intf == WCMOD_SPD_PCSBYP_6P25G)
               || (spd_intf == WCMOD_SPD_PCSBYP_10P3125))
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT);
     else
       data16 = 0;
    
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_MASK);
     break;
         case 2:
     if (spd_intf == WCMOD_SPD_PCSBYP_3P125G)
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT);
     else if ((spd_intf == WCMOD_SPD_PCSBYP_6P25G)
               || (spd_intf == WCMOD_SPD_PCSBYP_10P3125))
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_DWSDR_div1
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT);
     else
       data16 = 0;
       MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_MASK);
     break;
         case 3:
     if (spd_intf == WCMOD_SPD_PCSBYP_3P125G)
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT);
     else if ((spd_intf == WCMOD_SPD_PCSBYP_6P25G)
               || (spd_intf == WCMOD_SPD_PCSBYP_10P3125))
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_DWSDR_div1
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_DWSDR_div1
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT);
     else
       data16 = 0;

     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_MASK);
     break;
         default:
           printf("%-22s: Error: Invalid lane %d\n", __func__, this_lane);
       return SOC_E_ERROR;
         } /* switch(this_lane) */
         pc->lane_num_ignore = 0;

         if (spd_intf == WCMOD_SPD_PCSBYP_3P125G) {
       SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc,
                              DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_MASK,
                              DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_MASK |
                              DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_VAL_MASK));
         }

         MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40
                               << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT)
                              | SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK
                              | (SERDESDIGITAL_MISC1_FORCE_SPEED_dr_6363BRCM_X1
                               << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT),
                              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
                              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                                      SERDESDIGITAL_MISC1_FORCE_SPEED_MASK);
      } /*endif B0 */
      pc->lane_num_ignore = lane_num_ignore;
    }

    /* put the workaround for 1G and  2.5G for XENIA core aymmetric mode */
     if (pc->model_type == WCMOD_XN) {
        if ((spd_intf == WCMOD_SPD_2500) || (spd_intf == WCMOD_SPD_1000_SGMII) || (spd_intf == WCMOD_SPD_1000_FIBER)) {
            /* first program the 0x8308 to 0x7110 */
            if (spd_intf == WCMOD_SPD_2500){
                WRITE_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0x7110);
            } else {
                WRITE_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0x7120);
            }
            /* next enable the asymmetric bit in 0x8169 */
            MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(pc->unit, pc, 
                                XGXSBLK8_TXLNSWAP1_ASYMMETRIC_MODE_EN_MASK,
                                XGXSBLK8_TXLNSWAP1_ASYMMETRIC_MODE_EN_MASK);
            /*set the tx/rx clock control bit */
            if ((spd_intf == WCMOD_SPD_1000_SGMII) || (spd_intf == WCMOD_SPD_1000_FIBER)) {

                switch (pc->this_lane) {
                case 0: MODIFY_WC40_TX0_ANATXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        MODIFY_WC40_RX0_ANARXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        break;
                case 1: MODIFY_WC40_TX1_ANATXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        MODIFY_WC40_RX1_ANARXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        break;
                case 2: MODIFY_WC40_TX2_ANATXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        MODIFY_WC40_RX2_ANARXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        break;
                case 3: MODIFY_WC40_TX3_ANATXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        MODIFY_WC40_RX3_ANARXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        break;
                default: break;
                }
            } else {

                switch (pc->this_lane) {
                case 0: MODIFY_WC40_TX0_ANATXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        MODIFY_WC40_RX0_ANARXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        break;
                case 1: MODIFY_WC40_TX1_ANATXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        MODIFY_WC40_RX1_ANARXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        break;
                case 2: MODIFY_WC40_TX2_ANATXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        MODIFY_WC40_RX2_ANARXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        break;
                case 3: MODIFY_WC40_TX3_ANATXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        MODIFY_WC40_RX3_ANARXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        break;
                default: break;
                }
            }
            /* make sure os mode is set to 1 */
            WRITE_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, 0xffff);
       }
    }
    if (spd_intf == WCMOD_SPD_X2_10000) {       
        if(125 == pc->refclk) {
            data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                      SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                      (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div50 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
            mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
                     SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                     SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

            SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
        } else {
            data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                      SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                      (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
            mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
                     SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                     SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
            SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
        }
    }
    if (spd_intf == WCMOD_SPD_X2_23000) { 
       if(125 == pc->refclk) {
          data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                    SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                    (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div92 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
          mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
          
          SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
       } else {
          /* no proper div = 73.6 */
          data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                    SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                    (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
          mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
          SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
       }
     
       data16 = DIGITAL4_MISC4_SCR_EN_OVER_MASK ;
       mask16 = DIGITAL4_MISC4_SCR_EN_OVER_MASK ;
       SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC4r(pc->unit, pc, data16, mask16));
       
       data16 = DIGITAL4_MISC3_SCR_EN_PER_LANE_MASK ;
       mask16 = DIGITAL4_MISC3_SCR_EN_PER_LANE_MASK ;
       SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc, data16, mask16));
    }
   if ((spd_intf == WCMOD_SPD_10000_HI_DXGXS)     || (spd_intf == WCMOD_SPD_10000_DXGXS) ||
        (spd_intf == WCMOD_SPD_10000_HI_DXGXS_SCR) || (spd_intf == WCMOD_SPD_10000_DXGXS_SCR) ) {
       data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                 (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
       mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

       SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
    }
    if ((spd_intf == WCMOD_SPD_12773_DXGXS) || (spd_intf==WCMOD_SPD_12773_HI_DXGXS)||
        (spd_intf == WCMOD_SPD_10500_DXGXS) || (spd_intf==WCMOD_SPD_10500_HI_DXGXS) ) {
       data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                 (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div42 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
       mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

       SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
    } 
                                
    if (spd_intf == WCMOD_SPD_PCSBYP_5G) {
        if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
            /* first program PLL divider 0x8308 */
            SOC_IF_ERROR_RETURN( WRITE_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0x790a));
            SOC_IF_ERROR_RETURN( WRITE_WC40_DIGITAL4_MISC5r(pc->unit, pc, 0x40));
            SOC_IF_ERROR_RETURN( MODIFY_WC40_DIGITAL5_MISC7r(pc->unit, pc, 0x600, 0x600));
        }
    }

    if (spd_intf == WCMOD_SPD_CUSTOM_BYPASS) {
        if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
            /* first program PLL divider 0x8308 */
            switch (pc->pll_divider) {
                case 46:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div32 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 72:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div36 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 40:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 42:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div42 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 48:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div48 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 50:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div50 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 52:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div52 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 54:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 60:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div60 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 64:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div64 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 66:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 68:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div68 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 70:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 80:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div80 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 92:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div92 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 100:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div100 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                default:
                    break;
            }
            SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16,
                                SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
                                SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK));
            /* next program the os mode  0x8349 */
            SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL5_MISC7r(pc->unit, pc, DIGITAL5_MISC7_FORCE_OSCDR_MODE_MASK |
                                                            pc->oversample_mode << DIGITAL5_MISC7_FORCE_OSCDR_MODE_VAL_SHIFT, 
                                                            DIGITAL5_MISC7_FORCE_OSCDR_MODE_MASK | 
                                                            DIGITAL5_MISC7_FORCE_OSCDR_MODE_VAL_MASK));
            /* next inform micro controller the vco speed */
            data16 = (pc->vco_freq - 5750) / 250 + 1;
            SOC_IF_ERROR_RETURN(MODIFY_WC40_UC_INFO_B1_REGBr(pc->unit, pc, data16 <<
                                UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_SHIFT, UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_MASK));
        } 
    }
  } /* pc->port_type == WCMOD_INDEPENDENT */
  return SOC_E_NONE;
} /* Wcmod_speed_intf_set(wcmod_st* pc) */


void robo_serdes_init(robo_info_t *robo, int port)
{
  int myReturnValue;
  *retVal = SOC_E_NONE;

  ws->spd_intf  = WCMOD_SPD_1000_SGMII;
  ws->os_type   = WCMOD_OS5;
  ws->port_type = WCMOD_INDEPENDENT;
  ws->lane_swap = 0x32103210;
  ws->lane_select = WCMOD_LANE_BCST; 

  SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_MMDSELECTr(ws->unit, ws,
                                                       XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK,
                                                       XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK ));
#define MODIFY_WC40_XGXSBLK0_MMDSELECTr(_unit, _pc, _val, _mask) \
	WC40_REG_MODIFY((_unit), (_pc), 0x00, 0x0000800d, (_val), (_mask))
	#define XGXSBLK0_MMDSELECT_MULTIPRTS_EN_MASK                       0x8000

  /**retVal |= myReturnValue;*/

  wcmod_tier1_selector(SET_PORT_MODE, ws, &myReturnValue);
  *retVal |= myReturnValue;

  if (getGenericModelType(ws->model_type) == WCMOD_XN) {
    /* Xenia has neither OS5 nor OS8 */
    ws->os_type = WCMOD_OS_IND_LANE;
  }

  /* disable PLL */
  ws->per_lane_control = WCMOD_DISABLE;
  wcmod_tier1_selector(PLL_SEQUENCER_CONTROL, ws, &myReturnValue);
  *retVal |= myReturnValue;

  /* turn off RX sequencer */
  ws->per_lane_control = WCMOD_DISABLE;
  wcmod_tier1_selector(RX_SEQ_CONTROL, ws, &myReturnValue);
  *retVal |= myReturnValue;

  /* turn off clause 72 */
  ws->per_lane_control = 0;
  wcmod_tier1_selector(CLAUSE_72_CONTROL, ws, &myReturnValue);
  *retVal |= myReturnValue;

  /* disable AN on all lanes */
  ws->per_lane_control = 0;
  wcmod_tier1_selector(AUTONEG_CONTROL, ws, &myReturnValue);
  *retVal |= myReturnValue;

  /* disable parallel detect on all lanes */
  ws->per_lane_control = 0;
  wcmod_tier1_selector(PARALLEL_DETECT_CONTROL, ws, &myReturnValue);
  *retVal |= myReturnValue;

  /* disable 100FX on all lanes */
  ws->per_lane_control = 0;
  wcmod_tier1_selector(_100FX_CONTROL, ws, &myReturnValue);
  *retVal |= myReturnValue;

  /* set speed */
  wcmod_tier1_selector(SET_SPD_INTF, ws, &myReturnValue);
  *retVal |= myReturnValue;

  /* enable PLL sequencer */
  ws->per_lane_control = WCMOD_ENABLE;
  wcmod_tier1_selector(PLL_SEQUENCER_CONTROL, ws, &myReturnValue);
  *retVal |= myReturnValue;

  /* enable RX sequencer */
  ws->per_lane_control = WCMOD_ENABLE;
  wcmod_tier1_selector(RX_SEQ_CONTROL, ws, &myReturnValue);
  *retVal |= myReturnValue;
}
#endif

