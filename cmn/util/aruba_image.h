#ifndef _ARUBA_IMAGE_H_
#define _ARUBA_IMAGE_H_

#ifndef IDTSIM
#include <asm/types.h>
#endif

#define ARUBA_IMAGE_MAGIC             "ARUBA"

#define ARUBA_IMAGE_TYPE_ELF          0
#define ARUBA_IMAGE_TYPE_FPGA_BINARY    1
#define ARUBA_IMAGE_TYPE_CPBOOT_BINARY  2
#define ARUBA_IMAGE_TYPE_APBOOT_BINARY  3
#define ARUBA_IMAGE_TYPE_TPMINFO        4
#define ARUBA_IMAGE_TYPE_APBOOT_STAGE1_BINARY  5
#define ARUBA_IMAGE_TYPE_APBOOT_STAGE2_BINARY  6
#define ARUBA_IMAGE_TYPE_APBOOT_COMBINED_BINARY  7
#define ARUBA_IMAGE_TYPE_OS_ANCILLARY_IMAGE     8
#define ARUBA_IMAGE_TYPE_XLOADER_BINARY 9
#define ARUBA_IMAGE_TYPE_GRUB_BIN 10
#define ARUBA_IMAGE_TYPE_LSM_PACKAGE 11
#define ARUBA_IMAGE_TYPE_NONE                   255 // flag to turn off image type check

#define ARUBA_IMAGE_COMPRESSION_NONE  0
#define ARUBA_IMAGE_COMPRESSION_BZIP2 1
#define ARUBA_IMAGE_COMPRESSION_GZIP  2

#define ARUBA_FPGA_TYPE_INVALID       0
#define ARUBA_FPGA_TYPE_PINOT         1

#define ARUBA_HW_IGNORE_VER_CHECK     0x0DEFACED

#define ARUBA_FPGA_NAME_PINOT         "PINOT"
#define FPGA_MAX_NAME_LEN             32


#define ARUBA_IMAGE_DATE_YEAR         0
#define ARUBA_IMAGE_DATE_MONTH        1
#define ARUBA_IMAGE_DATE_DAY          2

#define ARUBA_IMAGE_TIME_HOUR         0
#define ARUBA_IMAGE_TIME_MINUTE       1
#define ARUBA_IMAGE_TIME_SECOND       2

#define ARUBA_IMAGE_STRUCT_VER        2

/*
 * NOTE: The AP image numbers defined below are mapped to/from image
 * filenames in aruba_image.c. This file must be changed if images are
 * added or changed.
 */

#define ARUBA_IMAGE_MSWITCH           0
#define ARUBA_IMAGE_CABERNET          1
#define ARUBA_IMAGE_SYRAH             2
#define ARUBA_IMAGE_MERLOT            3
#define ARUBA_IMAGE_MUSCAT            4
#define ARUBA_IMAGE_NEBBIOLO          5
#define ARUBA_IMAGE_MALBEC            6
#define ARUBA_IMAGE_PALOMINO          7
#define ARUBA_IMAGE_GRENACHE          8
#define ARUBA_IMAGE_MOSCATO           9
#define ARUBA_IMAGE_TALISKER         10
#define ARUBA_IMAGE_JURA_R           11
#define ARUBA_IMAGE_SCAPA            12
#define ARUBA_IMAGE_JURA_O           13
#define ARUBA_IMAGE_CORVINA          14
#define ARUBA_IMAGE_ARRAN            15
#define ARUBA_IMAGE_BLUEBLOOD        16
#define ARUBA_IMAGE_MSR2K            17
#define ARUBA_IMAGE_PORFIDIO         18
#define ARUBA_IMAGE_CAZULO           19
#define ARUBA_IMAGE_SCAPA_H          20
#define ARUBA_IMAGE_CARDHU           21
#define ARUBA_IMAGE_BOWMORE          22
#define ARUBA_IMAGE_TAMDHU           23
#define ARUBA_IMAGE_ARDBEG           24
#define ARUBA_IMAGE_ARDMORE          25
#define ARUBA_IMAGE_DALMORE	     26
#define ARUBA_IMAGE_K2		     27
#define ARUBA_IMAGE_GRAPPA	     28
#define ARUBA_IMAGE_SHUMWAY	     29
#define ARUBA_IMAGE_SPRINGBANK	     30
#define ARUBA_IMAGE_OUZO	     31
#define ARUBA_IMAGE_AMARULA	     32
#define ARUBA_IMAGE_GROZDOVA	     33
#define ARUBA_IMAGE_PALINKA	     34
#define ARUBA_IMAGE_HAZELBURN	     35
#define ARUBA_IMAGE_TOMATIN	     36
#define ARUBA_IMAGE_HAZELBURN_H	     37
#define ARUBA_IMAGE_TOMATIN_16	     38
#define ARUBA_IMAGE_SPRINGBANK_16    39
#define ARUBA_IMAGE_OCTOMORE	     40
#define ARUBA_IMAGE_BALVENIE	     41
#define ARUBA_IMAGE_OUZO_PLUS        42
#define ARUBA_IMAGE_X4	             43
#define ARUBA_IMAGE_EINAR            44
#define ARUBA_IMAGE_GLENFARCLAS      45
#define ARUBA_IMAGE_GLENFIDDICH	     46
#define ARUBA_IMAGE_EIGER            47
#define ARUBA_IMAGE_GLENMORANGIE     48
#define ARUBA_IMAGE_MILAGRO          49
#define ARUBA_IMAGE_OPUSONE          50
#define ARUBA_IMAGE_ABERLOUR         51
#define ARUBA_IMAGE_MILLSTONE        52
#define ARUBA_IMAGE_DEWARS           53 
#define ARUBA_IMAGE_BUNKER	         54
#define ARUBA_IMAGE_MASTERSON        55
#define ARUBA_IMAGE_SIERRA           56
#define ARUBA_IMAGE_KILCHOMAN	     57
#define ARUBA_IMAGE_SPEYBURN         58 
#define ARUBA_IMAGE_LAGAVULIN        59
#define ARUBA_IMAGE_LAPHROAIG        60
#define ARUBA_IMAGE_TOBA             61
#define ARUBA_IMAGE_ARRANTA          62

#if defined(CONFIG_PALOMINO)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_PALOMINO
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_PALOMINO
#endif
#if defined(CONFIG_MERLOT)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_MERLOT
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_MERLOT
#endif
#if defined(CONFIG_MOSCATO) || defined(CONFIG_OCTEON_CN3010_EVB_HS5)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_MOSCATO
// this is not a typo; moscato boots Palomino images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_PALOMINO
#endif
#if defined(CONFIG_JURA_R)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_JURA_R
// this is not a typo; Jura_R boots Talisker images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
#endif
#if defined(CONFIG_JURA_O)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_JURA_O
// this is not a typo; Jura-O boots Palomino images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_PALOMINO
#endif

#if defined(CONFIG_TALISKER) && ! defined (CONFIG_DALMORE)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_TALISKER
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
#endif

#if defined(CONFIG_SCAPA)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_SCAPA
// this is not a typo; Scapa boots Talisker images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
#endif

#if defined(CONFIG_SCAPA_H)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_SCAPA_H
// this is not a typo; Scapa-H boots Talisker images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
#endif

#if defined(CONFIG_BLUEBLOOD)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_BLUEBLOOD
// this is not a typo; BlueBlood boots Talisker images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
#endif

#if defined(CONFIG_MSR2K)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_MSR2K
// this is not a typo; MSR2K boots Talisker images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
#endif

#if defined(CONFIG_ARRAN) && ! defined (CONFIG_TAMDHU)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_ARRAN
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_ARRAN
#endif

#if defined(CONFIG_BOWMORE)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_BOWMORE
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_BOWMORE
#endif

#if defined(CONFIG_SPRINGBANK) && !defined(CONFIG_GLENFIDDICH) 
#if defined(__SPRINGBANK_FLASH__) && (__SPRINGBANK_FLASH__ == 16)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_SPRINGBANK_16
#else
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_SPRINGBANK
#endif
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_SPRINGBANK
#endif

#if defined(CONFIG_BALVENIE)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_BALVENIE
/* this is not a typo; BALVENIE boots Springbank images */
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_SPRINGBANK
#endif

#if defined(CONFIG_TAMDHU)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_TAMDHU
// this is not a typo; Tamdhu boots Arran images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_ARRAN
#define __SECONDARY_OS_IMAGE_TYPE	ARUBA_IMAGE_TAMDHU
#endif

#if defined(CONFIG_ARDBEG)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_ARDBEG
/* this is not a typo; Ardbeg boots Talisker images */
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
/* for 14MB Instant images, use the same image type as Dalmore */
#define __SECONDARY_OS_IMAGE_TYPE	ARUBA_IMAGE_DALMORE
#endif

#if defined(CONFIG_ARDMORE)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_ARDMORE
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_ARDMORE
#endif

#if defined(CONFIG_TOMATIN)
#if defined(__TOMATIN_FLASH__) && (__TOMATIN_FLASH__ == 16)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_TOMATIN_16
#else
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_TOMATIN
#endif
// this is not a typo; Tomatin boots Ardmore images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_ARDMORE
#endif

#if defined(CONFIG_EINAR)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_EINAR
// this is not a typo; Tomatin boots Ardmore images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_ARDMORE
#endif

#if defined(CONFIG_CARDHU)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_CARDHU
/* this is not a typo; Cardhu boots Talisker images */
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
#endif

#if defined(CONFIG_DALMORE)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_DALMORE
/* this is not a typo; Dalmore boots Talisker images for AOS */
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
/* for 14MB Instant images, allow the Dalmore image type if needed */
#define __SECONDARY_OS_IMAGE_TYPE	ARUBA_IMAGE_DALMORE
#endif

#if defined(CONFIG_HAZELBURN) && !defined(CONFIG_HAZELBURN_H)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_HAZELBURN
/* this is not a typo; Hazelburn boots Talisker images */
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
/* for 14MB Instant images, allow the Dalmore image type if needed */
#define __SECONDARY_OS_IMAGE_TYPE	ARUBA_IMAGE_DALMORE
#endif

#if defined(CONFIG_HAZELBURN_H)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_HAZELBURN_H
/* this is not a typo; Hazelburn boots Talisker images */
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_TALISKER
#endif

#if defined(CONFIG_K2)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_K2
// this is not a typo; K2 boots Ardmore images
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_ARDMORE
#endif

#if defined(CONFIG_OCTOMORE) && !defined(CONFIG_EIGER)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_OCTOMORE
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_OCTOMORE
#endif

#if defined(CONFIG_EIGER)
#define __BOOT_IMAGE_TYPE   ARUBA_IMAGE_EIGER
// this is not a typo; Eiger boots Octomore images
#define __OS_IMAGE_TYPE     ARUBA_IMAGE_OCTOMORE
#define __SECONDARY_OS_IMAGE_TYPE   ARUBA_IMAGE_EIGER
#define __COMPATIBLE_OS_IMAGE_TYPE  44
#endif

#if defined(CONFIG_X4)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_X4
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_X4
#endif

#if defined(CONFIG_GLENFARCLAS)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_GLENFARCLAS
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_OCTOMORE
#endif

#if defined(CONFIG_GLENFIDDICH)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_GLENFIDDICH
/* this is not a typo; GLENFIDDICH boots Springbank images */
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_SPRINGBANK
#define __SECONDARY_OS_IMAGE_TYPE   ARUBA_IMAGE_GLENFIDDICH
#endif

#if defined(CONFIG_GLENMORANGIE)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_GLENMORANGIE
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_GLENMORANGIE
#endif

#if defined(CONFIG_ABERLOUR)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_ABERLOUR
/* this is not a typo; ABERLOUR boots Gelnmorangie images */
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_GLENMORANGIE
#endif

#if defined(CONFIG_BUNKER)
#define __BOOT_IMAGE_TYPE       ARUBA_IMAGE_BUNKER
/* this is not a typo; BUNKER boots Gelnmorangie images */
#define __OS_IMAGE_TYPE         ARUBA_IMAGE_GLENMORANGIE
#endif

#if defined(CONFIG_MILLSTONE)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_MILLSTONE
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_SPRINGBANK
#define __SECONDARY_OS_IMAGE_TYPE ARUBA_IMAGE_GLENFIDDICH
#endif

#if defined(CONFIG_DEWARS)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_DEWARS
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_SPRINGBANK
#define __SECONDARY_OS_IMAGE_TYPE ARUBA_IMAGE_GLENFIDDICH
#endif

#if defined(CONFIG_KILCHOMAN)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_KILCHOMAN
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_KILCHOMAN
#endif

#if defined(CONFIG_MASTERSON)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_MASTERSON
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_MASTERSON
#endif

#if defined(CONFIG_SIERRA)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_SIERRA
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_OCTOMORE 
#endif

#if defined(CONFIG_SPEYBURN)
#define __BOOT_IMAGE_TYPE       ARUBA_IMAGE_SPEYBURN
/* this is not a typo; SPEYBURN boots Gelnmorangie images */
#define __OS_IMAGE_TYPE         ARUBA_IMAGE_GLENMORANGIE
#endif

#if defined(CONFIG_LAGAVULIN)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_LAGAVULIN
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_LAGAVULIN
#endif

#if defined(CONFIG_LAPHROAIG)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_LAPHROAIG
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_MASTERSON
#endif

#if defined(CONFIG_TOBA)
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_TOBA
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_OCTOMORE 
#endif

#if defined(CONFIG_ARRANTA)
#define __BOOT_IMAGE_TYPE       ARUBA_IMAGE_ARRANTA
/* this is not a typo; ARRANTA boots Gelnmorangie images */
#define __OS_IMAGE_TYPE         ARUBA_IMAGE_GLENMORANGIE
#endif

#define ARUBA_IMAGE_NEXT_HEADER_NONE          0x00000000
#define ARUBA_IMAGE_NEXT_HEADER_SIGN          0x01111111

#define NEXT_HEADER_SIGN_ALGO_UNDEFINED       0             
#define NEXT_HEADER_SIGN_ALGO_RSA_SHA1        1
#define NEXT_HEADER_SIGN_ALGO_RSA_SHA2        2
#define NEXT_HEADER_SIGN_ALGO_RSA2048_SHA256  3

#define NEXT_HEADER_SIGN_RSA_KEY_UNDEFINED    0
#define NEXT_HEADER_SIGN_RSA_KEY_512          1
#define NEXT_HEADER_SIGN_RSA_KEY_1024         2
#define NEXT_HEADER_SIGN_RSA_KEY_2048         3
#define NEXT_HEADER_SIGN_RSA_KEY_4096         4
 
#define NEXT_HEADER_SIGN_CERT_UNSPECIFIED     0
#define NEXT_HEADER_SIGN_CERT_X509_DER        1
#define NEXT_HEADER_SIGN_CERT_X509_PEM        2

#define AP45x_SAP_IMG_START	 0xbed00000
#define AP45x_FLASH_IMG_OFFSET	 0x100000

#define ARUBA_IMAGE_FLAGS_C_TEST_BUILD         0x00000001
#define ARUBA_IMAGE_FLAGS_SWATCH               0x00000002
#define ARUBA_IMAGE_FLAGS_DONT_CLEAR_ON_PURGE  0x00000004
#define ARUBA_IMAGE_FLAGS_SECURE_BOOTLOADER    0x00000008
#define ARUBA_IMAGE_FLAGS_FACTORY_IMAGE        0x00000010
#define ARUBA_IMAGE_FLAGS_FIPS_CERTIFIED       0x00000020


/* TODO: Should this definition be allowed for ArubaOS too? Yes! */
#define ARUBA_SECURE_KEY_MAGIC      "ARUBAKEY" /* Aruba Networks Secure KeyHash */
#define ARUBA_SECURE_XLD_MAGIC      "ARUBASXL" /* Aruba Networks Secure XLoader */
#define ARUBA_SECURE_XLD_MAGIC_SZ   strlen(ARUBA_SECURE_XLD_MAGIC)
#define ARUBA_SECURE_KEY_MAGIC_SZ   strlen(ARUBA_SECURE_KEY_MAGIC)
#define SECURE_XLOAD_IMAGE_SIZE     0x20000    /* Keep it limited to 1-Sector */

/* For MSR image header, port from MeshOS code */
#define ARUBA_MSR_IMAGE_FILE_STYLE_LEN	16
#define ARUBA_MSR_IMAGE_VER_LEN		16
#define ARUBA_MSR_IMAGE_MAX_INFO_LEN	32

/* For SecureBoot capable XLOADER image for Porfidio platform */
#define RSA2048_KEY_LEN             (2048/8)
#define RSA4096_KEY_LEN             (4096/8)
#define SHA512_DIGEST_LEN           (512/8)
#define SHA256_DIGEST_LEN           (256/8)
#define AES256_KEY_LEN              (256/8)

typedef struct _secure_xload_image_header_block_ {
	__u8                 secure_signature[RSA4096_KEY_LEN];
	__u8                 secure_pubkey[RSA4096_KEY_LEN];
} secure_xload_hdr_t;

typedef struct {
    __u32 fpga_type;
    __u32 fpga_hw_ver_id;
    char  fpga_name[32];
    __u32 fpga_board_rev_id;
} aruba_fpga_image_t;

typedef struct {
    __u32 cpboot_ver_id;
} aruba_cpboot_image_t;

typedef struct {
	__u8  xload_key_magic[sizeof(ARUBA_SECURE_KEY_MAGIC) - 1];
	__u32 xload_cpu_rev_id;
	__u8  xload_cpu_key[SHA512_DIGEST_LEN];
	__u8  xload_secure_magic[sizeof(ARUBA_SECURE_XLD_MAGIC) - 1];
} aruba_xloader_magic_t;

#define OEM_SIZE 32

typedef struct {
    char  oem[OEM_SIZE];
} aos_info_t;

typedef struct {
    __u32 size;         /* size of payload that follows */
    __u32 formatVer;    /* this format version */
    __u32 chksum;       /* a simple UINT32 sum of the entire image = 0 */
    char  magic[8];     /* ARUBA_IMAGE_MAGIC */
    /* of the form Linux version 2.4.17_mvl21-mswitch (sdekate@localhost.localdomain) (gcc version 3.2) #1172 Tue Feb 18 10:31:25 PST 2003 */
    char  buildString[256]; 
    char  arubaVersion[24];	/* version eg. "1.0.0.0-Beta" from file .arubaversion */
    char  valid;        /* is image valid? */
    char  type;         /* ARUBA_IMAGE_TYPE_* */
    char  comp;         /* ARUBA_IMAGE_COMPRESSION_* */
    char  machineType;  /* aruba machine type */
    /*
     * FIPS-specific fields
     */
    __u32 image_size;   /* Size of only the image and not extensions */
    __u32 nextHeaderID; /* Extension header following the Image */

    /*
     * MD5 checksum (not used for FIPS)
     * This is the new and improved way of verifying the integrity of an
     * image. The MD5 checksum is a digest over the entire image, with
     * md5_chksum and chksum set to 0. An image made before the
     * introduction of the MD5 checksum has md5_chksum set to all 0s. If
     * this is the case, the MD5 checksum is not verified. Should MD5
     * generate an all-0 checksum, the value in md5_fudge will be changed
     * and a new checksum generated.
     */
    unsigned char  md5_chksum[16];
    __u32 md5_fudge;
    
    /*
    flags - Use these flags to define indicate specific properties of the image
       0x00000001 - C_TEST_BUILD 
       0x00000002 - SWATCH
       0x00000004 - DONT_CLEAR_ON_PURGE
    */
    __u32 flags;

    /*  
    ** This TLV describes about the SHA2 Signature and the values are in NBO
    */
    struct {
          char nextHeaderType;
          __u32 nextHeaderLen;
          __u32 nextHeaderValue;
    } nextHeaderTLV;

    char  pad[384-348]; /* Leave some room for additional common
                           * header. */

    union {
        char  pad[128];	/* pad so the struct size is 512 bytes = 1
                             * sector */
        aruba_fpga_image_t fpga_info;
        aruba_cpboot_image_t cpboot_info;
        aos_info_t         aos_info;
    } u;
} aruba_image_t;

/* TEMPORARILY - used by upgradeImage. for old bootroms - Sameer */
typedef struct {
    __u32 size;         /* size of image that follows */
    char  magic[8];     /* ARUBA_IMAGE_MAGIC */
    char  valid;        /* is image valid? */
    char  date[3];      /* year, month, date downloaded */
    char  time[3];      /* hour, minutes, seconds downloaded */
    char  type;         /* ARUBA_IMAGE_TYPE_* */
    char  comp;         /* ARUBA_IMAGE_COMPRESSION_* */
    __u32 chksum;       /* a simple UINT32 sum of the entire image = 0 */
    char  filename[64]; /* name of file downloaded */
} aruba_image_t_old;

typedef struct {
    char algorithm;     /* Signature Algorithm used. */
    char keySize;       /* Size of the key used for signature */
    char certFormat;
    char nextHeaderID;
    char pad[4];
    __u32 signatureSize; /* Size of the signature */
    __u32 certSize;    /* Size of the certificate */
    __u32 totalSize;   /* total size of the payload considering the padding required for checksum calculation */        
    __u32 chksum;       /* a simple UINT32 sum of the entire signature = 0 */
} aruba_image_ext_sign_t;

/* MSR MeshOS image header, port from MeshOS code */
typedef struct {
    char file_style[ARUBA_MSR_IMAGE_FILE_STYLE_LEN];
    __u16 primary;
    __u16 app_id;
    char image_version[ARUBA_MSR_IMAGE_VER_LEN];
    char kernel_version[ARUBA_MSR_IMAGE_VER_LEN];
    char rootfs_version[ARUBA_MSR_IMAGE_VER_LEN];
    __u32 kernel_offset;
    __u32 kernel_len;
    __u32 rootfs_offset;
    __u32 rootfs_len;
    __u32 checksum;

    char branch[ARUBA_MSR_IMAGE_MAX_INFO_LEN];
    char creator[ARUBA_MSR_IMAGE_MAX_INFO_LEN];
    char timestamp[ARUBA_MSR_IMAGE_MAX_INFO_LEN];
    char type[ARUBA_MSR_IMAGE_MAX_INFO_LEN];

    __u32 kernel_crc;
    __u32 rootfs_crc;
    char app_name[ARUBA_MSR_IMAGE_FILE_STYLE_LEN];
    char app_version[ARUBA_MSR_IMAGE_VER_LEN];
    __u32 app_offset;
    __u32 app_len;
    __u32 app_crc;
    __u32 entry_point;
} aruba_msr_image_t;

typedef struct {
    char *name;
    char *val;
} t_env_var;

struct parmblock {
    t_env_var firmware_version;
    t_env_var boot_image_index;
    t_env_var memsize;
    t_env_var modetty0;
    t_env_var ethaddr;
    t_env_var env_end;
    char *argv[2];
    char text[0];
};

#ifndef __U_BOOT__
/*
 * Offset in NVRAM that contains device:partition to boot first.
 * One byte is used where the device is the upper nibble and
 * the partition is the lower nibble.
 */
#include "nvram.h"
#define ARUBA_IMAGE_NVRAM_OFFSET  NVRAM_BOOT_PART_OFFSET
#endif

extern char *ap_get_image_filename(int image_type);
extern int ap_get_image_type(char *image_file_name);
extern void ap_get_all_image_filenames(char *(*f)(char *));

#endif /* _ARUBA_IMAGE_H_ */
