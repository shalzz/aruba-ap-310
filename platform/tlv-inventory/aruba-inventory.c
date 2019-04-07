/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set ts=4 sw=4 expandtab: */
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

#if defined(__KERNEL__) && !defined(__U_BOOT__)
//
// Linux kernel
//
#include <linux/kernel.h>
#include <asm/string.h>
#define printf printk

#elif defined (__U_BOOT__)
//
// U-boot
//
#include <config.h>
#if !defined(CONFIG_APBOOT) && !defined(NLM_HAL_UBOOT)
#include "boot_flash.h"
#endif
#include <common.h>
#include "exports.h"
#include <environment.h>
//#include <mips-exts.h>  // reset command
#include "linux/types.h"
#include "config.h"
#include "linux/string.h"
#include "linux/ctype.h"
#if defined(CONFIG_ARUBA_CTRL_CPBOOT)
#include <vsprintf.h>
#endif
DECLARE_GLOBAL_DATA_PTR;
#else
//
// user space
//
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <memory.h>
#include <netinet/in.h>
#endif
#include "aruba-inventory.h"

#define INVENT_MAGIC    0x414E4956

#define INVENT_SIZE         4096
#define INVENT_FORMAT       1

#ifdef __TLV_INVENTORY_ALIGN__
#define invent_roundup(x,align) (((x) + (align) - 1) & ~((align) - 1))
#else
#define invent_roundup(x,align) (x)
#endif

int inventory_init = 0;

typedef void (*tlv_format_func_t)(__u8 *, unsigned);

#ifndef __U_BOOT__
extern unsigned long crc32(unsigned long, const unsigned char *, unsigned);
#endif

typedef struct invent_internal_tlv_t {
    __u16  it_type;
    __u16  it_len;    /* includes header */
    __u8   it_inuse;
    __u8   it_data[INVENT_MAX_DATA];
} invent_internal_tlv_t;

typedef struct invent_internal_card_t {
    invent_tlv_t ic_hdr;
    __u16        ic_num;
    __u16        ic_parent;
    __u8         ic_inuse;
    void        *ic_handle;
    invent_internal_tlv_t ic_tlvs[INVENT_MAX_TLVS];
} invent_internal_card_t;

typedef struct invent_internal_t {
    invent_internal_card_t ii_cards[INVENT_MAX_CARDS];
    void                  *ii_handle;
    void                  *ii_parent;
    __u8                  *ii_buf;
    unsigned               ii_len;
    invent_read_func       ii_read;
    invent_write_func      ii_write;
    __u8                   ii_inuse;
} invent_internal_t;

static invent_internal_t inventory[INVENT_MAX_INVENTORIES];

typedef struct invent_card_info_t {
    unsigned    ci_type;
    char       *ci_name;
} invent_card_info_t;

static invent_card_info_t invent_cards[] = {
    { INVENT_CARD_TYPE_BASE,        "Base",         },
    { INVENT_CARD_TYPE_CPU,         "CPU",          },
    { INVENT_CARD_TYPE_RADIO,       "Radio",        },
    { INVENT_CARD_TYPE_CHASSIS,     "Chassis",      },
    { INVENT_CARD_TYPE_LINECARD,    "LC",           },
    { INVENT_CARD_TYPE_FABRIC,      "Fabric",       },
    { INVENT_CARD_TYPE_ANTENNA,     "Antenna",      },
    { INVENT_CARD_TYPE_SYSTEM,      "System",       },
    { INVENT_CARD_TYPE_POE,         "POE",          },
    { INVENT_CARD_TYPE_FAN,         "Fan Tray",     },
    { INVENT_CARD_TYPE_PSU,         "Power Supply", },
    { INVENT_CARD_TYPE_UPLINK,      "Uplink",       },
    { INVENT_CARD_TYPE_XCVR,        "Transceiver",  },
    { INVENT_CARD_TYPE_INTF,        "Interface",    },
    { INVENT_CARD_TYPE_CONSOLE,     "Console",      },
    { INVENT_CARD_TYPE_DPI,         "DPI",          },
    { INVENT_CARD_TYPE_SC,          "Supervisor",   },
    { INVENT_CARD_TYPE_BP,          "Backplane",    },
    { INVENT_CARD_TYPE_CM,          "Cable Modem",  },
    { INVENT_CARD_TYPE_POWER_BOARD, "Power",        },
};

static int invent_n_cards = sizeof(invent_cards) / sizeof(invent_cards[0]);

__u16
invent_getshort(void *vp)
{
    __u16 ret;
    __u8 *p1 = vp;

    ret = (p1[0] << 8) | p1[1];
    return ret;
}

__u32
invent_getlong(void *vp)
{
    __u32 ret;
    __u8 *p1 = vp;

    ret = (p1[0] << 24) | (p1[1] << 16) | (p1[2] << 8) | p1[3];
    return ret;
}

static void
invent_putshort(void *vp, __u16 v)
{
    __u8 *p1 = vp;

    *p1++ = (v >> 8) & 0xff;
    *p1 = v & 0xff;
}

static void
invent_putlong(void *vp, __u32 v)
{
    __u8 *p1 = vp;

    *p1++ = (v >> 24) & 0xff;
    *p1++ = (v >> 16) & 0xff;
    *p1++ = (v >> 8) & 0xff;
    *p1 = v & 0xff;
}

static void
invent_tlv_print_mac(__u8 *mac, unsigned len)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void
invent_tlv_print_str(__u8 *s, unsigned len)
{
    printf("%.*s", len, s);
}

static void
invent_tlv_print_u16(__u8 *v, unsigned len)
{
    printf("%u", invent_getshort(v));
}

static void
invent_tlv_print_card_type(__u8 *v, unsigned len)
{
    int i;
    __u8 c = *v;

    for (i = 0; i < invent_n_cards; i++) {
        if (invent_cards[i].ci_type == c) {
            printf("%s", invent_cards[i].ci_name);
            return;
        }
    }
    printf("Unknown card 0x%x", c);
}

typedef struct invent_tlv_info_t {
    unsigned          ti_val;
    char             *ti_name;
    tlv_format_func_t ti_fmt;
} invent_tlv_info_t;

static invent_tlv_info_t invent_tlvs[] = {
    { INVENT_TLV_WIRED_MAC,          "Wired MAC",          invent_tlv_print_mac, },
    { INVENT_TLV_WIRED_MAC_COUNT,    "Wired MAC Count",    invent_tlv_print_u16, },
    { INVENT_TLV_WIRELESS_MAC,       "Wireless MAC",       invent_tlv_print_mac, },
    { INVENT_TLV_WIRELESS_MAC_COUNT, "Wireless MAC Count", invent_tlv_print_u16, },
    { INVENT_TLV_CARD_DATE,          "Date Code",          invent_tlv_print_str, },
    { INVENT_TLV_CARD_MAJOR_REV,     "Major Rev",          invent_tlv_print_str, },
    { INVENT_TLV_CARD_MINOR_REV,     "Minor Rev/Variant",  invent_tlv_print_str, },
    { INVENT_TLV_CARD_ASSY,          "Assembly",           invent_tlv_print_str, },
    { INVENT_TLV_CARD_SN,            "Serial",             invent_tlv_print_str, },
    { INVENT_TLV_CARD_TYPE,          "Type",               invent_tlv_print_card_type, },
    { INVENT_TLV_COUNTRY_CODE,       "Country",            invent_tlv_print_str, },
    { INVENT_TLV_OEM,                "OEM",                invent_tlv_print_str, },
    { INVENT_TLV_CARD_ST,            "Service Tag",        invent_tlv_print_str, },
    { INVENT_TLV_CARD_RADIO0_SN,     "Radio 5G SN",        invent_tlv_print_str, },
    { INVENT_TLV_CARD_RADIO1_SN,     "Radio 2G SN",        invent_tlv_print_str, },
    { INVENT_TLV_TEST_STATION,       "Test Station ID",    invent_tlv_print_str, },
    { INVENT_TLV_CARD_MODEL,         "Model",              invent_tlv_print_str, },
    { INVENT_TLV_CARD_MAC,           "MAC",                invent_tlv_print_mac, },
    { INVENT_TLV_CARD_INTERNAL_MAC,  "Internal MAC",       invent_tlv_print_mac, },
    { INVENT_TLV_FIPS,               "FIPS",               invent_tlv_print_str, },
    { INVENT_TLV_CARD_LICMODEL,      "LIC Model",          invent_tlv_print_str, },
    { INVENT_TLV_OEM_SN,             "SCALANCE NO",        invent_tlv_print_str, },
    { INVENT_TLV_CARD_MAX_EIRP,      "Max EIRP",           invent_tlv_print_str, },
};

static int invent_n_tlvs = sizeof(invent_tlvs) / sizeof(invent_tlvs[0]);

static char *invent_errors[] = {
    "Success",
    "Bad CRC",
    "Bad magic number",
    "Unknown format",
    "No free TLVs",
    "No room in buffer",
    "No card for reparent",
    "Too many inventories",
    "Not found",
    "Handle in use",
};

static int invent_n_errors = sizeof(invent_errors) / sizeof(invent_errors[0]);

static int
invent_find_inventory(void *hdl)
{
    int inv;

    for (inv = 0; inv < INVENT_MAX_INVENTORIES; inv++) {
        if (hdl == inventory[inv].ii_handle && inventory[inv].ii_inuse) {
            return inv;
        }
    }
    return INVENT_ERR_NOT_FOUND;
}

static void
__invent_init(void)
{
#if defined(__U_BOOT__) && !defined(__TLV_INVENTORY_NO_RELOCATE__)
    int i;

    // fix pointers after relocation
    for (i = 0; i < invent_n_cards; i++) {
        invent_cards[i].ci_name = (char *)((ulong) invent_cards[i].ci_name + gd->reloc_off);
    }

    for (i = 0; i < invent_n_tlvs; i++) {
        invent_tlvs[i].ti_name = (char *)((ulong) invent_tlvs[i].ti_name + gd->reloc_off);
        invent_tlvs[i].ti_fmt = (tlv_format_func_t)((ulong) invent_tlvs[i].ti_fmt + gd->reloc_off);
    }

    for (i = 0; i < invent_n_errors; i++) {
        invent_errors[i] = (char *)((ulong) invent_errors[i] + gd->reloc_off);
    }
#endif
}

int
invent_setup(__u8 *b, unsigned l, invent_read_func r, invent_write_func w, void **hdl)
{
    static int __inited = 0;
    int inv;

    if (!__inited) {
        __invent_init();
        __inited = 1;
    }

    for (inv = 0; inv < INVENT_MAX_INVENTORIES; inv++) {
        if (!inventory[inv].ii_inuse) {
            break;
        }
    }
    if (inv == INVENT_MAX_INVENTORIES) {
        return INVENT_ERR_TOO_MANY;
    }

    inventory[inv].ii_read = r;
    inventory[inv].ii_write = w;

    inventory[inv].ii_buf = b;
    inventory[inv].ii_len = l;

    *hdl = &inventory[inv];
    inventory[inv].ii_handle = *hdl;
    inventory[inv].ii_inuse = 1;

    return 0;
}


#if defined(__U_BOOT__) 
# if defined(CONFIG_GRAPPA_CPBOOT) || defined(CONFIG_OUZO) || defined(CONFIG_GROZDOVA) || defined(CONFIG_PALINKA_CPBOOT) || defined(CONFIG_OUZOPLUS_CPBOOT)  || defined(CONFIG_MILAGRO_CPBOOT) /*|| defined(CONFIG_AMARULA_CPBOOT)*/
/*Function definition is ahead*/
static int
invent_build_inventory(int inv);
static int
invent_find_inventory(void *hdl);

int get_inventory_index(void *hdl)
{ 
    return invent_find_inventory(hdl);
}
int
check_inventory(int inv)
{
    int r;
    inventory[inv].ii_read(inventory[inv].ii_buf, inventory[inv].ii_len);
    if ((r = invent_build_inventory(inv)) < 0) {
        return 0;
    } else {
        // printf("Inventory exists and appears to be OK\n");
        return 1;
    }
}
# endif
#endif
static int
invent_setup_empty_inventory(int inv)
{
  
#if 0
    memset(&inventory[inv], 0, sizeof(inventory[inv]));
#endif
    return 0;
}

static void
invent_parse_card_tlvs(int inv, unsigned cnum, invent_card_t *ctlv, unsigned size)
{
    invent_tlv_t *tlv = (invent_tlv_t *)(ctlv + 1);
    int cnt = 0;

    size -= sizeof(*ctlv);

    while (size) {
        __u8 *p;
        unsigned len;
        unsigned adj;
        __u16 type;

        type = invent_getshort(&tlv->it_type);

        inventory[inv].ii_cards[cnum].ic_tlvs[cnt].it_type = type;
        inventory[inv].ii_cards[cnum].ic_tlvs[cnt].it_inuse = 1;

        len = invent_getshort(&tlv->it_len);
        adj = invent_roundup(len, sizeof(__u32));

        len -= sizeof(*tlv);

        p = (__u8 *)(tlv + 1);
        if (len > INVENT_MAX_DATA) {
            len = INVENT_MAX_DATA;
        }
        memcpy(inventory[inv].ii_cards[cnum].ic_tlvs[cnt].it_data, p, len);
        inventory[inv].ii_cards[cnum].ic_tlvs[cnt].it_len = len;

        size -= adj;
        cnt++;
        p = (__u8 *)tlv;
        tlv = (invent_tlv_t *)(p + adj);
    }
}

static void
invent_dump_tlv(invent_internal_tlv_t *tlv)
{
    int i;

    for (i = 0; i < invent_n_tlvs; i++) {
        if (invent_tlvs[i].ti_val == tlv->it_type) {
            invent_tlvs[i].ti_fmt(tlv->it_data, tlv->it_len);
            break;
        }
    }
}

static char *
invent_tlv_name(invent_internal_tlv_t *tlv)
{
    static char buf[32];
    int i;

    for (i = 0; i < invent_n_tlvs; i++) {
        if (invent_tlvs[i].ti_val == tlv->it_type) {
            return invent_tlvs[i].ti_name;
        }
    }
    snprintf(buf, sizeof(buf), "Unknown TLV 0x%x", tlv->it_type);
    return buf;
}

void
invent_dump(void *hdl)
{
    int i;
    int inv;

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return;
    }

    for (i = 0; i < INVENT_MAX_CARDS; i++) {
        invent_internal_card_t *card = &inventory[inv].ii_cards[i];
        int j;

        if (!card->ic_inuse) {
            continue;
        }
        printf("card %u, parent ", i);
        if (card->ic_parent == INVENT_NO_PARENT) {
            printf("none");
        } else {
            printf("%u", card->ic_parent);
        }
        printf("\n");

        for (j = 0; j < INVENT_MAX_TLVS; j++) {
            invent_internal_tlv_t *tlv = &card->ic_tlvs[j];
            if (!card->ic_tlvs[j].it_inuse) {
                continue;
            }
            printf("\tTLV[%u], type %s, len %u ", j, invent_tlv_name(tlv), tlv->it_len);
            invent_dump_tlv(tlv);
            printf("\n");
        }
    }
}

static void
invent_dump_card(invent_internal_card_t *card, unsigned level)
{
    int i, j;

    printf("Card %u: ", card->ic_num);
    for (i = 0; i < level; i++) {
        printf("\t");
    }
    for (i = 0; i < INVENT_MAX_TLVS; i++) {
        if (card->ic_tlvs[i].it_inuse 
            && card->ic_tlvs[i].it_type == INVENT_TLV_CARD_TYPE) {
            invent_tlv_print_card_type(card->ic_tlvs[i].it_data, card->ic_tlvs[i].it_len);
        }
    }
    printf("\n");

    for (i = 0; i < INVENT_MAX_TLVS; i++) {
        if (!card->ic_tlvs[i].it_inuse 
            || card->ic_tlvs[i].it_type == INVENT_TLV_CARD_TYPE) {
            continue;
        }
        for (j = 0; j < level + 1; j++) {
            printf("\t");
        }
        printf("%-20.20s: ", invent_tlv_name(&card->ic_tlvs[i]));
        invent_dump_tlv(&card->ic_tlvs[i]);
        printf("\n");
    }
}

static void
invent_dump_all_cards(int inv, unsigned card, unsigned level)
{
    int i, j, k;

    for (i = 0; i < level; i++) {
        printf("\t");
    }
    for (i = 0; i < INVENT_MAX_CARDS; i++) {
        if (!inventory[inv].ii_cards[i].ic_inuse) {
            continue;
        }
        if (inventory[inv].ii_cards[i].ic_parent == card
            && inventory[inv].ii_cards[i].ic_handle == &inventory[inv]) {
            invent_dump_card(&inventory[inv].ii_cards[i], level);
            invent_dump_all_cards(inv, i, level);
        }
        for (j = 0; j < INVENT_MAX_INVENTORIES; j++) {
            if (!inventory[j].ii_inuse) {
                continue;
            }
            if (inventory[j].ii_parent != &inventory[inv]) {
                continue;
            }
            if (j == inv) {
                continue;
            }
            for (k = 0; k < INVENT_MAX_CARDS; k++) {
                if (inventory[j].ii_cards[k].ic_parent == card
                    && card == i
                    && inventory[j].ii_cards[k].ic_handle == &inventory[inv]) {
                    invent_dump_card(&inventory[j].ii_cards[k], level);
                    invent_dump_all_cards(j, k, level);
                    break;
                }
            }
        }
    }
}

void
invent_mfginfo(void *hdl)
{
    int inv;

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return;
    }

    printf("Inventory:\n");
    invent_dump_all_cards(inv, INVENT_NO_PARENT, 0);
}

static void
invent_add_all_cards(int inv, invent_tlv_t *tlv, void *hdl)
{
    unsigned len;
    __u16 type;

    while ((type = invent_getshort(&tlv->it_type)) != INVENT_TLV_END) {
        unsigned size;
        len = invent_getshort(&tlv->it_len);
        size = invent_roundup(len, sizeof(__u32));
        if (type == INVENT_TLV_CARD) {
            invent_card_t *ctlv = (invent_card_t *)tlv;
            unsigned cnum = invent_getshort(&ctlv->ic_num);
            unsigned parent = invent_getshort(&ctlv->ic_parent);

            inventory[inv].ii_cards[cnum].ic_inuse = 1;
            inventory[inv].ii_cards[cnum].ic_parent = parent;
            inventory[inv].ii_cards[cnum].ic_num = cnum;
            inventory[inv].ii_cards[cnum].ic_handle = hdl;
            invent_parse_card_tlvs(inv, cnum, ctlv, size);
        }
        tlv = (invent_tlv_t *)((__u8 *)tlv + size);
    }
}

static int
invent_build_inventory(int inv)
{
    invent_hdr_t *hdr = (invent_hdr_t *)&inventory[inv].ii_buf[0];
    __u32 len, sum, oldcrc, magic;

    magic = invent_getlong(&hdr->ih_magic);
    if (magic != INVENT_MAGIC) {
        return INVENT_ERR_BADMAGIC;
    }
    if (hdr->ih_format != INVENT_FORMAT) {
        return INVENT_ERR_BADFORMAT;
    }

    len = invent_getshort(&hdr->ih_len);
    if (len > inventory[inv].ii_len) {
        len = sizeof(inventory[inv].ii_len);
    }

    oldcrc = invent_getlong(&hdr->ih_crc);
    invent_putlong(&hdr->ih_crc, 0);

    sum = crc32(0, inventory[inv].ii_buf, len);
    if (sum != oldcrc) {
        return INVENT_ERR_BADCRC;
    }

    invent_add_all_cards(inv, (invent_tlv_t *)(hdr + 1), inventory[inv].ii_handle);

    return 0;
}

int
invent_init(void *hdl)
{
    int r;
    int inv;

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return inv;
    }

    inventory[inv].ii_read(inventory[inv].ii_buf, inventory[inv].ii_len);

    if ((r = invent_build_inventory(inv)) < 0) {
#if !defined(X86_TARGET_PLATFORM)
        /*In X86 eprom simulated via files not important for intergrity check
         * Also during bootup file system may not be UP*/
        printf("Inventory uninitialized - setting up empty inventory: %s\n",
            invent_error(r));
#endif        
        invent_setup_empty_inventory(inv);
        return 0;
    } else {
        // printf("Inventory exists and appears to be OK\n");
        return 1;
    }
}

char *
invent_error(int code)
{
    static char buf[128];

    code = -code;

    if (code < 0 || code >= invent_n_errors) {
        snprintf(buf, sizeof(buf), "Error %u\n", code);
        return buf;
    }

    return invent_errors[code];
}

#if 0
static void
bdump(void *v, unsigned sz, char *s)
{
    __u8 *p = (__u8 *)v;
    int i = 0;

    printf("%s: ", s);
    while (sz) {
        printf("%02x ", *p++);
        sz--;
        i++;
        if (i == 16) {
            i = 0;
            printf("\n");
        }
    }
    printf("\n");
}
#endif

static __u8 *
invent_write_card(int inv, __u8 *p, invent_internal_card_t *card, int num)
{
    invent_card_t *ctlv = (invent_card_t *)p;
    __u16 len;
    int i;

    if (!card->ic_inuse) {
        return p;
    }

    invent_putshort(&ctlv->ic_hdr.it_type, INVENT_TLV_CARD);
    invent_putshort(&ctlv->ic_num, num);
    invent_putshort(&ctlv->ic_parent, card->ic_parent);

    p += sizeof(*ctlv);

    for (i = 0; i < INVENT_MAX_TLVS; i++) {
        invent_internal_tlv_t *stlv = &card->ic_tlvs[i];
        invent_tlv_t *tlv = (invent_tlv_t *)p;

        if (!stlv->it_inuse) {
            continue;
        }

        invent_putshort(&tlv->it_type, stlv->it_type);

        p += sizeof(*tlv);

        memcpy(p, stlv->it_data, stlv->it_len);

        len = stlv->it_len + sizeof(*tlv);
        p += invent_roundup(stlv->it_len, sizeof(__u32));
        invent_putshort(&tlv->it_len, len);

        if (p - &inventory[inv].ii_buf[0] >= inventory[inv].ii_len) {
            return 0;
        }
    }

    len = p - (__u8 *)ctlv;
    invent_putshort(&ctlv->ic_hdr.it_len, len);

    return p;
}

int
invent_save(void *hdl)
{
    __u8 *p;
    invent_hdr_t *hdr;
    invent_tlv_t *tlv;
    int i;
    unsigned len;
    int inv;

    inv = invent_find_inventory(hdl);

    if (inv < 0) {
        return inv;
    }

    p = &inventory[inv].ii_buf[0];
    hdr = (invent_hdr_t *)&inventory[inv].ii_buf[0];

    hdr->ih_format = INVENT_FORMAT;
    invent_putlong(&hdr->ih_magic, INVENT_MAGIC);
    hdr->ih_pad = 0;

    p += sizeof(*hdr);

    for (i = 0; i < INVENT_MAX_CARDS; i++) {
        p = invent_write_card(inv, p, &inventory[inv].ii_cards[i], i);
        if (!p) {
            return INVENT_ERR_NO_ROOM;
        }
#ifdef __TLV_INVENTORY_ALIGN__
        while ((unsigned)p & 3) {
            p++;
        }
#endif
    }

    tlv = (invent_tlv_t *)p;
    invent_putshort(&tlv->it_len, sizeof(*tlv));
    invent_putshort(&tlv->it_type, INVENT_TLV_END);

    p += sizeof(*tlv);

    len = p - &inventory[inv].ii_buf[0];
    invent_putshort(&hdr->ih_len, len);

    invent_putlong(&hdr->ih_crc, 0);
    invent_putlong(&hdr->ih_crc, crc32(0, inventory[inv].ii_buf, len));

    return inventory[inv].ii_write(inventory[inv].ii_buf, inventory[inv].ii_len);
}

int
invent_del_card(void *hdl, unsigned number)
{
    invent_internal_card_t *card;
    int inv;

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return inv;
    }
   
    card = &inventory[inv].ii_cards[number];

    if (card->ic_inuse) {
        card->ic_inuse = 0;
    }

    return 0;
}

int
invent_set_card(void *hdl, unsigned number, unsigned type, unsigned parent)
{
    invent_internal_card_t *card;
    int inv;

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return inv;
    }
    
    card = &inventory[inv].ii_cards[number];

    card->ic_tlvs[0].it_type = INVENT_TLV_CARD_TYPE;
    card->ic_tlvs[0].it_len = sizeof(__u8);
    card->ic_tlvs[0].it_data[0] = type;
    card->ic_tlvs[0].it_inuse = 1;

    card->ic_parent = parent;
    card->ic_handle = &inventory[inv];
    card->ic_inuse = 1;

    return 0;
}

int
invent_add_tlv(void *hdl, unsigned card_number, unsigned type, __u8 *data, unsigned l)
{
    invent_internal_card_t *card;
    invent_internal_tlv_t *tlv;
    int i;
    int inv;

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return inv;
    }

    card = &inventory[inv].ii_cards[card_number];
    // first try to find the same type if it's in use
    for (i = 0; i < INVENT_MAX_TLVS; i++) {
        if (card->ic_tlvs[i].it_inuse && card->ic_tlvs[i].it_type == type) {
            break;
        }
    }
    if (i == INVENT_MAX_TLVS) {
        // if we didn't find it, then just find a free slot
        for (i = 0; i < INVENT_MAX_TLVS; i++) {
            if (!card->ic_tlvs[i].it_inuse) {
                break;
            }
        }
    }

    if (i == INVENT_MAX_TLVS) {
        return INVENT_ERR_NO_FREE_TLVS;
    }

    tlv = &card->ic_tlvs[i];
    tlv->it_inuse = 1;
    tlv->it_type = type;

    if (l > INVENT_MAX_DATA) {
        l = INVENT_MAX_DATA;
    }
    memcpy(tlv->it_data, data, l);
    tlv->it_len = l;

    return 0;
}

int
invent_remove_tlv(void *hdl, unsigned card_number, unsigned type)
{
    invent_internal_card_t *card;
    invent_internal_tlv_t *tlv;
    int i;
    int inv;

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return inv;
    }

    card = &inventory[inv].ii_cards[card_number];
    // first try to find the same type if it's in use
    for (i = 0; i < INVENT_MAX_TLVS; i++) {
        if (card->ic_tlvs[i].it_inuse && card->ic_tlvs[i].it_type == type) {
            break;
        }
    }

    if (i == INVENT_MAX_TLVS) {
        return INVENT_ERR_NO_FREE_TLVS;
    }

    tlv = &card->ic_tlvs[i];
    tlv->it_inuse = 0;

    return 0;
}

/*
 * Note that this function will always zero the first byte of the buffer,
 * even if it fails to retrieve the TLV.  This seemed like a good idea
 * at the time, but can surprise callers of this API.
 */
int
invent_get_card_tlv(void *hdl, unsigned card, unsigned tag, __u8 *val, unsigned *len)
{
    int i = 0;
    unsigned l = *len;
    int inv;

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return inv;
    }

    *len = 0;
    *val = 0;    // values to return if tag isn't found

    if (inventory[inv].ii_cards[card].ic_inuse) {
        while (tag != inventory[inv].ii_cards[card].ic_tlvs[i].it_type
               || !inventory[inv].ii_cards[card].ic_tlvs[i].it_inuse) {
            i++;
            if (i >= INVENT_MAX_TLVS) {  // didn't find the tag
                return INVENT_ERR_NOT_FOUND;
            }
        }

        if (l > inventory[inv].ii_cards[card].ic_tlvs[i].it_len) {
            l = inventory[inv].ii_cards[card].ic_tlvs[i].it_len;
        }
        *len = l;
        memcpy(val, inventory[inv].ii_cards[card].ic_tlvs[i].it_data, l);
        return 0;
    } else {
        return INVENT_ERR_NOT_FOUND;
    }
}

int
invent_adopt_inventory(void *hdl, void *phdl, unsigned parent, unsigned child)
{
    int inv, pinv;

    if (parent == INVENT_NO_PARENT) {
        return INVENT_ERR_NOT_FOUND;    // XXX
    }

    if (child == INVENT_NO_PARENT) {
        return INVENT_ERR_NOT_FOUND;    // XXX
    }

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return inv;
    }

    pinv = invent_find_inventory(phdl);
    if (pinv < 0) {
        return pinv;
    }

    inventory[inv].ii_parent = phdl;

    if (inventory[inv].ii_cards[child].ic_inuse) {
        inventory[inv].ii_cards[child].ic_parent = parent;
        inventory[inv].ii_cards[child].ic_handle = phdl;
    }

    return 0;
}

int
invent_orphan_inventory(void *hdl)
{
    int i;
    int inv;

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return inv;
    }

    // remove any cards that were reparented under the parent inventory
    for (i = 0; i < INVENT_MAX_CARDS; i++) {
        if (inventory[inv].ii_cards[i].ic_handle == inventory[inv].ii_parent) {
            inventory[inv].ii_cards[i].ic_handle = hdl;
        }
    }
    inventory[inv].ii_parent = 0;

    return 0;
}

int
invent_delete_inventory(void *hdl)
{
    int inv;

    inv = invent_find_inventory(hdl);
    if (inv < 0) {
        return inv;
    }

    inventory[inv].ii_inuse = 0;
    inventory[inv].ii_handle = 0;

    return 0;
}
