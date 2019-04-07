/*
 * Copyright 2007-2011 Freescale Semiconductor, Inc.
 *
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifdef powerpc
#include <common.h>
#include <lstring.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <asm/processor.h>
#include <linux/ctype.h>
#ifdef __TLV_INVENTORY__
#include "aruba-inventory.h"
extern void *inventory_handle;
#endif
#include <asm/io.h>

extern char version_string[];
extern char build_num[];
extern int apboot_os_partition;

DECLARE_GLOBAL_DATA_PTR;

static void
aruba_update_property(void *fdt, int nodeoffset, char *prop, char *val)
{
	int err;

	err = fdt_setprop(fdt, nodeoffset, prop, val, strlen(val) + 1);
	if (err < 0) {
		printf("WARNING: could not set %s to %s: %s.\n", prop, val,
			fdt_strerror(err));
	}

}

void
aruba_fdt_setup(void *fdt)
{
	int   nodeoffset;
	int   err;
	char buf[128];
#ifdef __TLV_INVENTORY__
	__u8 v[INVENT_MAX_DATA];
	unsigned short *sp = (unsigned short *)&v[0];
	unsigned l;
	int r;
#endif
	char *p;

	err = fdt_check_header(fdt);
	if (err < 0) {
		printf("fdt_aruba_setup: %s\n", fdt_strerror(err));
		return;
	}

	/*
	 * Find the "aruba_aos" node.
	 */
	nodeoffset = fdt_path_offset(fdt, "/aruba_aos");

	/*
	 * If there is no "aruba_aos" node in the blob, create it.
	 */
	if (nodeoffset < 0) {
		/*
		 * Create a new node "/aruba_aos" (offset 0 is root level)
		 */
		nodeoffset = fdt_add_subnode(fdt, 0, "aruba_aos");
		if (nodeoffset < 0) {
			printf("WARNING: could not create /aruba_aos",
				fdt_strerror(nodeoffset));
			return;
		}
	}

	/*
	 * Create or update /aruba_aos properties
	 */
	aruba_update_property(fdt, nodeoffset, "board_name", ARUBA_BOARD_TYPE);

	/* place holder in case we need to change things */
	snprintf(buf, sizeof(buf), "%u", 0);
#ifdef CONFIG_TOMATIN
        /* For Tomatin, there are two type of flash size, 16M and 32M
         * We use flash_variant to identify them
         * flash_variant: 0 - 32M, 1 - 16M  
         */
#if (__TOMATIN_FLASH__ == 16)
	snprintf(buf, sizeof(buf), "%u", 1);
#endif
#endif
	aruba_update_property(fdt, nodeoffset, "flash_variant", buf);

	snprintf(buf, sizeof(buf), "%s/%s", version_string, build_num);
	aruba_update_property(fdt, nodeoffset, "boot_version", buf);

#ifdef __TLV_INVENTORY__
	l = sizeof(v);

	r = invent_get_card_tlv(inventory_handle, __CARD_WITH_VARIANT__, 
			INVENT_TLV_CARD_VARIANT, v, &l);
	if (r == 0) {
		snprintf(buf, sizeof(buf), "%.*s", l, v);
	} else {
		snprintf(buf, sizeof(buf), "%s", "unknown");
	}
	aruba_update_property(fdt, nodeoffset, "board_variant", buf);

	/* 
	 * for now, we assume that the WMAC information is on the system
	 * card
	 */
	l = sizeof(v);
	r = invent_get_card_tlv(inventory_handle, 0, 
		INVENT_TLV_WIRELESS_MAC, v, &l);
	if (r == 0) {
		snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
			v[0], v[1], v[2], v[3], v[4], v[5]);
	} else {
		snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
			0, 0, 0, 0, 0, 0);
		
	}
	aruba_update_property(fdt, nodeoffset, "wireless_mac", buf);

	l = sizeof(v);
	r = invent_get_card_tlv(inventory_handle, 0, 
		INVENT_TLV_WIRELESS_MAC_COUNT, v, &l);
	if (r == 0) {
		snprintf(buf, sizeof(buf), "%u", *sp);
	} else {
		snprintf(buf, sizeof(buf), "%u", 0);
	}
	aruba_update_property(fdt, nodeoffset, "wireless_mac_count", buf);

	/* 
	 * for now, we assume that the MAC information is on the system
	 * card
	 */
	l = sizeof(v);
	r = invent_get_card_tlv(inventory_handle, 0, 
		INVENT_TLV_WIRED_MAC, v, &l);
	if (r == 0) {
		snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
			v[0], v[1], v[2], v[3], v[4], v[5]);
	} else {
		snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
			0, 0, 0, 0, 0, 0);
		
	}
	aruba_update_property(fdt, nodeoffset, "wired_mac", buf);

	l = sizeof(v);
	r = invent_get_card_tlv(inventory_handle, 0, 
		INVENT_TLV_WIRED_MAC_COUNT, v, &l);
	if (r == 0) {
		snprintf(buf, sizeof(buf), "%u", *sp);
	} else {
		snprintf(buf, sizeof(buf), "%u", 0);
	}
	aruba_update_property(fdt, nodeoffset, "wired_mac_count", buf);
#endif

	snprintf(buf, sizeof(buf), "%u", apboot_os_partition);
	aruba_update_property(fdt, nodeoffset, "booted_partition", buf);

	/* USB power mode */
	p = getenv("usb_power_mode");
	if (p) {
		strlcpy(buf, p, sizeof(buf));
	} else {
		strlcpy(buf, "0", sizeof(buf));
	}
	aruba_update_property(fdt, nodeoffset, "usb_power_mode", buf);
}
#endif
