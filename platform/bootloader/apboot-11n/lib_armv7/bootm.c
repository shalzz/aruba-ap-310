/* Copyright (C) 2011
 * Corscience GmbH & Co. KG - Simon Schwarz <schwarz@corscience.de>
 *  - Added prep subcommand support
 *  - Reorganized source - modeled after powerpc version
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA
 *
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <u-boot/zlib.h>
#include <asm/byteorder.h>
#include <fdt.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <asm/bootm.h>
#ifdef CONFIG_APBOOT
#include "../common/aruba-inventory.h"
extern void *inventory_handle;
#endif

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined(CONFIG_REVISION_TAG)
static struct tag *params;
#endif

static ulong get_sp(void)
{
	ulong ret;

	asm("mov %0, sp" : "=r"(ret) : );
	return ret;
}

void arch_lmb_reserve(struct lmb *lmb)
{
	ulong sp;

	/*
	 * Booting a (Linux) kernel image
	 *
	 * Allocate space for command line and board info - the
	 * address should be as high as possible within the reach of
	 * the kernel (see CONFIG_SYS_BOOTMAPSZ settings), but in unused
	 * memory, which means far enough below the current stack
	 * pointer.
	 */
	sp = get_sp();
	debug("## Current stack ends at 0x%08lx ", sp);

	/* adjust sp by 4K to be safe */
	sp -= 4096;
	lmb_reserve(lmb, sp,
		    gd->bd->bi_dram[0].start + gd->bd->bi_dram[0].size - sp);
}

#ifdef CONFIG_OF_LIBFDT
static int fixup_memory_node(void *blob)
{
	bd_t	*bd = gd->bd;
	int bank;
	u64 start[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];

	for (bank = 0; bank < CONFIG_NR_DRAM_BANKS; bank++) {
		start[bank] = bd->bi_dram[bank].start;
		size[bank] = bd->bi_dram[bank].size;
	}

	return fdt_fixup_memory_banks(blob, start, size, CONFIG_NR_DRAM_BANKS);
}
#endif

static void announce_and_cleanup(void)
{
	debug("\nStarting kernel ...\n\n");
	bootstage_mark_name(BOOTSTAGE_ID_BOOTM_HANDOFF, "start_kernel");
#ifdef CONFIG_BOOTSTAGE_REPORT
	bootstage_report();
#endif

#ifdef CONFIG_USB_DEVICE
	udc_disconnect();
#endif
	cleanup_before_linux();
}

#if defined (CONFIG_IPROC)
static void setup_aruba_tag(void);
#ifdef __ARUBA_WIRELESS_MAC__
static void setup_aruba_wmac_tag(void);
#endif	/* __ARUBA_WIRELESS_MAC__ */
#endif

#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined(CONFIG_REVISION_TAG)
static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *)bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 1 << 12;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}
#endif

#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags(bd_t *bd)
{
	int i;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size (tag_mem32);

		params->u.mem.start = bd->bi_dram[i].start;
		params->u.mem.size = bd->bi_dram[i].size;

		params = tag_next (params);
	}
}
#endif

#ifdef CONFIG_CMDLINE_TAG
static void setup_commandline_tag(bd_t *bd, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

	params = tag_next (params);
}
#endif

#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag(bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next (params);
}
#endif

#ifdef CONFIG_SERIAL_TAG
void setup_serial_tag(struct tag **tmp)
{
	struct tag *params = *tmp;
	struct tag_serialnr serialnr;
	void get_board_serial(struct tag_serialnr *serialnr);

	get_board_serial(&serialnr);
	params->hdr.tag = ATAG_SERIAL;
	params->hdr.size = tag_size (tag_serialnr);
	params->u.serialnr.low = serialnr.low;
	params->u.serialnr.high= serialnr.high;
	params = tag_next (params);
	*tmp = params;
}
#endif

#ifdef CONFIG_REVISION_TAG
void setup_revision_tag(struct tag **in_params)
{
	u32 rev = 0;
	u32 get_board_rev(void);

	rev = get_board_rev();
	params->hdr.tag = ATAG_REVISION;
	params->hdr.size = tag_size (tag_revision);
	params->u.revision.rev = rev;
	params = tag_next (params);
}
#endif

#if defined(CONFIG_IPROC) || defined(CONFIG_MACH_IPQ806x)
static void
setup_aruba_tag(void)
{
	extern char build_num[], version_string[];
	extern int apboot_os_partition;

	params->hdr.tag = ATAG_ARUBA_UBOOT;
	params->hdr.size = tag_size (tag_aruba_uboot);

	params->u.aruba_uboot.au_flash.size = CFG_FLASH_SIZE;
	params->u.aruba_uboot.au_flash.start = CFG_FLASH_BASE;
	params->u.aruba_uboot.au_booted_partition = apboot_os_partition;
	params->u.aruba_uboot.au_flags = 0;
	params->u.aruba_uboot.au_usb_power = 0;
	params->u.aruba_uboot.au_spare1 = 0;
	params->u.aruba_uboot.au_spare2 = 0;
	params->u.aruba_uboot.au_num_cores = CONFIG_SYS_NUM_CORES;

#ifdef CONFIG_APBOOT
        snprintf(params->u.aruba_uboot.au_bootver,
		sizeof(params->u.aruba_uboot.au_bootver),
		"%s/%s", version_string, build_num);
#else
        snprintf(params->u.aruba_uboot.au_bootver,
		sizeof(params->u.aruba_uboot.au_bootver),
		"%s/%s", "unknown", "unknown");
#endif
#ifdef ARUBA_BOARD_TYPE
	snprintf(params->u.aruba_uboot.au_boardname,
		sizeof(params->u.aruba_uboot.au_boardname),
		"%s", ARUBA_BOARD_TYPE);
#else
	snprintf(params->u.aruba_uboot.au_boardname,
		sizeof(params->u.aruba_uboot.au_boardname),
		"%s", "unknown");
#endif
#ifdef CONFIG_APBOOT
	{
		char v[INVENT_MAX_DATA];
		unsigned l = sizeof(v);
		int r;

		r = invent_get_card_tlv(inventory_handle, __CARD_WITH_VARIANT__, 
			INVENT_TLV_CARD_VARIANT, v, &l);
		if (r == 0) {
			snprintf(params->u.aruba_uboot.au_variant,
				sizeof(params->u.aruba_uboot.au_variant),
				"%.*s", l, v);
		} else {
			snprintf(params->u.aruba_uboot.au_variant,
				sizeof(params->u.aruba_uboot.au_variant),
				"%s", "unknown");
		}

		l = sizeof(v);
		r = invent_get_card_tlv(inventory_handle, 0, 
			INVENT_TLV_WIRED_MAC, v, &l);
		if (r == 0) {
			memcpy(params->u.aruba_uboot.au_mac, v, 6);
		} else {
			memset(params->u.aruba_uboot.au_mac, 0, 6);
		}

		l = sizeof(v);
		r = invent_get_card_tlv(inventory_handle, 0, 
			INVENT_TLV_WIRED_MAC_COUNT, v, &l);
		if (r == 0) {
			params->u.aruba_uboot.au_mac_count = invent_getshort(v);
		} else {
			params->u.aruba_uboot.au_mac_count = 0;
		}
	}
#endif

	params = tag_next (params);
}

#ifdef __ARUBA_WIRELESS_MAC__
static void
setup_aruba_wmac_tag(void)
{

#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	unsigned int cpu = 0;
	cpu = whoAmI();
#endif
	params->hdr.tag = ATAG_ARUBA_WMAC;
	params->hdr.size = tag_size (tag_aruba_wmac);

#ifdef CONFIG_APBOOT
	{
		char v[INVENT_MAX_DATA];
		unsigned l = sizeof(v);
		int r;

		/* 
		 * for now, we assume that the WMAC information is on the system
		 * card
		 */
		r = invent_get_card_tlv(inventory_handle, 0, 
			INVENT_TLV_WIRELESS_MAC, v, &l);
		if (r == 0) {
			memcpy(params->u.aruba_wmac.aw_wmac, v, 6);
		} else {
			memset(params->u.aruba_wmac.aw_wmac, 0, 6);
		}
		r = invent_get_card_tlv(inventory_handle, 0, 
			INVENT_TLV_WIRELESS_MAC_COUNT, v, &l);
		if (r == 0) {
			params->u.aruba_wmac.aw_count = invent_getshort(v);
		} else {
			params->u.aruba_wmac.aw_count = 0;
		}
	}
#endif

	params = tag_next (params);
}
#endif	/* __ARUBA_WIRELESS_MAC__ */
#endif
#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined(CONFIG_REVISION_TAG)
static void setup_end_tag(bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}
#endif

#ifdef CONFIG_OF_LIBFDT
static int create_fdt(bootm_headers_t *images)
{
	ulong of_size = images->ft_len;
	char **of_flat_tree = &images->ft_addr;
	ulong *initrd_start = &images->initrd_start;
	ulong *initrd_end = &images->initrd_end;
	struct lmb *lmb = &images->lmb;
	ulong rd_len;
	int ret;

	debug("using: FDT\n");

	boot_fdt_add_mem_rsv_regions(lmb, *of_flat_tree);

	rd_len = images->rd_end - images->rd_start;
	ret = boot_ramdisk_high(lmb, images->rd_start, rd_len,
			initrd_start, initrd_end);
	if (ret)
		return ret;

	ret = boot_relocate_fdt(lmb, of_flat_tree, &of_size);
	if (ret)
		return ret;

	fdt_chosen(*of_flat_tree, 1);
	fixup_memory_node(*of_flat_tree);
	fdt_fixup_ethernet(*of_flat_tree);
	fdt_initrd(*of_flat_tree, *initrd_start, *initrd_end, 1);
#ifdef CONFIG_OF_BOARD_SETUP
	ft_board_setup(*of_flat_tree, gd->bd);
#endif

	return 0;
}
#endif

/* Subcommand: PREP */
static void boot_prep_linux(bootm_headers_t *images)
{
#ifdef CONFIG_CMDLINE_TAG
	char *commandline = getenv("bootargs");

	debug("%s commandline: %s\n", __FUNCTION__, commandline);
#endif

	
#ifdef CONFIG_OF_LIBFDT
	if (images->ft_len) {
		debug("using: FDT\n");
		if (create_fdt(images)) {
			printf("FDT creation failed! hanging...");
			hang();
		}
	} else
#endif
	{
#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined(CONFIG_REVISION_TAG) || \
	defined(CONFIG_IPQ_ATAG_PART_LIST)
		debug("using: ATAGS\n");
		setup_start_tag(gd->bd);
#ifdef CONFIG_SERIAL_TAG
		setup_serial_tag(&params);
#endif
#ifdef CONFIG_CMDLINE_TAG
		setup_commandline_tag(gd->bd, commandline);
#endif
#ifdef CONFIG_REVISION_TAG
		setup_revision_tag(&params);
#endif
#ifdef CONFIG_SETUP_MEMORY_TAGS
		setup_memory_tags(gd->bd);
#endif
#ifdef CONFIG_INITRD_TAG
		if (images->rd_start && images->rd_end)
			setup_initrd_tag(gd->bd, images->rd_start,
			images->rd_end);
#endif
#if defined(CONFIG_IPROC) || defined(CONFIG_MACH_IPQ806x)
		setup_aruba_tag();
#ifdef __ARUBA_WIRELESS_MAC__
		setup_aruba_wmac_tag();
#endif	/* __ARUBA_WIRELESS_MAC__ */
#endif
		setup_end_tag(gd->bd);
//print_buffer(gd->bd->bi_boot_params, (void *)gd->bd->bi_boot_params, 1, 0x100, 0);
#else /* all tags */
		printf("FDT and ATAGS support not compiled in - hanging\n");
		hang();
#endif /* all tags */
	}
}

/* Subcommand: GO */
static void boot_jump_linux(bootm_headers_t *images)
{
	unsigned long machid = gd->bd->bi_arch_number;
	char *s;
	void (*kernel_entry)(int zero, int arch, uint params);
	unsigned long r2;

	kernel_entry = (void (*)(int, int, uint))images->ep;

	s = getenv("machid");
	if (s) {
		strict_strtoul(s, 16, &machid);
	}

	debug("## Transferring control to Linux (at address %08lx), machid %lx" \
		"...\n", (ulong) kernel_entry, machid);
	bootstage_mark(BOOTSTAGE_ID_RUN_OS);
	announce_and_cleanup();

#ifdef CONFIG_OF_LIBFDT
	if (images->ft_len)
		r2 = (unsigned long)images->ft_addr;
	else
#endif
		r2 = gd->bd->bi_boot_params;
        
	kernel_entry(0, machid, r2);
}

/* Main Entry point for arm bootm implementation
 *
 * Modeled after the powerpc implementation
 * DIFFERENCE: Instead of calling prep and go at the end
 * they are called if subcommand is equal 0.
 */
int do_bootm_linux(int flag, int argc, char *argv[], bootm_headers_t *images)
{
	/* No need for those on ARM */
	if (flag & BOOTM_STATE_OS_BD_T || flag & BOOTM_STATE_OS_CMDLINE)
		return -1;

	if (flag & BOOTM_STATE_OS_PREP) {
		boot_prep_linux(images);
		return 0;
	}

	if (flag & BOOTM_STATE_OS_GO) {
		boot_jump_linux(images);
		return 0;
	}

	boot_prep_linux(images);
	boot_jump_linux(images);
	return 0;
}

#ifdef CONFIG_CMD_BOOTZ

struct zimage_header {
	uint32_t	code[9];
	uint32_t	zi_magic;
	uint32_t	zi_start;
	uint32_t	zi_end;
};

#define	LINUX_ARM_ZIMAGE_MAGIC	0x016f2818

int bootz_setup(void *image, void **start, void **end)
{
	struct zimage_header *zi = (struct zimage_header *)image;

	if (zi->zi_magic != LINUX_ARM_ZIMAGE_MAGIC) {
		puts("Bad Linux ARM zImage magic!\n");
		return 1;
	}

	*start = (void *)zi->zi_start;
	*end = (void *)zi->zi_end;

	debug("Kernel image @ 0x%08x [ 0x%08x - 0x%08x ]\n",
		(uint32_t)image, (uint32_t)*start, (uint32_t)*end);

	return 0;
}
#endif	/* CONFIG_CMD_BOOTZ */
