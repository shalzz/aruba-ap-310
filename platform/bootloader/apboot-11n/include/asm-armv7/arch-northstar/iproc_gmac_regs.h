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

#ifndef	__IPROC_GMAC_REGS_H
#define	__IPROC_GMAC_REGS_H

#include "bcmgmacmib.h"

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)		_PADLINE(line)
#define	PAD				_XSTR(__LINE__)
#endif /* PAD */

/* We have 4 DMA TX channels */
#define	GMAC_NUM_DMA_TX		4

/* dma registers per channel(xmt or rcv) */
typedef volatile struct {
	uint32_t		control;		/* 0x00 enable, et al */
	uint32_t		ptr;			/* 0x04 last descriptor posted to chip */
	uint32_t		addrlow;		/* 0x08 descriptor ring base address low 32-bits (8K aligned) */
	uint32_t		addrhigh;		/* 0x0c descriptor ring base address bits 63:32 (8K aligned) */
	uint32_t		status0;		/* 0x10 current descriptor, xmt state */
	uint32_t		status1;		/* 0x14 active descriptor, xmt error */
	uint32_t		PAD[2];			/* 0x18-0x1c */
} dma64regs_t;

typedef volatile struct {
	dma64regs_t	dmaxmt;		/* dma tx */
	dma64regs_t	dmarcv;		/* dma rx */
} dma64_t;

/*
 * Host Interface Registers
 */
typedef volatile struct _gmacregs {
	uint32_t		dev_ctl;				/* 0x000 */
	uint32_t		dev_status;				/* 0x004 */
	uint32_t		PAD;					/* 0x008 */
	uint32_t		bist_status;  			/* 0x00c */
	uint32_t		PAD[4];					/* 0x010-0x01c */
	uint32_t		int_status;				/* 0x020 */
	uint32_t		int_mask;				/* 0x024 */
	uint32_t		gp_timer;				/* 0x028 */
	uint32_t		PAD[53];				/* 0x02c-0x0fc */
	uint32_t		int_recv_lazy;			/* 0x100 */
	uint32_t		flow_ctl_thresh; 		/* 0x104 */
	uint32_t		tx_arb_wrr_thresh; 		/* 0x108 */
	uint32_t		gmac_idle_cnt_thresh;	/* 0x10c */
	uint32_t		PAD[4];					/* 0x110-0x11c */
	uint32_t		fifo_access_addr;		/* 0x120 */
	uint32_t		fifo_access_byte;		/* 0x124 */
	uint32_t		fifo_access_data;		/* 0x128 */
	uint32_t		PAD[21];				/* 0x12c-0x17c */
	uint32_t		phy_access;				/* 0x180 */
	uint32_t		PAD;					/* 0x184 */
	uint32_t		phy_ctl;				/* 0x188 */
	uint32_t		txq_ctl;				/* 0x18c */
	uint32_t		rxq_ctl;				/* 0x190 */
	uint32_t		gpio_select;			/* 0x194 */
	uint32_t		gpio_output_en;			/* 0x198 */
	uint32_t		PAD;					/* 0x19c */
	uint32_t		txq_rxq_mem_ctl;		/* 0x1a0 */
	uint32_t		memory_ecc_status;		/* 0x1a4 */
	uint32_t		PAD[14];				/* 0x1a8-1dc */
	uint32_t		clk_ctl_status;			/* 0x1e0 */
	uint32_t		PAD;					/* 0x1e4 */
	uint32_t		pwr_ctl;				/* 0x1e8 */
	uint32_t		PAD[5];					/* 0x1ec-0x1fc */

	// DMA register
	dma64_t			dma_regs[GMAC_NUM_DMA_TX]; /* 0x200-0x2fc */

	/* GAMC MIB counters */
	gmacmib_t		mib;					/* 0x300-0x428 */
	uint32_t		PAD[245];				/* 0x42c-0x7fc */

	// UNIMAC registers
	uint32_t		unimac_version;			/* 0x800 */
	uint32_t		hd_bk_pctl;				/* 0x804 */
	uint32_t		cmd_cfg;				/* 0x808 */
	uint32_t		mac_addr_high;			/* 0x80c */
	uint32_t		mac_addr_low;			/* 0x810 */
	uint32_t		rx_max_length;			/* 0x814 */
	uint32_t		pause_quanta;			/* 0x818 */
	uint32_t		PAD[9];					/* 0x81c-x83c */
	uint32_t		sfd_offset;				/* 0x840 */
	uint32_t		mac_mode;				/* 0x844 */
	uint32_t		outer_tag;				/* 0x848 */
	uint32_t		inner_tag;				/* 0x84c */
	uint32_t		rx_pause_quanta_scale; 	/* 0x850 */
	uint32_t		tx_preamble; 			/* 0x854 */
	uint32_t		PAD;					/* 0x858 */
	uint32_t		tx_ipg_len;				/* 0x85c */
	uint32_t		PAD[168];				/* 0x860-0xafc */
	uint32_t		mac_pfc_type;			/* 0xb00 */
	uint32_t		mac_pfc_opcode;			/* 0xb04 */
	uint32_t		mac_pfc_da_0;			/* 0xb08 */
	uint32_t		mac_pfc_da_1;			/* 0xb0c */
	uint32_t		macsec_prog_tx_crc;		/* 0xb10 */
	uint32_t		macsec_ctl;				/* 0xb14 */
	uint32_t		ts_status_ctl; 			/* 0xb18 */
	uint32_t		tx_ts_data; 			/* 0xb1c */
	uint32_t		PAD[4];					/* 0xb20-0xb2c */
	uint32_t		pause_ctl;				/* 0xb30 */
	uint32_t		flush_ctl;				/* 0xb34 */
	uint32_t		rx_fifo_status;			/* 0xb38 */
	uint32_t		tx_fifo_status;			/* 0xb3c */
	uint32_t		mac_pfc_ctl;			/* 0xb40 */
	uint32_t		mac_pfc_refresh_ctl;	/* 0xb44 */
} gmacregs_t;

#define	GM_MIB_BASE			0x300
#define	GM_MIB_LIMIT		0x800

/*
 * register-specific flag definitions
 */

/* device control */
#define	DC_TSM				0x00000001
#define	DC_ROCS				0x00000002
#define	DC_CFCO				0x00000004
#define	DC_RLSS				0x00000008
#define	DC_MROR				0x00000010
#define	DC_FCM_MASK			0x00000060
#define	DC_FCM_SHIFT		5
#define	DC_NAE				0x00000080
#define	DC_TF				0x00000100
#define	DC_RDS_MASK			0x00030000
#define	DC_RDS_SHIFT		16
#define	DC_TDS_MASK			0x000c0000
#define	DC_TDS_SHIFT		18

/* device status */
#define	DS_RBF				0x00000001
#define	DS_RDF				0x00000002
#define	DS_RIF				0x00000004
#define	DS_TBF				0x00000008
#define	DS_TDF				0x00000010
#define	DS_TIF				0x00000020
#define	DS_PO				0x00000040
#define	DS_MM_MASK			0x00000300
#define	DS_MM_SHIFT			8

/* bist status */
#define	BS_MTF				0x00000001
#define	BS_MRF				0x00000002
#define	BS_TDB				0x00000004
#define	BS_TIB				0x00000008
#define	BS_TBF				0x00000010
#define	BS_RDB				0x00000020
#define	BS_RIB				0x00000040
#define	BS_RBF				0x00000080
#define	BS_SF				0x00000800

/* interrupt status and mask registers */
#define	I_MRO				0x00000001
#define	I_MTO				0x00000002
#define	I_TFD				0x00000004
#define	I_LS				0x00000008
#define	I_MDIO				0x00000010
#define	I_MR				0x00000020
#define	I_MT				0x00000040
#define	I_TO				0x00000080
#define	I_SLSC				0x00000100
#define	I_PDEE				0x00000400
#define	I_PDE				0x00000800
#define	I_DE				0x00001000
#define	I_RDU				0x00002000
#define	I_RFO				0x00004000
#define	I_XFU				0x00008000
#define	I_RI				0x00010000
#define	I_XI0				0x01000000
#define	I_XI1				0x02000000
#define	I_XI2				0x04000000
#define	I_XI3				0x08000000
#define	I_INTMASK			0x0f01fcff
#define	I_ERRMASK			0x0000fc00

/* interrupt receive lazy */
#define	IRL_TO_MASK			0x00ffffff
#define	IRL_FC_MASK			0xff000000
#define	IRL_FC_SHIFT		24

/* flow control thresholds */
#define	FCT_TT_MASK			0x00000fff
#define	FCT_RT_MASK			0x0fff0000
#define	FCT_RT_SHIFT		16

/* txq aribter wrr thresholds */
#define	WRRT_Q0T_MASK		0x000000ff
#define	WRRT_Q1T_MASK		0x0000ff00
#define	WRRT_Q1T_SHIFT		8
#define	WRRT_Q2T_MASK		0x00ff0000
#define	WRRT_Q2T_SHIFT		16
#define	WRRT_Q3T_MASK		0xff000000
#define	WRRT_Q3T_SHIFT		24

/* phy access */
#define	PA_DATA_MASK		0x0000ffff
#define	PA_ADDR_MASK		0x001f0000
#define	PA_ADDR_SHIFT		16
#define	PA_REG_MASK			0x1f000000
#define	PA_REG_SHIFT		24
#define	PA_WRITE			0x20000000
#define	PA_START			0x40000000

/* phy control */
#define	PC_EPA_MASK			0x0000001f
#define	PC_MCT_MASK			0x007f0000
#define	PC_MCT_SHIFT		16
#define	PC_MTE				0x00800000

/* rxq control */
#define	RC_DBT_MASK			0x00000fff
#define	RC_DBT_SHIFT		0
#define	RC_PTE				0x00001000
#define	RC_MDP_MASK			0x3f000000
#define	RC_MDP_SHIFT		24

#define RC_MAC_DATA_PERIOD	9

/* txq control */
#define	TC_DBT_MASK			0x00000fff
#define	TC_DBT_SHIFT		0

/* gpio select */
#define	GS_GSC_MASK			0x0000000f
#define	GS_GSC_SHIFT		0

/* gpio output enable */
#define	GS_GOE_MASK			0x0000ffff
#define	GS_GOE_SHIFT		0

/* clk control status */
#define CS_FA				0x00000001
#define CS_FH				0x00000002
#define CS_FI				0x00000004
#define CS_AQ				0x00000008
#define CS_HQ				0x00000010
#define CS_FC				0x00000020
#define CS_ER				0x00000100
#define CS_AA				0x00010000
#define CS_HA				0x00020000
#define CS_BA				0x00040000
#define CS_BH				0x00080000
#define CS_ES				0x01000000

/* command config */
#define	CC_TE			0x00000001
#define	CC_RE			0x00000002
#define	CC_ES_MASK		0x0000000c
#define	CC_ES_SHIFT		2
#define	CC_PROM			0x00000010
#define	CC_PAD_EN		0x00000020
#define	CC_CF			0x00000040
#define	CC_PF			0x00000080
#define	CC_RPI			0x00000100
#define	CC_TAI			0x00000200
#define	CC_HD			0x00000400
#define	CC_HD_SHIFT		10
#define	CC_SR			0x00002000
#define	CC_ML			0x00008000
#define	CC_AE			0x00400000
#define	CC_CFE			0x00800000
#define	CC_NLC			0x01000000
#define	CC_RL			0x02000000
#define	CC_RED			0x04000000
#define	CC_PE			0x08000000
#define	CC_TPI			0x10000000
#define	CC_AT			0x20000000

/* mac addr high */
#define	MH_HI_MASK			0xffff
#define	MH_HI_SHIFT			16
#define	MH_MID_MASK			0xffff
#define	MH_MID_SHIFT		0

/* mac addr low */
#define	ML_LO_MASK			0xffff
#define	ML_LO_SHIFT			0

/* Core specific control flags */
#define SICF_SWCLKE			0x0004
#define SICF_SWRST			0x0008

/* Core specific status flags */
#define SISF_SW_ATTACHED	0x0800

#endif /*__IPROC_GMAC_REGS_H */