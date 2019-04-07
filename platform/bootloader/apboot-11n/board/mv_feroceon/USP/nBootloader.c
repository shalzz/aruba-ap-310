/*
 * (C) Copyright 2006
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
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
#include <nand.h>
#include "cntmr/mvCntmr.h"
#include "uart/mvUart.h"
#include "nBootstrap.h"
#include <asm/io.h>

#define CFG_NAND_READ_DELAY \
	{ volatile int dummy; int i; for (i=0; i<10000; i++) dummy = i; }

typedef void 		(*VOIDFUNCPTR) (void); /* ptr to function returning void */

extern int board_nand_init(struct nand_chip *nand);
extern void mv_nand_hwcontrol(struct mtd_info *mtdinfo, int cmd);
static void mv_nand_write_byte(struct mtd_info *mtd, u_char byte);
static u_char mv_nand_read_byte(struct mtd_info *mtd);


static int nand_is_bad_block(struct mtd_info *mtd, int block)
{
	struct nand_chip *this = mtd->priv;
	int page_addr = block * CFG_NAND_PAGE_COUNT;

	/* Begin command latch cycle */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_CLE | NAND_CTRL_CHANGE);
//->hwcontrol(mtd, NAND_CTL_SETCLE);
#ifdef MV_LARGE_PAGE
	this->write_byte(mtd, NAND_CMD_READ0);
#else
	this->write_byte(mtd, NAND_CMD_READOOB);
#endif
	/* Set ALE and clear CLE to start address cycle */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_ALE | NAND_CTRL_CHANGE);
//->hwcontrol(mtd, NAND_CTL_CLRCLE);
	//this->hwcontrol(mtd, NAND_CTL_SETALE);
#ifdef MV_LARGE_PAGE
	/* Column address */
	this->write_byte(mtd, CFG_NAND_BAD_BLOCK_POS);			/* A[7:0] */
	this->write_byte(mtd, 8);					/* A[11:8] */
	/* Row address */
	this->write_byte(mtd, (uchar)(page_addr & 0xff));		/* A[19:12] */
	this->write_byte(mtd, (uchar)((page_addr >> 8) & 0xff));	/* A[27:20] */
	this->write_byte(mtd, (uchar)((page_addr >> 16) & 0x03));	/* A[29:28] */
#else
	/* Column address */
	this->write_byte(mtd, CFG_NAND_BAD_BLOCK_POS);			/* A[7:0] */
	/* Row address */
	this->write_byte(mtd, (uchar)(page_addr & 0xff));		/* A[16:9] */
	this->write_byte(mtd, (uchar)((page_addr >> 8) & 0xff));	/* A[24:17] */
#endif
	/* Latch in address */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_CTRL_CHANGE);
//->hwcontrol(mtd, NAND_CTL_CLRALE);
#ifdef MV_LARGE_PAGE
	/* Set command latch cycle */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_CLE | NAND_CTRL_CHANGE);
//->hwcontrol(mtd, NAND_CTL_SETCLE);
	this->write_byte(mtd, NAND_CMD_READSTART);
	/* Clear CLE to latch command cycle */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_CTRL_CHANGE);
//->hwcontrol(mtd, NAND_CTL_CLRCLE);
#endif

	/*
	 * Wait a while for the data to be ready
	 */
	if (this->dev_ready)
		this->dev_ready(mtd);
	else
		CFG_NAND_READ_DELAY;

	/*
	 * Read on byte
	 */
	if (this->read_byte(mtd) != 0xff)
		return 1;

	return 0;
}

static int nand_read_page(struct mtd_info *mtd, int block, int page, uchar *dst)
{
	struct nand_chip *this = mtd->priv;
	int page_addr = page + block * CFG_NAND_PAGE_COUNT;
	int i;

	/* Begin command latch cycle */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_CLE | NAND_CTRL_CHANGE);
//->hwcontrol(mtd, NAND_CTL_SETCLE);
	this->write_byte(mtd, NAND_CMD_READ0);
	/* Set ALE and clear CLE to start address cycle */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_ALE | NAND_CTRL_CHANGE);
//->hwcontrol(mtd, NAND_CTL_CLRCLE);
//	this->hwcontrol(mtd, NAND_CTL_SETALE);
#ifdef MV_LARGE_PAGE
	/* Column address */
	this->write_byte(mtd, 0);					/* A[7:0] */
	this->write_byte(mtd, 0);					/* A[11:8] */
	/* Row address */
	this->write_byte(mtd, (uchar)(page_addr & 0xff));		/* A[19:12] */
	this->write_byte(mtd, (uchar)((page_addr >> 8) & 0xff));	/* A[27:20] */
	this->write_byte(mtd, (uchar)((page_addr >> 16) & 0x03));	/* A[29:28] */
#else
	/* Column address */
	this->write_byte(mtd, 0);					/* A[7:0] */
	/* Row address */
	this->write_byte(mtd, (uchar)(page_addr & 0xff));		/* A[16:9] */
	this->write_byte(mtd, (uchar)((page_addr >> 8) & 0xff));	/* A[24:17] */
#endif
	/* Latch in address */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_CTRL_CHANGE);
//->hwcontrol(mtd, NAND_CTL_CLRALE);
#ifdef MV_LARGE_PAGE
	/* End command latch cycle */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_CLE | NAND_CTRL_CHANGE);
//->hwcontrol(mtd, NAND_CTL_SETCLE);
	this->write_byte(mtd, NAND_CMD_READSTART);
	/* Set clear CLE to start address cycle */
	this->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_CTRL_CHANGE);
//->hwcontrol(mtd, NAND_CTL_CLRCLE);
#endif

	/*
	 * Wait a while for the data to be ready
	 */
	if (this->dev_ready)
		this->dev_ready(mtd);
	else
		CFG_NAND_READ_DELAY;

	/*
	 * Read page into buffer
	 */
	for (i=0; i<CFG_NAND_PAGE_SIZE; i++)
		*dst++ = this->read_byte(mtd);

	return 0;
}

static int nand_load(struct mtd_info *mtd, int offs, int uboot_size, uchar *dst)
{
	int block;
	int blockcopy_count;
	int page;

	/*
	 * offs has to be aligned to a block address!
	 */
	block = offs / CFG_NAND_BLOCK_SIZE;
	blockcopy_count = 0;

	while (blockcopy_count < (uboot_size / CFG_NAND_BLOCK_SIZE)) {
		if (!nand_is_bad_block(mtd, block)) {
			/*
			 * Skip bad blocks
			 */
			for (page = 0; page < CFG_NAND_PAGE_COUNT; page++) {
				nand_read_page(mtd, block, page, dst);
				dst += CFG_NAND_PAGE_SIZE;
			}

			blockcopy_count++;
		}

		block++;
		/* End of NAND device */
		if (block >= NUM_BLOCKS)
			return -1;
	}

	return 0;
}

void nand_boot(void)
{

	ulong mem_size;
	struct nand_chip nand_chip;
	nand_info_t nand_info;
	int ret;

	/*
	 * DRAM was init in nBootstrap.S so we have access to memory
	 */
	mem_size = 0x08000000;

	/*
	 * Init board specific nand support
	 */
	nand_info.priv = &nand_chip;
	nand_chip.IO_ADDR_R = nand_chip.IO_ADDR_W = (void  __iomem *)CONFIG_SYS_NAND_BASE;
	nand_chip.dev_ready = NULL;	/* preset to NULL */
	board_nand_init(&nand_chip);

	/* Add write and read byte functions */
	nand_chip.write_byte = mv_nand_write_byte;
	nand_chip.read_byte = mv_nand_read_byte;

	/*
	 * Load U-Boot image from NAND into RAM
	 */
	ret = nand_load(&nand_info, CFG_NAND_U_BOOT_OFFS, CFG_NAND_U_BOOT_SIZE,
			(uchar *)CFG_NAND_U_BOOT_DST);
	if (ret != 0)
		/* Error reading NAND */
		while(1);

	/*
	 * Jump to U-Boot image
	 */
#if defined(MV_LARGE_PAGE)
    VOIDFUNCPTR ubootStart = (VOIDFUNCPTR)(CFG_NAND_U_BOOT_START + 0x90000);
#else
    VOIDFUNCPTR ubootStart = (VOIDFUNCPTR)(CFG_NAND_U_BOOT_START + 0x90000);
#endif
	(*ubootStart)();
}

/**
 * mv_nand_read_byte - [DEFAULT] read one byte to the chip
 * @mtd:	MTD device structure
 * @byte:	pointer to data byte to write
 *
 * Default read function for 8it buswith
 */
static u_char mv_nand_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *this = mtd->priv;
	return readb(this->IO_ADDR_R);
}

/**
 * nand_write_byte - [DEFAULT] write one byte to the chip
 * @mtd:	MTD device structure
 * @byte:	pointer to data byte to write
 *
 * Default write function for 8it buswith
 */
static void mv_nand_write_byte(struct mtd_info *mtd, u_char byte)
{
	struct nand_chip *this = mtd->priv;
	writeb(byte, this->IO_ADDR_W);
}
