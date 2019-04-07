// vim:set ts=4 sw=4:
#include <config.h>
#ifdef CONFIG_APBOOT
/*
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

/*
 * Aruba AP boot functions
 */

#include <common.h>
#include <command.h>
#include <lstring.h>
#include <flash.h>
#include <aruba_image.h>
#include <aruba_apboot.h>
#include <watchdog.h>
#include <asm-mips/global_data.h>
#include "net.h"
#include "md5.h"
#ifdef CONFIG_BZIP2
#include "bzlib.h"
#endif
#ifdef CONFIG_CMD_UBI
#include <ubi_uboot.h>
#endif

#ifdef AP_CAL_IN_FLASH
/* K2 / Springbank Calibration data is stored on flash. 
    * Calibration address:
     *          2.4G: 0x1ffe1000
      *            5G: 0x1ffe5000
       */
#define FLASH_CAL_OFFSET_2G      0x1000
#define FLASH_CAL_OFFSET_5G      0x5000

#define FLASH_CAL_HEADER_SIZE    0x10
#define FLASH_CAL_DATA_SIZE      0x1000

#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
extern void iproc_set_bank(unsigned);
#endif

short const sercomm_crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

typedef struct {
    unsigned char magic[6];
    unsigned char crc[6];
    unsigned char crc1;
    unsigned char crc2;
    unsigned char crc3;
    unsigned char crc4;
} aruba_cal_header;

static inline unsigned short
crc16_byte(unsigned short crc, const unsigned char data)
{
    return (crc >> 8) ^ sercomm_crc16_table[(crc ^ data) & 0xff];
}

static unsigned short
crc16(unsigned short crc, unsigned char const *buffer, size_t len)
{
    while (len--)
        crc = crc16_byte(crc, *buffer++);
    return crc;
}

static unsigned short  
hextochar(unsigned char data)
{   
    if((data >= 0x30)&&(data <=0x39))
        data -= 0x30;
    else if((data >= 0x41)&&(data <=0x46))
        data -= 0x37; 
    else if((data >= 0x61)&&(data <=0x66))
        data -= 0x57;
    else
        data = 0xff;
    return (unsigned short)data;
}

static int 
do_checkcal(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    aruba_cal_header *cal_hdr;
    unsigned short crc_s, crc_cal;
    int i;

    for (i = 0; i < 2; i++) {
        cal_hdr = (aruba_cal_header *)(CFG_MANUF_BASE + (i ? FLASH_CAL_OFFSET_5G : FLASH_CAL_OFFSET_2G));
#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
        if ((unsigned long)cal_hdr >= CONFIG_FLASH_MAP_SPLIT_ADDR) {
            iproc_set_bank(2);
        }
#endif
        printf("%s: ", i?"  5G":"2.4G");
        if (memcmp(cal_hdr->magic, "Aruba", 5) == 0) {
            /* verify the cal data */
            crc_s = ((hextochar(cal_hdr->crc1) << 12) & 0xf000) | 
                    ((hextochar(cal_hdr->crc2) << 8) & 0x0f00) |
                    ((hextochar(cal_hdr->crc3) << 4) & 0x00f0) |
                    (hextochar(cal_hdr->crc4) & 0x000f);
            crc_cal = crc16(0, (unsigned char *)cal_hdr + FLASH_CAL_HEADER_SIZE, FLASH_CAL_DATA_SIZE);

            if (crc_cal != crc_s) {
                printf("CRC check error, ");
            } else {
                printf("CRC check OK, ");
            }
            
            printf("stored: 0x%04x, calculated: 0x%04x\n", crc_s, crc_cal);

        } else {
            printf("No calibration data\n");
        }
#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
        iproc_set_bank(1);
#endif

    }

    return 0;
}
#endif

#ifdef QCA_CAL_IN_FLASH
/* For Octomore, the radio calibration data in flash */
#define FLASH_CAL_OFFSET_5G      0x1000
#define FLASH_CAL_OFFSET_2G      0x5000
#define FLASH_CAL_MAX_SIZE       0x9000

static int
do_checkcal(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int i, j;
    unsigned short sum, len, sum_stored;
    unsigned short *buf = (unsigned short *)CFG_LOAD_ADDR;

    aruba_flash_read((void *)CFG_ART_BASE, (unsigned char *)buf, FLASH_CAL_MAX_SIZE);

    for (i = 0; i < 2; i++) {
        printf("%s: ", i?"  5G":"2.4G");
        buf = (unsigned short *)(CFG_LOAD_ADDR + (i ? FLASH_CAL_OFFSET_5G:FLASH_CAL_OFFSET_2G));

        sum = 0;
        len = *buf;
        sum_stored = *(buf + 1);

        if (len == 0xffff) {
            printf("No calibration data\n");
            continue;
        }
        if (len <= FLASH_CAL_MAX_SIZE) {
            for (j = 0; j < len / 2; j++) {
                sum ^= *buf++;
            }
        }

        if (sum != 0xffff) {
            sum = sum ^ 0xffff;
            printf("CRC check error, ");
        } else {
            sum = sum_stored;
            printf("CRC check OK, ");
        }

        printf("stored: 0x%04x, calculated: 0x%04x\n", sum_stored, sum);
    }

    return 0;
}
#endif

int apboot_os_partition = -1;

static void
__do_osinfo_common(long base, unsigned size, unsigned partition)
{
	int valid;
	aruba_image_t *ai;

    WATCHDOG_RESET();

	printf("Partition %u", partition);
#ifdef AP_SLOW_FLASH_STAGING_AREA
	base = (long)aruba_copy_to_staging_area((void *)base, 
			(void *)AP_SLOW_FLASH_STAGING_AREA, size, 1);
#endif

	ai = (aruba_image_t *)base;

	if (!strncmp(ARUBA_IMAGE_MAGIC, ai->magic, strlen(ARUBA_IMAGE_MAGIC))) {
		printf(":\n");
		printf("    image type: %u\n", ai->type);
		printf("  machine type: %u\n", ai->machineType);
		printf("          size: %u\n", ntohl(ai->size) + ARUBA_HEADER_SIZE);
		printf("       version: %s\n", ai->arubaVersion);
		printf("  build string: %s\n", ai->buildString);
		printf("         flags: %s\n", aruba_osinfo_flags(ntohl(ai->flags)));
		printf("           oem: %s\n", ai->u.aos_info.oem);

        WATCHDOG_RESET();

		valid = aruba_basic_image_verify((void *)base,
			__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0);
		if (!valid) {
#if defined(__SIGNED_IMAGES__)
	        if (image_verify(ai)) {
				;
	        }
#endif
		}
	} else {
		printf(" does not contain a valid OS image\n", partition);
		return;
	}
}

#if AP_PROVISIONING_IN_BANK2
int
__do_bank(cmd_tbl_t *cmdtp, int flag, int bank_num)
{
	int rc;
	char *av[3];
	char bank[2];

	sprintf(bank, "%d", bank_num);
	av[0] = "bank";
	av[1] = bank;
	av[2] = 0;
	rc = do_bank(cmdtp, flag, 2, av);
	return rc;
}
#endif

static int
do_osinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if AP_PROVISIONING_IN_BANK2
	__do_bank(cmdtp, flag, 1);
#endif
	__do_osinfo_common(AP_PRODUCTION_IMAGE, AP_PRODUCTION_IMAGE_SIZE, 0);

#ifdef AP_PROVISIONING_IMAGE
    printf("\n");
#if AP_PROVISIONING_IN_BANK2
	__do_bank(cmdtp, flag, 2);
	__do_osinfo_common(AP_PROVISIONING_IMAGE, AP_PROVISIONING_IMAGE_SIZE, 1);
	__do_bank(cmdtp, flag, 1);
#else
	__do_osinfo_common(AP_PROVISIONING_IMAGE, AP_PROVISIONING_IMAGE_SIZE, 1);
#endif
#endif
	return 0;
}

#ifndef CONFIG_STAGE1
int 
do_boot_ap(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *av[6];
	int r;
	char *p = getenv("bootcmd");
	long base = AP_PRODUCTION_IMAGE;
	long boot_first = base;
#ifdef AP_PROVISIONING_IMAGE
	long boot_second = AP_PROVISIONING_IMAGE;
#endif
#if AP_PROVISIONING_IN_BANK2
	long boot_first_bank = 1;
	long boot_second_bank = 2;
#endif
#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT)
	int usb = 0;
#endif

#if AP_PROVISIONING_IN_BANK2
	__do_bank(cmdtp, flag, 1);
#endif
	if (argc == 1) {
		if (!p) {
			printf("bootcmd environment variable not found\n");
			return 1;
		}
		if (!strncmp(p, "tftp", 4)) {
			goto netboot;
		}
		if (!strcmp(p, "boot ap")) {
			goto flashboot;
		}
	}

#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT)
	if (argc == 2 && strcmp(argv[1], "ap") && strncmp(argv[1], "usb:", 4)) {
#else
	if (argc == 2 && strcmp(argv[1], "ap")) {
#endif
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT) && defined(powerpc)
	if (argc == 2 && !strncmp(argv[1], "usb:", 4)) {
		r = load_image_from_usb((void *)CONFIG_SYS_LOAD_ADDR, argv[1] + 4);
		if (r == 1) {
			return 1;
		}
		base = CONFIG_SYS_LOAD_ADDR;
		usb = 1;
		apboot_os_partition = -1;
		goto usbboot;
	}
#endif

flashboot:
	WATCHDOG_RESET();

	boot_first = AP_PRODUCTION_IMAGE;
	apboot_os_partition = 0;
#ifdef AP_PROVISIONING_IMAGE
	boot_second = AP_PROVISIONING_IMAGE;
#if AP_PROVISIONING_IN_BANK2
	boot_first_bank = 1;
	boot_second_bank = 2;
#endif
	p = getenv("os_partition");
	if (p) {
		if (!strcmp(p, "1")) {
			printf("Booting OS partition 1\n");
			apboot_os_partition = 1;
			boot_first = AP_PROVISIONING_IMAGE;
			boot_second = AP_PRODUCTION_IMAGE;
#if AP_PROVISIONING_IN_BANK2
			boot_first_bank = 2;
			boot_second_bank = 1;
#endif
		}  else  {
			printf("Booting OS partition 0\n");
		}
	}
#endif

#if AP_PROVISIONING_IN_BANK2
	__do_bank(cmdtp, flag, boot_first_bank);
	printf("Checking image @ 0x%x (bank %d)\n", boot_first, boot_first_bank);
#else
	printf("Checking image @ 0x%x\n", boot_first);
#endif

	base = boot_first;

#ifdef AP_SLOW_FLASH_STAGING_AREA
		base = (long)aruba_copy_to_staging_area((void *)base, 
			(void *)AP_SLOW_FLASH_STAGING_AREA, AP_PRODUCTION_IMAGE_SIZE, 0);
#endif

#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT) && defined(powerpc)
usbboot:
#endif

	if (aruba_basic_image_verify((void *)base,
		__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0)) {
#ifdef AP_PROVISIONING_IMAGE
		apboot_os_partition = 1 - apboot_os_partition;  /* swap (0/1) */
#if AP_PROVISIONING_IN_BANK2
		__do_bank(cmdtp, flag, boot_second_bank);
		printf("Checking image @ 0x%x (bank %d)\n", boot_second, boot_second_bank);
#else
		printf("Checking image @ 0x%x\n", boot_second);
#endif
		base = boot_second;
#ifdef AP_SLOW_FLASH_STAGING_AREA
		base = (long)aruba_copy_to_staging_area((void *)base, 
			(void *)AP_SLOW_FLASH_STAGING_AREA, AP_PROVISIONING_IMAGE_SIZE, 0);
#endif
		if (aruba_basic_image_verify((void *)base,
			__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0)) {
#if AP_PROVISIONING_IN_BANK2
            /* if the image in the second bank is invalid, we need switch back to the first bank */
            if (boot_second_bank == 2) {
                __do_bank(cmdtp, flag, 1);
            }
#endif
			goto netboot;
		}
#else
		goto netboot;
#endif
	}
#if defined(__SIGNED_IMAGES__)
	if (image_verify((aruba_image_t *)base)) {
		goto netboot;
	}
#endif

	r = boot_aruba_image(base + ARUBA_HEADER_SIZE);
	if (r) {
netboot:
#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT)
		if (usb) {
			return 1;
		}
#endif
		apboot_os_partition = -1;
		//
		// if we get here, something has failed, so now would be an
		// excellent time to try TFTP
		//
		av[0] = "tftpboot";
		av[1] = 0;
		r =  do_tftpboot(cmdtp, flag, 1, av);
	}
	return r;
}
#endif

#ifndef CONFIG_STAGE1
//
// this routine assumes a 128KB sector size, which is not
// really a good idea
//
static int
__do_upgrade_common(
	cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], unsigned addr,
	int copy_hdr, int protected, int machine, int kind, int max_size
)
{
	char *av[10];
	int r, i;
	char *p = getenv("autostart");
	int old_auto = 0;
	unsigned ramaddr;
	extern char BootFile[];
	extern IPaddr_t NetServerIP;
	char *sp = 0;
	char abuf[64], rbuf[32];
	extern IPaddr_t string_to_ip(char *);
#ifndef CONFIG_CMD_NAND
	flash_info_t *fl;
#endif
 	unsigned store_len;
	unsigned end;

#ifndef CFG_TFTP_STAGING_AREA
    DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	ramaddr = bd->bi_uboot_ram_addr + bd->bi_uboot_ram_used_size;
	ramaddr += (1024 * 1024);
#else
	ramaddr = CFG_TFTP_STAGING_AREA;
#endif

	if (p && !strcmp(p, "yes")) {
		old_auto = 1;
		setenv("autostart", "no");
	}

	sprintf(rbuf, "0x%x", ramaddr);

	av[0] = "tftp";
	av[1] = rbuf;
	av[2] = argv[2];
	av[3] = 0;

#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT) && defined(powerpc)
	if (!strncmp(argv[2], "usb:", 4)) {
		r = load_image_from_usb((void *)ramaddr, argv[2] + 4);
		if (r != 0) {
			goto out;
		}
	} else {
#endif
		p = strrchr(argv[2], ':');
		if (!p) {
			p = argv[2];
		} else {
			*p = '\0';
			p++;
			sp = getenv("serverip");
			if (sp) {
				strncpy(abuf, sp, sizeof(abuf) - 1);
				abuf[sizeof(abuf) - 1] = '\0';
			}
			NetServerIP = string_to_ip(argv[2]);
			setenv("serverip", argv[2]);
			av[2] = p;
		}
		strncpy(BootFile, p, 128);
		BootFile[127] = '\0';

		r = do_tftpb(cmdtp, flag, 3, av);

		if (r != 0) {
			goto out;
		}
#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT) && defined(powerpc)
	}
#endif

	if ((aruba_basic_image_verify((void *)ramaddr, machine, kind, &store_len,
		copy_hdr))) {
		r = 1;
		goto out;
	}

#if defined(__SIGNED_IMAGES__)
	if (image_verify((aruba_image_t *)ramaddr)) {
		r = 1;
		goto out;
	}
#endif

	if (store_len > max_size) {
		printf("Image is too big; (%u > %u)\n", store_len, max_size);
		r = 1;
		goto out;
	}

	i = 0;
#ifndef CONFIG_CMD_NAND
	fl = &flash_info[0];
	while (fl->start[i] < addr) {
		i++;
	}
	while (fl->start[i] < (addr + store_len) && (i < CFG_MAX_FLASH_SECT)) {
		i++;
	}

	/* handle the case where we are up against the end of the flash */
	if (i == CFG_MAX_FLASH_SECT) {
		end = CFG_FLASH_BASE + CFG_FLASH_SIZE - 1;
	} else {
		end = fl->start[i] - 1;
	}
#else
    end = addr + store_len - 1;
#endif
	r = aruba_copy_and_verify(cmdtp, flag, ramaddr, addr, end, store_len, 
		copy_hdr, protected);

out:
	if (old_auto) {
		setenv("autostart", "yes");
	}
	if (sp) {
		setenv("serverip", abuf);
	}
	if (r == 0) {
		printf("Upgrade successful.\n");
	} else {
		printf("**** ERROR: upgrade failed ****.\n");
	}
	return r;
}

static int
__do_upgrade_boot(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return __do_upgrade_common(cmdtp, flag, argc, argv, APBOOT_IMAGE, 0, 1,
		__BOOT_IMAGE_TYPE, ARUBA_IMAGE_TYPE_APBOOT_BINARY, APBOOT_SIZE);
}

#if defined(CONFIG_STAGE2)
static int
__do_upgrade_stage2_bank1(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return __do_upgrade_common(cmdtp, flag, argc, argv, CFG_STAGE2_BANK1, 1, 1,
		__BOOT_IMAGE_TYPE, ARUBA_IMAGE_TYPE_APBOOT_STAGE2_BINARY, CFG_STAGE2_MAX_FLASH_SIZE);
}

static int
__do_upgrade_stage2_bank2(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return __do_upgrade_common(cmdtp, flag, argc, argv, CFG_STAGE2_BANK2, 1, 1,
		__BOOT_IMAGE_TYPE, ARUBA_IMAGE_TYPE_APBOOT_STAGE2_BINARY, CFG_STAGE2_MAX_FLASH_SIZE);
}
#endif

static int
__do_upgrade_os(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return __do_upgrade_common(cmdtp, flag, argc, argv, AP_PRODUCTION_IMAGE,
		1, 0, __OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, AP_PRODUCTION_IMAGE_SIZE);
}

#ifdef AP_PROVISIONING_IMAGE 
static int
__do_upgrade_prov(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if AP_PROVISIONING_IN_BANK2
	int rc;
	__do_bank(cmdtp, flag, 2);
	rc = __do_upgrade_common(cmdtp, flag, argc, argv, AP_PROVISIONING_IMAGE,
		1, 0, __OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, AP_PROVISIONING_IMAGE_SIZE);
	__do_bank(cmdtp, flag, 1);
	return rc;
#else
	return __do_upgrade_common(cmdtp, flag, argc, argv, AP_PROVISIONING_IMAGE,
		1, 0, __OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, AP_PROVISIONING_IMAGE_SIZE);
#endif
}
#endif

int
do_upgrade(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if AP_PROVISIONING_IN_BANK2
	__do_bank(cmdtp, flag, 1);
#endif
    if ((argc != 3) && (argc != 4)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
    }
	if (!strcmp(argv[1], "boot") && (argc == 3)) {
		return __do_upgrade_boot(cmdtp, flag, argc, argv);
	}
	if (!strcmp(argv[1], "os") && (argc == 3)) {
		return __do_upgrade_os(cmdtp, flag, argc, argv);
	}
#ifdef AP_PROVISIONING_IMAGE
	if (!strcmp(argv[1], "os") && (argc == 4)) {
		char *av[] = { argv[0], argv[1], argv[3] };  /* remove partition number */
		if (!strcmp(argv[2], "0"))
			return __do_upgrade_os(cmdtp, flag, 3, av);
		else if (!strcmp(argv[2], "1"))
			return __do_upgrade_prov(cmdtp, flag, 3, av);
	}
	if (!strcmp(argv[1], "prov") && (argc == 3)) {
		return __do_upgrade_prov(cmdtp, flag, argc, argv);
	}
#endif
#if defined(CONFIG_STAGE2)
	//
	// allow these only for stage 2, as diag boot loader is too big and will
	// overwrite itself
	//
	if (!strcmp(argv[1], "stage2-bank1")) {
		return __do_upgrade_stage2_bank1(cmdtp, flag, argc, argv);
	}
	if (!strcmp(argv[1], "stage2-bank2")) {
		return __do_upgrade_stage2_bank2(cmdtp, flag, argc, argv);
	}
#endif
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
}
#endif

//
// these routines make a number of assumptions about the flash map, so
// be careful when porting to new platforms
//
int
__do_clear_image_common(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], long base)
{
#ifndef CONFIG_CMD_UBI
	char *av[5];
	char sbuf[16], ebuf[16];
	flash_info_t *fl;
	int i = 0;

	fl = &flash_info[0];
	while (fl->start[i] < base) {
		i++;
	}

	sprintf(sbuf, "0x%x", fl->start[i]);
	sprintf(ebuf, "0x%x", fl->start[i + 1] - 1);

	av[0] = "erase";
	av[1] = sbuf;
	av[2] = ebuf;
	av[3] = 0;
//printf("%s %s %s\n", av[0], av[1], av[2]);
	printf("Erasing flash sector @ %s...", sbuf);
	return do_flerase(cmdtp, flag, 3, av);
#else
    u8 buf[ARUBA_HEADER_SIZE];

    memset(buf, sizeof(buf), 0xff);

    base = (long)aruba_ubi_image((void *)buf, (void *)base, sizeof(buf), 0);

    if (base != (long)buf) {
        return 1;
    } else {
        return 0;
    }
#endif
}

int
__do_clear_os(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#ifdef AP_PROVISIONING_IMAGE
	/* "clear all" honors DONT_CLEAR flag in image header   */
	/* "clear os" forces an erase, ignoring DONT_CLEAR flag */
	if ((argc >= 2) && (strcmp(argv[1], "os") != 0))  {
		printf("Checking OS image and flags\n");
#ifdef CONFIG_CMD_UBI
        void *base = NULL;
#ifdef AP_SLOW_FLASH_STAGING_AREA
        base = aruba_copy_to_staging_area((void *)AP_PRODUCTION_IMAGE, 
                (void *)AP_SLOW_FLASH_STAGING_AREA, AP_PRODUCTION_IMAGE_SIZE, 1);
		if (!aruba_basic_image_verify(base,
				__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0)) {
			aruba_image_t *ai;
			ai = (aruba_image_t *)base;
#endif
#else
		if (!aruba_basic_image_verify((void *)AP_PRODUCTION_IMAGE,
				__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0)) {
			aruba_image_t *ai;
			ai = (aruba_image_t *)AP_PRODUCTION_IMAGE;
#endif
			if (ntohl(ai->flags) & ARUBA_IMAGE_FLAGS_DONT_CLEAR_ON_PURGE)  {
				printf("Preserving image partition 0\n");
				return 1;
			}  else  {
				printf("Clearing image partition 0\n");
			}
		}  else  {
			printf("Continuing with OS clear\n");
		}
	}
#endif
	return __do_clear_image_common(cmdtp, flag, argc, argv, 
		AP_PRODUCTION_IMAGE);
}

#ifdef AP_PROVISIONING_IMAGE
int
__do_clear_prov(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if AP_PROVISIONING_IN_BANK2
	int rc;
	__do_bank(cmdtp, flag, 2);
	rc = __do_clear_image_common(cmdtp, flag, argc, argv, AP_PROVISIONING_IMAGE);
	__do_bank(cmdtp, flag, 1);
	return rc;
#else
	return __do_clear_image_common(cmdtp, flag, argc, argv, AP_PROVISIONING_IMAGE);
#endif
}
#endif

int
__do_clear_cache(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *av[5];
	char sbuf[16], ebuf[16];
	int r = 0;
	long base;
	flash_info_t *fl;
	int i = 0;
#ifdef CONFIG_MACH_IPQ806x
    base = APBOOT_IMAGE;
#else
    base = AP_PRODUCTION_IMAGE;
#endif
#ifdef CONFIG_CMD_UBI
    struct ubi_device *ubi;
#endif
	fl = &flash_info[0];
	while (fl->start[i] < base) {
		i++;
	}
	while (i < CFG_MAX_FLASH_SECT 
	       && fl->start[i]
	       && fl->start[i] < (CFG_FLASH_BASE + CFG_FLASH_SIZE)) {

		//
		// skip manufacturing, the environment, and the TPM
		// sector, if there is one
		//
		if (
#ifdef CFG_MANUF_BASE
	 	    fl->start[i] == CFG_MANUF_BASE ||
#endif
		    fl->start[i] == CFG_ENV_ADDR) {
			i++;
			continue;
		}
#ifdef CFG_TPM_CERT
		if (fl->start[i] == CFG_TPM_CERT) {
			i++;
			continue;
		}
#endif
#ifdef powerpc
		/* skip the boot loader which is at the end on FSL */
		if (fl->start[i] >= APBOOT_IMAGE) {
			break;
		}
#endif

#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
        if ((unsigned long)fl->start[i] >= CONFIG_FLASH_MAP_SPLIT_ADDR) {
            iproc_set_bank(2);
        }
#endif

#ifdef CONFIG_MACH_IPQ806x
        aruba_flash_read((void *)fl->start[i], (uchar *)CONFIG_VERIFY_LOAD_ADDR, 16);
		if (memcmp((void *)CONFIG_VERIFY_LOAD_ADDR, "TheCache", 8)) {
#else
		if (memcmp((void *)fl->start[i], "TheCache", 8)) {
#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
            if ((unsigned long)fl->start[i] >= CONFIG_FLASH_MAP_SPLIT_ADDR) {
                iproc_set_bank(1);
            }
#endif
#endif
			i++;
			continue;
		}

		sprintf(sbuf, "0x%x", fl->start[i]);
		sprintf(ebuf, "0x%x", fl->start[i + 1] - 1);

#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
        if ((unsigned long)fl->start[i] >= CONFIG_FLASH_MAP_SPLIT_ADDR) {
            iproc_set_bank(1);
        }
#endif

 	    av[0] = "erase";
	    av[1] = sbuf;
	    av[2] = ebuf;
	    av[3] = 0;
	    printf("Erasing flash sector @ %s...", sbuf);

	    r += do_flerase(cmdtp, flag, 3, av);
		printf("\n");
		i++;
	}
#if defined(CONFIG_CMD_UBI) && defined(CONFIG_UBIFS)
    /* The cache data are stored in UBIFS */
    /* change device to nand */
    av[0] = "nand";
    sprintf(sbuf, "%s", "device");
    sprintf(ebuf, "%d", CONFIG_IPQ_NAND_NAND_INFO_IDX);
    av[1] = sbuf;
    av[2] = ebuf;
    av[3] = 0;
    do_nand(cmdtp, flag, 3, av);

    av[0] = "nand";
    sprintf(sbuf, "%s", "erase.part");
    sprintf(ebuf, "%s", AP_UBI_FS_NAME);
    av[1] = sbuf;
    av[2] = ebuf;
    av[3] = 0;

    printf("Erasing UBIFS ...");
    r = do_nand(cmdtp, flag, 3, av);

    if (r) {
        goto out;
    }

    /* rebuild UBIFS */
    av[0] = "ubi";
    sprintf(sbuf, "%s", "part");
    sprintf(ebuf, "%s", AP_UBI_FS_NAME);
    av[1] = sbuf;
    av[2] = ebuf;
    av[3] = 0;
    r = do_ubi(cmdtp, flag, 3, av);

    if (r) {
        goto out;
    }

    /* remove ubifs volume */
    av[0] = "ubi";
    sprintf(sbuf, "%s", "remove");
    sprintf(ebuf, "%s", AP_UBI_FS_NAME);
    av[1] = sbuf;
    av[2] = ebuf;
    av[3] = 0;
    r = do_ubi(cmdtp, flag, 3, av);

    if (r) {
        goto out;
    }

    /* recreate ubifs volume */
    ubi = ubi_get_device(CONFIG_MTD_MAX_DEVICE - 1);
    av[0] = "ubi";
    av[1] = "create";
    sprintf(sbuf, "%s", AP_UBI_FS_NAME);
    sprintf(ebuf, "%x", (ubi->avail_pebs - 4) * ubi->leb_size);
    av[2] = sbuf;
    av[3] = ebuf;
    av[4] = 0;
    r = do_ubi(cmdtp, flag, 4, av);

    /* change device back to nor */
out:
    av[0] = "nand";
    sprintf(sbuf, "%s", "device");
    sprintf(ebuf, "%d", CONFIG_IPQ_SPI_NAND_INFO_IDX);
    av[1] = sbuf;
    av[2] = ebuf;
    av[3] = 0;
    do_nand(cmdtp, flag, 3, av);

#endif
	return r;
}

int
do_clear(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if AP_PROVISIONING_IN_BANK2
	__do_bank(cmdtp, flag, 1);
#endif
    if ((argc != 2) && (argc != 3)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
    }
	if (!strcmp(argv[1], "os") && ((argc == 2) ||
			((argc == 3) && (!strcmp(argv[2], "0")))))
	{
		return __do_clear_os(cmdtp, flag, argc, argv);
	} else if (!strcmp(argv[1], "cache") && (argc == 2)) {
		return __do_clear_cache(cmdtp, flag, argc, argv);
#ifdef AP_PROVISIONING_IMAGE
	} else if ((!strcmp(argv[1], "prov") && (argc == 2)) ||
			((argc == 3) && (!strcmp(argv[1], "os") && (!strcmp(argv[2], "1")))))
	{
		return __do_clear_prov(cmdtp, flag, argc, argv);
#endif
	} else if (!strcmp(argv[1], "all") && (argc == 2)) {
		int r;
		r = __do_clear_os(cmdtp, flag, argc, argv);
		r += __do_clear_cache(cmdtp, flag, argc, argv);
		return r;
	}

	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

int in_diag_mode = 0;

int
do_diag(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	in_diag_mode = !in_diag_mode;
	return 0;
}

int reboot_on_timeout = 1;

int
do_reboot_on_timeout(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    if (argc < 2) {
		printf ("You must specify 'on' or 'off'\n");
		return 1;
    }
	if (!strcmp(argv[1], "off")) {
        reboot_on_timeout = 0;
        printf("Will not reboot due to idle timeout\n");
	} else {
        reboot_on_timeout = 1;
        printf("Will reboot due to idle timeout\n");
    }

	return 0;
}

int
do_factory_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *av[3];
	int r;
	extern int do_purgeenv(cmd_tbl_t *, int, int, char **);

#if AP_PROVISIONING_IN_BANK2
	__do_bank(cmdtp, flag, 1);
#endif
    if (argc != 1) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
    }

	av[0] = "clear";
	av[1] = "all";
	av[2] = 0;

	printf("Clearing state... ");
	r = do_clear(cmdtp, flag, 2, av);
	printf("done\n");

	printf("Purging environment... ");
	av[0] = "purgeenv";
	av[1] = 0;
	r += do_purgeenv(cmdtp, flag, 1, av);
	printf("done\n");
	return r;
}

#ifdef CONFIG_TWO_STAGE_SUPPORT
static int
__do_stage2_common(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], ulong addr)
{
	char *av[3];
	int i;
	unsigned int unc_len = CFG_STAGE2_MAX_RUNTIME_SIZE;
	aruba_image_t *ai;

	if (argc < 1 || argc > 2) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	printf("Checking for stage 2 @ 0x%x... ", addr);
	ai = (aruba_image_t *)addr;
	if (aruba_basic_image_verify((void *)addr, __BOOT_IMAGE_TYPE, 
		ARUBA_IMAGE_TYPE_APBOOT_STAGE2_BINARY, 0, 0)) {
		printf("not found\n");
		return 1;
	}
	printf("found\n");
#if defined(__SIGNED_IMAGES__)
	if (image_verify((aruba_image_t *)addr)) {
		return 1;
	}
#endif
	if (ai->comp == ARUBA_IMAGE_COMPRESSION_BZIP2) {
		printf ("Uncompressing Stage 2 @ 0x%lx\n", addr);
		/*
		 * If we've got less than 4 MB of malloc() space,
		 * use slower decompression algorithm which requires
		 * at most 2300 KB of memory.
		 */

		i = BZ2_bzBuffToBuffDecompress ((void *)STAGE2_RAM_ADDRESS,
						&unc_len, (void *)addr + sizeof(*ai), ntohl(ai->image_size),
						CFG_MALLOC_LEN < (4096 * 1024), 0);
		if (i != BZ_OK) {
			printf("BUNZIP2 ERROR %d\n", i);
			return 1;
		}
	} else {
		memcpy((void *)STAGE2_RAM_ADDRESS, (void *)(addr + sizeof(*ai)), ntohl(ai->image_size));
	}
	av[0] = "stage2";
	av[1] = 0;

	((ulong (*)(int, char *[]))STAGE2_RAM_ADDRESS)(1, av);

	return 0;
}

#ifdef CFG_STAGING_AREA
static int
check_staging_area(void)
{
	ulong *addr = (ulong *)CFG_STAGING_AREA;

	if (*addr != 0xffffffff) {
		printf("Checking for new stage2 image in staging area... ");
		if (aruba_basic_image_verify((void *)addr, __BOOT_IMAGE_TYPE, 
			ARUBA_IMAGE_TYPE_APBOOT_STAGE2_BINARY, 0, 0)) {
			printf("not found\n");
			return 0;
		}
#if defined(__SIGNED_IMAGES__)
		if (image_verify((aruba_image_t *)addr)) {
			printf("not found\n");
			return 0;
		}
#endif
		printf("found\n");
		return 1;
	}
	printf("No new stage2 image in staging area\n");
	return 0;
}

static ulong
select_stage2_bank(void)
{
	char *s;
	ulong addr;

	s = getenv("stage2_bank");
	if (!s) {
		addr = CFG_STAGE2_BANK1;
	} else {
		addr = simple_strtoul(s, 0, 16);
		if (addr != CFG_STAGE2_BANK1
		    && addr != CFG_STAGE2_BANK2) {
			addr = CFG_STAGE2_BANK1;
		} else {
			if (addr == CFG_STAGE2_BANK1) {
				addr = CFG_STAGE2_BANK2;
			} else if (addr == CFG_STAGE2_BANK2) {
				addr = CFG_STAGE2_BANK1;
			}
		}
	}
	return addr;
}

static void
update_stage2(cmd_tbl_t *cmdtp, int flag)
{
	ulong bank_addr, bank_end;
	int r;
	char abuf[32];
	char *av[4];
	unsigned len;
	extern int do_saveenv(cmd_tbl_t *, int, int, char **);
	extern int do_setenv(cmd_tbl_t *, int, int, char **);

	av[0] = "setenv";
	av[1] = "stage2_bank";
	av[2] = abuf;
	av[3] = 0;

	bank_addr = select_stage2_bank();
	bank_end = bank_addr + CFG_STAGE2_MAX_FLASH_SIZE - 1;

	len = CFG_STAGE2_MAX_FLASH_SIZE;

	r = aruba_copy_and_verify(cmdtp, flag, CFG_STAGING_AREA, bank_addr, bank_end, len, 1, 1);
	if (!r) {
		printf("Upgraded stage 2 @ 0x%lx\n", bank_addr);
	}
	printf("Clearing staging area...");
	// now remove image from staging area if upgrade is successful
	__do_clear_image_common(0, 0, 0, 0, CFG_STAGING_AREA);
	printf("Updating bank variable");
	sprintf(abuf, "0x%x", bank_addr);
	do_setenv(cmdtp, flag, 3, av);
	av[0] = "saveenv";
	av[1] = 0;
	do_saveenv(cmdtp, flag, 1, av);
	printf("done\n");
}
#endif

int
do_stage2(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int r = 1;
	ulong addr = 0xffffffff;
	char *s;

	if (argc < 1 || argc > 2) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (argc == 2) {
		addr = simple_strtoul(argv[1], NULL, 16);
	}

	if (addr == 0xffffffff) {
#ifdef CFG_STAGING_AREA
		if (check_staging_area()) {
			update_stage2(cmdtp, flag);
		}
#endif
		s = getenv("stage2_bank");
		if (s) {
			addr = simple_strtoul(s, 0, 16);
			if (addr == CFG_STAGE2_BANK1 || addr == CFG_STAGE2_BANK2) {
				r = __do_stage2_common(cmdtp, flag, argc, argv, addr);
			}
			// fall through to try both banks in order
		}
		if (__do_stage2_common(cmdtp, flag, argc, argv, CFG_STAGE2_BANK1)) {
			r = __do_stage2_common(cmdtp, flag, argc, argv, CFG_STAGE2_BANK2);
		}
	} else {
		r = __do_stage2_common(cmdtp, flag, argc, argv, addr);
	}
	return r;
}

U_BOOT_CMD(
	stage2,     2,     0,      do_stage2,
	"stage2 - boot 2nd-stage loader\n",
	"- boot 2nd-stage loader\n"
);
#endif

#ifndef CONFIG_STAGE1
U_BOOT_CMD(
	boot,     3,     0,      do_boot_ap,
	"boot - boot the OS image\n",
	"ap\n   - boot the OS image from flash\n"
#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT)
	"boot usb:path\n  - boot the OS image from USB\n"
#endif
);

U_BOOT_CMD(
	upgrade,     4,     0,      do_upgrade,
	"upgrade - upgrade the APBoot or OS image\n",
	"boot <file>\n   - upgrade the APBoot image from <file>\n"
#ifndef AP_PROVISIONING_IMAGE
	"upgrade os <file>\n   - upgrade the OS image from <file>\n"
#else
	"upgrade os [<n>] <file>\n   - upgrade OS in partition <n> from <file>\n"
#endif
#if defined(CONFIG_STAGE2)
    "upgrade stage2-bank1 <file>\n   - upgrade the stage2 firmware in bank 1\n"
    "upgrade stage2-bank2 <file>\n   - upgrade the stage2 firmware in bank 2\n"
#endif
#ifdef AP_PROVISIONING_IMAGE
	"upgrade prov <file>\n   - upgrade provisioning image from <file> (AKA, \"upgrade os 1 <file>\")\n"
#endif
#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT)
	"\n  <file> can be <TFTP-server-IP>:<path> or usb:<path>"
#else
	"\n  <file> can be <TFTP-server-IP>:<path>\n"
#endif
);
#endif

U_BOOT_CMD(
	clear,     3,     0,      do_clear,
	"clear - clear the OS image or other information\n",
	"all\n  - clear the cache and the OS\n"
	"clear cache\n  - clear the cache sectors (mesh, RAP, etc)\n"
#ifndef AP_PROVISIONING_IMAGE
	"clear os\n  - clear the OS image from the flash\n"
#else
	"clear os <n>\n  - clear image from specified partition (default=0)\n"
	"clear prov\n  - clear provisioning image from flash (AKA, \"clear os 1\")\n"
#endif
);

U_BOOT_CMD(
	osinfo,     3,     0,      do_osinfo,
	"osinfo - show the OS image version(s)\n",
);

U_BOOT_CMD(
	diag,     1,     1,      do_diag,
	"diag - display/hide diag commands in help\n",
	"- display/hide diag commands in help\n"
);

U_BOOT_CMD(
	factory_reset,     1,     0,      do_factory_reset,
	"factory_reset - reset to factory defaults\n",
	"- reset to factory defaults\n"
);

U_BOOT_CMD(
	autoreboot,    2,    1,     do_reboot_on_timeout,
	"autoreboot - toggles rebooting due to idle timeout\n",                   
	"<on/off> - toggle rebooting due to idle timeout\n"
);
#if defined(AP_CAL_IN_FLASH) || defined(QCA_CAL_IN_FLASH)
U_BOOT_CMD(
	checkcal,     1,     1,      do_checkcal,
	"checkcal - verify the calibration data\n",
	"- verify the calibration data\n"
);
#endif

#endif // CONFIG_APBOOT
