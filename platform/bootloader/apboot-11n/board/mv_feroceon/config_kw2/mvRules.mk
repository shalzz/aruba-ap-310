# This flags will be used only by the Marvell arch files compilation.
include $(TOPDIR)/config.mk
include $(TOPDIR)/include/config.mk


# General definitions
CPU_ARCH    = ARM
CHIP        = kw2
VENDOR      = Marvell
ENDIAN      = LE
LD_ENDIAN   = -EL

ifeq ($(BIG_ENDIAN),y)
ENDIAN      = BE
LD_ENDIAN   = -EB
endif


# Main directory structure
SRC_PATH           = $(TOPDIR)/board/mv_feroceon
HAL_DIR            = $(SRC_PATH)/mv_hal
COMMON_DIR         = $(SRC_PATH)/common
USP_DIR            = $(SRC_PATH)/USP
SOC_DIR            = $(SRC_PATH)/mv_$(CHIP)
HAL_IF_DIR         = $(SOC_DIR)/mv_hal_if
CONFIG_DIR         = $(SOC_DIR)/config
FAM_DIR            = $(SOC_DIR)/$(CHIP)_family
SOC_ENV_DIR        = $(FAM_DIR)/ctrlEnv
SOC_SYS_DIR        = $(FAM_DIR)/ctrlEnv/sys
SOC_CPU_DIR        = $(FAM_DIR)/cpu
SOC_DEVICE_DIR     = $(FAM_DIR)/device
BOARD_ENV_DIR      = $(FAM_DIR)/boardEnv
USP_ETH_SWITCH_DIR = $(USP_DIR)/ethSwitch

# HAL components
HAL_DRAM_DIR       = $(HAL_DIR)/ddr2
HAL_SPD_DIR        = $(HAL_DIR)/ddr2/spd
HAL_ETHPHY_DIR     = $(HAL_DIR)/eth-phy
HAL_FLASH_DIR      = $(HAL_DIR)/norflash
#HAL_PCI_DIR       = $(HAL_DIR)/pci
HAL_PCIIF_DIR      = $(HAL_DIR)/pci-if
HAL_PCIIF_UTIL_DIR = $(HAL_DIR)/pci-if/pci_util
HAL_RTC_DIR        = $(HAL_DIR)/rtc/integ_rtc
HAL_SFLASH_DIR     = $(HAL_DIR)/sflash
HAL_SATA_CORE_DIR  = $(HAL_DIR)/sata/CoreDriver/
HAL_CNTMR_DIR      = $(HAL_DIR)/cntmr
HAL_GPP_DIR        = $(HAL_DIR)/gpp
HAL_IDMA_DIR       = $(HAL_DIR)/idma
HAL_PEX_DIR        = $(HAL_DIR)/pex
HAL_TWSI_DIR       = $(HAL_DIR)/twsi
HAL_TWSI_ARCH_DIR  = $(HAL_TWSI_DIR)/Arch$(CPU_ARCH)
HAL_ETH_DIR        = $(HAL_DIR)/eth
HAL_ETH_GBE_DIR    = $(HAL_ETH_DIR)/gbe
HAL_UART_DIR       = $(HAL_DIR)/uart
HAL_XOR_DIR        = $(HAL_DIR)/xor
HAL_USB_DIR        = $(HAL_DIR)/usb
HAL_SATA_DIR       = $(HAL_DIR)/sata
HAL_MFLASH_DIR     = $(HAL_DIR)/mflash
HAL_SPI_DIR        = $(HAL_DIR)/spi
HAL_TS_DIR	       = $(HAL_DIR)/ts
#HAL_AUDIO_DIR	   = $(HAL_DIR)/audio
HAL_CESA_DIR	   = $(HAL_DIR)/cesa
HAL_NFC_DIR	   = $(HAL_DIR)/nfc
HAL_PDMA_DIR	   = $(HAL_DIR)/pdma


# OS services
OSSERVICES_DIR     = $(SRC_PATH)/uboot_oss

# Internal definitions
MV_DEFINE = -DMV_UBOOT -DMV_CPU_$(ENDIAN) -DMV_$(CPU_ARCH)

# Internal include path
HAL_PATH           = -I$(HAL_DIR) -I$(HAL_SATA_CORE_DIR)
COMMON_PATH        = -I$(COMMON_DIR) 
OSSERVICES_PATH    = -I$(OSSERVICES_DIR)
USP_PATH           = -I$(USP_DIR) -I$(TOPDIR) -I$(USP_ETH_SWITCH_DIR)
SOC_PATH	   = -I$(FAM_DIR) -I$(SOC_DIR) -I$(SOC_ENV_DIR) -I$(SOC_SYS_DIR) -I$(CONFIG_DIR) -I$(SOC_CPU_DIR) -I$(SOC_DEVICE_DIR) -I$(HAL_IF_DIR)
BOARD_PATH	   = -I$(BOARD_ENV_DIR)

CFLAGS   += $(MV_DEFINE) $(OSSERVICES_PATH) $(HAL_PATH) $(COMMON_PATH) \
            $(USP_PATH) $(SOC_PATH) $(BOARD_PATH) $(SYS_PATH)

AFLAGS  += $(MV_DEFINE) $(OSSERVICES_PATH) $(HAL_PATH) $(COMMON_PATH) \
           $(USP_PATH) $(SOC_PATH) $(BOARD_PATH) $(SYS_PATH)

CPPFLAGS += $(CFLAGS)

