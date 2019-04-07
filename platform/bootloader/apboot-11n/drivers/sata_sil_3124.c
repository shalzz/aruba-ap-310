/*
 * sata_sil24.c - Driver for Silicon Image 3124/3132 SATA-2 controllers
 *
 * Copyright 2005  Tejun Heo
 *
 * Based on preview driver from Silicon Image.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */


#include <common.h>

#ifdef CONFIG_PCI

#ifdef CFG_SIL_SATA_3124

#include <command.h>
#include <asm/processor.h>
#include <pci.h>
#include <ide.h>
#include <ata.h>
#include <ata_scsi.h>
#include <malloc.h>

block_dev_desc_t * ide_get_dev(int dev);

#define DRV_NAME	"sata_sil24"
#define DRV_VERSION	"0.23"

#define ASUS_SATA_DEBUG		0
#define DMA_BASE_ADDR  0
#define SATA_RESET_TIMEOUT	10000	//10 sec

#define msleep(ms_data)	udelay(ms_data*1000)
#define mdelay(ms_data)	udelay(ms_data*1000)
#define dma_addr_t unsigned int
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define ata_id_has_lba(id)	((id)[49] & (1 << 9))
#define ata_id_has_dma(id)	((id)[49] & (1 << 8))
#define ata_id_has_lba48(id)	((id)[83] & (1 << 10))
#define ata_id_u32(id,n)	\
	(((u32) (id)[(n) + 1] << 16) | ((u32) (id)[(n)]))
#define ata_id_u64(id,n)	\
	( ((u64) (id)[(n) + 3] << 48) |	\
	  ((u64) (id)[(n) + 2] << 32) |	\
	  ((u64) (id)[(n) + 1] << 16) |	\
	  ((u64) (id)[(n) + 0]) )



static  unsigned int sata_cmd_addr[4];
static  unsigned int sata_scr_addr[4];
static  unsigned int xfer_modes[4];
static  unsigned int lba64_mode[4];
static struct ata_taskfile sata_tf;


static  struct sil24_cmd_block *sata_cmd_block;	// use malloc to allocate memory, or it need long delay to ready for some motherboard, why?
unsigned char *sata_private_data;		// use malloc to allocate memory, or it need long delay to ready for some motherboard, why?
static int sge_offset;


#if CONFIG_OCTEON
#include "octeon_pci.h"
#define writel(val, addr)     octeon_pci_mem1_writel(val, addr)
#define readl(addr)           octeon_pci_mem1_readl(addr)
#define writew(val, addr)     octeon_pci_mem1_writew(val, addr)
#define readw(addr)           octeon_pci_mem1_readw(addr)
#endif

struct sil24_prb {
	u16	ctrl;
	u16	prot;
	u32	rx_cnt;
	u8	fis[6 * 4];
};

/*
 * Scatter gather entry (SGE) 16 bytes
 */
struct sil24_sge {
	u64	addr;
	u32	cnt;
	u32	flags;
};

/*
 * Port multiplier
 */
struct sil24_port_multiplier {
	u32	diag;
	u32	sactive;
};

enum {
	/*
	 * Global controller registers (128 bytes @ BAR0)
	 */
		/* 32 bit regs */
	HOST_SLOT_STAT		= 0x00, /* 32 bit slot stat * 4 */
	HOST_CTRL		= 0x40,
	HOST_IRQ_STAT		= 0x44,
	HOST_PHY_CFG		= 0x48,
	HOST_BIST_CTRL		= 0x50,
	HOST_BIST_PTRN		= 0x54,
	HOST_BIST_STAT		= 0x58,
	HOST_MEM_BIST_STAT	= 0x5c,
	HOST_FLASH_CMD		= 0x70,
		/* 8 bit regs */
	HOST_FLASH_DATA		= 0x74,
	HOST_TRANSITION_DETECT	= 0x75,
	HOST_GPIO_CTRL		= 0x76,
	HOST_I2C_ADDR		= 0x78, /* 32 bit */
	HOST_I2C_DATA		= 0x7c,
	HOST_I2C_XFER_CNT	= 0x7e,
	HOST_I2C_CTRL		= 0x7f,

	/* HOST_SLOT_STAT bits */
	HOST_SSTAT_ATTN		= (1 << 31),

	/*
	 * Port registers
	 * (8192 bytes @ +0x0000, +0x2000, +0x4000 and +0x6000 @ BAR2)
	 */
	PORT_REGS_SIZE		= 0x2000,
	PORT_PRB		= 0x0000, /* (32 bytes PRB + 16 bytes SGEs * 6) * 31 (3968 bytes) */

	PORT_PM			= 0x0f80, /* 8 bytes PM * 16 (128 bytes) */
		/* 32 bit regs */
	PORT_CTRL_STAT		= 0x1000, /* write: ctrl-set, read: stat */
	PORT_CTRL_CLR		= 0x1004, /* write: ctrl-clear */
	PORT_IRQ_STAT		= 0x1008, /* high: status, low: interrupt */
	PORT_IRQ_ENABLE_SET	= 0x1010, /* write: enable-set */
	PORT_IRQ_ENABLE_CLR	= 0x1014, /* write: enable-clear */
	PORT_ACTIVATE_UPPER_ADDR= 0x101c,
	PORT_EXEC_FIFO		= 0x1020, /* command execution fifo */
	PORT_CMD_ERR		= 0x1024, /* command error number */
	PORT_FIS_CFG		= 0x1028,
	PORT_FIFO_THRES		= 0x102c,
		/* 16 bit regs */
	PORT_DECODE_ERR_CNT	= 0x1040,
	PORT_DECODE_ERR_THRESH	= 0x1042,
	PORT_CRC_ERR_CNT	= 0x1044,
	PORT_CRC_ERR_THRESH	= 0x1046,
	PORT_HSHK_ERR_CNT	= 0x1048,
	PORT_HSHK_ERR_THRESH	= 0x104a,
		/* 32 bit regs */
	PORT_PHY_CFG		= 0x1050,
	PORT_SLOT_STAT		= 0x1800,
	PORT_CMD_ACTIVATE	= 0x1c00, /* 64 bit cmd activate * 31 (248 bytes) */
	PORT_EXEC_DIAG		= 0x1e00, /* 32bit exec diag * 16 (64 bytes, 0-10 used on 3124) */
	PORT_PSD_DIAG		= 0x1e40, /* 32bit psd diag * 16 (64 bytes, 0-8 used on 3124) */
	PORT_SCONTROL		= 0x1f00,
	PORT_SSTATUS		= 0x1f04,
	PORT_SERROR		= 0x1f08,
	PORT_SACTIVE		= 0x1f0c,

	/* PORT_CTRL_STAT bits */
	PORT_CS_PORT_RST	= (1 << 0), /* port reset */
	PORT_CS_DEV_RST		= (1 << 1), /* device reset */
	PORT_CS_INIT		= (1 << 2), /* port initialize */
	PORT_CS_IRQ_WOC		= (1 << 3), /* interrupt write one to clear */
	PORT_CS_CDB16		= (1 << 5), /* 0=12b cdb, 1=16b cdb */
	PORT_CS_RESUME		= (1 << 6), /* port resume */
	PORT_CS_32BIT_ACTV	= (1 << 10), /* 32-bit activation */
	PORT_CS_PM_EN		= (1 << 13), /* port multiplier enable */
	PORT_CS_RDY		= (1 << 31), /* port ready to accept commands */

	/* PORT_IRQ_STAT/ENABLE_SET/CLR */
	/* bits[11:0] are masked */
	PORT_IRQ_COMPLETE	= (1 << 0), /* command(s) completed */
	PORT_IRQ_ERROR		= (1 << 1), /* command execution error */
	PORT_IRQ_PORTRDY_CHG	= (1 << 2), /* port ready change */
	PORT_IRQ_PWR_CHG	= (1 << 3), /* power management change */
	PORT_IRQ_PHYRDY_CHG	= (1 << 4), /* PHY ready change */
	PORT_IRQ_COMWAKE	= (1 << 5), /* COMWAKE received */
	PORT_IRQ_UNK_FIS	= (1 << 6), /* Unknown FIS received */
	PORT_IRQ_SDB_FIS	= (1 << 11), /* SDB FIS received */

	/* bits[27:16] are unmasked (raw) */
	PORT_IRQ_RAW_SHIFT	= 16,
	PORT_IRQ_MASKED_MASK	= 0x7ff,
	PORT_IRQ_RAW_MASK	= (0x7ff << PORT_IRQ_RAW_SHIFT),

	/* ENABLE_SET/CLR specific, intr steering - 2 bit field */
	PORT_IRQ_STEER_SHIFT	= 30,
	PORT_IRQ_STEER_MASK	= (3 << PORT_IRQ_STEER_SHIFT),

	/* PORT_CMD_ERR constants */
	PORT_CERR_DEV		= 1, /* Error bit in D2H Register FIS */
	PORT_CERR_SDB		= 2, /* Error bit in SDB FIS */
	PORT_CERR_DATA		= 3, /* Error in data FIS not detected by dev */
	PORT_CERR_SEND		= 4, /* Initial cmd FIS transmission failure */
	PORT_CERR_INCONSISTENT	= 5, /* Protocol mismatch */
	PORT_CERR_DIRECTION	= 6, /* Data direction mismatch */
	PORT_CERR_UNDERRUN	= 7, /* Ran out of SGEs while writing */
	PORT_CERR_OVERRUN	= 8, /* Ran out of SGEs while reading */
	PORT_CERR_PKT_PROT	= 11, /* DIR invalid in 1st PIO setup of ATAPI */
	PORT_CERR_SGT_BOUNDARY	= 16, /* PLD ecode 00 - SGT not on qword boundary */
	PORT_CERR_SGT_TGTABRT	= 17, /* PLD ecode 01 - target abort */
	PORT_CERR_SGT_MSTABRT	= 18, /* PLD ecode 10 - master abort */
	PORT_CERR_SGT_PCIPERR	= 19, /* PLD ecode 11 - PCI parity err while fetching SGT */
	PORT_CERR_CMD_BOUNDARY	= 24, /* ctrl[15:13] 001 - PRB not on qword boundary */
	PORT_CERR_CMD_TGTABRT	= 25, /* ctrl[15:13] 010 - target abort */
	PORT_CERR_CMD_MSTABRT	= 26, /* ctrl[15:13] 100 - master abort */
	PORT_CERR_CMD_PCIPERR	= 27, /* ctrl[15:13] 110 - PCI parity err while fetching PRB */
	PORT_CERR_XFR_UNDEF	= 32, /* PSD ecode 00 - undefined */
	PORT_CERR_XFR_TGTABRT	= 33, /* PSD ecode 01 - target abort */
	PORT_CERR_XFR_MSGABRT	= 34, /* PSD ecode 10 - master abort */
	PORT_CERR_XFR_PCIPERR	= 35, /* PSD ecode 11 - PCI prity err during transfer */
	PORT_CERR_SENDSERVICE	= 36, /* FIS received while sending service */

	/* bits of PRB control field */
	PRB_CTRL_PROTOCOL	= (1 << 0), /* override def. ATA protocol */
	PRB_CTRL_PACKET_READ	= (1 << 4), /* PACKET cmd read */
	PRB_CTRL_PACKET_WRITE	= (1 << 5), /* PACKET cmd write */
	PRB_CTRL_NIEN		= (1 << 6), /* Mask completion irq */
	PRB_CTRL_SRST		= (1 << 7), /* Soft reset request (ign BSY?) */

	/* PRB protocol field */
	PRB_PROT_PACKET		= (1 << 0),
	PRB_PROT_TCQ		= (1 << 1),
	PRB_PROT_NCQ		= (1 << 2),
	PRB_PROT_READ		= (1 << 3),
	PRB_PROT_WRITE		= (1 << 4),
	PRB_PROT_TRANSPARENT	= (1 << 5),

	/*
	 * Other constants
	 */
	SGE_TRM			= (1 << 31), /* Last SGE in chain */
	SGE_LNK			= (1 << 30), /* linked list
						Points to SGT, not SGE */
	SGE_DRD			= (1 << 29), /* discard data read (/dev/null)
						data address ignored */

	/* board id */
	BID_SIL3124		= 0,
	BID_SIL3132		= 1,
	BID_SIL3131		= 2,

	IRQ_STAT_4PORTS		= 0xf,
};

struct sil24_ata_block {
	struct sil24_prb prb;
	struct sil24_sge sge[LIBATA_MAX_PRD];
};

struct sil24_atapi_block {
	struct sil24_prb prb;
	u8 cdb[16];
	struct sil24_sge sge[LIBATA_MAX_PRD - 1];
};

struct sil24_cmd_block {
	struct sil24_ata_block ata;
};


/* ap->host_set->private_data */
struct sil24_host_priv {
	unsigned int host_base;	/* global controller control (128 bytes @BAR0) */
	unsigned int port_base;	/* port registers (4 * 8192 bytes @BAR2) */
};


/*
 * Use bits 30-31 of host_flags to encode available port numbers.
 * Current maxium is 4.
 */
#define SIL24_NPORTS2FLAG(nports)	((((unsigned)(nports) - 1) & 0x3) << 30)
#define SIL24_FLAG2NPORTS(flag)		((((flag) >> 30) & 0x3) + 1)


/**
 *	swap_buf_le16 -
 *	@buf:  Buffer to swap
 *	@buf_words:  Number of 16-bit words in buffer.
 *
 *	Swap halves of 16-bit words if needed to convert from
 *	little-endian byte order to native cpu byte order, or
 *	vice-versa.
 *
 *	LOCKING:
 */
void swap_buf_le16(u16 *buf, unsigned int buf_words)
{
//#ifdef __BIG_ENDIAN
	unsigned int i;

	for (i = 0; i < buf_words; i++)
		buf[i] = le16_to_cpu(buf[i]);
//#endif /* __BIG_ENDIAN */
}


static const char * xfer_mode_str[] = {
	"UDMA/16",
	"UDMA/25",
	"UDMA/33",
	"UDMA/44",
	"UDMA/66",
	"UDMA/100",
	"UDMA/133",
	"UDMA7",
	"MWDMA0",
	"MWDMA1",
	"MWDMA2",
	"PIO0",
	"PIO1",
	"PIO2",
	"PIO3",
	"PIO4",
};

/**
 *	ata_udma_string - convert UDMA bit offset to string
 *	@mask: mask of bits supported; only highest bit counts.
 *
 *	Determine string which represents the highest speed
 *	(highest bit in @udma_mask).
 *
 *	LOCKING:
 *	None.
 *
 *	RETURNS:
 *	Constant C string representing highest speed listed in
 *	@udma_mask, or the constant C string "<n/a>".
 */
static const char *ata_mode_string(unsigned int mask)
{
	int i;

	for (i = 7; i >= 0; i--)
		if (mask & (1 << i))
			goto out;
	for (i = ATA_SHIFT_MWDMA + 2; i >= ATA_SHIFT_MWDMA; i--)
		if (mask & (1 << i))
			goto out;
	for (i = ATA_SHIFT_PIO + 4; i >= ATA_SHIFT_PIO; i--)
		if (mask & (1 << i))
			goto out;

	return "<n/a>";

out:
	return xfer_mode_str[i];
}

/**
 *	ata_tf_to_fis - Convert ATA taskfile to SATA FIS structure
 *	@tf: Taskfile to convert
 *	@fis: Buffer into which data will output
 *	@pmp: Port multiplier port
 *
 *	Converts a standard ATA taskfile to a Serial ATA
 *	FIS structure (Register - Host to Device).
 *
 *	LOCKING:
 *	Inherited from caller.
 */

void ata_tf_to_fis(struct ata_taskfile *tf, u8 *fis, u8 pmp)
{
	fis[0] = 0x27;	/* Register - Host to Device FIS */
	fis[1] = (pmp & 0xf) | (1 << 7); /* Port multiplier number,
					    bit 7 indicates Command FIS */
	fis[2] = tf->command;
	fis[3] = tf->feature;

	fis[4] = tf->lbal;
	fis[5] = tf->lbam;
	fis[6] = tf->lbah;
	fis[7] = tf->device;

	fis[8] = tf->hob_lbal;
	fis[9] = tf->hob_lbam;
	fis[10] = tf->hob_lbah;
	fis[11] = tf->hob_feature;

	fis[12] = tf->nsect;
	fis[13] = tf->hob_nsect;
	fis[14] = 0;
	fis[15] = tf->ctl;

	fis[16] = 0;
	fis[17] = 0;
	fis[18] = 0;
	fis[19] = 0;

        if ( tf->command == ATA_CMD_ID_ATA )
	   fis[12] = 0; 	// nsec = 0;
}

/**
 *	ata_tf_from_fis - Convert SATA FIS to ATA taskfile
 *	@fis: Buffer from which data will be input
 *	@tf: Taskfile to output
 *
 *	Converts a standard ATA taskfile to a Serial ATA
 *	FIS structure (Register - Host to Device).
 *
 *	LOCKING:
 *	Inherited from caller.
 */

void ata_tf_from_fis(u8 *fis, struct ata_taskfile *tf)
{
	tf->command	= fis[2];	/* status */
	tf->feature	= fis[3];	/* error */

	tf->lbal	= fis[4];
	tf->lbam	= fis[5];
	tf->lbah	= fis[6];
	tf->device	= fis[7];

	tf->hob_lbal	= fis[8];
	tf->hob_lbam	= fis[9];
	tf->hob_lbah	= fis[10];

	tf->nsect	= fis[12];
	tf->hob_nsect	= fis[13];
}



static int sil24_scr_map[] = {
	[SCR_CONTROL]	= 0,
	[SCR_STATUS]	= 1,
	[SCR_ERROR]	= 2,
	[SCR_ACTIVE]	= 3,
};


static u32 sil24_scr_read(int dev_no, unsigned sc_reg)
{
	unsigned int scr_addr = sata_scr_addr[dev_no];
	if (sc_reg < ARRAY_SIZE(sil24_scr_map)) {
		unsigned int addr;
		addr = scr_addr + sil24_scr_map[sc_reg] * 4;
		return readl(scr_addr + sil24_scr_map[sc_reg] * 4);
	}
	return 0xffffffffU;
}

static void sil24_scr_write(int dev_no, unsigned sc_reg, u32 val)
{
	unsigned int scr_addr = sata_scr_addr[dev_no];
	if (sc_reg < ARRAY_SIZE(sil24_scr_map)) {
		unsigned int addr;
		addr = scr_addr + sil24_scr_map[sc_reg] * 4;
		writel(val, scr_addr + sil24_scr_map[sc_reg] * 4);
	}
}

#if 0
static void sil24_tf_read(int dev_no, struct ata_taskfile *tf)
{
	tf = &sata_tf;
}
#endif

static void scr_write_flush(int dev_no, unsigned int reg, u32 val)
{
	sil24_scr_write(dev_no, reg, val);
	sil24_scr_read(dev_no, reg);
}

static unsigned int sata_dev_present(int dev_no)
{
	return ((sil24_scr_read(dev_no, SCR_STATUS) & 0xf) == 0x3) ? 1 : 0;
}


static int sil24_issue_SRST(int dev_no)
{
	u32 irq_enable, irq_stat;
	int cnt;

	unsigned int port = sata_cmd_addr[dev_no];
	struct sil24_prb *prb = &sata_cmd_block->ata.prb;
	dma_addr_t paddr=(dma_addr_t)&sata_cmd_block->ata.prb;
	paddr = paddr - CFG_SDRAM_BASE;

	/* temporarily turn off IRQs during SRST */
	irq_enable = readl(port + PORT_IRQ_ENABLE_SET);
	writel(irq_enable, port + PORT_IRQ_ENABLE_CLR);

	/*
	 * XXX: Not sure whether the following sleep is needed or not.
	 * The original driver had it.  So....
	 */
	msleep(10);	// *** East add delay, or the reset command will fail

	//prb->ctrl = PRB_CTRL_SRST;
	prb->ctrl = cpu_to_le16(PRB_CTRL_SRST);
	prb->fis[1] = 0; /* no PM yet */	

	msleep(10);	// *** East add delay, or the reset command will fail

#if ASUS_SATA_DEBUG	// Dump PRB control field
	u8 *ptr;
	int i;
	u8 tmp;

	ptr=(u8 *)prb;

	for(i=0; i<16; i++)
	   printf("%02x ", *(ptr+i));
	printf("\n");

	printf("low 32bit %lx port %lx\n", paddr, (u64)port);
#endif
	writel((u32)paddr, port + PORT_CMD_ACTIVATE);

	for (cnt = 0; cnt < SATA_RESET_TIMEOUT; cnt++) {	// wait 10 sec for Hitachi SATA II HD
		irq_stat = readl(port + PORT_IRQ_STAT);
		writel(irq_stat, port + PORT_IRQ_STAT);		/* clear irq */

		irq_stat >>= PORT_IRQ_RAW_SHIFT;
		if (irq_stat & (PORT_IRQ_COMPLETE | PORT_IRQ_ERROR))
			break;

		msleep(1);
	}

	/* restore IRQs */ // don't enable irq
	//writel(irq_enable, port + PORT_IRQ_ENABLE_SET);

	if (!(irq_stat & PORT_IRQ_COMPLETE))
		return -1;
	return 0;
}

/**
 *	__sata_phy_reset - Wake/reset a low-level SATA PHY
 *
 *	This function issues commands to standard SATA Sxxx
 *	PHY registers, to wake up the phy (and device), and
 *	clear any reset condition.
 *
 *	LOCKING:
 *	PCI/etc. bus probe sem.
 *
 */
static int __sata_phy_reset(int dev_no)
{
	u32 sstatus;
	int i;

        /* issue phy wake/reset */
        scr_write_flush(dev_no, SCR_CONTROL, 0x301);
        /* Couldn't find anything in SATA I/II specs, but
         * AHCI-1.1 10.4.2 says at least 1 ms. */
        mdelay(1);
	scr_write_flush(dev_no, SCR_CONTROL, 0x300); /* phy wake/clear reset */

	/* wait for phy to become ready, if necessary */
	for(i=0; i<10; i++) {
		msleep(200);
		sstatus = sil24_scr_read(dev_no, SCR_STATUS);
		if ((sstatus & 0xf) != 1)
			break;
	}

	/* TODO: phy layer with polling, timeouts, etc. */
#if 0
	if (dev_no!=0)
	   printf("                  ");
#endif
	sstatus = sil24_scr_read(dev_no, SCR_STATUS);
	if (sata_dev_present(dev_no)) {
		const char *speed;
		u32 tmp;

		tmp = (sstatus >> 4) & 0xf;
		if (tmp & (1 << 0))
			speed = "1.5";
		else if (tmp & (1 << 1))
			speed = "3.0";
		else
			speed = "<unknown>";
#if 0
		printf("ata%u: SATA link up %s Gbps (SStatus %X)\n",
		       dev_no, speed, sstatus);
#endif
		return 0;
	} else {
#if 0
		printf("ata%u: SATA link down (SStatus %X)\n", 
		       dev_no, sstatus);
#endif
		return 1;
	}

}

static int sil24_phy_reset(int dev_no)
{

	if ( __sata_phy_reset(dev_no) !=0 )
		return -1;

	if ( sil24_issue_SRST(dev_no) < 0) {
		printf("ata%u: SRST failed, disabling port\n", dev_no);
		return -1;
	}

	return 0;
}


static inline void sil24_fill_sg(void)
{
	struct sil24_sge *sge;
	unsigned int sge_addr;

	sge_addr = (dma_addr_t)sata_private_data;
	sge_addr = (sge_addr - CFG_SDRAM_BASE) & 0xffffffff;
	sge_addr = sge_addr+DMA_BASE_ADDR;		
	
	sge_offset = ((dma_addr_t)sata_private_data)+DMA_BASE_ADDR+DMA_BASE_ADDR-CFG_SDRAM_BASE-sge_addr;

	sge = &sata_cmd_block->ata.sge[0];
	sge->addr = cpu_to_le64(sge_addr);
	sge->cnt  = cpu_to_le32(sata_tf.nsect*512);
	sge->flags = cpu_to_le32(SGE_TRM);

#if ASUS_SATA_DEBUG
	printf("*** sge_addr=%lx, cnt=%d, offset=%x\n", sge_addr, sata_tf.nsect*512, sge_offset);
#endif
}

static void sil24_qc_prep(int dev_no)
{
	struct sil24_prb *prb = &sata_cmd_block->ata.prb;

	switch (sata_tf.protocol) {
	case ATA_PROT_PIO:
	case ATA_PROT_DMA:
	case ATA_PROT_NODATA:
		prb->ctrl = 0;
		break;

	case ATA_PROT_ATAPI:
	case ATA_PROT_ATAPI_DMA:
	case ATA_PROT_ATAPI_NODATA:
		if (sata_tf.protocol != ATA_PROT_ATAPI_NODATA) {
			if (sata_tf.flags & ATA_TFLAG_WRITE)
				prb->ctrl = cpu_to_le16(PRB_CTRL_PACKET_WRITE);
			else
				prb->ctrl = cpu_to_le16(PRB_CTRL_PACKET_READ);
		} else
			prb->ctrl = 0;

		break;

	default:
		break;
	}

	ata_tf_to_fis(&sata_tf, sata_cmd_block->ata.prb.fis, 0);

#if ASUS_SATA_DEBUG
	u8 *ptr;
	int i;
	ptr=(u8 *)prb;

	printf("sil24_qc_prep dump buffer qc->tf.protocol=%d\n", sata_tf.protocol);

	for(i=0; i<0x32; i++)
      	{
	   if ( i%16==7 )
		 printf("%02x-", *(ptr+i));
	   else
		 printf("%02x ", *(ptr+i));
	   if (i%16==15)
		printf("\n");
	}
	printf("\n");

#endif

	sil24_fill_sg();

}


static int sil24_qc_issue(int dev_no)
{
	int cnt;
	unsigned int port = sata_cmd_addr[dev_no];
	dma_addr_t paddr=(dma_addr_t)&sata_cmd_block->ata.prb;
	paddr  = paddr  - CFG_SDRAM_BASE;

	udelay(200);

#if ASUS_SATA_DEBUG	// Dump qc buffer
	printf("sil24_qc_issue dump buffer\n"); 

	u8 *ptr;
	u8 tmp;
	ptr=(u8 *)prb;
	for(i=0; i<0x40; i++)
	   tmp=*(ptr+i);

	for(i=0; i<0x40; i++)
      	{
	   if ( i%16==7 )
		 printf("%02x-", *(ptr+i));
	   else
		 printf("%02x ", *(ptr+i));
	   if (i%16==15)
		printf("\n");
	}
	printf("\n");
#endif
	writel((u32)paddr, port + PORT_CMD_ACTIVATE);

	u32 irq_stat;
	for (cnt = 0; cnt < 1000*1000L; cnt++) {
		irq_stat = readl(port + PORT_IRQ_STAT);
		writel(irq_stat, port + PORT_IRQ_STAT);		// clear irq 

		irq_stat >>= PORT_IRQ_RAW_SHIFT;
		if (irq_stat & (PORT_IRQ_COMPLETE | PORT_IRQ_ERROR))
			break;
		udelay(1);
	}
#if ASUS_SATA_DEBUG
	printf("*** irq_stat=%x, irq_enable=%lx\n", irq_stat, readl(port + PORT_IRQ_ENABLE_SET));
#endif

	if (!(irq_stat & PORT_IRQ_COMPLETE))
	{
  	      	printf("IRQ Error: irq_stat=%x, irq_enable=%lx\n", irq_stat, readl(port + PORT_IRQ_ENABLE_SET));
		return -1;
	}


#if ASUS_SATA_DEBUG
    int j;
    u8	*dbuf;
    dbuf = (u8 *)sata_private_data; 
    dbuf += sge_offset;   
    
    printf("Dump Buffer: \n");
    for(i=0x00; i<512; i+=16)
    {
       printf("%04X: ", i);
       for(j=0; j<16; j++)
       {
          if ( j==7 )
             printf("%02x-",dbuf[i+j]);
          else
             printf("%02x ", dbuf[i+j]);
       }
       printf("  ");
       for(j=0; j<16; j++)
       {
          if ( dbuf[i+j]>=0x20 && dbuf[i+j]<='z' )
             printf("%c", dbuf[i+j]);
          else 
             printf(".");
       }
       printf("\n");
       if (i%256 == 0 && i!=0)
          printf("\n");
    }
#endif

	return 0;
}


static int __sil24_restart_controller(unsigned int port)
{
	u64 tmp;	// u32 -> u64 why??
	int cnt;

	writel(PORT_CS_INIT, port + PORT_CTRL_STAT);

	/* Max ~10ms */
	for (cnt = 0; cnt < 10; cnt++) {
		tmp = (u64) readl(port + PORT_CTRL_STAT);
		if ( tmp & PORT_CS_RDY )
			return 0;
		udelay(1000);
	}

	return -1;
}

#if 0
static void sil24_restart_controller(int dev)
{
	if (__sil24_restart_controller(sata_cmd_addr[dev]) )
		printf(" ata%d: failed to restart controller\n", dev);
}
#endif

static int __sil24_reset_controller(unsigned int port)
{
	int cnt;
	u32 tmp;

	/* Reset controller state.  Is this correct? */
	writel(PORT_CS_DEV_RST, port + PORT_CTRL_STAT);
	readl(port + PORT_CTRL_STAT);	/* sync */

	/* Max ~100ms */
	for (cnt = 0; cnt < 100; cnt++) {
		udelay(1000);
		tmp = readl(port + PORT_CTRL_STAT);
		if (!(tmp & PORT_CS_DEV_RST))
			break;
	}

	if (tmp & PORT_CS_DEV_RST)
		return -1;

	if (tmp & PORT_CS_RDY)
		return 0;

	return __sil24_restart_controller(port);
}


/*-----------------------------------------*/
enum ata_completion_errors {
    AC_ERR_OTHER		= (1 << 0),
    AC_ERR_DEV		= (1 << 1),
    AC_ERR_ATA_BUS		= (1 << 2),
    AC_ERR_HOST_BUS		= (1 << 3),
};



static inline unsigned int ac_err_mask(u8 status)
{
    if (status & ATA_BUSY)
	return AC_ERR_ATA_BUS;
    if (status & (ATA_ERR | ATA_DF))
	return AC_ERR_DEV;
    return 0;
}
/*-----------------------------------------*/



static int sil24_port_start(void)
{

   static int do_once=0;

   if ( do_once )
   {
      memset(sata_private_data, 0, 512);
      return 0;
   }

   sata_private_data = malloc (512);
   if ( sata_private_data == NULL )
   {
      printf("Memory allocate fail.\n");
      return -1;
   }

   sata_cmd_block = malloc (sizeof (struct sil24_cmd_block));
   if ( sata_cmd_block == NULL )
   {
      printf("Memory alllocate fail.\n");
      return -1;
   }

   do_once = 1;
   memset(sata_private_data, 0, 512);

   return 0;
}


#if 0
static void sil24_port_stop(void)
{

   free(sata_private_data);
   free(sata_cmd_block);
}
#endif


static int ata_dev_read(int dev_no, lbaint_t blknr, lbaint_t blkcnt,  ulong *buffer)
{
   u8	*dbuf;
   dbuf = (u8 *)sata_private_data; 
   dbuf += sge_offset; 

   memset(sata_private_data, 0, 512);		// clear DMA buffer

   if ( lba64_mode[dev_no] == 0 )
      sata_tf.command = ATA_CMD_READ;
   else
      sata_tf.command = ATA_CMD_READ_EXT;	// ATA_DFLAG_LBA48 
   sata_tf.feature = 0;
   sata_tf.flags = ATA_TFLAG_ISADDR | ATA_TFLAG_DEVICE | lba64_mode[dev_no];
   sata_tf.protocol = ATA_PROT_ATAPI;

   sata_tf.lbal = ( (blknr >>  0) & 0xFF);
   sata_tf.lbam = ( (blknr >>  8) & 0xFF );
   sata_tf.lbah = ( (blknr >> 16) & 0xFF );

#ifdef CONFIG_LBA48
   if (lba64_mode[dev_no] == ATA_DFLAG_LBA48 )
   {
      sata_tf.hob_lbal = ( (blknr >> 24) & 0xFF);
      sata_tf.hob_lbam = ( (blknr >> 32) & 0xFF);
      sata_tf.hob_lbah = ( (blknr >> 40) & 0xFF);
   }
#endif

   sata_tf.nsect = blkcnt;

   sata_tf.device = ATA_DEVICE_OBS | SATA_LBA;
   sata_tf.ctl |= ATA_DRQ;

   sil24_qc_prep(dev_no);
   if ( sil24_qc_issue(dev_no) != 0)
      return -1;

   memcpy(buffer, dbuf, blkcnt*512L);

#if ASUS_SATA_DEBUG
    int j;
    dbuf = (u8 *)buffer; 

    printf ("ide_read dev %d start %LX, blocks %lX buffer at %lX\n",
		dev_no, blknr, blkcnt, (ulong)buffer);    

    for(i=0x00; i<512; i+=16)
    {
       printf("%04X: ", i);
       for(j=0; j<16; j++)
       {
          if ( j==7 )
             printf("%02x-",dbuf[i+j]);
          else
             printf("%02x ", dbuf[i+j]);
       }
       printf("  ");
       for(j=0; j<16; j++)
       {
          if ( dbuf[i+j]>=0x20 && dbuf[i+j]<='z' )
             printf("%c", dbuf[i+j]);
          else 
             printf(".");
       }
       printf("\n");
       if (i%256 == 0 && i!=0)
          printf("\n");
    }
#endif

   return (0);
}

static int ata_dev_write(int dev_no, lbaint_t blknr, lbaint_t blkcnt, ulong *buffer)
{
   u8	*dbuf;
   dbuf = (u8 *)sata_private_data; 
   dbuf += sge_offset; 

   memset(sata_private_data, 0, 512);		// clear DMA buffer

   if ( lba64_mode[dev_no] == 0 )
      sata_tf.command = ATA_CMD_WRITE;
   else
      sata_tf.command = ATA_CMD_WRITE_EXT;	// ATA_DFLAG_LBA48
   sata_tf.feature = 0;
   sata_tf.flags = ATA_TFLAG_ISADDR | ATA_TFLAG_DEVICE | ATA_TFLAG_WRITE | lba64_mode[dev_no];
   sata_tf.protocol = ATA_PROT_ATAPI;

   sata_tf.lbal = (blknr >>  0) & 0xFF;
   sata_tf.lbam = (blknr >>  8) & 0xFF;
   sata_tf.lbah = (blknr >> 16) & 0xFF;

#ifdef CONFIG_LBA48
   if (lba64_mode[dev_no] == ATA_DFLAG_LBA48 )
   {
      sata_tf.hob_lbal = ( (blknr >> 24) & 0xFF);
      sata_tf.hob_lbam = ( (blknr >> 32) & 0xFF);
      sata_tf.hob_lbah = ( (blknr >> 40) & 0xFF);
   }
#endif

   sata_tf.nsect = blkcnt;

   sata_tf.device = ATA_DEVICE_OBS | SATA_LBA;
   sata_tf.ctl |= ATA_DRQ;

   memcpy(dbuf, buffer, blkcnt*512);

   sil24_qc_prep(dev_no);
   if ( sil24_qc_issue(dev_no) != 0)
      return -1;

   return (0);
}


/**
 *	ata_dev_set_xfermode - Issue SET FEATURES - XFER MODE command
 *	@dev: Device to which command will be sent
 *
 *	Issue SET FEATURES - XFER MODE command to device @dev
 *	on port @ap.
 *
 *	LOCKING:
 *	PCI/etc. bus probe sem.
 */

static int ata_dev_set_xfermode(int dev_no, int xfer_mode)
{

   /* set up set-features taskfile */

   sata_tf.command = ATA_CMD_SET_FEATURES;
   sata_tf.feature = SETFEATURES_XFER;
   sata_tf.flags = ATA_TFLAG_ISADDR | ATA_TFLAG_DEVICE;
   sata_tf.protocol = ATA_PROT_NODATA;
   sata_tf.nsect = xfer_mode;

   sata_tf.device = ATA_DEVICE_OBS;
   sata_tf.ctl |= ATA_DRQ;

   sil24_qc_prep(dev_no);
   if ( sil24_qc_issue(dev_no) != 0)
   {
      return -1;
   }

   return 0;

}

static int ata_dev_identify(int dev_no, unsigned long *buffer, int init_type)
{
   u8	*dbuf;

   dbuf = (u8 *)sata_private_data; 
   dbuf += sge_offset;

   if (!sata_dev_present(dev_no)) {
	return -1;
   }

   sata_tf.feature = 0;
   sata_tf.device = ATA_DEVICE_OBS;
   sata_tf.ctl |= ATA_DRQ;
   sata_tf.command  = ATA_CMD_ID_ATA;
   sata_tf.protocol = ATA_PROT_PIO;
   sata_tf.nsect    = 1;

   sil24_qc_prep(dev_no);
   if ( sil24_qc_issue(dev_no) != 0)
      return -1;

   swap_buf_le16((void *)dbuf, ATA_ID_WORDS);

   if (!ata_id_has_dma(dbuf) || !ata_id_has_lba(dbuf)) 
   {
      //return -1;
   }

   xfer_modes[dev_no] = dbuf[ATA_ID_UDMA_MODES];
   if (!xfer_modes[dev_no])
      xfer_modes[dev_no] = (dbuf[ATA_ID_MWDMA_MODES]) << ATA_SHIFT_MWDMA;
   if (!xfer_modes[dev_no]) 
   {
      xfer_modes[dev_no] = (dbuf[ATA_ID_PIO_MODES]) << (ATA_SHIFT_PIO + 3);
      xfer_modes[dev_no] |= (0x7 << ATA_SHIFT_PIO);
   }

   if (ata_id_has_lba48(dbuf))
      lba64_mode[dev_no] = ATA_DFLAG_LBA48;
   else
      lba64_mode[dev_no] = 0;

   // print device info to dmesg 
    if ( init_type != 0 )
       printf("ata%u: SATA max %s: lba%s mode\n         ",
	    dev_no,
	    ata_mode_string(xfer_modes[dev_no]),
	    (lba64_mode[dev_no] & ATA_DFLAG_LBA48) ? " 48" : "");

#if ASUS_SATA_DEBUG  
    printf("Dump Buffer: \n");
    for(i=0x00; i<512; i+=16)
    {
       printf("%04X: ", i);
       for(j=0; j<16; j++)
       {
          if ( j==7 )
             printf("%02x-",dbuf[i+j]);
          else
             printf("%02x ", dbuf[i+j]);
       }
       printf("  ");
       for(j=0; j<16; j++)
       {
          if ( dbuf[i+j]>=0x20 && dbuf[i+j]<='z' )
             printf("%c", dbuf[i+j]);
          else 
             printf(".");
       }
       printf("\n");
       if (i%256 == 0 && i!=0)
          printf("\n");
    }
#endif

   memcpy(buffer, dbuf, 512);

   return 0;
}

static int ata_device_add(int dev_no)
{
   unsigned char buf[512];

#if ASUS_SATA_DEBUG 
   printf("   ata%u: %cATA max %s cmd 0x%lX ctl 0x%lX bmdma 0x%lX irq %lu\n",
			dev_no,
			'S',
			"UDMA/133Hz",
	       		sata_cmd_addr[dev_no],
	       		0,
	       		0,
	       		0);
#endif
   if ( sil24_phy_reset(dev_no) != 0)
	return -1;

   memset(buf,0,sizeof(buf));
   
  if ( ata_dev_identify(dev_no, (void *)buf, 0) != 0)      
     return -1;

   return 0;

}

int sil24_init_one(void)
{
        static int board_num = 0;
	int ret_val=0;
	int dev_val=0;
        uint32_t host_base;
        uint32_t port_base;

	int i;
	pci_dev_t dev;

        if (board_num > 0)
        {
            printf("ERROR, only a single SATA 3124 device is supported\n");
            return -1;
        }
	dev = pci_find_device(0x1095, 0x3124, board_num);
        if (dev < 0)

	{
#if CONFIG_OCTEON_NAC38
            if (!board_num)
                printf("Can't find SATA 3124 device\n");
#endif
	   return -1;
	}
       	
	pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &host_base);
	host_base = host_base & 0xfffffff0;

	pci_read_config_dword(dev, PCI_BASE_ADDRESS_2, &port_base);
	port_base = port_base & 0xfffffff0;


        /* Register (just partially) with ide subsystem */
        ide_register_device(IF_TYPE_SATA, BLOCK_QUIRK_IDE_4PORT, 0, host_base,0,port_base);
        ide_register_device(IF_TYPE_SATA, BLOCK_QUIRK_IDE_4PORT, 1, host_base,0,port_base);
        ide_register_device(IF_TYPE_SATA, BLOCK_QUIRK_IDE_4PORT, 2, host_base,0,port_base);
        ide_register_device(IF_TYPE_SATA, BLOCK_QUIRK_IDE_4PORT, 3, host_base,0,port_base);

        /*
	 * Configure the device
	 */
	/*
	 * FIXME: This device is certainly 64-bit capable.  We just
	 * don't know how to use it.  After fixing 32bit activation in
	 * this function, enable 64bit masks here. DMA_27BIT_MASK DMA_32BIT_MASK DMA_64BIT_MASK
	 */

	/* GPIO off */
	writel(0, host_base + HOST_FLASH_CMD);

	/* Mask interrupts during initialization */
	writel(0, host_base + HOST_CTRL);

	for (i = 0; i < 4; i++) {
		unsigned long port = port_base + i * PORT_REGS_SIZE;
		unsigned long portu = (unsigned long)port;
		u32 tmp;
		int cnt;

		sata_cmd_addr[i] = portu + PORT_PRB;
		sata_scr_addr[i] = portu + PORT_SCONTROL;

		//ata_std_ports(&probe_ent->port[i]);

		/* Initial PHY setting */
		writel(0x20c, port + PORT_PHY_CFG);

		/* Clear port RST */
		tmp = readl(port + PORT_CTRL_STAT);
		if (tmp & PORT_CS_PORT_RST) {
			writel(PORT_CS_PORT_RST, port + PORT_CTRL_CLR);
			readl(port + PORT_CTRL_STAT);	/* sync */
			for (cnt = 0; cnt < 10; cnt++) {
				msleep(10);
				tmp = readl(port + PORT_CTRL_STAT);
				if (!(tmp & PORT_CS_PORT_RST))
					break;
			}
			if (tmp & PORT_CS_PORT_RST)
				printf("failed to clear port RST\n");
		}

		/* Zero error counters. */
		writew(0x8000, port + PORT_DECODE_ERR_THRESH);
		writew(0x8000, port + PORT_CRC_ERR_THRESH);
		writew(0x8000, port + PORT_HSHK_ERR_THRESH);
		writew(0x0000, port + PORT_DECODE_ERR_CNT);
		writew(0x0000, port + PORT_CRC_ERR_CNT);
		writew(0x0000, port + PORT_HSHK_ERR_CNT);

		/* FIXME: 32bit activation? */
		writel(0, port + PORT_ACTIVATE_UPPER_ADDR);
		writel(PORT_CS_32BIT_ACTV, port + PORT_CTRL_STAT);

		/* Configure interrupts */
		writel(0xffff, port + PORT_IRQ_ENABLE_CLR);
		writel(PORT_IRQ_COMPLETE | PORT_IRQ_ERROR | PORT_IRQ_SDB_FIS,
		       port + PORT_IRQ_ENABLE_SET);

		/* Clear interrupts */
		writel(0x0fff0fff, port + PORT_IRQ_STAT);
		writel(PORT_CS_IRQ_WOC, port + PORT_CTRL_CLR);

		/* Clear port multiplier enable and resume bits */
		writel(PORT_CS_PM_EN | PORT_CS_RESUME, port + PORT_CTRL_CLR);

		/* Reset itself */
		if (__sil24_reset_controller(port))
			printf("failed to reset controller\n");
	}

	msleep(10);	// *** East add
	/* Turn on interrupts */
	writel(IRQ_STAT_4PORTS, host_base + HOST_CTRL);

	sil24_port_start();

	//pci_set_master(pdev);

	/* FIXME: check ata_device_add return value */
	ret_val = -1;
	for(i=0; i<4; i++)
	{ 
	   dev_val = ata_device_add(i);
	   if ( dev_val == 0 )
	      ret_val = 0;
	}

        board_num++;
	//free(probe_ent);
	return ret_val;
}


/* ------------------------------------------------------------------------- */

/*
 * copy src to dest, skipping leading and trailing blanks and null
 * terminate the string
 * "len" is the size of available memory including the terminating '\0'
 */
static void ident_cpy (unsigned char *dst, unsigned char *src, unsigned int len)
{
	unsigned char *end, *last;

	last = dst;
	end  = src + len - 1;

	/* reserve space for '\0' */
	if (len < 2)
		goto OUT;

	/* skip leading white space */
	while ((*src) && (src<end) && (*src==' '))
		++src;

	/* copy string, omitting trailing white space */
	while ((*src) && (src<end)) {
		*dst++ = *src;
		if (*src++ != ' ')
			last = dst;
	}
OUT:
	*last = '\0';
}

void sata_ident (block_dev_desc_t *dev_desc)
{
	static int do_first=0;
	int device;
	device=dev_desc->dev_on_bus;;

	ulong iobuf[ATA_SECTORWORDS];
	hd_driveid_t *iop = (hd_driveid_t *)iobuf;
	memset(iobuf,0,sizeof(iobuf));

        if ( ata_dev_identify(device, iobuf, 1) != 0 )
	   return;

	if ( do_first == 0 )
        {
  	   ata_dev_set_xfermode(device, xfer_modes[device]);
           do_first = 1;
        }

	dev_desc->if_type=IF_TYPE_SATA;

	ident_cpy (dev_desc->revision, iop->fw_rev, sizeof(dev_desc->revision));
	ident_cpy (dev_desc->vendor, iop->model, sizeof(dev_desc->vendor));
	ident_cpy (dev_desc->product, iop->serial_no, sizeof(dev_desc->product));
#ifdef __LITTLE_ENDIAN
	/*
	 * firmware revision and model number have Big Endian Byte
	 * order in Word. Convert both to little endian.
	 *
	 * See CF+ and CompactFlash Specification Revision 2.0:
	 * 6.2.1.6: Identfy Drive, Table 39 for more details
	 */

	strswab (dev_desc->revision);
	strswab (dev_desc->vendor);
#endif /* __LITTLE_ENDIAN */

	if ((iop->config & 0x0080)==0x0080)
		dev_desc->removable = 1;
	else
		dev_desc->removable = 0;

#ifdef __BIG_ENDIAN
	/* swap shorts */
	dev_desc->lba = (iop->lba_capacity << 16) | (iop->lba_capacity >> 16);
#else	/* ! __BIG_ENDIAN */
	/*
	 * do not swap shorts on little endian
	 *
	 * See CF+ and CompactFlash Specification Revision 2.0:
	 * 6.2.1.6: Identfy Drive, Table 39, Word Address 57-58 for details.
	 */
	dev_desc->lba = iop->lba_capacity;
#endif	/* __BIG_ENDIAN */

#ifdef CONFIG_LBA48
	if (iop->command_set_2 & 0x0400) { /* LBA 48 support */
		dev_desc->lba48 = 1;
		dev_desc->lba = (unsigned long long)iop->lba48_capacity[0] |
						  ((unsigned long long)iop->lba48_capacity[1] << 16) |
						  ((unsigned long long)iop->lba48_capacity[2] << 32) |
						  ((unsigned long long)iop->lba48_capacity[3] << 48);
	} else {
		dev_desc->lba48 = 0;
	}
#endif /* CONFIG_LBA48 */
	/* assuming HD */
	dev_desc->type=DEV_TYPE_HARDDISK;
	dev_desc->blksz=ATA_BLOCKSIZE;
	dev_desc->lun=0; /* just to fill something in... */


}

ulong sata_read (int device, lbaint_t blknr, ulong blkcnt, ulong *buffer)
{
   int ret, n;
   
   
   n=0;
   while (blkcnt-- > 0) 
   {
       ret = ata_dev_read(ide_get_dev(device)->dev_on_bus, blknr, 1, buffer);
       if ( ret != 0 )
	  return n;

	++n;
	++blknr;
	buffer += ATA_SECTORWORDS;
   }

   return (n);
}

ulong sata_write (int device, lbaint_t blknr, ulong blkcnt, ulong *buffer)
{
   int ret, n;

   n=0;
   while (blkcnt-- > 0) 
   {
       ret = ata_dev_write(ide_get_dev(device)->dev_on_bus, blknr, 1, buffer);
       if ( ret != 0 )
	  return n;
	++n;
	++blknr;
	buffer += ATA_SECTORWORDS;
   }

   return (n);
}

void sil24_sata_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
   sil24_init_one();
}


#endif /* CFG_SIL_SATA_3124 */

#endif /* CONFIG_PCI */
