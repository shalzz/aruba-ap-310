/*****************************************************************************
* Copyright 2011 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*****************************************************************************/

#ifndef __IPROC_COMMON_H__
#define __IPROC_COMMON_H__

#define iproc_get_chipid()	((*(volatile unsigned int *)ChipcommonA_ChipID) & 0xFFFF)
#define iproc_get_chiprev()	(((*(volatile unsigned int *)ChipcommonA_ChipID) >> 16) & 0xF)
#define iproc_get_chipsku()	(((*(volatile unsigned int *)ChipcommonA_ChipID) >> 20) & 0xF)

#define CHIP_ID_NS_10		53010
#define CHIP_ID_NS_11		53011
#define CHIP_ID_NS_12		53012
#define CHIP_ID_NS_13		53013
#define CHIP_ID_Vega_14		53014
#define CHIP_ID_Vega_15		53015
#define CHIP_ID_Vega_16		53016
#define CHIP_ID_CoVega		53017
#define CHIP_ID_CoNS_18		53018
#define CHIP_ID_CoNS_19		53019
#define CHIP_ID_Helix4		56340
#define CHIP_ID_Hr2			56150
#define CHIP_ID_NSP_22		53022
#define CHIP_ID_NSP_23		53023
#define CHIP_ID_NSP_25		53025


#define CHIP_REV_A0		(0)
#define CHIP_REV_A1		(1)
#define CHIP_REV_A2		(2)
#define CHIP_REV_A3		(3)
#define CHIP_REV_B0		(4)
#define CHIP_REV_B1		(5)
#define CHIP_REV_B2		(6)
#define CHIP_REV_B3		(7)
#define CHIP_REV_C0		(8)
#define CHIP_REV_C1		(9)
#define CHIP_REV_C2		(10)
#define CHIP_REV_C3		(11)
#define CHIP_REV_D0		(12)
#define CHIP_REV_D1		(13)
#define CHIP_REV_D2		(14)
#define CHIP_REV_D3		(15)

void bench_screen_test1(void);
uint32_t iproc_get_axi_clk(uint32_t refclk);
uint32_t iproc_read_ecc_syndrome(void);
void iproc_clear_ecc_syndrome(void);

#endif /* __IPROC_COMMON_H__ */

