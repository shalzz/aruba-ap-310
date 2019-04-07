include mvRules.mk


# Objects list

COMMON_OBJS     = $(COMMON_DIR)/mvCommon.o

OSSERVICES_OBJS    = $(OSSERVICES_DIR)/mvOs.o $(OSSERVICES_DIR)/mvOsSata.o 

BOARD_OBJS = $(BOARD_ENV_DIR)/mvBoardEnvLib.o $(BOARD_ENV_DIR)/mvBoardEnvSpec.o

ifeq (${APBOOT},true)
BOARD_OBJS +=   ${SRC_PATH}/${MV_OUTPUT}/${MV_OUTPUT}.o
endif

##$(HAL_AUDIO_DIR)/mvAudio.o $(HAL_AUDIO_DIR)/mvAudioAddrDec.o mvDramIf.c
##$(HAL_TS_DIR)/mvTsu.o $(HAL_TS_DIR)/mvTsuAddrDec.o 
HAL_OBJS =	$(HAL_CESA_DIR)/mvCesaAddrDec.o $(HAL_CESA_DIR)/mvMD5.o \
		$(HAL_CNTMR_DIR)/mvCntmr.o \
		$(HAL_DRAM_DIR)/mvDramIf.o $(HAL_SPD_DIR)/mvSpd.o \
		$(HAL_ETH_GBE_DIR)/mvEth.o $(HAL_ETH_GBE_DIR)/mvEthAddrDec.o $(HAL_ETH_GBE_DIR)/mvEthDebug.o \
		$(HAL_ETHPHY_DIR)/mvEthPhy.o \
		$(HAL_GPP_DIR)/mvGpp.o \
		$(HAL_PEX_DIR)/mvPex.o $(HAL_PEX_DIR)/mvPexAddrDec.o \
		$(HAL_RTC_DIR)/mvRtc.o \
		$(HAL_SATA_CORE_DIR)/mvSata.o $(HAL_SATA_CORE_DIR)/mvSataAddrDec.o \
		$(HAL_SATA_CORE_DIR)/mvStorageDev.o $(HAL_SATA_CORE_DIR)/mvLog.o \
		$(HAL_SPI_DIR)/mvSpi.o $(HAL_SPI_DIR)/mvSpiCmnd.o \
		$(HAL_TWSI_DIR)/mvTwsi.o \
		$(HAL_UART_DIR)/mvUart.o \
		$(HAL_USB_DIR)/mvUsb.o $(HAL_USB_DIR)/mvUsbAddrDec.o \
		$(HAL_XOR_DIR)/mvXor.o $(HAL_XOR_DIR)/mvXorAddrDec.o \
		$(HAL_PCIIF_DIR)/mvPciIf.o \
		$(HAL_PCIIF_UTIL_DIR)/mvPciUtils.o

COBJS-$(CONFIG_MV_NFC_SUPPORT)	+= $(HAL_NFC_DIR)/mvNfc.o $(HAL_PDMA_DIR)/mvPdma.o

HAL_IF_OBJS = $(HAL_IF_DIR)/mvSysCesa.o $(HAL_IF_DIR)/mvSysDdr.o \
		$(HAL_IF_DIR)/mvSysEth.o \
		$(HAL_IF_DIR)/mvSysEthPhy.o $(HAL_IF_DIR)/mvSysPex.o $(HAL_IF_DIR)/mvSysSata.o \
		$(HAL_IF_DIR)/mvSysSFlash.o $(HAL_IF_DIR)/mvSysSpi.o $(HAL_IF_DIR)/mvSysTdm.o \
		$(HAL_IF_DIR)/mvSysUsb.o $(HAL_IF_DIR)/mvSysXor.o


SOC_OBJS = $(SOC_DIR)/mv_cmd.o $(SOC_DIR)/mv_dram.o $(SOC_DIR)/mv_main.o $(SOC_DIR)/mv_phy.o \
		$(SOC_DIR)/cmd_sar.o $(SOC_ENV_DIR)/mvCtrlEnvLib.o \
		$(SOC_ENV_DIR)/mvCtrlEnvAddrDec.o $(SOC_ENV_DIR)/mvCtrlEthCompLib.o \
		$(SOC_SYS_DIR)/mvAhbToMbus.o $(SOC_SYS_DIR)/mvCpuIf.o \
		$(SOC_CPU_DIR)/mvCpu.o $(SOC_DEVICE_DIR)/mvDevice.o \
		${SOC_DIR}/mv_watchdog.o

USP_OBJS = $(USP_DIR)/cmd_rcvr.o $(USP_DIR)/mv_flash.o $(USP_DIR)/mv_serial.o $(USP_DIR)/mv_spi.o \
		$(USP_DIR)/mv_pageTable.o $(USP_DIR)/mv_rtc.o $(USP_DIR)/mv_mon_init.o \
		$(USP_DIR)/mv_pci.o $(USP_DIR)/mv_mmc.o \
		$(USP_DIR)/mv_ide.o $(USP_DIR)/mv_egiga.o $(USP_DIR)/mv_usb.o \
		$(USP_DIR)/mv_fs.o $(USP_DIR)/mv_loadnet.o $(USP_DIR)/mv_protectionUnit.o \
		$(USP_DIR)/mv_i2c.o $(USP_DIR)/mv_ext2_boot.o \
		$(USP_ETH_SWITCH_DIR)/mvSwitch.o

COBJS-$(CONFIG_MV_NFC_SUPPORT)	+= $(USP_DIR)/orion_nfc_hal.o
COBJS-$(CONFIG_MV_LEGACY_NAND)	+= $(USP_DIR)/mv_nand.o

#ifeq ($(NAND_BOOT), y)
#USP_OBJS        += $(USP_DIR)/nBootloader.o
#endif

SOBJS = $(USP_DIR)/jump.o \
		$(SOC_DIR)/platform.o \
		$(HAL_DRAM_DIR)/mvDramIfBasicInit.o \
		$(HAL_DRAM_DIR)/mvDramIfConfig.o \
		$(SOC_SYS_DIR)/mvCpuIfInit.o \
		$(HAL_TWSI_DIR)/mvTwsiEeprom.o \
		$(SOC_SYS_DIR)/mvCpuIfInit.o

ifeq ($(NAND_BOOT), y)
SOBJS        += ../mv_kw2/nBootstrap.o
endif

COBJS	:= $(COBJS-y)
#OBJS	:= $(addprefix $(obj),$(COBJS))

OBJS = $(COMMON_OBJS) $(OSSERVICES_OBJS) $(HAL_OBJS) $(HAL_IF_OBJS) $(SOC_OBJS) $(BOARD_OBJS) $(USP_OBJS) $(COBJS)

LIB             = lib$(BOARD).a

all:    $(LIB)

$(LIB): .depend $(SOBJS) $(OBJS)
	@echo "  LIB     $@"
	${Q}$(AR) ${ARFLAGS} $@ $(SOBJS) $(OBJS)

#########################################################################

.depend:        Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c) $(USB_OBJS:.o=.c)
	${Q}$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) $(USB_OBJS:.o=.c) > $@

sinclude .depend

#########################################################################

