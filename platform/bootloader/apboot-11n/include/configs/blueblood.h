/*
 * This file contains the configuration parameters for the BlueBlood board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <configs/ar7240.h>
/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS     1	    /* max number of memory banks */
#define CFG_MAX_FLASH_SECT      128    /* max number of sectors on one chip */
#define CFG_FLASH_SECTOR_SIZE   (64*1024)
#define CFG_FLASH_SIZE          0x00800000 /* Total flash size */

//#define ENABLE_DYNAMIC_CONF 1

#if (CFG_MAX_FLASH_SECT * CFG_FLASH_SECTOR_SIZE) != CFG_FLASH_SIZE
#	error "Invalid flash configuration"
#endif

#define CFG_FLASH_WORD_SIZE     unsigned short 

/* 
 * We boot from this flash
 */
#define CFG_FLASH_BASE		    0xbf000000

/* 
 * Defines to change flash size on reboot
 */
#ifdef ENABLE_DYNAMIC_CONF
#define UBOOT_FLASH_SIZE          (256 * 1024)
#define UBOOT_ENV_SEC_START        (CFG_FLASH_BASE + UBOOT_FLASH_SIZE)

#define CFG_FLASH_MAGIC           0xaabacada  
#define CFG_FLASH_MAGIC_F         (UBOOT_ENV_SEC_START + CFG_FLASH_SECTOR_SIZE - 0x20)
#define CFG_FLASH_SECTOR_SIZE_F   *(volatile int *)(CFG_FLASH_MAGIC_F + 0x4)
#define CFG_FLASH_SIZE_F          *(volatile int *)(CFG_FLASH_MAGIC_F + 0x8) /* Total flash size */
#define CFG_MAX_FLASH_SECT_F      (CFG_FLASH_SIZE / CFG_FLASH_SECTOR_SIZE) /* max number of sectors on one chip */
#endif


/* 
 * The following #defines are needed to get flash environment right 
 */
#define	CFG_MONITOR_BASE	TEXT_BASE
#define	CFG_MONITOR_LEN		(192 << 10)

#undef CONFIG_BOOTARGS
/* XXX - putting rootfs in last partition results in jffs errors */
//#define	CONFIG_BOOTARGS     "console=ttyS0,115200 root=31:02 rootfstype=jffs2 init=/sbin/init mtdparts=ar7240-nor0:256k(u-boot),64k(u-boot-env),2752k(rootfs),896k(uImage),64k(NVRAM),64k(ART)"

/* default mtd partition table */
#undef MTDPARTS_DEFAULT
//#define MTDPARTS_DEFAULT    "mtdparts=ar7240-nor0:256k(u-boot),64k(u-boot-env),5120k(rootfs),1024k(uImage)"

#undef CFG_PLL_FREQ
#define CFG_PLL_FREQ	CFG_PLL_400_400_200


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
#	define CFG_HZ          (200000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_300_300_150)
#	define CFG_HZ          (300000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_350_350_175)
#	define CFG_HZ          (350000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_333_333_166)
#	define CFG_HZ          (333000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_266_266_133)
#	define CFG_HZ          (266000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_266_266_66)
#	define CFG_HZ          (266000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_400_400_200) || (CFG_PLL_FREQ == CFG_PLL_400_400_100)
#	define CFG_HZ          (400000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_320_320_80) || (CFG_PLL_FREQ == CFG_PLL_320_320_160)
#	define CFG_HZ          (320000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_410_400_200)
#	define CFG_HZ          (410000000/2)
#elif (CFG_PLL_FREQ == CFG_PLL_420_400_200)
#	define CFG_HZ          (420000000/2)
#endif


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
#define CFG_TPM_CERT        0xbf7a0000
#define CFG_MANUF_BASE      0xbf7e0000
#define CFG_ENV_ADDR        0xbf7f0000
#define CFG_ENV_SIZE        0x10000

//#define CONFIG_BOOTCOMMAND "bootm 0x9f300000"
//#define CONFIG_FLASH_16BIT

/* DDR init values */

#define CONFIG_NR_DRAM_BANKS	2
#define CFG_DDR_REFRESH_VAL     0x4f10
#define CFG_DDR_CONFIG_VAL      0x7fbc8cd0
#define CFG_DDR_MODE_VAL_INIT   0x133
#define LOW_DRIVE_STRENGTH
#ifdef LOW_DRIVE_STRENGTH
#	define CFG_DDR_EXT_MODE_VAL    0x2
#else
#	define CFG_DDR_EXT_MODE_VAL    0x0
#endif
#define CFG_DDR_MODE_VAL        0x33

#define CFG_DDR_TRTW_VAL        0x1f
#define CFG_DDR_TWTR_VAL        0x1e

#define CFG_DDR_CONFIG2_VAL	 0x99d0e6a8
#define CFG_DDR_RD_DATA_THIS_CYCLE_VAL  0x00ff

/* DDR2 Init values */
#define CFG_DDR2_EXT_MODE_VAL    0x402

/* DDR value from Flash */
#ifdef ENABLE_DYNAMIC_CONF
#define CFG_DDR_MAGIC           0xaabacada  
#define CFG_DDR_MAGIC_F         (UBOOT_ENV_SEC_START + CFG_FLASH_SECTOR_SIZE - 0x30)
#define CFG_DDR_CONFIG_VAL_F    *(volatile int *)(CFG_DDR_MAGIC_F + 4)
#define CFG_DDR_CONFIG2_VAL_F	*(volatile int *)(CFG_DDR_MAGIC_F + 8)
#define CFG_DDR_EXT_MODE_VAL_F  *(volatile int *)(CFG_DDR_MAGIC_F + 12)
#endif

#define CONFIG_NET_MULTI

#define CONFIG_MEMSIZE_IN_BYTES
#define CONFIG_PCI

#define CONFIG_APBOOT
#define CONFIG_MII

#define CONFIG_NET_RETRY_COUNT 5

#define CONFIG_AUTO_COMPLETE 1
#define CFG_CONSOLE_INFO_QUIET 1
/*-----------------------------------------------------------------------
 * Cache Configuration
 */

#define CONFIG_COMMANDS (( CONFIG_CMD_DFL        | CFG_CMD_MII   | CFG_CMD_PING  \
   | CFG_CMD_NET | CFG_CMD_PCI     | CFG_CMD_ENV | CFG_CMD_FLASH  \
   | CFG_CMD_RUN | CFG_CMD_ELF | CFG_CMD_BSP  | CFG_CMD_DHCP ) \
   & ~( CFG_CMD_FPGA | CFG_CMD_BDI | CFG_CMD_BEDBUG | CFG_CMD_BOOTD | CFG_CMD_LOADS \
         | CFG_CMD_NFS | CFG_CMD_AUTOSCRIPT | CFG_CMD_LOADB | CFG_CMD_ASKENV \
         | CFG_CMD_IMLS))

//#define CONFIG_IPADDR   192.168.1.10
//#define CONFIG_SERVERIP 192.168.1.27
//#define CONFIG_ETHADDR 0x00:0xaa:0xbb:0xcc:0xdd:0xee
#define CFG_FAULT_ECHO_LINK_DOWN    1


#define CFG_PHY_ADDR 0 
#define CFG_AG7240_NMACS 1
#define CFG_GMII     0
#define CFG_MII0_RMII             1
#define CFG_AG7100_GE0_RMII             1

#define CFG_BOOTM_LEN	(16 << 20) /* 16 MB */
//#define DEBUG
//#define CFG_HUSH_PARSER
//#define CFG_PROMPT_HUSH_PS2 "hush>"

/*
** Parameters defining the location of the calibration/initialization
** information for the two Merlin devices.
** NOTE: **This will change with different flash configurations**
*/

#if 0
#define WLANCAL                        0xbfff1000
#define BOARDCAL                        0xbfff0000
#define ATHEROS_PRODUCT_ID              138
#define CAL_SECTOR                      (CFG_MAX_FLASH_SECT - 1)

/* For Kite, only PCI-e interface is valid */
#define AR7240_ART_PCICFG_OFFSET        3
#endif

#define CONFIG_APBOOT

#undef CFG_PROMPT
#define CFG_PROMPT "apboot> "

#define AR7100
#define CFG_64BIT_VSPRINTF

#define nvram_manuf_t std_manuf_t

#define ARUBA_HEADER_SIZE 512

#define CONFIG_AR5416	1

#define ARUBA_BOARD_TYPE	"BlueBlood"
#define AP_PRODUCTION_IMAGE_SIZE        (6 * 1024 * 1024)

#define __SIGNED_IMAGES__ 1
#define __ENFORCE_SIGNED_IMAGES__ 1
#define APBOOT_SIZE     (1024 * 1024)
#define ARUBA_HEADER_SIZE       512
#define CFG_64BIT_STRTOUL 1

#define	CONFIG_EXTRA_ENV_SETTINGS			\
	"autoload=n\0"					\
	"boardname=BlueBlood\0"				\
	"servername=aruba-master\0"			\
	"bootcmd=boot ap\0"			        \
	"autostart=yes\0"			        \
	"bootfile=mips32.ari\0"			        \
	""
#define CFG_AG7240_GE0_RGMII 1
#define	CFG_LONGHELP			/* undef to save memory      */

#define GPIO_RED_STATUS_LED	0
#define GPIO_GREEN_STATUS_LED	14
#define GPIO_TPM_CLK            11
#define GPIO_TPM_DATA           12
//#define GPIO_FLASH_WRITE        13	// XXX
#define GPIO_HW_RESET           7
#define	GPIO_2G_RED_LED		15
#define	GPIO_2G_GREEN_LED	16
#define GPIO_CONFIG_CLEAR       1

#define FLASH_WRITE_ENABLE	1
#define FLASH_WRITE_DISABLE	0

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

#define __MODEL_STR__ "AP-68X"

#define CFG_TFTP_STAGING_AREA (32 * 1024 * 1024)

#include <cmd_confdefs.h>

// low-budget I/O configuration
//#define CONFIG_DTT_LM75 1
//#define CONFIG_DTT_SENSORS {0x48}
//#define CFG_EEPROM_PAGE_WRITE_ENABLE
//#define CFG_EEPROM_PAGE_WRITE_BITS  2
//#define CONFIG_RTC_S35390 1

#define ARUBA_AR7100

#undef CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DEFAULT            (CONFIG_BOOTP_SUBNETMASK | \
                                        CONFIG_BOOTP_GATEWAY     | \
                                        CONFIG_BOOTP_DNS)

#define CONFIG_BOOTP_MASK               CONFIG_BOOTP_DEFAULT

#define	ARUBA_S26


#endif	/* __CONFIG_H */
