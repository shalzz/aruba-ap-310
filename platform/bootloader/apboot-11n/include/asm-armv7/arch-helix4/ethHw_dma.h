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
/*
 * Generic Broadcom Home Networking Division (HND) DMA engine SW interface
 * This supports the following chips: BCM42xx, 44xx, 47xx .
 *
 * $Id: ethHw_dma.h 241182 2011-02-17 21:50:03Z gmo $
 */

#ifndef	_eth_hw_dma_h_
#define	_eth_hw_dma_h_

#include <malloc.h>
#include <stdbool.h>
#include <asm/arch/iproc_gmac_regs.h>
#include <asm/arch/bcmdmareg.h>
#include <asm/arch/bcmutils.h>

#define	I_ERRORS		(I_PDEE | I_PDE | I_DE | I_RDU | I_RFO | I_XFU)
#define	DEF_INTMASK		(I_XI0 | I_XI1 | I_XI2 | I_XI3 | I_RI | I_ERRORS)

typedef struct {
	uint32 loaddr;
	uint32 hiaddr;
} dma64addr_t;

typedef dma64addr_t dmaaddr_t;

/* range param for dma_getnexttxp() and dma_txreclaim */
typedef enum txd_range {
	HNDDMA_RANGE_ALL		= 1,
	HNDDMA_RANGE_TRANSMITTED,
	HNDDMA_RANGE_TRANSFERED
} txd_range_t;

/* dma function type */

#define osldma_t ulong
#define hnddma_seg_map_t void

/* map/unmap direction */
#define	DMA_TX			1	/* TX direction for DMA */
#define	DMA_RX			2	/* RX direction for DMA */

#define NTXD			2
#define NRXD			8
#define NRXBUFPOST		8

/*----------------------------------------**
**    DMA Descriptor and buffer memory    **
**----------------------------------------*/
/* Memory for RX buffers and RX DMA descriptors. */
#define RX_BUF_SIZE        2048
#define RX_BUF_NUM         NRXD
#define RX_DESC_NUM        RX_BUF_NUM

/* Use 2nd half of reserved uncache RAM (2MB) for Rx buffer */
#define RX_BUF_BASE        (rxDataBuf)
#define RX_BUF_LEN         (RX_BUF_NUM * RX_BUF_SIZE)
/* descriptor table needs to be on 4KB boundry xxxx-x000 */
#define RX_DESC_BASE       (rxDescAlignedBuf)
#define RX_DESC_LEN        (RX_DESC_NUM * sizeof( dma64dd_t ))
/* descriptor base should be RX_BUFF_BASE + 0x4000 - so ok */

#define RX_BUF(i)          (uint8_t *)(RX_BUF_BASE + ((i) * RX_BUF_SIZE))
#define RX_DESC(i)         (dma64dd_t *)(RX_DESC_BASE + ((i) * sizeof( dma64dd_t )))
#define RX_FLUSH_CACHE()

/* Memory for TX buffers and TX DMA descriptors.
 * For every Tx buffer, there must be two descriptors (one for config and one
 * for data) and one config buffer (for config information)
 */
#define TX_BUF_SIZE        2048
#define TX_BUF_NUM         NTXD
#define TX_DESC_NUM        TX_BUF_NUM

/* Starting from RX_BUF_BASE + 128KB for Tx buffer */
#define TX_BUF_BASE        (txDataBuf)
#define TX_BUF_LEN         (TX_BUF_NUM * TX_BUF_SIZE)
/* descriptor table needs to be on 4KB boundry xxxx-x000 */
#define TX_DESC_BASE       (txDescAlignedBuf)
#define TX_DESC_LEN        (TX_DESC_NUM * sizeof( dma64dd_t ))
/* descriptor base should be RX_BUFF_BASE + 0x1000 - so ok */

#define TX_BUF(i)          (uint8_t *)(TX_BUF_BASE + ((i) * TX_BUF_SIZE))
#define TX_DESC(i)         (dma64dd_t *)(TX_DESC_BASE + ((i) * sizeof( dma64dd_t )))
#define TX_FLUSH_CACHE()

#define PHYSADDR_MASK	0xffffffff

/* macros to convert between byte offsets and indexes */
#define	B2I(bytes, type)	((bytes) / sizeof(type))
#define	I2B(index, type)	((index) * sizeof(type))
/*
 * Map an address to a certain kernel segment
 */
#define _ULCAST_ (unsigned long)
#undef PHYSADDR
#define PHYSADDR(a)	(_ULCAST_(a) & PHYSADDR_MASK)
#define PHYSADDRHI(_pa) ((_pa).hiaddr)
#define PHYSADDRHISET(_pa, _val) \
	do { \
		(_pa).hiaddr = (_val);		\
	} while (0)
#define PHYSADDRLO(_pa) ((_pa).loaddr)
#define PHYSADDRLOSET(_pa, _val) \
	do { \
		(_pa).loaddr = (_val);		\
	} while (0)

#define CFE_CACHE_FLUSH_D	1
#define CFE_CACHE_INVAL_I	2
#define CFE_CACHE_INVAL_D	4
#define CFE_CACHE_INVAL_L2	8

/* map/unmap shared (dma-able) memory */
#define	DMA_MAP(osh, va, size, direction, lb, dmah) ({ \
	PHYSADDR((ulong)(va)); \
})
#define	DMA_UNMAP(osh, pa, size, direction, p, dmah) \
	do {} while (0)


#if defined(BCM_RPC_NOCOPY) || defined(BCM_RCP_TXNOCOPY)
/* add 40 bytes to allow for extra RPC header and info  */
#define BCMEXTRAHDROOM 220
#else /* BCM_RPC_NOCOPY || BCM_RPC_TXNOCOPY */
#ifdef CTFMAP
#define BCMEXTRAHDROOM 176
#else /* CTFMAP */
#define BCMEXTRAHDROOM 172
#endif /* CTFMAP */
#endif /* BCM_RPC_NOCOPY || BCM_RPC_TXNOCOPY */

/* register access macros */
#ifdef IL_BIGENDIAN
#define wreg32(r, v)		(*(volatile uint32*)(r) = (uint32)(v))
#define rreg32(r)		(*(volatile uint32*)(r))
#define wreg16(r, v)		(*(volatile uint16*)((ulong)(r) ^ 2) = (uint16)(v))
#define rreg16(r)		(*(volatile uint16*)((ulong)(r) ^ 2))
#define wreg8(r, v)		(*(volatile uint8*)((ulong)(r) ^ 3) = (uint8)(v))
#define rreg8(r)		(*(volatile uint8*)((ulong)(r) ^ 3))
#else	/* !IL_BIGENDIAN */
#define wreg32(r, v)		(*(volatile uint32*)(r) = (uint32)(v))
#define rreg32(r)		(*(volatile uint32*)(r))
#define wreg16(r, v)		(*(volatile uint16*)(r) = (uint16)(v))
#define rreg16(r)		(*(volatile uint16*)(r))
#define wreg8(r, v)		(*(volatile uint8*)(r) = (uint8)(v))
#define rreg8(r)		(*(volatile uint8*)(r))
#endif	/* IL_BIGENDIAN */
#define R_REG(osh, r) ({ \
	__typeof(*(r)) __osl_v; \
	switch (sizeof(*(r))) { \
	case sizeof(uint8):	__osl_v = rreg8((void *)(r)); break; \
	case sizeof(uint16):	__osl_v = rreg16((void *)(r)); break; \
	case sizeof(uint32):	__osl_v = rreg32((void *)(r)); break; \
	} \
	__osl_v; \
})
#define W_REG(osh, r, v) do { \
	switch (sizeof(*(r))) { \
	case sizeof(uint8):	wreg8((void *)(r), (v)); break; \
	case sizeof(uint16):	wreg16((void *)(r), (v)); break; \
	case sizeof(uint32):	wreg32((void *)(r), (v)); break; \
	} \
} while (0)
#define	AND_REG(osh, r, v)		W_REG(osh, (r), R_REG(osh, r) & (v))
#define	OR_REG(osh, r, v)		W_REG(osh, (r), R_REG(osh, r) | (v))

/* descriptor bumping macros */
#define	XXD(x, n)	((x) & ((n) - 1))	/* faster than %, but n must be power of 2 */
#define	TXD(x)		XXD((x), di->ntxd)
#define	RXD(x)		XXD((x), di->nrxd)
#define	NEXTTXD(i)	TXD((i) + 1)
#define	PREVTXD(i)	TXD((i) - 1)
#define	NEXTRXD(i)	RXD((i) + 1)
#define	PREVRXD(i)	RXD((i) - 1)

#define	NTXDACTIVE(h, t)	TXD((t) - (h))
#define	NRXDACTIVE(h, t)	RXD((t) - (h))

#define	R_SM(r)			*(r)
#define	W_SM(r, v)		(*(r) = (v))

/* allocate/free shared (dma-able) consistent (uncached) memory */
#define	DMA_CONSISTENT_ALIGN	4096		/* 4k alignment */
#define	DMA_ALLOC_CONSISTENT(osh, size, align, tot, pap, dmah) \
	osl_dma_alloc_consistent((osh), (size), (align), (tot), (pap))
#define	DMA_FREE_CONSISTENT(osh, va, size, pa, dmah) \
	osl_dma_free_consistent((osh), (void*)(va), (size), (pa))

#ifndef _hnddma_pub_
#define _hnddma_pub_
typedef const struct hnddma_pub hnddma_t;
#endif /* _hnddma_pub_ */

#define DMAREG(ch, dir, qnum)	((dir == DMA_TX) ? \
	                         (void *)(uintptr)&(ch->regs->dma_regs[qnum].dmaxmt) : \
	                         (void *)(uintptr)&(ch->regs->dma_regs[qnum].dmarcv))

/*
 * Exported data structure (read-only)
 */
/* export structure */
struct hnddma_pub {
	uint		txavail;	/* # free tx descriptors */
	uint		dmactrlflags;	/* dma control flags */

	/* rx error counters */
	uint		rxgiants;	/* rx giant frames */
	uint		rxnobuf;	/* rx out of dma descriptors */
	/* tx error counters */
	uint		txnobuf;	/* tx out of dma descriptors */
	uint		txnodesc;	/* tx out of dma descriptors running count */
};

/*
 * Least-common denominator rxbuf start-of-data offset:
 * Must be >= size of largest rxhdr
 * Must be 2-mod-4 aligned so IP is 0-mod-4
 */
#define	HWRXOFF		30

#define d64txregs	dregs.d64_u.txregs_64
#define d64rxregs	dregs.d64_u.rxregs_64
#define txd64		dregs.d64_u.txd_64
#define rxd64		dregs.d64_u.rxd_64

#define	MAXNAMEL	8		/* 8 char names */

/* dma engine software state */
typedef struct dma_info {
	struct hnddma_pub hnddma;	/* exported structure, don't use hnddma_t,
					 * which could be const
					 */
	uint		*msg_level;	/* message level pointer */
	char		name[MAXNAMEL];	/* callers name for diag msgs */

	void		*osh;		/* os handle */
	si_t		*sih;		/* sb handle */

	bool		dma64;		/* this dma engine is operating in 64-bit mode */
	bool		addrext;	/* this dma engine supports DmaExtendedAddrChanges */

	union {
		struct {
			dma64regs_t	*txregs_64;	/* 64-bit dma tx engine registers */
			dma64regs_t	*rxregs_64;	/* 64-bit dma rx engine registers */
			dma64dd_t	*txd_64;	/* pointer to dma64 tx descriptor ring */
			dma64dd_t	*rxd_64;	/* pointer to dma64 rx descriptor ring */
		} d64_u;
	} dregs;

	uint16		dmadesc_align;	/* alignment requirement for dma descriptors */

	uint16		ntxd;		/* # tx descriptors tunable */
	uint16		txin;		/* index of next descriptor to reclaim */
	uint16		txout;		/* index of next descriptor to post */
	void		**txp;		/* pointer to parallel array of pointers to packets */
	osldma_t 	*tx_dmah;	/* DMA TX descriptor ring handle */
	hnddma_seg_map_t	*txp_dmah;	/* DMA MAP meta-data handle */
	dmaaddr_t	txdpa;		/* Aligned physical address of descriptor ring */
	dmaaddr_t	txdpaorig;	/* Original physical address of descriptor ring */
	uint16		txdalign;	/* #bytes added to alloc'd mem to align txd */
	uint32		txdalloc;	/* #bytes allocated for the ring */
	uint32		xmtptrbase;	/* When using unaligned descriptors, the ptr register
					 * is not just an index, it needs all 13 bits to be
					 * an offset from the addr register.
					 */

	uint16		nrxd;		/* # rx descriptors tunable */
	uint16		rxin;		/* index of next descriptor to reclaim */
	uint16		rxout;		/* index of next descriptor to post */
	void		**rxp;		/* pointer to parallel array of pointers to packets */
	osldma_t 	*rx_dmah;	/* DMA RX descriptor ring handle */
	hnddma_seg_map_t	*rxp_dmah;	/* DMA MAP meta-data handle */
	dmaaddr_t	rxdpa;		/* Aligned physical address of descriptor ring */
	dmaaddr_t	rxdpaorig;	/* Original physical address of descriptor ring */
	uint16		rxdalign;	/* #bytes added to alloc'd mem to align rxd */
	uint32		rxdalloc;	/* #bytes allocated for the ring */
	uint32		rcvptrbase;	/* Base for ptr reg when using unaligned descriptors */

	/* tunables */
	uint16		rxbufsize;	/* rx buffer size in bytes,
					 * not including the extra headroom
					 */
	uint		rxextrahdrroom;	/* extra rx headroom, reverseved to assist upper stack
					 *  e.g. some rx pkt buffers will be bridged to tx side
					 *  without byte copying. The extra headroom needs to be
					 *  large enough to fit txheader needs.
					 *  Some dongle driver may not need it.
					 */
	uint		nrxpost;	/* # rx buffers to keep posted */
	uint		rxoffset;	/* rxcontrol offset */
	uint		ddoffsetlow;	/* add to get dma address of descriptor ring, low 32 bits */
	uint		ddoffsethigh;	/*   high 32 bits */
	uint		dataoffsetlow;	/* add to get dma address of data buffer, low 32 bits */
	uint		dataoffsethigh;	/*   high 32 bits */
	bool		aligndesc_4k;	/* descriptor base need to be aligned or not */
	uint8		rxburstlen;	/* burstlen field for rx (for cores supporting burstlen) */
	uint8		txburstlen;	/* burstlen field for tx (for cores supporting burstlen) */
	uint		dma_avoidance_cnt;
} dma_info_t;


#endif	/* _eth_hw_dma_h_ */
