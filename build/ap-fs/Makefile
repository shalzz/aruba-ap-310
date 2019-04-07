include ${ARUBA_SRC}/mk/Makefile.top
include ${ARUBA_SRC}/mk/Makefile.bottom

export KERNEL_SRC

ifdef FAT_AP
ifdef ARRAN
ifndef TAMDHU
FAT_AP_USELESS_FILES = \
    $(TARGET_FILESYSTEM)/usr/sbin/pppdump \
    $(TARGET_FILESYSTEM)/usr/sbin/pppstats \
    $(TARGET_FILESYSTEM)/lib/ppp_deflate.ko \
    $(TARGET_FILESYSTEM)/lib/usbcore.ko \
    $(TARGET_FILESYSTEM)/lib/usbserial.ko \
    $(TARGET_FILESYSTEM)/lib/option.ko \
    $(TARGET_FILESYSTEM)/lib/cdc-acm.ko \
    $(TARGET_FILESYSTEM)/lib/sierra.ko \
    $(TARGET_FILESYSTEM)/lib/usb-storage.ko \
    $(TARGET_FILESYSTEM)/lib/scsi_mod.ko \
    $(TARGET_FILESYSTEM)/lib/sr_mod.ko \
    $(TARGET_FILESYSTEM)/lib/sg.ko \
    $(TARGET_FILESYSTEM)/lib/cdrom.ko \
    $(TARGET_FILESYSTEM)/lib/usbnet.ko \
    $(TARGET_FILESYSTEM)/lib/cdc_ether.ko \
    $(TARGET_FILESYSTEM)/lib/dgworks.ko \
    $(TARGET_FILESYSTEM)/lib/ptumlusbnet.ko \
    $(TARGET_FILESYSTEM)/lib/nls_base.ko \
    $(TARGET_FILESYSTEM)/lib/ehci-hcd.ko \
    $(TARGET_FILESYSTEM)/lib/nls_cp437.ko \
    $(TARGET_FILESYSTEM)/lib/nls_iso8859-1.ko \
    $(TARGET_FILESYSTEM)/lib/ntfs.ko \
    $(TARGET_FILESYSTEM)/lib/sd_mod.ko \
    $(TARGET_FILESYSTEM)/lib/vfat.ko \
    $(TARGET_FILESYSTEM)/lib/fat.ko
endif
endif
ifdef TALISKER
FAT_AP_USELESS_FILES = \
    $(TARGET_FILESYSTEM)/lib/nls_cp437.ko \
    $(TARGET_FILESYSTEM)/lib/nls_iso8859-1.ko \
    $(TARGET_FILESYSTEM)/lib/ntfs.ko \
    $(TARGET_FILESYSTEM)/lib/sd_mod.ko \
    $(TARGET_FILESYSTEM)/lib/vfat.ko \
    $(TARGET_FILESYSTEM)/lib/fat.ko
endif
ifdef SPRINGBANK
ifdef GLENFIDDICH
FAT_AP_USELESS_FILES = \
    $(TARGET_FILESYSTEM)/aruba/lib/wl.ko \
    $(TARGET_FILESYSTEM)/aruba/lib/et.ko
else
FAT_AP_USELESS_FILES = \
    $(TARGET_FILESYSTEM)/aruba/lib/wl_v5.1.ko \
    $(TARGET_FILESYSTEM)/aruba/lib/et_v5.ko \
    $(TARGET_FILESYSTEM)/aruba/bluetooth/bin/btusb.ko
endif
endif
endif

ifdef EIGER
FS_DIRS_FILE=fs-dirs_eiger
else
FS_DIRS_FILE=fs-dirs
endif

# The emake parallel build app needs dependencies to determine 
# the proper order in which targets are invoked.
#ifdef ARUBA_EMAKE
#    arubainstall_deps = $(TARGET_FILESYSTEM)/lib/modules/asap_mod.ko \
#                        $(TARGET_FILESYSTEM)/lib/modules/asap_switch.ko \
#                        $(TARGET_FILESYSTEM)/lib/bonding.ko \
#                        $(TARGET_FILESYSTEM)/lib/cdc-acm.ko \
#                        $(TARGET_FILESYSTEM)/lib/cdc_ether.ko \
#                        $(TARGET_FILESYSTEM)/lib/cdrom.ko \
#                        $(TARGET_FILESYSTEM)/lib/crc-ccitt.ko \
#                        $(TARGET_FILESYSTEM)/lib/drvlog_mod.ko \
#                        $(TARGET_FILESYSTEM)/lib/GobiNet.ko \
#                        $(TARGET_FILESYSTEM)/lib/GobiSerial.ko \
#                        $(TARGET_FILESYSTEM)/lib/hw_cdc_driver.ko \
#                        $(TARGET_FILESYSTEM)/lib/option.ko \
#                        $(TARGET_FILESYSTEM)/lib/ppp_async.ko \
#                        $(TARGET_FILESYSTEM)/lib/ppp_deflate.ko \
#                        $(TARGET_FILESYSTEM)/lib/ppp_generic.ko \
#                        $(TARGET_FILESYSTEM)/lib/pppoe.ko \
#                        $(TARGET_FILESYSTEM)/lib/pppox.ko \
#                        $(TARGET_FILESYSTEM)/lib/rndis_host.ko \
#                        $(TARGET_FILESYSTEM)/lib/sierra.ko \
#                        $(TARGET_FILESYSTEM)/lib/sierra_net.ko \
#                        $(TARGET_FILESYSTEM)/lib/sr_mod.ko \
#                        $(TARGET_FILESYSTEM)/lib/usbnet.ko \
#                        $(TARGET_FILESYSTEM)/lib/usbserial.ko

# If one of the following platforms is set
#ifneq ($(or $(ARRAN),$(TALISKER)),)
#    arubainstall_deps += $(TARGET_FILESYSTEM)/aruba/lib/dfs.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/kat.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/spectrum.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/wlan_bip.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/wlan_ccmp.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/wlan.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/wlan_scan_sta.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/wlan_tkip.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/wlan_wep.ko
#endif
#ifneq ($(or $(ARDMORE),$(ARRAN),$(TALISKER)),)
#    arubainstall_deps += $(TARGET_FILESYSTEM)/lib/nls_base.ko
#endif
#ifneq ($(or $(ARDMORE),$(ARRAN),$(SPRINGBANK),$(TALISKER)),)
#    arubainstall_deps += $(TARGET_FILESYSTEM)/lib/dgworks.ko \
#						 $(TARGET_FILESYSTEM)/lib/ehci-hcd.ko \
#						 $(TARGET_FILESYSTEM)/lib/PTUMLCsp.ko \
#						 $(TARGET_FILESYSTEM)/lib/ptumlusbnet.ko \
#						 $(TARGET_FILESYSTEM)/lib/usbcore.ko
#endif
#ifneq ($(or $(ARRAN),$(SPRINGBANK),$(TALISKER)),)
#    arubainstall_deps += $(TARGET_FILESYSTEM)/lib/fat.ko \
#						 $(TARGET_FILESYSTEM)/lib/nls_cp437.ko \
#						 $(TARGET_FILESYSTEM)/lib/nls_iso8859-1.ko \
#						 $(TARGET_FILESYSTEM)/lib/ntfs.ko \
#						 $(TARGET_FILESYSTEM)/lib/sd_mod.ko
#endif
#ifneq ($(or $(GLENMORANGIE),$(OCTOMORE),$(X4)),)
#    arubainstall_deps += $(TARGET_FILESYSTEM)/aruba/lib/adf.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/asf.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/ath_dfs.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/ath_spectral.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/umac.ko
#endif
#ifneq ($(or $(ARRAN),$(GLENMORANGIE),$(OCTOMORE),$(TALISKER),$(X4)),)
#    arubainstall_deps += $(TARGET_FILESYSTEM)/aruba/lib/ath_dev.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/ath_hal.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/ath_pktlog.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/ath_rate_atheros.ko
#endif
#ifneq ($(or $(ARDMORE),$(ARRAN),$(GLENMORANGIE),$(OCTOMORE),$(SPRINGBANK),$(TALISKER)),)
#    arubainstall_deps += \
#						 $(TARGET_FILESYSTEM)/lib/scsi_mod.ko \
#						 $(TARGET_FILESYSTEM)/lib/sg.ko \
#						 $(TARGET_FILESYSTEM)/lib/slhc.ko \
#						 $(TARGET_FILESYSTEM)/lib/usb-storage.ko
#endif
#ifneq ($(or $(ARDMORE),$(GLENMORANGIE),$(OCTOMORE),$(SPRINGBANK),$(X4),$(MASTERSON)),)
#    arubainstall_deps += $(TARGET_FILESYSTEM)/lib/usb_wwan.ko
#endif
#ifdef ARDMORE
#    arubainstall_deps += $(TARGET_FILESYSTEM)/lib/modules/p1020.ko
#endif
#ifdef ARRAN
#    arubainstall_deps += $(TARGET_FILESYSTEM)/lib/mv_cesa.ko
#endif
#ifdef GLENMORANGIE
#    arubainstall_deps += $(TARGET_FILESYSTEM)/lib/essedma.ko \
#						 $(TARGET_FILESYSTEM)/lib/qcedev.ko \
#						 $(TARGET_FILESYSTEM)/lib/qcrypto.ko \
#						 $(TARGET_FILESYSTEM)/lib/qrfs.ko
#endif
#ifdef OCTOMORE
#    arubainstall_deps += $(TARGET_FILESYSTEM)/lib/modules/qca-nss-crypto.ko \
#						 $(TARGET_FILESYSTEM)/lib/modules/qca-nss-drv.ko \
#						 $(TARGET_FILESYSTEM)/lib/modules/qca-nss-gmac.ko
#endif
#ifdef SPRINBANK
#    arubainstall_deps += $(TARGET_FILESYSTEM)/aruba/bluetooth/bin/btusb.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/et.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/et_v5.ko \
#						 $(TARGET_FILESYSTEM)/aruba/lib/wl_v5.1.ko \
#						 $(TARGET_FILESYSTEM)/lib/ohci-hcd.ko \
#						 $(TARGET_FILESYSTEM)/lib/pegasus.ko
#endif
#ifdef TALISKER
#    arubainstall_deps += $(TARGET_FILESYSTEM)/lib/ag7100_mod.ko \
#						 $(TARGET_FILESYSTEM)/lib/ag7240_mod.ko \
#						 $(TARGET_FILESYSTEM)/lib/airprime.ko \
#						 $(TARGET_FILESYSTEM)/lib/athrs_gmac.ko \
#						 $(TARGET_FILESYSTEM)/lib/hso.ko \
#						 $(TARGET_FILESYSTEM)/lib/qca955x.ko
#endif
#ifdef MASTERSON  
#    arubainstall_deps += $(TARGET_FILESYSTEM)/aruba/lib/wl.ko \
#						 $(TARGET_FILESYSTEM)/lib/wlcsm.ko \
#						 $(TARGET_FILESYSTEM)/lib/bcmvlan.ko \
#						 $(TARGET_FILESYSTEM)/lib/nciTMSkmod.ko \
#						 $(TARGET_FILESYSTEM)/lib/pwrmngtd.ko \
#						 $(TARGET_FILESYSTEM)/lib/chipinfo.ko \
#						 $(TARGET_FILESYSTEM)/lib/bcmmcast.ko \
#						 $(TARGET_FILESYSTEM)/lib/bcmspu.ko \
#						 $(TARGET_FILESYSTEM)/lib/bcmpdc.ko \
#						 $(TARGET_FILESYSTEM)/lib/bcm_enet.ko \
#						 $(TARGET_FILESYSTEM)/lib/wfd.ko \
#						 $(TARGET_FILESYSTEM)/lib/rdpa_cmd.ko \
#						 $(TARGET_FILESYSTEM)/lib/rdpa_gpl_ext.ko \
#						 $(TARGET_FILESYSTEM)/lib/rdp_fpm.ko \
#						 $(TARGET_FILESYSTEM)/lib/rdpa_mw.ko \
#						 $(TARGET_FILESYSTEM)/lib/rdpa.ko \
#						 $(TARGET_FILESYSTEM)/lib/bdmf.ko \
#						 $(TARGET_FILESYSTEM)/lib/rdpa_gpl.ko
#endif
#endif

clean savesymbols:
	@true

cleaninstall:
	@echo "  RMFS    ${TARGET_FILESYSTEM}"
	@rm -fr ${TARGET_FILESYSTEM}

all:
	@${ARUBA_SRC}/tools/bin/ap-make-fs-dirs ${ARUBA_SRC}/build/ap-fs/${FS_DIRS_FILE} ${TARGET_FILESYSTEM}

#ifdef ARUBA_EMAKE
#$(arubainstall_deps):
#	@echo "Missing .ko file: Rebuild $@"
#	${MAKE} -C $(KERNEL_SRC) modules ARCH=${MAIN_ARCH} CROSS_COMPILE=${KERNEL_TOOLS_PREFIX}
#endif

ifdef GLENMORANGIE
arubainstall_nostrip=${TARGET_FILESYSTEM}/aruba/bin/nor_sbl1_glenmorangie.mbn
arubainstall_bunker_nostrip=${TARGET_FILESYSTEM}/aruba/bin/nor_sbl1_bunker.mbn
else
arubainstall_nostrip=
endif

arubainstall: $(arubainstall_deps)
# regulatory testing purposes
ifdef MERLOT
ifdef FIPS
	find ${TARGET_FILESYSTEM} -name wlan_wep.ko -exec rm -f '{}' \;
	find ${TARGET_FILESYSTEM} -name scsi_mod.ko -exec rm -f '{}' \;
	find ${TARGET_FILESYSTEM} -name uhci-hcd.ko -exec rm -f '{}' \;
	find ${TARGET_FILESYSTEM} -name usbcore.ko -exec rm -f '{}' \;
	find ${TARGET_FILESYSTEM} -name usbserial.ko -exec rm -f '{}' \;
	find ${TARGET_FILESYSTEM} -name usb-storage.ko -exec rm -f '{}' \;
	find ${TARGET_FILESYSTEM} -name cdrom.ko -exec rm -f '{}' \;
	rm -rf ${TARGET_FILESYSTEM}/etc/httpd
	rm -rf ${TARGET_FILESYSTEM}/lib/libssl.so.0.9.8m
	rm -rf ${TARGET_FILESYSTEM}/aruba/bin/ethtool
	rm -rf ${TARGET_FILESYSTEM}/aruba/bin/usb_modeswitch
	rm -rf ${TARGET_FILESYSTEM}/etc/usb_modeswitch.conf
	rm -rf ${TARGET_FILESYSTEM}/etc/init.d/usb_setup
	rm -rf ${TARGET_FILESYSTEM}/etc/init.d/usb_restart
	rm -rf ${TARGET_FILESYSTEM}/usr/sbin/mini_httpd
	rm -rf ${TARGET_FILESYSTEM}/usr/sbin/dnsmasq245
	rm -rf ${TARGET_FILESYSTEM}/usr/sbin/dnsmasq
	rm -rf ${TARGET_FILESYSTEM}/etc/dnsmasq245_172.conf
	rm -rf ${TARGET_FILESYSTEM}/etc/dnsmasq.conf
	rm -rf ${TARGET_FILESYSTEM}/etc/dnsmasq245.conf
endif
endif

ifndef REGULATORY_TESTING
	@rm -rf ${TARGET_FILESYSTEM}/sbin/rd_help*
	@rm -rf ${TARGET_FILESYSTEM}/sbin/set_chan*
	@rm -rf ${TARGET_FILESYSTEM}/sbin/set_power*
	@rm -rf ${TARGET_FILESYSTEM}/sbin/set_beacons*
	@rm -rf ${TARGET_FILESYSTEM}/sbin/set_cont_tx_pkts*
	@rm -rf ${TARGET_FILESYSTEM}/sbin/mdk_client.out*
	@rm -rf ${TARGET_FILESYSTEM}/sbin/nart.out*
#	@rm -rf ${TARGET_FILESYSTEM}/lib/art.ko
else
	@rm -rf ${TARGET_FILESYSTEM}/aruba/bin/rapper
	@rm -rf ${TARGET_FILESYSTEM}/aruba/bin/sapd
	@rm -rf ${TARGET_FILESYSTEM}/aruba/bin/meshd
	@rm -rf ${TARGET_FILESYSTEM}/aruba/bin/stm
	@rm -rf ${TARGET_FILESYSTEM}/aruba/bin/hostapd
endif

ifdef MASTERSON
	find $(TARGET_FILESYSTEM) -type f | xargs file | grep ELF | cut -d : -f 1 | while read f; do \
		echo "  STRIP   $$f"; \
		if [[ $$f == *.ko ]]; then \
			$(KSTRIP) -R .comment -R .mdebug.abi64 $$f; \
		else \
			$(STRIP) -R .comment -R .mdebug.abi32 $$f; \
		fi \
	done
else
	find $(TARGET_FILESYSTEM) -type f | xargs file | grep ELF | cut -d : -f 1 | while read f; do \
		if [ \( -z "`echo ${arubainstall_nostrip} | grep $$f`" \) -a \( -z "`echo ${arubainstall_bunker_nostrip} | grep $$f`" \) ]; then \
			echo "  STRIP   $$f"; \
			$(STRIP) -R .comment -R .mdebug.abi32 $$f; \
		else \
			echo "  Skipping STRIP   $$f"; \
		fi \
	done
endif

ifdef FAT_AP
	@for f in $(FAT_AP_USELESS_FILES); do \
		$(RM) -rf $$f; \
	done
ifdef DALMORE
	find ${TARGET_FILESYSTEM}/etc/httpd/ -type f | grep -vE -e "(.gz|.cgi)" | xargs gzip -f
endif
endif
#ifdef __PKG_FILESYSTEM
	@${ARUBA_SRC}/tools/bin/ap-make-ramfs ${TARGET_FILESYSTEM}
#endif
