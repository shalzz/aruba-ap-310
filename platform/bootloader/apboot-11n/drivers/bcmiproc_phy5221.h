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
*  @file    bcmiproc_phy5221.h
*
*  @brief   serdes routines
*
*  @note
*
*   These routines provide access to the serdes
*/
/****************************************************************************/

#ifndef _bcm_iproc_phy5221_h_
#define _bcm_iproc_phy5221_h_


/* ---- Include Files ---------------------------------------------------- */
#include <config.h>
#include <common.h>
#include <net.h>
#include <asm/arch/bcmenetphy.h>
#include <asm/arch/iproc_regs.h>
#include <asm/arch/iproc_gmac_regs.h>
#include <asm/arch/reg_utils.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/bcmutils.h>
#include "ethHw_data.h"

#define PHY_AUX_MULTIPLE_PHYr_BANK		0x0000
#define PHY_AUX_MULTIPLE_PHYr_ADDR		0x1e

#define PHY522X_SUPER_ISOLATE_MODE    (1<<3)

/* ---- External Function Prototypes ------------------------------------- */

extern int phy5221_wr_reg(bcm_eth_t *eth_data, uint phyaddr, uint16 reg_bank,
                uint8 reg_addr, uint16 *data);
extern int phy5221_rd_reg(bcm_eth_t *eth_data, uint phyaddr, uint16 reg_bank,
			uint8 reg_addr, uint16 *data);
extern int phy5221_init(bcm_eth_t *eth_data, uint phyaddr);
extern int phy5221_link_get(bcm_eth_t *eth_data, uint phyaddr, int *link);
extern int phy5221_enable_set(bcm_eth_t *eth_data, uint phyaddr, int enable);
extern int phy5221_speed_set(bcm_eth_t *eth_data, uint phyaddr, int speed);
extern int phy5221_speed_get(bcm_eth_t *eth_data, uint phyaddr, int *speed, int *duplex);
extern int phy5221_lb_set(bcm_eth_t *eth_data, uint phyaddr, int enable);
extern void phy5221_disp_status(bcm_eth_t *eth_data, uint phyaddr);
extern void phy5221_chk_err(bcm_eth_t *eth_data, uint phyaddr);

#endif	/* _bcm_iproc_phy5221_h_ */
