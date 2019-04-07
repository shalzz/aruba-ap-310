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
*  @file    bcmiproc_phy5221.c
*
*  @brief   phy routines
*
*  @note
*
*   These routines provide access to the external phy
*/
/****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */
#include "bcmiproc_phy.h"
#include "bcmiproc_phy5221.h"

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

#define	PHY_EXTERNAL	1

extern void chip_phy_wr(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg, uint16_t v);
extern uint16_t chip_phy_rd(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg);


/* ==== Public Functions ================================================= */

int
phy5221_wr_reg(bcm_eth_t *eth_data, uint phyaddr, uint16 reg_bank,
                uint8 reg_addr, uint16 *data)
{
	uint16  wr_data=*data;
	uint16  test_reg;

	NET_TRACE(("%s enter\n", __FUNCTION__));

	NET_REG_TRACE(("%s going to write phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
		 __FUNCTION__, phyaddr, reg_bank, reg_addr, wr_data));

	if (reg_bank) {
		test_reg = chip_phy_rd(eth_data, PHY_EXTERNAL, phyaddr, 0x1f);
		chip_phy_wr(eth_data, PHY_EXTERNAL, phyaddr, 0x1f, (test_reg | 0x0080));

		chip_phy_wr(eth_data, PHY_EXTERNAL, phyaddr, reg_addr, wr_data);

		chip_phy_wr(eth_data, PHY_EXTERNAL, phyaddr, 0x1f, test_reg);
    } else {
		chip_phy_wr(eth_data, PHY_EXTERNAL, phyaddr, reg_addr, wr_data);
	}
	return SOC_E_NONE;
}


int
phy5221_rd_reg(bcm_eth_t *eth_data, uint phyaddr, uint16 reg_bank,
			uint8 reg_addr, uint16 *data)
{
	uint16  test_reg;

	NET_TRACE(("%s enter\n", __FUNCTION__));

	//NET_REG_TRACE(("%s going to read phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x)\n",
	//		 __FUNCTION__, phyaddr, reg_bank, reg_addr));

	if (reg_bank) {
		test_reg = chip_phy_rd(eth_data, PHY_EXTERNAL, phyaddr, 0x1f);
		chip_phy_wr(eth_data, PHY_EXTERNAL, phyaddr, 0x1f, (test_reg | 0x0080));

		*data = chip_phy_rd(eth_data, PHY_EXTERNAL, phyaddr, reg_addr);

		chip_phy_wr(eth_data, PHY_EXTERNAL, phyaddr, 0x1f, test_reg);
	} else {
		*data = chip_phy_rd(eth_data, PHY_EXTERNAL, phyaddr, reg_addr);
	}
	NET_REG_TRACE(("%s rd phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
			 __FUNCTION__, phyaddr, reg_bank, reg_addr, *data));
	return SOC_E_NONE;
}


int
phy5221_mod_reg(bcm_eth_t *eth_data, uint phyaddr, uint16 reg_bank,
			uint8 reg_addr, uint16 data, uint16 mask)
{
	uint16 test_reg;
	uint16  rd_data;

	NET_TRACE(("%s enter\n", __FUNCTION__));

	NET_REG_TRACE(("%s going to modify phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x) mask(0x%x)\n",
		 __FUNCTION__, phyaddr, reg_bank, reg_addr, data, mask));

	if (reg_bank) {
		test_reg = chip_phy_rd(eth_data, PHY_EXTERNAL, phyaddr, 0x1f);
		chip_phy_wr(eth_data, PHY_EXTERNAL, phyaddr, 0x1f, (test_reg | 0x0080));

		rd_data = chip_phy_rd(eth_data, PHY_EXTERNAL, phyaddr, reg_addr);
		NET_REG_TRACE(("%s rd phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
				 __FUNCTION__, phyaddr, reg_bank, reg_addr, rd_data));
		rd_data &= ~(mask);
		rd_data |= data;
		chip_phy_wr(eth_data, PHY_EXTERNAL, phyaddr, reg_addr, rd_data);
		NET_REG_TRACE(("%s wrt phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
				 __FUNCTION__, phyaddr, reg_bank, reg_addr, rd_data));

		chip_phy_wr(eth_data, PHY_EXTERNAL, phyaddr, 0x1f, test_reg);
    } else {
		rd_data = chip_phy_rd(eth_data, PHY_EXTERNAL, phyaddr, reg_addr);
		NET_REG_TRACE(("%s rd phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
				 __FUNCTION__, phyaddr, reg_bank, reg_addr, rd_data));
		rd_data &= ~(mask);
		rd_data |= data;
		chip_phy_wr(eth_data, PHY_EXTERNAL, phyaddr, reg_addr, rd_data);
		NET_REG_TRACE(("%s wrt phyaddr(0x%x) reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
				 __FUNCTION__, phyaddr, reg_bank, reg_addr, rd_data));
	}

	return SOC_E_NONE;
}


void
phy5221_fe_reset(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16 ctrl;
	unsigned long init_time;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	/* set reset flag */
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &ctrl);
	ctrl |= MII_CTRL_RESET;
	phy5221_wr_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &ctrl);
	init_time = get_timer(0);
	for (;;) {

		udelay(100);

		/* check if out of reset */
		if (!(phy5221_rd_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &ctrl) & MII_CTRL_RESET)) {
			NET_TRACE(("et%d: %s reset complete\n", eth_data->unit, __FUNCTION__));
			return;
		}

		if (get_timer(init_time) > 10) {
			/* timeout */
			NET_ERROR(("et%d: %s reset not complete\n", eth_data->unit, __FUNCTION__));
			return;
		}
	}
}


/*
 * Function:    
 *  phy5221_fe_init
 * Purpose: 
 *  Initialize the PHY (MII mode) to a known good state.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 * Returns: 
 *  SOC_E_XXX

 * Notes: 
 *  No synchronization performed at this level.
 */
int
phy5221_fe_init(bcm_eth_t *eth_data, uint phyaddr)
{
    uint16  mii_ana, mii_ctrl;

    /* Reset PHY */
	phy5221_fe_reset(eth_data, phyaddr);

    mii_ana = MII_ANA_HD_10 | MII_ANA_FD_10 | MII_ANA_HD_100 | 
          MII_ANA_FD_100 | MII_ANA_ASF_802_3;
    mii_ctrl = MII_CTRL_FD | MII_CTRL_SS_100 | MII_CTRL_AE | MII_CTRL_RAN;

	phy5221_wr_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &mii_ctrl);
	phy5221_wr_reg(eth_data, phyaddr, PHY_MII_ANAr_BANK, PHY_MII_ANAr_ADDR, &mii_ana);

    return(SOC_E_NONE);
}


/*
 * Function:    
 *  phy5221_fe_speed_set
 * Purpose: 
 *  Set the current operating speed (forced).
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  duplex - (OUT) Boolean, true indicates full duplex, false 
 *      indicates half.
 * Returns: 
 *  SOC_E_XXX
 * Notes: 
 *  No synchronization performed at this level. Autonegotiation is 
 *  not manipulated. 
 */
int
phy5221_fe_speed_set(bcm_eth_t *eth_data, uint phyaddr, int speed)
{
    uint16     mii_ctrl;

    if (speed == 0) {
        return SOC_E_NONE;
    }

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &mii_ctrl);

    mii_ctrl &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
    switch(speed) {
    case 10:
	    mii_ctrl |= MII_CTRL_SS_10;
	    break;
    case 100:
	    mii_ctrl |= MII_CTRL_SS_100;
	    break;
    case 1000:  
	    mii_ctrl |= MII_CTRL_SS_1000;
	    break;
    default:
	    return SOC_E_CONFIG;
    }

	phy5221_wr_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &mii_ctrl);

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy5221_init
 * Purpose:
 *      Initialize xgxs6 phys
 * Parameters:
 *      eth_data - ethernet data
 *      phyaddr - physical address
 * Returns:
 *      0
 */
int
phy5221_init(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16	phyid0, phyid1;
//	soc_pbmp_t pbmp_100fx;
	uint16      tmp = 0;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_PHY_ID0r_BANK, PHY_MII_PHY_ID0r_ADDR, &phyid0);
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_PHY_ID1r_BANK, PHY_MII_PHY_ID1r_ADDR, &phyid1);

	//printf("%s phyaddr(0x%x) Phy ChipID: 0x%04x:0x%04x\n", __FUNCTION__, phyaddr, phyid1, phyid0);

    phy5221_fe_init(eth_data, phyaddr);

	phy5221_rd_reg(eth_data, phyaddr, 0, 0x16, &tmp);
	tmp = 2;
	phy5221_wr_reg(eth_data, phyaddr, 0, 0x16, &tmp);

#if 0
    /*
      * Get 100-FX ports from config.bcm.
      */
	/* Speed/Duplex selection and auto-negotiation Disable */
	tmp = 0x2100;
	phy5221_wr_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &tmp);

	/* Scrambler and Descrambler Disable and Far-End-Fault Enable */
	phy5221_rd_reg(eth_data, phyaddr, 0, 0x10, &tmp);
	tmp |= 0x0220;
	phy5221_wr_reg(eth_data, phyaddr, 0, 0x10, &tmp);

	/* Configure Transmit and Receive for Binary Signaling */
	phy5221_rd_reg(eth_data, phyaddr, 0, 0x17, &tmp);
	tmp |= 0x020;
	phy5221_wr_reg(eth_data, phyaddr, 0, 0x17, &tmp);

	/* Enable Internal EFX Signal Detect Function */
	/* Enable the shadow register */
	tmp = 0x008b;
	phy5221_wr_reg(eth_data, phyaddr, 0, 0x1f, &tmp);

	/* Enable the special Signal Detect function */
	tmp = 0x0200;
	phy5221_wr_reg(eth_data, phyaddr, 0, 0x19, &tmp);
	/* Configure the transmit amplitude for 1V pk-pk differential */
	tmp  = 0x0084;
	phy5221_wr_reg(eth_data, phyaddr, 0, 0x1d, &tmp);
	/* Exist the shadow register */
	tmp  = 0x000b;
	phy5221_wr_reg(eth_data, phyaddr, 0, 0x1f, &tmp);
#endif

	return 0;
}


/*
 * Function:    
 *  phy5221_link_get
 * Purpose: 
 *  Determine the current link up/down status
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #. 
 *  link - (OUT) Boolean, true indicates link established.
 * Returns:
 *  SOC_E_XXX
 * Notes: 
 *  No synchronization performed at this level.
 */
int
phy5221_link_get(bcm_eth_t *eth_data, uint phyaddr, int *link)
{
	uint16        mii_ctrl, mii_stat;
	unsigned long init_time;

    *link = FALSE;      /* Default */

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &mii_stat);
	/* the first read of status register will not show link up, second read will show link up */
    if (!(mii_stat & MII_STAT_LA) ) {
		phy5221_rd_reg(eth_data, phyaddr, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &mii_stat);
	}

    if (!(mii_stat & MII_STAT_LA) || (mii_stat == 0xffff)) {
    /* mii_stat == 0xffff check is to handle removable PHY daughter cards */
        return SOC_E_NONE;
    }

    /* Link appears to be up; we are done if autoneg is off. */

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &mii_ctrl);

    if (!(mii_ctrl & MII_CTRL_AE)) {
		*link = TRUE;
		return SOC_E_NONE;
    }

    /*
     * If link appears to be up but autonegotiation is still in
     * progress, wait for it to complete.  For BCM5228, autoneg can
     * still be busy up to about 200 usec after link is indicated.  Also
     * continue to check link state in case it goes back down.
     */
	init_time = get_timer(0);
    for (;;) {

		phy5221_rd_reg(eth_data, phyaddr, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &mii_stat);

	    if (!(mii_stat & MII_STAT_LA)) {
			/* link is down */
	        return SOC_E_NONE;
	    }

	    if (mii_stat & MII_STAT_AN_DONE) {
			/* AutoNegotiation done */
	        break;
	    }

		if(get_timer(init_time) > 1) {
			/* timeout */
	        return SOC_E_BUSY;
		}
    }

    /* Return link state at end of polling */
    *link = ((mii_stat & MII_STAT_LA) != 0);

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy5221_enable_set
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
phy5221_enable_set(bcm_eth_t *eth_data, uint phyaddr, int enable)
{
	uint16	data;       /* New value to write to PHY register */

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	data  = enable ? 0 : MII_ECR_TD; /* Transmitt enable/disable */
	phy5221_mod_reg(eth_data, phyaddr, PHY_MII_ECRr_BANK, PHY_MII_ECRr_ADDR, data, MII_ECR_TD);

	data  = enable ? 0 : PHY522X_SUPER_ISOLATE_MODE; 
    /* Device needs to be put in super-isolate mode in order to disable 
     * the link in 10BaseT mode
     */
	phy5221_mod_reg(eth_data, phyaddr, PHY_AUX_MULTIPLE_PHYr_BANK, PHY_AUX_MULTIPLE_PHYr_ADDR,
					data, PHY522X_SUPER_ISOLATE_MODE);

    return SOC_E_NONE;
}


/*
 * Function:
 *      phy5221_speed_set
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
phy5221_speed_set(bcm_eth_t *eth_data, uint phyaddr, int speed)
{
	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	phy5221_fe_speed_set(eth_data, phyaddr, speed);

	return 0;
}


/*
 * Function:     
 *    phy5221_auto_negotiate_gcd (greatest common denominator).
 * Purpose:    
 *    Determine the current greatest common denominator between 
 *    two ends of a link
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #. 
 *    speed - (OUT) greatest common speed.
 *    duplex - (OUT) greatest common duplex.
 *    link - (OUT) Boolean, true indicates link established.
 * Returns:    
 *    SOC_E_XXX
 * Notes: 
 *    No synchronization performed at this level.
 */
static int
phy5221_auto_negotiate_gcd(bcm_eth_t *eth_data, uint phyaddr, int *speed, int *duplex)
{
    int        t_speed, t_duplex;
    uint16     mii_ana, mii_anp, mii_stat;
    uint16     mii_gb_stat, mii_esr, mii_gb_ctrl;

    mii_gb_stat = 0;            /* Start off 0 */
    mii_gb_ctrl = 0;            /* Start off 0 */

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_ANAr_BANK, PHY_MII_ANAr_ADDR, &mii_ana);
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_ANPr_BANK, PHY_MII_ANPr_ADDR, &mii_anp);
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &mii_stat);

    if (mii_stat & MII_STAT_ES) {    /* Supports extended status */
        /*
         * If the PHY supports extended status, check if it is 1000MB
         * capable.  If it is, check the 1000Base status register to see
         * if 1000MB negotiated.
         */
		phy5221_rd_reg(eth_data, phyaddr, PHY_MII_ESRr_BANK, PHY_MII_ESRr_ADDR, &mii_esr);

        if (mii_esr & (MII_ESR_1000_X_FD | MII_ESR_1000_X_HD | 
                       MII_ESR_1000_T_FD | MII_ESR_1000_T_HD)) {
			phy5221_rd_reg(eth_data, phyaddr, PHY_MII_GB_STATr_BANK, PHY_MII_GB_STATr_ADDR, &mii_gb_stat);
			phy5221_rd_reg(eth_data, phyaddr, PHY_MII_GB_CTRLr_BANK, PHY_MII_GB_CTRLr_ADDR, &mii_gb_ctrl);
        }
    }

    /*
     * At this point, if we did not see Gig status, one of mii_gb_stat or 
     * mii_gb_ctrl will be 0. This will cause the first 2 cases below to 
     * fail and fall into the default 10/100 cases.
     */

    mii_ana &= mii_anp;

    if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000FD) &&
        (mii_gb_stat & MII_GB_STAT_LP_1000FD)) {
        t_speed  = 1000;
        t_duplex = 1;
    } else if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000HD) &&
               (mii_gb_stat & MII_GB_STAT_LP_1000HD)) {
        t_speed  = 1000;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_FD_100) {         /* [a] */
        t_speed = 100;
        t_duplex = 1;
    } else if (mii_ana & MII_ANA_T4) {            /* [b] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_HD_100) {        /* [c] */
        t_speed = 100;
        t_duplex = 0;
    } else if (mii_ana & MII_ANA_FD_10) {        /* [d] */
        t_speed = 10;
        t_duplex = 1 ;
    } else if (mii_ana & MII_ANA_HD_10) {        /* [e] */
        t_speed = 10;
        t_duplex = 0;
    } else {
        return(SOC_E_FAIL);
    }

    if (speed)  *speed  = t_speed;
    if (duplex)    *duplex = t_duplex;

    return(SOC_E_NONE);
}


/*
 * Function:
 *      phy5221_speed_get
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
phy5221_speed_get(bcm_eth_t *eth_data, uint phyaddr, int *speed, int *duplex)
{
    int     rv;
    uint16  mii_ctrl, mii_stat;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __FUNCTION__, phyaddr));

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &mii_ctrl);
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &mii_stat);

    *speed = 0;
    *duplex = 0;
    if (mii_ctrl & MII_CTRL_AE) {   /* Auto-negotiation enabled */
        if (!(mii_stat & MII_STAT_AN_DONE)) { /* Auto-neg NOT complete */
            rv = SOC_E_NONE;
        } else {
	        rv = phy5221_auto_negotiate_gcd(eth_data, phyaddr, speed, duplex);
		}
    } else {                /* Auto-negotiation disabled */
	    /*
	     * Simply pick up the values we force in CTRL register.
	     */
		if (mii_ctrl & MII_CTRL_FD)
			*duplex = 1;

	    switch(MII_CTRL_SS(mii_ctrl)) {
	    case MII_CTRL_SS_10:
	        *speed = 10;
	        break;
	    case MII_CTRL_SS_100:
	        *speed = 100;
	        break;
	    case MII_CTRL_SS_1000:
	        *speed = 1000;
	        break;
	    default:            /* Just pass error back */
	        return(SOC_E_UNAVAIL);
	    }
    	rv = SOC_E_NONE;
    }

    return(rv);
}


int
phy5221_lb_set(bcm_eth_t *eth_data, uint phyaddr, int enable)
{
	uint16  mii_ctrl;

	/* set reset flag */
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &mii_ctrl);
    mii_ctrl &= ~MII_CTRL_LE;
    mii_ctrl |= enable ? MII_CTRL_LE : 0;
	phy5221_wr_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &mii_ctrl);

    return 0;
}


void
phy5221_disp_status(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16		tmp0, tmp1, tmp2;
	int			speed, duplex;

	printf("et%d: %s: phyaddr:%d\n", eth_data->unit, __FUNCTION__, phyaddr);

#if 0
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_CTRLr_BANK, PHY_MII_CTRLr_ADDR, &tmp0);
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &tmp1);
	printf("  MII-Control: 0x%x; MII-Status: 0x%x\n", tmp0, tmp1);

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_PHY_ID0r_BANK, PHY_MII_PHY_ID0r_ADDR, &tmp0);
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_PHY_ID1r_BANK, PHY_MII_PHY_ID1r_ADDR, &tmp1);
	printf("  Phy ChipID: 0x%04x:0x%04x\n", tmp0, tmp1);

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_ANAr_BANK, PHY_MII_ANAr_ADDR, &tmp0);
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_ANPr_BANK, PHY_MII_ANPr_ADDR, &tmp1);
	phy5221_speed_get(eth_data, phyaddr, &speed, &duplex);
	printf("  AutoNeg Ad: 0x%x; AutoNeg Partner: 0x%x; speed:%d; duplex:%d\n", tmp0, tmp1, speed, duplex);

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_ESRr_BANK, PHY_MII_ESRr_ADDR, &tmp0);
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_ECRr_BANK, PHY_MII_ECRr_ADDR, &tmp1);
	phy5221_rd_reg(eth_data, phyaddr, 0x0000, 0x11, &tmp2);
	printf("  Reg0x0f: 0x%x; 100Base-X AUX ctrl: 0x%x; 100Base-X AUX stat: 0x%x\n", tmp0, tmp1, tmp2);

	phy5221_rd_reg(eth_data, phyaddr, 0x0000, 0x12, &tmp0);
	phy5221_rd_reg(eth_data, phyaddr, 0x0000, 0x13, &tmp1);
	phy5221_rd_reg(eth_data, phyaddr, 0x0000, 0x14, &tmp2);
	printf("  100Base-X RCV ERR: 0x%x; 100Base-X FALSE CARRIER: 0x%x; 100Base-X DISCON: 0x%x\n", tmp0, tmp1, tmp2);
#endif

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_ANAr_BANK, PHY_MII_ANAr_ADDR, &tmp0);
	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_ANPr_BANK, PHY_MII_ANPr_ADDR, &tmp1);
	phy5221_speed_get(eth_data, phyaddr, &speed, &duplex);
	printf("  AutoNeg Ad: 0x%x; AutoNeg Partner: 0x%x; speed:%d; duplex:%d\n", tmp0, tmp1, speed, duplex);

	for (tmp0=0; tmp0<0x20; tmp0++) {
		phy5221_rd_reg(eth_data, phyaddr, 0, tmp0, &tmp1);
		//printf("reg0x%02x: 0x%04x, ", tmp0, tmp1);
		//if ((tmp&0x3) == 0x3)
		//	printf("\n");
	}
	phy5221_rd_reg(eth_data, phyaddr, 1, 0x18, &tmp1);
	for (tmp0=0x1a; tmp0<0x1f; tmp0++) {
		phy5221_rd_reg(eth_data, phyaddr, 1, tmp0, &tmp1);
		//printf("reg0x%02x: 0x%04x, ", tmp0, tmp1);
		//if ((tmp&0x3) == 0x3)
		//	printf("\n");
	}

}


void
phy5221_chk_err(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16		tmp0;

	phy5221_rd_reg(eth_data, phyaddr, PHY_MII_STATr_BANK, PHY_MII_STATr_ADDR, &tmp0);
	if (!(tmp0 & MII_STAT_LA))
		printf("ERROR: reg 0x01 (LINK down): 0x%x\n", tmp0);
//	if (tmp0 & (MII_STAT_JBBR|MII_STAT_RF))
	if (tmp0 & MII_STAT_RF)
		printf("ERROR: reg 0x01: 0x%x\n", tmp0);

	phy5221_rd_reg(eth_data, phyaddr, 0, 0x11, &tmp0);
	if (!(tmp0 & 0x100))
		printf("ERROR: reg 0x11 (LINK down): 0x%x\n", tmp0);
	if (tmp0 & 0x8bf)
		printf("ERROR: reg 0x11: 0x%x\n", tmp0);

	phy5221_rd_reg(eth_data, phyaddr, 0, 0x12, &tmp0);
	if (tmp0)
		printf("ERROR: reg 0x12 (RCV ERR CNT): 0x%x\n", tmp0);

	phy5221_rd_reg(eth_data, phyaddr, 0, 0x13, &tmp0);
	if (tmp0)
		printf("ERROR: reg 0x13 (FALSE CARRIER CNT): 0x%x\n", tmp0);

	phy5221_rd_reg(eth_data, phyaddr, 0, 0x14, &tmp0);
	if (tmp0 & 0xc000)
		printf("ERROR: reg 0x14: 0x%x\n", tmp0);

	phy5221_rd_reg(eth_data, phyaddr, 0, 0x19, &tmp0);
	if (!(tmp0 & 0x4))
		printf("ERROR: reg 0x19 (LINK down): 0x%x\n", tmp0);
//	if (tmp0 & 0xc1)
	if (tmp0 & 0xc0)
		printf("ERROR: reg 0x19: 0x%x\n", tmp0);
}
