/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set ts=4 sw=4 expandtab: */
#ifndef __aruba_inventory_h__
#define __aruba_inventory_h__
/*
 * Copyright (C) 2002-2011 by Aruba Networks, Inc.
 * All Rights Reserved.
 * 
 * This software is an unpublished work and is protected by copyright and 
 * trade secret law.  Unauthorized copying, redistribution or other use of 
 * this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */

#include <asm/types.h>

#define INVENT_MAX_CARDS    8
#define INVENT_MAX_TLVS     12
#define INVENT_MAX_DATA     96
#if defined(AP_PLATFORM) || defined(CONFIG_APBOOT)
#define INVENT_MAX_INVENTORIES 1
#endif
#ifndef INVENT_MAX_INVENTORIES
#define INVENT_MAX_INVENTORIES 128
#endif

typedef ssize_t (*invent_read_func)(__u8 *, size_t);
typedef ssize_t (*invent_write_func)(__u8 *, size_t);

typedef struct invent_hdr_t {
    __u32 ih_magic;
    __u16 ih_len;     /* includes header */
    __u8  ih_format;
    __u8  ih_pad;
    __u32 ih_crc;     /* includes header */
} invent_hdr_t;

typedef struct invent_tlv_t {
    __u16  it_type;
    __u16  it_len;    /* includes header */
    __u8   it_data[0];
} invent_tlv_t;

typedef struct invent_card_t {
    invent_tlv_t ic_hdr;
    __u16        ic_num;
    __u16        ic_parent;
    invent_tlv_t ic_tlvs[0];
} invent_card_t;

extern int invent_setup(__u8 *, unsigned, invent_read_func, invent_write_func, void **);
extern int invent_init(void *);
extern int invent_save(void *);
extern int invent_set_card(void *, unsigned, unsigned, unsigned);
extern int invent_del_card(void *, unsigned);
extern int invent_add_tlv(void *, unsigned, unsigned, __u8 *, unsigned);
extern int invent_remove_tlv(void *, unsigned, unsigned);
extern char *invent_error(int);
extern void invent_dump(void *);
extern void invent_mfginfo(void *);
extern int invent_get_card_tlv(void *, unsigned card, unsigned tag, __u8 *val, unsigned *len);
extern int invent_adopt_inventory(void *, void *, unsigned, unsigned);
extern int invent_orphan_inventory(void *);
extern int invent_delete_inventory(void *);
#if defined (__U_BOOT__)
#if defined(CONFIG_GRAPPA_CPBOOT) || defined(CONFIG_GROZDOVA) || defined(CONFIG_OUZO) || defined(CONFIG_PALINKA_CPBOOT) || defined(CONFIG_AMARULA_CPBOOT)|| defined(CONFIG_OUZOPLUS_CPBOOT) || defined(CONFIG_MILAGRO_CPBOOT)
extern int check_inventory(int inv);
extern int get_inventory_index(void *hdl);
#endif
#endif
extern int inventory_init;
extern __u16 invent_getshort(void *);
extern __u32 invent_getlong(void *);

#define    INVENT_OK                 0
#define    INVENT_ERR_BADCRC        -1
#define    INVENT_ERR_BADMAGIC      -2
#define    INVENT_ERR_BADFORMAT     -3
#define    INVENT_ERR_NO_FREE_TLVS  -4
#define    INVENT_ERR_NO_ROOM       -5
#define    INVENT_ERR_NO_CARD       -6
#define    INVENT_ERR_TOO_MANY      -7
#define    INVENT_ERR_NOT_FOUND     -8
#define    INVENT_ERR_EXISTS        -9

#define INVENT_TLV_CARD                 0x0001
#define INVENT_TLV_WIRED_MAC            0x0002
#define INVENT_TLV_WIRED_MAC_COUNT      0x0003
#define INVENT_TLV_WIRELESS_MAC         0x0004
#define INVENT_TLV_WIRELESS_MAC_COUNT   0x0005
#define INVENT_TLV_COUNTRY_CODE         0x0006
#define INVENT_TLV_OEM                  0x0007
#define INVENT_TLV_FIPS                 0x0008
#define INVENT_TLV_TEST_STATION         0x0009
#define INVENT_TLV_OEM_SN               0x000a

#define INVENT_TLV_CARD_TYPE            0x0101
#define INVENT_TLV_CARD_ASSY            0x0102
#define INVENT_TLV_CARD_SN              0x0103
#define INVENT_TLV_CARD_DATE            0x0104
#define INVENT_TLV_CARD_MAJOR_REV       0x0105
#define INVENT_TLV_CARD_MINOR_REV       0x0106
#define INVENT_TLV_CARD_VARIANT         INVENT_TLV_CARD_MINOR_REV
/* Service Tag for Dell OEM */
#define INVENT_TLV_CARD_ST              0x0107
/* radio card serial number */
#define INVENT_TLV_CARD_RADIO0_SN       0x0108
#define INVENT_TLV_CARD_RADIO1_SN       0x0109
#define INVENT_TLV_CARD_MODEL           0x010a
#define INVENT_TLV_CARD_MAC             0x010b
#define INVENT_TLV_CARD_INTERNAL_MAC    0x010c
/* MAX EIRP for 2G/5G radios */
#define INVENT_TLV_CARD_MAX_EIRP        0x010d

#define INVENT_TLV_CARD_LICMODEL        0x0108

#define INVENT_TLV_END                  0xffff

#define INVENT_CARD_TYPE_BASE       0x0001
#define INVENT_CARD_TYPE_CPU        0x0002
#define INVENT_CARD_TYPE_RADIO      0x0003
#define INVENT_CARD_TYPE_CHASSIS    0x0004
#define INVENT_CARD_TYPE_LINECARD   0x0005
#define INVENT_CARD_TYPE_FABRIC     0x0006
#define INVENT_CARD_TYPE_ANTENNA    0x0007
#define INVENT_CARD_TYPE_SYSTEM     0x0008
#define INVENT_CARD_TYPE_POE        0x0009
#define INVENT_CARD_TYPE_FAN        0x000a
#define INVENT_CARD_TYPE_PSU        0x000b
#define INVENT_CARD_TYPE_UPLINK     0x000c
#define INVENT_CARD_TYPE_XCVR       0x000d
#define INVENT_CARD_TYPE_INTF       0x000e
#define INVENT_CARD_TYPE_CONSOLE    0x000f
#define INVENT_CARD_TYPE_DPI        0x0010
#define INVENT_CARD_TYPE_SC         0x0011
#define INVENT_CARD_TYPE_BP         0x0012
#define INVENT_CARD_TYPE_POWER_BOARD 0x0013
#define INVENT_CARD_TYPE_CM         0x0014

#define INVENT_NO_PARENT            0xffff

#endif /* __aruba_inventory_h__ */
