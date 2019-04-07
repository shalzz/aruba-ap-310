/*
 * Broadcom Gigabit Ethernet MAC defines.
 *
 * $Copyright Open Broadcom Corporation$
 * $Id: etcgmac.h 267700 2011-06-19 15:41:07Z sudhirbs $
 */
#ifndef _eth_hw_data_h_
#define _eth_hw_data_h_

#include <asm/arch/iproc_gmac_regs.h>
#include <asm/arch/ethHw_dma.h>
#include <stdbool.h>

#define SUCCESS			0
#define FAILURE			-1

#define TRUE	true
#define FALSE	false

#define ETH_ADDR_LEN	6

#define PHY_ADDR	30

#define NUMRXQ		1	/* gmac has one rx queue */
#define RX_Q0		0	/* receive DMA queue */

#define NUMTXQ		4
#define TC_BK		0	/* background traffic class */
#define TC_BE		1	/* best effort traffic class */
#define TC_CL		2	/* controlled load traffic class */
#define TC_VO		3	/* voice traffic class */
#define TC_NONE		-1	/* traffic class none */
#define TX_Q0		TC_BK	/* DMA txq 0 */
#define TX_Q1		TC_BE	/* DMA txq 1 */
#define TX_Q2		TC_CL	/* DMA txq 2 */
#define TX_Q3		TC_VO	/* DMA txq 3 */

/* init options */
#define ET_INIT_FULL     0x1
#define ET_INIT_INTRON   0x2

/* forcespeed values */
#define	ET_AUTO		-1
#define	ET_10HALF	0
#define	ET_10FULL	1
#define	ET_100HALF	2
#define	ET_100FULL	3
#define	ET_1000HALF	4
#define	ET_1000FULL	5

/* XXX-NS, where is good place to put MAX_GMAC_CORES ? */
#define MAX_GMAC_CORES	3

/* private chip state */
typedef struct bcmgmac_s {
	gmacregs_t	*regs;		/* pointer to chip registers */
	osl_t 		*osh;		/* os handle */
	void		*robo;		/* optional robo private data */

	void 		*etphy;		/* pointer to et for shared mdc/mdio contortion */

	uint32		intstatus;	/* saved interrupt condition bits */
	uint32		intmask;	/* current software interrupt mask */
	uint32		def_intmask;	/* default interrupt mask */

	dma_info_t	*di[NUMTXQ];	/* dma engine software state */

	bool		mibgood;	/* true once mib registers have been cleared */
	gmacmib_t	mib;		/* mib statistic counters */
	si_t 		*sih;		/* si utils handle */
/* XXX-NS */
	si_t 		*ext_sih[MAX_GMAC_CORES];		/* si utils handle */

	char		*vars;		/* sprom name=value */
	uint		vars_size;
	uint		phyaddr;	/* sb chips: mdio 5-bit phy address */
#ifdef CONFIG_APBOOT
	uint		phyaddr_port;	/* sb chips: mdio 5-bit phy address */
#endif
} bcmgmac_t;

#define ch_t	bcmgmac_t

/* private chip state */
typedef struct _bcm_eth_s {
	uint32_t		unit;		/* unit number */
	uint32_t		mac;		/* mac number */
	unsigned char	enetaddr[ETH_ADDR_LEN];
	bool			loopback;
	uint32_t		dmactrlflags;
	uint32_t		rxoffset;
	gmacregs_t		*regs;		/* pointer to chip registers */
	bcmgmac_t		bcmgmac;
} bcm_eth_t;

#define GMAC_RESET_DELAY 	2

#endif /* _eth_hw_data_h_ */
