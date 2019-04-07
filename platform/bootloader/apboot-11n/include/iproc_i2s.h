/*****************************************************************************
Copyright 2012 Broadcom Corporation.  All rights reserved.
This program is the proprietary software of Broadcom Corporation and/or its 
licensors, and may only be used, duplicated, modified or distributed pursuant to
the terms and conditions of a separate, written license agreement executed 
between you and Broadcom (an "Authorized License").  Except as set forth in an 
Authorized License, Broadcom grants no license (express or implied), right to 
use, or waiver of any kind with respect to the Software, and Broadcom expressly
reserves all rights in and to the Software and all intellectual property rights
therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL
USE OF THE SOFTWARE.  
 
Except as expressly set forth in the Authorized License,
 
1.     This program, including its structure, sequence and organization, 
constitutes the valuable trade secrets of Broadcom, and you shall use all 
reasonable efforts to protect the confidentiality thereof, and to use this 
information only in connection with your use of Broadcom integrated circuit 
products.
 
2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR 
CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
PERFORMANCE OF THE SOFTWARE.

3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE 
OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY
PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED 
REMEDY.
*****************************************************************************/

#ifndef IPROC_I2S_H
#define IPROC_I2S_H

// I2S block register definitions

#define I2S_DEVCONTROL_REG			(0x000)
#define I2S_DEVSTATUS_REG			(0x004)
#define I2S_INTSTATUS_REG			(0x020)
#define I2S_INTMASK_REG				(0x024)
#define I2S_GPTIMER_REG				(0x028)
#define	I2S_CONTROL_REG				(0x030)
#define I2S_CLOCKDIVIDER_REG		(0x034)
#define I2S_TXPLAYTH_REG			(0x040)
#define I2S_FIFOCOUNTER_REG			(0x044)
#define I2S_STXCTRL_REG				(0x050)
#define I2S_SRXCTRL_REG				(0x054)
#define I2S_SRXPCNT_REG				(0x058)
#define I2S_STXCHSTATUS0_REG		(0x05C)
#define I2S_STXCHSTATUS1_REG		(0x060)
#define I2S_SRXCHSTATUS0_REG		(0x064)
#define I2S_SRXCHSTATUS1_REG		(0x068)
#define I2S_TDMCONTROL_REG			(0x070)
#define I2S_TDMCH0_CTRL_REG			(0x074)
#define I2S_TDMCH1_CTRL_REG			(0x078)
#define I2S_INTRECELAZY_DMA0_REG	(0x100)
#define I2S_INTRECELAZY_DMA1_REG	(0x104)
#define I2S_GPIOSEL_REG				(0X194)
#define I2S_GPIOOUTPUTEN_REG		(0x198)
#define I2S_CLOCKCONTROL_REG		(0x1e0)
#define I2S_DMA0_XMTCONTROL_REG		(0x200)
#define I2S_DMA0_XMTPTR_REG			(0x204)
#define I2S_DMA0_XMTADDRLOW_REG		(0x208)
#define I2S_DMA0_XMTADDRHIGH_REG	(0x20C)
#define I2S_DMA0_XMTSTATUS0_REG		(0x210)
#define I2S_DMA0_XMTSTATUS1_REG		(0x214)
#define I2S_DMA0_RCVCONTROL_REG		(0x220)
#define I2S_DMA0_RCVPTR_REG			(0x224)
#define I2S_DMA0_RCVADDRLOW_REG		(0x228)
#define I2S_DMA0_RCVADDRHIGH_REG	(0x22C)
#define I2S_DMA0_RCVSTATUS0_REG		(0x230)
#define I2S_DMA0_RCVSTATUS1_REG		(0x234)
#define I2S_DMA1_XMTCONTROL_REG		(0x240)
#define I2S_DMA1_XMTPTR_REG			(0x244)
#define I2S_DMA1_XMTADDRLOW_REG		(0x248)
#define I2S_DMA1_XMTADDRHIGH_REG	(0x24C)
#define I2S_DMA1_XMTSTATUS0_REG		(0x250)
#define I2S_DMA1_XMTSTATUS1_REG		(0x254)
#define I2S_DMA1_RCVCONTROL_REG		(0x260)
#define I2S_DMA1_RCVPTR_REG			(0x264)
#define I2S_DMA1_RCVADDRLOW_REG		(0x268)
#define I2S_DMA1_RCVADDRHIGH_REG	(0x26C)
#define I2S_DMA1_RCVSTATUS0_REG		(0x270)
#define I2S_DMA1_RCVSTATUS1_REG		(0x274)
#define I2S_FIFOADDRESS_REG			(0x280)
#define I2S_FIFODATA_REG			(0x284)

// I2S Register Bitmasks

#define I2S_DC_TDM_SEL_BIT      4
#define I2S_DC_BCLKD_BIT        5
#define I2S_DC_OPCHSEL_BIT      7
#define I2S_DC_DPX_BIT          8 
#define I2S_DC_WL_TX_BIT        10
#define I2S_DC_WL_RX_BIT        12
#define I2S_DC_TX8_BIT          14
#define I2S_DC_RX8_BIT          15

// Device Control Register
#define I2S_DC_TDM_SEL			(0x00000010)		// TDM select
#define I2S_DC_BCLKD_IN			(0x00000020)		// Bitclock direction
#define I2S_DC_I2SCFG			(0x00000040)		// Core SRAM for half duplex
#define I2S_DC_OPCHSEL			(0x00000080)		// Select Operation Channel
#define I2S_DC_WL_TX8			(0x00004000)		// TX 8-bit word length
#define I2S_DC_WL_RX8			(0x00008000)		// RX 8-bit word length

#define I2S_DC_WL_TXRX_MASK		(0x0000FC00)		// TX/RX word length mask
#define I2S_DC_DPX_SHIFT		(8)
#define I2S_DC_DPX_MASK			(0x3<<I2S_DC_DPX_SHIFT)	// Duplex mask

// Device Status Register
#define I2S_DS_BUSY				(0x00000010)		// BUSY
#define I2S_DS_TNF_CHO			(0x00000100)		// TX FIFO not full
#define I2S_DS_TXUNDER_CHO		(0x00000200)		// TX FIFO underflow channel 0
#define I2S_DS_TXOVER_CHO		(0x00000400)		// TX FIFO overflow channel 0
#define I2S_DS_PLAYING			(0x00000800)		// I2S PLAYING
#define I2S_DS_RNE_CH0			(0x00001000)		// RX FIFO not empty
#define I2S_DS_RXUNDER_CH0		(0x00002000)		// RX FIFO underflow bit channel 0
#define I2S_DS_RXOVER_CH0		(0x00004000)		// RX FIFO overflow bit channel 0
#define I2S_DS_RECORDING		(0x00008000)		// I2S RECORDING

// I2S Control Register
#define I2S_CONTROL_PLAYEN		(0x00000001)		// Play enable
#define I2S_CONTROL_RECEN		(0x00000002)		// Record enable
#define I2S_CONTROL_CLSLM		(0x00000008)		// Data puts close to LSB/MSB
#define I2S_CONTROL_TXLRCHSEL	(0x00000010)		// Select transmit Left/right channel first
#define I2S_CONTROL_RXLRCHSEL	(0x00000020)		// Select receive Left/right channel first
#define I2S_CONTROL_MUTE		(0x00000010)		// Transmit mute


// SPDIF Transmitter Control Register
#define I2S_STXC_WL_MASK		(0x00000003)		// Word length mask
#define I2S_STXC_STX_EN			(0x00000004)		// Enable/disable TX path
#define	I2S_STXC_SPDIF_CHCODE	(0x00000008)		// Select Preamble cell-order

// CLKDIV Register
#define I2S_CLKDIV_SRATE_SHIFT		(0)
#define I2S_CLKDIV_SRATE_MASK		(0xF<<I2S_CLKDIV_SRATE_SHIFT)

/* IntStatus Register               */
/* IntMask Register                 */
#define I2S_INT_GPTIMERINT		(1<<7)	/* General Purpose Timer Int/IntEn */
#define I2S_INT_DESCERR			(1<<10)	/* Descriptor Read Error/ErrorEn */
#define I2S_INT_DATAERR			(1<<11)	/* Descriptor Data Transfer Error/ErrorEm */
#define I2S_INT_DESC_PROTO_ERR		(1<<12)	/* Descriptor Protocol Error/ErrorEn */
#define I2S_INT_RCVFIFO_OFLOW		(1<<14)	/* Receive FIFO Overflow */
#define I2S_INT_XMTFIFO_UFLOW		(1<<15)	/* Transmit FIFO Overflow */
#define I2S_INT_RCV_INT			(1<<16)	/* Receive Interrupt */
#define I2S_INT_XMT_INT			(1<<24)	/* Transmit Interrupt */
#define I2S_INT_RXSIGDET		(1<<26)	/* Receive signal toggle */
#define I2S_INT_SPDIF_PAR_ERR		(1<<27)	/* SPDIF Rx parity error */
#define I2S_INT_VALIDITY		(1<<28)	/* SPDIF Rx Validity bit interrupt */
#define I2S_INT_CHSTATUS		(1<<29)	/* SPDIF Rx channel status interrupt */
#define I2S_INT_LATE_PREAMBLE		(1<<30)	/* SPDIF Rx preamble not detected */
#define I2S_INT_SPDIF_VALID_DATA	(1<<31)	/* SPDIF Rx Valid data */

/*  I2S FIFOCounter Register             */
/* TX Fifo data counter in 4-byte units */
#define I2S_FC_TX_CNT_SHIFT		(0)
#define I2S_FC_TX_CNT_MASK		(0xFF<<I2S_FC_TX_CNT_SHIFT)
/* RX Fifo data counter in 4-byte units */
#define I2S_FC_RX_CNT_SHIFT		(8)
#define I2S_FC_RX_CNT_MASK		(0xFF<<I2S_FC_RX_CNT_SHIFT)

/* I2S DMA Registers */
#define I2S_DMA0_TX_BASE			0x1802a200	
#define I2S_DMA0_RX_BASE			0x1802a220	

#define AUDIO_DEST_HDMI				(2)
#define AUDIO_DEST_LOCAL			(0)

// IDM Register defines
#define ROM_S0_IDM_IO_STATUS			0x1810D500
#define I2S_M0_IDM_IO_CONTROL_DIRECT 	0x18117408
#define I2S_M0_IDM_RESET_CONTROL 		0x18117800
#define I2S_M0_IDM_RESET_STATUS 		0x18117804

// IDM Register Masks
#define I2S_IDM_CD_CLK_ENABLE				0x1
#define I2S_IDM_CD_CLK_DIV					0x40

// intReceLazy Register
#define I2S_INTRECE_LAZY_FC				(1<<24)		// Interrupt when rx framecount is 1


/* WM8955 Reg Definition */

#define WM8955_RESET    0xf

#define DAC_CONTROL_REG  0x5

#define  SIXDB_ATTENUATE_ENABLE 0x80
#define  NO_SOFT_MUTE           (0<<3)
#define  SOFT_MUTE              (1<<3)
#define  SAMPLE_RATE_48Khz      0x6
#define  SAMPLE_RATE_44_1Khz    0x4
#define  SAMPLE_RATE_32Khz      0x2


#define LOUT2_OUTPUT_REG        0x28
#define ROUT2_OUTPUT_REG        0x29

#define MIN_MINUS_67_DB         0x30
#define MAX_6_DB                0x7F
#define DB_INC_STEP             0x1
#define zero_cross_enable       0x80
#define VOLUME_UPDATE           (1<<8)


#define AUDIO_INTERFACE_FORMAT_REG  0x7

#define  DSP_FORMAT                0x3
#define  I2S_FORMAT                0x2
#define  LEFT_JUSTIFIED            0x1
#define  RIGHT_JUSTIFIED           0x0

#define  AUDIO_DATA_32bit          (0x3 << 2)
#define  AUDIO_DATA_24bit          (0x2 << 2)
#define  AUDIO_DATA_20bit          (0x1 << 2)
#define  AUDIO_DATA_16bit          (0x0 << 2)

#define RIGHT_CH_DACLRC_HIGH       (1 << 4 )
#define MSB_ON_2nd_CLK             (1 << 4 )
#define MASTER_MODE                ( 1 << 6 )
#define SLAVE_MODE                 ( 0 << 6 )
#define BCLK_INVERT                ( 1 << 7 )

#define  CLK_PLL_REG               0x2B

#define CLK_FROM_PLL               ( 1 << 8 )
#define CLK_OUT_ENABLE             ( 1 << 7 )
#define CLK_OUT_SRC_PLL            ( 1 << 6 )
#define PLL_OUT_DIV_BY_2           ( 1 << 5 )
#define PLL_ENABLE                 ( 1 << 3 )

#define SAMPLE_RATE_REG            0x8

#define USB_MODE                  ( 1 << 0 )
#define NORMAL_MODE               ( 0 << 0 )
#define SAMPLE_RATE_CONTROL       ( 0 << 1 )
#define MCLK_DIV_BY_2             ( 1 << 6 )
#define CLKOUT_DIV_BY_2           ( 1 << 8 )


#define POWER_MANAGEMENT_REG2     0x1A

#define DAC_LEFT_ON               (1 << 8 )
#define DAC_RIGHT_ON              (1 << 7 )
#define LOUT2_BUF_ON              (1 << 4 )
#define ROUT2_BUF_ON              (1 << 3 )

#define MIXER_LEFT_1              0x22
#define MIXER_RIGHT_2             0x25


/* WM8750 register space */

#define WM8750_LINVOL    0x00
#define WM8750_RINVOL    0x01
#define WM8750_LOUT1V    0x02
#define WM8750_ROUT1V    0x03
#define WM8750_ADCDAC    0x05
#define WM8750_IFACE     0x07
#define WM8750_SRATE     0x08
#define WM8750_LDAC      0x0a
#define WM8750_RDAC      0x0b
#define WM8750_BASS      0x0c
#define WM8750_TREBLE    0x0d
#define WM8750_RESET     0x0f
#define WM8750_3D        0x10
#define WM8750_ALC1      0x11
#define WM8750_ALC2      0x12
#define WM8750_ALC3      0x13
#define WM8750_NGATE     0x14
#define WM8750_LADC      0x15
#define WM8750_RADC      0x16
#define WM8750_ADCTL1    0x17
#define WM8750_ADCTL2    0x18
#define WM8750_PWR1      0x19
#define WM8750_PWR2      0x1a
#define WM8750_ADCTL3    0x1b
#define WM8750_ADCIN     0x1f
#define WM8750_LADCIN    0x20
#define WM8750_RADCIN    0x21
#define WM8750_LOUTM1    0x22
#define WM8750_LOUTM2    0x23
#define WM8750_ROUTM1    0x24
#define WM8750_ROUTM2    0x25
#define WM8750_MOUTM1    0x26
#define WM8750_MOUTM2    0x27
#define WM8750_LOUT2V    0x28
#define WM8750_ROUT2V    0x29
#define WM8750_MOUTV     0x2a

#define WM8750_CACHE_REGNUM 0x2a

#define WM8750_SYSCLK	0

#define WM8750_LINVOL_0DB	0x17



//Defines for WM8955
#define WM8955_RESET     0x0f
#define WM8955_PWR1      0x19
#define WM8955_L_MIXER_1 0x22
#define WM8955_R_MIXER_2 0x25
#define WM8955_PWR2      0x1a
#define WM8955_DAC_CTRL  0x5
#define WM8955_SRATE_CTRL 0x8
#define CLK_PLL_CTRL      0x2b
#define AUDIO_INT_CTRL    0x7


extern int iproc_i2s_enable_pll(void);
extern int iproc_i2s_init(int op);
extern void iproc_i2s_reset(void);
extern void iproc_i2s_refill_fifo(void);

extern void iproc_i2s_check_dma(void);
extern void iproc_i2s_DMA_TX_Enable(void);
extern void iproc_i2s_DMA_TX_init(void);
extern int iproc_i2s_tx_test(void);
extern int iproc_i2s_record(void);

extern void iproc_i2s_recording_start(void);
extern void iproc_i2s_playback_recorded_audio(void);

extern void iproc_PCM_DMA_tx(void);


extern void WM8750_disable_ADC_data( void );
extern void WM8750_configure_format_mode(void);
extern void WM8750_configure_volume ( void );
extern void WM8750_configure_sigal_path( void );
extern void WM8750_poweron_channels( void );
extern void WM8750_reset_codec ( void );
extern int WM8750_SPI_Enable( void );


extern int WM8955_SPI_Enable( void );
extern void WM8955_reset_codec ( void );
extern void WM8955_poweron_channels( void );
extern void WM8955_configure_sigal_path( void );
extern void WM8955_configure_volume ( void );
extern void WM8955_configure_format_mode(void);




#endif
