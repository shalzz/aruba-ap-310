/*-
 * Copyright (c) 2001 Atsushi Onoe
 * Copyright (c) 2002-2005 Sam Leffler, Errno Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/net80211/_ieee80211.h,v 1.2 2004/12/31 22:42:38 sam Exp $
 * $Id: //depot/sw/releases/linux_5.1/src/802_11/madwifi/madwifi/net80211/_ieee80211.h#1 $
 */
#ifndef _NET80211__IEEE80211_H_
#define _NET80211__IEEE80211_H_

#ifndef __packed
#define __packed        __attribute__((__packed__))
#endif

enum ieee80211_phytype {
	IEEE80211_T_DS,			/* direct sequence spread spectrum */
	IEEE80211_T_FH,			/* frequency hopping */
	IEEE80211_T_OFDM,		/* frequency division multiplexing */
	IEEE80211_T_TURBO,		/* high rate OFDM, aka turbo mode */
	IEEE80211_T_HT,			/* HT - full GI */
	IEEE80211_T_MAX
};
#define	IEEE80211_T_CCK	IEEE80211_T_DS	/* more common nomenclature */

/* XXX not really a mode; there are really multiple PHY's */
enum ieee80211_phymode {
	IEEE80211_MODE_AUTO	= 0,	/* autoselect */
	IEEE80211_MODE_11A	= 1,	/* 5GHz, OFDM */
	IEEE80211_MODE_11B	= 2,	/* 2GHz, CCK */
	IEEE80211_MODE_11G	= 3,	/* 2GHz, OFDM */
	IEEE80211_MODE_FH	= 4,	/* 2GHz, GFSK */
	IEEE80211_MODE_TURBO_A	= 5,	/* 5GHz, OFDM, 2x clock dynamic turbo */
	IEEE80211_MODE_TURBO_G	= 6,	/* 2GHz, OFDM, 2x clock  dynamic turbo*/
	IEEE80211_MODE_11NA_HT20 	= 7,    /* 5Ghz, HT20 */
	IEEE80211_MODE_11NG_HT20 	= 8,    /* 2Ghz, HT20 */
	IEEE80211_MODE_11NA_HT40PLUS 	= 9,    /* 5Ghz, HT40 (ext ch +1) */
	IEEE80211_MODE_11NA_HT40MINUS 	= 10,   /* 5Ghz, HT40 (ext ch -1) */
	IEEE80211_MODE_11NG_HT40PLUS 	= 11,   /* 2Ghz, HT40 (ext ch +1) */
	IEEE80211_MODE_11NG_HT40MINUS 	= 12,   /* 2Ghz, HT40 (ext ch -1) */
        IEEE80211_MODE_11NA_HT40= 13,   /* 5Ghz, HT40 (ext ch -1/+1) */
        IEEE80211_MODE_11NG_HT40= 14,   /* 2Ghz, HT40 (ext ch -1/+1) */
};
#ifdef __FAT_AP__
#define	IEEE80211_MODE_MAX	(IEEE80211_MODE_11NG_HT40 + 1)
#else
#define	IEEE80211_MODE_MAX	(IEEE80211_MODE_11NG_HT40MINUS + 1)
#endif

enum ieee80211_opmode {
	IEEE80211_M_STA		= 1,	/* infrastructure station */
	IEEE80211_M_IBSS 	= 0,	/* IBSS (adhoc) station */
	IEEE80211_M_AHDEMO	= 3,	/* Old lucent compatible adhoc demo */
	IEEE80211_M_HOSTAP	= 6,	/* Software Access Point */
	IEEE80211_M_MONITOR	= 8,	/* Monitor mode */
	IEEE80211_M_WDS		= 2,	/* WDS link */
        IEEE80211_M_TUNNEL      = 14,   /* Aruba tunnel mode */
        IEEE80211_M_MESHP       = 22,
        IEEE80211_M_MESHC       = 17,
	IEEE80211_M_SPECTRUM	= 16,	/* Monitor mode */
};


/*
 * 802.11n
 */
enum ieee80211_11nmode {
	IEEE80211_MIXN  	=  0,
	IEEE80211_PUREN		=  1
};

enum ieee80211_cwm_mode {
	IEEE80211_CWM_MODE20,
	IEEE80211_CWM_MODE2040,	  	
	IEEE80211_CWM_MODE40,
	IEEE80211_CWM_MODEMAX

};

enum ieee80211_cwm_extprotspacing {
	IEEE80211_CWM_EXTPROTSPACING20,
	IEEE80211_CWM_EXTPROTSPACING25,
	IEEE80211_CWM_EXTPROTSPACINGMAX
};

enum ieee80211_cwm_width {
	IEEE80211_CWM_WIDTH20,
	IEEE80211_CWM_WIDTH40
};

enum ieee80211_cwm_extprotmode {
	IEEE80211_CWM_EXTPROTNONE,	/* no protection */
	IEEE80211_CWM_EXTPROTCTSONLY,	/* CTS to self */
	IEEE80211_CWM_EXTPROTRTSCTS,	/* RTS-CTS */
	IEEE80211_CWM_EXTPROTMAX
};

/* CWM (Channel Width Management) Information */
struct ieee80211_cwm {

	/* Configuration */
	enum ieee80211_cwm_mode		cw_mode;	       	/* CWM mode */
	int8_t				cw_extoffset;	       	/* CWM Extension Channel Offset */
	enum ieee80211_cwm_extprotmode	cw_extprotmode;	       	/* CWM Extension Channel Protection Mode */
	enum ieee80211_cwm_extprotspacing cw_extprotspacing; 	/* CWM Extension Channel Protection Spacing */
	u_int32_t 			cw_enable; 		/* CWM State Machine Enabled */
	u_int32_t 			cw_extbusythreshold; 	/* CWM Extension Channel Busy Threshold */

	/* State */
        enum ieee80211_cwm_width 	cw_width;	       	/* CWM channel width */
};                                          


enum ieee80211_fixed_rate_mode {
	IEEE80211_FIXED_RATE_NONE  = 0,
	IEEE80211_FIXED_RATE_MCS   = 1	/* HT rates */
};

/* Holds the fixed rate information for each VAP */
  
struct ieee80211_fixed_rate {
	enum ieee80211_fixed_rate_mode	mode;	
	u_int32_t			series;
	u_int32_t			retries;
};

/*
 * 802.11g protection mode.
 */
enum ieee80211_protmode {
	IEEE80211_PROT_NONE	= 0,	/* no protection */
	IEEE80211_PROT_CTSONLY	= 1,	/* CTS to self */
	IEEE80211_PROT_RTSCTS	= 2,	/* RTS-CTS */
};

/*
 * Authentication mode.
 */
enum ieee80211_authmode {
	IEEE80211_AUTH_NONE	= 0,
	IEEE80211_AUTH_OPEN	= 1,		/* open */
	IEEE80211_AUTH_SHARED	= 2,		/* shared-key */
	IEEE80211_AUTH_8021X	= 3,		/* 802.1x */
	IEEE80211_AUTH_AUTO	= 4,		/* auto-select/accept */
	/* NB: these are used only for ioctls */
	IEEE80211_AUTH_WPA	= 5,		/* WPA/RSN w/ 802.1x/PSK */
};

/*
 * Roaming mode is effectively who controls the operation
 * of the 802.11 state machine when operating as a station.
 * State transitions are controlled either by the driver
 * (typically when management frames are processed by the
 * hardware/firmware), the host (auto/normal operation of
 * the 802.11 layer), or explicitly through ioctl requests
 * when applications like wpa_supplicant want control.
 */
enum ieee80211_roamingmode {
	IEEE80211_ROAMING_DEVICE= 0,	/* driver/hardware control */
	IEEE80211_ROAMING_AUTO	= 1,	/* 802.11 layer control */
	IEEE80211_ROAMING_MANUAL= 2,	/* application control */
};

/*
 * Scanning mode controls station scanning work; this is
 * used only when roaming mode permits the host to select
 * the bss to join/channel to use.
 */
enum ieee80211_scanmode {
	IEEE80211_SCAN_DEVICE	= 0,	/* driver/hardware control */
	IEEE80211_SCAN_BEST	= 1,	/* 802.11 layer selects best */
	IEEE80211_SCAN_FIRST	= 2,	/* take first suitable candidate */
};

/*
 * Channels are specified by frequency and attributes.
 */
struct ieee80211_channel {
	u_int16_t	ic_freq;	/* setting in Mhz */
	u_int32_t	ic_flags;	/* see below */
	u_int8_t	ic_ieee;	/* IEEE channel number */
        u_int8_t        ic_flagext;     /* see below */
	int8_t		ic_maxregpower;	/* maximum regulatory tx power in dBm */
	int8_t		ic_twicemaxAPpower;	/* maximum AP tx power capability in 0.5 dBm */
	int8_t		ic_maxpower;	/* maximum tx power in dBm */
	int8_t		ic_minpower;	/* minimum tx power in dBm */
	u_int16_t	ic_antennagain; /* antenna gain allowed for this channel */
	u_int16_t	ic_ext_flags;	/* ext flags */
	u_int8_t	ic_regclassid;	/* regulatory class id */
};

#ifdef CONFIG_NEWNDRIVER
#define	IEEE80211_CHAN_MAX	851
#define	IEEE80211_CHAN_BYTES	107	/* howmany(IEEE80211_CHAN_MAX, NBBY) */
#else
#define	IEEE80211_CHAN_MAX	255
#define	IEEE80211_CHAN_BYTES	32	/* howmany(IEEE80211_CHAN_MAX, NBBY) */
#endif
#define	IEEE80211_CHAN_ANY	0xffff	/* token for ``any channel'' */
#define	IEEE80211_CHAN_ANYC \
	((struct ieee80211_channel *) IEEE80211_CHAN_ANY)

#define	IEEE80211_RADAR_11HCOUNT	5
#define	IEEE80211_RADAR_TEST_MUTE_CHAN	36 /* Move to channel 36 for mute test */

/* bits 0-3 are for private use by drivers */
/* channel attributes */
#define IEEE80211_CHAN_RADAR	0x0001	/* Radar found on channel */
#define	IEEE80211_CHAN_TURBO	0x0010	/* Turbo channel */
#define	IEEE80211_CHAN_CCK	0x0020	/* CCK channel */
#define	IEEE80211_CHAN_OFDM	0x0040	/* OFDM channel */
#define	IEEE80211_CHAN_2GHZ	0x0080	/* 2 GHz spectrum channel. */
#define	IEEE80211_CHAN_5GHZ	0x0100	/* 5 GHz spectrum channel */
#define	IEEE80211_CHAN_PASSIVE	0x0200	/* Only passive scan allowed */
#define	IEEE80211_CHAN_DYN	0x0400	/* Dynamic CCK-OFDM channel */
#define	IEEE80211_CHAN_GFSK	0x0800	/* GFSK channel (FHSS PHY) */
#define IEEE80211_CHAN_DFS	0x1000	/* Radar found on channel */
#define	IEEE80211_CHAN_STURBO	0x2000	/* 11a static turbo channel only */
#define	IEEE80211_CHAN_HALF	0x4000	/* Half rate channel */
#define	IEEE80211_CHAN_QUARTER	0x8000	/* Quarter rate channel */
#define	IEEE80211_CHAN_HT20	0x10000
#define IEEE80211_CHAN_HT40PLUS  0x20000        /* HT 40 with extension channel above */
#define IEEE80211_CHAN_HT40MINUS 0x40000        /* HT 40 with extension channel below */

/* flagext */
#define IEEE80211_CHAN_DFS_CLEAR        0x0008  /* if channel has been checked for DFS */
#define IEEE80211_CHAN_11D_EXCLUDED 0x0010      /* excluded in 11D */
#define IEEE80211_CHAN_DISALLOW_ADHOC   0x0040  /* ad-hoc is not allowed */

/*
 * Useful combinations of channel characteristics.
 */
#define	IEEE80211_CHAN_FHSS \
	(IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_GFSK)
#define	IEEE80211_CHAN_A \
	(IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_OFDM)
#define	IEEE80211_CHAN_B \
	(IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_CCK)
#define	IEEE80211_CHAN_PUREG \
	(IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_OFDM)
#define	IEEE80211_CHAN_G \
	(IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_DYN)
#define IEEE80211_CHAN_108A \
	(IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_OFDM | IEEE80211_CHAN_TURBO)
#define	IEEE80211_CHAN_108G \
	(IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_OFDM | IEEE80211_CHAN_TURBO)
#define	IEEE80211_CHAN_ST \
	(IEEE80211_CHAN_108A | IEEE80211_CHAN_STURBO)
#define IEEE80211_IS_CHAN_11D_EXCLUDED(_c) \
        (((_c)->ic_flagext & IEEE80211_CHAN_11D_EXCLUDED) != 0)

#define IEEE80211_CHAN_11NG_HT20 \
        (IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_HT20)
#define IEEE80211_CHAN_11NA_HT20 \
        (IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_HT20)
#define IEEE80211_CHAN_11NG_HT40PLUS \
        (IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_HT40PLUS)
#define IEEE80211_CHAN_11NG_HT40MINUS \
        (IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_HT40MINUS)
#define IEEE80211_CHAN_11NA_HT40PLUS \
        (IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_HT40PLUS)
#define IEEE80211_CHAN_11NA_HT40MINUS \
        (IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_HT40MINUS)

#define	IEEE80211_CHAN_ALL \
	(IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_HT20 | IEEE80211_CHAN_GFSK | \
	 IEEE80211_CHAN_CCK | IEEE80211_CHAN_OFDM | IEEE80211_CHAN_DYN | IEEE80211_CHAN_HT40PLUS |IEEE80211_CHAN_HT40MINUS)

#define	IEEE80211_CHAN_ALLTURBO \
	(IEEE80211_CHAN_ALL | IEEE80211_CHAN_TURBO | IEEE80211_CHAN_STURBO)

#define	IEEE80211_IS_CHAN_FHSS(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_FHSS) == IEEE80211_CHAN_FHSS)
#define	IEEE80211_IS_CHAN_A(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_A) == IEEE80211_CHAN_A)
#define	IEEE80211_IS_CHAN_B(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_B) == IEEE80211_CHAN_B)
#define	IEEE80211_IS_CHAN_PUREG(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_PUREG) == IEEE80211_CHAN_PUREG)
#define	IEEE80211_IS_CHAN_G(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_G) == IEEE80211_CHAN_G)
#define	IEEE80211_IS_CHAN_ANYG(_c) \
	(IEEE80211_IS_CHAN_PUREG(_c) || IEEE80211_IS_CHAN_G(_c))
#define	IEEE80211_IS_CHAN_ST(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_ST) == IEEE80211_CHAN_ST)
#define	IEEE80211_IS_CHAN_108A(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_108A) == IEEE80211_CHAN_108A)
#define	IEEE80211_IS_CHAN_108G(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_108G) == IEEE80211_CHAN_108G)

#define	IEEE80211_IS_CHAN_2GHZ(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_2GHZ) != 0)
#define	IEEE80211_IS_CHAN_5GHZ(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_5GHZ) != 0)
#define	IEEE80211_IS_CHAN_OFDM(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_OFDM) != 0)
#define	IEEE80211_IS_CHAN_CCK(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_CCK) != 0)
#define	IEEE80211_IS_CHAN_GFSK(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_GFSK) != 0)
#define	IEEE80211_IS_CHAN_TURBO(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_TURBO) != 0)
#define	IEEE80211_IS_CHAN_STURBO(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_STURBO) != 0)
#define	IEEE80211_IS_CHAN_DTURBO(_c) \
	(((_c)->ic_flags & \
	(IEEE80211_CHAN_TURBO | IEEE80211_CHAN_STURBO)) == IEEE80211_CHAN_TURBO)
#define	IEEE80211_IS_CHAN_HALF(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_HALF) != 0)
#define	IEEE80211_IS_CHAN_QUARTER(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_QUARTER) != 0)
#define	IEEE80211_IS_CHAN_RADAR(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_RADAR) != 0)
#define	IEEE80211_IS_CHAN_11NG_HT20(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_11NG_HT20) == IEEE80211_CHAN_11NG_HT20)
#define	IEEE80211_IS_CHAN_11NA_HT20(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_11NA_HT20) == IEEE80211_CHAN_11NA_HT20)
#define	IEEE80211_IS_CHAN_11NG_HT40PLUS(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_11NG_HT40PLUS) == IEEE80211_CHAN_11NG_HT40PLUS)
#define	IEEE80211_IS_CHAN_11NG_HT40MINUS(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_11NG_HT40MINUS) == IEEE80211_CHAN_11NG_HT40MINUS)
#define	IEEE80211_IS_CHAN_11NA_HT40PLUS(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_11NA_HT40PLUS) == IEEE80211_CHAN_11NA_HT40PLUS)
#define	IEEE80211_IS_CHAN_11NA_HT40MINUS(_c) \
	(((_c)->ic_flags & IEEE80211_CHAN_11NA_HT40MINUS) == IEEE80211_CHAN_11NA_HT40MINUS)

#define	IEEE80211_IS_CHAN_11N(_c) \
	(((_c)->ic_flags & (IEEE80211_CHAN_HT20 | IEEE80211_CHAN_HT40PLUS | IEEE80211_CHAN_HT40MINUS)) != 0)
#define	IEEE80211_IS_CHAN_11N_HT20(_c) \
	(((_c)->ic_flags & (IEEE80211_CHAN_HT20)) != 0)
#define	IEEE80211_IS_CHAN_11N_HT40(_c) \
	(((_c)->ic_flags & (IEEE80211_CHAN_HT40PLUS | IEEE80211_CHAN_HT40MINUS)) != 0)
#define	IEEE80211_IS_CHAN_11NG(_c) \
	(IEEE80211_IS_CHAN_2GHZ((_c)) && IEEE80211_IS_CHAN_11N((_c)))
#define	IEEE80211_IS_CHAN_11NA(_c) \
	(IEEE80211_IS_CHAN_5GHZ((_c)) && IEEE80211_IS_CHAN_11N((_c)))

#define IEEE80211_IS_CHAN_HT20_CAPABLE(_c) \
     ( ((_c)->ic_flags & IEEE80211_CHAN_HT20) == IEEE80211_CHAN_HT20 )
#define IEEE80211_IS_CHAN_HT40PLUS_CAPABLE(_c) \
     ( ((_c)->ic_flags & IEEE80211_CHAN_HT40PLUS) == IEEE80211_CHAN_HT40PLUS )
#define IEEE80211_IS_CHAN_HT40MINUS_CAPABLE(_c) \
     ( ((_c)->ic_flags & IEEE80211_CHAN_HT40MINUS) == IEEE80211_CHAN_HT40MINUS )
#define IEEE80211_IS_CHAN_HT40_CAPABLE(_c) \
     ( IEEE80211_IS_CHAN_HT40PLUS_CAPABLE(_c) || IEEE80211_IS_CHAN_HT40MINUS_CAPABLE(_c) )
#define IEEE80211_IS_CHAN_HT_CAPABLE(_c) \
     ( IEEE80211_IS_CHAN_HT20_CAPABLE(_c) || IEEE80211_IS_CHAN_HT40_CAPABLE(_c) )
#define IEEE80211_IS_CHAN_11N_CTL_CAPABLE(_c)  IEEE80211_IS_CHAN_HT20_CAPABLE(_c)
#define IEEE80211_IS_CHAN_11N_CTL_U_CAPABLE(_c) \
     ( ((_c)->ic_flags & IEEE80211_CHAN_HT40PLUS) == IEEE80211_CHAN_HT40PLUS )
#define IEEE80211_IS_CHAN_11N_CTL_L_CAPABLE(_c) \
     ( ((_c)->ic_flags & IEEE80211_CHAN_HT40MINUS) == IEEE80211_CHAN_HT40MINUS )
#define IEEE80211_IS_CHAN_11N_CTL_40_CAPABLE(_c) \
     ( IEEE80211_IS_CHAN_11N_CTL_U_CAPABLE((_c)) || IEEE80211_IS_CHAN_11N_CTL_L_CAPABLE((_c)) )
#define IEEE80211_IS_CHAN_DFS_REQUIRED(_c) \
     ( ((_c)->ic_flags & IEEE80211_CHAN_DFS) == IEEE80211_CHAN_DFS )
#define IEEE80211_GET_BCAST_ADDR(_c)  ((_c)->ic_dev->broadcast)

#define ARUBA_CHAN_IS11A(_c) \
	(IEEE80211_IS_CHAN_11NA((_c)) || IEEE80211_IS_CHAN_A((_c)))

/* ni_chan encoding for FH phy */
#define	IEEE80211_FH_CHANMOD	80
#define	IEEE80211_FH_CHAN(set,pat)	(((set)-1)*IEEE80211_FH_CHANMOD+(pat))
#define	IEEE80211_FH_CHANSET(chan)	((chan)/IEEE80211_FH_CHANMOD+1)
#define	IEEE80211_FH_CHANPAT(chan)	((chan)%IEEE80211_FH_CHANMOD)

/*
 * 802.11 rate set.
 */
#define	IEEE80211_RATE_SIZE	8		/* 802.11 standard */
#ifdef CONFIG_NEWNDRIVER
#define	IEEE80211_RATE_MAXSIZE	44		/* max rates we'll handle */
#else
#define	IEEE80211_RATE_MAXSIZE	15		/* max rates we'll handle */
#endif
#define	IEEE80211_HT_RATE_SIZE	128

/* For legacy hardware - leaving it as is for now */

#define IEEE80211_RATE_MCS	0x8000
#define IEEE80211_RATE_MCS_VAL	0x7FFF

//#define IEEE80211_RATE_IDX_ENTRY(val, idx) (val&(0xff<<(8*idx))>>(idx*8))
#define IEEE80211_RATE_IDX_ENTRY(val, idx) (((val&(0xff<<(idx*8)))>>(idx*8)))
/*
 * 11n A-MPDU & A-MSDU limits
 */
#define IEEE80211_AMPDU_LIMIT_MIN           (1 * 1024)
#define IEEE80211_AMPDU_LIMIT_MAX           (64 * 1024 - 1)
#define IEEE80211_AMPDU_LIMIT_DEFAULT       IEEE80211_AMPDU_LIMIT_MAX
#define IEEE80211_AMPDU_SUBFRAME_MIN        2 
#define IEEE80211_AMPDU_SUBFRAME_MAX        64 
#define IEEE80211_AMPDU_SUBFRAME_DEFAULT    64 
#define IEEE80211_AMSDU_LIMIT_MAX           4096
#define IEEE80211_RIFS_AGGR_DIV             10

struct ieee80211_rateset {
	u_int8_t		rs_nrates;
	u_int8_t		rs_rates[IEEE80211_RATE_MAXSIZE];
};

struct ieee80211_roam {
	int8_t			rssi11a;	/* rssi thresh for 11a bss */
	int8_t			rssi11b;	/* for 11g sta in 11b bss */
	int8_t			rssi11bOnly;	/* for 11b sta */
	u_int8_t		pad1;
	u_int8_t		rate11a;	/* rate thresh for 11a bss */
	u_int8_t		rate11b;	/* for 11g sta in 11b bss */
	u_int8_t		rate11bOnly;	/* for 11b sta */
	u_int8_t		pad2;
};
#define IEEE80211_MMIC_LEN 8
struct ieee80211_mmie {
    u_int8_t        eid;
    u_int8_t        len;
    u_int16_t       kid;
    u_int8_t        ipn[6];
    u_int8_t        mic[IEEE80211_MMIC_LEN];
} __packed;
#endif /* _NET80211__IEEE80211_H_ */
