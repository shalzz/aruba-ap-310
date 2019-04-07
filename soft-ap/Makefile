include $(ARUBA_SRC)/mk/Makefile.top

# Merlot definition overrides standard AP definition
SUPPLICANT_${AP_PLATFORM} = wpa_supplicant-0.6.10/wpa_supplicant
SUPPLICANT_${MERLOT} = wpa_supplicant-0.5.7

ifdef FAT_AP
SUPPLICANT_${TALISKER} = wpa_supplicant-0.5.7
SUPPLICANT_${DALMORE} = wpa_supplicant-0.5.7 wpa_supplicant-0.6.10/wpa_supplicant
SUPPLICANT_${TAMDHU} = wpa_supplicant-0.5.7 wpa_supplicant-0.6.10/wpa_supplicant
endif

WIRELESS_TOOLS_DIR = wireless_tools.29

DIRS_${AP_PLATFORM} = ${MWF} ntpclient \
	${WIRELESS_TOOLS_DIR} rp-pppoe-3.5/src ppp-2.4.1 mtd-utils-2.0.0
DIRS_${AP_PLATFORM} += ${SUPPLICANT_yes} hostapd-0.5.7

DIRS_${BRCM_AP}  += tools
DIRS_${EIGER}   += eiger

EXCLUDE_DIRS_${QCA_AP} = ${MWF}
EXCLUDE_DIRS_${BRCM_AP} = ${MWF}

include $(ARUBA_SRC)/mk/Makefile.bottom
