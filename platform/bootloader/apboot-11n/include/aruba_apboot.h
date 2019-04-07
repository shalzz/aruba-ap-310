#ifndef __aruba_apboot_h__
#define __aruba_apboot_h__

#include "aruba_image.h"

extern int aruba_basic_image_verify(void *, int, int, unsigned *, int);
extern int image_verify(aruba_image_t *);
extern int boot_aruba_image(unsigned);
extern char *aruba_osinfo_flags(unsigned);

extern int aruba_copy_and_verify(
    cmd_tbl_t *cmdtp, int flag, ulong src, ulong dst, ulong end, unsigned len,
    int copy_hdr, int protected);

#ifdef AP_SLOW_FLASH_STAGING_AREA
void * aruba_copy_to_staging_area(void *src, void *dest, unsigned size, unsigned quiet);
#endif

#ifdef CONFIG_CMD_UBI
void *aruba_ubi_image(void *maddr, void *faddr, unsigned size, unsigned rw);
#endif

extern int do_tftpb(cmd_tbl_t *, int, int, char **);
extern int do_tftpboot(cmd_tbl_t *, int, int, char **);
extern int do_protect(cmd_tbl_t *, int, int, char **);
extern int do_flerase(cmd_tbl_t *, int, int, char **);
extern int do_mem_cp(cmd_tbl_t *, int, int, char **);
extern int do_mem_cmp(cmd_tbl_t *, int, int, char **);

#ifdef CONFIG_CMD_NAND
extern int do_nand(cmd_tbl_t *, int, int, char **);
#endif

#ifdef CONFIG_CMD_UBI
extern int do_ubi(cmd_tbl_t *, int, int, char **);
#endif
#if AP_PROVISIONING_IN_BANK2
extern int do_bank(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#endif

extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];

#ifndef AP_PRODUCTION_IMAGE
#define	AP_PRODUCTION_IMAGE	(CFG_FLASH_BASE + APBOOT_SIZE)
#endif
#ifndef APBOOT_IMAGE
#define APBOOT_IMAGE	CFG_FLASH_BASE
#endif

#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT)
extern int load_image_from_usb(void *, char *);
#endif

extern int aruba_flash_read(void *, void *, ulong);

#endif /* __aruba_apboot_h__ */
