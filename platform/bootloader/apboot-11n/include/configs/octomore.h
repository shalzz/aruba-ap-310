
/* * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.* */

#ifndef _OCTOMORE_H_
#define _OCTOMORE_H_

#if !defined(DO_DEPS_ONLY)
/*
 * Beat the system! tools/scripts/make-asm-offsets uses
 * the following hard-coded define for both u-boot's
 * ASM offsets and platform specific ASM offsets :(
 */
//#include <generated/generic-asm-offsets.h>
#ifdef __ASM_OFFSETS_H__
#undef __ASM_OFFSETS_H__
#endif
#if !defined(DO_SOC_DEPS_ONLY)
//#include <generated/generic-asm-offsets.h>
#endif
#endif /* !DO_DEPS_ONLY */

/*
 * Disabled for actual chip.
 * #define CONFIG_RUMI
 */

#define CONFIG_BOARD_EARLY_INIT_F

#define CFG_64BIT_VSPRINTF
#define CFG_64BIT_STRTOUL

#define CFG_ENV_OFFSET                  board_env_offset
#define CONFIG_IPQ806X_UART
//#define IPROC_ETH_MALLOC_BASE           0xD00000
#define CONFIG_APBOOT
#undef CONFIG_CMD_FPGA		            /* FPGA configuration support */
#undef CONFIG_CMD_IMI
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_NFS		            /* NFS support */
#define CONFIG_CMD_NET		            /* network support */
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#undef CONFIG_SYS_MAX_FLASH_SECT
#define CONFIG_NR_DRAM_BANKS            1 
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CFG_CBSIZE                      1024
#define CFG_LONGHELP
#define CFG_PROMPT                      "apboot> "
#define CFG_MAXARGS                     64
#define CONFIG_NET_MULTI 
#define ARUBA_ARMV7
#define CONFIG_SYS_NUM_CORES            2
#define __CARD_WITH_VARIANT__           2
#define __TLV_INVENTORY__               1
#define INVENT_MAX_INVENTORIES          1
#define __TLV_INVENTORY_BUF_SIZE__      (24 * 1024)
#define __TLV_INVENTORY_NO_RELOCATE__	/* ARMv7 -pie will take care of this */
#define CONFIG_COMMANDS ((CFG_CMD_DFL | CFG_CMD_DHCP | CFG_CMD_ELF | CFG_CMD_PCI | CFG_CMD_MEMORY | CMD_CMD_PORTIO | CFG_CMD_ITEST | CFG_CMD_NET | CFG_CMD_PING  |CFG_CMD_FLASH |CFG_CMD_ENV | CFG_CMD_NAND | CFG_CMD_UBI) | CFG_CMD_MII)
#include <cmd_confdefs.h>

#define CFG_64BIT_VSPRINTF

#define CFG_BAUDRATE_TABLE              {9600, 19200, 38400, 57600, 115200 }
#define CFG_PBSIZE                      (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)
#define CFG_MAX_NAND_DEVICE             1
#define CFG_NAND_BASE                   0 /* fixme_octomore */
#define CFG_MEMTEST_START               0
#define CFG_MEMTEST_END                 0
#define CFG_MANUF_BASE                  board_mfg_offset
#define CFG_MANUF_NAND_BASE             0x5a40000
#define CFG_MANUF_NOR_BASE              0x390000
#define CFG_MFG_SIZE                    0x10000
#define CFG_ART_BASE                    0x330000
#define CFG_TPM_CERT                    0x380000
#define APBOOT_IMAGE                    board_apboot_offset

#define QCA_CAL_IN_FLASH
#define USE_FLASH_READ_SUPPORT
//#define CONFIG_AUTO_COMPLETE 1

/* Environment */
#define CONFIG_MSM_PCOMM
#define CONFIG_ARCH_CPU_INIT

#define CFG_FLASH_BASE                  CONFIG_SYS_FLASH_BASE 
#define CFG_ENV_ADDR                    CONFIG_ENV_ADDR
#define CFG_MAX_FLASH_BANKS             1
#define CFG_LOAD_ADDR                   CFG_TFTP_STAGING_AREA
#define CONFIG_BOOTM_LINUX
#define CFG_HZ				            1000
#define CFG_TFTP_STAGING_AREA           (CONFIG_SYS_SDRAM_BASE + (64 << 20)) 
#define ARUBA_HEADER_SIZE               512
#define nvram_manuf_t                   std_manuf_t
#define CFG_SDRAM_BASE                  CONFIG_SYS_SDRAM_BASE
#define APBOOT_SIZE                     (1024 * 1024)
#define AP_PRODUCTION_IMAGE             0x0
#define AP_PRODUCTION_IMAGE_SIZE        (24 * 1024 * 1024)
#define AP_PRODUCTION_MTD_SIZE          0x2000000
#define AP_PRODUCTION_IMAGE_NAME        "aos0"
#define AP_PROVISIONING_IMAGE           0x2000000
#define AP_PROVISIONING_IMAGE_SIZE      (24 * 1024 * 1024)
#define AP_PROVISIONING_MTD_SIZE        0x2000000
#define AP_PROVISIONING_IMAGE_NAME      "aos1"
#define AP_UBI_FS_OFFSET                0x4000000
#define AP_UBI_FS_SIZE                  0x4000000
#define AP_UBI_FS_NAME                  "ubifs"
//#define CFG_FLASH_SIZE                  (4 * 1024 * 1024)
#define MTDIDS_DEFAULT                  "nand0=nand0"
#define CONFIG_MTD_MAX_DEVICE           3
#define ARUBA_BOARD_TYPE                "Octomore"
#define __SIGNED_IMAGES__               1
#define __ENFORCE_SIGNED_IMAGES__       1
#define AP_SLOW_FLASH_STAGING_AREA      CFG_TFTP_STAGING_AREA
#define CONFIG_VERIFY_LOAD_ADDR         (CONFIG_SYS_SDRAM_BASE + (32 << 20)) 
/*
 * Size of malloc() pool
 */

/*
 * select serial console configuration
 */
#define CONFIG_CONS_INDEX               1

/*
 * Enable crash dump support, this will dump the memory
 * regions via TFTP in case magic word found in memory
 */
#define CONFIG_IPQ_APPSBL_DLOAD

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE                 9600 
#define CONFIG_SYS_BAUDRATE_TABLE       {4800, 9600, 19200, 38400, 57600, 115200}

#define V_PROMPT                        "(Octomore) # "
#define CONFIG_SYS_PROMPT               V_PROMPT
#define CONFIG_SYS_CBSIZE               (256 * 2) /* Console I/O Buffer Size */

#define CONFIG_SYS_INIT_SP_ADDR         CONFIG_SYS_SDRAM_BASE + GENERATED_IPQ_RESERVE_SIZE \
                                            - GENERATED_GBL_DATA_SIZE
#define CONFIG_SYS_MAXARGS              16
#define GENERATED_IPQ_RESERVE_SIZE      (0x1500000) /* sizeof(ipq_mem_reserve_t) */
//#define GENERATED_IPQ_RESERVE_SIZE      (0x8388608) /* sizeof(ipq_mem_reserve_t) */
//#define GENERATED_IPQ_RESERVE_SIZE      (sizeof(ipq_mem_reserve_t)) /* sizeof(ipq_mem_reserve_t) */

#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/* from ipq */
//#define CONFIG_ENV_SIZE                 CONFIG_ENV_SECT_SIZE
#define CONFIG_ENV_SIZE                 0x10000
#define CFG_ENV_SIZE                    CONFIG_ENV_SIZE 
#define CONFIG_SYS_MALLOC_LEN           (4 << 20)
#define CONFIG_SYS_SDRAM_BASE           0x40000000
//#define CONFIG_SYS_TEXT_BASE            0x41200000
#define CONFIG_SYS_SDRAM_SIZE           0x10000000
#define CONFIG_MAX_RAM_BANK_SIZE        CONFIG_SYS_SDRAM_SIZE

#define CFG_MAX_FLASH_SECT              64
#define NAND_MAX_CHIPS                  1
#define CONFIG_IDENT_STRING             "\noctomore"
#define TOTAL_MALLOC_LEN                CONFIG_SYS_MALLOC_LEN
#define CFG_MONITOR_BASE                CONFIG_SYS_TEXT_BASE

#define CONFIG_ENV_SPI_MAX_HZ           50000000
#define CONFIG_SYS_MAX_FLASH_BANKS      1

#define CONFIG_IPQ806X_PCI

#ifdef CONFIG_IPQ806X_PCI
#define CONFIG_PCI
#define CONFIG_CMD_PCI
#define CONFIG_PCI_SCAN_SHOW
#define CONFIG_AR5416
#define CONFIG_BOARD_PCI_DEINIT
#endif

#ifndef __ASSEMBLY__
#include <compiler.h>
#include "../../board/qca/octomore/ipq806x_cdp.h"
extern loff_t board_env_offset;
extern loff_t board_env_range;
extern loff_t board_mfg_offset;
extern loff_t board_apboot_offset;
extern uint32_t flash_index;
extern uint32_t flash_chip_select;
extern uint32_t flash_block_size;
extern board_ipq806x_params_t *gboard_param;

static uint32_t inline clk_is_dummy(void)
{
	return gboard_param->clk_dummy;
}

/*
 * XXX XXX Please do not instantiate this structure. XXX XXX
 * This is just a convenience to avoid
 *      - adding #defines for every new reservation
 *      - updating the multiple associated defines like smem base,
 *        kernel start etc...
 *      - re-calculation of the defines if the order changes or
 *        some reservations are deleted
 * For new reservations just adding a member to the structure should
 * suffice.
 * Ensure that the size of this structure matches with the definition
 * of the following IPQ806x compile time definitions
 *      PHYS_OFFSET     (linux-sources/arch/arm/mach-msm/Kconfig)
 *      zreladdr        (linux-sources/arch/arm/mach-msm/Makefile.boot)
 *      CONFIG_SYS_INIT_SP_ADDR defined above should point to the bottom.
 *
 */

#if !defined(DO_DEPS_ONLY) || defined(DO_SOC_DEPS_ONLY)
#include "generated/generic-asm-offsets.h"
#if 1
typedef struct {
	uint8_t	nss[16 * 1024 * 1024];
	uint8_t	smem[2 * 1024 * 1024];
	uint8_t	uboot[1 * 1024 * 1024];
	uint8_t	nsstcmdump[128 * 1024];
	uint8_t sbl3[384 * 1024];
	uint8_t plcfwdump[512*1024];
	uint8_t wlanfwdump[(1 * 1024 * 1024) - GENERATED_GBL_DATA_SIZE];
	uint8_t init_stack[GENERATED_GBL_DATA_SIZE];
} __attribute__ ((__packed__)) ipq_mem_reserve_t;
#else
typedef struct {
	uint8_t	nss[4 * 1024 * 1024];
	uint8_t	smem[2 * 1024 * 1024];
	uint8_t	uboot[1 * 1024 * 1024];
	uint8_t	nsstcmdump[1 * 1024 * 1024];
} __attribute__ ((__packed__)) ipq_mem_reserve_t;
#endif

/* Convenience macros for the above convenience structure :-) */
#define IPQ_MEM_RESERVE_SIZE(x)		    sizeof(((ipq_mem_reserve_t *)0)->x)
#define IPQ_MEM_RESERVE_BASE(x)		    (CONFIG_SYS_SDRAM_BASE + \
	                                        ((uint32_t)&(((ipq_mem_reserve_t *)0)->x)))
#endif

#define CONFIG_IPQ_SMEM_BASE		    IPQ_MEM_RESERVE_BASE(smem)
#define IPQ_KERNEL_START_ADDR	        (CONFIG_SYS_SDRAM_BASE +  \
                                            GENERATED_IPQ_RESERVE_SIZE)

#define IPQ_DRAM_KERNEL_SIZE	        (CONFIG_SYS_SDRAM_SIZE - \
                                            GENERATED_IPQ_RESERVE_SIZE)

#define IPQ_BOOT_PARAMS_ADDR		    (IPQ_KERNEL_START_ADDR + 0x100)

#define IPQ_NSSTCM_DUMP_ADDR		    (IPQ_MEM_RESERVE_BASE(nsstcmdump))

#define IPQ_TEMP_DUMP_ADDR		        (IPQ_MEM_RESERVE_BASE(nsstcmdump))

#endif /* __ASSEMBLY__ */

#define CONFIG_CMD_MEMORY

#define CONFIG_CMDLINE_TAG              1	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS        1
#define CONFIG_INITRD_TAG               1

#define CONFIG_CMD_IMI

#define CONFIG_CMD_SOURCE               1

#define CONFIG_FIT
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_NULLDEV
#define CONFIG_CMD_XIMG

#define IPQ_ROOT_FS_PART_NAME		    "rootfs"
/*
 * SPI Flash Configs
 */

#define CONFIG_IPQ_SPI
#define CONFIG_SPI_FLASH
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_MACRONIX
#define CONFIG_SPI_FLASH_STMICRO
#define CONFIG_SYS_HZ                   1000

#define CONFIG_SF_DEFAULT_BUS           0
#define CONFIG_SF_DEFAULT_CS            0
#define CONFIG_SF_DEFAULT_MODE SPI_MODE_0

/*
 * NAND Flash Configs
 */

#define CONFIG_IPQ_NAND
#define CONFIG_CMD_NAND
#define CONFIG_CMD_NAND_YAFFS
#define CONFIG_CMD_MEMORY
#define CONFIG_SYS_NAND_SELF_INIT
#define CONFIG_SYS_NAND_ONFI_DETECTION

#define CONFIG_IPQ_MAX_SPI_DEVICE       1
#define CONFIG_IPQ_MAX_NAND_DEVICE	    1

#define CONFIG_IPQ_NAND_NAND_INFO_IDX	0
#define CONFIG_IPQ_SPI_NAND_INFO_IDX	1

/*
 * Expose SPI driver as a pseudo NAND driver to make use
 * of U-Boot's MTD framework.
 */
#define CONFIG_SYS_MAX_NAND_DEVICE	(CONFIG_IPQ_MAX_NAND_DEVICE + \
                                     CONFIG_IPQ_MAX_SPI_DEVICE)

#define CONFIG_SYS_NAND_QUIET 1
/*
 * U-Boot Env Configs
 */

#define CFG_ENV_IS_IN_NAND
//#define CFG_ENV_IS_IN_SPI_FLASH
//#define CMD_SAVEENV
#define CONFIG_CMD_SAVEENV
#define CONFIG_BOARD_LATE_INIT

#if defined(CFG_ENV_IS_IN_NAND) || defined(CFG_ENV_IS_IN_SPI_FLASH)

#define CONFIG_ENV_SPI_CS               flash_chip_select
#define CONFIG_ENV_SPI_MODE             SPI_MODE_0
//#define CONFIG_ENV_SECT_SIZE            0x10000     /* 64KB */
#define CONFIG_ENV_SECT_SIZE            flash_block_size
//#define CONFIG_ENV_OFFSET               0x2a0000
#define CONFIG_ENV_OFFSET               board_env_offset
#define CONFIG_ENV_SPI_BUS              flash_index
#define CONFIG_ENV_RANGE		        board_env_range
#define CONFIG_SYS_FLASH_BASE           0x0
#define CONFIG_SYS_MAX_FLASH_BANKS      1
#define	CFG_FLASH_SIZE                  (4 * 1024 * 1024)
#define CONFIG_ENV_ADDR                 (CONFIG_ENV_OFFSET + CONFIG_SYS_FLASH_BASE)
#define CONFIG_ENV_OVERWRITE
#endif

/* NSS firmware loaded using bootm */
#define CONFIG_IPQ_FIRMWARE
#define CONFIG_BOOTCOMMAND              "bootipq"
#define CONFIG_BOOTARGS                 "console=ttyHSL1,115200n8"

#undef CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DEFAULT		(CONFIG_BOOTP_SUBNETMASK | \
					CONFIG_BOOTP_GATEWAY	 | \
					CONFIG_BOOTP_DNS)

#undef CONFIG_BOOTARGS
#undef CONFIG_BOOTDELAY
//#define CONFIG_BOOTDELAY 2
#undef CONFIG_ETHADDR
#undef CONFIG_IPADDR
#undef CONFIG_SERVERIP
#undef CONFIG_LOADADDR
#undef CONFIG_BOOTFILE
#undef CONFIG_BOOTCOMMAND
#undef CONFIG_NETMASK

#undef	CONFIG_EXTRA_ENV_SETTINGS
#define	CONFIG_EXTRA_ENV_SETTINGS			\
	"autoload=n\0"					\
	"boardname=Octomore\0"			\
	"servername=aruba-master\0"			\
	"bootcmd=boot ap\0"			        \
	"autostart=yes\0"			        \
	"bootfile=ipq806x.ari\0"		        \
	"mtdids=" MTDIDS_DEFAULT "\0"	      \
	"mtdparts=" "mtdparts=nand0:" MK_STR(AP_PRODUCTION_MTD_SIZE) "@" MK_STR(AP_PRODUCTION_IMAGE) "(" AP_PRODUCTION_IMAGE_NAME ")," \
	 MK_STR(AP_PROVISIONING_MTD_SIZE) "@" MK_STR(AP_PROVISIONING_IMAGE) "(" AP_PROVISIONING_IMAGE_NAME "),"   \
                MK_STR(AP_UBI_FS_SIZE) "@" MK_STR(AP_UBI_FS_OFFSET) "(" AP_UBI_FS_NAME ")" "\0"	      \
	""

#define CONFIG_CMD_ECHO
#define CONFIG_BOOTDELAY                2

#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS

#define CONFIG_RBTREE
#define CONFIG_UBIFS
#define CONFIG_CMD_UBI

#define CONFIG_IPQ_SNPS_GMAC
#define CONFIG_IPQ_SWITCH_ATHRS17

#define CONFIG_MII		/* MII PHY management */
#define CONFIG_PHY_GIGE

/* Add MBN header to U-Boot */
#define CONFIG_MBN_HEADER

#define CONFIG_SYS_RX_ETH_BUFFER	    8
#define CFG_RX_ETH_BUFFER 8

#ifdef CONFIG_IPQ_APPSBL_DLOAD
#define CONFIG_CMD_TFTPPUT
/* We will be uploading very big files */
#define CONFIG_NET_RETRY_COUNT          5
#endif

/* L1 cache line size is 64 bytes, L2 cache line size is 128 bytes
 * Cache flush and invalidation based on L1 cache, so the cache line
 * size is configured to 64 */
#define CONFIG_SYS_CACHELINE_SIZE  64

#define __MODEL_STR__ "AP-32x"

#define __BOARD_HAS_POWER_INFO__
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_BOOT_RETRY_TIME     120
#define CONFIG_RESET_TO_RETRY      1
#define CONFIG_HW_WATCHDOG         1
#define CONFIG_RESET_BUTTON        1
#define CFG_LATE_BOARD_INIT        1
#define CONFIG_AUTO_COMPLETE       1

#define CONFIG_FAST_MEM_COPY

/* GPIO outputs */
#define GPIO_EXT_WDOG_POKE 61
/* for LEDs: high: on, low: off */
#define GPIO_RED_STATUS_LED 30
#define GPIO_GREEN_STATUS_LED 28
#define GPIO_AMBER_STATUS_LED 29
#define GPIO_GREEN_WL_LED 31
#define GPIO_AMBER_WL_LED 32
#define GPIO_HW_RESET 27   /* Active low to force system HW reset */
#define GPIO_TPM_RST_L 26  /* Active low to reset TPM */
#define GPIO_PHY_0_RST_L 67 /* Active low to reset PHY0 */
#define GPIO_PHY_1_RST_L 68 /* Active low to reset PHY1 */
#define GPIO_PCIE_1_RST_L 3 /* Active low to reset PCIE 1 */
#define GPIO_PCIE_2_RST_L 48 /* Active low to reset PCIE 2 */

#define GPIO_POR_RST_CLK 62 /* pulse this bit low to set the warm reset status */
#define GPIO_WD_LATCH_CLR_L 52 /* pulse this bit low to reset watchdog status D flipflop */
#define GPIO_USB_ENABLE 51 /* active low to enable power to USB port */
#define GPIO_QFPROM_EN 66 /* drive low to power off VDD_QFPROM_BLOW */
#define GPIO_BLE_RST_L 2 /* active low to reset BLE */
#define GPIO_WD_DISABLE 7 /* active high to disable external watchgod timer */

/* GPIO inputs */
#define GPIO_POWER_AT 59
#define GPIO_POWER_DC 8
//#define GPIO_POE_PRES_0 6 /* active low indicating POE source port 0 is present */
#define GPIO_CONFIG_CLEAR 9
#define GPIO_POR_STS_IN 64 /* active low indicating reset caused by cold HW reset */
#define GPIO_WD_STS_IN 65 /* active high indicating reset caused by ext watchdog */
#define GPIO_USB_FLAG 63 /* active low indicating USB over current */
#define GPIO_PHY0_INT 53
#define GPIO_PHY1_INT 54

#endif /* _OCTOMORE_H */
