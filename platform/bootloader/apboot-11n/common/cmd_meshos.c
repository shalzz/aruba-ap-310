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

/* The image type saved in flash */
#define MSR_PRI_IMG			0
#define MSR_BAK_IMG			1
#define MSR_MESHOS_IMG		2
#define MSR_AOS_IMG			3

#define MSR_ENV_MARK		1
#define MSR_ENV_RECOVER		0

#define MSR_IMAGE_MAX_SIZE		(12 * 1024 *1024)
#define APBOOT_BACKUP_IMAGE		0x100000

extern int __second_flash_exist;
extern void read_buff2(uint32_t from, int len, uchar *buf);
extern void do_write_backup_image(unsigned int addr, unsigned int len);
extern void hw_watchdog_off(void);
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

int apboot_os_partition = -1;

/* Used by compress the backup MeshOS image */
extern int gunzip(void *, int, unsigned char *, unsigned long *);

int msr_image_type(void *load_addr);
int msr_boot_image(void *addr,int type);

/*
* Description:
*   Print MeshOS image header info for debug.
*
* Parameters:
*   NULL
*
* Return:
*   void
*/
void
msr_img_hdr_print(aruba_msr_image_t *ami)
{
    printf("----------------------MSR Image Header----------------\n");
    printf("file_style: %s\n", ami->file_style);
    printf("primary: %d\n", ami->primary);
    printf("app_id: %d\n", ami->app_id);
    printf("image_version: %s\n", ami->image_version);
    printf("kernel_version: %s\n", ami->kernel_version);
    printf("rootfs_version: %s\n", ami->rootfs_version);
    printf("kernel_offset: 0x%x\n", ami->kernel_offset);
    printf("kernel_len: 0x%x\n", ami->kernel_len);
    printf("rootfs_offset: 0x%x\n", ami->rootfs_offset);
    printf("rootfs_len: 0x%x\n", ami->rootfs_len);
    printf("checksum: 0x%x\n", ami->checksum);
    printf("branch: %s\n", ami->branch);
    printf("creator: %s\n", ami->creator);
    printf("timestamp: %s\n", ami->timestamp);
    printf("type: %s\n", ami->type);
    printf("kernel_crc: 0x%x\n", ami->kernel_crc);
    printf("rootfs_crc: 0x%x\n", ami->rootfs_crc);
    printf("app_name: %s\n", ami->app_name);
    printf("app_version: %s\n", ami->app_version);
    printf("app_offset: 0x%x\n", ami->app_offset);
    printf("app_len: 0x%x\n", ami->app_len);
    printf("app_crc: 0x%x\n", ami->app_crc);
    printf("entry_point: 0x%x\n", ami->entry_point);
    printf("------------------------------------------------------\n");
}

/* 
* Description: 
*   Export some MSR image header variable to APBoot env variable,
*   some apps in MeshOS will use it.
*
* Parameters:
*   ami: MeshOS image header pointer
*   image_type:
*     MSR_MESHOS_IMG, MSR_AOS_IMG
*   active:
*     MSR_PRI_IMG, MSR_BAK_IMG  
*   save: 
*     1: Save; 0: not save
*
* Return:
*   void
*/
void
msr_export_env(aruba_msr_image_t *ami, int image_type, int active, int save)
{
    if (image_type == MSR_MESHOS_IMG && active == MSR_PRI_IMG) {
        setenv("pri_img_name", "MeshOS");
        setenv("pri_img_ver", ami->image_version);
        setenv("pri_img_type", ami->type);
        setenv("pri_img_timestamp", ami->timestamp);
    } else if (image_type == MSR_MESHOS_IMG && active == MSR_BAK_IMG) {
        setenv("bak_img_name", "MeshOS");
        setenv("bak_img_ver", ami->image_version);
        setenv("bak_img_type", ami->type);
        setenv("bak_img_timestamp", ami->timestamp);
    } else if (image_type == MSR_AOS_IMG && active == MSR_PRI_IMG) {
        setenv("pri_img_name", "AOS"); 
        setenv("pri_img_ver", NULL);   
        setenv("pri_img_type", NULL);  
        setenv("pri_img_timestamp", NULL);
    } else if (image_type == MSR_AOS_IMG && active == MSR_BAK_IMG) {
        setenv("bak_img_name", "AOS");
        setenv("bak_img_ver", NULL);
        setenv("bak_img_type", NULL);
        setenv("bak_img_timestamp", NULL);
    } else {
        printf("%s: unknown image type: %d %d.\n",
            __func__, image_type, active);
    }

    if (save) {
        saveenv();
    }
}

/*
* Description:
*  Recover APBoot ENV, avoid saveenv() save unnecessary variables
*
* Parameters:
*  flag:
*    MSR_ENV_MARK: Mark env
*    MSR_ENV_RECOVER: Recover env to marked env.
*
* Return:
*    void 
*/
void
msr_env_recover(unsigned int flag)
{
	static int ipaddr_is_exist = 0;
	static int netmask_is_exist = 0;
	static int dnsip_is_exist = 0;
	static int gatewayip_is_exist = 0;
	static int serverip_is_exist = 0;
	static int filesize_is_exist = 0;
	static int fileaddr_is_exist = 0;
	static int autostart_is_yes = 0;	

	if (flag == MSR_ENV_MARK) {
		if (getenv("ipaddr")) {
			ipaddr_is_exist = 1;
		}
		if (getenv("netmask")) {
			netmask_is_exist = 1;
		}
		if (getenv("dnsip")) {
			dnsip_is_exist = 1;
		}
		if (getenv("gatewayip")) {
			gatewayip_is_exist = 1;
		}
		if (getenv("serverip")) {
			serverip_is_exist = 1;
		}
		if (getenv("filesize")) {
			filesize_is_exist = 1;
		}
		if (getenv("fileaddr")) {
			fileaddr_is_exist = 1;
		}
		if (getenv("autostart")) {
			if (!strcmp(getenv("autostart"), "yes")) {
				autostart_is_yes = 1;
			}else {
				autostart_is_yes = 0;
			}
		}else {
			autostart_is_yes = 0;
		}
	} else if (flag == MSR_ENV_RECOVER) {
		if (!ipaddr_is_exist && getenv("ipaddr")) {
			setenv("ipaddr", NULL);
		}
		if (!netmask_is_exist && getenv("netmask")) {
			setenv("netmask", NULL);
		}
		if (!dnsip_is_exist && getenv("dnsip")) {
			setenv("dnsip", NULL);
		}
		if (!gatewayip_is_exist && getenv("gatewayip")) {
			setenv("gatewayip", NULL);
		}
		if (!serverip_is_exist && getenv("serverip")) {
			setenv("serverip", NULL);
		}
		if (!filesize_is_exist && getenv("filesize")) {
			setenv("filesize", NULL);
		}
		if (!fileaddr_is_exist && getenv("fileaddr")) {
			setenv("fileaddr", NULL);
		}
		if (autostart_is_yes) {
			setenv("autostart", "yes");
		} else {
			setenv("autostart", "no");
		}
	} else {
		printf("%s: flag = %d, unsupport.\n", __func__, flag);
	}
}

/*
* Description:
*   APBoot cannot suport MeshOS image lower than 4.4, this function check the
*   MeshOS version.
*
* Parameters:
*   ver: MeshOS version string pointer
*
* Return:
*   0: Supported versoin
*  -1: Not support
*/
int
msr_ver_check(char *ver)
{
    int pri, sec, cnt;
    char pri_str[32] = {0}, sec_str[32] = {0};
    char *p, *p1;

    if (!ver) {
        return -1;
    }

    /* Parse version string to integer */
    p = ver;
    p1 = pri_str;
    cnt = 0;
    while (*p != '.' && cnt++ < 32) {
        *p1++ = *p++;
    }

    p++;
    p1 = sec_str;
    cnt = 0;
    while(*p != '.' && cnt++ < 32) {
        *p1++ = *p++;
    }

    /* 
      MeshOS image version format is A.B.C.D, just parse A and B,
      ignore C and D.
    */
    pri = simple_strtoul(pri_str, 0, 10);
    sec = simple_strtoul(sec_str, 0, 10);

    /* Only support MeshOS4.4 or higher */
    if ((pri = 4 && sec >= 4) || (pri > 4)) {
        return 0;
    } else {
        return -1;
    }
}

/*
* Description:
*   Initialize parameters for boot up MeshOS image.
*
* Parameters:
*   pb: Pointer to parameters structure, it locates in memory 0x80500000
*   type: MSR_PRI_IMG or MSR_BAK_IMG.
*
* Return:
*   void
*/
void
msr_parm_block_init(struct parmblock *pb, int type)
{
    char *pcmd, *p;
	extern char version_string[];
	char ver_buf[128] = {0};

    pcmd = pb->text;

    pb->firmware_version.name = pcmd;
    pcmd += sprintf(pcmd, "Firmware");
    pb->firmware_version.val = ++pcmd;
    pcmd += sprintf(pcmd, "1.0.0");

    pb->boot_image_index.name = ++pcmd;
    pcmd += sprintf(pcmd, "Image");
    pb->boot_image_index.val = ++pcmd;
    pcmd += sprintf(pcmd, "1");

    pb->memsize.name = ++pcmd;
    pcmd += sprintf(pcmd, "memsize");
    pb->memsize.val = ++pcmd;
    pcmd += sprintf(pcmd, "0x08000000");

    pb->modetty0.name = ++pcmd;
    pcmd += sprintf(pcmd, "modetty0");
    pb->modetty0.val = ++pcmd;
    pcmd += sprintf(pcmd, "0,n,8,1,hw");
    pb->ethaddr.name = ++pcmd;
    pcmd += sprintf(pcmd, "ethaddr");
    pb->ethaddr.val = ++pcmd;
    p = getenv("ethaddr");
    if (p) {
        pcmd += sprintf(pcmd, p);
    } else {
        printf("Get ethaddr failed, use default.\n");
        pcmd += sprintf(pcmd, "00.1a.1e.00.00.01");
    }

    pb->env_end.name = NULL;
    pb->env_end.val = NULL;

    pb->argv[0] = pcmd;
    pb->argv[1] = ++pcmd;

	snprintf(ver_buf, sizeof(ver_buf), "%s", version_string);

/* For MST200AC with Marvell PHY */
#if (CONFIG_MSR_SUBTYPE == 201)
    sprintf(pcmd, "console=ttyS0,9600 init=/sbin/init mem=128M bootloader=1 secondflash=%d image=%d boot_ver=\"%s\" phy_type=1", 
		__second_flash_exist, type, ver_buf);
#else
    sprintf(pcmd, "console=ttyS0,9600 init=/sbin/init mem=128M bootloader=1 secondflash=%d image=%d boot_ver=\"%s\"", 
		__second_flash_exist, type, ver_buf);
#endif

}

/*
* Description:
*  Check whether the image format is MeshOS image or not.
*
* Parameters:
*  load_addr: MeshOS image address in memory
*
* Return:
*  1: MeshOS
*  0: Others
*/
int
msr_image_type(void *load_addr)
{
    aruba_msr_image_t *ami = (aruba_msr_image_t *)load_addr;

    /* This field always filled "Mission image" in MeshOS */
    if (!strcmp(ami->file_style, "Mission image")) {
        return 1;
    } else {
        return 0;
    }
}

/*
* Description:
*   Calculate checksum of MeshOS image
*
* Parameters:
*   addr: MeshOS image addr in memory
*
* Return:
*  0: Passed
*  1: Failed
*/
int
msr_image_cal_cksum(void *addr)
{
    aruba_msr_image_t *ami;
    unsigned int checksum, i;
    unsigned int image_checksum = 0;
    unsigned char *p;

    ami = (aruba_msr_image_t *)addr;
    checksum = ami->checksum;
    p = (unsigned char*)(addr) + ami->kernel_offset;

    for (i = 0; i < ami->kernel_len; i++) {
        image_checksum += (unsigned int)*p;
        p++;
    }

    p = (unsigned char*)(addr) + ami->rootfs_offset;
    for (i = 0; i < ami->rootfs_len; i++) {
        image_checksum += (unsigned int)*p;
        p++;
    }

    /* Checksum is right */
    if (image_checksum == checksum) {
        return 0;
    } else {
        return 1;
    }
}

/*
* Description:
*   Check image type and do image file verify for MeshOS and AOS.
*
* Parameters:
*   addr: Image file addr in memory
*   image_type: Image type, MSR_MESHOS_IMG_TYPE or MSR_AOS_IMG_TYPE
*   len: Store image length
*   copy_hdr: Inherit from CLI, used by aruba_basic_image_verify
*
* Return:
*   0: Successed
*   1: Failed
*/
int
msr_img_check_and_verify(unsigned int addr, int *image_type, 
    unsigned int *len, int copy_hdr)
{
    int r = 0, type = 0;
   
    /* MeshOS image type check */
    if (msr_image_type((void *)addr)) {
        type = MSR_MESHOS_IMG;
    /* AOS image type check */
    } else if (!aruba_basic_image_verify((void *)addr,
        __OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, len, copy_hdr)) {
        type = MSR_AOS_IMG;       
    } else {
        printf("#Error:Unknown image type.\n");
        r = 1;
        goto out;             
    }

    if (type == MSR_MESHOS_IMG) {
        printf("verifying checksum... ");
        if (!msr_image_cal_cksum((void*)addr)) {
            printf("passed\n");
        } else {
            printf("failed\n");
            r = 1;
            goto out;
        }
    } else if (type == MSR_AOS_IMG) {
#if defined(__SIGNED_IMAGES__)
        if (image_verify((aruba_image_t *)addr)) {
            printf("Image verify failed.\n");
            r = 1;
            goto out;
        }
#endif
    }

    r = 0;
out:
    *image_type = type;

    return r;
}

/*
* Description:
*   Export APBoot primary info to APBoot ENV variable when do prugeenv 
*
* Parameters
*   void 
*
* Return:
*   void
*/
void
msr_export_default_env(void)
{
    aruba_msr_image_t *ami = NULL;
    unsigned int boot_first = AP_PRODUCTION_IMAGE;
#ifdef AP_PROVISIONING_IMAGE
    unsigned int boot_second = AP_PROVISIONING_IMAGE;
#endif
    unsigned int addr = boot_first;

    if (msr_image_type((void*)addr)) {
        if (!msr_image_cal_cksum((void*)addr)) {
            ami = (aruba_msr_image_t*)addr;
            msr_export_env(ami, MSR_MESHOS_IMG, MSR_PRI_IMG, 0);
            return;
        }
    }
 
    if (aruba_basic_image_verify((void *)addr,
        __OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0)) {
#ifdef AP_PROVISIONING_IMAGE
        addr = boot_second;
        if (aruba_basic_image_verify((void *)addr,
           __OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0)) {
           return;
        }
#else
        return;
#endif /* AP_PROVISIONING_IMAGE */
    }
   

#if defined(__SIGNED_IMAGES__)
    if (image_verify((aruba_image_t *)addr)) {
        return;        
    }  
#endif 

    /* The first flash have AOS, then set swap_to_aos=1 */
    setenv("swap_to_aos", "1");
    msr_export_env(NULL, MSR_AOS_IMG, MSR_PRI_IMG, 0);

    return; 
}

/*
* Description:
*   Export APBoot backup image info to APBoot ENV variable when do prugeenv 
*
* Parameters
*   void 
*
* Return:
*   void
*/
void
msr_export_default_bakimg_env(void)
{
	unsigned int mem_addr = 0x84000000;
	unsigned int base = 0x100000;
	aruba_msr_image_t *ami = NULL;
	aruba_image_t *ai = NULL;

	if (__second_flash_exist) {
		printf("\nParse backup image...\n");
	} else {
		return;
	}

	read_buff2(base, MSR_IMAGE_MAX_SIZE, (void*)mem_addr);

	if (msr_image_type((void*)mem_addr)) {
		ami = (aruba_msr_image_t *)mem_addr;
		if (!msr_image_cal_cksum((void*)mem_addr)) {
			msr_export_env(ami, MSR_MESHOS_IMG, MSR_BAK_IMG, 0);
		} 
		return;
	}

	ai = (aruba_image_t*)mem_addr;
	if (aruba_basic_image_verify((void *)mem_addr,
		__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0)) {
		return;
	}
#if defined(__SIGNED_IMAGES__)
	if (image_verify(ai)) {
		return;
	}
#endif
	msr_export_env(NULL, MSR_AOS_IMG, MSR_BAK_IMG, 0);

	return;
}

/*
* Description:
*   Boot up MeshOS image routine.
*
* Parameters:
*   addr: MeshOS image addr in memory
*   type: Image type, PRI_IMG, BAK_IMG OR MEM_IMG
* Return:
*   0: Successed
*  -1: Failed
*/
int
msr_boot_image(void *addr, int type)
{
    unsigned int parm_addr = 0x80050000;
    unsigned int uncompress_addr = 0x80060000;
    unsigned int entry_point, flash_size = 16; /* 16MB */
    unsigned long image_len;
    unsigned int dst_len = 0x1000000; /* 16MB */
    struct parmblock *pb;
    aruba_msr_image_t *ami = (aruba_msr_image_t *)addr;
    void (*linux_entry)(int, char **, void *, int);

    printf("verifying checksum... ");
    if (!msr_image_cal_cksum(addr)) {
        printf("passed\n");
    } else {
        printf("failed\n");
        return -1;
    }
    
	pb = (struct parmblock *)parm_addr;
    msr_parm_block_init(pb, type);

    image_len = ami->kernel_len;
    printf("uncompressing... ");
    WATCHDOG_RESET();
    gunzip((void *)uncompress_addr, dst_len,
        ((unsigned char *)addr + ami->kernel_offset), &image_len);
	printf("done\n");

    entry_point = ami->entry_point;
    linux_entry = (void *)entry_point;
 
    printf(".text @ 0x%x  .entry @ 0x%x\n\n",
        uncompress_addr, ami->entry_point);
    /*
     * APBoot use AR7100 chipset internal watchdog, and MeshOS use RTC watchdog
     * there has not routine to feedon internel watchdog in MeshOS, so disable
     * it before load MeshOS.
     */
    hw_watchdog_off();
    linux_entry(2, pb->argv, pb, flash_size);

    return 0;
}

/*
* Description:
*   Boot up backup image from the second flash. It supports both MeshOS and
*   AOS image file.
*
* Parameters:
*   cmdtp: Inherit from CLI, used by do_boot_linux
*   flag: Inherit from CLI, used by do_boot_linux
*
* Return:
*   0: Successed
*   1: Failed
*/
int    
msr_boot_bak_image(cmd_tbl_t *cmdtp, int flag)   
{
    unsigned int parm_addr = 0x80050000;
    unsigned int uncompress_addr = 0x80060000;
    unsigned int addr = 0x84000000;
    unsigned int from = APBOOT_BACKUP_IMAGE;  
    unsigned int load_len = MSR_IMAGE_MAX_SIZE;
    unsigned long image_len;  
    unsigned int entry_point, flash_size = 16; /* 16MB */
    unsigned int dst_len = 0x1000000; /* 16MB */
    int image_type = 0, r = 0;
    struct parmblock *pb;     
    aruba_msr_image_t *ami;   
    void (*linux_entry)(int, char **, void *, int);

    WATCHDOG_RESET();
    printf("Load backup image... ");
    read_buff2(from, load_len, (uchar*)addr); 
    printf("done\n");

    if ((r = msr_img_check_and_verify(addr, &image_type, 0, 0)) != 0) {
        goto out;
    }

    /* Image is MeshOS */
    if (image_type == MSR_MESHOS_IMG) {
        pb = (struct parmblock *)parm_addr;
        msr_parm_block_init(pb, MSR_BAK_IMG);  

        ami = (aruba_msr_image_t *)addr;
        image_len = ami->kernel_len;

        WATCHDOG_RESET();
        printf("uncompressing... ");
        gunzip((void *)uncompress_addr, dst_len,
            ((unsigned char *)addr + ami->kernel_offset), &image_len);
		printf("done\n");

        entry_point = ami->entry_point;
        linux_entry = (void *)entry_point;
        printf(".text @ 0x%x  .entry @ 0x%x\n\n",
            uncompress_addr, ami->entry_point);
     
        /* 
         * APBoot use AR7100 chipset internal watchdog, and MeshOS use RTC 
         * watchdog there has not routine to feedon internel watchdog in
         * MeshOS, so disable it before load MeshOS.
         */
        hw_watchdog_off();

        linux_entry(2, pb->argv, pb, flash_size);                                 
    /* Image is AOS */
    } else if (image_type == MSR_AOS_IMG) { 
        WATCHDOG_RESET();

        (void) boot_aruba_image(addr + ARUBA_HEADER_SIZE);
    }

    r = 1;
out:
    printf("#Error: Boot backup image failed.\n");
    return r;
}

/*
* Description:
*   Upgrade backup image in the second flash. It supports both MeshOS and AOS
*   image. Since the second flash is not in processor's address space, when 
*   verify image, it will copy to RAM from the second flash.
*
* Parameters:
*   cmdtp: Inherit from CLI, used by do_mem_cmp
*   flag: Inherit from CLI, used by do_mem_cmp
*   ram_addr: The source image address in memory
*   store_len: The image length
*
* Return:
*   0: Successed
*  -1: Failed
*/
int
msr_upgrade_bak_image(cmd_tbl_t *cmdtp, int flag, unsigned int ram_addr, int store_len)
{
    char sbuf[32], dbuf[32], cbuf[32];
    char *av[6];
    unsigned int flash_addr = APBOOT_BACKUP_IMAGE;
    unsigned int cmp_addr = 0x85000000;

    if (store_len > MSR_IMAGE_MAX_SIZE) {
        printf("Image size: (0x%x) > max size: (0x%x), upgrade failed.\n",
            store_len, MSR_IMAGE_MAX_SIZE);
        return -1;
    }

    do_write_backup_image(ram_addr, store_len);
    /* Load image from flash to RAM for verify */
    printf("load image to memory... ");

    read_buff2(flash_addr, store_len, (uchar*)cmp_addr);
    printf("done\n");

    snprintf(sbuf, sizeof(sbuf), "0x%x", ram_addr);
    snprintf(dbuf, sizeof(dbuf), "0x%x", cmp_addr);
    snprintf(cbuf, sizeof(cbuf), "0x%x", store_len);
    av[0] = "cmp.b";
    av[1] = sbuf;
    av[2] = dbuf;
    av[3] = cbuf;
    av[4] = 0;

    WATCHDOG_RESET();

    printf("compare image... ");
    if (do_mem_cmp(cmdtp, flag, 4, av)) {
        printf("failed\n");
        return -1;
    }

    WATCHDOG_RESET();

    return 0;
}

/*
* Description:
*   Upgrade MeshOS image in the first flash chipset. It will burn at 0xbf100000
*   
* Parameters:
*   cmdtp: Inherit from CLI, used by aruba_copy_and_verify.
*   flag: Inherit from CLI, used by aruba_copy_and_verify.
*   ram_addr: The source image address in memory.
*   flash_addr: The destination address in flash, 0xbf100000
*   copy_hdr: Inherit from __do_upgrade_os, used by aruba_copy_and_verify
*   protected: Inherit from __do_upgrade_os, used by aruba_copy_and_verify
*
* Return:
*   0: Successed
*  -1: Failed
*/
int
msr_upgrade_img(cmd_tbl_t *cmdtp, int flag, void *ram_addr,
 unsigned int flash_addr, int copy_hdr, int protected)
{
    aruba_msr_image_t *ami;
    int i, store_len;
    flash_info_t *fl;
	unsigned end;

    ami = (aruba_msr_image_t *)ram_addr;

    printf("verifying checksum... ");
    if (!msr_image_cal_cksum(ram_addr)) {
        printf("passed\n");
    } else {
        printf("failed\n");
        return -1;
    }

    if (msr_ver_check(ami->image_version)) {
        printf("Not supported MeshOS version: %s\n",
            ami->image_version);
        return -1;
    }

    store_len = ami->kernel_len + ami->kernel_offset;
    while (store_len & 3) {
        store_len++;
    }

    if (store_len > MSR_IMAGE_MAX_SIZE) {
        printf("Image size: (0x%x) > max size: (0x%x), upgrade failed.\n",
            store_len, MSR_IMAGE_MAX_SIZE);
        return -1;
    }

    i = 0;
    fl = &flash_info[0];
    while (fl->start[i] < flash_addr) {
        i++;
    }
    while (fl->start[i] < (flash_addr + store_len) && (i < CFG_MAX_FLASH_SECT)) {
        i++;
    }

	/* handle the case where we are up against the end of the flash */
	if (i == CFG_MAX_FLASH_SECT) {
		end = CFG_FLASH_BASE + CFG_FLASH_SIZE - 1;
	} else {
		end = fl->start[i] - 1;
	}
	
	msr_env_recover(MSR_ENV_RECOVER);
    msr_export_env(ami, MSR_MESHOS_IMG, MSR_PRI_IMG, 1);

    WATCHDOG_RESET();

    return aruba_copy_and_verify(cmdtp, flag, (unsigned int)ram_addr, flash_addr, end, store_len,
        copy_hdr, protected);
}

static void
msr_meshos_info_print(aruba_msr_image_t *ami)
{
	if (!ami) {
		return;
	}

	printf("\n");
	printf("%15s: %s\n", "image name", "MeshOS");
	printf("%15s: %s\n", "image type", ami->file_style);
	printf("%15s: %u\n", "size", ami->kernel_len);
	printf("%15s: %s\n", "version", ami->image_version);
	printf("%15s: %s %s\n\n", "build string", ami->creator, ami->timestamp);

	WATCHDOG_RESET();
}

static void
msr_aos_info_print(aruba_image_t *ai)
{
	if (!ai) {
		return;
	}

	printf(":\n");
	printf("    image name: ArubaOS\n");
	printf("    image type: %u\n", ai->type);
	printf("  machine type: %u\n", ai->machineType);
	printf("          size: %u\n", ai->size + ARUBA_HEADER_SIZE);
	printf("       version: %s\n", ai->arubaVersion);
	printf("  build string: %s\n", ai->buildString);
	printf("         flags: %s\n", aruba_osinfo_flags(ai->flags));
	
	WATCHDOG_RESET();
}

static int
__do_osinfo_common(long base, unsigned size, unsigned partition)
{
	int valid;
	aruba_image_t *ai;
	aruba_msr_image_t *ami;
#ifdef AP_SLOW_FLASH_STAGING_AREA
	base = (long)aruba_copy_to_staging_area((void *)base, 
			(void *)AP_SLOW_FLASH_STAGING_AREA, size, 1);
#endif
    WATCHDOG_RESET();

	printf("Partition %u", partition);

	/* The backup image start addr in the second flash */
	if (base == APBOOT_BACKUP_IMAGE) {
		unsigned int mem_addr = 0x84000000;
		unsigned int load_hdr_len = 64 * 1024; /* Load one sector first */

		read_buff2(base, load_hdr_len, (uchar*)mem_addr);
		if (msr_image_type((void*)mem_addr)) {
			ami = (aruba_msr_image_t *)mem_addr;
			msr_meshos_info_print(ami);
			printf("verify Checksum ... ");
			read_buff2(base, MSR_IMAGE_MAX_SIZE, (void*)mem_addr);

            if (!msr_image_cal_cksum((void*)mem_addr)) { 
                printf("passed\n");            
            } else {          
                printf("failed\n");            
            }
			return 0;
		} else {
			ai = (aruba_image_t*)mem_addr;

			if (!strcmp(ARUBA_IMAGE_MAGIC, ai->magic)) {
				msr_aos_info_print(ai);
				read_buff2(base, MSR_IMAGE_MAX_SIZE, (void*)mem_addr);

				valid = aruba_basic_image_verify((void *)mem_addr,
					__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0);
				if (!valid) {
#if defined(__SIGNED_IMAGES__)
            		if (image_verify(ai)) {            
                		;             
            		}
        		}
#endif 
			} else {
				printf(" does not contain a valid OS image\n", partition);
			} 
		}
		
		return 0;	
	}

	/* MeshOS image check, only check the primary partition */
	if (base == AP_PRODUCTION_IMAGE) {
		if (msr_image_type((void*)base)) { 
			ami = (aruba_msr_image_t *)base;
			msr_meshos_info_print(ami);

			printf("verify Checksum ... ");
			if (!msr_image_cal_cksum((void*)base)) { 
				printf("passed\n");
				return 1;
			} else {
				printf("failed\n");
				return 0;
			}
		}
	}

	/* AOS image check */
	ai = (aruba_image_t *)base;

	if (!strcmp(ARUBA_IMAGE_MAGIC, ai->magic)) {
		msr_aos_info_print(ai);

		valid = aruba_basic_image_verify((void *)base,
			__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0);
		if (!valid) {
#if defined(__SIGNED_IMAGES__)
	        if (image_verify(ai)) {
				;
	        }
		}
#endif
	} else {
		printf(" does not contain a valid OS image\n", partition);
	}

	return 0;
}

static int
do_osinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (__second_flash_exist) {
		printf("In the first flash:\n\n");
	}

	/* If MeshOS in the first flash, only check primary partition, only successed MeshOS return 1 */
	if (__do_osinfo_common(AP_PRODUCTION_IMAGE, AP_PRODUCTION_IMAGE_SIZE, 0)) {
		printf("\n");
		goto check_backup_img;
	}

#ifdef AP_PROVISIONING_IMAGE
    printf("\n");
	__do_osinfo_common(AP_PROVISIONING_IMAGE, AP_PROVISIONING_IMAGE_SIZE, 1);
#endif

check_backup_img:

	if (__second_flash_exist) {
		printf("\nIn the second flash:\n\n");
		/* Backup image dont care image size */
		__do_osinfo_common(APBOOT_BACKUP_IMAGE, 0, 0);
	}

	return 0;
}

#ifndef CONFIG_STAGE1
int 
do_boot_ap(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int r;
	char *av[32];
	char *p = getenv("bootcmd");
    char *p1 = getenv("force_backup");
	char *p2 = getenv("swap_to_aos");
	long base = AP_PRODUCTION_IMAGE;
	long boot_first = base;
#ifdef AP_PROVISIONING_IMAGE
	long boot_second = AP_PROVISIONING_IMAGE;
#endif
	int force_backup = 0, swap_to_aos = 0;

    /* Force start from backup image */
    if (p1) {
        if (!strncmp(p1, "1", 1)) {
            printf("Force boot backup image\n");
			force_backup = 1;
            setenv("force_backup", NULL);
            saveenv();
            goto boot_bak;
        }
    }

	if (p2) {
		if (!strncmp(p2, "1", 1)) {
			swap_to_aos = 1;
		}
	}

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

	if (argc == 2 && strcmp(argv[1], "ap")) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

flashboot:
	WATCHDOG_RESET();

	msr_env_recover(MSR_ENV_MARK);
	boot_first = AP_PRODUCTION_IMAGE;
	apboot_os_partition = 0;
#ifdef AP_PROVISIONING_IMAGE
	boot_second = AP_PROVISIONING_IMAGE;
	p = getenv("os_partition");
	if (p) {
		if (!strcmp(p, "1")) {
			printf("Booting OS partition 1\n");
			apboot_os_partition = 1;
			boot_first = AP_PROVISIONING_IMAGE;
			boot_second = AP_PRODUCTION_IMAGE;
		}  else  {
			printf("Booting OS partition 0\n");
		}
	}
#endif
	printf("Checking image @ 0x%x\n", boot_first);
	base = boot_first;

    /* Check whether the image is MeshOS or not, if it's MeshOS image, boot up */
    if (msr_image_type((void *)base)) {
        printf("Start to boot MeshOS image\n");
        if (msr_boot_image((void *)base, MSR_PRI_IMG)) {
            printf("Boot MeshOS failed.\n");
        }
		/* Continue to run normal flow */
    } 

#ifdef AP_SLOW_FLASH_STAGING_AREA
		base = (long)aruba_copy_to_staging_area((void *)base, 
			(void *)AP_SLOW_FLASH_STAGING_AREA, AP_PRODUCTION_IMAGE_SIZE, 0);
#endif
	if (aruba_basic_image_verify((void *)base,
		__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0)) {
#ifdef AP_PROVISIONING_IMAGE
		apboot_os_partition = 1 - apboot_os_partition;  /* swap (0/1) */
		printf("Checking image @ 0x%x\n", boot_second);
		base = boot_second;
#ifdef AP_SLOW_FLASH_STAGING_AREA
		base = (long)aruba_copy_to_staging_area((void *)base, 
			(void *)AP_SLOW_FLASH_STAGING_AREA, AP_PROVISIONING_IMAGE_SIZE, 0);
#endif
		if (aruba_basic_image_verify((void *)base,
			__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0)) {
			goto boot_bak;
		}
#else
		goto boot_bak;
#endif
	}
#if defined(__SIGNED_IMAGES__)
	if (image_verify((aruba_image_t *)base)) {
		goto boot_bak;
	}
#endif

	if (!swap_to_aos) {
		msr_env_recover(MSR_ENV_RECOVER);
		setenv("swap_to_aos", "1");
		saveenv();
	}

	r = boot_aruba_image(base + ARUBA_HEADER_SIZE);
	if (r) {

boot_bak:
		/* When AMI completed, it will never boot image from the 2nd flash */
		if (!force_backup && swap_to_aos) {
			goto netboot;
		}
        /* Not have the second flash */
        if (!__second_flash_exist) {
            goto netboot;
        }
        printf("Start to load backup image.\n");
        msr_boot_bak_image(cmdtp, flag);
        printf("Boot backup image failed.\n");
netboot:
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
    char *p1 = getenv("force_big_image");
    char *p2 = getenv("bootcmd");
	int old_auto = 0;
	unsigned ramaddr;
	extern char BootFile[];
	extern IPaddr_t NetServerIP;
	char *sp = 0;
	char abuf[64], rbuf[32];
	extern IPaddr_t string_to_ip(char *);
	flash_info_t *fl;
 	unsigned store_len;
    int image_type = 0, big_image = 0;
    int aos_max_size = MSR_IMAGE_MAX_SIZE;
    int mesh_prov = 0;
	unsigned end;

#ifndef CFG_TFTP_STAGING_AREA
    DECLARE_GLOBAL_DATA_PTR;
	bd_t *bd = gd->bd;

	ramaddr = bd->bi_uboot_ram_addr + bd->bi_uboot_ram_used_size;
	ramaddr += (1024 * 1024);
#else
	ramaddr = CFG_TFTP_STAGING_AREA;
#endif

    /* Do MeshOS provisoining and reserved API for upgrade backup image */
    if (p2 && !strncmp(p2, "upgrade", 7)) {
        printf("Begin to do image provisioning\n");
        setenv("bootcmd", "boot ap");
		saveenv();
        mesh_prov = 1;
    }

	msr_env_recover(MSR_ENV_MARK);

	if (p && !strcmp(p, "yes")) {
		old_auto = 1;
		setenv("autostart", "no");
	}

    /* 
    * When AMI completed, if the AOS size is larger than 7M, set this
    * ENV variable, the AOS size will extend to 12M.
    */
    if (p1 && !strcmp(p1, "1")) {
        big_image = 1;
    }

	sprintf(rbuf, "0x%x", ramaddr);

	av[0] = "tftp";
	av[1] = rbuf;
	av[2] = argv[2];
	av[3] = 0;

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

    /* 
     * Upgrade backup image in the second flash.
     * The first flash address space range is 0xbf000000--0xbfffffff
     * The second falsh address space range is 0x00000000--0x00ffffff 
     */
    if (addr < CFG_FLASH_BASE) {
        aruba_msr_image_t *ami;
       
        if (!__second_flash_exist) {
            printf("Not have the second flash, upgrade failed.\n");
            r = 1;
            goto out;
        } 

        if ((r = msr_img_check_and_verify(ramaddr, &image_type, &store_len, 
            copy_hdr)) != 0) {
            goto out;
        }

        /* Upgrade MeshOS */
        if (image_type == MSR_MESHOS_IMG) {
            int msr_max_size = MSR_IMAGE_MAX_SIZE;

            WATCHDOG_RESET();

            ami = (aruba_msr_image_t*)ramaddr;
            store_len = ami->kernel_len + ami->kernel_offset;
            while (store_len & 3) {
                store_len++;
            }
            
            if (store_len > msr_max_size) {
                printf("MeshOS image is too big; (%u > %u)\n", 
                    store_len, msr_max_size);
                r = 1;
                goto out;
            }
            if (msr_ver_check(ami->image_version)) {
                printf("Not supported MeshOS version: %s\n",
                    ami->image_version);
                r = 1;
                goto out;
            }
        /* Upgrade AOS */
        } else if (image_type == MSR_AOS_IMG) {
            if ((big_image && store_len > aos_max_size) ||
                (!big_image && store_len > max_size)) {
                printf("AOS image is too big; (%u > %u)",
                    store_len, max_size);
                r = 1;
                goto out;
            }
        }

        r = msr_upgrade_bak_image(cmdtp, flag, ramaddr, store_len);

        if (!r) {
            if (image_type == MSR_MESHOS_IMG) {
				msr_env_recover(MSR_ENV_RECOVER);
                msr_export_env(ami, MSR_MESHOS_IMG, MSR_BAK_IMG, 1);
            } else if (image_type == MSR_AOS_IMG) {
				msr_env_recover(MSR_ENV_RECOVER);
                msr_export_env(NULL, MSR_AOS_IMG, MSR_BAK_IMG, 1);
            }
        }

        goto out;
    }

    /* Upgrade MeshOS image file to the first flash */
    if (msr_image_type((void *)ramaddr)) {
		if (addr != AP_PRODUCTION_IMAGE) {
			printf("#Error: MeshOS can only be write in the primary partiton.\n");
			r = 1;
			goto out;
		}

        printf("Upgrade MeshOS image\n");
        WATCHDOG_RESET();
        r = msr_upgrade_img(cmdtp, flag, (void *)ramaddr, addr, copy_hdr, protected);
        goto out;
    }

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

    /* Only upgrade primary partition allow big AOS image */
    if (addr == AP_PRODUCTION_IMAGE) {
        if ((!big_image && store_len > max_size) ||
            (big_image && store_len > aos_max_size)) {
            printf("Image is too big; (%u > %u)\n", store_len, max_size);
            r = 1;
            goto out;
        }
	} else {
        if (store_len > max_size) {
		    printf("Image is too big; (%u > %u)\n", store_len, max_size);
		    r = 1;
		    goto out;
        }
	}

	i = 0;
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

	r = aruba_copy_and_verify(cmdtp, flag, ramaddr, addr, end, store_len, 
		copy_hdr, protected);

    if (!r && (addr == AP_PRODUCTION_IMAGE
#ifdef AP_PROVISIONING_IMAGE
		|| addr == AP_PROVISIONING_IMAGE
#endif
		)) {
		msr_env_recover(MSR_ENV_RECOVER);
        msr_export_env(NULL, MSR_AOS_IMG, MSR_PRI_IMG, 1);
    }

out:
	if (old_auto) {
		setenv("autostart", "yes");
	}
	if (sp) {
		setenv("serverip", abuf);
	}
	if (r == 0) {
		printf("Upgrade successful.\n");
        if (mesh_prov) {
            printf("\n*** Image provisioning successed! Reset system now. ***\n");
            do_reset(NULL, 0, 0, NULL);
        }
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

/* Upgrade backup image in the second flash */
static int
__do_upgrade_backup(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    return __do_upgrade_common(cmdtp, flag, argc, argv, APBOOT_SIZE, 
        1, 0, __OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, AP_PRODUCTION_IMAGE_SIZE);
}

#ifdef AP_PROVISIONING_IMAGE 
static int
__do_upgrade_prov(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return __do_upgrade_common(cmdtp, flag, argc, argv, AP_PROVISIONING_IMAGE,
		1, 0, __OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, AP_PROVISIONING_IMAGE_SIZE);
}
#endif

int
do_upgrade(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
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

	/* Upgrade the second flash image file */
    if (!strcmp(argv[1], "os") && (argc == 4)) {
        char *av[] = { argv[0], argv[1], argv[3] };  /* remove partition number */
		if (!strcmp(argv[2], "2"))
            return __do_upgrade_backup(cmdtp, flag, 3, av);
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
}

int
__do_clear_os(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#ifdef AP_PROVISIONING_IMAGE
	/* "clear all" honors DONT_CLEAR flag in image header   */
	/* "clear os" forces an erase, ignoring DONT_CLEAR flag */
	if ((argc >= 2) && (strcmp(argv[1], "os") != 0))  {
		printf("Checking OS image and flags\n");
		if (!aruba_basic_image_verify((void *)AP_PRODUCTION_IMAGE,
				__OS_IMAGE_TYPE, ARUBA_IMAGE_TYPE_ELF, 0, 0))
		{
			aruba_image_t *ai;
			ai = (aruba_image_t *)AP_PRODUCTION_IMAGE;
			if (ntohl(ai->flags) & ARUBA_IMAGE_FLAGS_DONT_CLEAR_ON_PURGE)  {
				printf("Dont_clear flag set -- skipping OS clear\n");
				return 1;
			}  else  {
				printf("Flag not set -- continuing with OS clear\n");
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
	return __do_clear_image_common(cmdtp, flag, argc, argv, AP_PROVISIONING_IMAGE);
}
#endif

int
__do_clear_cache(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *av[5];
	char sbuf[16], ebuf[16];
	long base;
	flash_info_t *fl;
	int i = 0;
	int r = 0;

    base = AP_PRODUCTION_IMAGE;
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
		if (memcmp((void *)fl->start[i], "TheCache", 8)) {
			i++;
			continue;
		}
		sprintf(sbuf, "0x%x", fl->start[i]);
		sprintf(ebuf, "0x%x", fl->start[i + 1] - 1);

 	    av[0] = "erase";
	    av[1] = sbuf;
	    av[2] = ebuf;
	    av[3] = 0;
	    printf("Erasing flash sector @ %s...", sbuf);
//printf("%s %s %s \n", av[0], av[1], av[2]);
	    r += do_flerase(cmdtp, flag, 3, av);
		printf("\n");
		i++;
	}
	return r;
}

int
do_clear(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
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
						&unc_len, (void *)addr + sizeof(*ai), ai->image_size,
						CFG_MALLOC_LEN < (4096 * 1024), 0);
		if (i != BZ_OK) {
			printf("BUNZIP2 ERROR %d\n", i);
			return 1;
		}
	} else {
		memcpy((void *)STAGE2_RAM_ADDRESS, (void *)(addr + sizeof(*ai)), ai->image_size);
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
	"upgrade prov <file>\n   - upgrade provisioning image from <file> (aka, \"upgrade os 1 <file>\")\n"
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
	"clear prov\n  - clear provisioning image from flash (aka, \"clear os 1\")\n"
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

#endif // CONFIG_APBOOT
