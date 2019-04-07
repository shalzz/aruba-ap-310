/*
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
#include <zlib.h>
#include <asm/byteorder.h>
#ifdef CONFIG_HAS_DATAFLASH
#include <dataflash.h>
#endif
#ifdef CONFIG_APBOOT
#include "../common/aruba-inventory.h"
extern void *inventory_handle;
#endif

extern unsigned int whoAmI(void);

/*cmd_boot.c*/
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern void mvEgigaStrToMac( char *source , char *dest );

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_VFD) || \
    defined (CONFIG_LCD) ||	\
    defined (CONFIG_MARVELL_TAG)


static void setup_start_tag (bd_t *bd);

# ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd);
# endif
static void setup_commandline_tag (bd_t *bd, char *commandline);

#if 0
static void setup_ramdisk_tag (bd_t *bd);
#endif
# ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start,
			      ulong initrd_end);
# endif
static void setup_end_tag (bd_t *bd);

# if defined (CONFIG_VFD) || defined (CONFIG_LCD)
static void setup_videolfb_tag (gd_t *gd);
# endif

#if defined (CONFIG_MARVELL_TAG)
static void setup_marvell_tag(void);
#endif

#if defined (ARUBA_ARM)
static void setup_aruba_tag(void);
#ifdef __ARUBA_WIRELESS_MAC__
static void setup_aruba_wmac_tag(void);
#endif	/* __ARUBA_WIRELESS_MAC__ */
#endif

#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
static struct tag *params[2];
#else
static struct tag *params[1];
#endif
#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */

#ifdef CONFIG_SHOW_BOOT_PROGRESS
# include <status_led.h>
# define SHOW_BOOT_PROGRESS(arg)	show_boot_progress(arg)
#else
# define SHOW_BOOT_PROGRESS(arg)
#endif

#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
extern image_header_t header[2]; /* from cmd_bootm.c */
#else
extern image_header_t header[1]; /* from cmd_bootm.c */
#endif

#if 0
static void
bdump(__u8 *p, unsigned l)
{
    int i = 0;
    while (l--) {
        printf("%02x ", *p++);
        i++;
        if (i == 16) {
            printf("\n");
            i = 0;
        }
    }
    if (i) {
        printf("\n");
    }
}
#endif

void do_bootm_linux (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
		     ulong addr, ulong *len_ptr, int verify)
{
	DECLARE_GLOBAL_DATA_PTR;
	ulong len = 0; 
	ulong data = 0;
	image_header_t *hdr;
	ulong checksum;
	ulong initrd_start, initrd_end;
	void (*theKernel)(int zero, int arch, uint params);
	bd_t *bd = gd->bd;
#if defined(MV78200)
	unsigned int cpu = 0;
#endif
	unsigned int cpuindex = 0;

#ifdef CONFIG_CMDLINE_TAG
	char *commandline;
#ifdef CONFIG_MARVELL
#if defined(MV78200)
	cpu = whoAmI();
	if (cpu == 0)
#endif
		commandline = getenv ("bootargs");
#if defined(MV78200)
	else
		commandline = getenv ("bootargs2");
#endif
#else
	commandline = getenv ("bootargs");
#endif
#endif
#if defined(DUAL_OS_SHARED_MEM_78200)
	cpuindex = cpu;
#endif
	hdr = &header[cpuindex];
	theKernel = (void (*)(int, int, uint))ntohl(hdr->ih_ep);

	debug(" theKernel %x\n", theKernel);

	/*
	 * Check if there is an initrd image
	 */
	if (argc >= 3) {
		SHOW_BOOT_PROGRESS (9);

		addr = simple_strtoul (argv[2], NULL, 16);

		printf ("## Loading Ramdisk Image at %08lx ...\n", addr);

		/* Copy header so we can blank CRC field for re-calculation */
#ifdef CONFIG_HAS_DATAFLASH
		if (addr_dataflash (addr)) {
			read_dataflash (addr, sizeof (image_header_t),
					(char *) &header[cpuindex]);
		} else
#endif
			memcpy (&header[cpuindex], (char *) addr,
				sizeof (image_header_t));

		if (ntohl (hdr->ih_magic) != IH_MAGIC) {
			printf ("Bad Magic Number\n");
			SHOW_BOOT_PROGRESS (-10);
			do_reset (cmdtp, flag, argc, argv);
		}

		data = (ulong) &header[cpuindex];
		len = sizeof (image_header_t);

		checksum = ntohl (hdr->ih_hcrc);
		hdr->ih_hcrc = 0;

		if (crc32 (0, (const char *) data, len) != checksum) {
			printf ("Bad Header Checksum\n");
			SHOW_BOOT_PROGRESS (-11);
			do_reset (cmdtp, flag, argc, argv);
		}

		SHOW_BOOT_PROGRESS (10);

#ifndef ARUBA_ARM
		print_image_hdr (hdr);
#endif

		data = addr + sizeof (image_header_t);
		len = ntohl (hdr->ih_size);

#ifdef CONFIG_HAS_DATAFLASH
		if (addr_dataflash (addr)) {
			read_dataflash (data, len, (char *) CFG_LOAD_ADDR);
			data = CFG_LOAD_ADDR;
		}
#endif

		if (verify) {
			ulong csum = 0;

			printf ("   Verifying Checksum ... ");
			csum = crc32 (0, (const char *) data, len);
			if (csum != ntohl (hdr->ih_dcrc)) {
				printf ("Bad Data CRC\n");
				SHOW_BOOT_PROGRESS (-12);
				do_reset (cmdtp, flag, argc, argv);
			}
			printf ("OK\n");
		}

		SHOW_BOOT_PROGRESS (11);

		if ((hdr->ih_os != IH_OS_LINUX) ||
		    (hdr->ih_arch != IH_ARCH_ARM) ||
		    (hdr->ih_type != IH_TYPE_RAMDISK)) {
			printf ("No Linux ARM Ramdisk Image\n");
			SHOW_BOOT_PROGRESS (-13);
			do_reset (cmdtp, flag, argc, argv);
		}

#if defined(CONFIG_B2) || defined(CONFIG_EVB4510) || defined(CONFIG_ARMADILLO)
		/*
		 *we need to copy the ramdisk to SRAM to let Linux boot
		 */
		memmove ((void *) ntohl(hdr->ih_load), (uchar *)data, len);
		data = ntohl(hdr->ih_load);
#endif /* CONFIG_B2 || CONFIG_EVB4510 */

		/*
		 * Now check if we have a multifile image
		 */
	} else if ((hdr->ih_type == IH_TYPE_MULTI) && (len_ptr[1])) {
		ulong tail = ntohl (len_ptr[0]) % 4;
		int i;

		SHOW_BOOT_PROGRESS (13);

		/* skip kernel length and terminator */
		data = (ulong) (&len_ptr[2]);
		/* skip any additional image length fields */
		for (i = 1; len_ptr[i]; ++i)
			data += 4;
		/* add kernel length, and align */
		data += ntohl (len_ptr[0]);
		if (tail) {
			data += 4 - tail;
		}

		len = ntohl (len_ptr[1]);

	} else {
		/*
		 * no initrd image
		 */
		SHOW_BOOT_PROGRESS (14);

		len = data = 0;
	}

#ifdef	DEBUG
	if (!data) {
		printf ("No initrd\n");
	}
#endif

	if (data) {
		initrd_start = data;
		initrd_end = initrd_start + len;
	} else {
		initrd_start = 0;
		initrd_end = 0;
	}

	SHOW_BOOT_PROGRESS (15);

	debug ("## Transferring control to Linux (at address %08lx) ...\n",
	       (ulong) theKernel);

#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	/*Calculate boot params offset*/	
	bd->bi_boot_params = ntohl(hdr->ih_ep)-0x8000+0x100;
#endif

#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_LCD) || \
    defined (CONFIG_VFD) || \
    defined (CONFIG_MARVELL_TAG) || \
    defined(ARUBA_ARM)
	setup_start_tag (bd);
#ifdef CONFIG_SERIAL_TAG
	setup_serial_tag (&params[cpuindex]);
#endif
#ifdef CONFIG_REVISION_TAG
	setup_revision_tag (&params[cpuindex]);
#endif
#ifdef CONFIG_SETUP_MEMORY_TAGS
	setup_memory_tags (bd);
#endif
#ifdef CONFIG_CMDLINE_TAG
	setup_commandline_tag (bd, commandline);
#endif
#ifdef CONFIG_INITRD_TAG
	if (initrd_start && initrd_end)
		setup_initrd_tag (bd, initrd_start, initrd_end);
#endif
#if defined (CONFIG_VFD) || defined (CONFIG_LCD)
	setup_videolfb_tag ((gd_t *) gd);
#endif
#if defined (CONFIG_MARVELL_TAG)
        /* Linux open port doesn't support the Marvell TAG */
	char *env = getenv("mainlineLinux");
	if(!env || ((strcmp(env,"no") == 0) ||  (strcmp(env,"No") == 0)))
	    setup_marvell_tag ();
#endif
#if defined(ARUBA_ARM)
        setup_aruba_tag();
#ifdef __ARUBA_WIRELESS_MAC__
        setup_aruba_wmac_tag();
#endif	/* __ARUBA_WIRELESS_MAC__ */
#endif
	setup_end_tag (bd);
#endif

#ifndef ARUBA_ARM
	/* we assume that the kernel is in place */
	printf ("\nStarting kernel ...\n\n");
#endif
	
#ifdef CONFIG_USB_DEVICE
	{
		extern void udc_disconnect (void);
		udc_disconnect ();
	}
#endif

	cleanup_before_linux ();
#if defined(CONFIG_MARVELL) && defined(DUAL_OS_SHARED_MEM_78200)
	mvSemaUnlock(1);
#endif	

#if 0
printf("jumping to %p, with %x, %x\n", theKernel, bd->bi_arch_number, bd->bi_boot_params);
printf("%p %p %x\n", params[cpuindex], bd->bi_boot_params, (unsigned)((unsigned)params[cpuindex] - (unsigned)bd->bi_boot_params));
bdump((void *)bd->bi_boot_params, (unsigned)((unsigned)params[cpuindex] - (unsigned)bd->bi_boot_params));
#endif
	theKernel (0, bd->bi_arch_number, bd->bi_boot_params);
}


#if defined (CONFIG_SETUP_MEMORY_TAGS) || \
    defined (CONFIG_CMDLINE_TAG) || \
    defined (CONFIG_INITRD_TAG) || \
    defined (CONFIG_SERIAL_TAG) || \
    defined (CONFIG_REVISION_TAG) || \
    defined (CONFIG_LCD) || \
    defined (CONFIG_VFD) || \
    defined (CONFIG_MARVELL_TAG) || \
    defined(ARUBA_ARM)
static void setup_start_tag (bd_t *bd)
{
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif
	params[cpu] = (struct tag *) bd->bi_boot_params;
	params[cpu]->hdr.tag = ATAG_CORE;
	params[cpu]->hdr.size = tag_size (tag_core);

	params[cpu]->u.core.flags = 0;
	params[cpu]->u.core.pagesize = 0;
	params[cpu]->u.core.rootdev = 0;

	params[cpu] = tag_next (params[cpu]);
}


#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags (bd_t *bd)
{
	int i;
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		params[cpu]->hdr.tag = ATAG_MEM;
		params[cpu]->hdr.size = tag_size (tag_mem32);

		params[cpu]->u.mem.start = bd->bi_dram[i].start;
		params[cpu]->u.mem.size = bd->bi_dram[i].size;

		params[cpu] = tag_next (params[cpu]);
	}
}
#endif /* CONFIG_SETUP_MEMORY_TAGS */


static void setup_commandline_tag (bd_t *bd, char *commandline)
{
	char *p;
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params[cpu]->hdr.tag = ATAG_CMDLINE;
	params[cpu]->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params[cpu]->u.cmdline.cmdline, p);
	params[cpu] = tag_next (params[cpu]);
}


#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag (bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params[cpu]->hdr.tag = ATAG_INITRD2;
	params[cpu]->hdr.size = tag_size (tag_initrd);

	params[cpu]->u.initrd.start = initrd_start;
	params[cpu]->u.initrd.size = initrd_end - initrd_start;

	params[cpu] = tag_next (params[cpu]);
}
#endif /* CONFIG_INITRD_TAG */


#if defined (CONFIG_VFD) || defined (CONFIG_LCD)
extern ulong calc_fbsize (void);
static void setup_videolfb_tag (gd_t *gd)
{
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	/* An ATAG_VIDEOLFB node tells the kernel where and how large
	 * the framebuffer for video was allocated (among other things).
	 * Note that a _physical_ address is passed !
	 *
	 * We only use it to pass the address and size, the other entries
	 * in the tag_videolfb are not of interest.
	 */
	params[cpu]->hdr.tag = ATAG_VIDEOLFB;
	params[cpu]->hdr.size = tag_size (tag_videolfb);

	params[cpu]->u.videolfb.lfb_base = (u32) gd->fb_base;
	/* Fb size is calculated according to parameters for our panel
	 */
	params[cpu]->u.videolfb.lfb_size = calc_fbsize();

	params[cpu] = tag_next (params[cpu]);
}
#endif /* CONFIG_VFD || CONFIG_LCD */

#if defined(CONFIG_MARVELL_TAG)
static void setup_marvell_tag (void)
{
	char *env;
	char temp[20];
	int i;
	unsigned int boardId;
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	params[cpu]->hdr.tag = ATAG_MARVELL;
	params[cpu]->hdr.size = tag_size (tag_mv_uboot);
	memset(&params[cpu]->u.mv_uboot, 0, sizeof(params[cpu]->u.mv_uboot));

	params[cpu]->u.mv_uboot.uboot_version = VER_NUM;
    if(strcmp(getenv("nandEcc"), "4bit") == 0)
    {
        params[cpu]->u.mv_uboot.nand_ecc = 4;
    }
    else if(strcmp(getenv("nandEcc"), "1bit") == 0)
    {
        params[cpu]->u.mv_uboot.nand_ecc = 1;
    }

	extern unsigned int mvBoardIdGet(void);	

	boardId = mvBoardIdGet();
	params[cpu]->u.mv_uboot.uboot_version |= boardId;

	params[cpu]->u.mv_uboot.tclk = CFG_TCLK;
	params[cpu]->u.mv_uboot.sysclk = CFG_BUS_CLK;
	
#if defined(MV78XX0)
	/* Dual CPU Firmware load address */
        env = getenv("fw_image_base");
        if(env)
		params[cpu]->u.mv_uboot.fw_image_base = simple_strtoul(env, NULL, 16);
	else
		params[cpu]->u.mv_uboot.fw_image_base = 0;

	/* Dual CPU Firmware size */
        env = getenv("fw_image_size");
        if(env)
		params[cpu]->u.mv_uboot.fw_image_size = simple_strtoul(env, NULL, 16);
	else
		params[cpu]->u.mv_uboot.fw_image_size = 0;
#endif

#if defined(MV_INCLUDE_USB)
    extern unsigned int mvCtrlUsbMaxGet(void);

    for (i = 0 ; i < mvCtrlUsbMaxGet(); i++)
    {
	sprintf( temp, "usb%dMode", i);
	env = getenv(temp);
	if((!env) || (strcmp(env,"Host") == 0 ) || (strcmp(env,"host") == 0) )
		params[cpu]->u.mv_uboot.isUsbHost |= (1 << i);
	else
		params[cpu]->u.mv_uboot.isUsbHost &= ~(1 << i);
    }
#endif /*#if defined(MV_INCLUDE_USB)*/
#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH)
	extern unsigned int mvCtrlEthMaxPortGet(void);
	extern int mvMacStrToHex(const char* macStr, unsigned char* macHex);

	for (i = 0 ;i < 4;i++)
	{
		memset(params[cpu]->u.mv_uboot.macAddr[i], 0, sizeof(params[cpu]->u.mv_uboot.macAddr[i]));
		params[cpu]->u.mv_uboot.mtu[i] = 0; 
	}

	for (i = 0 ;i < mvCtrlEthMaxPortGet();i++)
	{
        sprintf( temp,(i ? "eth%daddr" : "ethaddr"), i);

        env = getenv(temp);
	    if (env)
		mvMacStrToHex(env, params[cpu]->u.mv_uboot.macAddr[i]);

        sprintf( temp,(i ? "eth%dmtu" : "ethmtu"), i);

        env = getenv(temp);
        if (env)
            params[cpu]->u.mv_uboot.mtu[i] = simple_strtoul(env, NULL, 10); 
	}
#endif /* (MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH) */

	params[cpu] = tag_next (params[cpu]);
}	
#endif

#if defined(ARUBA_ARM)
static void
setup_aruba_tag(void)
{
	unsigned int cpu = 0;
	extern char build_num[], version_string[];
	extern int apboot_os_partition;
	extern unsigned gBoardId;

#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif
	params[cpu]->hdr.tag = ATAG_ARUBA_UBOOT;
	params[cpu]->hdr.size = tag_size (tag_aruba_uboot);

	params[cpu]->u.aruba_uboot.au_flash.size = CFG_FLASH_SIZE;
	params[cpu]->u.aruba_uboot.au_flash.start = CFG_FLASH_BASE;
	params[cpu]->u.aruba_uboot.au_booted_partition = apboot_os_partition;
	params[cpu]->u.aruba_uboot.au_flags = 0;
	params[cpu]->u.aruba_uboot.au_spare1 = gBoardId;
	params[cpu]->u.aruba_uboot.au_spare2 = 0;

#ifdef CONFIG_APBOOT
        snprintf(params[cpu]->u.aruba_uboot.au_bootver,
		sizeof(params[cpu]->u.aruba_uboot.au_bootver),
		"%s/%s", version_string, build_num);
#else
        snprintf(params[cpu]->u.aruba_uboot.au_bootver,
		sizeof(params[cpu]->u.aruba_uboot.au_bootver),
		"%s/%s", "unknown", "unknown");
#endif
#ifdef ARUBA_BOARD_TYPE
	snprintf(params[cpu]->u.aruba_uboot.au_boardname,
		sizeof(params[cpu]->u.aruba_uboot.au_boardname),
		"%s", ARUBA_BOARD_TYPE);
#else
	snprintf(params[cpu]->u.aruba_uboot.au_boardname,
		sizeof(params[cpu]->u.aruba_uboot.au_boardname),
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
			snprintf(params[cpu]->u.aruba_uboot.au_variant,
				sizeof(params[cpu]->u.aruba_uboot.au_variant),
				"%.*s", l, v);
		} else {
			snprintf(params[cpu]->u.aruba_uboot.au_variant,
				sizeof(params[cpu]->u.aruba_uboot.au_variant),
				"%s", "unknown");
		}
#ifdef notyet
		l = sizeof(v);
		r = invent_get_card_tlv(inventory_handle, 0, 
			INVENT_TLV_WIRED_MAC_COUNT, v, &l);
		if (r == 0) {
			params[cpu]->u.aruba_uboot.au_spare2 = *(unsigned short *)v;
		} else {
			params[cpu]->u.aruba_uboot.au_spare2 = 0;
		}
#endif
	}
#endif

	params[cpu] = tag_next (params[cpu]);
}

#ifdef __ARUBA_WIRELESS_MAC__
static void
setup_aruba_wmac_tag(void)
{
	unsigned int cpu = 0;

#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif
	params[cpu]->hdr.tag = ATAG_ARUBA_WMAC;
	params[cpu]->hdr.size = tag_size (tag_aruba_wmac);

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
			memcpy(params[cpu]->u.aruba_wmac.aw_wmac, v, 6);
		} else {
			memset(params[cpu]->u.aruba_wmac.aw_wmac, 0, 6);
		}
		r = invent_get_card_tlv(inventory_handle, 0, 
			INVENT_TLV_WIRELESS_MAC_COUNT, v, &l);
		if (r == 0) {
			params[cpu]->u.aruba_wmac.aw_count = *(unsigned short *)v;
		} else {
			params[cpu]->u.aruba_wmac.aw_count = 0;
		}
	}
#endif

	params[cpu] = tag_next (params[cpu]);
}
#endif	/* __ARUBA_WIRELESS_MAC__ */
#endif

#ifdef CONFIG_SERIAL_TAG
void setup_serial_tag (struct tag **tmp)
{
	struct tag *params = *tmp;
	struct tag_serialnr serialnr;
	void get_board_serial(struct tag_serialnr *serialnr);
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	get_board_serial(&serialnr);
	params[cpu]->hdr.tag = ATAG_SERIAL;
	params[cpu]->hdr.size = tag_size (tag_serialnr);
	params[cpu]->u.serialnr.low = serialnr.low;
	params[cpu]->u.serialnr.high= serialnr.high;
	params[cpu] = tag_next (params[cpu]);
	*tmp = params;
}
#endif

#ifdef CONFIG_REVISION_TAG
void setup_revision_tag(struct tag **in_params)
{
	u32 rev = 0;
	u32 get_board_rev(void);
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	rev = get_board_rev();
	params[cpu]->hdr.tag = ATAG_REVISION;
	params[cpu]->hdr.size = tag_size (tag_revision);
	params[cpu]->u.revision.rev = rev;
	params[cpu] = tag_next (params[cpu]);
}
#endif  /* CONFIG_REVISION_TAG */


static void setup_end_tag (bd_t *bd)
{
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	params[cpu]->hdr.tag = ATAG_NONE;
	params[cpu]->hdr.size = 0;
}

#endif /* CONFIG_SETUP_MEMORY_TAGS || CONFIG_CMDLINE_TAG || CONFIG_INITRD_TAG */
