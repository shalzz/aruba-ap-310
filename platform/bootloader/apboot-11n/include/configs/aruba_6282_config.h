#ifndef __aruba_6282_config_h__
#define __aruba_6282_config_h__

#define	CFG_64BIT_VSPRINTF

#define ARUBA_BOARD_TYPE	"Arran"

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

#define CONFIG_NET_MULTI
#define CONFIG_MEMSIZE_IN_BYTES
#define CONFIG_PCI

#define CONFIG_MII
#define CONFIG_PHY_GIGE

#define CONFIG_NET_RETRY_COUNT 5

#undef CONFIG_AUTO_COMPLETE
#define CONFIG_AUTO_COMPLETE 1
#undef CFG_CONSOLE_INFO_QUIET
#define CFG_CONSOLE_INFO_QUIET 1

#undef  CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	2
//#define CFG_LATE_BOARD_INIT	1

// reset if no command received in two minutes
#define CONFIG_BOOT_RETRY_TIME 120 	// seconds
#define CONFIG_RESET_TO_RETRY 1

#define	CONFIG_EXTRA_ENV_SETTINGS			\
	"autoload=n\0"					\
	"boardname=Arran\0"				\
	"servername=aruba-master\0"			\
	"bootcmd=boot ap\0"			        \
	"autostart=yes\0"			        \
	"bootfile=armv5te.ari\0"		        \
	""

#undef CFG_PROMPT
#define CFG_PROMPT "apboot> "

#define CFG_TFTP_STAGING_AREA (32 * 1024 * 1024)

#define	nvram_manuf_t std_manuf_t

//#define CFG_TPM_CERT        0xbffa0000
#define CFG_MANUF_BASE      0xf8f80000
#undef CFG_ENV_ADDR
#define CFG_ENV_ADDR        0xf8fc0000
//#define CFG_ENV_SIZE        0x40000
#define CFG_MFG_SIZE        0x40000

#undef CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DEFAULT		(CONFIG_BOOTP_SUBNETMASK | \
					CONFIG_BOOTP_GATEWAY	 | \
					CONFIG_BOOTP_DNS)

#undef CONFIG_BOOTP_MASK
#define CONFIG_BOOTP_MASK		CONFIG_BOOTP_DEFAULT

#define	__MODEL_STR__	"AP-13x"

#define CONFIG_AR5416

#define       CONFIG_HW_WATCHDOG      1

// work around lack of CONFIG_ARRAN define
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_ARRAN
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_ARRAN

#undef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE         9600

#define	__TLV_INVENTORY__	1
#define INVENT_MAX_INVENTORIES  1

#define	CFG_LATE_BOARD_INIT

#endif
