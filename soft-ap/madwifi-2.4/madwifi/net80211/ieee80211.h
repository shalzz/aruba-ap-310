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
 * $FreeBSD: src/sys/net80211/ieee80211.h,v 1.8 2004/12/31 22:44:26 sam Exp $
 */
#ifndef _NET80211_IEEE80211_H_
#define _NET80211_IEEE80211_H_

#ifndef __packed
#define __packed        __attribute__((__packed__))
#endif

/*
 * 802.11 protocol definitions.
 */

#define	IEEE80211_ADDR_LEN	6		/* size of 802.11 address */
/* is 802.11 address multicast/broadcast? */
#define	IEEE80211_IS_MULTICAST(_a)	(*(_a) & 0x01)

/* IEEE 802.11 PLCP header */
struct ieee80211_plcp_hdr {
	u_int16_t	i_sfd;
	u_int8_t	i_signal;
	u_int8_t	i_service;
	u_int16_t	i_length;
	u_int16_t	i_crc;
} __attribute__ ((packed));

#define IEEE80211_PLCP_SFD      0xF3A0 
#define IEEE80211_PLCP_SERVICE  0x00

/* add 4 bytes rx_channel info into packet which send to meshd */
#define MESHD_RXCHAN_LEN   		4



/*
 * generic definitions for IEEE 802.11 frames
 */
struct ieee80211_frame {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_addr1[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr2[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr3[IEEE80211_ADDR_LEN];
	u_int8_t	i_seq[2];
	/* possibly followed by addr4[IEEE80211_ADDR_LEN]; */
	/* see below */
} __attribute__ ((packed));

struct ieee80211_qosframe {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_addr1[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr2[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr3[IEEE80211_ADDR_LEN];
	u_int8_t	i_seq[2];
	u_int8_t	i_qos[2];
	/* possibly followed by addr4[IEEE80211_ADDR_LEN]; */
	/* see below */
} __attribute__ ((packed));

struct ieee80211_qoscntl {
	u_int8_t	i_qos[2];
};

struct ieee80211_frame_addr4 {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_addr1[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr2[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr3[IEEE80211_ADDR_LEN];
	u_int8_t	i_seq[2];
	u_int8_t	i_addr4[IEEE80211_ADDR_LEN];
} __attribute__ ((packed));


struct ieee80211_qosframe_addr4 {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_addr1[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr2[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr3[IEEE80211_ADDR_LEN];
	u_int8_t	i_seq[2];
	u_int8_t	i_addr4[IEEE80211_ADDR_LEN];
	u_int8_t	i_qos[2];
} __attribute__ ((packed));

struct ieee80211_qosframe_addr4_wds_war {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_addr1[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr2[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr3[IEEE80211_ADDR_LEN];
	u_int8_t	i_seq[2];
	u_int8_t	i_addr4[IEEE80211_ADDR_LEN];
	u_int8_t	i_qos[2];
	u_int8_t	i_wdswar[2];
} __attribute__ ((packed));

struct ieee80211_ctlframe_addr2 {
	u_int8_t	i_fc[2];
	u_int8_t	i_aidordur[2]; /* AID or duration */
	u_int8_t	i_addr1[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr2[IEEE80211_ADDR_LEN];
} __attribute__ ((packed));

/* HTC frame for TxBF*/
// for TxBF RC
struct ieee80211_frame_min_one {
    u_int8_t    i_fc[2];
    u_int8_t    i_dur[2];
    u_int8_t    i_addr1[IEEE80211_ADDR_LEN];

} __packed;// For TxBF RC

struct ieee80211_qosframe_htc {
    u_int8_t    i_fc[2];
    u_int8_t    i_dur[2];
    u_int8_t    i_addr1[IEEE80211_ADDR_LEN];
    u_int8_t    i_addr2[IEEE80211_ADDR_LEN];
    u_int8_t    i_addr3[IEEE80211_ADDR_LEN];
    u_int8_t    i_seq[2];
    u_int8_t    i_qos[2];
    u_int8_t    i_htc[4];
    /* possibly followed by addr4[IEEE80211_ADDR_LEN]; */
    /* see below */
} __packed;
struct ieee80211_qosframe_htc_addr4 {
    u_int8_t    i_fc[2];
    u_int8_t    i_dur[2];
    u_int8_t    i_addr1[IEEE80211_ADDR_LEN];
    u_int8_t    i_addr2[IEEE80211_ADDR_LEN];
    u_int8_t    i_addr3[IEEE80211_ADDR_LEN];
    u_int8_t    i_seq[2];
    u_int8_t    i_addr4[IEEE80211_ADDR_LEN];
    u_int8_t    i_qos[2];
    u_int8_t    i_htc[4];
} __packed;
struct ieee80211_htc {
    u_int8_t    i_htc[4];
};
/*HTC frame for TxBF*/

#define	IEEE80211_WHQ(wh)		((struct ieee80211_qosframe *)(wh))
#define	IEEE80211_WH4(wh)		((struct ieee80211_frame_addr4 *)(wh))
#define	IEEE80211_WHQ4(wh)		((struct ieee80211_qosframe_addr4 *)(wh))

#define	IEEE80211_FC0_VERSION_MASK		0x03
#define	IEEE80211_FC0_VERSION_SHIFT		0
#define	IEEE80211_FC0_VERSION_0			0x00
#define	IEEE80211_FC0_TYPE_MASK			0x0c
#define	IEEE80211_FC0_TYPE_SHIFT		2
#define	IEEE80211_FC0_TYPE_MGT			0x00
#define	IEEE80211_FC0_TYPE_CTL			0x04
#define	IEEE80211_FC0_TYPE_DATA			0x08

#define	IEEE80211_FC0_SUBTYPE_MASK		0xf0
#define	IEEE80211_FC0_SUBTYPE_SHIFT		4
/* for TYPE_MGT */
#define	IEEE80211_FC0_SUBTYPE_ASSOC_REQ		0x00
#define	IEEE80211_FC0_SUBTYPE_ASSOC_RESP	0x10
#define	IEEE80211_FC0_SUBTYPE_REASSOC_REQ	0x20
#define	IEEE80211_FC0_SUBTYPE_REASSOC_RESP	0x30
#define	IEEE80211_FC0_SUBTYPE_PROBE_REQ		0x40
#define	IEEE80211_FC0_SUBTYPE_PROBE_RESP	0x50
#define	IEEE80211_FC0_SUBTYPE_BEACON		0x80
#define	IEEE80211_FC0_SUBTYPE_ATIM		0x90
#define	IEEE80211_FC0_SUBTYPE_DISASSOC		0xa0
#define	IEEE80211_FC0_SUBTYPE_AUTH		0xb0
#define	IEEE80211_FC0_SUBTYPE_DEAUTH		0xc0
#define IEEE80211_FC0_SUBTYPE_ACTION		0xd0
#define IEEE80211_FCO_SUBTYPE_ACTION_NO_ACK 0xe0
/* for TYPE_CTL */
#define	IEEE80211_FC0_SUBTYPE_BAR		0x80
#ifdef __FAT_AP__
#define	IEEE80211_FC0_SUBTYPE_BA		0x90
#endif
#define	IEEE80211_FC0_SUBTYPE_PS_POLL		0xa0
#define	IEEE80211_FC0_SUBTYPE_RTS		0xb0
#define	IEEE80211_FC0_SUBTYPE_CTS		0xc0
#define	IEEE80211_FC0_SUBTYPE_ACK		0xd0
#define	IEEE80211_FC0_SUBTYPE_CF_END		0xe0
#define	IEEE80211_FC0_SUBTYPE_CF_END_ACK	0xf0
/* for TYPE_DATA (bit combination) */
#define	IEEE80211_FC0_SUBTYPE_DATA		0x00
#define	IEEE80211_FC0_SUBTYPE_CF_ACK		0x10
#define	IEEE80211_FC0_SUBTYPE_CF_POLL		0x20
#define	IEEE80211_FC0_SUBTYPE_CF_ACPL		0x30
#define	IEEE80211_FC0_SUBTYPE_NODATA		0x40
#define	IEEE80211_FC0_SUBTYPE_CFACK		0x50
#define	IEEE80211_FC0_SUBTYPE_CFPOLL		0x60
#define	IEEE80211_FC0_SUBTYPE_CF_ACK_CF_ACK	0x70
#define	IEEE80211_FC0_SUBTYPE_QOS		0x80
#define	IEEE80211_FC0_SUBTYPE_QOS_NULL		0xc0

#define	IEEE80211_FC1_DIR_MASK			0x03
#define	IEEE80211_FC1_DIR_NODS			0x00	/* STA->STA */
#define	IEEE80211_FC1_DIR_TODS			0x01	/* STA->AP  */
#define	IEEE80211_FC1_DIR_FROMDS		0x02	/* AP ->STA */
#define	IEEE80211_FC1_DIR_DSTODS		0x03	/* AP ->AP  */

#define	IEEE80211_FC1_MORE_FRAG			0x04
#define	IEEE80211_FC1_RETRY			0x08
#define	IEEE80211_FC1_PWR_MGT			0x10
#define	IEEE80211_FC1_MORE_DATA			0x20
#define	IEEE80211_FC1_WEP			0x40
#define	IEEE80211_FC1_ORDER			0x80

#define	IEEE80211_SEQ_FRAG_MASK			0x000f
#define	IEEE80211_SEQ_FRAG_SHIFT		0
#define	IEEE80211_SEQ_SEQ_MASK			0xfff0
#define	IEEE80211_SEQ_SEQ_SHIFT			4
#define IEEE80211_SEQ_MAX               	4096

#define	IEEE80211_SEQ_LEQ(a,b)	((int)((a)-(b)) <= 0)

#define	IEEE80211_NWID_LEN			32

#define	IEEE80211_QOS_TXOP			0x00ff
/* bit 8 is reserved */

#define IEEE80211_QOS_AMSDU                     0x80
#define IEEE80211_QOS_AMSDU_S                   7
#define	IEEE80211_QOS_ACKPOLICY			0x60
#define	IEEE80211_QOS_ACKPOLICY_S		5
#define	IEEE80211_QOS_EOSP			0x10
#define	IEEE80211_QOS_EOSP_S			4
#define	IEEE80211_QOS_TID			0x0f

#define IEEE80211_HTC0_TRQ                  0x02
#define	IEEE80211_HTC2_CalPos               0x03
#define	IEEE80211_HTC2_CalSeq               0x0C
#define	IEEE80211_HTC2_CSI_NONCOMP_BF       0x80
#define	IEEE80211_HTC2_CSI_COMP_BF          0xc0

#define IEEE80211_IS_BEACON(_frame)    ((((_frame)->i_fc[0] & IEEE80211_FC0_TYPE_MASK) == IEEE80211_FC0_TYPE_MGT) && \
                                        (((_frame)->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK) == IEEE80211_FC0_SUBTYPE_BEACON))

#define IEEE80211_IS_DATA(_frame)      (((_frame)->i_fc[0] & IEEE80211_FC0_TYPE_MASK) == IEEE80211_FC0_TYPE_DATA)

#define IEEE80211_IS_QOS_FRAME(_frame)    ((((_frame)->i_fc[0] & IEEE80211_FC0_TYPE_MASK) == IEEE80211_FC0_TYPE_DATA) && \
                                           ((_frame)->i_fc[0] & IEEE80211_FC0_SUBTYPE_QOS))

/*
 * Country/Region Codes from MS WINNLS.H
 * Numbering from ISO 3166
 * XXX belongs elsewhere
 */
enum CountryCode {
    CTRY_ALBANIA              = 8,       /* Albania */
    CTRY_ALGERIA              = 12,      /* Algeria */
    CTRY_ARGENTINA            = 32,      /* Argentina */
    CTRY_ARMENIA              = 51,      /* Armenia */
    CTRY_AUSTRALIA            = 36,      /* Australia */
    CTRY_AUSTRIA              = 40,      /* Austria */
    CTRY_AZERBAIJAN           = 31,      /* Azerbaijan */
    CTRY_BAHRAIN              = 48,      /* Bahrain */
    CTRY_BELARUS              = 112,     /* Belarus */
    CTRY_BELGIUM              = 56,      /* Belgium */
    CTRY_BELIZE               = 84,      /* Belize */
    CTRY_BERMUDA              = 60,      /* Bermuda   */
    CTRY_BOLIVIA              = 68,      /* Bolivia */
    CTRY_BOSNIA_HERZEGOVINA   = 70, 	 /* Bosnia and Herzegovina */
    CTRY_BRAZIL               = 76,      /* Brazil */
    CTRY_BRUNEI_DARUSSALAM    = 96,      /* Brunei Darussalam */
    CTRY_BULGARIA             = 100,     /* Bulgaria */
    CTRY_CANADA               = 124,     /* Canada */
    CTRY_CHILE                = 152,     /* Chile */
    CTRY_CHINA                = 156,     /* People's Republic of China */
    CTRY_COLOMBIA             = 170,     /* Colombia */
    CTRY_COSTA_RICA           = 188,     /* Costa Rica */
    CTRY_CROATIA              = 191,     /* Croatia */
    CTRY_CYPRUS               = 196,
    CTRY_CZECH                = 203,     /* Czech Republic */
    CTRY_DENMARK              = 208,     /* Denmark */
    CTRY_DOMINICAN_REPUBLIC   = 214,     /* Dominican Republic */
    CTRY_ECUADOR              = 218,     /* Ecuador */
    CTRY_EGYPT                = 818,     /* Egypt */
    CTRY_EL_SALVADOR          = 222,     /* El Salvador */
    CTRY_ESTONIA              = 233,     /* Estonia */
    CTRY_FAEROE_ISLANDS       = 234,     /* Faeroe Islands */
    CTRY_FINLAND              = 246,     /* Finland */
    CTRY_FRANCE               = 250,     /* France */
    CTRY_FRANCE2              = 255,     /* France2 */
    CTRY_CHAD                 = 260,     /* Chad */
    CTRY_MALI                 = 261,     /* Mali */
    CTRY_GEORGIA              = 268,     /* Georgia */
    CTRY_GERMANY              = 276,     /* Germany */
    CTRY_GHANA                = 288,     /* Ghana */
    CTRY_GREECE               = 300,     /* Greece */
    CTRY_GUATEMALA            = 320,     /* Guatemala */
    CTRY_HONDURAS             = 340,     /* Honduras */
    CTRY_HONG_KONG            = 344,     /* Hong Kong S.A.R., P.R.C. */
    CTRY_HUNGARY              = 348,     /* Hungary */
    CTRY_ICELAND              = 352,     /* Iceland */
    CTRY_INDIA                = 356,     /* India */
    CTRY_INDONESIA            = 360,     /* Indonesia */
    CTRY_IRAN                 = 364,     /* Iran */
    CTRY_IRAQ                 = 368,     /* Iraq */
    CTRY_IRELAND              = 372,     /* Ireland */
    CTRY_ISRAEL               = 376,     /* Israel */
    CTRY_ITALY                = 380,     /* Italy */
    CTRY_JAMAICA              = 388,     /* Jamaica */
    CTRY_JAPAN                = 392,     /* Japan */
    CTRY_JAPAN1               = 393,     /* Japan (JP1) */
    CTRY_JAPAN2               = 394,     /* Japan (JP0) */
    CTRY_JAPAN3               = 395,     /* Japan (JP1-1) */
    CTRY_JAPAN4               = 396,     /* Japan (JE1) */
    CTRY_JAPAN5               = 397,     /* Japan (JE2) */
    CTRY_JAPAN6               = 399,	 /* Japan (JP6) */
    CTRY_JAPAN7               = 4007,	 /* Japan */
    CTRY_JAPAN8               = 4008,	 /* Japan */
    CTRY_JAPAN9               = 4009,	 /* Japan */
    CTRY_JAPAN10	      = 4010,	 /* Japan */
    CTRY_JAPAN11	      = 4011,	 /* Japan */
    CTRY_JAPAN12	      = 4012,	 /* Japan */
    CTRY_JAPAN13	      = 4013,	 /* Japan */
    CTRY_JAPAN14	      = 4014,	 /* Japan */
    CTRY_JAPAN15	      = 4015,	 /* Japan */
    CTRY_JAPAN16	      = 4016,	 /* Japan */
    CTRY_JAPAN17	      = 4017,	 /* Japan */
    CTRY_JAPAN18	      = 4018,	 /* Japan */
    CTRY_JAPAN19	      = 4019,	 /* Japan */
    CTRY_JAPAN20	      = 4020,	 /* Japan */
    CTRY_JAPAN21	      = 4021,	 /* Japan */
    CTRY_JAPAN22	      = 4022,	 /* Japan */
    CTRY_JAPAN23	      = 4023,	 /* Japan */
    CTRY_JAPAN24	      = 4024,	 /* Japan */
    CTRY_JORDAN               = 400,     /* Jordan */
    CTRY_KAZAKHSTAN           = 398,     /* Kazakhstan */
    CTRY_KENYA                = 404,     /* Kenya */
    CTRY_KOREA_NORTH          = 408,     /* North Korea */
    CTRY_KOREA_ROC            = 410,     /* South Korea */
    CTRY_KOREA_ROC2           = 411,     /* South Korea */
    CTRY_KOREA_ROC3           = 412,     /* South Korea */
    CTRY_KUWAIT               = 414,     /* Kuwait */
    CTRY_LATVIA               = 428,     /* Latvia */
    CTRY_LEBANON              = 422,     /* Lebanon */
    CTRY_LIBYA                = 434,     /* Libya */
    CTRY_LIECHTENSTEIN        = 438,     /* Liechtenstein */
    CTRY_LITHUANIA            = 440,     /* Lithuania */
    CTRY_LUXEMBOURG           = 442,     /* Luxembourg */
    CTRY_MACAU                = 446,     /* Macau */
    CTRY_MACEDONIA            = 807,     /* the Former Yugoslav Republic of Macedonia */
    CTRY_MALAYSIA             = 458,     /* Malaysia */
    CTRY_MALTA		          = 470,	 /* Malta */
    CTRY_MAURITIUS            = 480,     /* Mauritius */
    CTRY_MEXICO               = 484,     /* Mexico */
    CTRY_MONACO               = 492,     /* Principality of Monaco */
    CTRY_MOROCCO              = 504,     /* Morocco */
    CTRY_NETHERLANDS          = 528,     /* Netherlands */
    CTRY_NEW_ZEALAND          = 554,     /* New Zealand */
    CTRY_NICARAGUA            = 558,     /* Nicaragua */
    CTRY_NORWAY               = 578,     /* Norway */
    CTRY_OMAN                 = 512,     /* Oman */
    CTRY_PAKISTAN             = 586,     /* Islamic Republic of Pakistan */
    CTRY_PANAMA               = 591,     /* Panama */
    CTRY_PARAGUAY             = 600,     /* Paraguay */
    CTRY_PERU                 = 604,     /* Peru */
    CTRY_PHILIPPINES          = 608,     /* Republic of the Philippines */
    CTRY_POLAND               = 616,     /* Poland */
    CTRY_PORTUGAL             = 620,     /* Portugal */
    CTRY_PUERTO_RICO          = 630,     /* Puerto Rico */
    CTRY_QATAR                = 634,     /* Qatar */
    CTRY_ROMANIA              = 642,     /* Romania */
    CTRY_RUSSIA               = 643,     /* Russia */
    CTRY_SAUDI_ARABIA         = 682,     /* Saudi Arabia */
    CTRY_SERBIA               = 891,     /* Serbia and Montenegro */
    CTRY_SINGAPORE            = 702,     /* Singapore */
    CTRY_SLOVAKIA             = 703,     /* Slovak Republic */
    CTRY_SLOVENIA             = 705,     /* Slovenia */
    CTRY_SOUTH_AFRICA         = 710,     /* South Africa */
    CTRY_SPAIN                = 724,     /* Spain */
    CTRY_SRI_LANKA	      = 144,     /* Sri Lanka */
    CTRY_SWEDEN               = 752,     /* Sweden */
    CTRY_SWITZERLAND          = 756,     /* Switzerland */
    CTRY_SYRIA                = 760,     /* Syria */
    CTRY_TAIWAN               = 158,     /* Taiwan */
    CTRY_THAILAND             = 764,     /* Thailand */
    CTRY_TRINIDAD_Y_TOBAGO    = 780,     /* Trinidad y Tobago */
    CTRY_TUNISIA              = 788,     /* Tunisia */
    CTRY_TURKEY               = 792,     /* Turkey */
    CTRY_UAE                  = 784,     /* U.A.E. */
    CTRY_UKRAINE              = 804,     /* Ukraine */
    CTRY_UNITED_KINGDOM       = 826,     /* United Kingdom */
    CTRY_UNITED_STATES        = 840,     /* United States */
    CTRY_UNITED_STATES_FCC49  = 842,     /* United States (Public Safety)*/
    CTRY_URUGUAY              = 858,     /* Uruguay */
    CTRY_UZBEKISTAN           = 860,     /* Uzbekistan */
    CTRY_VENEZUELA            = 862,     /* Venezuela */
    CTRY_VIET_NAM             = 704,     /* Viet Nam */
    CTRY_YEMEN                = 887,     /* Yemen */
    CTRY_ZIMBABWE             = 716,     /* Zimbabwe */
    CTRY_AIRMONITOR           = 1,       /* Air Monitor */
    CTRY_SPECTRUM             = 2,       /* Spectrum */
    CTRY_MARITIME_OFFS        = 3,       /* Maritime Offs */
    CTRY_MARITIME_FOB         = 4,       /* Maritime FOB */
    CTRY_NIGERIA              = 566,     /* Nigeria */
};

#define IEEE80211_IS_MFP_FRAME(_frame) ((((_frame)->i_fc[0] & IEEE80211_FC0_TYPE_MASK) == IEEE80211_FC0_TYPE_MGT) && \
                                        ((_frame)->i_fc[1] & IEEE80211_FC1_WEP) && \
                                        ((((_frame)->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK) == IEEE80211_FC0_SUBTYPE_DEAUTH) || \
                                         (((_frame)->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK) == IEEE80211_FC0_SUBTYPE_DISASSOC) || \
                                         (((_frame)->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK) == IEEE80211_FC0_SUBTYPE_ACTION)))

/* 
 * Country information element.
 */
#define IEEE80211_COUNTRY_MAX_TRIPLETS (83)
struct ieee80211_ie_country {
	u_int8_t	country_id;
	u_int8_t	country_len;
	u_int8_t	country_str[3];
	u_int8_t	country_triplet[IEEE80211_COUNTRY_MAX_TRIPLETS*3];
} __attribute__ ((packed));

/* does frame have QoS sequence control data */
#define	IEEE80211_QOS_HAS_SEQ(wh) \
	(((wh)->i_fc[0] & \
	  (IEEE80211_FC0_TYPE_MASK | IEEE80211_FC0_SUBTYPE_QOS)) == \
	  (IEEE80211_FC0_TYPE_DATA | IEEE80211_FC0_SUBTYPE_QOS))

#define WME_QOSINFO_COUNT	0x0f  /* Mask for Param Set Count field */
/*
 * WME/802.11e information element.
 */
struct ieee80211_ie_wme {
	u_int8_t	wme_id;		/* IEEE80211_ELEMID_VENDOR */
	u_int8_t	wme_len;	/* length in bytes */
	u_int8_t	wme_oui[3];	/* 0x00, 0x50, 0xf2 */
	u_int8_t	wme_type;	/* OUI type */
	u_int8_t	wme_subtype;	/* OUI subtype */
	u_int8_t	wme_version;	/* spec revision */
	u_int8_t	wme_info;	/* QoS info */
} __attribute__ ((packed));

/*
 * WME/802.11e Tspec Element
 */
struct ieee80211_wme_tspec {
	u_int8_t	ts_id;
	u_int8_t	ts_len;
	u_int8_t	ts_oui[3];
	u_int8_t	ts_oui_type;
	u_int8_t	ts_oui_subtype;
	u_int8_t	ts_version;
	u_int8_t	ts_tsinfo[3];
	u_int8_t	ts_nom_msdu[2];
	u_int8_t	ts_max_msdu[2];
	u_int8_t	ts_min_svc[4];
	u_int8_t	ts_max_svc[4];
	u_int8_t	ts_inactv_intv[4];
	u_int8_t	ts_susp_intv[4];
	u_int8_t	ts_start_svc[4];
	u_int8_t	ts_min_rate[4];
	u_int8_t	ts_mean_rate[4];
	u_int8_t	ts_max_burst[4];
	u_int8_t	ts_min_phy[4];
	u_int8_t	ts_peak_rate[4];
	u_int8_t	ts_delay[4];
	u_int8_t	ts_surplus[2];
	u_int8_t	ts_medium_time[2];
} __attribute__ ((packed));

/*
 * WME AC parameter field
 */

struct ieee80211_wme_acparams {
	u_int8_t	acp_aci_aifsn;
	u_int8_t	acp_logcwminmax;
	u_int16_t	acp_txop;
} __attribute__ ((packed));

#define IEEE80211_WME_PARAM_LEN 24
#define WME_NUM_AC		4	/* 4 AC categories */

#define WME_PARAM_ACI		0x60	/* Mask for ACI field */
#define WME_PARAM_ACI_S		5	/* Shift for ACI field */
#define WME_PARAM_ACM		0x10	/* Mask for ACM bit */
#define WME_PARAM_ACM_S		4	/* Shift for ACM bit */
#define WME_PARAM_AIFSN		0x0f	/* Mask for aifsn field */
#define WME_PARAM_AIFSN_S	0	/* Shift for aifsn field */
#define WME_PARAM_LOGCWMIN	0x0f	/* Mask for CwMin field (in log) */
#define WME_PARAM_LOGCWMIN_S	0	/* Shift for CwMin field */
#define WME_PARAM_LOGCWMAX	0xf0	/* Mask for CwMax field (in log) */
#define WME_PARAM_LOGCWMAX_S	4	/* Shift for CwMax field */

#define WME_AC_TO_TID(_ac) (       \
	((_ac) == WME_AC_VO) ? 6 : \
	((_ac) == WME_AC_VI) ? 5 : \
	((_ac) == WME_AC_BK) ? 1 : \
	0)

#define TID_TO_WME_AC(_tid) (      \
	((_tid) < 1) ? WME_AC_BE : \
	((_tid) < 3) ? WME_AC_BK : \
	((_tid) == 3) ? WME_AC_BE : \
	((_tid) < 6) ? WME_AC_VI : \
	WME_AC_VO)

/*
 * WME Parameter Element
 */

struct ieee80211_wme_param {
	u_int8_t	param_id;
	u_int8_t	param_len;
	u_int8_t	param_oui[3];
	u_int8_t	param_oui_type;
	u_int8_t	param_oui_sybtype;
	u_int8_t	param_version;
	u_int8_t	param_qosInfo;
	u_int8_t	param_reserved;
	struct ieee80211_wme_acparams	params_acParams[WME_NUM_AC];
} __attribute__ ((packed));

/*
 * WME U-APSD qos info field defines
 */
#define WME_CAPINFO_UAPSD_EN			0x00000080
#define WME_CAPINFO_UAPSD_VO			0x00000001
#define WME_CAPINFO_UAPSD_VI			0x00000002
#define WME_CAPINFO_UAPSD_BK			0x00000004
#define WME_CAPINFO_UAPSD_BE			0x00000008
#define WME_CAPINFO_UAPSD_ACFLAGS_SHIFT		0
#define WME_CAPINFO_UAPSD_ACFLAGS_MASK		0xF
#define WME_CAPINFO_UAPSD_MAXSP_SHIFT		5
#define WME_CAPINFO_UAPSD_MAXSP_MASK		0x3
#define WME_CAPINFO_IE_OFFSET			8
#define WME_UAPSD_MAXSP(_qosinfo) (((_qosinfo) >> WME_CAPINFO_UAPSD_MAXSP_SHIFT) & WME_CAPINFO_UAPSD_MAXSP_MASK)
#define WME_UAPSD_AC_ENABLED(_ac, _qosinfo) ( (1<<(3 - (_ac))) &   \
		(((_qosinfo) >> WME_CAPINFO_UAPSD_ACFLAGS_SHIFT) & WME_CAPINFO_UAPSD_ACFLAGS_MASK) )

/*
 * Atheros Advanced Capability information element.
 */
struct ieee80211_ie_athAdvCap {
	u_int8_t	athAdvCap_id;		/* IEEE80211_ELEMID_VENDOR */
	u_int8_t	athAdvCap_len;		/* length in bytes */
	u_int8_t	athAdvCap_oui[3];	/* 0x00, 0x03, 0x7f */
	u_int8_t	athAdvCap_type;		/* OUI type */
	u_int8_t	athAdvCap_subtype;	/* OUI subtype */
	u_int8_t	athAdvCap_version;	/* spec revision */
	u_int8_t	athAdvCap_capability;	/* Capability info */
	u_int16_t	athAdvCap_defKeyIndex;
} __attribute__ ((packed));

/*
 * Atheros Extended Capability information element.
 */
struct ieee80211_ie_ath_extcap {
    u_int8_t    ath_extcap_id;          /* IEEE80211_ELEMID_VENDOR */
    u_int8_t    ath_extcap_len;         /* length in bytes */
    u_int8_t    ath_extcap_oui[3];      /* 0x00, 0x03, 0x7f */
    u_int8_t    ath_extcap_type;        /* OUI type */
    u_int8_t    ath_extcap_subtype;     /* OUI subtype */
    u_int8_t    ath_extcap_version;     /* spec revision */
    u_int32_t   ath_extcap_extcap              : 16,  /* B0-15  extended capabilities */
                ath_extcap_weptkipaggr_rxdelim : 8,   /* B16-23 num delimiters for receiving WEP/TKIP aggregates */
                ath_extcap_reserved            : 8;   /* B24-31 reserved */
} __packed;

/*
 * Atheros XR information element.
 */
struct ieee80211_xr_param {
	u_int8_t	param_id;
	u_int8_t	param_len;
	u_int8_t	param_oui[3];
	u_int8_t	param_oui_type;
	u_int8_t	param_oui_sybtype;
	u_int8_t	param_version;
	u_int8_t	param_Info;
	u_int8_t	param_base_bssid[IEEE80211_ADDR_LEN];
	u_int8_t	param_xr_bssid[IEEE80211_ADDR_LEN];
	u_int16_t	param_xr_beacon_interval;
	u_int8_t	param_base_ath_capability;
	u_int8_t	param_xr_ath_capability;
} __attribute__ ((packed));

/* Atheros capabilities */
#define IEEE80211_ATHC_TURBOP	0x0001		/* Turbo Prime */
#define IEEE80211_ATHC_COMP	0x0002		/* Compression */
#define IEEE80211_ATHC_FF	0x0004		/* Fast Frames */
#define IEEE80211_ATHC_XR	0x0008		/* Xtended Range support */
#define IEEE80211_ATHC_AR	0x0010		/* Advanced Radar support */
#define IEEE80211_ATHC_BURST	0x0020		/* Bursting - not negotiated */
#define IEEE80211_ATHC_WME	0x0040		/* CWMin tuning */
#define IEEE80211_ATHC_BOOST	0x0080		/* Boost */
/* Atheros extended capabilities */
#define IEEE80211_ATH_EXTCAP_OWL         0x0001      /* OWL device */
#define IEEE80211_ATHEC_WEPTKIPAGGR	     0x0002
#define IEEE80211_ATHEC_EXTRADELIMWAR    0x0004

/* Atheros extended capabilities */
/* OWL device capable of WDS workaround */
#define IEEE80211_ATHEC_OWLWDSWAR        0x00000001



/*
 * Management Notification Frame
 */
struct ieee80211_mnf {
	u_int8_t	mnf_category;
	u_int8_t	mnf_action;
	u_int8_t	mnf_dialog;
	u_int8_t	mnf_status;
} __attribute__ ((packed));
#define	MNF_SETUP_REQ	0
#define	MNF_SETUP_RESP	1
#define	MNF_TEARDOWN	2

/*
 * Management Action Frames 
 */

/* generic frame format */
struct ieee80211_action {
	u_int8_t	ia_category;
	u_int8_t	ia_action;
} __packed;

/* categories */
/* See table 8-38, IEEE802.11-2012 */
#define IEEE80211_ACTION_CAT_SPECTRUM          0   /* Spectrum management */
#define IEEE80211_ACTION_CAT_QOS               1   /* qos */
#define IEEE80211_ACTION_CAT_DLS               2   /* DLS */
#define IEEE80211_ACTION_CAT_BA                 3   	/* BA */
#define IEEE80211_ACTION_CAT_COEX           4   /* Coex */
#define IEEE80211_ACTION_CAT_RRM                5       /* Radio Resource Measurement */
#define IEEE80211_ACTION_CAT_FT                6   /* Fast BSS Transition */
#define IEEE80211_ACTION_CAT_HT			7	/* HT per IEEE802.11n-D1.06 */
#define IEEE80211_ACTION_CAT_SAQ		8
#define IEEE80211_ACTION_CAT_PDP               9
#define IEEE80211_ACTION_CAT_WNM               10
#define IEEE80211_ACTION_CAT_UNPROTECTED_WNM   11
#define IEEE80211_ACTION_CAT_TDLS              12
#define IEEE80211_ACTION_CAT_MESH              13
#define IEEE80211_ACTION_CAT_MULTIHOP          14
#define IEEE80211_ACTION_CAT_VENDOR_PROTECTED  126

/* HT actions */
#define IEEE80211_ACTION_HT_TXCHWIDTH		0	/* recommended transmission channel width */
#define IEEE80211_ACTION_HT_SMPOWERSAVE		1	/* Spatial Multiplexing (SM) Power Save */
#define IEEE80211_ACTION_HT_CSI             4   /* CSI Frame */
#define IEEE80211_ACTION_HT_NONCOMP_BF      5   /* Non-compressed Beamforming*/
#define IEEE80211_ACTION_HT_COMP_BF         6   /* Compressed Beamforming*/

/* Radio Measurement Actions */
#define IEEE80211_ACTION_RRM_REQUEST         0 /* Action: Radio Measurement Request */
#define IEEE80211_ACTION_RRM_REPORT          1 /* Action: Radio Measurement Report */

/* Link Measurement Actions */
#define IEEE80211_ACTION_LM_REQUEST          2
#define IEEE80211_ACTION_LM_REPORT           3

#ifdef __FAT_AP__
/* Neighbour Report Actions */
#define IEEE80211_ACTION_NR_REQUEST          4
#define IEEE80211_ACTION_NR_REPORT           5

/* WNM BSS transition Actions */
#define IEEE80211_ACTION_BSS_TM_REQUEST      7
#endif

#define IEEE80211_ACTION_CSA                 4 /* Action: Channel Switch announcement. */

/* HT - recommended transmission channel width */
struct ieee80211_action_ht_txchwidth {
	struct ieee80211_action 	at_header;
	u_int8_t			at_chwidth;	
} __packed;

#define IEEE80211_A_HT_TXCHWIDTH_20	0
#define IEEE80211_A_HT_TXCHWIDTH_2040	1

/* HT - Spatial Multiplexing (SM) Power Save */
struct ieee80211_action_ht_smpowersave {
	struct ieee80211_action		as_header;
	u_int8_t			as_control;
} __packed;

/*HT - CSI Frame */     //for TxBF RC
#define MIMO_CONTROL_LEN 6
struct ieee80211_action_ht_CSI {
    struct ieee80211_action     as_header;
    u_int8_t                   mimo_control[MIMO_CONTROL_LEN];
} __packed;

/*HT - V/CV report frame*/
struct ieee80211_action_ht_txbf_rpt {
    struct ieee80211_action     as_header;
    u_int8_t                   mimo_control[MIMO_CONTROL_LEN];
} __packed;

/* values defined for 'as_control' field per 802.11n-D1.06 */
#define IEEE80211_A_HT_SMPOWERSAVE_DISABLED     0x00   /* SM Power Save Disabled, SM packets ok  */
#define IEEE80211_A_HT_SMPOWERSAVE_ENABLED      0x01   /* SM Power Save Enabled bit  */
#define IEEE80211_A_HT_SMPOWERSAVE_MODE         0x02   /* SM Power Save Mode bit */
#define IEEE80211_A_HT_SMPOWERSAVE_RESERVED     0xFC   /* SM Power Save Reserved bits */

/* values defined for SM Power Save Mode bit */
#define IEEE80211_A_HT_SMPOWERSAVE_STATIC       0x00   /* Static, SM packets not ok */
#define IEEE80211_A_HT_SMPOWERSAVE_DYNAMIC      0x02   /* Dynamic, SM packets ok if preceded by RTS */

/* BA actions */
#define IEEE80211_ACTION_BA_ADDBA_REQUEST       0   /* ADDBA request */
#define IEEE80211_ACTION_BA_ADDBA_RESPONSE      1   /* ADDBA response */
#define IEEE80211_ACTION_BA_DELBA	        2   /* DELBA */

struct ieee80211_ba_parameterset {
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
        u_int16_t	buffersize      : 10,   /* B6-15  buffer size */
			tid             : 4,    /* B2-5   TID */
			bapolicy        : 1,    /* B1   block ack policy */
			amsdusupported  : 1;    /* B0   amsdu supported */
#else
        u_int16_t   amsdusupported  : 1,    /* B0   amsdu supported */
			bapolicy        : 1,    /* B1   block ack policy */
			tid             : 4,    /* B2-5   TID */
			buffersize      : 10;   /* B6-15  buffer size */
#endif
} __packed;

#define  IEEE80211_BA_POLICY_DELAYED      0
#define  IEEE80211_BA_POLICY_IMMEDIATE    1
#define  IEEE80211_BA_AMSDU_SUPPORTED     1

struct ieee80211_ba_seqctrl {
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
        u_int16_t    	startseqnum     : 12,    /* B4-15  starting sequence number */
			fragnum         : 4;     /* B0-3  fragment number */
#else
        u_int16_t    	fragnum         : 4,     /* B0-3  fragment number */
			startseqnum     : 12;    /* B4-15  starting sequence number */
#endif
} __packed;

struct ieee80211_delba_parameterset {
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
        u_int16_t	tid             : 4,     /* B12-15  tid */
			initiator       : 1,     /* B11     initiator */
			reserved0       : 11;    /* B0-10   reserved */
#else
        u_int16_t    	reserved0       : 11,    /* B0-10   reserved */
			initiator       : 1,     /* B11     initiator */
			tid             : 4;     /* B12-15  tid */
#endif
} __packed;

/* BA - ADDBA request */
struct ieee80211_action_ba_addbarequest {
	struct ieee80211_action			rq_header;
        u_int8_t                		rq_dialogtoken;
        struct ieee80211_ba_parameterset        rq_baparamset; 
        u_int16_t                	        rq_batimeout;	/* in TUs */
        struct ieee80211_ba_seqctrl     	rq_basequencectrl;
} __packed;

/* BA - ADDBA response */
struct ieee80211_action_ba_addbaresponse {
	struct ieee80211_action			rs_header;
        u_int8_t                		rs_dialogtoken;
        u_int16_t                		rs_statuscode;
        struct ieee80211_ba_parameterset        rs_baparamset; 
        u_int16_t                		rs_batimeout;          /* in TUs */
} __packed;

/* BA - DELBA */
struct ieee80211_action_ba_delba {
	struct ieee80211_action			dl_header;
        struct ieee80211_delba_parameterset     dl_delbaparamset;
        u_int16_t		                dl_reasoncode;
} __packed;

/*
 * Control frames.
 */
struct ieee80211_frame_min {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_addr1[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr2[IEEE80211_ADDR_LEN];
	/* FCS */
} __attribute__ ((packed));

/*
 * BAR frame format
 */
#define IEEE80211_BAR_CTL_TID_M     0xF000      /* tid mask             */
#define IEEE80211_BAR_CTL_TID_S         12      /* tid shift            */
#define IEEE80211_BAR_CTL_NOACK     0x0001      /* no-ack policy        */
#define IEEE80211_BAR_CTL_COMBA     0x0004      /* compressed block-ack */

struct ieee80211_frame_bar {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_ra[IEEE80211_ADDR_LEN];
	u_int8_t	i_ta[IEEE80211_ADDR_LEN];
    u_int16_t   i_ctl;
    u_int16_t   i_seq;
	/* FCS */
} __packed;
struct ieee80211_frame_rts {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_ra[IEEE80211_ADDR_LEN];
	u_int8_t	i_ta[IEEE80211_ADDR_LEN];
	/* FCS */
} __attribute__ ((packed));

struct ieee80211_frame_cts {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_ra[IEEE80211_ADDR_LEN];
	/* FCS */
} __attribute__ ((packed));

struct ieee80211_frame_ack {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_ra[IEEE80211_ADDR_LEN];
	/* FCS */
} __attribute__ ((packed));

struct ieee80211_frame_pspoll {
	u_int8_t	i_fc[2];
	u_int8_t	i_aid[2];
	u_int8_t	i_bssid[IEEE80211_ADDR_LEN];
	u_int8_t	i_ta[IEEE80211_ADDR_LEN];
	/* FCS */
} __attribute__ ((packed));

struct ieee80211_frame_cfend {		/* NB: also CF-End+CF-Ack */
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];	/* should be zero */
	u_int8_t	i_ra[IEEE80211_ADDR_LEN];
	u_int8_t	i_bssid[IEEE80211_ADDR_LEN];
	/* FCS */
} __attribute__ ((packed));

/*
 * BEACON management packets
 *
 *	octet timestamp[8]
 *	octet beacon interval[2]
 *	octet capability information[2]
 *	information element
 *		octet elemid
 *		octet length
 *		octet information[length]
 */

typedef u_int8_t *ieee80211_mgt_beacon_t;

#define	IEEE80211_BEACON_INTERVAL(beacon) \
	((beacon)[8] | ((beacon)[9] << 8))
#define	IEEE80211_BEACON_CAPABILITY(beacon) \
	((beacon)[10] | ((beacon)[11] << 8))

#define	IEEE80211_CAPINFO_ESS			0x0001
#define	IEEE80211_CAPINFO_IBSS			0x0002
#define	IEEE80211_CAPINFO_CF_POLLABLE		0x0004
#define	IEEE80211_CAPINFO_CF_POLLREQ		0x0008
#define	IEEE80211_CAPINFO_PRIVACY		0x0010
#define	IEEE80211_CAPINFO_SHORT_PREAMBLE	0x0020
#define	IEEE80211_CAPINFO_PBCC			0x0040
#define	IEEE80211_CAPINFO_CHNL_AGILITY		0x0080
/* bits 8-9 are reserved (8 now for specturm management) */
#define IEEE80211_CAPINFO_SPECTRUM_MGMT		0x0100
#define	IEEE80211_CAPINFO_SHORT_SLOTTIME	0x0400
#define	IEEE80211_CAPINFO_UAPSD			0x0800
#define	IEEE80211_CAPINFO_RRM			0x1000
#define	IEEE80211_CAPINFO_DSSSOFDM		0x2000
/* bits 14-15 are reserved */

/*
 * 802.11i/WPA information element (maximally sized).
 */
struct ieee80211_ie_wpa {
	u_int8_t	wpa_id;		/* IEEE80211_ELEMID_VENDOR */
	u_int8_t	wpa_len;	/* length in bytes */
	u_int8_t	wpa_oui[3];	/* 0x00, 0x50, 0xf2 */
	u_int8_t	wpa_type;	/* OUI type */
	u_int16_t	wpa_version;	/* spec revision */
	u_int32_t	wpa_mcipher[1];	/* multicast/group key cipher */
	u_int16_t	wpa_uciphercnt;	/* # pairwise key ciphers */
	u_int32_t	wpa_uciphers[8];/* ciphers */
	u_int16_t	wpa_authselcnt;	/* authentication selector cnt*/
	u_int32_t	wpa_authsels[8];/* selectors */
	u_int16_t	wpa_caps;	/* 802.11i capabilities */
	u_int16_t	wpa_pmkidcnt;	/* 802.11i pmkid count */
	u_int16_t	wpa_pmkids[8];	/* 802.11i pmkids */
} __attribute__ ((packed));

#ifndef IEEE80211N_IE
/* Temporary vendor specific IE for 11n pre-standard interoperability */
#define HT_OUI 0x4c9000
#endif

#ifdef ATH_SUPPORT_TxBF
union ieee80211_hc_txbf {
    struct {
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
        u_int32_t   reserved              : 3,
                channel_estimation_cap    : 2,
                csi_max_rows_bfer         : 2,
                comp_bfer_antennas        : 2,
                noncomp_bfer_antennas     : 2,
                csi_bfer_antennas         : 2,
                minimal_grouping          : 2,
                explicit_comp_bf          : 2,
                explicit_noncomp_bf       : 2,
                explicit_csi_feedback     : 2,
                explicit_comp_steering    : 1,
                explicit_noncomp_steering : 1,
                explicit_csi_txbf_capable : 1,
                calibration               : 2,
                implicit_txbf_capable     : 1,
                tx_ndp_capable            : 1,
                rx_ndp_capable            : 1,
                tx_staggered_sounding     : 1,
                rx_staggered_sounding     : 1,
                implicit_rx_capable       : 1;
#else
        u_int32_t   implicit_rx_capable   : 1,
                rx_staggered_sounding     : 1,
                tx_staggered_sounding     : 1,
                rx_ndp_capable            : 1,
                tx_ndp_capable            : 1,
                implicit_txbf_capable     : 1,
                calibration               : 2,
                explicit_csi_txbf_capable : 1,
                explicit_noncomp_steering : 1,
                explicit_comp_steering    : 1,
                explicit_csi_feedback     : 2,
                explicit_noncomp_bf       : 2,
                explicit_comp_bf          : 2,
                minimal_grouping          : 2,
                csi_bfer_antennas         : 2,
                noncomp_bfer_antennas     : 2,
                comp_bfer_antennas        : 2,
                csi_max_rows_bfer         : 2,
                channel_estimation_cap    : 2,
                reserved                  : 3;
#endif
    };

    u_int32_t value;
} __packed;
#endif

struct ieee80211_ie_htcap_cmn {
	u_int16_t	hc_cap;			/* HT capabilities */
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
	u_int8_t	hc_reserved 	: 3, 	/* B5-7 reserved */
                hc_mpdudensity 	: 3, 	/* B2-4 MPDU density (aka Minimum MPDU Start Spacing) */
                hc_maxampdu 	: 2;	/* B0-1 maximum rx A-MPDU factor */
#else
	u_int8_t	hc_maxampdu 	: 2,	/* B0-1 maximum rx A-MPDU factor */
                hc_mpdudensity 	: 3, 	/* B2-4 MPDU density (aka Minimum MPDU Start Spacing) */
                hc_reserved 	: 3; 	/* B5-7 reserved */
#endif
	u_int8_t 	hc_mcsset[16]; 		/* supported MCS set */
	u_int16_t	hc_extcap;		/* extended HT capabilities */
#ifdef ATH_SUPPORT_TxBF
    union ieee80211_hc_txbf hc_txbf;    /* txbf capabilities */
#else
    u_int32_t   hc_txbf;                /* txbf capabilities */
#endif
	u_int8_t	hc_antenna;		/* antenna capabilities */
} __packed;

/*
 * 802.11n HT Capability IE
 */
struct ieee80211_ie_htcap {
	u_int8_t	                    hc_id;			/* element ID */
	u_int8_t	                    hc_len;			/* length in bytes */
	struct ieee80211_ie_htcap_cmn   hc_ie;
} __packed;

/*
 * Temporary vendor private HT Capability IE
 */
struct vendor_ie_htcap {
	u_int8_t	                    hc_id;			/* element ID */
	u_int8_t	                    hc_len;			/* length in bytes */
	u_int8_t                        hc_oui[3];
	u_int8_t                        hc_ouitype;
	struct ieee80211_ie_htcap_cmn   hc_ie;
} __packed;

/* HT capability flags */
#define	IEEE80211_HTCAP_C_ADVCODING		      0x0001
#define	IEEE80211_HTCAP_C_CHWIDTH40	      	  0x0002	
#define	IEEE80211_HTCAP_C_SMPOWERSAVE_STATIC      0x0000 /* Capable of SM Power Save (Static) */
#define	IEEE80211_HTCAP_C_SMPOWERSAVE_DYNAMIC     0x0004 /* Capable of SM Power Save (Dynamic) */
#define	IEEE80211_HTCAP_C_SM_RESERVED             0x0008 /* Reserved */
#define	IEEE80211_HTCAP_C_SM_ENABLED              0x000c /* SM enabled, no SM Power Save */
#define	IEEE80211_HTCAP_C_GREENFIELD	      0x0010
#define IEEE80211_HTCAP_C_SHORTGI20		      0x0020
#define IEEE80211_HTCAP_C_SHORTGI40     	  0x0040
#define IEEE80211_HTCAP_C_TXSTBC        	  0x0080
#define IEEE80211_HTCAP_C_RXSTBC        	  0x0300  /* 2 bits */
#define IEEE80211_HTCAP_C_RXSTBC_S                 8
#define IEEE80211_HTCAP_C_DELAYEDBLKACK 	  0x0400
#define IEEE80211_HTCAP_C_MAXAMSDUSIZE  	  0x0800  /* 1 = 8K, 0 = 3839B */
#define IEEE80211_HTCAP_C_DSSSCCK40     	  0x1000  
#define IEEE80211_HTCAP_C_PSMP          	  0x2000  
#define IEEE80211_HTCAP_C_INTOLERANT40		  0x4000  
#define IEEE80211_HTCAP_C_LSIGTXOPPROT  	  0x8000  

#define	IEEE80211_HTCAP_C_SM_MASK                 0x000c /* Spatial Multiplexing (SM) capabitlity bitmask */

/* B0-1 maximum rx A-MPDU factor 2^(13+Max Rx A-MPDU Factor) */
enum {
	IEEE80211_HTCAP_MAXRXAMPDU_8192,	/* 2 ^ 13 */
	IEEE80211_HTCAP_MAXRXAMPDU_16384,   /* 2 ^ 14 */
	IEEE80211_HTCAP_MAXRXAMPDU_32768,   /* 2 ^ 15 */
	IEEE80211_HTCAP_MAXRXAMPDU_65536,   /* 2 ^ 16 */
};
#define IEEE80211_HTCAP_MAXRXAMPDU_FACTOR   13

/* B2-4 MPDU density (usec) */
enum {
	IEEE80211_HTCAP_MPDUDENSITY_NA,		/* No time restriction */
	IEEE80211_HTCAP_MPDUDENSITY_0_25,  	/* 1/4 usec */
	IEEE80211_HTCAP_MPDUDENSITY_0_5,    /* 1/2 usec */
	IEEE80211_HTCAP_MPDUDENSITY_1,     	/* 1 usec */
	IEEE80211_HTCAP_MPDUDENSITY_2,     	/* 2 usec */
	IEEE80211_HTCAP_MPDUDENSITY_4,     	/* 4 usec */
	IEEE80211_HTCAP_MPDUDENSITY_8,     	/* 8 usec */
	IEEE80211_HTCAP_MPDUDENSITY_16,     	/* 16 usec */
};

/* HT extended capability flags */
#define	IEEE80211_HTCAP_EXTC_PCO		        0x0001
#define IEEE80211_HTCAP_EXTC_TRANS_TIME_RSVD    0x0000  
#define IEEE80211_HTCAP_EXTC_TRANS_TIME_400     0x0002 /* 20-40 switch time */
#define IEEE80211_HTCAP_EXTC_TRANS_TIME_1500    0x0004 /* in us             */
#define IEEE80211_HTCAP_EXTC_TRANS_TIME_5000    0x0006 
#define IEEE80211_HTCAP_EXTC_RSVD_1             0x00f8
#define IEEE80211_HTCAP_EXTC_MCS_FEEDBACK_NONE  0x0000
#define IEEE80211_HTCAP_EXTC_MCS_FEEDBACK_RSVD  0x0100
#define IEEE80211_HTCAP_EXTC_MCS_FEEDBACK_UNSOL 0x0200
#define IEEE80211_HTCAP_EXTC_MCS_FEEDBACK_FULL  0x0300
#define IEEE80211_HTCAP_EXTC_RSVD_2             0xfc00
#ifdef ATH_SUPPORT_TxBF
#define IEEE80211_HTCAP_EXTC_HTC_SUPPORT        0x0400
#endif

struct ieee80211_ie_htinfo_cmn {
	u_int8_t	hi_ctrlchannel;		/* control channel */
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
	u_int8_t	hi_serviceinterval : 3,    /* B5-7 svc interval granularity */
                hi_ctrlaccess      : 1,    /* B4   controlled access only */
                hi_rifsmode        : 1,    /* B3   rifs mode */
                hi_txchwidth       : 1,    /* B2   recommended xmiss width set */
                hi_extchoff        : 2;    /* B0-1 extension channel offset */
#else
	u_int8_t    hi_extchoff        : 2,    /* B0-1 extension channel offset */
                hi_txchwidth       : 1,    /* B2   recommended xmiss width set */
                hi_rifsmode        : 1,    /* B3   rifs mode */
                hi_ctrlaccess      : 1,    /* B4   controlled access only */
                hi_serviceinterval : 3;    /* B5-7 svc interval granularity */
#endif
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
    u_int8_t   hi_reserved0       : 3,   /* B5-7 Reserved */
                hi_obssnonhtpresent: 1,    /* B4    OBSS Non-HT STAs Present */
                hi_txburstlimit    : 1,    /* B3    Transmit Burst Limit */
                hi_nongfpresent    : 1,    /* B2    Non-greenfield STAs present */
                hi_opmode          : 2;    /* B0-1  Operating Mode */
#else
    u_int8_t   hi_opmode          : 2,    /* B0-1  Operating Mode */
                hi_nongfpresent    : 1,    /* B2    Non-greenfield STAs present */
                hi_txburstlimit    : 1,    /* B3    Transmit Burst Limit */
                hi_obssnonhtpresent: 1,    /* B4    OBSS Non-HT STAs Present */
                hi_reserved0       : 3;   /* B5-7 Reserved */
#endif
    u_int8_t    hi_reserved1;
    u_int16_t   hi_miscflags;

	u_int8_t 	hi_basicmcsset[16]; 	/* basic MCS set */
} __packed;

/*
 * 802.11n HT Information IE
 */
struct ieee80211_ie_htinfo {
	u_int8_t	                    hi_id;			/* element ID */
	u_int8_t	                    hi_len;			/* length in bytes */
	struct ieee80211_ie_htinfo_cmn  hi_ie;
} __packed;

/*
 * Temporary vendor private HT Information IE
 */
struct vendor_ie_htinfo {
	u_int8_t	                    hi_id;			/* element ID */
	u_int8_t                    	hi_len;			/* length in bytes */
    u_int8_t                        hi_oui[3];
    u_int8_t                        hi_ouitype;
    struct ieee80211_ie_htinfo_cmn  hi_ie;
} __packed;

/* extension channel offset (2 bit signed number) */
enum {
	IEEE80211_HTINFO_EXTOFFSET_NA	 = 0,	/* 0  no extension channel is present */			
	IEEE80211_HTINFO_EXTOFFSET_ABOVE = 1,   /* +1 extension channel above control channel */ 
	IEEE80211_HTINFO_EXTOFFSET_UNDEF = 2,   /* -2 undefined */ 
	IEEE80211_HTINFO_EXTOFFSET_BELOW = 3	/* -1 extension channel below control channel*/
};

/* recommended transmission width set */
enum {
	IEEE80211_HTINFO_TXWIDTH_20,		
	IEEE80211_HTINFO_TXWIDTH_2040      	
};

/* operating flags */
#define	IEEE80211_HTINFO_OPMODE_PURE		    0x00 /* no protection */
#define IEEE80211_HTINFO_OPMODE_MIXED_PROT_OPT	0x01 /* prot optional (legacy device maybe present) */      	
#define	IEEE80211_HTINFO_OPMODE_MIXED_PROT_40   0x02 /* prot required (20 MHz) */   
#define	IEEE80211_HTINFO_OPMODE_MIXED_PROT_ALL	0x03 /* prot required (legacy devices present) */      	
#define IEEE80211_HTINFO_OPMODE_MASK		0x03 /* For protection 0x00-0x03 */

/* Non-greenfield STAs present */
enum {
	IEEE80211_HTINFO_NON_GF_NOT_PRESENT,	/* Non-greenfield STAs not present */
	IEEE80211_HTINFO_NON_GF_PRESENT,	/* Non-greenfield STAs present */
};

/* Transmit Burst Limit */
enum {
	IEEE80211_HTINFO_TXBURST_UNLIMITED, /* Transmit Burst is unlimited */
	IEEE80211_HTINFO_TXBURST_LIMITED, /* Transmit Burst is limited */
};

/* OBSS Non-HT STAs present */
enum {
	IEEE80211_HTINFO_OBBSS_NONHT_NOT_PRESENT, /* OBSS Non-HT STAs not present */
	IEEE80211_HTINFO_OBBSS_NONHT_PRESENT, /* OBSS Non-HT STAs present */
};

/* misc flags */
#define IEEE80211_HTINFO_BASICSTBCMCS    0x007F    /* B0-6 basic STBC MCS */
#define IEEE80211_HTINFO_DUALSTBCPROT    0x0080    /* B7   dual stbc protection */
#define IEEE80211_HTINFO_SECONDARYBEACON 0x0100    /* B8   secondary beacon */
#define IEEE80211_HTINFO_LSIGTXOPPROT    0x0200    /* B9   lsig txop prot full support */
#define IEEE80211_HTINFO_PCOACTIVE       0x0400    /* B10  pco active */
#define IEEE80211_HTINFO_PCOPHASE        0x0800    /* B11  pco phase */

/* RIFS mode */
enum {
	IEEE80211_HTINFO_RIFSMODE_PROHIBITED,	/* use of rifs prohibited */
	IEEE80211_HTINFO_RIFSMODE_ALLOWED,	/* use of rifs permitted */
};

/*
 * Management information element payloads.
 */

enum {
	IEEE80211_ELEMID_SSID		= 0,
	IEEE80211_ELEMID_RATES		= 1,
	IEEE80211_ELEMID_FHPARMS	= 2,
	IEEE80211_ELEMID_DSPARMS	= 3,
	IEEE80211_ELEMID_CFPARMS	= 4,
	IEEE80211_ELEMID_TIM		= 5,
	IEEE80211_ELEMID_IBSSPARMS	= 6,
	IEEE80211_ELEMID_COUNTRY	= 7,
	IEEE80211_ELEMID_REQINFO	= 10,
	IEEE80211_ELEMID_BSS_LOAD	= 11,
	IEEE80211_ELEMID_CHALLENGE	= 16,
	/* 17-31 reserved for challenge text extension */
	IEEE80211_ELEMID_PWRCNSTR	= 32,
	IEEE80211_ELEMID_PWRCAP		= 33,
	IEEE80211_ELEMID_TPCREQ		= 34,
	IEEE80211_ELEMID_TPCREP		= 35,
	IEEE80211_ELEMID_SUPPCHAN	= 36,
	IEEE80211_ELEMID_CHANSWITCHANN	= 37,
	IEEE80211_ELEMID_MEASREQ	= 38,
	IEEE80211_ELEMID_MEASREP	= 39,
	IEEE80211_ELEMID_QUIET		= 40,
	IEEE80211_ELEMID_IBSSDFS	= 41,
	IEEE80211_ELEMID_ERP		= 42,
	IEEE80211_ELEMID_HTCAP_ANA	= 45,
	IEEE80211_ELEMID_RSN		= 48,
	IEEE80211_ELEMID_XRATES		= 50,
	IEEE80211_ELEMID_HTCAP		= 51,
	IEEE80211_ELEMID_HTINFO		= 52,
	IEEE80211_ELEMID_MDIE		= 54,
	IEEE80211_ELEMID_FTIE		= 55,
	IEEE80211_ELEMID_TIE		= 56,
	IEEE80211_ELEMID_RIC_DATA	= 57,
	IEEE80211_ELEMID_EXTCHANSWITCHANN = 60,	/* Fix this later as per ANA definition */
	IEEE80211_ELEMID_HTINFO_ANA	= 61,
	IEEE80211_ELEMID_SEC_CH_OFF	= 62,
    IEEE80211_ELEMID_BSS_AAC    = 67,
    IEEE80211_ELEMID_TIME_ADVT  = 69,
    IEEE80211_ELEMID_RRM_CAP    = 70,
    IEEE80211_ELEMID_TIME_ZONE  = 98,
    IEEE80211_ELEMID_IWKING     = 107,
    IEEE80211_ELEMID_ADVTPROT   = 108,
    IEEE80211_ELEMID_EBR        = 109,
    IEEE80211_ELEMID_QOSMAPSET  = 110,
    IEEE80211_ELEMID_ROAMCONS   = 111,
    IEEE80211_ELEMID_EMERALRTID = 112,
    IEEE80211_ELEMID_EXT_CAP    = 127,
	IEEE80211_ELEMID_TPC		= 150,
	IEEE80211_ELEMID_CCKM		= 156,
//UMAC_ACDEV
    IEEE80211_ELEMID_VHTCAP           = 191,
    IEEE80211_ELEMID_VHTOP            = 192,
	IEEE80211_ELEMID_VENDOR		= 221,	/* vendor private */
};

enum {
    IEEE80211_SUBELEMID_PMK_R1KH_ID  = 1,
    IEEE80211_SUBELEMID_GTK          = 2,
    IEEE80211_SUBELEMID_PMK_R0KH_ID  = 3,
};

#define IEEE80211_CHANSWITCHANN_BYTES 5
#define IEEE80211_EXTCHANSWITCHANN_BYTES 6
#define IEEE80211_SEC_CH_OFF_BYTES 3


struct ieee80211_tim_ie {
	u_int8_t	tim_ie;			/* IEEE80211_ELEMID_TIM */
	u_int8_t	tim_len;
	u_int8_t	tim_count;		/* DTIM count */
	u_int8_t	tim_period;		/* DTIM period */
	u_int8_t	tim_bitctl;		/* bitmap control */
	u_int8_t	tim_bitmap[1];		/* variable-length bitmap */
} __attribute__ ((packed));

struct ieee80211_country_ie {
	u_int8_t	ie;			/* IEEE80211_ELEMID_COUNTRY */
	u_int8_t	len;
	u_int8_t	cc[3];			/* ISO CC+(I)ndoor/(O)utdoor */
	struct {
		u_int8_t schan;			/* starting channel */
		u_int8_t nchan;			/* number channels */
		u_int8_t maxtxpwr;		/* tx power cap */
	} __attribute__ ((packed)) band[4];			/* up to 4 sub bands */
} __attribute__ ((packed));

/* 
 * 20/40 BSS coexistence ie
 */
struct ieee80211_ie_bss_coex {
        u_int8_t elem_id;
        u_int8_t elem_len;
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
        u_int8_t reserved1          : 1,
                 reserved2          : 1,
                 reserved3          : 1,
                 obss_exempt_grant  : 1,
                 obss_exempt_req    : 1,
                 ht20_width_req       : 1,
                 ht40_intolerant      : 1,
                 inf_request        : 1;
#else
        u_int8_t inf_request        : 1,
                 ht40_intolerant      : 1,
                 ht20_width_req       : 1,
                 obss_exempt_req    : 1,
                 obss_exempt_grant  : 1,
                 reserved3          : 1,
                 reserved2          : 1,
                 reserved1          : 1;
#endif
} __packed;

/*
 * 20/40 BSS intolerant channel report ie
 */
struct ieee80211_ie_intolerant_report {
        u_int8_t elem_id;
        u_int8_t elem_len;
        u_int8_t reg_class;
        u_int8_t chan_list[1];          /* variable-length channel list */
} __packed;
 
/*
 * 20/40 coext management action frame 
 */
struct ieee80211_action_bss_coex_frame {
        struct ieee80211_action                ac_header;
        struct ieee80211_ie_bss_coex           coex;
        struct ieee80211_ie_intolerant_report    chan_report;
} __packed;



#define IEEE80211_ELEMID_PWRCNSTR_LEN             1
#define IEEE80211_ELEMID_TPCREP_LEN               2
#define IEEE80211_ELEMID_BSS_AAC_LEN              6
#define IEEE80211_ELEMID_RRM_CAP_LEN              5
#define IEEE80211_ELEMID_BSS_LOAD_LEN             5
#define IEEE80211_ELEMID_EXT_CAP_LEN              8
#define IEEE80211_ELEMID_MDIE_LEN                 3
#define IEEE80211_ELEMID_FTIE_LEN                 98
#define IEEE80211_ELEMID_MSFT_LEN                 8 

#define IEEE80211_ELEMID_QUIET_LEN                6
#define IEEE80211_ELEMID_QUIET_PERIOD             200
#define IEEE80211_ELEMID_QUIET_DURATION           30
#define IEEE80211_ELEMID_QUIET_OFFSET             20

#define IEEE80211_ELEMID_MSFT_OUI_VAL1            0 // 0x00, 0x50, 0xF2 //
#define IEEE80211_ELEMID_MSFT_OUI_VAL2            80 // 0x00, 0x50, 0xF2 //
#define IEEE80211_ELEMID_MSFT_OUI_VAL3            242 // 0x00, 0x50, 0xF2 //
#define IEEE80211_ELEMID_MSFT_OUITYPE             17 // 0x11 //
#define IEEE80211_ELEMID_MSFT_COST                4  // 0x4 // 

struct ieee80211_quiet_ie {
        u_int8_t        ie;                 /* IEEE80211_ELEMID_QUIET */
        u_int8_t        len;
        u_int8_t        tbttcount;          /* quiet start */
        u_int8_t        period;             /* beacon intervals between quiets*/
        u_int8_t        duration;           /* TUs of each quiet*/
        u_int8_t        offset;             /* TUs of from TBTT of quiet start*/
} __packed;

struct ieee80211_tpc_ie {
        u_int8_t    ie;
        u_int8_t    len;
        u_int8_t    pwrlimit;
} __packed;

struct ieee80211_ie_hdr{
    u_int8_t            ie;
    u_int8_t            len;
} __packed;

struct ieee80211_action_csa {
    struct ieee80211_action    as_header;
    u_int8_t                   csa_ie[IEEE80211_EXTCHANSWITCHANN_BYTES];
} __packed;

/****************************************
 *************** Hotspot IE's ***********
 ****************************************/

#define IEEE80211_ELEMID_IE_BASE_LEN                2       /* Accounts for IE Type and Len */

#define IEEE80211_RC_LEN_1_MASK                     0x0F
#define IEEE80211_RC_LEN_2_MASK                     0xF0
#define IEEE80211_RC_LEN_2_OFFSET                   4

/***** Interworking IE  Definitions *****/
struct ieee80211_acc_nwk_parameterset {
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
    u_int8_t    uesa            : 1,    /* B7 - Unauthentication emergency services reachable */
                esr             : 1,    /* B6 - emergency services reachable */
                asra            : 1,    /* B5 - Additional Steps required for access */
                internet        : 1,    /* B4 - Internet */
                access_nwk_type : 4;    /* B0-B3 - access network type */
#else
    u_int8_t    access_nwk_type : 4,    /* B0-B3 - access network type */
                internet        : 1,    /* B4 - Internet */
                asra            : 1,    /* B5 - Additional Steps required for access */
                esr             : 1,    /* B6 - emergency services reachable */
                uesa            : 1;    /* B7 - Unauthentication emergency services reachable */
#endif
} __packed;

/* Venue Information Defintion */
struct ieee80211_venue_info_t {
    u_int8_t        venue_group;        /* B0 - B7  - Venue Group */
    u_int8_t        venue_type;         /* B8 - B15 - Venue Type */
} __packed;
#define IEEE80211_VENUE_INFO_LEN        (sizeof(struct ieee80211_venue_info_t))

struct ieee80211_iwking_ie_opt_info {
        struct ieee80211_venue_info_t           venue_info;
        u_int8_t                                hessid[IEEE80211_ADDR_LEN];
} __packed;

/* Interworking IE - 802.11u */
struct ieee80211_iwking_ie {
        u_int8_t                                ie;
        u_int8_t                                len;
        struct ieee80211_acc_nwk_parameterset   acc_nwk_options;
        u_int8_t                                optional_info[0];
} __packed;

#define DOT11U_ENABLED              0x01
#define DOT11U_HESSID_ENABLED       0x02
#define DOT11U_VENUE_ENABLED        0x04

#define IEEE80211_ELEMID_IWKING_LEN             (sizeof(struct ieee80211_iwking_ie) +       \
                                                 sizeof(struct ieee80211_iwking_ie_opt_info) - \
                                                 sizeof(struct ieee80211_ie_hdr) )
#define IWKING_IE_LEN_NO_OPT_PARAM              (sizeof(struct ieee80211_iwking_ie) - \
                                                 sizeof(struct ieee80211_ie_hdr))
#define IWKING_IE_LEN_OPT_PARAM_VENUE           (sizeof(struct ieee80211_iwking_ie) + \
                                                 sizeof(struct ieee80211_venue_info_t) - \
                                                 sizeof(struct ieee80211_ie_hdr))
#define IWKING_IE_LEN_OPT_PARAM_HESSID          (sizeof(struct ieee80211_iwking_ie) + \
                                                 IEEE80211_ADDR_LEN - \
                                                 sizeof(struct ieee80211_ie_hdr))
#define IWKING_IE_LEN_OPT_PARAM_ALL             IEEE80211_ELEMID_IWKING_LEN


/***** Hotspot 2.0 Indication Definitions *****/

#define IEEE80211_VAP_WPA2_ENT_ENABLED(vap) (vap->des_config.encryption & WIFI_ENCRYPTION_WPA2_ENT)

struct hs_2_ie_hotspot_cfg {
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
    u_int8_t    release_number          : 4,  /* B7-B4 - release number to identify HS2.0 Release Capability */
                reserved                : 1,  /* B3 - reserved for future use */
                anqp_domainid_present   : 1,  /* B2 - ANQP Domain ID Present flag */
                pps_moid_present        : 1,  /* B1 - PPS MO ID Present flag */
                dgaf_disabled           : 1;  /* B0 - DGAF Capability flag */
#else
    u_int8_t    dgaf_disabled           : 1,  /* B0 - DGAF Capability flag */
                pps_moid_present        : 1,  /* B1 - PPS MO ID Present flag */    
                anqp_domainid_present   : 1,  /* B2 - ANQP Domain ID Present flag */
                reserved                : 1,  /* B3 - reserved */
                release_number          : 4;  /* B4-B7 - release number to identify HS2.0 Release Capability*/
#endif
} __packed;

/*
 * OSENE
 */
struct ieee80211_ie_osen {
	u_int32_t	wpa_mcipher[1];	/* group key cipher: 00 0f ac 07 */
	u_int16_t	wpa_uciphercnt;	/* # unicast ciphers */
	u_int32_t	wpa_uciphers[8];/* unicast ciphers */
	u_int16_t	wpa_authselcnt;	/* akm suite count: 01 00 */
	u_int32_t	wpa_authsels[1];/* selectors: 50 6f 9a 01 */
	u_int16_t	wpa_caps;	/* 802.11i capabilities: 00 00 */
} __packed;

struct ieee80211_vendor_spec_ie {
    u_int8_t        ie;
    u_int8_t        len;
    u_int8_t        oi[3];
    u_int8_t        type;
} __packed;
#define VENDOR_SPEC_PAYLOAD_LEN     (sizeof(struct ieee80211_vendor_spec_ie) - sizeof(struct ieee80211_ie_hdr))     /* Payload starts after Len field */

/* Hotspot 2.0 Indication IE - WFA Specs */
struct ieee80211_vs_hs_2_ie {
    struct ieee80211_vendor_spec_ie     vs;
    struct hs_2_ie_hotspot_cfg          cfg;
} __packed;
#define IEEE80211_ELEMID_VENDOR_SPEC_HS_2_LEN       (VENDOR_SPEC_PAYLOAD_LEN + sizeof(struct hs_2_ie_hotspot_cfg))

/* Hotspot 2.0 OSEN - WFA Specs */
struct ieee80211_vs_osene {
    struct ieee80211_vendor_spec_ie     vs;
    struct ieee80211_ie_osen            osen;
} __packed;
#define IEEE80211_ELEMID_VENDOR_SPEC_OSEN_LEN       (VENDOR_SPEC_PAYLOAD_LEN + sizeof(struct ieee80211_ie_osen))


/***** Advertisement Protocol IE defintions *****/
enum {
    IEEE80211_ADVT_PROTOCOL_ANQP                        = 0,
    IEEE80211_ADVT_PROTOCOL_MIH_INFO_SERVICE            = 1,
    IEEE80211_ADVT_PROTOCOL_MIH_CMD_EVENT_SERVICES_CAP  = 2,
    IEEE80211_ADVT_PROTOCOL_EMER_ALERT_SYS              = 3,
    IEEE80211_ADVT_PROTOCOL_VENDOR_SPECIFIC             = 221,
};

/* Advertisement Tuple definition */
struct ieee80211_advt_tuple {
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
    u_int8_t        pame_bi:1,                         /* B7 - Pre-Association Message Exchange - Bssid independent */
                    query_rsp_len_limit:7;             /* B0 - B6  - Query response length limit */
#else
    u_int8_t        query_rsp_len_limit:7,             /* B0 - B6  - Query response length limit */
                    pame_bi:1;                         /* B7 - Pre-Association Message Exchange - Bssid independent */
#endif
    u_int8_t        prot_id;                           /* B8 - B15 - advertisement Protocl Id */
} __packed;


/* Advertisement Protocol IE - 802.11u */
#define IEEE80211_ELEMID_ADVT_PROT_LEN              2       /* + (num_advt_prot_tuples * 2)*/
struct ieee80211_advt_prot_ie {
    u_int8_t                        ie;
    u_int8_t                        len;
    struct ieee80211_advt_tuple     advt_tuple;
} __packed;


/***** Roaming Consortium Definitions *****/
/* Roaming Consortium IE  - 802.11u */
#define IEEE80211_ROAM_CONS_MAX_OIS                 3
#define IEEE80211_ELEMID_ROAM_CONS_FIXED_LEN        2       /* Num ANQP + OI1 and OI2 Lengths */
#define IEEE80211_ELEMID_ROAM_CONS_VARIABLE_LEN     (5 * IEEE80211_ROAM_CONS_MAX_OIS)    /* Could increase based on no of OI's */
#define IEEE80211_ELEMID_ROAM_CONS_LEN              (IEEE80211_ELEMID_ROAM_CONS_FIXED_LEN + IEEE80211_ELEMID_ROAM_CONS_VARIABLE_LEN)
struct ieee80211_roaming_consortium_ie {
    u_int8_t         ie;
    u_int8_t         len;
    u_int8_t         num_anqp_ois;
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
    u_int8_t        len_oi_2:4,                 /* B0-B3 - OI2 Length */
                    len_oi_1:4;                 /* B4-B7 - OI1 Length */
#else
    u_int8_t        len_oi_1:4,                 /* B0-B3 - OI1 Length */
                    len_oi_2:4;                 /* B4-B7 - OI2 Length */
#endif

    u_int8_t         oi_1[5];
    u_int8_t         oi_2[5];
    u_int8_t         oi_3[5];
} __packed;

/***** Time Advertisement Definitions *****/
enum {
    TIME_ADVT_CAP_NO_STD_EXT_TS = 0,
    TIME_ADVT_CAP_UTC_OFFSET    = 1,
    TIME_ADVT_CAP_RSVD                  /* 2 - 255 */
};

/* Time Advertisement IE - 802.11p */
#define IEEE80211_ELEMID_TIME_ADVT_LEN              32
struct ieee80211_time_advt_ie {
    u_int8_t         ie;
    u_int8_t         len;
    u_int8_t         timing_cap;
    u_int8_t         time_advt_pyld[0];
} __packed;

/***** Time Zone Definitions *****/
/* Time Zone Element  - 802.11v */
#define MAX_TZ_LEN      64
#define IEEE80211_ELEMID_TIME_ZONE_LEN              MAX_TZ_LEN
struct ieee80211_time_zone_ie {
    u_int8_t         ie;
    u_int8_t         len;
    u_int8_t         tz[0];
} __packed;

/***** P2P Definitions *****/
enum {
    IEEE80211_P2P_ATTR_STATUS = 0,
    IEEE80211_P2P_ATTR_MINOR_REASON_CODE,
    IEEE80211_P2P_ATTR_P2P_CAP,
    IEEE80211_P2P_ATTR_P2P_DEV_ID,
    IEEE80211_P2P_ATTR_GRP_OWNER_INTENT,
    IEEE80211_P2P_ATTR_CFG_TIMEOUT,
    IEEE80211_P2P_ATTR_LISTEN_CHANNEL,
    IEEE80211_P2P_ATTR_GRP_BSSID,
    IEEE80211_P2P_ATTR_EXT_LISTEN_TIMING,
    IEEE80211_P2P_ATTR_INT_INTF_ADDR,
    IEEE80211_P2P_ATTR_MANAGEABILITY,
    IEEE80211_P2P_ATTR_CHANNEL_LIST,
    IEEE80211_P2P_ATTRR_NOTICE_OF_ABSENCE,
    IEEE80211_P2P_ATTR_DEV_INFO,
    IEEE80211_P2P_ATTR_GRP_INFO,
    IEEE80211_P2P_ATTR_INTF,
    IEEE80211_P2P_ATTR_OP_CHANNEL,
    IEEE80211_P2P_ATTR_INV_FLAGS,
    IEEE80211_P2P_ATTR_RSVD_1,
    IEEE80211_P2P_ATTR_VSA,
    IEEE80211_P2P_ATTR_RSVD_2
};

/* P2P Attribute - Capability Payload */
struct ieee80211_p2p_cap_payload {
    u_int8_t        dev_cap;
    u_int8_t        grp_cap;
} __packed;

/* P2P Attribute - Device ID Payload */
struct ieee80211_p2p_dev_id_payload {
    u_int8_t        addr[IEEE80211_ADDR_LEN];
} __packed;

/* P2P Attribute - Device Manageability Payload */
struct ieee80211_p2p_mgab_payload {
#if _BYTE_ORDER == _ATH_BIG_ENDIAN
    u_int8_t        rsvd:5,                     /* B3-B7 - Reserved */
                    coexistence_opt:1,          /* B2 - Coexistence Optional */
                    cross_conn_permitted:1,     /* B1 - Cross Connection Permitted */
                    dev_mgmt:1;                 /* B0 - P2P Device Mgmt */
#else
    u_int8_t        dev_mgmt:1,                 /* B0 - P2P Device Mgmt */
                    cross_conn_permitted:1,     /* B1 - Cross Connection Permitted */
                    coexistence_opt:1,          /* B2 - Coexistence Optional */
                    rsvd:5;                     /* B3-B7 - Reserved */
#endif
} __packed;

struct ieee80211_vs_p2p_attr {
    u_int8_t        id;
    u_int16_t       len;
    u_int8_t        value[0];
} __packed;

#define IEEE80211_ELEMID_P2P_ATTR_LEN       (sizeof(struct ieee80211_vs_p2p_attr) + \
                                             sizeof(struct ieee80211_p2p_cap_payload) + \
                                             sizeof(struct ieee80211_p2p_dev_id_payload) + \
                                             sizeof(struct ieee80211_p2p_mgab_payload))
#define IEEE80211_ELEMID_VENDOR_SPEC_P2P_LEN  (VENDOR_SPEC_PAYLOAD_LEN + IEEE80211_ELEMID_P2P_ATTR_LEN)

/* P2P IE */
struct ieee80211_vs_p2p_ie {
    struct ieee80211_vendor_spec_ie     vs;
    u_int8_t                            attr[0];
} __packed;


/*
 * 802.11 BSS AAC IE
 */
struct ieee80211_bss_aac_elem {
    u_int8_t                        eid;            /* element ID */
    u_int8_t                        len;            /* length in bytes */
    u_int16_t                       aac_bitmask;    /* bitmask */
    u_int16_t                       aac_vi;         /* aac for video */
    u_int16_t                       aac_vo;         /* aac for voice */
} __packed;

/*
 * 802.11 BSS Load IE
 */
struct ieee80211_bss_load_elem {
    u_int8_t                        eid;            /* element ID */
    u_int8_t                        len;            /* length in bytes */
    u_int16_t                       station_count;  
    u_int8_t                        chan_util;         
    u_int16_t                       aac;         
} __packed;

/*
 * 802.11 Quiet IE
 */
struct ieee80211_quiet_elem {
    u_int8_t                        eid;            /* element ID */
    u_int8_t                        len;            /* length in bytes */
    u_int8_t                        qcount;  
    u_int8_t                        qperiod;         
    u_int16_t                       qduration;         
    u_int16_t                       qoffset;         
} __packed;

/*
 * 802.11r Mobility Domain IE
 */
struct ieee80211_mob_domain_elem {
    u_int8_t       eid;         /* element ID */
    u_int8_t       len;         /* length in bytes */
    u_int16_t      domain_id;         
    u_int8_t       ft_cap;
} __packed;

/*
 * MSFT WISPr IE
 */
struct ieee80211_msft_wispr_elem {
    u_int8_t       eid;         /* element ID */
    u_int8_t       len;         /* length in bytes */
    u_int8_t       oui[3];      /* Microsoft Vendor Specific OUI */
    u_int8_t       oui_type;    /* Microsoft vendor Specific OUI Type */
    u_int32_t      cost_attr;    /* Cost value */              
} __packed;

#define IEEE80211_CHALLENGE_LEN		128

#define IEEE80211_SUPPCHAN_LEN		26

#define	IEEE80211_RATE_BASIC		0x80
#define	IEEE80211_RATE_HT		0x80
#define	IEEE80211_RATE_VAL		0x7f

/* EPR information element flags */
#define	IEEE80211_ERP_NON_ERP_PRESENT	0x01
#define	IEEE80211_ERP_USE_PROTECTION	0x02
#define	IEEE80211_ERP_LONG_PREAMBLE	0x04

/* Atheros private advanced capabilities info */
#define	ATHEROS_CAP_TURBO_PRIME		0x01
#define	ATHEROS_CAP_COMPRESSION		0x02
#define	ATHEROS_CAP_FAST_FRAME		0x04
/* bits 3-6 reserved */
#define	ATHEROS_CAP_BOOST		0x80

#define	ATH_OUI			0x7f0300		/* Atheros OUI */
#define	ATH_OUI_TYPE		0x01
#define	ATH_OUI_SUBTYPE		0x01
#define ATH_OUI_VERSION		0x00
#define	ATH_OUI_TYPE_XR		0x03
#define	ATH_OUI_VER_XR		0x01
#define	ATH_OUI_EXTCAP_TYPE	0x04	/* Atheros Extended Cap Type */
#define	ATH_OUI_EXTCAP_SUBTYPE	0x01	/* Atheros Extended Cap Sub-type */
#define ATH_OUI_EXTCAP_VERSION	0x00    /* Atheros Extended Cap Version */

#define	WPA_OUI			0xf25000
#define	WPA_OUI_TYPE		0x01
#define	WPA_VERSION		1		/* current supported version */

#define WSC_OUI                 0x0050f204

#define	WPA_CSE_NULL		0x00
#define	WPA_CSE_WEP40		0x01
#define	WPA_CSE_TKIP		0x02
#define	WPA_CSE_CCMP		0x04
#define	WPA_CSE_WEP104		0x05

#define	WPA_ASE_NONE		    0x00
#define	WPA_ASE_8021X_UNSPEC	0x01
#define	WPA_ASE_8021X_PSK	    0x02
#define	WPA_ASE_FT_8021X	    0x03
#define	WPA_ASE_FT_PSK	        0x04

#define	WPA_ASE_FT_AUTH_8021X	0x10
#define	WPA_ASE_FT_AUTH_PSK	    0x20

#define	RSN_OUI			0xac0f00
#define	RSN_VERSION		1		/* current supported version */

#define	RSN_CSE_NULL		0x00
#define	RSN_CSE_WEP40		0x01
#define	RSN_CSE_TKIP		0x02
#define	RSN_CSE_WRAP		0x03
#define	RSN_CSE_CCMP		0x04
#define	RSN_CSE_WEP104		0x05
#define RSN_CSE_AES_128_CMAC	0x06
#define RSN_CSE_NOTALLOWED  0x07

#define	RSN_ASE_NONE		    0x00
#define	RSN_ASE_8021X_UNSPEC	0x01
#define	RSN_ASE_8021X_PSK	    0x02
#define	RSN_ASE_FT_8021X	    0x03
#define	RSN_ASE_FT_PSK	        0x04
#define RSN_ASE_256SHA_8021X	0x05
#define RSN_ASE_256SHA_PSK	0x06

#define	RSN_ASE_FT_AUTH_8021X	0x10
#define	RSN_ASE_FT_AUTH_PSK	    0x20
/* 802.11W */
#define RSN_ASE_AKM5		0x40
#define RSN_ASE_AKM6		0x80

#define	RSN_ASE_8021X_SUITEB_1	0x01
#define	RSN_ASE_8021X_SUITEB_2	0x02
#define SUITEB_8021X_TYPE_1	0x04
#define SUITEB_8021X_TYPE_2	0x08


#define	RSN_CAP_PREAUTH		0x01
/* 802.11W */
#define	RSN_CAP_MFP_CAPABLE	0x80
#define	RSN_CAP_MFP_REQUIRED	0x40

#define SUITEB_CSE_GCM128	0x1
#define SUITEB_CSE_GCM256	0x2

#define	WME_OUI			0xf25000
#define	WME_OUI_TYPE		0x02
#define	WME_INFO_OUI_SUBTYPE	0x00
#define	WME_PARAM_OUI_SUBTYPE	0x01
#define	WME_VERSION		1

/* WME stream classes */
#define	WME_AC_BE	0		/* best effort */
#define	WME_AC_BK	1		/* background */
#define	WME_AC_VI	2		/* video */
#define	WME_AC_VO	3		/* voice */

/* WME AC comparison */
#define WME_AC_GT(a,b) (((a)>(b) && ((a)!=WME_AC_BK && (b)!=WME_AC_BE)) \
                             || ((a)==WME_AC_BE && (b)==WME_AC_BK))

#define SUITEB_OUI			0x342204
#define SUITEB_EID_LEN	20
/*
 * AUTH management packets
 *
 *	octet algo[2]
 *	octet seq[2]
 *	octet status[2]
 *	octet chal.id
 *	octet chal.length
 *	octet chal.text[253]
 */

typedef u_int8_t *ieee80211_mgt_auth_t;

#define	IEEE80211_AUTH_ALGORITHM(auth) \
	((auth)[0] | ((auth)[1] << 8))
#define	IEEE80211_AUTH_TRANSACTION(auth) \
	((auth)[2] | ((auth)[3] << 8))
#define	IEEE80211_AUTH_STATUS(auth) \
	((auth)[4] | ((auth)[5] << 8))

#define	IEEE80211_AUTH_ALG_OPEN		0x0000
#define	IEEE80211_AUTH_ALG_SHARED	0x0001
#define	IEEE80211_AUTH_ALG_FT		0x0002
#define	IEEE80211_AUTH_ALG_LEAP		0x0080

enum {
	IEEE80211_AUTH_OPEN_REQUEST		= 1,
	IEEE80211_AUTH_OPEN_RESPONSE		= 2,
};

enum {
	IEEE80211_AUTH_SHARED_REQUEST		= 1,
	IEEE80211_AUTH_SHARED_CHALLENGE		= 2,
	IEEE80211_AUTH_SHARED_RESPONSE		= 3,
	IEEE80211_AUTH_SHARED_PASS		= 4,
};

/*
 * Reason codes
 *
 * Unlisted codes are reserved
 */

enum {
	IEEE80211_REASON_UNSPECIFIED		= 1,
	IEEE80211_REASON_AUTH_EXPIRE		= 2,
	IEEE80211_REASON_AUTH_LEAVE		= 3,
	IEEE80211_REASON_ASSOC_EXPIRE		= 4,
	IEEE80211_REASON_ASSOC_TOOMANY		= 5,
	IEEE80211_REASON_NOT_AUTHED		= 6,
	IEEE80211_REASON_NOT_ASSOCED		= 7,
	IEEE80211_REASON_ASSOC_LEAVE		= 8,
	IEEE80211_REASON_ASSOC_NOT_AUTHED	= 9,

	IEEE80211_REASON_RSN_REQUIRED		= 11,
	IEEE80211_REASON_RSN_INCONSISTENT	= 12,
	IEEE80211_REASON_IE_INVALID		= 13,
	IEEE80211_REASON_MIC_FAILURE		= 14,

	IEEE80211_STATUS_SUCCESS		= 0,
	IEEE80211_STATUS_UNSPECIFIED		= 1,
	IEEE80211_STATUS_CAPINFO		= 10,
	IEEE80211_STATUS_NOT_ASSOCED		= 11,
	IEEE80211_STATUS_OTHER			= 12,
	IEEE80211_STATUS_ALG			= 13,
	IEEE80211_STATUS_SEQUENCE		= 14,
	IEEE80211_STATUS_CHALLENGE		= 15,
	IEEE80211_STATUS_TIMEOUT		= 16,
	IEEE80211_STATUS_TOOMANY		= 17,
	IEEE80211_STATUS_BASIC_RATE		= 18,
	IEEE80211_STATUS_SP_REQUIRED		= 19,
	IEEE80211_STATUS_PBCC_REQUIRED		= 20,
	IEEE80211_STATUS_CA_REQUIRED		= 21,
	IEEE80211_STATUS_TOO_MANY_STATIONS	= 22,
	IEEE80211_STATUS_RATES			= 23,
	IEEE80211_STATUS_SHORTSLOT_REQUIRED	= 25,
	IEEE80211_STATUS_DSSSOFDM_REQUIRED	= 26,
        IEEE80211_STATUS_TRY_LATER		= 30,
	IEEE80211_STATUS_REFUSED		= 37,
	IEEE80211_STATUS_INVALID_PARAM		= 38,
};

#define	IEEE80211_WEP_KEYLEN		5	/* 40bit */
#define	IEEE80211_WEP_IVLEN		3	/* 24bit */
#define	IEEE80211_WEP_KIDLEN		1	/* 1 octet */
#define	IEEE80211_WEP_CRCLEN		4	/* CRC-32 */
#define	IEEE80211_WEP_NKID		4	/* number of key ids */

/*
 * 802.11i defines an extended IV for use with non-WEP ciphers.
 * When the EXTIV bit is set in the key id byte an additional
 * 4 bytes immediately follow the IV for TKIP.  For CCMP the
 * EXTIV bit is likewise set but the 8 bytes represent the
 * CCMP header rather than IV+extended-IV.
 */
#define	IEEE80211_WEP_EXTIV		0x20
#define	IEEE80211_WEP_EXTIVLEN		4	/* extended IV length */
#define	IEEE80211_WEP_MICLEN		8	/* trailing MIC */
#define IEEE80211_BIP_MMIELEN       18

#define	IEEE80211_CRC_LEN		4

/*
 * Maximum acceptable MTU is:
 *	IEEE80211_MAX_LEN - WEP overhead - CRC -
 *		QoS overhead - RSN/WPA overhead
 * Min is arbitrarily chosen > IEEE80211_MIN_LEN.  The default
 * mtu is Ethernet-compatible; it's set by ether_ifattach.
 */
#define	IEEE80211_MTU_MAX		2290
#define	IEEE80211_MTU_MIN		32

#define	IEEE80211_MAX_LEN		(2300 + IEEE80211_CRC_LEN + \
    (IEEE80211_WEP_IVLEN + IEEE80211_WEP_KIDLEN + IEEE80211_WEP_CRCLEN))
#define IEEE80211_MAX_MPDU_LEN          (3840 + IEEE80211_CRC_LEN + \
    (IEEE80211_WEP_IVLEN + IEEE80211_WEP_KIDLEN + IEEE80211_WEP_CRCLEN))
#define	IEEE80211_ACK_LEN \
	(sizeof(struct ieee80211_frame_ack) + IEEE80211_CRC_LEN)
#define	IEEE80211_MIN_LEN \
	(sizeof(struct ieee80211_frame_min) + IEEE80211_CRC_LEN)

/* An 802.11 data frame can be one of three types:
1. An unaggregated frame: The maximum length of an unaggregated data frame is 2324 bytes + headers. 
2. A data frame that is part of an AMPDU: The maximum length of an AMPDU may be upto 65535 bytes, but data frame is limited to 2324 bytes + header. 
3. An AMSDU: The maximum length of an AMSDU is eihther 3839 or 7095 bytes. 
The maximum frame length supported by hardware is 4095 bytes. 
A length of 3839 bytes is chosen here to support unaggregated data frames, any size AMPDUs and 3839 byte AMSDUs. 
*/
#define	IEEE80211N_MAX_FRAMELEN	 3839	
#define IEEE80211N_MAX_LEN (IEEE80211N_MAX_FRAMELEN + IEEE80211_CRC_LEN + \
    (IEEE80211_WEP_IVLEN + IEEE80211_WEP_KIDLEN + IEEE80211_WEP_CRCLEN))

#define IEEE80211_TX_CHAINMASK_MIN	1
#define IEEE80211_TX_CHAINMASK_MAX	7

#define IEEE80211_RX_CHAINMASK_MIN	1
#define IEEE80211_RX_CHAINMASK_MAX	7

/*
 * The 802.11 spec says at most 2007 stations may be
 * associated at once.  For most AP's this is way more
 * than is feasible so we use a default of 128.  This
 * number may be overridden by the driver and/or by
 * user configuration.
 */
#define	IEEE80211_AID_MAX		2007
#define	IEEE80211_AID_DEF		256

#define	IEEE80211_AID(b)	((b) &~ 0xc000)

/* 
 * RTS frame length parameters.  The default is specified in
 * the 802.11 spec.  The max may be wrong for jumbo frames.
 */
#define	IEEE80211_RTS_DEFAULT		512
#define	IEEE80211_RTS_MIN		1
#define	IEEE80211_RTS_MAX		2346

/* 
 * Regulatory extention identifier for country IE.
 */
#define IEEE80211_REG_EXT_ID		201

/* Define the BSS Available Admission Capacity Bitmask for AC_VO */
#define IEEE80211_BSS_AAC_BIT_AC_VO     0x0800
#define IEEE80211_BSS_AAC_BIT_AC_VI     0x0400

static __inline
int ieee80211_is4addr(const void *frame_wh)
{
    if((((struct ieee80211_frame*)frame_wh)->i_fc[1] & IEEE80211_FC1_DIR_MASK) == 
       IEEE80211_FC1_DIR_DSTODS) {
        return 1;
    }
    return 0;
}

static __inline
const u_int8_t *ieee80211_qos3or4_field(const struct ieee80211_qosframe *qwh)
{
    if(ieee80211_is4addr(qwh)) {
        return ((struct ieee80211_qosframe_addr4*)qwh)->i_qos;
    }
    return qwh->i_qos;
}

static __inline
u_int8_t ieee80211_qos3or4_get_tid(const struct ieee80211_qosframe *qwh)
{
    const u_int8_t *qos = ieee80211_qos3or4_field(qwh);
    return (qos[0] & IEEE80211_QOS_TID);
}

static __inline
unsigned short ieee80211_iptos_to_wme(unsigned short iptos)
{
    return TID_TO_WME_AC(iptos >> 5);
}

#define IEEE80211_SET_VLAN_TCI(_skb, _vtag) do {      \
        _skb->aruba_vlan_tci = _vtag;                       \
    } while(0)
#define IEEE80211_GET_VLAN_TCI(_skb) (_skb->aruba_vlan_tci)

/* Time Advertisement Len = Time Value Len + Time Error Len + Time Update Counter Len */
#define IEEE80211_TIME_ADVT_LEN(cap_type, len) \
{                               \
    if(cap_type == TIME_ADVT_CAP_NO_STD_EXT_TS)         \
    {                                                   \
        len = 0 + 0 + 0;                                \
    }                                                   \
    else if (cap_type == TIME_ADVT_CAP_UTC_OFFSET)      \
    {                                                   \
        len = sizeof(uint16_t) + sizeof(uint16_t) + 0;  \
    }                                                   \
    else                                                \
    {                                                   \
        len = 10 + sizeof(uint16_t) + sizeof(uint16_t); \
    }                                                   \
}

//UMAC_ACDEV
/* VHT capability flags */
/* B0-B1 Maximum MPDU Length */
#define IEEE80211_VHTCAP_MAX_MPDU_MASK         0x00000003
#define IEEE80211_VHTCAP_MAX_MPDU_LEN_3839     0x00000000 /* A-MSDU Length 3839 octets */
#define IEEE80211_VHTCAP_MAX_MPDU_LEN_7935     0x00000001 /* A-MSDU Length 7991 octets */
#define IEEE80211_VHTCAP_MAX_MPDU_LEN_11454    0x00000002 /* A-MSDU Length 11454 octets */

/* B2-B3 Supported Channel Width */
#define IEEE80211_VHTCAP_SUP_CHAN_WIDTH_80     0x00000000 /* Does not support 160 or 80+80 */
#define IEEE80211_VHTCAP_SUP_CHAN_WIDTH_160    0x00000004 /* Supports 160 */
#define IEEE80211_VHTCAP_SUP_CHAN_WIDTH_80_160 0x00000008 /* Support both 160 or 80+80 */
#define IEEE80211_VHTCAP_SUP_CHAN_WIDTH_S      2          /* B2-B3 */

#define IEEE80211_VHTCAP_RX_LDPC             0x00000010 /* B4 RX LDPC */
#define IEEE80211_VHTCAP_SHORTGI_80          0x00000020 /* B5 Short GI for 80MHz */
#define IEEE80211_VHTCAP_SHORTGI_160         0x00000040 /* B6 Short GI for 160 and 80+80 MHz */
#define IEEE80211_VHTCAP_TX_STBC             0x00000080 /* B7 Tx STBC */

#define IEEE80211_VHTCAP_RX_STBC             0x00000700 /* B8-B10 Rx STBC */
#define IEEE80211_VHTCAP_RX_STBC_S           8

#define IEEE80211_VHTCAP_SU_BFORMER          0x00000800 /* B11 SU Beam former capable */
#define IEEE80211_VHTCAP_SU_BFORMEE          0x00001000 /* B12 SU Beam formee capable */
#define IEEE80211_VHTCAP_BF_MAX_ANT          0x0000E000 /* B13-B15 Compressed steering number of
                                                         * beacomformer Antennas supported */
#define IEEE80211_VHTCAP_BF_MAX_ANT_S        13

#define IEEE80211_VHTCAP_SOUND_DIMENSIONS    0x00070000 /* B16-B18 Sounding Dimensions */
#define IEEE80211_VHTCAP_SOUND_DIMENSIONS_S  16

#define IEEE80211_VHTCAP_MU_BFORMER          0x00080000 /* B19 MU Beam Former */
#define IEEE80211_VHTCAP_MU_BFORMEE          0x00100000 /* B20 MU Beam Formee */
#define IEEE80211_VHTCAP_TXOP_PS             0x00200000 /* B21 VHT TXOP PS */
#define IEEE80211_VHTCAP_PLUS_HTC_VHT        0x00400000 /* B22 +HTC-VHT capable */

#define IEEE80211_VHTCAP_MAX_AMPDU_LEN_FACTOR  13
#define IEEE80211_VHTCAP_MAX_AMPDU_LEN_EXP   0x03800000 /* B23-B25 maximum AMPDU Length Exponent */
#define IEEE80211_VHTCAP_MAX_AMPDU_LEN_EXP_S 23

#define IEEE80211_VHTCAP_LINK_ADAPT          0x18000000 /* B26-B27 VHT Link Adaptation capable */
#define IEEE80211_VHTCAP_LINK_ADAPT_S        26
#define IEEE80211_VHTCAP_RESERVED            0xF0000000 /* B28-B31 Reserved */

/*
 * 802.11ac VHT Capability IE
 */
struct ieee80211_ie_vhtcap {
        u_int8_t    elem_id;
        u_int8_t    elem_len;
        u_int32_t   vht_cap_info;
        u_int16_t   rx_mcs_map;          /* B0-B15 Max Rx MCS for each SS */
        u_int16_t   rx_high_data_rate;   /* B16-B28 Max Rx data rate,
                                            Note:  B29-B31 reserved */
        u_int16_t   tx_mcs_map;          /* B32-B47 Max Tx MCS for each SS */
        u_int16_t   tx_high_data_rate;   /* B48-B60 Max Tx data rate,
                                            Note: B61-B63 reserved */
} __packed;


/* VHT Operation  */
#define IEEE80211_VHTOP_CHWIDTH_2040      0 /* 20/40 MHz Operating Channel */
#define IEEE80211_VHTOP_CHWIDTH_80        1 /* 80 MHz Operating Channel */
#define IEEE80211_VHTOP_CHWIDTH_160       2 /* 160 MHz Operating Channel */
#define IEEE80211_VHTOP_CHWIDTH_80_80     3 /* 80 + 80 MHz Operating Channel */

/*
 * 802.11ac VHT Operation IE
 */
struct ieee80211_ie_vhtop {
        u_int8_t    elem_id;
        u_int8_t    elem_len;
        u_int8_t    vht_op_chwidth;              /* BSS Operational Channel width */
        u_int8_t    vht_op_ch_freq_seg1;         /* Channel Center frequency */
        u_int8_t    vht_op_ch_freq_seg2;         /* Channel Center frequency applicable
                                                  * for 80+80MHz mode of operation */ 
        u_int16_t   vhtop_basic_mcs_set;         /* Basic MCS set */
} __packed;
#endif /* _NET80211_IEEE80211_H_ */
