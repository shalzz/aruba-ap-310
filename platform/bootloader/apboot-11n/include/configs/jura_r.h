/*
 * This file contains the configuration parameters for the dbau1x00 board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <configs/ar7100.h>

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS     1       /* max number of memory banks */
#define CFG_MAX_FLASH_SECT      256     /* max number of sectors on one chip */
#define CFG_FLASH_SECTOR_SIZE   (64*1024)
#define CFG_FLASH_SIZE          0x01000000 /* Total flash size */

#define CFG_FLASH_WORD_SIZE     unsigned short 
#define CFG_FLASH_ADDR0         (0x5555)        
#define CFG_FLASH_ADDR1         (0x2AAA)

/* 
 * We boot from this flash
 */
#define CFG_FLASH_BASE          0xbf000000

#undef CONFIG_ROOTFS_RD
#undef CONFIG_ROOTFS_FLASH
#undef CONFIG_BOOTARGS_FL
#undef CONFIG_BOOTARGS_RD
#undef CONFIG_BOOTARGS
#undef  MTDPARTS_DEFAULT
#undef  MTDIDS_DEFAULT

//#define CONFIG_ROOTFS_FLASH
//#define CONFIG_BOOTARGS CONFIG_BOOTARGS_FL

//#define CONFIG_BOOTARGS_RD     "console=ttyS0,115200 root=01:00 rd_start=0x80600000 rd_size=5242880 init=/sbin/init mtdparts=ar9100-nor0:256k(u-boot),64k(u-boot-env),4096k(rootfs),2048k(uImage)"

/* XXX - putting rootfs in last partition results in jffs errors */

//#define CONFIG_BOOTARGS_FL     "console=ttyS0,115200 root=31:02 rootfstype=jffs2 init=/sbin/init mtdparts=ar7100-nor0:256k(uboot),128k(env),6144k(rootfs),64k(caldata),1024k(uImage)"

//#define MTDPARTS_DEFAULT    "mtdparts=ar7100-nor0:256k(u-boot),128k(env),6144k(rootfs),64k(caldata),1024k(uImage)"
//#define MTDIDS_DEFAULT      "nor0=ar7100-nor0"

/* 
 * The following #defines are needed to get flash environment right 
 */
#define CFG_MONITOR_BASE    TEXT_BASE
#define CFG_MONITOR_LEN     (192 << 10)

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

#define CFG_HZ	(uint64_t)(680000000/2)

/* 
 * timeout values are in ticks 
 */
#define CFG_FLASH_ERASE_TOUT    (2 * CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT    (2 * CFG_HZ) /* Timeout for Flash Write */

/*
 * Cache lock for stack
 */
#define CFG_INIT_SP_OFFSET  0x1000

#define CFG_ENV_IS_IN_FLASH    1
#undef CFG_ENV_IS_NOWHERE  

/* Address and size of Primary Environment Sector   */
#define CFG_TPM_CERT        0xbffa0000
#define CFG_MANUF_BASE      0xbffe0000
#define CFG_ENV_ADDR        0xbfff0000
#define CFG_ENV_SIZE        0x10000

//#define CONFIG_BOOTCOMMAND "bootm 0xbf670000"

#define CONFIG_NR_DRAM_BANKS    		2

/*
** Parameters defining the location of the calibration/initialization
** information for the two Merlin devices.
** NOTE: **This will change with different flash configurations**
*/

#ifdef notused
#define MERLIN24CAL			0xbf661000
#define MERLIN50CAL			0xbf665000
#define BOARDCAL			0xbf660000
#define ATHEROS_PRODUCT_ID		136
#define CAL_SECTOR			102
#endif

/*
** Configure the CPU clock settings
*/

#define CFG_PLL_FREQ	CFG_PLL_680_340_170
/*
** These values for DDR configuration came from the
** BDM configuration script
*/

// DDR_REFRESH and DDR_CONFIG updated per Thomas
#define CFG_DDR_REFRESH_VAL     		0x4564		// 0xB8000014
#define CFG_DDR_CONFIG_VAL      		0x6fb88cce	// 0xB8000000
#define CFG_DDR_CONFIG2_VAL             	0x812cd6a8	// 0xB8000004
#define CFG_DDR_MODE_VAL_INIT   		0x131		// 0xB8000008 (9100)
// 0x63 == CAS 6
#define CFG_DDR_MODE_VAL        		0x63		// 0xB8000008
#define CFG_DDR_EXT_MODE_VAL    		0x0		// 0xB800000C
//#define CFG_DDR_TRTW_VAL        		0x1f		// not used
//#define CFG_DDR_TWTR_VAL        		0x1e		// not used
#define CFG_DDR_RD_DATA_THIS_CYCLE_VAL  	0xff		// 0xB8000018


#define CONFIG_NET_MULTI
#define CONFIG_MEMSIZE_IN_BYTES
#define CONFIG_PCI

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

#define CONFIG_COMMANDS (( CONFIG_CMD_DFL        | CFG_CMD_MII   | CFG_CMD_PING  \
   | CFG_CMD_NET | CFG_CMD_PCI     | CFG_CMD_ENV | CFG_CMD_FLASH  \
   | CFG_CMD_RUN | CFG_CMD_ELF | CFG_CMD_BSP  | CFG_CMD_DHCP ) \
   & ~( CFG_CMD_FPGA | CFG_CMD_BDI | CFG_CMD_BEDBUG | CFG_CMD_BOOTD | CFG_CMD_LOADS \
         | CFG_CMD_NFS | CFG_CMD_AUTOSCRIPT | CFG_CMD_LOADB | CFG_CMD_ASKENV \
         | CFG_CMD_IMLS))

#include <cmd_confdefs.h>

#undef CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DEFAULT		(CONFIG_BOOTP_SUBNETMASK | \
					CONFIG_BOOTP_GATEWAY	 | \
					CONFIG_BOOTP_DNS)

#define CONFIG_BOOTP_MASK		CONFIG_BOOTP_DEFAULT
/*
** Ethernet Configuration
** Set default values, and select the PHY to use
*/

//#define CONFIG_IPADDR       		192.168.1.2
//#define CONFIG_SERVERIP     		192.168.1.1
//#define CONFIG_ETHADDR      		00:03:7f:ff:ff:fe
//#define CFG_FAULT_ECHO_LINK_DOWN    1
#define CONFIG_PHY_GIGE       		1              /* GbE speed/duplex detect */

#define CFG_VSC8601_PHY				1
#define CFG_AG7100_NMACS 			1
//#define CFG_ATHRHDR_REG 			1
//#define CFG_SWITCH_FREQ				1

//#define CFG_ATHRHDR_EN 				1
//#define ATHRHDR_LEN   				2
//#define ATHRHDR_MAX_DATA  			10

#define CFG_PHY_ADDR				0  /* Port 0 */
#define CFG_GMII     				0
#define CFG_MII0_RGMII				1
//#define CFG_MII1_RMII				1

/*
** Configure Parser
*/

#define CFG_BOOTM_LEN   (16 << 20) /* 16 MB */
//#define DEBUG
//#define CFG_HUSH_PARSER
//#define CFG_PROMPT_HUSH_PS2 "hush>"
#undef CFG_PROMPT
#define CFG_PROMPT "apboot> "

#define AR7100
#define CFG_64BIT_VSPRINTF

#define nvram_manuf_t std_manuf_t

#define ARUBA_HEADER_SIZE 512

#define CONFIG_AR5416	1

#define ARUBA_BOARD_TYPE	"Jura-R"
#ifdef notyet
#define AP_PROVISIONING_IMAGE   0xbf500000

#define AP_PROVISIONING_IMAGE_SIZE      (4 * 1024 * 1024)
#endif
#define AP_PRODUCTION_IMAGE_SIZE        (8 * 1024 * 1024)

#define __SIGNED_IMAGES__ 1
#define __ENFORCE_SIGNED_IMAGES__ 1
#define APBOOT_SIZE     (1024 * 1024)
#define ARUBA_HEADER_SIZE       512
#define CFG_64BIT_STRTOUL 1

#define	CONFIG_EXTRA_ENV_SETTINGS			\
	"autoload=n\0"					\
	"boardname=Jura-R\0"				\
	"servername=aruba-master\0"			\
	"bootcmd=boot ap\0"			        \
	"autostart=yes\0"			        \
	"bootfile=mips32.ari\0"			        \
	""
#define CFG_AG7100_GE0_RGMII 1
#define	CFG_LONGHELP			/* undef to save memory      */

#define	GPIO_2G_GREEN_LED	0
#define	GPIO_5G_GREEN_LED	1
#define GPIO_RED_STATUS_LED	2
#define GPIO_GREEN_STATUS_LED	3
#define GPIO_TPM_CLK            4
#define GPIO_TPM_DATA           5
#define GPIO_FLASH_WRITE        6
#define GPIO_HW_RESET           7
#define	GPIO_2G_RED_LED		8
#define	GPIO_5G_RED_LED		11

#define FLASH_WRITE_ENABLE	1
#define FLASH_WRITE_DISABLE	0

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

#define __MODEL_STR__ "AP-18x"

#define CFG_TFTP_STAGING_AREA (32 * 1024 * 1024)

// low-budget I/O configuration
#undef CONFIG_DTT_LM75
#undef CONFIG_DTT_SENSORS
#define CFG_EEPROM_PAGE_WRITE_ENABLE
#define CFG_EEPROM_PAGE_WRITE_BITS  2
#undef CONFIG_RTC_S35390

#define ARUBA_AR7100
#endif  /* __CONFIG_H */
