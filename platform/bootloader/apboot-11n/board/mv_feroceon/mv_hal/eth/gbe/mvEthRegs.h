/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates
This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/


#ifndef __INCmvEthRegsh
#define __INCmvEthRegsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mvSysEthConfig.h"

#define ETH_MAX_DECODE_WIN              6
#define ETH_MAX_HIGH_ADDR_REMAP_WIN     4

/****************************************/
/*        Ethernet Unit Registers       */
/****************************************/
#define ETH_PHY_ADDR_REG(port)              (MV_ETH_REGS_BASE(port) + 0x000)
#define ETH_SMI_REG(port)                   (MV_ETH_REGS_BASE(port) + 0x004)
#define ETH_UNIT_DEF_ADDR_REG(port)         (MV_ETH_REGS_BASE(port) + 0x008)
#define ETH_UNIT_DEF_ID_REG(port)           (MV_ETH_REGS_BASE(port) + 0x00c)
#define ETH_UNIT_RESERVED(port)             (MV_ETH_REGS_BASE(port) + 0x014)
#define ETH_UNIT_INTR_CAUSE_REG(port)       (MV_ETH_REGS_BASE(port) + 0x080)
#define ETH_UNIT_INTR_MASK_REG(port)        (MV_ETH_REGS_BASE(port) + 0x084)


#define ETH_UNIT_ERROR_ADDR_REG(port)       (MV_ETH_REGS_BASE(port) + 0x094)
#define ETH_UNIT_INT_ADDR_ERROR_REG(port)   (MV_ETH_REGS_BASE(port) + 0x098)

/* Ethernet Unit Control (EUC) register */
#define ETH_UNIT_CONTROL_REG(port)          (MV_ETH_REGS_BASE(port) + 0x0B0)

#define ETH_PHY_POLLING_ENABLE_BIT	        1
#define ETH_PHY_POLLING_ENABLE_MASK	        (1 << ETH_PHY_POLLING_ENABLE_BIT)

#define ETH_PORT_RESET_BIT                  24
#define ETH_PORT_RESET_MASK                 (1 << ETH_PORT_RESET_BIT)
/*-----------------------------------------------------------------------------------------------*/

/**** Address decode registers ****/

#define ETH_WIN_BASE_REG(port, win)         (MV_ETH_REGS_BASE(port) + 0x200 + ((win)<<3))
#define ETH_WIN_SIZE_REG(port, win)         (MV_ETH_REGS_BASE(port) + 0x204 + ((win)<<3))
#define ETH_WIN_REMAP_REG(port, win)        (MV_ETH_REGS_BASE(port) + 0x280 + ((win)<<2))
#define ETH_BASE_ADDR_ENABLE_REG(port)      (MV_ETH_REGS_BASE(port) + 0x290)
#define ETH_ACCESS_PROTECT_REG(port)        (MV_ETH_REGS_BASE(port) + 0x294)

/* The target associated with this window	    */
#define ETH_WIN_TARGET_OFFS             0
#define ETH_WIN_TARGET_MASK             (0xf << ETH_WIN_TARGET_OFFS)
/* The target attributes associated with window */
#define ETH_WIN_ATTR_OFFS               8
#define ETH_WIN_ATTR_MASK               (0xff << ETH_WIN_ATTR_OFFS)

/* The Base address associated with window */
#define ETH_WIN_BASE_OFFS		            16
#define ETH_WIN_BASE_MASK		            (0xFFFF << ETH_WIN_BASE_OFFS)

#define ETH_WIN_SIZE_OFFS		            16
#define ETH_WIN_SIZE_MASK		            (0xFFFF << ETH_WIN_SIZE_OFFS)
/* Ethernet Port Access Protect Register (EPAPR)    */
#define ETH_PROT_NO_ACCESS                  0
#define ETH_PROT_READ_ONLY                  1
#define ETH_PROT_FULL_ACCESS                3
#define ETH_PROT_WIN_OFFS(winNum)           (2 * (winNum))
#define ETH_PROT_WIN_MASK(winNum)           (0x3 << ETH_PROT_WIN_OFFS(winNum))
/*-----------------------------------------------------------------------------------------------*/

#define ETH_PORT_CONFIG_REG(port)           (MV_ETH_REGS_BASE(port) + 0x400)
#define ETH_PORT_CONFIG_EXTEND_REG(port)    (MV_ETH_REGS_BASE(port) + 0x404)

#define ETH_VLAN_ETHER_TYPE_REG(port)       (MV_ETH_REGS_BASE(port) + 0x410)
#define ETH_MAC_ADDR_LOW_REG(port)          (MV_ETH_REGS_BASE(port) + 0x414)
#define ETH_MAC_ADDR_HIGH_REG(port)         (MV_ETH_REGS_BASE(port) + 0x418)
#define ETH_SDMA_CONFIG_REG(port)           (MV_ETH_REGS_BASE(port) + 0x41c)
#define ETH_DIFF_SERV_PRIO_REG(port, code)  (MV_ETH_REGS_BASE(port) + 0x420  + ((code)<<2))

/* Port Serial Control0 register (PSC0) */
#define ETH_PORT_SERIAL_CTRL_REG(port)      (MV_ETH_REGS_BASE(port) + 0x43c)

#define ETH_TX_FC_MODE_OFFSET               5
#define ETH_TX_FC_MODE_MASK                 (3 << ETH_TX_FC_MODE_OFFSET)
#define ETH_TX_FC_NO_PAUSE                  (0 << ETH_TX_FC_MODE_OFFSET)
#define ETH_TX_FC_SEND_PAUSE                (1 << ETH_TX_FC_MODE_OFFSET)

#define ETH_TX_BP_MODE_OFFSET               7
#define ETH_TX_BP_MODE_MASK                 (3 << ETH_TX_BP_MODE_OFFSET)
#define ETH_TX_BP_NO_JAM                    (0 << ETH_TX_BP_MODE_OFFSET)
#define ETH_TX_BP_SEND_JAM                  (1 << ETH_TX_BP_MODE_OFFSET)

#define ETH_RETRANSMIT_FOREVER_BIT          11
#define ETH_RETRANSMIT_FOREVER_MASK         (1 << ETH_RETRANSMIT_FOREVER_BIT)

#define ETH_DTE_ADVERT_BIT                  14
#define ETH_DTE_ADVERT_MASK                 (1 << ETH_DTE_ADVERT_BIT)

#define ETH_IGNORE_RX_ERR_BIT               28
#define ETH_IGNORE_RX_ERR_MASK              (1 << ETH_IGNORE_RX_ERR_BIT)

#define ETH_IGNORE_COL_BIT                  29
#define ETH_IGNORE_COL_MASK                 (1 << ETH_IGNORE_COL_BIT)

#define ETH_IGNORE_CARRIER_SENSE_BIT        30
#define ETH_IGNORE_CARRIER_SENSE_MASK       (1 << ETH_IGNORE_CARRIER_SENSE_BIT)
/*-----------------------------------------------------------------------------------------------*/

#define ETH_VLAN_TAG_TO_PRIO_REG(port)      (MV_ETH_REGS_BASE(port) + 0x440)

/***** Ethernet Port Status reg (PSR) *****/
#define ETH_PORT_STATUS_REG(port)           (MV_ETH_REGS_BASE(port) + 0x444)

#define ETH_TX_IN_PROGRESS_OFFS             0
#define ETH_TX_IN_PROGRESS_MASK(txp)        (1 << ((txp) + ETH_TX_IN_PROGRESS_OFFS))
#define ETH_TX_IN_PROGRESS_ALL_MASK         (0xFF << ETH_TX_IN_PROGRESS_OFFS)

#define ETH_TX_FIFO_EMPTY_OFFS              8
#define ETH_TX_FIFO_EMPTY_MASK(txp)         (1 << ((txp) + ETH_TX_FIFO_EMPTY_OFFS))
#define ETH_TX_FIFO_EMPTY_ALL_MASK          (0xFF << ETH_TX_FIFO_EMPTY_OFFS)

#define ETH_RX_FIFO_EMPTY_BIT               16
#define ETH_RX_FIFO_EMPTY_MASK              (1 << ETH_RX_FIFO_EMPTY_BIT)
/*-----------------------------------------------------------------------------------------------*/


/***** Transmit Queue Command (TxQC) register *****/
#define ETH_TX_QUEUE_COMMAND_REG(port)      (MV_ETH_REGS_BASE(port) + 0x448)

#define ETH_TXQ_ENABLE_OFFSET               0
#define ETH_TXQ_ENABLE_MASK                 (0x000000FF << ETH_TXQ_ENABLE_OFFSET)

#define ETH_TXQ_DISABLE_OFFSET              8
#define ETH_TXQ_DISABLE_MASK                (0x000000FF << ETH_TXQ_DISABLE_OFFSET)
/*-----------------------------------------------------------------------------------------------*/

#define ETH_PORT_MARVELL_HEADER_REG(port)   (MV_ETH_REGS_BASE(port) + 0x454)
#define ETH_MAX_TOKEN_BUCKET_SIZE_REG(port) (MV_ETH_REGS_BASE(port) + 0x45c)
#define ETH_INTR_CAUSE_REG(port)            (MV_ETH_REGS_BASE(port) + 0x460)
#define ETH_INTR_CAUSE_EXT_REG(port)        (MV_ETH_REGS_BASE(port) + 0x464)
#define ETH_INTR_MASK_REG(port)             (MV_ETH_REGS_BASE(port) + 0x468)
#define ETH_INTR_MASK_EXT_REG(port)         (MV_ETH_REGS_BASE(port) + 0x46c)
#define ETH_TX_FIFO_URGENT_THRESH_REG(port) (MV_ETH_REGS_BASE(port) + 0x474)
#define ETH_RX_MINIMAL_FRAME_SIZE_REG(port) (MV_ETH_REGS_BASE(port) + 0x47c)
#define ETH_RX_DISCARD_PKTS_CNTR_REG(port)  (MV_ETH_REGS_BASE(port) + 0x484)
#define ETH_RX_OVERRUN_PKTS_CNTR_REG(port)  (MV_ETH_REGS_BASE(port) + 0x488)
#define ETH_INTERNAL_ADDR_ERROR_REG(port)   (MV_ETH_REGS_BASE(port) + 0x494)
#define ETH_TX_FIXED_PRIO_CFG_REG(port)     (MV_ETH_REGS_BASE(port) + 0x4dc)
#define ETH_TX_TOKEN_RATE_CFG_REG(port)     (MV_ETH_REGS_BASE(port) + 0x4e0)
#define ETH_TX_QUEUE_COMMAND1_REG(port)     (MV_ETH_REGS_BASE(port) + 0x4e4)
#define ETH_MAX_TRANSMIT_UNIT_REG(port)     (MV_ETH_REGS_BASE(port) + 0x4e8)
#define ETH_TX_TOKEN_BUCKET_SIZE_REG(port)  (MV_ETH_REGS_BASE(port) + 0x4ec)
#define ETH_TX_TOKEN_BUCKET_COUNT_REG(port) (MV_ETH_REGS_BASE(port) + 0x780)
#define ETH_RX_DESCR_STAT_CMD_REG(port, q)  (MV_ETH_REGS_BASE(port) + 0x600 + ((q)<<4))
#define ETH_RX_BYTE_COUNT_REG(port, q)      (MV_ETH_REGS_BASE(port) + 0x604 + ((q)<<4))
#define ETH_RX_BUF_PTR_REG(port, q)         (MV_ETH_REGS_BASE(port) + 0x608 + ((q)<<4))
#define ETH_RX_CUR_DESC_PTR_REG(port, q)    (MV_ETH_REGS_BASE(port) + 0x60c + ((q)<<4))

/***** Receive Queue Command (RxQC) register *****/
#define ETH_RX_QUEUE_COMMAND_REG(port)      (MV_ETH_REGS_BASE(port) + 0x680)

#define ETH_RXQ_ENABLE_OFFSET               0
#define ETH_RXQ_ENABLE_MASK                 (0x000000FF << ETH_RXQ_ENABLE_OFFSET)

#define ETH_RXQ_DISABLE_OFFSET              8
#define ETH_RXQ_DISABLE_MASK                (0x000000FF << ETH_RXQ_DISABLE_OFFSET)
/*-----------------------------------------------------------------------------------------------*/
#define ETH_TX_CUR_DESC_PTR_REG(port, q)    (MV_ETH_REGS_BASE(port) + 0x6c0 + ((q)<<2))

#define ETH_TXQ_TOKEN_COUNT_REG(port, q)    (MV_ETH_REGS_BASE(port) + 0x700 + ((q)<<4))
#define ETH_TXQ_TOKEN_CFG_REG(port, q)      (MV_ETH_REGS_BASE(port) + 0x704 + ((q)<<4))
#define ETH_TXQ_ARBITER_CFG_REG(port, q)    (MV_ETH_REGS_BASE(port) + 0x708 + ((q)<<4))

#if (MV_ETH_VERSION >= 4)
#define ETH_TXQ_CMD_1_REG(port)             (MV_ETH_REGS_BASE(port) + 0x4E4)
#define ETH_EJP_TX_HI_IPG_REG(port)         (MV_ETH_REGS_BASE(port) + 0x7A8)
#define ETH_EJP_TX_LO_IPG_REG(port)         (MV_ETH_REGS_BASE(port) + 0x7B8)
#define ETH_EJP_HI_TKN_LO_PKT_REG(port)     (MV_ETH_REGS_BASE(port) + 0x7C0)
#define ETH_EJP_HI_TKN_ASYNC_PKT_REG(port)  (MV_ETH_REGS_BASE(port) + 0x7C4)
#define ETH_EJP_LO_TKN_ASYNC_PKT_REG(port)  (MV_ETH_REGS_BASE(port) + 0x7C8)
#define ETH_EJP_TX_SPEED_REG(port)          (MV_ETH_REGS_BASE(port) + 0x7D0)
#endif /* MV_ETH_VERSION >= 4 */


/******* GigE MAC registers *******/
#define NETA_REG_BASE(port) 				(MV_ETH_REGS_BASE(port) & ~0x2000)

#define NETA_GMAC_CTRL_0_REG(p)             (NETA_REG_BASE(p) + 0x2C00)

#define NETA_GMAC_PORT_EN_BIT               0
#define NETA_GMAC_PORT_EN_MASK              (1 << NETA_GMAC_PORT_EN_BIT)

#define NETA_GMAC_PORT_TYPE_BIT             1
#define NETA_GMAC_PORT_TYPE_MASK            (1 << NETA_GMAC_PORT_TYPE_BIT)
#define NETA_GMAC_PORT_TYPE_SGMII           (0 << NETA_GMAC_PORT_TYPE_BIT)
#define NETA_GMAC_PORT_TYPE_1000X           (1 << NETA_GMAC_PORT_TYPE_BIT)

#define NETA_GMAC_MAX_RX_SIZE_OFFS          2
#define NETA_GMAC_MAX_RX_SIZE_MASK          (0x1FFF << NETA_GMAC_MAX_RX_SIZE_OFFS)

#define NETA_GMAC_MIB_CNTR_EN_BIT           15
#define NETA_GMAC_MIB_CNTR_EN_MASK          (1 << NETA_GMAC_MIB_CNTR_EN_BIT)
/*-------------------------------------------------------------------------------*/

#define NETA_GMAC_CTRL_1_REG(p)             (NETA_REG_BASE(p) + 0x2C04)

#define NETA_GMAC_CTRL_2_REG(p)             (NETA_REG_BASE(p) + 0x2C08)

#define NETA_GMAC_PORT_RGMII_BIT	    4
#define NETA_GMAC_PORT_RGMII_MASK           (1 << NETA_GMAC_PORT_RGMII_BIT)

#define NETA_GMAC_PORT_RESET_BIT            6
#define NETA_GMAC_PORT_RESET_MASK           (1 << NETA_GMAC_PORT_RESET_BIT)
/*-------------------------------------------------------------------------------*/

#define NETA_GMAC_AN_CTRL_REG(p)                (NETA_REG_BASE(p) + 0x2C0C)

#define NETA_FORCE_LINK_FAIL_BIT                0
#define NETA_FORCE_LINK_FAIL_MASK               (1 << NETA_FORCE_LINK_FAIL_BIT)

#define NETA_FORCE_LINK_PASS_BIT                1
#define NETA_FORCE_LINK_PASS_MASK               (1 << NETA_FORCE_LINK_PASS_BIT)

#define NETA_SET_MII_SPEED_100_BIT              5
#define NETA_SET_MII_SPEED_100_MASK             (1 << NETA_SET_MII_SPEED_100_BIT)

#define NETA_SET_GMII_SPEED_1000_BIT            6
#define NETA_SET_GMII_SPEED_1000_MASK           (1 << NETA_SET_GMII_SPEED_1000_BIT)

#define NETA_ENABLE_SPEED_AUTO_NEG_BIT          7
#define NETA_ENABLE_SPEED_AUTO_NEG_MASK         (1 << NETA_ENABLE_SPEED_AUTO_NEG_BIT)

#define NETA_SET_FLOW_CONTROL_BIT               8
#define NETA_SET_FLOW_CONTROL_MASK              (1 << NETA_SET_FLOW_CONTROL_BIT)

#define NETA_FLOW_CONTROL_ADVERTISE_BIT         9
#define NETA_FLOW_CONTROL_ADVERTISE_MASK        (1 << NETA_FLOW_CONTROL_ADVERTISE_BIT)

#define NETA_FLOW_CONTROL_ASYMETRIC_BIT         10
#define NETA_FLOW_CONTROL_ASYMETRIC_MASK        (1 << NETA_FLOW_CONTROL_ASYMETRIC_BIT)

#define NETA_ENABLE_FLOW_CONTROL_AUTO_NEG_BIT   11
#define NETA_ENABLE_FLOW_CONTROL_AUTO_NEG_MASK  (1 << NETA_ENABLE_FLOW_CONTROL_AUTO_NEG_BIT)

#define NETA_SET_FULL_DUPLEX_BIT                12
#define NETA_SET_FULL_DUPLEX_MASK               (1 << NETA_SET_FULL_DUPLEX_BIT)

#define NETA_ENABLE_DUPLEX_AUTO_NEG_BIT         13
#define NETA_ENABLE_DUPLEX_AUTO_NEG_MASK        (1 << NETA_ENABLE_DUPLEX_AUTO_NEG_BIT)

/*-------------------------------------------------------------------------------*/

#define NETA_GMAC_STATUS_REG(p)                 (NETA_REG_BASE(p) + 0x2C10)

#define NETA_GMAC_LINK_UP_BIT               0
#define NETA_GMAC_LINK_UP_MASK              (1 << NETA_GMAC_LINK_UP_BIT)

#define NETA_GMAC_SPEED_1000_BIT            1
#define NETA_GMAC_SPEED_1000_MASK           (1 << NETA_GMAC_SPEED_1000_BIT)

#define NETA_GMAC_SPEED_100_BIT             2
#define NETA_GMAC_SPEED_100_MASK            (1 << NETA_GMAC_SPEED_100_BIT)

#define NETA_GMAC_FULL_DUPLEX_BIT           3
#define NETA_GMAC_FULL_DUPLEX_MASK          (1 << NETA_GMAC_FULL_DUPLEX_BIT)

#define NETA_RX_FLOW_CTRL_ENABLE_BIT        4
#define NETA_RX_FLOW_CTRL_ENABLE_MASK       (1 << NETA_RX_FLOW_CTRL_ENABLE_BIT)

#define NETA_TX_FLOW_CTRL_ENABLE_BIT        5
#define NETA_TX_FLOW_CTRL_ENABLE_MASK       (1 << NETA_TX_FLOW_CTRL_ENABLE_BIT)
/*-------------------------------------------------------------------------------*/

#define NETA_GMAC_SERIAL_REG(p)             (NETA_REG_BASE(p) + 0x2C14)

#define NETA_GMAC_FIFO_PARAM_0_REG(p)       (NETA_REG_BASE(p) + 0x2C18)
#define NETA_GMAC_FIFO_PARAM_1_REG(p)       (NETA_REG_BASE(p) + 0x2C1C)

#define NETA_GMAC_CAUSE_REG(p)              (NETA_REG_BASE(p) + 0x2C20)
#define NETA_GMAC_MASK_REG(p)               (NETA_REG_BASE(p) + 0x2C24)

#define NETA_GMAC_SERDES_CFG_0_REG(p)       (NETA_REG_BASE(p) + 0x2C28)
#define NETA_GMAC_SERDES_CFG_1_REG(p)       (NETA_REG_BASE(p) + 0x2C2C)
#define NETA_GMAC_SERDES_CFG_2_REG(p)       (NETA_REG_BASE(p) + 0x2C30)
#define NETA_GMAC_SERDES_CFG_3_REG(p)       (NETA_REG_BASE(p) + 0x2C34)

#define NETA_GMAC_PRBS_STATUS_REG(p)        (NETA_REG_BASE(p) + 0x2C38)
#define NETA_GMAC_PRBS_ERR_CNTR_REG(p)      (NETA_REG_BASE(p) + 0x2C3C)

#define NETA_GMAC_STATUS_1_REG(p)           (NETA_REG_BASE(p) + 0x2C40)

#define NETA_GMAC_MIB_CTRL_REG(p)           (NETA_REG_BASE(p) + 0x2C44)
#define NETA_GMAC_CTRL_3_REG(p)             (NETA_REG_BASE(p) + 0x2C48)

#define NETA_GMAC_QSGMII_REG(p)             (NETA_REG_BASE(p) + 0x2C4C)
#define NETA_GMAC_QSGMII_STATUS_REG(p)      (NETA_REG_BASE(p) + 0x2C50)
#define NETA_GMAC_QSGMII_ERR_CNTR_REG(p)    (NETA_REG_BASE(p) + 0x2C54)

/* 8 FC Timer registers: 0x2c58 .. 0x2c74 */
#define NETA_GMAC_FC_TIMER_REG(p, r)        (NETA_REG_BASE(p) + 0x2C58 + ((r) << 2))

/* 4 DSA Tag registers: 0x2c78 .. 0x2c84 */
#define NETA_GMAC_DSA_TAG_REG(p, r)         (NETA_REG_BASE(p) + 0x2C78 + ((r) << 2))

#define NETA_GMAC_FC_WIN_0_REG(p)           (NETA_REG_BASE(p) + 0x2C88)
#define NETA_GMAC_FC_WIN_1_REG(p)           (NETA_REG_BASE(p) + 0x2C8C)

#define NETA_GMAC_CTRL_4_REG(p)             (NETA_REG_BASE(p) + 0x2C90)

#define NETA_GMAC_SERIAL_1_REG(p)           (NETA_REG_BASE(p) + 0x2C94)

#define NETA_LOW_POWER_CTRL_0_REG(p)        (NETA_REG_BASE(p) + 0x2CC0)

/* Low Power Idle Control #1 register */
#define NETA_LOW_POWER_CTRL_1_REG(p)        (NETA_REG_BASE(p) + 0x2CC4)

#define NETA_LPI_REQUEST_EN_BIT             0
#define NETA_LPI_REQUEST_EN_MASK            (1 << NETA_LPI_REQUEST_EN_BIT)

#define NETA_LPI_REQUEST_FORCE_BIT          1
#define NETA_LPI_REQUEST_FORCE_MASK         (1 << NETA_LPI_REQUEST_FORCE_BIT)

#define NETA_LPI_MANUAL_MODE_BIT            2
#define NETA_LPI_MANUAL_MODE_MASK           (1 << NETA_LPI_MANUAL_MODE_BIT)
/*-------------------------------------------------------------------------------*/

#define NETA_LOW_POWER_CTRL_2_REG(p)        (NETA_REG_BASE(p) + 0x2CC8)
#define NETA_LOW_POWER_STATUS_REG(p)        (NETA_REG_BASE(p) + 0x2CCC)
#define NETA_LOW_POWER_CNTR_REG(p)          (NETA_REG_BASE(p) + 0x2CD0)


#define ETH_MIB_COUNTERS_BASE(port)         (MV_ETH_REGS_BASE(port) + 0x1000)
#define ETH_DA_FILTER_SPEC_MCAST_BASE(port) (MV_ETH_REGS_BASE(port) + 0x1400)
#define ETH_DA_FILTER_OTH_MCAST_BASE(port)  (MV_ETH_REGS_BASE(port) + 0x1500)
#define ETH_DA_FILTER_UCAST_BASE(port)      (MV_ETH_REGS_BASE(port) + 0x1600)

/* Phy address register definitions */
#define ETH_PHY_ADDR_OFFS          0
#define ETH_PHY_ADDR_MASK          (0x1f << ETH_PHY_ADDR_OFFS)

/* MIB Counters register definitions */
#define ETH_MIB_GOOD_OCTETS_RECEIVED_LOW    0x0
#define ETH_MIB_GOOD_OCTETS_RECEIVED_HIGH   0x4
#define ETH_MIB_BAD_OCTETS_RECEIVED         0x8
#define ETH_MIB_INTERNAL_MAC_TRANSMIT_ERR   0xc
#define ETH_MIB_GOOD_FRAMES_RECEIVED        0x10
#define ETH_MIB_BAD_FRAMES_RECEIVED         0x14
#define ETH_MIB_BROADCAST_FRAMES_RECEIVED   0x18
#define ETH_MIB_MULTICAST_FRAMES_RECEIVED   0x1c
#define ETH_MIB_FRAMES_64_OCTETS            0x20
#define ETH_MIB_FRAMES_65_TO_127_OCTETS     0x24
#define ETH_MIB_FRAMES_128_TO_255_OCTETS    0x28
#define ETH_MIB_FRAMES_256_TO_511_OCTETS    0x2c
#define ETH_MIB_FRAMES_512_TO_1023_OCTETS   0x30
#define ETH_MIB_FRAMES_1024_TO_MAX_OCTETS   0x34
#define ETH_MIB_GOOD_OCTETS_SENT_LOW        0x38
#define ETH_MIB_GOOD_OCTETS_SENT_HIGH       0x3c
#define ETH_MIB_GOOD_FRAMES_SENT            0x40
#define ETH_MIB_EXCESSIVE_COLLISION         0x44
#define ETH_MIB_MULTICAST_FRAMES_SENT       0x48
#define ETH_MIB_BROADCAST_FRAMES_SENT       0x4c
#define ETH_MIB_UNREC_MAC_CONTROL_RECEIVED  0x50
#define ETH_MIB_FC_SENT                     0x54
#define ETH_MIB_GOOD_FC_RECEIVED            0x58
#define ETH_MIB_BAD_FC_RECEIVED             0x5c
#define ETH_MIB_UNDERSIZE_RECEIVED          0x60
#define ETH_MIB_FRAGMENTS_RECEIVED          0x64
#define ETH_MIB_OVERSIZE_RECEIVED           0x68
#define ETH_MIB_JABBER_RECEIVED             0x6c
#define ETH_MIB_MAC_RECEIVE_ERROR           0x70
#define ETH_MIB_BAD_CRC_EVENT               0x74
#define ETH_MIB_COLLISION                   0x78
#define ETH_MIB_LATE_COLLISION              0x7c


/****************************************/
/*        Ethernet Unit Register BITs   */
/****************************************/
#if (MV_ETH_VERSION >= 4)
#define ETH_TX_EJP_RESET_BIT                0
#define ETH_TX_EJP_RESET_MASK               (1 << ETH_TX_EJP_RESET_BIT)

#define ETH_TX_EJP_ENABLE_BIT               2
#define ETH_TX_EJP_ENABLE_MASK              (1 << ETH_TX_EJP_ENABLE_BIT)

#define ETH_TX_LEGACY_WRR_BIT               3
#define ETH_TX_LEGACY_WRR_MASK              (1 << ETH_TX_LEGACY_WRR_BIT)
#endif /* (MV_ETH_VERSION >= 4) */


/***** BITs of Port Configuration reg (PxCR) *****/
#define ETH_UNICAST_PROMISCUOUS_MODE_BIT    0
#define ETH_UNICAST_PROMISCUOUS_MODE_MASK   (1<<ETH_UNICAST_PROMISCUOUS_MODE_BIT)

#define ETH_DEF_RX_QUEUE_OFFSET             1
#define ETH_DEF_RX_QUEUE_ALL_MASK           (0x7<<ETH_DEF_RX_QUEUE_OFFSET)
#define ETH_DEF_RX_QUEUE_MASK(queue)        ((queue)<<ETH_DEF_RX_QUEUE_OFFSET)

#define ETH_DEF_RX_ARP_QUEUE_OFFSET         4
#define ETH_DEF_RX_ARP_QUEUE_ALL_MASK       (0x7<<ETH_DEF_RX_ARP_QUEUE_OFFSET)
#define ETH_DEF_RX_ARP_QUEUE_MASK(queue)    ((queue)<<ETH_DEF_RX_ARP_QUEUE_OFFSET)

#define ETH_REJECT_NOT_IP_ARP_BCAST_BIT     7
#define ETH_REJECT_NOT_IP_ARP_BCAST_MASK    (1<<ETH_REJECT_NOT_IP_ARP_BCAST_BIT)

#define ETH_REJECT_IP_BCAST_BIT             8
#define ETH_REJECT_IP_BCAST_MASK            (1<<ETH_REJECT_IP_BCAST_BIT)

#define ETH_REJECT_ARP_BCAST_BIT            9
#define ETH_REJECT_ARP_BCAST_MASK           (1<<ETH_REJECT_ARP_BCAST_BIT)

#define ETH_TX_NO_SET_ERROR_SUMMARY_BIT     12
#define ETH_TX_NO_SET_ERROR_SUMMARY_MASK    (1<<ETH_TX_NO_SET_ERROR_SUMMARY_BIT)

#define ETH_CAPTURE_TCP_FRAMES_ENABLE_BIT   14
#define ETH_CAPTURE_TCP_FRAMES_ENABLE_MASK  (1<<ETH_CAPTURE_TCP_FRAMES_ENABLE_BIT)

#define ETH_CAPTURE_UDP_FRAMES_ENABLE_BIT   15
#define ETH_CAPTURE_UDP_FRAMES_ENABLE_MASK  (1<<ETH_CAPTURE_UDP_FRAMES_ENABLE_BIT)

#define ETH_DEF_RX_TCP_QUEUE_OFFSET         16
#define ETH_DEF_RX_TCP_QUEUE_ALL_MASK       (0x7<<ETH_DEF_RX_TCP_QUEUE_OFFSET)
#define ETH_DEF_RX_TCP_QUEUE_MASK(queue)    ((queue)<<ETH_DEF_RX_TCP_QUEUE_OFFSET)

#define ETH_DEF_RX_UDP_QUEUE_OFFSET         19
#define ETH_DEF_RX_UDP_QUEUE_ALL_MASK       (0x7<<ETH_DEF_RX_UDP_QUEUE_OFFSET)
#define ETH_DEF_RX_UDP_QUEUE_MASK(queue)    ((queue)<<ETH_DEF_RX_UDP_QUEUE_OFFSET)

#define ETH_DEF_RX_BPDU_QUEUE_OFFSET        22
#define ETH_DEF_RX_BPDU_QUEUE_ALL_MASK      (0x7<<ETH_DEF_RX_BPDU_QUEUE_OFFSET)
#define ETH_DEF_RX_BPDU_QUEUE_MASK(queue)   ((queue)<<ETH_DEF_RX_BPDU_QUEUE_OFFSET)

#define ETH_RX_CHECKSUM_MODE_OFFSET         25
#define ETH_RX_CHECKSUM_NO_PSEUDO_HDR       (0<<ETH_RX_CHECKSUM_MODE_OFFSET)
#define ETH_RX_CHECKSUM_WITH_PSEUDO_HDR     (1<<ETH_RX_CHECKSUM_MODE_OFFSET)

/***** BITs of Port Configuration Extend reg (PxCXR) *****/
#define ETH_CAPTURE_SPAN_BPDU_ENABLE_BIT    1
#define ETH_CAPTURE_SPAN_BPDU_ENABLE_MASK   (1<<ETH_CAPTURE_SPAN_BPDU_ENABLE_BIT)

#define ETH_TX_DISABLE_GEN_CRC_BIT          3
#define ETH_TX_DISABLE_GEN_CRC_MASK         (1<<ETH_TX_DISABLE_GEN_CRC_BIT)


/***** BITs of Port Sdma Configuration reg (SDCR) *****/
#define ETH_RX_FRAME_INTERRUPT_BIT          0
#define ETH_RX_FRAME_INTERRUPT_MASK         (1<<ETH_RX_FRAME_INTERRUPT_BIT)

#define ETH_BURST_SIZE_1_64BIT_VALUE        0
#define ETH_BURST_SIZE_2_64BIT_VALUE        1
#define ETH_BURST_SIZE_4_64BIT_VALUE        2
#define ETH_BURST_SIZE_8_64BIT_VALUE        3
#define ETH_BURST_SIZE_16_64BIT_VALUE       4

#define ETH_RX_BURST_SIZE_OFFSET            1
#define ETH_RX_BURST_SIZE_ALL_MASK          (0x7<<ETH_RX_BURST_SIZE_OFFSET)
#define ETH_RX_BURST_SIZE_MASK(burst)       ((burst)<<ETH_RX_BURST_SIZE_OFFSET)

#define ETH_RX_NO_DATA_SWAP_BIT             4
#define ETH_RX_NO_DATA_SWAP_MASK            (1<<ETH_RX_NO_DATA_SWAP_BIT)
#define ETH_RX_DATA_SWAP_MASK               (0<<ETH_RX_NO_DATA_SWAP_BIT)

#define ETH_TX_NO_DATA_SWAP_BIT             5
#define ETH_TX_NO_DATA_SWAP_MASK            (1<<ETH_TX_NO_DATA_SWAP_BIT)
#define ETH_TX_DATA_SWAP_MASK               (0<<ETH_TX_NO_DATA_SWAP_BIT)

#define ETH_DESC_SWAP_BIT                   6
#define ETH_DESC_SWAP_MASK                  (1<<ETH_DESC_SWAP_BIT)
#define ETH_NO_DESC_SWAP_MASK               (0<<ETH_DESC_SWAP_BIT)

#define ETH_RX_INTR_COAL_OFFSET             7
#define ETH_RX_INTR_COAL_ALL_MASK           (0x3fff<<ETH_RX_INTR_COAL_OFFSET)
#define ETH_RX_INTR_COAL_MASK(value)        (((value)<<ETH_RX_INTR_COAL_OFFSET)  \
	& ETH_RX_INTR_COAL_ALL_MASK)

#define ETH_TX_BURST_SIZE_OFFSET            22
#define ETH_TX_BURST_SIZE_ALL_MASK          (0x7<<ETH_TX_BURST_SIZE_OFFSET)
#define ETH_TX_BURST_SIZE_MASK(burst)       ((burst)<<ETH_TX_BURST_SIZE_OFFSET)

#define ETH_RX_INTR_COAL_MSB_BIT            25
#define ETH_RX_INTR_COAL_MSB_MASK           (1<<ETH_RX_INTR_COAL_MSB_BIT)

/* BITs Port #x Tx FIFO Urgent Threshold (PxTFUT) */
#define ETH_TX_INTR_COAL_OFFSET             4
#define ETH_TX_INTR_COAL_ALL_MASK           (0x3fff << ETH_TX_INTR_COAL_OFFSET)
#define ETH_TX_INTR_COAL_MASK(value)        (((value) << ETH_TX_INTR_COAL_OFFSET)  \
	& ETH_TX_INTR_COAL_ALL_MASK)


/* BITs of SDMA Descriptor Command/Status field */
#if defined(MV_CPU_BE) && !defined(MV_KW2_BE_ETH_WA)

#define ETH_GET_32B_DESCR_VAL(val)	(val)
#define ETH_GET_16B_DESCR_VAL(val)	(val)
#define ETH_SET_32B_DESCR_VAL(var, val)	((var) = (val))
#define ETH_SET_16B_DESCR_VAL(var, val)	((var) = (val))

typedef struct _ethRxDesc {
	MV_U16 byteCnt;	/* Descriptor buffer byte count     */
	MV_U16 bufSize;	/* Buffer size                      */
	MV_U32 cmdSts;	/* Descriptor command status        */
	MV_U32 nextDescPtr;	/* Next descriptor pointer          */
	MV_U32 bufPtr;	/* Descriptor buffer pointer        */
	MV_ULONG returnInfo;	/* User resource return information */
} ETH_RX_DESC;

typedef struct _ethTxDesc {
	MV_U16 byteCnt;	/* Descriptor buffer byte count     */
	MV_U16 L4iChk;	/* CPU provided TCP Checksum        */
	MV_U32 cmdSts;	/* Descriptor command status        */
	MV_U32 nextDescPtr;	/* Next descriptor pointer          */
	MV_U32 bufPtr;	/* Descriptor buffer pointer        */
	MV_ULONG returnInfo;	/* User resource return information */
	MV_U8 *alignBufPtr;	/* Pointer to 8 byte aligned buffer */
} ETH_TX_DESC;

#elif defined(MV_CPU_LE) || defined(MV_KW2_BE_ETH_WA)

#ifdef MV_KW2_BE_ETH_WA
#define ETH_GET_32B_DESCR_VAL(val)	MV_BYTE_SWAP_32BIT(val)
#define ETH_GET_16B_DESCR_VAL(val)	MV_BYTE_SWAP_16BIT(val)
#define ETH_SET_32B_DESCR_VAL(var, val) \
				(var = MV_BYTE_SWAP_32BIT(val))
#define ETH_SET_16B_DESCR_VAL(var, val) \
				(var = MV_BYTE_SWAP_16BIT(val))
#else
#define ETH_GET_32B_DESCR_VAL(val)	(val)
#define ETH_GET_16B_DESCR_VAL(val)	(val)
#define ETH_SET_32B_DESCR_VAL(var, val)	((var) = (val))
#define ETH_SET_16B_DESCR_VAL(var, val)	((var) = (val))
#endif

typedef struct _ethRxDesc {
	MV_U32 cmdSts;	/* Descriptor command status        */
	MV_U16 bufSize;	/* Buffer size                      */
	MV_U16 byteCnt;	/* Descriptor buffer byte count     */
	MV_U32 bufPtr;	/* Descriptor buffer pointer        */
	MV_U32 nextDescPtr;	/* Next descriptor pointer          */
	MV_ULONG returnInfo;	/* User resource return information */
} ETH_RX_DESC;

typedef struct _ethTxDesc {
	MV_U32 cmdSts;	/* Descriptor command status        */
	MV_U16 L4iChk;	/* CPU provided TCP Checksum        */
	MV_U16 byteCnt;	/* Descriptor buffer byte count     */
	MV_U32 bufPtr;	/* Descriptor buffer pointer        */
	MV_U32 nextDescPtr;	/* Next descriptor pointer          */
	MV_ULONG returnInfo;	/* User resource return information */
	MV_U8 *alignBufPtr;	/* Pointer to 32 byte aligned buffer */
} ETH_TX_DESC;

#else
#error "MV_CPU_BE or MV_CPU_LE must be defined"
#endif				/* MV_CPU_BE || MV_CPU_LE */

/* Buffer offset from buffer pointer */
#define ETH_RX_BUF_OFFSET               0x2


/* Tx & Rx descriptor bits */
#define ETH_ERROR_SUMMARY_BIT               0
#define ETH_ERROR_SUMMARY_MASK              (1<<ETH_ERROR_SUMMARY_BIT)

#define ETH_BUFFER_OWNER_BIT                31
#define ETH_BUFFER_OWNED_BY_DMA             (1<<ETH_BUFFER_OWNER_BIT)
#define ETH_BUFFER_OWNED_BY_HOST            (0<<ETH_BUFFER_OWNER_BIT)

/* Tx descriptor bits */
#define ETH_TX_ERROR_CODE_OFFSET            1
#define ETH_TX_ERROR_CODE_MASK              (3<<ETH_TX_ERROR_CODE_OFFSET)
#define ETH_TX_LATE_COLLISION_ERROR         (0<<ETH_TX_ERROR_CODE_OFFSET)
#define ETH_TX_UNDERRUN_ERROR               (1<<ETH_TX_ERROR_CODE_OFFSET)
#define ETH_TX_EXCESSIVE_COLLISION_ERROR    (2<<ETH_TX_ERROR_CODE_OFFSET)

#define ETH_TX_LLC_SNAP_FORMAT_BIT          9
#define ETH_TX_LLC_SNAP_FORMAT_MASK         (1<<ETH_TX_LLC_SNAP_FORMAT_BIT)

#define ETH_TX_IP_FRAG_BIT                  10
#define ETH_TX_IP_FRAG_MASK                 (1<<ETH_TX_IP_FRAG_BIT)
#define ETH_TX_IP_FRAG                      (0<<ETH_TX_IP_FRAG_BIT)
#define ETH_TX_IP_NO_FRAG                   (1<<ETH_TX_IP_FRAG_BIT)

#define ETH_TX_IP_HEADER_LEN_OFFSET         11
#define ETH_TX_IP_HEADER_LEN_ALL_MASK       (0xF<<ETH_TX_IP_HEADER_LEN_OFFSET)
#define ETH_TX_IP_HEADER_LEN_MASK(len)      ((len)<<ETH_TX_IP_HEADER_LEN_OFFSET)

#define ETH_TX_VLAN_TAGGED_FRAME_BIT        15
#define ETH_TX_VLAN_TAGGED_FRAME_MASK       (1<<ETH_TX_VLAN_TAGGED_FRAME_BIT)

#define ETH_TX_L4_TYPE_BIT                  16
#define ETH_TX_L4_TCP_TYPE                  (0<<ETH_TX_L4_TYPE_BIT)
#define ETH_TX_L4_UDP_TYPE                  (1<<ETH_TX_L4_TYPE_BIT)

#define ETH_TX_GENERATE_L4_CHKSUM_BIT       17
#define ETH_TX_GENERATE_L4_CHKSUM_MASK      (1<<ETH_TX_GENERATE_L4_CHKSUM_BIT)

#define ETH_TX_GENERATE_IP_CHKSUM_BIT       18
#define ETH_TX_GENERATE_IP_CHKSUM_MASK      (1<<ETH_TX_GENERATE_IP_CHKSUM_BIT)

#define ETH_TX_ZERO_PADDING_BIT             19
#define ETH_TX_ZERO_PADDING_MASK            (1<<ETH_TX_ZERO_PADDING_BIT)

#define ETH_TX_LAST_DESC_BIT                20
#define ETH_TX_LAST_DESC_MASK               (1<<ETH_TX_LAST_DESC_BIT)

#define ETH_TX_FIRST_DESC_BIT               21
#define ETH_TX_FIRST_DESC_MASK              (1<<ETH_TX_FIRST_DESC_BIT)

#define ETH_TX_GENERATE_CRC_BIT             22
#define ETH_TX_GENERATE_CRC_MASK            (1<<ETH_TX_GENERATE_CRC_BIT)

#define ETH_TX_ENABLE_INTERRUPT_BIT         23
#define ETH_TX_ENABLE_INTERRUPT_MASK        (1<<ETH_TX_ENABLE_INTERRUPT_BIT)

#define ETH_TX_AUTO_MODE_BIT                30
#define ETH_TX_AUTO_MODE_MASK               (1<<ETH_TX_AUTO_MODE_BIT)


/* Rx descriptor bits */
#define ETH_RX_ERROR_CODE_OFFSET            1
#define ETH_RX_ERROR_CODE_MASK              (3<<ETH_RX_ERROR_CODE_OFFSET)
#define ETH_RX_CRC_ERROR                    (0<<ETH_RX_ERROR_CODE_OFFSET)
#define ETH_RX_OVERRUN_ERROR                (1<<ETH_RX_ERROR_CODE_OFFSET)
#define ETH_RX_MAX_FRAME_LEN_ERROR          (2<<ETH_RX_ERROR_CODE_OFFSET)
#define ETH_RX_RESOURCE_ERROR               (3<<ETH_RX_ERROR_CODE_OFFSET)

#define ETH_RX_L4_CHECKSUM_OFFSET           3
#define ETH_RX_L4_CHECKSUM_MASK             (0xffff<<ETH_RX_L4_CHECKSUM_OFFSET)

#define ETH_RX_VLAN_TAGGED_FRAME_BIT        19
#define ETH_RX_VLAN_TAGGED_FRAME_MASK       (1<<ETH_RX_VLAN_TAGGED_FRAME_BIT)

#define ETH_RX_BPDU_FRAME_BIT               20
#define ETH_RX_BPDU_FRAME_MASK              (1<<ETH_RX_BPDU_FRAME_BIT)

#define ETH_RX_L4_TYPE_OFFSET               21
#define ETH_RX_L4_TYPE_MASK                 (3<<ETH_RX_L4_TYPE_OFFSET)
#define ETH_RX_L4_TCP_TYPE                  (0<<ETH_RX_L4_TYPE_OFFSET)
#define ETH_RX_L4_UDP_TYPE                  (1<<ETH_RX_L4_TYPE_OFFSET)
#define ETH_RX_L4_OTHER_TYPE                (2<<ETH_RX_L4_TYPE_OFFSET)

#define ETH_RX_NOT_LLC_SNAP_FORMAT_BIT      23
#define ETH_RX_NOT_LLC_SNAP_FORMAT_MASK     (1<<ETH_RX_NOT_LLC_SNAP_FORMAT_BIT)

#define ETH_RX_IP_FRAME_TYPE_BIT            24
#define ETH_RX_IP_FRAME_TYPE_MASK           (1<<ETH_RX_IP_FRAME_TYPE_BIT)

#define ETH_RX_IP_HEADER_OK_BIT             25
#define ETH_RX_IP_HEADER_OK_MASK            (1<<ETH_RX_IP_HEADER_OK_BIT)

#define ETH_RX_LAST_DESC_BIT                26
#define ETH_RX_LAST_DESC_MASK               (1<<ETH_RX_LAST_DESC_BIT)

#define ETH_RX_FIRST_DESC_BIT               27
#define ETH_RX_FIRST_DESC_MASK              (1<<ETH_RX_FIRST_DESC_BIT)

#define ETH_RX_UNKNOWN_DA_BIT               28
#define ETH_RX_UNKNOWN_DA_MASK              (1<<ETH_RX_UNKNOWN_DA_BIT)

#define ETH_RX_ENABLE_INTERRUPT_BIT         29
#define ETH_RX_ENABLE_INTERRUPT_MASK        (1<<ETH_RX_ENABLE_INTERRUPT_BIT)

#define ETH_RX_L4_CHECKSUM_OK_BIT           30
#define ETH_RX_L4_CHECKSUM_OK_MASK          (1<<ETH_RX_L4_CHECKSUM_OK_BIT)

/* Rx descriptor bufSize field */
#define ETH_RX_IP_FRAGMENTED_FRAME_BIT      2
#define ETH_RX_IP_FRAGMENTED_FRAME_MASK     (1<<ETH_RX_IP_FRAGMENTED_FRAME_BIT)

#define ETH_RX_BUFFER_MASK                  0xFFF8


/* Ethernet Cause Register BITs */
#define ETH_CAUSE_RX_READY_SUM_BIT          0
#define ETH_CAUSE_EXTEND_BIT                1

#define ETH_CAUSE_RX_READY_OFFSET           2
#define ETH_CAUSE_RX_READY_BIT(queue)       (ETH_CAUSE_RX_READY_OFFSET + (queue))
#define ETH_CAUSE_RX_READY_MASK(queue)      (1 << (ETH_CAUSE_RX_READY_BIT(queue)))

#define ETH_CAUSE_RX_ERROR_SUM_BIT          10
#define ETH_CAUSE_RX_ERROR_OFFSET           11
#define ETH_CAUSE_RX_ERROR_BIT(queue)       (ETH_CAUSE_RX_ERROR_OFFSET + (queue))
#define ETH_CAUSE_RX_ERROR_MASK(queue)      (1 << (ETH_CAUSE_RX_ERROR_BIT(queue)))

#define ETH_CAUSE_TX_END_BIT                19
#define ETH_CAUSE_SUM_BIT                   31

/* Ethernet Cause Extended Register BITs */
#define ETH_CAUSE_TX_BUF_OFFSET             0
#define ETH_CAUSE_TX_BUF_BIT(queue)         (ETH_CAUSE_TX_BUF_OFFSET + (queue))
#define ETH_CAUSE_TX_BUF_MASK(queue)        (1 << (ETH_CAUSE_TX_BUF_BIT(queue)))

#define ETH_CAUSE_TX_ERROR_OFFSET           8
#define ETH_CAUSE_TX_ERROR_BIT(queue)       (ETH_CAUSE_TX_ERROR_OFFSET + (queue))
#define ETH_CAUSE_TX_ERROR_MASK(queue)      (1 << (ETH_CAUSE_TX_ERROR_BIT(queue)))

#define ETH_CAUSE_PHY_STATUS_CHANGE_BIT     16
#define ETH_CAUSE_RX_OVERRUN_BIT            18
#define ETH_CAUSE_TX_UNDERRUN_BIT           19
#define ETH_CAUSE_LINK_STATE_CHANGE_BIT     20
#define ETH_CAUSE_INTERNAL_ADDR_ERR_BIT     23
#define ETH_CAUSE_EXTEND_SUM_BIT            31

/* Marvell Header Register */
/* Marvell Header register bits */
#define ETH_MVHDR_EN_BIT                    0
#define ETH_MVHDR_EN_MASK                   (1 << ETH_MVHDR_EN_BIT)

#define ETH_MVHDR_DAPREFIX_BIT              1
#define ETH_MVHDR_DAPREFIX_MASK             (0x3 << ETH_MVHDR_DAPREFIX_BIT)
#define ETH_MVHDR_DAPREFIX_PRI_1_2          (0x1 << ETH_MVHDR_DAPREFIX_BIT)
#define ETH_MVHDR_DAPREFIX_DBNUM_PRI        (0x2 << ETH_MVHDR_DAPREFIX_BIT)
#define ETH_MVHDR_DAPREFIX_SPID_PRI         (0x3 << ETH_MVHDR_DAPREFIX_BIT)

#define ETH_MVHDR_MHMASK_BIT                8
#define ETH_MVHDR_MHMASK_MASK               (0x3 << ETH_MVHDR_MHMASK_BIT)
#define ETH_MVHDR_MHMASK_8_QUEUE            (0x0 << ETH_MVHDR_MHMASK_BIT)
#define ETH_MVHDR_MHMASK_4_QUEUE            (0x1 << ETH_MVHDR_MHMASK_BIT)
#define ETH_MVHDR_MHMASK_2_QUEUE            (0x3 << ETH_MVHDR_MHMASK_BIT)


/* Relevant for 6183 ONLY */
#define ETH_UNIT_PORTS_PADS_CALIB_0_REG     (MV_ETH_REGS_BASE(0) + 0x0A0)
#define ETH_UNIT_PORTS_PADS_CALIB_1_REG     (MV_ETH_REGS_BASE(0) + 0x0A4)
#define ETH_UNIT_PORTS_PADS_CALIB_2_REG     (MV_ETH_REGS_BASE(0) + 0x0A8)
/* Ethernet Unit Ports Pads Calibration_REG (ETH_UNIT_PORTS_PADS_CALIB_x_REG)  */
#define ETH_ETHERNET_PAD_CLIB_DRVN_OFFS		0
#define ETH_ETHERNET_PAD_CLIB_DRVN_MASK		(0x1F << ETH_ETHERNET_PAD_CLIB_DRVN_OFFS)

#define ETH_ETHERNET_PAD_CLIB_DRVP_OFFS         5
#define ETH_ETHERNET_PAD_CLIB_DRVP_MASK         (0x1F << ETH_ETHERNET_PAD_CLIB_DRVP_OFFS)

#define ETH_ETHERNET_PAD_CLIB_TUNEEN_OFFS       16
#define ETH_ETHERNET_PAD_CLIB_TUNEEN_MASK       (0x1 << ETH_ETHERNET_PAD_CLIB_TUNEEN_OFFS)

#define ETH_ETHERNET_PAD_CLIB_LOCKN_OFFS        17
#define ETH_ETHERNET_PAD_CLIB_LOCKN_MASK        (0x1F << ETH_ETHERNET_PAD_CLIB_LOCKN_OFFS)

#define ETH_ETHERNET_PAD_CLIB_OFFST_OFFS        24
#define ETH_ETHERNET_PAD_CLIB_OFFST_MASK        (0x1F << ETH_ETHERNET_PAD_CLIB_OFFST_OFFS)

#define ETH_ETHERNET_PAD_CLIB_WR_EN_OFFS        31
#define ETH_ETHERNET_PAD_CLIB_WR_EN_MASK        (0x1  << ETH_ETHERNET_PAD_CLIB_WR_EN_OFFS)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCmvEthRegsh */
