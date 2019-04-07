/*
 *  Copyright (C) 2008 Marvell Semiconductors, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef _MVSDMMC_INCLUDE
#define _MVSDMMC_INCLUDE


#define MV88F6281_SDHC_BASE  	0xf1090000	// from 00-4c	

#define P(x) (MV88F6281_SDHC_BASE + (x))

// #define GPIO_REG_READ32(addr) (*(volatile unsigned int*)(INTEGRATOR_GPIO_BASE+addr))
// #define GPIO_REG_WRITE32(addr, val) (*(volatile unsigned int*)(INTEGRATOR_GPIO_BASE+addr)=val)

#define SDIO_REG_WRITE32(addr,val) (*(volatile unsigned long*)(P(addr)) = val)
#define SDIO_REG_WRITE16(addr,val) (*(volatile unsigned short*)(P(addr)) = val)
#define SDIO_REG_READ16(addr) (*(volatile unsigned short*)(P(addr)))
#define SDIO_REG_READ32(addr) (*(volatile unsigned long*)(P(addr)))

#define MVSDMMC_DMA_SIZE			65536

#define MVSDMMC_CMD_TIMEOUT			2 /* 100 usec*/


/*
 * The base MMC clock rate
 */

#define MVSDMMC_CLOCKRATE_MIN			100000
#define MVSDMMC_CLOCKRATE_MAX			50000000

#define MVSDMMC_BASE_FAST_CLOCK			200000000//100000000


/*
 * SDIO register
 */

#define SDIO_SYS_ADDR_LOW			0x000
#define SDIO_SYS_ADDR_HI			0x004
#define SDIO_BLK_SIZE				0x008
#define SDIO_BLK_COUNT				0x00c
#define SDIO_ARG_LOW				0x010
#define SDIO_ARG_HI				0x014
#define SDIO_XFER_MODE				0x018
#define SDIO_CMD				0x01c
#define SDIO_RSP(i)				(0x020 + ((i)<<2))
#define SDIO_RSP0				0x020
#define SDIO_RSP1				0x024
#define SDIO_RSP2				0x028
#define SDIO_RSP3				0x02c
#define SDIO_RSP4				0x030
#define SDIO_RSP5				0x034
#define SDIO_RSP6				0x038
#define SDIO_RSP7				0x03c
#define SDIO_BUF_DATA_PORT			0x040
#define SDIO_RSVED				0x044

#define SDIO_PRESENT_STATE0			0x048
#define SDIO_PRESENT_STATE1			0x04c
#define SDIO_HOST_CTRL				0x050
#define SDIO_BLK_GAP_CTRL			0x054
#define SDIO_CLK_CTRL				0x058
#define SDIO_SW_RESET				0x05c
#define SDIO_NOR_INTR_STATUS			0x060
#define SDIO_ERR_INTR_STATUS			0x064
#define SDIO_NOR_STATUS_EN			0x068
#define SDIO_ERR_STATUS_EN			0x06c
#define SDIO_NOR_INTR_EN			0x070
#define SDIO_ERR_INTR_EN			0x074
#define SDIO_AUTOCMD12_ERR_STATUS		0x078
#define SDIO_CURR_BYTE_LEFT			0x07c
#define SDIO_CURR_BLK_LEFT			0x080
#define SDIO_AUTOCMD12_ARG_LOW			0x084
#define SDIO_AUTOCMD12_ARG_HI			0x088
#define SDIO_AUTOCMD12_INDEX			0x08c
#define SDIO_AUTO_RSP(i)			(0x090 + ((i)<<2))
#define SDIO_AUTO_RSP0				0x090
#define SDIO_AUTO_RSP1				0x094
#define SDIO_AUTO_RSP2				0x098
#define SDIO_CLK_DIV				0x128

#define WINDOW_CTRL(i)				(0x108 + ((i) << 3))
#define WINDOW_BASE(i)				(0x10c + ((i) << 3))


/*
 * SDIO_PRESENT_STATE
 */

#define CARD_BUSY				(1 << 1)
#define CMD_INHIBIT				(1 << 0)
#define CMD_TXACTIVE				(1 << 8)
#define CMD_RXACTIVE				(1 << 9)
#define CMD_AUTOCMD12ACTIVE			(1 << 14)

#define CMD_BUS_BUSY				(CMD_AUTOCMD12ACTIVE|	\
						CMD_RXACTIVE|	\
						CMD_TXACTIVE|	\
						CMD_INHIBIT|	\
						CARD_BUSY)

/*
 * SDIO_CMD
 */

#define SDIO_CMD_RSP_NONE			(0 << 0)
#define SDIO_CMD_RSP_136			(1 << 0)
#define SDIO_CMD_RSP_48				(2 << 0)
#define SDIO_CMD_RSP_48BUSY			(3 << 0)

#define SDIO_CMD_CHECK_DATACRC16		(1 << 2)
#define SDIO_CMD_CHECK_CMDCRC			(1 << 3)
#define SDIO_CMD_INDX_CHECK			(1 << 4)
#define SDIO_CMD_DATA_PRESENT			(1 << 5)
#define SDIO_UNEXPECTED_RESP			(1 << 7)


/*
 * SDIO_XFER_MODE
 */

#define SDIO_XFER_MODE_STOP_CLK			(1 << 5)
#define SDIO_XFER_MODE_HW_WR_DATA_EN		(1 << 1)
#define SDIO_XFER_MODE_AUTO_CMD12		(1 << 2)
#define SDIO_XFER_MODE_INT_CHK_EN		(1 << 3)
#define SDIO_XFER_MODE_TO_HOST			(1 << 4)


/*
 * SDIO_HOST_CTRL
 */

#define SDIO_HOST_CTRL_PUSH_PULL_EN 		(1 << 0)

#define SDIO_HOST_CTRL_CARD_TYPE_MEM_ONLY 	(0 << 1)
#define SDIO_HOST_CTRL_CARD_TYPE_IO_ONLY 	(1 << 1)
#define SDIO_HOST_CTRL_CARD_TYPE_IO_MEM_COMBO 	(2 << 1)
#define SDIO_HOST_CTRL_CARD_TYPE_IO_MMC 	(3 << 1)
#define SDIO_HOST_CTRL_CARD_TYPE_MASK	 	(3 << 1)

#define SDIO_HOST_CTRL_BIG_ENDIAN 		(1 << 3)
#define SDIO_HOST_CTRL_LSB_FIRST 		(1 << 4)
#define SDIO_HOST_CTRL_ID_MODE_LOW_FREQ 	(1 << 5)
#define SDIO_HOST_CTRL_HALF_SPEED 		(1 << 6)
#define SDIO_HOST_CTRL_DATA_WIDTH_4_BITS 	(1 << 9)
#define SDIO_HOST_CTRL_HI_SPEED_EN 		(1 << 10)


#define SDIO_HOST_CTRL_TMOUT_MASK 		(0xf << 11)
#define SDIO_HOST_CTRL_TMOUT_MAX 		(0xf << 11)
#define SDIO_HOST_CTRL_TMOUT(x) 		((x) << 11)
#define SDIO_HOST_CTRL_TMOUT_EN 		(1 << 15)

#define SDIO_HOST_CTRL_DFAULT_OPEN_DRAIN 	\
		(SDIO_HOST_CTRL_TMOUT(x)(0xf))
#define SDIO_HOST_CTRL_DFAULT_PUSH_PULL 	\
		(SDIO_HOST_CTRL_TMOUT(x)(0xf) | SDIO_HOST_CTRL_PUSH_PULL_EN)


/*
 * NOR status bits
 */

#define SDIO_NOR_ERROR				(1 << 15)
#define SDIO_NOR_UNEXP_RSP			(1 << 14)
#define SDIO_NOR_AUTOCMD12_DONE			(1 << 13)
#define SDIO_NOR_SUSPEND_ON			(1 << 12)
#define SDIO_NOR_LMB_FF_8W_AVAIL		(1 << 11)
#define SDIO_NOR_LMB_FF_8W_FILLED		(1 << 10)
#define SDIO_NOR_READ_WAIT_ON			(1 << 9)
#define SDIO_NOR_CARD_INT			(1 << 8)
#define SDIO_NOR_READ_READY			(1 << 5)
#define SDIO_NOR_WRITE_READY			(1 << 4)
#define SDIO_NOR_DMA_INI			(1 << 3)
#define SDIO_NOR_BLK_GAP_EVT			(1 << 2)
#define SDIO_NOR_XFER_DONE			(1 << 1)
#define SDIO_NOR_CMD_DONE			(1 << 0)


/*
 * ERR status bits
 */

#define SDIO_ERR_CRC_STATUS			(1 << 14)
#define SDIO_ERR_CRC_STARTBIT			(1 << 13)
#define SDIO_ERR_CRC_ENDBIT			(1 << 12)
#define SDIO_ERR_RESP_TBIT			(1 << 11)
#define SDIO_ERR_SIZE				(1 << 10)
#define SDIO_ERR_CMD_STARTBIT			(1 << 9)
#define SDIO_ERR_AUTOCMD12			(1 << 8)
#define SDIO_ERR_DATA_ENDBIT			(1 << 6)
#define SDIO_ERR_DATA_CRC			(1 << 5)
#define SDIO_ERR_DATA_TIMEOUT			(1 << 4)
#define SDIO_ERR_CMD_INDEX			(1 << 3)
#define SDIO_ERR_CMD_ENDBIT			(1 << 2)
#define SDIO_ERR_CMD_CRC			(1 << 1)
#define SDIO_ERR_CMD_TIMEOUT			(1 << 0)

#define SDIO_POLL_MASK 				0xffff /* enable all for polling */


#define MMC_BLOCK_SIZE                  512
#define MMC_CMD_RESET                   0
#define MMC_CMD_SEND_OP_COND            1
#define MMC_CMD_ALL_SEND_CID            2
#define MMC_CMD_SET_RCA                 3
#define MMC_CMD_SELECT_CARD		7
#define MMC_CMD_SEND_CSD                9
#define MMC_CMD_SEND_CID                10
#define MMC_CMD_SEND_STATUS             13
#define MMC_CMD_SET_BLOCKLEN            16
#define MMC_CMD_READ_BLOCK              17
#define MMC_CMD_RD_BLK_MULTI            18
#define MMC_CMD_WRITE_BLOCK             24
#define MMC_MAX_BLOCK_SIZE              512

#if 0
typedef struct mmc_cid
{
	/* FIXME: BYTE_ORDER */
	uchar year:4,
	month:4;
	uchar sn[3];
	uchar fwrev:4,
	hwrev:4;
	uchar name[6];
	uchar id[3];
} mmc_cid_t;

typedef struct mmc_csd
{
	uchar   ecc:2,
		file_format:2,
		tmp_write_protect:1,
		perm_write_protect:1,
		copy:1,
		file_format_grp:1;
	uint64_t content_prot_app:1,
		rsvd3:4,
		write_bl_partial:1,
		write_bl_len:4,
		r2w_factor:3,
		default_ecc:2,
		wp_grp_enable:1,
		wp_grp_size:5,
		erase_grp_mult:5,
		erase_grp_size:5,
		c_size_mult1:3,
		vdd_w_curr_max:3,
		vdd_w_curr_min:3,
		vdd_r_curr_max:3,
		vdd_r_curr_min:3,
		c_size:12,
		rsvd2:2,
		dsr_imp:1,
		read_blk_misalign:1,
		write_blk_misalign:1,
		read_bl_partial:1;
	ushort  read_bl_len:4,
		ccc:12;
	uchar   tran_speed;
	uchar   nsac;
	uchar   taac;
	uchar   rsvd1:2,
		spec_vers:4,
		csd_structure:2;
} mmc_csd_t;                

typedef struct {
	char		pnm_0;	/* product name */
	char		oid_1;	/* OEM/application ID */
	char		oid_0;
	uint8_t		mid;	/* manufacturer ID */
	char		pnm_4;
	char		pnm_3;
	char		pnm_2;
	char		pnm_1;
	uint8_t		psn_2;	/* product serial number */
	uint8_t		psn_1;
	uint8_t		psn_0;	/* MSB */
	uint8_t		prv;	/* product revision */
	uint8_t		crc;	/* CRC7 checksum, b0 is unused and set to 1 */
	uint8_t		mdt_1;	/* manufacturing date, LSB, RRRRyyyy yyyymmmm */
	uint8_t		mdt_0;	/* MSB */
	uint8_t		psn_3;	/* LSB */
} sd_cid_t;
#endif
#endif /* _MVSDMMC_INCLUDE */
