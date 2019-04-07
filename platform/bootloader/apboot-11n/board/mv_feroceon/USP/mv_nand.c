/*
 * (C) Copyright 2005 2N TELEKOMUNIKACE, Ladislav Michl
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#if defined(CONFIG_CMD_NAND)
#include <nand.h>
#include <mvTypes.h>
#include "mvBoardEnvLib.h"
#ifdef MV78200
extern MV_BOOL mvSocUnitIsMappedToThisCpu(MV_SOC_UNIT unit);
#endif

#define writeb(d,addr)	*(volatile u_char *)(addr) = (d)
/*
 *	hardware specific access to control-lines
 */
#define	MASK_CLE	0x01
#define	MASK_ALE	0x02

static void mv_nand_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *this = mtd->priv;
	u_int32_t	IO_ADDR_W = (u_int32_t)this->IO_ADDR_W;
	
	IO_ADDR_W &= ~(MASK_ALE|MASK_CLE);

	if (ctrl & NAND_CTRL_CHANGE) {
		if ( ctrl & NAND_CLE )
			IO_ADDR_W |= MASK_CLE;
		if ( ctrl & NAND_ALE )
			IO_ADDR_W |= MASK_ALE;
		this->IO_ADDR_W = (void __iomem *) IO_ADDR_W;
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, this->IO_ADDR_W);
}


int board_nand_init(struct nand_chip *nand)
{
#if defined(CONFIG_NAND_RS_ECC_SUPPORT)
	int bootDev = (MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & (0xF << 11)) >> 11;
#endif

#ifdef MV78200
    /* Check in dual CPU system which CPU use nand */
 /*   if (mvSocUnitIsMappedToThisCpu(NAND_FLASH))
    {*/
#endif
#if defined(MV_LARGE_PAGE)
	nand->options = NAND_SAMSUNG_LP_OPTIONS;
#endif
#if defined(CONFIG_NAND_RS_ECC_SUPPORT)
	if ((bootDev == 0x6) || (bootDev == 0xF)) {
		printf("4bit RS ECC, Size: ");
		nand->ecc.mode = NAND_ECC_RS_SOFT;
	} else {
		printf("1bit HM ECC, Size: ");
		nand->ecc.mode = NAND_ECC_SOFT;
	}
#else
	nand->ecc.mode = NAND_ECC_SOFT;
#endif
	nand->cmd_ctrl  = mv_nand_hwcontrol;
	nand->chip_delay = 30;
	return 0;
#ifdef MV78200
/*    }
    else
    	return 1;*/
#endif
}
#endif
