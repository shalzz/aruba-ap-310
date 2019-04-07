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

    * 	Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *	Neither the name of Marvell nor the names of its contributors may be
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

#ifndef __INCmvBoardEnvSpech
#define __INCmvBoardEnvSpech

#include "mvSysHwConfig.h"

/* For future use */
#define BD_ID_DATA_START_OFFS			0x0
#define BD_DETECT_SEQ_OFFS			0x0
#define BD_SYS_NUM_OFFS				0x4
#define BD_NAME_OFFS				0x8

/* I2C bus addresses */
#define MV_BOARD_CTRL_I2C_ADDR			0x0	/* Controller slave addr */
#define MV_BOARD_CTRL_I2C_ADDR_TYPE 		ADDR7_BIT
#define MV_BOARD_DIMM0_I2C_ADDR			0x56
#define MV_BOARD_DIMM0_I2C_ADDR_TYPE 		ADDR7_BIT
#define MV_BOARD_DIMM1_I2C_ADDR			0x54
#define MV_BOARD_DIMM1_I2C_ADDR_TYPE 		ADDR7_BIT
#define MV_BOARD_EEPROM_I2C_ADDR	    	0x51
#define MV_BOARD_EEPROM_I2C_ADDR_TYPE 		ADDR7_BIT
#define MV_BOARD_MAIN_EEPROM_I2C_ADDR	   	0x50
#define MV_BOARD_MAIN_EEPROM_I2C_ADDR_TYPE 	ADDR7_BIT
#define MV_BOARD_MUX_I2C_ADDR_ENTRY		0x2
#define MV_BOARD_DIMM_I2C_CHANNEL		0x0

#define BOOT_FLASH_INDEX		0
#define MAIN_FLASH_INDEX		1

#define BOARD_ETH_START_PORT_NUM	0

/* Board specific configuration */
/* ============================ */

/* boards ID numbers */

#define BOARD_ID_BASE			0x20

/* New board ID numbers */
#define DB_88F6535_BP_ID		(BOARD_ID_BASE)
#define RD_88F6510_SFU_ID		(BOARD_ID_BASE+0x1)
#define RD_88F6560_GW_ID		(BOARD_ID_BASE+0x2)
#define RD_88F6530_MDU_ID		(BOARD_ID_BASE+0x3)
#define DB_CUSTOMER_ID			(BOARD_ID_BASE+0x4)
#define DB_ARRAN_ID			(BOARD_ID_BASE+0x5)
#define DB_BOWMORE_ID			(BOARD_ID_BASE+0x6)
#define DB_TAMDHU_ID			(BOARD_ID_BASE+0x7)
#define MV_MAX_BOARD_ID			(DB_TAMDHU_ID + 1)

/* old configuration (from schmatics)
#define DB_88F6535_MPP0_7		0x11111111
#define DB_88F6535_MPP8_15		0x31111111
#define DB_88F6535_MPP16_23		0x23555603
#define DB_88F6535_MPP24_31		0x04400000
#define DB_88F6535_MPP32_39		0x22044444
#define DB_88F6535_MPP40_47		0x22222222
#define DB_88F6535_MPP48_55		0x33322222
#define DB_88F6535_MPP56_63		0x33333333
#define DB_88F6535_MPP64_69		0x00000333
*/
/* DB-88F6535-BP */
#define DB_88F6535_MPP0_7		0x11111111
#define DB_88F6535_MPP8_15		0x31111111
#define DB_88F6535_MPP16_23		0x03555603
#define DB_88F6535_MPP24_31		0x04400000
#define DB_88F6535_MPP32_39		0x00444444
#define DB_88F6535_MPP40_47		0x00000000
#define DB_88F6535_MPP48_55		0x00000000
#define DB_88F6535_MPP56_63		0x00000000
#define DB_88F6535_MPP64_71		0x90000000
#define DB_88F6535_MPP72_79		0x00000000
#define DB_88F6535_MPP80_87		0x90000000
#define DB_88F6535_MPP88_88		0x00000000

/* GPPs
MPP#	NAME			IN/OUT
--------------------------------------
17		XVR_TXfault_RST	IN/OUT?
31		UsbDevice_Vbus	IN
68		SD_Status	IN
67		PEX_VCC_OFF	OUT
69		SD_WP		IN
*/
#define DB_88F6535_GPP_OUT_ENA_LOW	(BIT31 | BIT23 | BIT17)
#define DB_88F6535_GPP_OUT_ENA_MID	0x0
#define DB_88F6535_GPP_OUT_ENA_HIGH	(BIT5 | BIT23)

#define DB_88F6535_GPP_OUT_VAL_LOW	0x0
#define DB_88F6535_GPP_OUT_VAL_MID	0x0
#define DB_88F6535_GPP_OUT_VAL_HIGH	(BIT3)

#define DB_88F6535_GPP_POL_LOW		(BIT23)
#define DB_88F6535_GPP_POL_MID		0x0
#define DB_88F6535_GPP_POL_HIGH		0x0

/* RD-88F6510-SFU */
#define RD_88F6510_MPP0_7		0x11111111
#define RD_88F6510_MPP8_15		0x31111111
#define RD_88F6510_MPP16_23		0x20000003
#define RD_88F6510_MPP24_31		0x06622222
#define RD_88F6510_MPP32_39		0x00044444
#define RD_88F6510_MPP40_47		0x00006660
#define RD_88F6510_MPP48_55		0x00000460
#define RD_88F6510_MPP56_63		0x00000000
#define RD_88F6510_MPP64_69		0x00500000
#define RD_88F6510_MPP72_79		0x00000000
#define RD_88F6510_MPP80_87		0x00000000
#define RD_88F6510_MPP88_88		0x00000000

/* GPPs
MPP#	NAME			IN/OUT
--------------------------------------
17		WiFi_Host2WLAN	OUT
18		free		free
19		WiFi_WLAN2Host	IN
20		WiFi_PDn	OUT
21		WiFi_PWR	IN/OUT?
22		WiFi_RSTn	OUT
31		free		free
37		NF&SPI_WP	OUT
39		free (ground)	free
40		free (3.3V)	free
44		LED_SYS		OUT
45		LED_PON		OUT
46		LED_Phone	OUT
47		LED_PWR		OUT
48		free		free
51		free		free
52		free		free
53		free		free
54		free		free
55		free		free
56		free (ground)	free
57		free		free
58		free		free
59		free		free
60		free		free
61		free		free
62		free		free
63		free		free
64		free		free
65		free		free
66		PB		IN
67		free		free
68		TXfault_RST	IN/OUT?
*/
#define RD_88F6510_GPP_OUT_ENA_LOW	0x0
#define RD_88F6510_GPP_OUT_ENA_MID	0x0
#define RD_88F6510_GPP_OUT_ENA_HIGH	(BIT4)

#define RD_88F6510_GPP_OUT_VAL_LOW	0x0
#define RD_88F6510_GPP_OUT_VAL_MID	(BIT5)
#define RD_88F6510_GPP_OUT_VAL_HIGH	0x0

#define RD_88F6510_GPP_POL_LOW		0x0
#define RD_88F6510_GPP_POL_MID		0x0
#define RD_88F6510_GPP_POL_HIGH		0x0

/* RD-88F6560-GW */
#define RD_88F6560_MPP0_7		0x33333333
#define RD_88F6560_MPP8_15		0x31111111
#define RD_88F6560_MPP16_23		0x11111113
#define RD_88F6560_MPP24_31		0x04411001
#define RD_88F6560_MPP32_39		0x60004444
#define RD_88F6560_MPP40_47		0x04006060
#define RD_88F6560_MPP48_55		0x00000460
#define RD_88F6560_MPP56_63		0x00000000
#define RD_88F6560_MPP64_71		0x90500000
#define RD_88F6560_MPP72_79		0x00044444
#define RD_88F6560_MPP80_87		0x10000000
#define RD_88F6560_MPP88_88		0x00000000

/* GPPs
MPP#	NAME			IN/OUT
--------------------------------------
24		TDM_Reset	OUT
25		TDM_INTm0	IN
31		SD_WP		IN
37		HDD_PWR_CTRL	OUT
38		free		free
40		Fan_PWR_CTRL	OUT
44		Phy_INTn	IN
45		FXO_CTRL	OUT
47		WPS_Switch	IN
48		SD_Status	IN
51		TDM_INTn1	IN
52		Phone_Led	OUT
53		free		free
54		free		free
55		free		free
56		free		free
57		free		free
58		USB_Dev_Vbus	IN
59		USB_OC		IN
60		GP0		IN
61		GP1		IN
62		GP2		IN
63		free		free
64		free		free
65		free		free
66		free		free
68		SYS_LED		OUT
77		RST_PEX#	OUT
*/
#define RD_88F6560_GPP_OUT_ENA_LOW	BIT31
#define RD_88F6560_GPP_OUT_ENA_MID	(BIT5 | BIT12 | BIT15 | BIT16 | BIT19 | BIT26 | BIT27 | BIT28 | BIT29 | BIT30)
#define RD_88F6560_GPP_OUT_ENA_HIGH	0x0

#define RD_88F6560_GPP_OUT_VAL_LOW	0x0
#define RD_88F6560_GPP_OUT_VAL_MID	(BIT20)
#define RD_88F6560_GPP_OUT_VAL_HIGH	(BIT13)

#define RD_88F6560_GPP_POL_LOW		0x0
#define RD_88F6560_GPP_POL_MID		(BIT19)
#define RD_88F6560_GPP_POL_HIGH		0x0

/* RD-88F6530-MDU */
#define RD_88F6530_MPP0_7		0x11111111
#define RD_88F6530_MPP8_15		0x31111111
#define RD_88F6530_MPP16_23		0x00005003
#define RD_88F6530_MPP24_31		0x04422222
#define RD_88F6530_MPP32_39		0x00444444
#define RD_88F6530_MPP40_47		0x00044400
#define RD_88F6530_MPP48_55		0x00000000
#define RD_88F6530_MPP56_63		0x00000000
#define RD_88F6530_MPP64_69		0x00000000
#define RD_88F6530_MPP72_79		0x00000000
#define RD_88F6530_MPP80_87		0x00000000
#define RD_88F6530_MPP88_88		0x00000000

/* GPPs
MPP#	NAME			IN/OUT
-------------------------------
*/
/*the output GPIO is 17, 21, 23, 59, 60 */
/*61,62,63 as INPUT pin */

#define RD_88F6530_GPP_OUT_ENA_LOW	0xff5dffff
#define RD_88F6530_GPP_OUT_ENA_MID	0xffffffff
#define RD_88F6530_GPP_OUT_ENA_HIGH	0xffffffff

/*the output default value = 1 */
#define RD_88F6530_GPP_OUT_VAL_LOW	0x00A20000
#define RD_88F6530_GPP_OUT_VAL_MID	0x00000000
#define RD_88F6530_GPP_OUT_VAL_HIGH	(BIT4)

#define RD_88F6530_GPP_POL_LOW		0x0
#define RD_88F6530_GPP_POL_MID		0x0
#define RD_88F6530_GPP_POL_HIGH		0x0

/* DB-CUSTOMER */
#define DB_CUSTOMER_MPP0_7		0x21111111
#define DB_CUSTOMER_MPP8_15		0x00003311
#define DB_CUSTOMER_MPP16_23		0x00001100
#define DB_CUSTOMER_MPP24_31		0x00000000
#define DB_CUSTOMER_MPP32_39		0x00000000
#define DB_CUSTOMER_MPP40_47		0x00000000
#define DB_CUSTOMER_MPP48_55		0x00000000
#define DB_CUSTOMER_OE_LOW		0x0
#define DB_CUSTOMER_OE_HIGH		(~((BIT6) | (BIT7) | (BIT8) | (BIT9)))
#define DB_CUSTOMER_OE_VAL_LOW		0x0
#define DB_CUSTOMER_OE_VAL_HIGH		0x0

/* Arran 6560 */
#define ARRAN_MPP0_7		0x00000000
#define ARRAN_MPP8_15		0x31100000
#define ARRAN_MPP16_23		0x00003303
#define ARRAN_MPP24_31		0x00000000
#define ARRAN_MPP32_39		0x00004444
#define ARRAN_MPP40_47		0x00000000
#define ARRAN_MPP48_55		0x00000000
#define ARRAN_MPP56_63		0x00000000
#define ARRAN_MPP64_71		0x00330000
#define ARRAN_MPP72_79		0x00000000
#define ARRAN_MPP80_87		0x00000000
#define ARRAN_MPP88_88		0x00000000

#if 0
#define ARRAN_GPP_OUT_ENA_LOW		0x0
#define ARRAN_GPP_OUT_ENA_MID		0x0
#define ARRAN_GPP_OUT_ENA_HIGH		0x0

#define ARRAN_GPP_OUT_VAL_LOW		0x0
#define ARRAN_GPP_OUT_VAL_MID		0x0
#define ARRAN_GPP_OUT_VAL_HIGH		0x0

#define ARRAN_GPP_POL_LOW			0x0
#define ARRAN_GPP_POL_MID			0x0
#define ARRAN_GPP_POL_HIGH			0x0
#else
#define ARRAN_GPP_OUT_ENA_LOW	(~(BIT0))
#define ARRAN_GPP_OUT_ENA_MID	0xffffffff
#define ARRAN_GPP_OUT_ENA_HIGH	(~(BIT24 | BIT22 | BIT18 | BIT17 | BIT15 | BIT13 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6))

#define ARRAN_GPP_OUT_VAL_LOW	(BIT0)
#define ARRAN_GPP_OUT_VAL_MID	0x0
#define ARRAN_GPP_OUT_VAL_HIGH	(BIT24 | BIT22 | BIT15 | BIT13 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6)

#define ARRAN_GPP_POL_LOW		0x0
#define ARRAN_GPP_POL_MID		0x0
#define ARRAN_GPP_POL_HIGH		0x0
#endif

/* Bowmore */
#define BOWMORE_MPP0_7		0x00000000
#define BOWMORE_MPP8_15		0x31100000
#define BOWMORE_MPP16_23		0x00003303
#define BOWMORE_MPP24_31		0x00000000
#define BOWMORE_MPP32_39		0x00004444
#define BOWMORE_MPP40_47		0x00000000
#define BOWMORE_MPP48_55		0x00000000
#define BOWMORE_MPP56_63		0x00000000
#define BOWMORE_MPP64_71		0x00330000
#define BOWMORE_MPP72_79		0x00000000
#define BOWMORE_MPP80_87		0x00000000
#define BOWMORE_MPP88_88		0x00000000

#if 0
#define BOWMORE_GPP_OUT_ENA_LOW		0x0
#define BOWMORE_GPP_OUT_ENA_MID		0x0
#define BOWMORE_GPP_OUT_ENA_HIGH		0x0

#define BOWMORE_GPP_OUT_VAL_LOW		0x0
#define BOWMORE_GPP_OUT_VAL_MID		0x0
#define BOWMORE_GPP_OUT_VAL_HIGH		0x0

#define BOWMORE_GPP_POL_LOW			0x0
#define BOWMORE_GPP_POL_MID			0x0
#define BOWMORE_GPP_POL_HIGH			0x0
#else
#define BOWMORE_GPP_OUT_ENA_LOW	(~(BIT0))
#define BOWMORE_GPP_OUT_ENA_MID	0xffffffff
#define BOWMORE_GPP_OUT_ENA_HIGH	(~(BIT24 | BIT22 | BIT18 | BIT17 | BIT15 | BIT13 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6))

#define BOWMORE_GPP_OUT_VAL_LOW	(BIT0)
#define BOWMORE_GPP_OUT_VAL_MID	0x0
#define BOWMORE_GPP_OUT_VAL_HIGH	(BIT24 | BIT22 | BIT15 | BIT13 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6)

#define BOWMORE_GPP_POL_LOW		0x0
#define BOWMORE_GPP_POL_MID		0x0
#define BOWMORE_GPP_POL_HIGH		0x0
#endif

/* Tamdhu */
#define TAMDHU_MPP0_7		0x03333333
#define TAMDHU_MPP8_15		0x31100000
#define TAMDHU_MPP16_23		0x00003303
#define TAMDHU_MPP24_31		0x03300000
#define TAMDHU_MPP32_39		0x00004444
#define TAMDHU_MPP40_47		0x00000000
#define TAMDHU_MPP48_55		0x22250500
#define TAMDHU_MPP56_63		0x22222222
#define TAMDHU_MPP64_71		0x00330002
#define TAMDHU_MPP72_79		0x00000000
#define TAMDHU_MPP80_87		0x00000000
#define TAMDHU_MPP88_88		0x00000000

#if 0
#define TAMDHU_GPP_OUT_ENA_LOW		0x0
#define TAMDHU_GPP_OUT_ENA_MID		0x0
#define TAMDHU_GPP_OUT_ENA_HIGH		0x0

#define TAMDHU_GPP_OUT_VAL_LOW		0x0
#define TAMDHU_GPP_OUT_VAL_MID		0x0
#define TAMDHU_GPP_OUT_VAL_HIGH		0x0

#define TAMDHU_GPP_POL_LOW			0x0
#define TAMDHU_GPP_POL_MID			0x0
#define TAMDHU_GPP_POL_HIGH			0x0
#else
#define TAMDHU_GPP_OUT_ENA_LOW	(~(BIT0))
#define TAMDHU_GPP_OUT_ENA_MID	0xffffffff
#define TAMDHU_GPP_OUT_ENA_HIGH	(~(BIT24 | BIT22 | BIT18 | BIT17 | BIT15 | BIT13 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6))

#define TAMDHU_GPP_OUT_VAL_LOW	(BIT0)
#define TAMDHU_GPP_OUT_VAL_MID	0x0
#define TAMDHU_GPP_OUT_VAL_HIGH	(BIT24 | BIT22 | BIT15 | BIT13 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6)

#define TAMDHU_GPP_POL_LOW		0x0
#define TAMDHU_GPP_POL_MID		0x0
#define TAMDHU_GPP_POL_HIGH		0x0
#endif

#endif /* __INCmvBoardEnvSpech */
