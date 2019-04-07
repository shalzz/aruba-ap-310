include $(ARUBA_SRC)/mk/Makefile.top

SECTIONS= HOSTAPD

ifdef HOSTAPD
APPLICATION=hostapd

DIR_WPA_SUPPLICANT=.

# define HOSTAPD_DUMP_STATE to include SIGUSR1 handler for dumping state to
# a file (undefine it, if you want to save in binary size)
#CFLAGS_LOCAL += -DHOSTAPD_DUMP_STATE

ifdef FIPS
ifndef NO_OPENSSL
INCS_LOCAL += -I$(ARUBA_SRC)/openssl-f/include
CFLAGS_LOCAL += -I$(OPENSSL_DIR)/include
LIBS_LOCAL_PATH += -L$(INSTALL_LIBS_DIR) -L${OPENSSL_DIR}/${OBJDIR}
endif
endif


# Include directories for CVS version
CFLAGS_LOCAL += -I. -I../utils -I$(ARUBA_SRC)/cmn -I$(ARUBA_SRC)/soft-ap/ASAP_mod -I$(DIR_WPA_SUPPLICANT) -I../wireless_tools.28 -I$(ARUBA_SRC)/soft-ap/madwifi-2.4 -I$(ARUBA_SRC)/soft-ap/madwifi-2.4/madwifi

# Uncomment following line and set the path to your kernel tree include
# directory if your C library does not include all header files.
# CFLAGS += -DUSE_KERNEL_HEADERS -I/usr/src/linux/include

-include .config

CONFIG_OS=unix

ifdef FIPS
#
# crypto.c is a wrapper code for libcrypto
#
CFILES =hostapd.c eloop.c ieee802_1x.c eapol_sm.c radius.c rc4.c  \
	common.c ieee802_11.c config.c ieee802_11_auth.c accounting.c \
	sta_info.c radius_client.c wpa.c aes_wrap.c ctrl_iface.c \
	driver_conf.c os_$(CONFIG_OS).c preauth.c pmksa_cache.c beacon.c \
	hw_features.c wme.c ap_list.c reconfig.c \
	mlme.c vlan_init.c ieee802_11h.c
ifndef NO_OPENSSL
CFILES += sha1.c crypto.c md5.c
CFLAGS_LOCAL += -I$(OPENSSL_DIR)/include
LIBS_LOCAL += -lcrypto
LIBS_LOCAL_PATH += -L${OPENSSL_DIR}/${OBJDIR}
else
INCS_LOCAL += -I$(CYASSL_DIR) -I$(CYASSL_DIR)/cyassl
LIBS_LOCAL += -lcyassl -laruba-crypto-util
LIBS_LOCAL_PATH += -L$(CYASSL_DIR)/src/.libs
endif
else # FIPS
CFILES =hostapd.c eloop.c ieee802_1x.c eapol_sm.c radius.c md5.c rc4.c md4.c \
	common.c ieee802_11.c config.c ieee802_11_auth.c accounting.c \
	sta_info.c radius_client.c sha1.c wpa.c aes_wrap.c ctrl_iface.c \
	driver_conf.c os_$(CONFIG_OS).c preauth.c pmksa_cache.c beacon.c \
	hw_features.c wme.c ap_list.c reconfig.c \
	mlme.c vlan_init.c ieee802_11h.c
endif

#HOBJS=hlr_auc_gw.o common.o os_$(CONFIG_OS).o milenage.o aes_wrap.o

CFLAGS_LOCAL += -DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX

ifdef CONFIG_IAPP
CFLAGS += -DCONFIG_IAPP
OBJS += iapp.o
endif

ifdef CONFIG_RSN_PREAUTH
CFLAGS += -DCONFIG_RSN_PREAUTH
CONFIG_L2_PACKET=y
endif

ifdef CONFIG_PEERKEY
CFLAGS += -DCONFIG_PEERKEY
endif

ifdef CONFIG_IEEE80211W
CFLAGS += -DCONFIG_IEEE80211W
NEED_SHA256=y
endif

ifdef CONFIG_DRIVER_HOSTAP
CFLAGS += -DCONFIG_DRIVER_HOSTAP
OBJS += driver.o
endif

ifdef CONFIG_DRIVER_WIRED
CFLAGS += -DCONFIG_DRIVER_WIRED
OBJS += driver_wired.o
endif

ifdef CONFIG_DRIVER_MADWIFI
CFLAGS_LOCAL += -DCONFIG_DRIVER_MADWIFI
CFILES += driver_madwifi.c
CONFIG_L2_PACKET=y
endif

ifdef CONFIG_DRIVER_ARUBA
CFLAGS_LOCAL += -DCONFIG_DRIVER_ARUBA
CFILES += driver_aruba.c
CONFIG_L2_PACKET=y
endif

ifdef CONFIG_DRIVER_TEST
CFLAGS += -DCONFIG_DRIVER_TEST
OBJS += driver_test.o
endif

CFILES += $(DIR_WPA_SUPPLICANT)/l2_packet_linux.c

ifdef CONFIG_EAP_MD5
CFLAGS += -DEAP_MD5
OBJS += eap_md5.o
endif

ifdef CONFIG_EAP_TLS
CFLAGS += -DEAP_TLS
OBJS += eap_tls.o
TLS_FUNCS=y
endif

ifdef CONFIG_EAP_PEAP
CFLAGS += -DEAP_PEAP
OBJS += eap_peap.o
TLS_FUNCS=y
CONFIG_EAP_TLV=y
CONFIG_EAP_MSCHAPV2=y
endif

ifdef CONFIG_EAP_TTLS
CFLAGS += -DEAP_TTLS
OBJS += eap_ttls.o
TLS_FUNCS=y
endif

ifdef CONFIG_EAP_MSCHAPV2
CFLAGS += -DEAP_MSCHAPv2
OBJS += eap_mschapv2.o
MS_FUNCS=y
endif

ifdef CONFIG_EAP_GTC
CFLAGS += -DEAP_GTC
OBJS += eap_gtc.o
endif

ifdef CONFIG_EAP_SIM
CFLAGS += -DEAP_SIM
OBJS += eap_sim.o
CONFIG_EAP_SIM_COMMON=y
endif

ifdef CONFIG_EAP_AKA
CFLAGS += -DEAP_AKA
OBJS += eap_aka.o
CONFIG_EAP_SIM_COMMON=y
endif

ifdef CONFIG_EAP_SIM_COMMON
OBJS += $(DIR_WPA_SUPPLICANT)/eap_sim_common.o
# Example EAP-SIM/AKA interface for GSM/UMTS authentication. This can be
# replaced with another file implementating the interface specified in
# eap_sim_db.h.
OBJS += eap_sim_db.o
endif

ifdef CONFIG_EAP_PAX
CFLAGS += -DEAP_PAX
OBJS += eap_pax.o $(DIR_WPA_SUPPLICANT)/eap_pax_common.o
endif

ifdef CONFIG_EAP_PSK
CFLAGS += -DEAP_PSK
OBJS += eap_psk.o $(DIR_WPA_SUPPLICANT)/eap_psk_common.o
endif

ifdef CONFIG_EAP_SAKE
CFLAGS += -DEAP_SAKE
OBJS += eap_sake.o $(DIR_WPA_SUPPLICANT)/eap_sake_common.o
endif

ifdef CONFIG_EAP_GPSK
CFLAGS += -DEAP_GPSK
OBJS += eap_gpsk.o $(DIR_WPA_SUPPLICANT)/eap_gpsk_common.o
ifdef CONFIG_EAP_GPSK_SHA256
CFLAGS += -DEAP_GPSK_SHA256
NEED_SHA256=y
endif
endif

ifdef CONFIG_EAP_VENDOR_TEST
CFLAGS += -DEAP_VENDOR_TEST
OBJS += eap_vendor_test.o
endif

ifdef CONFIG_EAP_TLV
CFLAGS += -DEAP_TLV
OBJS += eap_tlv.o
endif

ifdef CONFIG_EAP
CFLAGS += -DEAP_SERVER
OBJS += eap.o eap_methods.o eap_identity.o
endif

ifndef CONFIG_TLS
CONFIG_TLS=openssl
endif

ifdef TLS_FUNCS
# Shared TLS functions (needed for EAP_TLS, EAP_PEAP, and EAP_TTLS)
CFLAGS_LOCAL += -DEAP_TLS_FUNCS
CFILES += eap_tls_common.c
ifeq ($(CONFIG_TLS), openssl)
CFILES += $(DIR_WPA_SUPPLICANT)/tls_openssl.c
LIBS_LOCAL += -lssl -lcrypto
LIBS_p += -lcrypto
LIBS_h += -lcrypto
endif
ifeq ($(CONFIG_TLS), gnutls)
CFILES += $(DIR_WPA_SUPPLICANT)/tls_gnutls.c
LIBS_LOCAL += -lgnutls -lgcrypt -lgpg-error
LIBS_p += -lgcrypt
LIBS_h += -lgcrypt
endif
ifdef CONFIG_GNUTLS_EXTRA
CFLAGS_LOCAL += -DCONFIG_GNUTLS_EXTRA
LIBS_LOCAL += -lgnutls-extra
endif
NEED_CRYPTO=y
else
CFILES += $(DIR_WPA_SUPPLICANT)/tls_none.c
endif

ifneq ($(OEM), siemens)
LIBS_LOCAL += -lmm
endif
ifdef CONFIG_PKCS12
CFLAGS += -DPKCS12_FUNCS
endif

ifdef MS_FUNCS
OBJS += $(DIR_WPA_SUPPLICANT)/ms_funcs.o
NEED_CRYPTO=y
endif

ifdef NEED_CRYPTO
ifndef TLS_FUNCS
ifeq ($(CONFIG_TLS), openssl)
LIBS_LOCAL += -lcrypto
#LIBS_p += -lcrypto
#LIBS_h += -lcrypto
endif
ifeq ($(CONFIG_TLS), gnutls)
LIBS_LOCAL += -lgcrypt
#LIBS_p += -lgcrypt
#LIBS_h += -lgcrypt
endif
endif
ifeq ($(CONFIG_TLS), openssl)
CFILES += $(DIR_WPA_SUPPLICANT)/crypto.c
#OBJS_p += $(DIR_WPA_SUPPLICANT)/crypto.o
#HOBJS += $(DIR_WPA_SUPPLICANT)/crypto.o
CONFIG_INTERNAL_SHA256=y
endif
ifeq ($(CONFIG_TLS), gnutls)
CFILES += $(DIR_WPA_SUPPLICANT)/crypto_gnutls.c
#OBJS_p += $(DIR_WPA_SUPPLICANT)/crypto_gnutls.o
#HOBJS += $(DIR_WPA_SUPPLICANT)/crypto_gnutls.o
CONFIG_INTERNAL_SHA256=y
endif
else
CONFIG_INTERNAL_AES=y
CONFIG_INTERNAL_SHA1=y
CONFIG_INTERNAL_MD5=y
CONFIG_INTERNAL_SHA256=y
endif

ifndef FIPS
ifdef CONFIG_INTERNAL_AES
CFLAGS_LOCAL += -DINTERNAL_AES
endif
ifdef CONFIG_INTERNAL_SHA1
CFLAGS_LOCAL += -DINTERNAL_SHA1
endif
ifdef CONFIG_INTERNAL_SHA256
CFLAGS_LOCAL += -DINTERNAL_SHA256
endif
ifdef CONFIG_INTERNAL_MD5
CFLAGS_LOCAL += -DINTERNAL_MD5
endif
ifdef CONFIG_INTERNAL_MD4
CFLAGS_LOCAL += -DINTERNAL_MD4
endif
endif

ifndef FIPS
ifdef NEED_SHA256
CFILES += sha256.c
endif
endif

ifdef CONFIG_RADIUS_SERVER
CFLAGS_LOCAL += -DRADIUS_SERVER
CFILES += radius_server.c
endif

ifdef CONFIG_IPV6
CFLAGS_LOCAL += -DCONFIG_IPV6
endif

ifdef CONFIG_FULL_DYNAMIC_VLAN
# define CONFIG_FULL_DYNAMIC_VLAN to have hostapd manipulate bridges
# and vlan interfaces for the vlan feature.
CFLAGS += -DCONFIG_FULL_DYNAMIC_VLAN
endif

ifdef RGK_AUTOGEN_THIS
driver_conf.c: Makefile .config
	rm -f driver_conf.c
	echo '/* THIS FILE AUTOMATICALLY GENERATED, DO NOT EDIT! */' \
							> driver_conf.c
	echo '#include "includes.h"'			>> driver_conf.c
	echo '#include "hostapd.h"'			>> driver_conf.c
	echo '#include "driver.h"'			>> driver_conf.c
ifdef CONFIG_DRIVER_HOSTAP
	echo "void hostap_driver_register(void);"	>> driver_conf.c
endif
ifdef CONFIG_DRIVER_WIRED
	echo "void wired_driver_register(void);"	>> driver_conf.c
endif
ifdef CONFIG_DRIVER_MADWIFI
	echo "void madwifi_driver_register(void);"	>> driver_conf.c
endif
ifdef CONFIG_DRIVER_ARUBA
	echo "void aruba_driver_register(void);"	>> driver_conf.c
endif
ifdef CONFIG_DRIVER_PRISM54
	echo "void prism54_driver_register(void);"	>> driver_conf.c
endif
ifdef CONFIG_DRIVER_DEVICESCAPE
	echo "void devicescape_driver_register(void);"	>> driver_conf.c
endif
ifdef CONFIG_DRIVER_BSD
	echo "void bsd_driver_register(void);"		>> driver_conf.c
endif
ifdef CONFIG_DRIVER_TEST
	echo "void test_driver_register(void);"		>> driver_conf.c
endif
	echo 'void register_drivers(void) {'		>> driver_conf.c
ifdef CONFIG_DRIVER_HOSTAP
	echo "hostap_driver_register();"		>> driver_conf.c
endif
ifdef CONFIG_DRIVER_WIRED
	echo "wired_driver_register();"			>> driver_conf.c
endif
ifdef CONFIG_DRIVER_MADWIFI
	echo "madwifi_driver_register();"		>> driver_conf.c
endif
ifdef CONFIG_DRIVER_ARUBA
	echo "aruba_driver_register();"			>> driver_conf.c
endif
ifdef CONFIG_DRIVER_PRISM54
	echo "prism54_driver_register();"		>> driver_conf.c
endif
ifdef CONFIG_DRIVER_DEVICESCAPE
	echo "devicescape_driver_register();"		>> driver_conf.c
endif
ifdef CONFIG_DRIVER_BSD
	echo "bsd_driver_register();"			>> driver_conf.c
endif
ifdef CONFIG_DRIVER_TEST
	echo "test_driver_register();"			>> driver_conf.c
endif
	echo '}'					>> driver_conf.c

endif
endif

include $(ARUBA_SRC)/mk/Makefile.bottom
arubainstall:
	$(INSTALL) mesh_psk.conf $(DESTDIR)/bin

