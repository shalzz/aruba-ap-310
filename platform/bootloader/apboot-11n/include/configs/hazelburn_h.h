/*
 * This file contains the configuration parameters for the Cardhu board (based on db12x).
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <configs/ar7240.h>

#define ARUBA_BOARD_TYPE    "Hazelburn-H"

#define FLASH_SIZE 16

#if !defined(CONFIG_ATH_NAND_FL)
#ifndef FLASH_SIZE
#define FLASH_SIZE 8
#endif
#endif
/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS     1	    /* max number of memory banks */
#if (FLASH_SIZE == 16)
#define CFG_MAX_FLASH_SECT      256    /* max number of sectors on one chip */
#elif (FLASH_SIZE == 8)
#define CFG_MAX_FLASH_SECT      128    /* max number of sectors on one chip */
#else
#define CFG_MAX_FLASH_SECT      64    /* max number of sectors on one chip */
#endif

#define CFG_FLASH_SECTOR_SIZE   (64*1024)
#if (FLASH_SIZE == 16)
#define CFG_FLASH_SIZE          0x01000000 /* Total flash size */
#elif (FLASH_SIZE == 8)
#define CFG_FLASH_SIZE          0x00800000    /* max number of sectors on one chip */
#else
#define CFG_FLASH_SIZE          0x00400000 /* Total flash size */
#endif

#define CONFIG_WASP_SUPPORT 1
#undef CFG_ATHRS26_PHY

#if (CFG_MAX_FLASH_SECT * CFG_FLASH_SECTOR_SIZE) != CFG_FLASH_SIZE
#	error "Invalid flash configuration"
#endif

#define CFG_FLASH_WORD_SIZE     unsigned short

// Set this to zero, for 16bit ddr2
#define DDR2_32BIT_SUPPORT 	0

/* S16 specific defines */

#define CONFIG_AG7240_GE0_IS_CONNECTED 1

/* Assuming nand flash wouldn't use compressed u-boot */
#ifdef CONFIG_ATH_NAND_FL
#	define CFG_FLASH_BASE			0
#else
	/*
	 * We boot from this flash
	 */
#	define CFG_FLASH_BASE			0xbf000000
#endif


/*
 * The following #defines are needed to get flash environment right
 */
#define	CFG_MONITOR_BASE	TEXT_BASE
#define	CFG_MONITOR_LEN		(192 << 10)


#undef CONFIG_BOOTARGS
#undef CFG_PLL_FREQ


#undef CFG_HZ
/*
 * MIPS32 24K Processor Core Family Software User's Manual
 *
 * 6.2.9 Count Register (CP0 Register 9, Select 0)
 * The Count register acts as a timer, incrementing at a constant
 * rate, whether or not an instruction is executed, retired, or
 * any forward progress is made through the pipeline.  The counter
 * increments every other clock, if the DC bit in the Cause register
 * is 0.
 */
/* Since the count is incremented every other tick, divide by 2 */
/* XXX derive this from CFG_PLL_FREQ */
#if (CFG_PLL_FREQ == CFG_PLL_200_200_100)
#   define CFG_HZ          (200000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_300_300_150)
#   define CFG_HZ          (300000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_350_350_175)
#   define CFG_HZ          (350000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_333_333_166)
#   define CFG_HZ          (333000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_266_266_133)
#   define CFG_HZ          (266000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_266_266_66)
#   define CFG_HZ          (266000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_400_400_200) || (CFG_PLL_FREQ == CFG_PLL_400_400_100)
#   define CFG_HZ          (400000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_320_320_80) || (CFG_PLL_FREQ == CFG_PLL_320_320_160)
#   define CFG_HZ          (320000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_410_400_200)
#   define CFG_HZ          (410000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_420_400_200)
#   define CFG_HZ          (420000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_240_240_120)
#   define CFG_HZ          (240000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_160_160_80)
#   define CFG_HZ          (160000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_400_200_200)
#   define CFG_HZ          (400000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_500_400_200)
#   define CFG_HZ          (500000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_600_400_200) || (CFG_PLL_FREQ == CFG_PLL_600_450_200) || \
      (CFG_PLL_FREQ == CFG_PLL_600_600_300) || (CFG_PLL_FREQ == CFG_PLL_600_550_275) || \
      (CFG_PLL_FREQ == CFG_PLL_600_332_166) || (CFG_PLL_FREQ == CFG_PLL_600_575_287) || \
      (CFG_PLL_FREQ == CFG_PLL_600_525_262) || (CFG_PLL_FREQ == CFG_PLL_600_332_200) || \
      (CFG_PLL_FREQ == CFG_PLL_600_266_133) || (CFG_PLL_FREQ == CFG_PLL_600_266_200) || \
      (CFG_PLL_FREQ == CFG_PLL_600_650_325) ||  (CFG_PLL_FREQ == CFG_PLL_600_1_2G_400_200)
#   define CFG_HZ          (600000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_600_1_2G_400_200)
#   define CFG_HZ          (600000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_600_500_1G_250)
#   define CFG_HZ          (600000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_600_550_1_1G_275)
#   define CFG_HZ          (600000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_600_500_250)
#   define CFG_HZ          (600000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_600_350_175)
#   define CFG_HZ          (600000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_600_300_150)
#   define CFG_HZ          (600000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_533_400_200) || (CFG_PLL_FREQ == CFG_PLL_533_500_250)
#   define CFG_HZ          (533000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_700_400_200)
#   define CFG_HZ          (700000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_650_600_300)
#   define CFG_HZ          (650000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_560_480_240)
#   define CFG_HZ          (560000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_566_475_237) || (CFG_PLL_FREQ == CFG_PLL_566_450_225) || \
      (CFG_PLL_FREQ == CFG_PLL_566_550_275) || (CFG_PLL_FREQ == CFG_PLL_566_525_262) || \
      (CFG_PLL_FREQ == CFG_PLL_566_400_200) || (CFG_PLL_FREQ == CFG_PLL_566_500_250)
#   define CFG_HZ          (566000000/2)
#endif

/* Dalmore clock settings */
/* when SRIF PLL mode is used the following settings are ignored */
#undef CFG_PLL_FREQ
#undef CFG_HZ
/* Hazelburn: change DDR clock to 215MHz to avoid the interference to 2.4G radio */
#define CFG_PLL_FREQ    CFG_PLL_560_430_215
#define CFG_HZ (uint64_t)(560000000/2)

/*
 * timeout values are in ticks
 */
#define CFG_FLASH_ERASE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Write */

/*
 * Cache lock for stack
 */
#define CFG_INIT_SP_OFFSET	0x1000

#define	CFG_ENV_IS_IN_FLASH    1
#undef CFG_ENV_IS_NOWHERE

/* Address and size of Primary Environment Sector	*/
#define CFG_TPM_CERT        0xbffa0000
#define CFG_MANUF_BASE      0xbffe0000
#define CFG_ENV_ADDR        0xbfff0000
#define CFG_ENV_SIZE		CFG_FLASH_SECTOR_SIZE


/* DDR init values */

#define CONFIG_NR_DRAM_BANKS	2

/* used for disabling jtag */
#define CONFIG_JTAG_DISABLE 1

/* DDR settings for WASP */
#define CFG_934X_DDR_PRE_PRODUCTION 1
#define CFG_934X_CPU_FROM_SRIF 0
#define CFG_934X_DDR_FROM_SRIF 0
#define CFG_934X_DDR2_FORCED_SETTINGS
#define CFG_934X_DDR2_CONFIG_VAL         0xc7d48cd0
#define CFG_934X_DDR2_CONFIG2_VAL        0x9dd0e6a8
#define CFG_934X_DDR2_MODE_VAL_INIT      0x133
#define CFG_934X_DDR2_MODE_VAL           0x33
#define CFG_934X_DDR2_EXT_MODE_VAL_INIT  0x382
#define CFG_934X_DDR2_EXT_MODE_VAL       0x402
#define CFG_934X_DDR2_EN_TWL_VAL         0xe59
#define CFG_934X_DDR2_TAP_VAL            0x5

#define CFG_DDR_REFRESH_VAL     0x409c

#define CFG_DDR2_RD_DATA_THIS_CYCLE_VAL_32	0xff
#define CFG_DDR2_RD_DATA_THIS_CYCLE_VAL_16	0xffff

#if DDR2_32BIT_SUPPORT
#define CFG_DDR2_RD_DATA_THIS_CYCLE_VAL		CFG_DDR2_RD_DATA_THIS_CYCLE_VAL_32
#else
#define CFG_DDR2_RD_DATA_THIS_CYCLE_VAL		CFG_DDR2_RD_DATA_THIS_CYCLE_VAL_16
#endif

#define CFG_DDR1_RD_DATA_THIS_CYCLE_VAL		0xffff
#define CFG_SDRAM_RD_DATA_THIS_CYCLE_VAL	0xffffffff


#ifdef ENABLE_DYNAMIC_CONF
#define CFG_DDR_MAGIC           0xaabacada
#define CFG_DDR_MAGIC_F         (UBOOT_ENV_SEC_START + CFG_FLASH_SECTOR_SIZE - 0x30)
#define CFG_DDR_CONFIG_VAL_F    *(volatile int *)(CFG_DDR_MAGIC_F + 4)
#define CFG_DDR_CONFIG2_VAL_F	*(volatile int *)(CFG_DDR_MAGIC_F + 8)
#define CFG_DDR_EXT_MODE_VAL_F  *(volatile int *)(CFG_DDR_MAGIC_F + 12)
#endif

#define CONFIG_NET_MULTI
#define CONFIG_MEMSIZE_IN_BYTES
#define CONFIG_PCI 1

#define CONFIG_APBOOT
#define CONFIG_MII

#define CONFIG_NET_RETRY_COUNT 5

#define CONFIG_AUTO_COMPLETE 1
#define CFG_CONSOLE_INFO_QUIET 1

#define	CFG_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)  /* Print Buffer Size */
#undef	CFG_MAXARGS
#define	CFG_MAXARGS		64		/* max number of command args*/

#define CFG_SDRAM_BASE		0x80000000     /* Cached addr */

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#ifndef COMPRESSED_UBOOT
#define CONFIG_COMMANDS (( CONFIG_CMD_DFL        | CFG_CMD_MII   | CFG_CMD_PING  \
   | CFG_CMD_NET | CFG_CMD_PCI     | CFG_CMD_ENV | CFG_CMD_FLASH  \
   | CFG_CMD_RUN | CFG_CMD_ELF | CFG_CMD_BSP  | CFG_CMD_DHCP ) \
   & ~( CFG_CMD_FPGA | CFG_CMD_BDI | CFG_CMD_BEDBUG | CFG_CMD_BOOTD | CFG_CMD_LOADS \
         | CFG_CMD_NFS | CFG_CMD_AUTOSCRIPT | CFG_CMD_LOADB | CFG_CMD_ASKENV \
         | CFG_CMD_IMLS))
#else
#define CONFIG_COMMANDS (( CONFIG_CMD_DFL | CFG_CMD_PING | CFG_CMD_NET))
#endif /* #ifndef COMPRESSED_UBOOT */


#define CONFIG_AR7240 1
#define CFG_ATHRS27_PHY 1
#define CONFIG_ATHRS17_PHY 1
#define CFG_AG7240_NMACS 2

#define CFG_PHY_ADDR 0
#define CFG_GMII     0
#define CFG_MII0_RMII             1
#define CFG_AG7100_GE0_RMII             1

#define CFG_BOOTM_LEN	(16 << 20) /* 16 MB */

#undef CFG_PROMPT
#define CFG_PROMPT "apboot> "

#define AR7100

#define CFG_64BIT_VSPRINTF

#define AP_HAS_SEPARATE_WIRELESS_MAC 1
#define nvram_manuf_t moscato_manuf_t

#define ARUBA_HEADER_SIZE 512

#define AP_PROVISIONING_IMAGE  0xbf800000
#define AP_PROVISIONING_IMAGE_SIZE  (7 * 1024 * 1024)
#define AP_PRODUCTION_IMAGE_SIZE    (7 * 1024 * 1024)

#define __SIGNED_IMAGES__ 1
#define __ENFORCE_SIGNED_IMAGES__ 1
#define APBOOT_SIZE     (1024 * 1024)
#define ARUBA_HEADER_SIZE       512
#define CFG_64BIT_STRTOUL 1

/*
** Parameters defining the location of the calibration/initialization
** information for the two Merlin devices.
** NOTE: **This will change with different flash configurations**
*/


#include <cmd_confdefs.h>

#undef CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DEFAULT		(CONFIG_BOOTP_SUBNETMASK | \
					CONFIG_BOOTP_GATEWAY	 | \
					CONFIG_BOOTP_DNS)
#define CONFIG_BOOTP_MASK      CONFIG_BOOTP_DEFAULT

#define	CONFIG_EXTRA_ENV_SETTINGS			\
	"autoload=n\0"					\
	"boardname=Hazelburn-H\0"				\
	"servername=aruba-master\0"			\
	"bootcmd=boot ap\0"			        \
	"autostart=yes\0"			        \
	"bootfile=mips32.ari\0"			        \
	""

#define	CFG_LONGHELP			/* undef to save memory      */

#define GPIO_RED_STATUS_LED    3
#define GPIO_AMBER_STATUS_LED    3
#define GPIO_SPI1_CS1_L        5
#define GPIO_TPM_DATA          11
#define GPIO_TPM_CLK           16
#define GPIO_TPM_RST           13
#define GPIO_SPI2_CS2_L        4
#define GPIO_GREEN_STATUS_LED  12
#define GPIO_PHY_RESET         22

/* GPIO inputs */
#define	GPIO_POWER_AT		20
#define	GPIO_POWER_DC		14
#define GPIO_CONFIG_CLEAR      1

#define CONFIG_RESET_BUTTON

#define	__CPU_ID__	1
#define	CONFIG_HW_WATCHDOG	1
#undef	CONFIG_BAUDRATE
#define	CONFIG_BAUDRATE		9600
#undef  CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	2
#define CFG_LATE_BOARD_INIT	1

// reset if no command received in two minutes
#define CONFIG_BOOT_RETRY_TIME 120 	// seconds
#define CONFIG_RESET_TO_RETRY 1

/* mfg. sector size */
#define	CFG_MFG_SIZE		0x10000
#define __TLV_INVENTORY__	1
/* the inventory is not this big, but we need to preserve the radio data */
#define __TLV_INVENTORY_BUF_SIZE__	(24 * 1024)
/* for Hazelburn, the variant is on the base board */
#define __CARD_WITH_VARIANT__   1

#define __MODEL_STR__ "AP-103H"

#define CFG_TFTP_STAGING_AREA (32 * 1024 * 1024)

#define ARUBA_AR7100

#define CONFIG_ATH_WMAC	1
#define CONFIG_AR5416   1

#define	__BOARD_HAS_POWER_INFO__

#endif	/* __CONFIG_H */
