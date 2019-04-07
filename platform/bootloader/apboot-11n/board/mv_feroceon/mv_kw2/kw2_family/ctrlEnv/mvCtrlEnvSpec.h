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

#ifndef __INCmvCtrlEnvSpech
#define __INCmvCtrlEnvSpech

#include "mvDeviceId.h"
#include "mvSysHwConfig.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MV_ARM_SOC
#define SOC_NAME_PREFIX			"MV88F"

/* units base and port numbers */
#ifdef MV_ASMLANGUAGE
#define MV_XOR_REGS_OFFSET(unit)	0x60800
#else
#define MV_XOR0_REGS_OFFSET		0x60800
#define MV_XOR_REGS_OFFSET(unit)	0x60800
#endif

#define MV_DRAM_REGS_OFFSET			0x0
#define MV_DEV_BUS_REGS_OFFSET			0x10000
#define MV_RTC_REGS_OFFSET			0x10300
#define MV_SPI_REGS_OFFSET(unit)		(0x10600 + (unit * 0x80))
#define MV_TWSI_SLAVE_REGS_OFFSET(chanNum)	(0x11000 + (chanNum * 0x100))
#define MV_UART_REGS_OFFSET(chanNum)		(0x12000 + (chanNum * 0x100))
#define MV_MPP_REGS_OFFSET			0x18000
#define MV_GPP_REGS_OFFSET_0			0x18100
#define MV_GPP_REGS_OFFSET(unit)		(0x18100 + ((unit) * 0x40))
#define MV_ETH_COMPLEX_OFFSET			0x18800
#define MV_CPUIF_REGS_OFFSET			0x20000
#define MAX_AHB_TO_MBUS_REG_BASE		0x20000
#define MV_CNTMR_REGS_OFFSET			0x20300
#define MV_DEV_PMU_REGS_OFFSET			0x1C000
#define MV_PMU_REGS_OFFSET			0x28000
#define MV_CESA_TDMA_REGS_OFFSET(chanNum)	(0x30000 + (chanNum * 0x2000))
#define MV_CESA_REGS_OFFSET(chanNum)		(0x3D000 + (chanNum * 0x2000))
#define MV_PEX_IF_REGS_OFFSET(pexIf)      	(0x40000 + (0x4000 * pexIf))
#define MV_USB_REGS_OFFSET(dev)       		0x50000

#if defined(CONFIG_MV_ETH_LEGACY)
#define MV_ETH_REGS_OFFSET(port)      		(((port) == 0) ? 0x72000 : 0x76000)
#else
#define MV_ETH_REGS_OFFSET(port)      		(((port) == 0) ? 0x70000 : 0x74000)
#endif /* CONFIG_MV_ETH_LEGACY */

#define MV_ETH_ONLY_REGS_OFFSET			0x72000
#define MV_SATA_REGS_OFFSET			0x80000
#define MV_SDMMC_REGS_OFFSET			0x90000
#define MV_PON_REGS_OFFSET			0xA0000
#define MV_EPON_MAC_REGS_OFFSET			0xA0000
#define MV_GPON_MAC_REGS_OFFSET			0xAC000
#define MV_BM_REGS_OFFSET			0xC0000
#define MV_PNC_REGS_OFFSET			0xC8000
#define MV_TDM_REGS_OFFSET			0xD0000
#define MV_COMM_UNIT_REGS_OFFSET		0xB0000
#define MV_NFC_REGS_OFFSET			0xF0000

/*#define MV_PDMA_REGS_BASE			0x0 */ /* Unused, for compilation purposes only */

#define INTER_REGS_SIZE				_1M

/* This define describes the TWSI interrupt bit and location */
#define TWSI_CPU_MAIN_INT_CAUSE_REG	0x20200
#define TWSI_CPU_HIGH_INT_CAUSE_REG	0x20210
#define TWSI0_CPU_MAIN_INT_BIT(ch)	((ch == 0) ? (1 << 18) : (1 << 25))
#define TWSI_SPEED			100000

#define MV_GPP_MAX_PINS			96
#define MV_GPP_MAX_GROUP    		3
#define MV_CNTMR_MAX_COUNTER 		2
#define MV_UART_MAX_CHAN		2

#define MV_XOR_6510_6530_MAX_UNIT	0
#define MV_XOR_6510_6530_MAX_CHAN	0
#define MV_XOR_MAX_UNIT			1
#define MV_XOR_MAX_CHAN         	2 /* total channels for all units together*/
#define MV_XOR_MAX_CHAN_PER_UNIT       	2 /* channels for units */

#define MV_SATA_6510_6530_MAX_CHAN	0
#define MV_SATA_6550_6560_MAX_CHAN	1

#define MV_6510_MPP_MAX_GROUP    	12
#define MV_6530_MPP_MAX_GROUP    	12
#define MV_6550_MPP_MAX_GROUP    	12
#define MV_6560_MPP_MAX_GROUP    	12
#define MV_65XX_MPP_MAX_GROUP		12

#define MV_DRAM_MAX_CS      		4
#define MV_SPI_MAX_CS			8
/* This define describes the maximum number of supported PCI\PCIX Interfaces*/
#define MV_PCI_MAX_IF		0
#define MV_PCI_START_IF		0

/* This define describes the maximum number of supported PEX Interfaces 	*/
#define MV_INCLUDE_PEX0
#define MV_DISABLE_PEX_DEVICE_BAR
#define MV_PEX_MAX_IF			2
#define MV_PEX_MAX_IF_6510		0
#define MV_PEX_START_IF			MV_PCI_MAX_IF

/* This define describes the maximum number of supported PCI Interfaces 	*/
#define MV_PCI_IF_MAX_IF		(MV_PEX_MAX_IF + MV_PCI_MAX_IF)

#define MV_IDMA_MAX_CHAN    		0

#define MV_USB_6510_6530_MAX_PORTS	0
#define MV_USB_MAX_PORTS		1

#define MV_65XX_NAND			1

#define MV_6510_6530_SDIO		0
#define MV_6550_6560_SDIO		1

#define MV_65XX_TS			0

#define MV_6510_TDM_MAX_PORTS		2
#define MV_6530_TDM_MAX_PORTS		32
#define MV_6550_TDM_MAX_PORTS		8
#define MV_6560_TDM_MAX_PORTS		8
#define MV_65XX_TDM			1

#define MV_DEVICE_MAX_CS      		4

/* Others */
#define PEX_HOST_BUS_NUM(pciIf)		(pciIf)
#define PEX_HOST_DEV_NUM(pciIf)		0

#define PCI_IO(pciIf)		((pciIf == 0) ? PEX0_IO : PEX1_IO)
#define PCI_MEM(pciIf, memNum)  ((pciIf == 0) ? PEX0_MEM0 : PEX1_MEM0)

/* CESA version #3: One channel, 2KB SRAM, TDMA, CHAIN Mode support */
#define MV_CESA_VERSION					3
#define MV_CESA_SRAM_SIZE               (2 * 1024)

/* This define describes the maximum number of supported Ethernet ports 	*/
#define MV_ETH_VERSION 					4 /* for Legacy mode */
#define MV_ETH_MAX_PORTS				3
#define MV_ETH_MAX_RXQ					8
#define MV_ETH_MAX_TXQ					8
#define MV_ETH_TX_CSUM_MAX_SIZE			1600
/* New GMAC module is used */
#define MV_ETH_GMAC_NEW	
/* New WRR/EJP module is used */
#define MV_ETH_WRR_NEW
/* New L2/3/4 PMT module */
#define MV_ETH_PMT_NEW

/* Suport MIB counters for PON port */
#define MV_PON_MIB_SUPPORT

#define MV_PON_PORT_ID					2
#define MV_ETH_TCAM_LINES				512	/* TCAM num of entries */
#define MV_ETH_PMT_SIZE					(4*1024) /* PMT entires */

/* This define describes the the support of USB 	*/
#define MV_USB_VERSION  		1

#define MV_SPI_VERSION			2

#define MV_INCLUDE_SDRAM_CS0
#define MV_INCLUDE_SDRAM_CS1
#define MV_INCLUDE_SDRAM_CS2
#define MV_INCLUDE_SDRAM_CS3

#define MV_INCLUDE_DEVICE_CS0
#define MV_INCLUDE_DEVICE_CS1
#define MV_INCLUDE_DEVICE_CS2
#define MV_INCLUDE_DEVICE_CS3

#define MPP_GROUP_1_MASK {\
	0xF0FFF000,	0x00FFFFFF,	0x00F00000	\
}

#define MPP_GROUP_1_TYPE {\
	{0,		0,		0},		/* Reserved for AUTO */ \
	{0x20000000,	0x00422222,	0x00000000},	/* TDM_2CH */ \
	{0x10555000,	0x00111301,	0x00400000},	/* TDM_32CH */ \
	{0x0,		0x0,		0x0},		/* MAC0 */ \
	{0x0,		0x0,		0x0},		/* MAC1 */ \
	{0x0,		0x0,		0x0},		/* SW_P5 */ \
	{0x0,		0x0,		0x0},		/* SW_P6 */ \
}

#define MPP_GROUP_2_MASK {\
	0xFF000000,	0xFFFFFFFF,	0x000FFFFF	\
}
#define MPP_GROUP_2_TYPE {\
	{0,		0,		0},		/* Reserved for AUTO */ \
	{0x0,		0x0,		0x0},		/* TDM_2CH */ \
	{0x0,		0x0,		0x00044000},	/* TDM_32CH */ \
	{0x22000000,	0x22222222,	0x00022222},	/* MAC0 */ \
	{0x0,		0x0,		0x0},		/* MAC1 */ \
	{0x0,		0x0,		0x0},		/* SW_P5 */ \
	{0x0,		0x0,		0x0},		/* SW_P6 */ \
}

#define MPP_GROUP_3_MASK {\
	0xFFF00000,	0xFFFFFFFF,	0x00000FFF	\
}
#define MPP_GROUP_3_TYPE {\
	{0, 0, 0},					/* Reserved for AUTO */ \
	{0x0,		0x0,		0x0},		/* TDM_2CH */ \
	{0x0,		0x0,		0x0},		/* TDM_32CH */ \
	{0x22200000,	0x22222222,	0x00000222},	/* MAC0 */ \
	{0x33300000,	0x33333333,	0x00000333},	/* MAC1 */ \
	{0x44400000,	0x44444444,	0x00000444},	/* SW_P5 */ \
	{0x88800000,	0x88888888,	0x00000888},	/* SW_P6 */ \
}


#ifndef MV_ASMLANGUAGE

#define TBL_UNUSED	0	/* Used to mark unused entry */

/* This enumerator defines the Marvell Units ID      */
typedef enum _mvUnitId {
	DRAM_UNIT_ID,
	PEX_UNIT_ID,
	ETH_GIG_UNIT_ID,
	USB_UNIT_ID,
	IDMA_UNIT_ID,
	XOR_UNIT_ID,
	SATA_UNIT_ID,
	TDM_2CH_UNIT_ID,
	TDM_32CH_UNIT_ID,
	UART_UNIT_ID,
	CESA_UNIT_ID,
	SPI_UNIT_ID,
	AUDIO_UNIT_ID,
	SDIO_UNIT_ID,
	TS_UNIT_ID,
	XPON_UNIT_ID,
	BM_UNIT_ID,
	PNC_UNIT_ID,
	NFC_UNIT_ID,
	MAX_UNITS_ID
} MV_UNIT_ID;

/* This enumerator describes the Marvell controller possible devices that   */
/* can be connected to its device interface.                                */
typedef enum _mvDevice {
#if defined(MV_INCLUDE_DEVICE_CS0)
	DEV_CS0 = 0,    /* Device connected to dev CS[0]    */
#endif
#if defined(MV_INCLUDE_DEVICE_CS1)
	DEV_CS1 = 1,        /* Device connected to dev CS[1]    */
#endif
#if defined(MV_INCLUDE_DEVICE_CS2)
	DEV_CS2 = 2,        /* Device connected to dev CS[2]    */
#endif
#if defined(MV_INCLUDE_DEVICE_CS3)
	DEV_CS3 = 3,        /* Device connected to dev CS[2]    */
#endif
	BOOT_CS,        /* Device connected to BOOT dev    */
	MV_DEV_MAX_CS = MV_DEVICE_MAX_CS
} MV_DEVICE;

/* This enumerator described the possible Controller paripheral targets.    */
/* Controller peripherals are designated memory/IO address spaces that the  */
/* controller can access. They are also refered as "targets"                */
typedef enum _mvTarget {
	TBL_TERM = -1, 	/* none valid target, used as targets list terminator*/
	SDRAM_CS0,	/* SDRAM chip select 0		*/
	SDRAM_CS1,	/* SDRAM chip select 1		*/
	SDRAM_CS2,	/* SDRAM chip select 2		*/
	SDRAM_CS3,	/* SDRAM chip select 3		*/
	DEVICE_CS0,	/* Device chip select 0		*/
	DEVICE_CS1,	/* Device chip select 1		*/
	DEVICE_CS2,	/* Device chip select 2		*/
	DEVICE_CS3,	/* Device chip select 3		*/
	PEX0_MEM,	/* PCI Express 0 Memory		*/
	PEX0_IO,	/* PCI Express 0 IO		*/
	PEX1_MEM,	/* PCI Express 1 Memory		*/
	PEX1_IO,	/* PCI Express 1 IO		*/
	INTER_REGS,	/* Internal registers		*/
	NAND_NOR_CS,	/* NAND_NOR_CS			*/
	SPI_CS0,	/* SPI_CS0			*/
	SPI_CS1,	/* SPI_CS1			*/
	SPI_CS2,	/* SPI_CS2			*/
	SPI_CS3,	/* SPI_CS3			*/
	SPI_CS4,	/* SPI_CS4			*/
	SPI_CS5,	/* SPI_CS5			*/
	SPI_CS6,	/* SPI_CS6			*/
	SPI_CS7,	/* SPI_CS7			*/
	SPI_B_CS0,	/* SPI_B_CS0			*/
	BOOT_ROM_CS,	/* BOOT_ROM_CS			*/
	DEV_BOOCS,	/* DEV_BOOCS			*/
	CRYPT1_ENG,	/* Crypto1 Engine		*/
	CRYPT2_ENG,	/* Crypto2 Engine		*/
	PNC_BM,	        /* PNC + BM 		        */
	ETH_CTRL,	/* ETH Controller		*/
	PON_CTRL,	/* PON Controller		*/
	NFC_CTRL,	/* NAND flash controller	*/
	MAX_TARGETS
} MV_TARGET;

#define TARGETS_DEF_ARRAY	{			\
	{0x0E, DRAM_TARGET_ID }, /* SDRAM_CS0 */	\
	{0x0D, DRAM_TARGET_ID }, /* SDRAM_CS1 */	\
	{0x0B, DRAM_TARGET_ID }, /* SDRAM_CS0 */	\
	{0x07, DRAM_TARGET_ID }, /* SDRAM_CS1 */	\
	{0x3E, DEV_TARGET_ID},   /* DEVICE_CS0 */	\
	{0x3D, DEV_TARGET_ID},   /* DEVICE_CS1 */	\
	{0x3D, DEV_TARGET_ID},   /* DEVICE_CS2 */	\
	{0x3D, DEV_TARGET_ID},   /* DEVICE_CS3 */	\
	{0xE8, PEX0_TARGET_ID }, /* PEX0_MEM */		\
	{0xE0, PEX0_TARGET_ID }, /* PEX0_IO */		\
	{0xD8, PEX0_TARGET_ID }, /* PEX1_MEM */		\
	{0xD0, PEX0_TARGET_ID }, /* PEX1_IO */		\
	{0xFF, 0xFF           }, /* INTER_REGS */	\
	{0x2F, DEV_TARGET_ID  }, /* NAND_NOR_CS */	\
	{0x1E, DEV_TARGET_ID  }, /* SPI_CS0 */		\
	{0x5E, DEV_TARGET_ID  }, /* SPI_CS1 */		\
	{0x9E, DEV_TARGET_ID  }, /* SPI_CS2 */		\
	{0xDE, DEV_TARGET_ID  }, /* SPI_CS3 */		\
	{0x1F, DEV_TARGET_ID  }, /* SPI_CS4 */		\
	{0x5F, DEV_TARGET_ID  }, /* SPI_CS5 */		\
	{0x9F, DEV_TARGET_ID  }, /* SPI_CS6 */		\
	{0xDF, DEV_TARGET_ID  }, /* SPI_CS7 */		\
	{0x1A, DEV_TARGET_ID},	 /* SPI_B_CS0 */	\
	{0x1D, DEV_TARGET_ID  }, /* BOOT_ROM_CS */	\
	{0x1E, DEV_TARGET_ID  }, /* DEV_BOOCS */	\
	{0x01, CRYPT_TARGET_ID}, /* CRYPT1_ENG */	\
	{0x05, CRYPT_TARGET_ID}, /* CRYPT2_ENG */	\
	{0x00, PNC_BM_TARGET_ID},/* PNC_BM */		\
	{0x00, ETH_TARGET_ID},	 /* ETH */		\
	{0x00, PON_TARGET_ID},	 /* PON */		\
	{0x00, NFC_TARGET_ID},	 /* NFC */		\
}


#define TARGETS_NAME_ARRAY	{		\
	"SDRAM_CS0",    /* SDRAM_CS0 */		\
	"SDRAM_CS1",    /* SDRAM_CS1 */		\
	"SDRAM_CS2",    /* SDRAM_CS1 */		\
	"SDRAM_CS3",    /* SDRAM_CS1 */		\
	"DEVICE_CS0",	/* DEVICE_CS0 */	\
	"DEVICE_CS1",	/* DEVICE_CS1 */	\
	"DEVICE_CS2",	/* DEVICE_CS2 */	\
	"DEVICE_CS3",	/* DEVICE_CS3 */	\
	"PEX0_MEM",	/* PEX0_MEM */		\
	"PEX0_IO",	/* PEX0_IO */		\
	"PEX1_MEM",	/* PEX1_MEM */		\
	"PEX1_IO",	/* PEX1_IO */		\
	"INTER_REGS",	/* INTER_REGS */	\
	"NAND_NOR_CS",	/* NAND_NOR_CS */	\
	"SPI_CS0",	/* SPI_CS0 */		\
	"SPI_CS1",	/* SPI_CS1 */		\
	"SPI_CS2",	/* SPI_CS2 */		\
	"SPI_CS3",	/* SPI_CS3 */		\
	"SPI_CS4",	/* SPI_CS4 */		\
	"SPI_CS5",	/* SPI_CS5 */		\
	"SPI_CS6",	/* SPI_CS6 */		\
	"SPI_CS7",	/* SPI_CS7 */		\
	"SPI_B_CS0",	/* SPI_B_CS0 */		\
	"BOOT_ROM_CS",	/* BOOT_ROM_CS */	\
	"DEV_BOOTCS",	/* DEV_BOOCS */		\
	"CRYPT1_ENG",	/* CRYPT1_ENG */	\
	"CRYPT2_ENG",	/* CRYPT2_ENG */	\
	"PNC_BM",	/* PNC_BM */		\
	"ETH_CTRL",	/* ETH_CTRL */		\
	"PON_CTRL",	/* PON_CTRL */		\
	"NFC_CTRL",	/* NFC_CTRL */		\
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCmvCtrlEnvSpech */
