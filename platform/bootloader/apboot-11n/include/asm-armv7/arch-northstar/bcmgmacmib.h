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
 * Hardware-specific MIB definition for
 * Broadcom Home Networking Division
 * GbE Unimac core
 */

#ifndef	_bcmgmacmib_h_
#define	_bcmgmacmib_h_


/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	/* PAD */

/* GMAC MIB structure */

typedef struct _gmacmib {
	uint32_t	tx_good_octets;			/* 0x300 */
	uint32_t	tx_good_octets_high;	/* 0x304 */
	uint32_t	tx_good_pkts;			/* 0x308 */
	uint32_t	tx_octets;				/* 0x30c */
	uint32_t	tx_octets_high;			/* 0x310 */
	uint32_t	tx_pkts;				/* 0x314 */
	uint32_t	tx_broadcast_pkts;		/* 0x318 */
	uint32_t	tx_multicast_pkts;		/* 0x31c */
	uint32_t	tx_len_64;				/* 0x320 */
	uint32_t	tx_len_65_to_127;		/* 0x324 */
	uint32_t	tx_len_128_to_255;		/* 0x328 */
	uint32_t	tx_len_256_to_511;		/* 0x32c */
	uint32_t	tx_len_512_to_1023;		/* 0x330 */
	uint32_t	tx_len_1024_to_1522;	/* 0x334 */
	uint32_t	tx_len_1523_to_2047;	/* 0x338 */
	uint32_t	tx_len_2048_to_4095;	/* 0x33c */
	uint32_t	tx_len_4095_to_8191;	/* 0x340 */
	uint32_t	tx_len_8192_to_max;		/* 0x344 */
	uint32_t	tx_jabber_pkts;			/* 0x348 */
	uint32_t	tx_oversize_pkts;		/* 0x34c */
	uint32_t	tx_fragment_pkts;		/* 0x350 */
	uint32_t	tx_underruns;			/* 0x354 */
	uint32_t	tx_total_cols;			/* 0x358 */
	uint32_t	tx_single_cols;			/* 0x35c */
	uint32_t	tx_multiple_cols;		/* 0x360 */
	uint32_t	tx_excessive_cols;		/* 0x364 */
	uint32_t	tx_late_cols;			/* 0x368 */
	uint32_t	tx_defered;				/* 0x36c */
	uint32_t	tx_carrier_lost;		/* 0x370 */
	uint32_t	tx_pause_pkts;			/* 0x374 */
	uint32_t	tx_uni_pkts;			/* 0x378 */
	uint32_t	tx_q0_pkts;				/* 0x37c */
	uint32_t	tx_q0_octets;			/* 0x380 */
	uint32_t	tx_q0_octets_high;		/* 0x384 */
	uint32_t	tx_q1_pkts;				/* 0x388 */
	uint32_t	tx_q1_octets;			/* 0x38c */
	uint32_t	tx_q1_octets_high;		/* 0x390 */
	uint32_t	tx_q2_pkts;				/* 0x394 */
	uint32_t	tx_q2_octets;			/* 0x398 */
	uint32_t	tx_q2_octets_high;		/* 0x39c */
	uint32_t	tx_q3_pkts;				/* 0x3a0 */
	uint32_t	tx_q3_octets;			/* 0x3a4 */
	uint32_t	tx_q3_octets_high;		/* 0x3a8 */
	uint32_t	PAD;					/* 0x3ac */
	uint32_t	rx_good_octets;			/* 0x3b0 */
	uint32_t	rx_good_octets_high;	/* 0x3b4 */
	uint32_t	rx_good_pkts;			/* 0x3b8 */
	uint32_t	rx_octets;				/* 0x3bc */
	uint32_t	rx_octets_high;			/* 0x3c0 */
	uint32_t	rx_pkts;				/* 0x3c4 */
	uint32_t	rx_broadcast_pkts;		/* 0x3c8 */
	uint32_t	rx_multicast_pkts;		/* 0x3cc */
	uint32_t	rx_len_64;				/* 0x3d0 */
	uint32_t	rx_len_65_to_127;		/* 0x3d4 */
	uint32_t	rx_len_128_to_255;		/* 0x3d8 */
	uint32_t	rx_len_256_to_511;		/* 0x3dc */
	uint32_t	rx_len_512_to_1023;		/* 0x3e0 */
	uint32_t	rx_len_1024_to_1522;	/* 0x3e4 */
	uint32_t	rx_len_1523_to_2047;	/* 0x3e8 */
	uint32_t	rx_len_2048_to_4095;	/* 0x3ec */
	uint32_t	rx_len_4095_to_8191;	/* 0x3f0 */
	uint32_t	rx_len_8192_to_max;		/* 0x3f4 */
	uint32_t	rx_jabber_pkts;			/* 0x3f8 */
	uint32_t	rx_oversize_pkts;		/* 0x3fc */
	uint32_t	rx_fragment_pkts;		/* 0x400 */
	uint32_t	rx_missed_pkts;			/* 0x404 */
	uint32_t	rx_crc_align_errs;		/* 0x408 */
	uint32_t	rx_undersize;			/* 0x40c */
	uint32_t	rx_crc_errs;			/* 0x410 */
	uint32_t	rx_align_errs;			/* 0x414 */
	uint32_t	rx_symbol_errs;			/* 0x418 */
	uint32_t	rx_pause_pkts;			/* 0x41c */
	uint32_t	rx_nonpause_pkts;		/* 0x420 */
	uint32_t	rx_sachanges;			/* 0x424 */
	uint32_t	rx_uni_pkts;			/* 0x428 */
} gmacmib_t;

#define	GM_MIB_BASE		0x300
#define	GM_MIB_LIMIT	0x800

#endif	/* _bcmgmacmib_h_ */
