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
*  @file    bcmiproc_serdes.h
*
*  @brief   serdes routines
*
*  @note
*
*   These routines provide access to the serdes
*/
/****************************************************************************/

#ifndef _bcm_iproc_serdes_h_
#define _bcm_iproc_serdes_h_


/* ---- Include Files ---------------------------------------------------- */
#include <config.h>
#include <common.h>
#include <net.h>
#include <asm/arch/bcmenetphy.h>
#include <asm/arch/iproc_regs.h>
#include <asm/arch/iproc_gmac_regs.h>
#include <asm/iproc/reg_utils.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/bcmutils.h>
#include "ethHw_data.h"

/*
 * MII Link Advertisment (Clause 37) 
 */
#define MII_ANA_C37_NP          (1 << 15)  /* Next Page */
#define MII_ANA_C37_RF_OK       (0 << 12)  /* No error, link OK */
#define MII_ANA_C37_RF_LINK_FAIL (1 << 12)  /* Offline */
#define MII_ANA_C37_RF_OFFLINE  (2 << 12)  /* Link failure */
#define MII_ANA_C37_RF_AN_ERR   (3 << 12)  /* Auto-Negotiation Error */
#define MII_ANA_C37_PAUSE       (1 << 7)   /* Symmetric Pause */
#define MII_ANA_C37_ASYM_PAUSE  (1 << 8)   /* Asymmetric Pause */
#define MII_ANA_C37_HD          (1 << 6)   /* Half duplex */
#define MII_ANA_C37_FD          (1 << 5)   /* Full duplex */ 

/* MII Control Register: bit definitions */

#define MII_CTRL_FS_2500        (1 << 5) /* Force speed to 2500 Mbps */
#define MII_CTRL_SS_MSB         (1 << 6) /* Speed select, MSb */
#define MII_CTRL_CST            (1 << 7) /* Collision Signal test */
#define MII_CTRL_FD             (1 << 8) /* Full Duplex */
#define MII_CTRL_RAN            (1 << 9) /* Restart Autonegotiation */
#define MII_CTRL_IP             (1 << 10) /* Isolate Phy */
#define MII_CTRL_PD             (1 << 11) /* Power Down */
#define MII_CTRL_AE             (1 << 12) /* Autonegotiation enable */
#define MII_CTRL_SS_LSB         (1 << 13) /* Speed select, LSb */
#define MII_CTRL_LE             (1 << 14) /* Loopback enable */
#define MII_CTRL_RESET          (1 << 15) /* PHY reset */

#define MII_CTRL_SS(_x)         ((_x) & (MII_CTRL_SS_LSB|MII_CTRL_SS_MSB))
#define MII_CTRL_SS_10          0
#define MII_CTRL_SS_100         (MII_CTRL_SS_LSB)
#define MII_CTRL_SS_1000        (MII_CTRL_SS_MSB)
#define MII_CTRL_SS_INVALID     (MII_CTRL_SS_LSB | MII_CTRL_SS_MSB)
#define MII_CTRL_SS_MASK        (MII_CTRL_SS_LSB | MII_CTRL_SS_MSB)

/* ---- External Function Prototypes ------------------------------------- */

extern void serdes_set_blk(bcm_eth_t *eth_data, uint phyaddr, uint blk);
extern void serdes_wr_reg(bcm_eth_t *eth_data, uint phyaddr, uint reg, uint data);
extern uint16 serdes_rd_reg(bcm_eth_t *eth_data, uint phyaddr, uint reg);
extern uint16 serdes_get_id(bcm_eth_t *eth_data, uint phyaddr, uint off);
extern void serdes_reset(bcm_eth_t *eth_data, uint phyaddr);
extern int serdes_reset_core(bcm_eth_t *eth_data, uint phyaddr);
extern int serdes_start_pll(bcm_eth_t *eth_data, uint phyaddr);
extern int serdes_init(bcm_eth_t *eth_data, uint phyaddr);
extern int serdes_enable_set(bcm_eth_t *eth_data, uint phyaddr, int enable);
extern int serdes_speed_set(bcm_eth_t *eth_data, uint phyaddr, int speed);
extern int serdes_speed_get(bcm_eth_t *eth_data, uint phyaddr, int *speed);
extern int serdes_lb_set(bcm_eth_t *eth_data, uint phyaddr, int enable);
extern void serdes_disp_status(bcm_eth_t *eth_data, uint phyaddr);

#endif	/* _bcm_iproc_serdes_h_ */
