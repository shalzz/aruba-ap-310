
/*
   Copyright 2003-2004 Red Hat, Inc.  All rights reserved.
   Copyright 2003-2004 Jeff Garzik

   The contents of this file are subject to the Open
   Software License version 1.1 that can be found at
   http://www.opensource.org/licenses/osl-1.1.txt and is included herein
   by reference.

   Alternatively, the contents of this file may be used under the terms
   of the GNU General Public License version 2 (the "GPL") as distributed
   in the kernel source COPYING file, in which case the provisions of
   the GPL are applicable instead of the above.  If you wish to allow
   the use of your version of this file only under the terms of the
   GPL and not to allow others to use your version of this file under
   the OSL, indicate your decision by deleting the provisions above and
   replace them with the notice and other provisions required by the GPL.
   If you do not delete the provisions above, a recipient may use your
   version of this file under either the OSL or the GPL.

 */

//#ifndef __LINUX_ATA_H__
//#define __LINUX_ATA_H__

//#include <linux/types.h>

/* defines only for the constants which don't work well as enums */
#define ATA_DMA_BOUNDARY	0xffffUL
#define ATA_DMA_MASK		0xffffffffULL
#define HZ 1000

enum {
	/* various global constants */
	ATA_MAX_DEVICES		= 2,	/* per bus/port */
	ATA_MAX_PRD		= 256,	/* we could make these 256/256 */
	ATA_SECT_SIZE		= 512,

	ATA_ID_WORDS		= 256,
	ATA_ID_PROD_OFS		= 27,
	ATA_ID_FW_REV_OFS	= 23,
	ATA_ID_SERNO_OFS	= 10,
	ATA_ID_MAJOR_VER	= 80,
	ATA_ID_PIO_MODES	= 64,
	ATA_ID_MWDMA_MODES	= 63,
	ATA_ID_UDMA_MODES	= 88,
	ATA_ID_PIO4		= (1 << 1),

	ATA_PCI_CTL_OFS		= 2,
	ATA_SERNO_LEN		= 20,
	ATA_UDMA0		= (1 << 0),
	ATA_UDMA1		= ATA_UDMA0 | (1 << 1),
	ATA_UDMA2		= ATA_UDMA1 | (1 << 2),
	ATA_UDMA3		= ATA_UDMA2 | (1 << 3),
	ATA_UDMA4		= ATA_UDMA3 | (1 << 4),
	ATA_UDMA5		= ATA_UDMA4 | (1 << 5),
	ATA_UDMA6		= ATA_UDMA5 | (1 << 6),
	ATA_UDMA7		= ATA_UDMA6 | (1 << 7),
	/* ATA_UDMA7 is just for completeness... doesn't exist (yet?).  */

	ATA_UDMA_MASK_40C	= ATA_UDMA2,	/* udma0-2 */

	/* DMA-related */
	ATA_PRD_SZ		= 8,
	ATA_PRD_TBL_SZ		= (ATA_MAX_PRD * ATA_PRD_SZ),
	ATA_PRD_EOT		= (1 << 31),	/* end-of-table flag */

	ATA_DMA_TABLE_OFS	= 4,
	ATA_DMA_STATUS		= 2,
	ATA_DMA_CMD		= 0,
	ATA_DMA_WR		= (1 << 3),
	ATA_DMA_START		= (1 << 0),
	ATA_DMA_INTR		= (1 << 2),
	ATA_DMA_ERR		= (1 << 1),
	ATA_DMA_ACTIVE		= (1 << 0),

	/* bits in ATA command block registers */
	ATA_HOB			= (1 << 7),	/* LBA48 selector */
	ATA_NIEN		= (1 << 1),	/* disable-irq flag */
	SATA_LBA		= (1 << 6),	/* LBA28 selector */
	ATA_DEV1		= (1 << 4),	/* Select Device 1 (slave) */
	ATA_DEVICE_OBS		= (1 << 7) | (1 << 5), /* obs bits in dev reg */
	ATA_DEVCTL_OBS		= (1 << 3),	/* obsolete bit in devctl reg */
	ATA_BUSY		= (1 << 7),	/* BSY status bit */
	ATA_DRDY		= (1 << 6),	/* device ready */
	ATA_DF			= (1 << 5),	/* device fault */
	ATA_DRQ			= (1 << 3),	/* data request i/o */
	ATA_ERR			= (1 << 0),	/* have an error */
	ATA_SRST		= (1 << 2),	/* software reset */
	ATA_ABORTED		= (1 << 2),	/* command aborted */

	/* ATA command block registers */
	ATA_REG_DATA		= 0x00,
	ATA_REG_ERR		= 0x01,
	ATA_REG_NSECT		= 0x02,
	ATA_REG_LBAL		= 0x03,
	ATA_REG_LBAM		= 0x04,
	ATA_REG_LBAH		= 0x05,
	ATA_REG_DEVICE		= 0x06,
	ATA_REG_STATUS		= 0x07,

	ATA_REG_FEATURE		= ATA_REG_ERR, /* and their aliases */
	ATA_REG_CMD		= ATA_REG_STATUS,
	ATA_REG_BYTEL		= ATA_REG_LBAM,
	ATA_REG_BYTEH		= ATA_REG_LBAH,
	ATA_REG_DEVSEL		= ATA_REG_DEVICE,
	ATA_REG_IRQ		= ATA_REG_NSECT,

	/* ATA device commands */
	ATA_CMD_CHK_POWER	= 0xE5, /* check power mode */
	ATA_CMD_EDD		= 0x90,	/* execute device diagnostic */
	ATA_CMD_FLUSH		= 0xE7,
	ATA_CMD_FLUSH_EXT	= 0xEA,
	ATA_CMD_ID_ATA		= 0xEC,
	ATA_CMD_ID_ATAPI	= 0xA1,
	SATA_CMD_READ		= 0xC8,
	SATA_CMD_READ_EXT	= 0x25,
	SATA_CMD_WRITE		= 0xCA,
	SATA_CMD_WRITE_EXT	= 0x35,
	ATA_CMD_PIO_READ	= 0x20,
	ATA_CMD_PIO_READ_EXT	= 0x24,
	ATA_CMD_PIO_WRITE	= 0x30,
	ATA_CMD_PIO_WRITE_EXT	= 0x34,
	ATA_CMD_SET_FEATURES	= 0xEF,
	ATA_CMD_PACKET		= 0xA0,
	ATA_CMD_VERIFY		= 0x40,
	ATA_CMD_VERIFY_EXT	= 0x42,

	/* SETFEATURES stuff */
	SETFEATURES_XFER	= 0x03,
	XFER_UDMA_7		= 0x47,
	XFER_UDMA_6		= 0x46,
	XFER_UDMA_5		= 0x45,
	XFER_UDMA_4		= 0x44,
	XFER_UDMA_3		= 0x43,
	XFER_UDMA_2		= 0x42,
	XFER_UDMA_1		= 0x41,
	XFER_UDMA_0		= 0x40,
	XFER_MW_DMA_2		= 0x22,
	XFER_MW_DMA_1		= 0x21,
	XFER_MW_DMA_0		= 0x20,
	XFER_PIO_4		= 0x0C,
	XFER_PIO_3		= 0x0B,
	XFER_PIO_2		= 0x0A,
	XFER_PIO_1		= 0x09,
	XFER_PIO_0		= 0x08,
	XFER_SW_DMA_2		= 0x12,
	XFER_SW_DMA_1		= 0x11,
	XFER_SW_DMA_0		= 0x10,
	XFER_PIO_SLOW		= 0x00,

	/* ATAPI stuff */
	ATAPI_PKT_DMA		= (1 << 0),
	ATAPI_DMADIR		= (1 << 2),	/* ATAPI data dir:
						   0=to device, 1=to host */
	ATAPI_CDB_LEN		= 16,

	/* cable types */
	ATA_CBL_NONE		= 0,
	ATA_CBL_PATA40		= 1,
	ATA_CBL_PATA80		= 2,
	ATA_CBL_PATA_UNK	= 3,
	ATA_CBL_SATA		= 4,

	/* SATA Status and Control Registers */
	SCR_STATUS		= 0,
	SCR_ERROR		= 1,
	SCR_CONTROL		= 2,
	SCR_ACTIVE		= 3,
	SCR_NOTIFICATION	= 4,

	/* struct ata_taskfile flags */
	ATA_TFLAG_LBA48		= (1 << 0), /* enable 48-bit LBA and "HOB" */
	ATA_TFLAG_ISADDR	= (1 << 1), /* enable r/w to nsect/lba regs */
	ATA_TFLAG_DEVICE	= (1 << 2), /* enable r/w to device reg */
	ATA_TFLAG_WRITE		= (1 << 3), /* data dir: host->dev==1 (write) */
};

enum ata_tf_protocols {
	/* ATA taskfile protocols */
	ATA_PROT_UNKNOWN,	/* unknown/invalid */
	ATA_PROT_NODATA,	/* no data */
	ATA_PROT_PIO,		/* PIO single sector */
	ATA_PROT_PIO_MULT,	/* PIO multiple sector */
	ATA_PROT_DMA,		/* DMA */
	ATA_PROT_ATAPI,		/* packet command, PIO data xfer*/
	ATA_PROT_ATAPI_NODATA,	/* packet command, no data */
	ATA_PROT_ATAPI_DMA,	/* packet command with special DMA sauce */
};

enum ata_ioctls {
	ATA_IOC_GET_IO32	= 0x309,
	ATA_IOC_SET_IO32	= 0x324,
};

/* core structures */

struct ata_prd {
	u32			addr;
	u32			flags_len;
};

struct ata_taskfile {
	unsigned long		flags;		/* ATA_TFLAG_xxx */
	u8			protocol;	/* ATA_PROT_xxx */

	u8			ctl;		/* control reg */

	u8			hob_feature;	/* additional data */
	u8			hob_nsect;	/* to support LBA48 */
	u8			hob_lbal;
	u8			hob_lbam;
	u8			hob_lbah;

	u8			feature;
	u8			nsect;
	u8			lbal;
	u8			lbam;
	u8			lbah;

	u8			device;

	u8			command;	/* IO operation */
};

#define ata_id_is_ata(id)	(((id)[0] & (1 << 15)) == 0)
#define ata_id_is_sata(id)	((id)[93] == 0)
#define ata_id_rahead_enabled(id) ((id)[85] & (1 << 6))
#define ata_id_wcache_enabled(id) ((id)[85] & (1 << 5))
#define ata_id_has_flush(id) ((id)[83] & (1 << 12))
#define ata_id_has_flush_ext(id) ((id)[83] & (1 << 13))
#define ata_id_has_lba48(id)	((id)[83] & (1 << 10))
#define ata_id_has_wcache(id)	((id)[82] & (1 << 5))
#define ata_id_has_pm(id)	((id)[82] & (1 << 3))
#define ata_id_has_lba(id)	((id)[49] & (1 << 9))
#define ata_id_has_dma(id)	((id)[49] & (1 << 8))
#define ata_id_removeable(id)	((id)[0] & (1 << 7))
#define ata_id_u32(id,n)	\
	(((u32) (id)[(n) + 1] << 16) | ((u32) (id)[(n)]))
#define ata_id_u64(id,n)	\
	( ((u64) (id)[(n) + 3] << 48) |	\
	  ((u64) (id)[(n) + 2] << 32) |	\
	  ((u64) (id)[(n) + 1] << 16) |	\
	  ((u64) (id)[(n) + 0]) )

static inline int atapi_cdb_len(u16 *dev_id)
{
	u16 tmp = dev_id[0] & 0x3;
	switch (tmp) {
	case 0:		return 12;
	case 1:		return 16;
	default:	return -1;
	}
}

static inline int is_atapi_taskfile(struct ata_taskfile *tf)
{
	return (tf->protocol == ATA_PROT_ATAPI) ||
	       (tf->protocol == ATA_PROT_ATAPI_NODATA) ||
	       (tf->protocol == ATA_PROT_ATAPI_DMA);
}

static inline int ata_ok(u8 status)
{
	return ((status & (ATA_BUSY | ATA_DRDY | ATA_DF | ATA_DRQ | ATA_ERR))
			== ATA_DRDY);
}

//#endif /* __LINUX_ATA_H__ */



enum {
	/* various global constants */
	LIBATA_MAX_PRD		= ATA_MAX_PRD / 2,
	ATA_MAX_PORTS		= 8,
	ATA_DEF_QUEUE		= 1,
	ATA_MAX_QUEUE		= 1,
	ATA_MAX_SECTORS		= 200,	/* FIXME */
	ATA_MAX_BUS		= 2,
	ATA_DEF_BUSY_WAIT	= 10000,
	ATA_SHORT_PAUSE		= (HZ >> 6) + 1,

	ATA_SHT_EMULATED	= 1,
	ATA_SHT_CMD_PER_LUN	= 1,
	ATA_SHT_THIS_ID		= -1,
	ATA_SHT_USE_CLUSTERING	= 0,

	/* struct ata_device stuff */
	ATA_DFLAG_LBA48		= (1 << 0), /* device supports LBA48 */
	ATA_DFLAG_PIO		= (1 << 1), /* device currently in PIO mode */
	ATA_DFLAG_LOCK_SECTORS	= (1 << 2), /* don't adjust max_sectors */

	ATA_DEV_UNKNOWN		= 0,	/* unknown device */
	ATA_DEV_ATA		= 1,	/* ATA device */
	ATA_DEV_ATA_UNSUP	= 2,	/* ATA device (unsupported) */
	ATA_DEV_ATAPI		= 3,	/* ATAPI device */
	ATA_DEV_ATAPI_UNSUP	= 4,	/* ATAPI device (unsupported) */
	ATA_DEV_NONE		= 5,	/* no device */

	/* struct ata_port flags */
	ATA_FLAG_SLAVE_POSS	= (1 << 1), /* host supports slave dev */
					    /* (doesn't imply presence) */
	ATA_FLAG_PORT_DISABLED	= (1 << 2), /* port is disabled, ignore it */
	ATA_FLAG_SATA		= (1 << 3),
	ATA_FLAG_NO_LEGACY	= (1 << 4), /* no legacy mode check */
	ATA_FLAG_SRST		= (1 << 5), /* use ATA SRST, not E.D.D. */
	ATA_FLAG_MMIO		= (1 << 6), /* use MMIO, not PIO */
	ATA_FLAG_SATA_RESET	= (1 << 7), /* use COMRESET */
	ATA_FLAG_PIO_DMA	= (1 << 8), /* PIO cmds via DMA */

	ATA_QCFLAG_ACTIVE	= (1 << 1), /* cmd not yet ack'd to scsi lyer */
	ATA_QCFLAG_SG		= (1 << 3), /* have s/g table? */
	ATA_QCFLAG_SINGLE	= (1 << 4), /* no s/g, just a single buffer */
	ATA_QCFLAG_DMAMAP	= ATA_QCFLAG_SG | ATA_QCFLAG_SINGLE,

	/* various lengths of time */
	ATA_TMOUT_EDD		= 5 * HZ,	/* hueristic */
	ATA_TMOUT_PIO		= 30 * HZ,
	ATA_TMOUT_BOOT		= 30 * HZ,	/* hueristic */
	ATA_TMOUT_BOOT_QUICK	= 7 * HZ,	/* hueristic */
	ATA_TMOUT_CDB		= 30 * HZ,
	ATA_TMOUT_CDB_QUICK	= 5 * HZ,

	/* ATA bus states */
	BUS_UNKNOWN		= 0,
	BUS_DMA			= 1,
	BUS_IDLE		= 2,
	BUS_NOINTR		= 3,
	BUS_NODATA		= 4,
	BUS_TIMER		= 5,
	BUS_PIO			= 6,
	BUS_EDD			= 7,
	BUS_IDENTIFY		= 8,
	BUS_PACKET		= 9,

	/* SATA port states */
	PORT_UNKNOWN		= 0,
	PORT_ENABLED		= 1,
	PORT_DISABLED		= 2,

	/* encoding various smaller bitmaps into a single
	 * unsigned long bitmap
	 */
	ATA_SHIFT_UDMA		= 0,
	ATA_SHIFT_MWDMA		= 8,
	ATA_SHIFT_PIO		= 11,
};

enum pio_task_states {
	PIO_ST_UNKNOWN,
	PIO_ST_IDLE,
	PIO_ST_POLL,
	PIO_ST_TMOUT,
	PIO_ST,
	PIO_ST_LAST,
	PIO_ST_LAST_POLL,
	PIO_ST_ERR,
};

