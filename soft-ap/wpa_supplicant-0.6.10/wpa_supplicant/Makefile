include $(ARUBA_SRC)/mk/Makefile.top

SECTIONS= SUPPLICANT

ifdef SUPPLICANT
APPLICATION=wpa_supplicant

VPATH = ../src/utils ../src/crypto ../src/drivers ../src/l2_packet ../src/eap_peer ../src/eap_common ../src/eap_peer ../src/eapol_supp ../src/tls ../src/rsn_supp ../src/common

CFLAGS_LOCAL += -I../src
CFLAGS_LOCAL += -I../src/crypto
CFLAGS_LOCAL += -I../src/utils
CFLAGS_LOCAL += -I../src/common
CFLAGS_LOCAL += -I../src/rsn_supp
CFLAGS_LOCAL += -I../src/drivers
CFLAGS_LOCAL += -I../src/eap_common
CFLAGS_LOCAL += -I../src/eapol_supp
CFLAGS_LOCAL += -I../src/l2_packet
CFLAGS_LOCAL += -I../src/tls
CFLAGS_LOCAL += -I../src/eap_peer
CFLAGS_LOCAL += -I../../../sap/nsapd
CFLAGS_LOCAL += -I$(ARUBA_SRC)/cmn

ifneq ($(MASTERSON)$(LAGAVULIN),)
BRCMDRVSRC=$(ARUBA_SRC)/soft-ap/broadcom/esdk5.0/main/src
else
BRCMDRVSRC=$(ARUBA_SRC)/soft-ap/broadcom/src
endif
CFLAGS_LOCAL += -I${BRCMDRVSRC}/../components/shared \
				-I${BRCMDRVSRC}/common/include \
				-I${BRCMDRVSRC}/include \
				-I${BRCMDRVSRC}/shared/bcmwifi/include

CFILES = config.c
CFILES += common.c
CFILES += wpa_debug.c
CFILES += wpabuf.c
CFILES += md5.c
CFILES += rc4.c
CFILES += md4.c
CFILES += sha1.c
CFILES += des.c

-include .config

ifdef FIPS
ifndef FAT_AP
CFLAGS_LOCAL += -I$(OPENSSL_DIR)/include
endif
ifndef NO_OPENSSL 
LIBS_LOCAL += -lcrypto                                                                                                      
LIBS_LOCAL_PATH += -L${OPENSSL_DIR}/${OBJDIR}                                                                               
endif
endif        

ifdef NO_OPENSSL
INCS_LOCAL += -I$(CYASSL_DIR) -I$(CYASSL_DIR)/cyassl
LIBS_LOCAL += -lcyassl
LIBS_LOCAL_PATH += -L$(CYASSL_DIR)/.src/libs
endif

CONFIG_OS=unix

CFILES += os_$(CONFIG_OS).c

ifndef CONFIG_ELOOP
CONFIG_ELOOP=eloop
endif
CFILES += $(CONFIG_ELOOP).c


ifdef CONFIG_EAPOL_TEST
CFLAGS_LOCAL += -Werror -DEAPOL_TEST
endif

ifndef CONFIG_BACKEND
CONFIG_BACKEND=file
endif

ifeq ($(CONFIG_BACKEND), file)
CFILES += config_file.c
ifndef CONFIG_NO_CONFIG_BLOBS
NEED_BASE64=y
endif
CFLAGS_LOCAL += -DCONFIG_BACKEND_FILE
endif

ifeq ($(CONFIG_BACKEND), none)
OBJS += config_none.o
endif

ifdef CONFIG_NO_CONFIG_WRITE
CFLAGS_LOCAL += -DCONFIG_NO_CONFIG_WRITE
endif

ifdef CONFIG_NO_CONFIG_BLOBS
CFLAGS_LOCAL += -DCONFIG_NO_CONFIG_BLOBS
endif

ifdef CONFIG_NO_SCAN_PROCESSING
CFLAGS_LOCAL += -DCONFIG_NO_SCAN_PROCESSING
endif

ifdef CONFIG_DRIVER_HOSTAP
CFLAGS_LOCAL += -DCONFIG_DRIVER_HOSTAP
CFILES += driver_hostap.c
CONFIG_WIRELESS_EXTENSION=y
endif

ifdef CONFIG_DRIVER_WEXT
CFLAGS_LOCAL += -DCONFIG_DRIVER_WEXT
CONFIG_WIRELESS_EXTENSION=y
endif

ifdef ARDMORE
CFLAGS_LOCAL += -DCONFIG_DRIVER_BROADCOM
CFILES += driver_broadcom.c
$(warning the CFILES is $(CFILES))
CONFIG_WIRELESS_EXTENSION=y
else
ifdef SPRINGBANK
CFLAGS_LOCAL += -DCONFIG_DRIVER_BROADCOM
CFILES += driver_broadcom.c
$(warning the CFILES is $(CFILES))
CONFIG_WIRELESS_EXTENSION=y
else
ifdef MASTERSON 
CFLAGS_LOCAL += -DCONFIG_DRIVER_BROADCOM
CFILES += driver_broadcom.c
CONFIG_WIRELESS_EXTENSION=y
else
ifdef LAGAVULIN 
CFLAGS_LOCAL += -DCONFIG_DRIVER_BROADCOM
CFILES += driver_broadcom.c
CONFIG_WIRELESS_EXTENSION=y
else
ifdef CONFIG_DRIVER_MADWIFI
CFLAGS_LOCAL += -DCONFIG_DRIVER_MADWIFI
CFILES += driver_madwifi.c
CONFIG_WIRELESS_EXTENSION=y
endif
endif
endif
endif
endif

ifndef CONFIG_L2_PACKET
CONFIG_L2_PACKET=linux
endif

ifdef CONFIG_DRIVER_WIRED
CFLAGS_LOCAL += -DCONFIG_DRIVER_WIRED
CFILES += driver_wired.c
endif

ifdef CONFIG_DRIVER_TEST
CFLAGS_LOCAL += -DCONFIG_DRIVER_TEST
CFILES += driver_test.c
endif

ifndef CONFIG_L2_PACKET
CONFIG_L2_PACKET=linux
endif

CFILES += l2_packet_$(CONFIG_L2_PACKET).c

ifdef CONFIG_EAP_TLS
# EAP-TLS
CFLAGS_LOCAL += -DEAP_TLS
CFILES += eap_tls.c
TLS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_PEAP
# EAP-PEAP
ifeq ($(CONFIG_EAP_PEAP), dyn)
CFLAGS_LOCAL += -DEAP_PEAP_DYNAMIC
EAPDYN += ../src/eap_peer/eap_peap.so
else
CFLAGS_LOCAL += -DEAP_PEAP
CFILES += eap_peap.c
CFILES += eap_peap_common.c
endif
TLS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_TTLS
# EAP-TTLS
ifeq ($(CONFIG_EAP_TTLS), dyn)
CFLAGS_LOCAL += -DEAP_TTLS_DYNAMIC
EAPDYN += ../src/eap_peer/eap_ttls.so
else
CFLAGS_LOCAL += -DEAP_TTLS
CFILES += eap_ttls.c
endif
MS_FUNCS=y
TLS_FUNCS=y
CHAP=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_MD5
# EAP-MD5
ifeq ($(CONFIG_EAP_MD5), dyn)
CFLAGS += -DEAP_MD5_DYNAMIC
EAPDYN += ../src/eap_peer/eap_md5.so
else
CFLAGS_LOCAL += -DEAP_MD5
CFILES += eap_md5.c
endif
CHAP=y
CONFIG_IEEE8021X_EAPOL=y
endif

# backwards compatibility for old spelling
ifdef CONFIG_MSCHAPV2
ifndef CONFIG_EAP_MSCHAPV2
CONFIG_EAP_MSCHAPV2=y
endif
endif

ifdef CONFIG_EAP_MSCHAPV2
# EAP-MSCHAPv2
ifeq ($(CONFIG_EAP_MSCHAPV2), dyn)
CFLAGS += -DEAP_MSCHAPv2_DYNAMIC
EAPDYN += ../src/eap_peer/eap_mschapv2.so
EAPDYN += ../src/eap_peer/mschapv2.so
else
CFLAGS_LOCAL += -DEAP_MSCHAPv2
CFILES += eap_mschapv2.c
CFILES += mschapv2.c
endif
MS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_GTC
# EAP-GTC
ifeq ($(CONFIG_EAP_GTC), dyn)
CFLAGS += -DEAP_GTC_DYNAMIC
EAPDYN += ../src/eap_peer/eap_gtc.so
else
CFLAGS += -DEAP_GTC
OBJS += ../src/eap_peer/eap_gtc.o
OBJS_h += ../src/eap_server/eap_gtc.o
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_OTP
# EAP-OTP
ifeq ($(CONFIG_EAP_OTP), dyn)
CFLAGS += -DEAP_OTP_DYNAMIC
EAPDYN += ../src/eap_peer/eap_otp.so
else
CFLAGS += -DEAP_OTP
OBJS += ../src/eap_peer/eap_otp.o
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_SIM
# EAP-SIM
ifeq ($(CONFIG_EAP_SIM), dyn)
CFLAGS += -DEAP_SIM_DYNAMIC
EAPDYN += ../src/eap_peer/eap_sim.so
else
CFLAGS += -DEAP_SIM
OBJS += ../src/eap_peer/eap_sim.o
OBJS_h += ../src/eap_server/eap_sim.o
endif
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_SIM_COMMON=y
endif

ifdef CONFIG_EAP_LEAP
# EAP-LEAP
ifeq ($(CONFIG_EAP_LEAP), dyn)
CFLAGS += -DEAP_LEAP_DYNAMIC
EAPDYN += ../src/eap_peer/eap_leap.so
else
CFLAGS += -DEAP_LEAP
OBJS += ../src/eap_peer/eap_leap.o
endif
MS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_PSK
# EAP-PSK
ifeq ($(CONFIG_EAP_PSK), dyn)
CFLAGS += -DEAP_PSK_DYNAMIC
EAPDYN += ../src/eap_peer/eap_psk.so
else
CFLAGS += -DEAP_PSK
OBJS += ../src/eap_peer/eap_psk.o ../src/eap_common/eap_psk_common.o
OBJS_h += ../src/eap_server/eap_psk.o
endif
CONFIG_IEEE8021X_EAPOL=y
NEED_AES=y
endif

ifdef CONFIG_EAP_AKA
# EAP-AKA
ifeq ($(CONFIG_EAP_AKA), dyn)
CFLAGS += -DEAP_AKA_DYNAMIC
EAPDYN += ../src/eap_peer/eap_aka.so
else
CFLAGS += -DEAP_AKA
OBJS += ../src/eap_peer/eap_aka.o
OBJS_h += ../src/eap_server/eap_aka.o
endif
CONFIG_IEEE8021X_EAPOL=y
CONFIG_EAP_SIM_COMMON=y
endif

ifdef CONFIG_EAP_AKA_PRIME
# EAP-AKA'
ifeq ($(CONFIG_EAP_AKA_PRIME), dyn)
CFLAGS += -DEAP_AKA_PRIME_DYNAMIC
else
CFLAGS += -DEAP_AKA_PRIME
endif
NEED_SHA256=y
endif

ifdef CONFIG_EAP_SIM_COMMON
OBJS += ../src/eap_common/eap_sim_common.o
OBJS_h += ../src/eap_server/eap_sim_db.o
NEED_AES=y
NEED_FIPS186_2_PRF=y
endif

ifdef CONFIG_EAP_FAST
# EAP-FAST
ifeq ($(CONFIG_EAP_FAST), dyn)
CFLAGS += -DEAP_FAST_DYNAMIC
EAPDYN += ../src/eap_peer/eap_fast.so
EAPDYN += ../src/eap_common/eap_fast_common.o
else
CFLAGS += -DEAP_FAST
OBJS += ../src/eap_peer/eap_fast.o ../src/eap_peer/eap_fast_pac.o
OBJS += ../src/eap_common/eap_fast_common.o
OBJS_h += ../src/eap_server/eap_fast.o
endif
TLS_FUNCS=y
CONFIG_IEEE8021X_EAPOL=y
NEED_T_PRF=y
endif

ifdef CONFIG_EAP_PAX
# EAP-PAX
ifeq ($(CONFIG_EAP_PAX), dyn)
CFLAGS += -DEAP_PAX_DYNAMIC
EAPDYN += ../src/eap_peer/eap_pax.so
else
CFLAGS += -DEAP_PAX
OBJS += ../src/eap_peer/eap_pax.o ../src/eap_common/eap_pax_common.o
OBJS_h += ../src/eap_server/eap_pax.o
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_SAKE
# EAP-SAKE
ifeq ($(CONFIG_EAP_SAKE), dyn)
CFLAGS += -DEAP_SAKE_DYNAMIC
EAPDYN += ../src/eap_peer/eap_sake.so
else
CFLAGS += -DEAP_SAKE
OBJS += ../src/eap_peer/eap_sake.o ../src/eap_common/eap_sake_common.o
OBJS_h += ../src/eap_server/eap_sake.o
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_GPSK
# EAP-GPSK
ifeq ($(CONFIG_EAP_GPSK), dyn)
CFLAGS += -DEAP_GPSK_DYNAMIC
EAPDYN += ../src/eap_peer/eap_gpsk.so
else
CFLAGS += -DEAP_GPSK
OBJS += ../src/eap_peer/eap_gpsk.o ../src/eap_common/eap_gpsk_common.o
OBJS_h += ../src/eap_server/eap_gpsk.o
endif
CONFIG_IEEE8021X_EAPOL=y
ifdef CONFIG_EAP_GPSK_SHA256
CFLAGS += -DEAP_GPSK_SHA256
endif
NEED_SHA256=y
endif

ifdef CONFIG_WPS
# EAP-WSC
CFLAGS += -DCONFIG_WPS -DEAP_WSC
OBJS += wps_supplicant.o
OBJS += ../src/utils/uuid.o
OBJS += ../src/eap_peer/eap_wsc.o ../src/eap_common/eap_wsc_common.o
OBJS += ../src/wps/wps.o
OBJS += ../src/wps/wps_common.o
OBJS += ../src/wps/wps_attr_parse.o
OBJS += ../src/wps/wps_attr_build.o
OBJS += ../src/wps/wps_attr_process.o
OBJS += ../src/wps/wps_dev_attr.o
OBJS += ../src/wps/wps_enrollee.o
OBJS += ../src/wps/wps_registrar.o
OBJS_h += ../src/eap_server/eap_wsc.o
CONFIG_IEEE8021X_EAPOL=y
NEED_DH_GROUPS=y
NEED_SHA256=y
NEED_BASE64=y
NEED_CRYPTO=y
NEED_80211_COMMON=y

ifdef CONFIG_WPS_UPNP
CFLAGS += -DCONFIG_WPS_UPNP
OBJS += ../src/wps/wps_upnp.o
OBJS += ../src/wps/wps_upnp_ssdp.o
OBJS += ../src/wps/wps_upnp_web.o
OBJS += ../src/wps/wps_upnp_event.o
OBJS += ../src/wps/httpread.o
endif

endif

ifdef CONFIG_EAP_IKEV2
# EAP-IKEv2
ifeq ($(CONFIG_EAP_IKEV2), dyn)
CFLAGS += -DEAP_IKEV2_DYNAMIC
EAPDYN += ../src/eap_peer/eap_ikev2.so ../src/eap_peer/ikev2.o
EAPDYN += ../src/eap_common/eap_ikev2_common.o ../src/eap_common/ikev2_common.o
else
CFLAGS += -DEAP_IKEV2
OBJS += ../src/eap_peer/eap_ikev2.o ../src/eap_peer/ikev2.o
OBJS += ../src/eap_common/eap_ikev2_common.o ../src/eap_common/ikev2_common.o
OBJS_h += ../src/eap_server/eap_ikev2.o
OBJS_h += ../src/eap_server/ikev2.o
endif
CONFIG_IEEE8021X_EAPOL=y
NEED_DH_GROUPS=y
NEED_DH_GROUPS_ALL=y
endif

ifdef CONFIG_EAP_VENDOR_TEST
ifeq ($(CONFIG_EAP_VENDOR_TEST), dyn)
CFLAGS += -DEAP_VENDOR_TEST_DYNAMIC
EAPDYN += ../src/eap_peer/eap_vendor_test.so
else
CFLAGS += -DEAP_VENDOR_TEST
OBJS += ../src/eap_peer/eap_vendor_test.o
OBJS_h += ../src/eap_server/eap_vendor_test.o
endif
CONFIG_IEEE8021X_EAPOL=y
endif

ifdef CONFIG_EAP_TNC
# EAP-TNC
CFLAGS += -DEAP_TNC
OBJS += ../src/eap_peer/eap_tnc.o
OBJS += ../src/eap_peer/tncc.o
NEED_BASE64=y
ifndef CONFIG_NATIVE_WINDOWS
ifndef CONFIG_DRIVER_BSD
LIBS += -ldl
endif
endif
endif

ifdef CONFIG_IEEE8021X_EAPOL
# IEEE 802.1X/EAPOL state machines (e.g., for RADIUS authentication)
CFLAGS_LOCAL += -DIEEE8021X_EAPOL
CFILES += eapol_supp_sm.c eap.c eap_common.c eap_methods.c
ifdef CONFIG_DYNAMIC_EAP_METHODS
CFLAGS_LOCAL += -DCONFIG_DYNAMIC_EAP_METHODS
LIBS_LOCAL += -ldl -rdynamic
endif
endif

ifdef CONFIG_EAP_SERVER
CFLAGS += -DEAP_SERVER
OBJS_h += ../src/eap_server/eap.o
OBJS_h += ../src/eap_server/eap_identity.o
OBJS_h += ../src/eap_server/eap_methods.o
endif

ifdef CONFIG_RADIUS_CLIENT
OBJS_h += ../src/utils/ip_addr.o
OBJS_h += ../src/radius/radius.o
OBJS_h += ../src/radius/radius_client.o
endif

ifdef CONFIG_AUTHENTICATOR
OBJS_h += ../hostapd/eapol_sm.o
OBJS_h += ../hostapd/ieee802_1x.o
endif

ifdef CONFIG_WPA_AUTHENTICATOR
OBJS_h += ../hostapd/wpa.o
OBJS_h += ../hostapd/wpa_auth_ie.o
ifdef CONFIG_IEEE80211R
OBJS_h += ../hostapd/wpa_ft.o
endif
ifdef CONFIG_PEERKEY
OBJS_h += ../hostapd/peerkey.o
endif
endif

ifdef CONFIG_PCSC
# PC/SC interface for smartcards (USIM, GSM SIM)
CFLAGS += -DPCSC_FUNCS -I/usr/include/PCSC
OBJS += ../src/utils/pcsc_funcs.o
# -lpthread may not be needed depending on how pcsc-lite was configured
ifdef CONFIG_NATIVE_WINDOWS
#Once MinGW gets support for WinScard, -lwinscard could be used instead of the
#dynamic symbol loading that is now used in pcsc_funcs.c
#LIBS += -lwinscard
else
LIBS += -lpcsclite -lpthread
endif
endif

ifdef CONFIG_SIM_SIMULATOR
CFLAGS += -DCONFIG_SIM_SIMULATOR
NEED_MILENAGE=y
endif

ifdef CONFIG_USIM_SIMULATOR
CFLAGS += -DCONFIG_USIM_SIMULATOR
NEED_MILENAGE=y
endif

ifdef NEED_MILENAGE
OBJS += ../src/hlr_auc_gw/milenage.o
endif

ifndef CONFIG_TLS
CONFIG_TLS=openssl
endif

ifeq ($(CONFIG_TLS), internal)
ifndef CONFIG_CRYPTO
CONFIG_CRYPTO=internal
endif
endif
ifeq ($(CONFIG_CRYPTO), libtomcrypt)
CFLAGS_LOCAL += -DCONFIG_INTERNAL_X509
endif
ifeq ($(CONFIG_CRYPTO), internal)
CFLAGS_LOCAL += -DCONFIG_INTERNAL_X509
endif


ifdef TLS_FUNCS
# Shared TLS functions (needed for EAP_TLS, EAP_PEAP, EAP_TTLS, and EAP_FAST)
CFLAGS_LOCAL += -DEAP_TLS_FUNCS
CFILES += eap_tls_common.c
NEED_TLS_PRF=y
ifeq ($(CONFIG_TLS), openssl)
CFLAGS_LOCAL += -DEAP_TLS_OPENSSL
CFILES += tls_openssl.c
LIBS_LOCAL += -lssl -lcrypto
endif
ifeq ($(CONFIG_TLS), gnutls)
OBJS += ../src/crypto/tls_gnutls.o
LIBS += -lgnutls -lgcrypt -lgpg-error
LIBS_p += -lgcrypt
ifdef CONFIG_GNUTLS_EXTRA
CFLAGS += -DCONFIG_GNUTLS_EXTRA
LIBS += -lgnutls-extra
endif
endif
ifeq ($(CONFIG_TLS), schannel)
OBJS += ../src/crypto/tls_schannel.o
endif
ifeq ($(CONFIG_TLS), internal)
CFILES += tls_internal.c
CFILES += tlsv1_common.c tlsv1_record.c
CFILES += tlsv1_cred.c tlsv1_client.c
CFILES += tlsv1_client_write.c tlsv1_client_read.c
CFILES += asn1.c rsa.c x509v3.c
NEED_BASE64=y
NEED_TLS_PRF=y
CFLAGS_LOCAL += -DCONFIG_TLS_INTERNAL
CFLAGS_LOCAL += -DCONFIG_TLS_INTERNAL_CLIENT
ifeq ($(CONFIG_CRYPTO), internal)
endif
ifeq ($(CONFIG_CRYPTO), libtomcrypt)
LIBS_LOCAL += -ltomcrypt -ltfm
endif
endif
ifeq ($(CONFIG_TLS), none)
CFILES += tls_none.c
CFLAGS_LOCAL += -DEAP_TLS_NONE
CONFIG_INTERNAL_AES=y
CONFIG_INTERNAL_SHA1=y
CONFIG_INTERNAL_MD5=y
CONFIG_INTERNAL_SHA256=y
endif
ifdef CONFIG_SMARTCARD
ifndef CONFIG_NATIVE_WINDOWS
ifneq ($(CONFIG_L2_PACKET), freebsd)
LIBS += -ldl
endif
endif
endif
NEED_CRYPTO=y
else
CFILES += tls_none.c
endif

ifdef CONFIG_PKCS12
CFLAGS_LOCAL += -DPKCS12_FUNCS
endif

ifdef CONFIG_SMARTCARD
CFLAGS_LOCAL += -DCONFIG_SMARTCARD
endif

ifdef MS_FUNCS
CFILES += ms_funcs.c
NEED_CRYPTO=y
endif

ifdef CHAP
CFILES += chap.c
endif

ifdef NEED_CRYPTO
ifndef TLS_FUNCS
ifeq ($(CONFIG_TLS), openssl)
LIBS_LOCAL += -lcrypto
endif
ifeq ($(CONFIG_TLS), gnutls)
LIBS_LOCAL += -lgcrypt
endif
ifeq ($(CONFIG_TLS), schannel)
endif
ifeq ($(CONFIG_TLS), internal)
ifeq ($(CONFIG_CRYPTO), libtomcrypt)
LIBS_LOCAL += -ltomcrypt -ltfm
endif
endif
endif
ifeq ($(CONFIG_TLS), openssl)
OBJS += ../src/crypto/crypto_openssl.o
OBJS_p += ../src/crypto/crypto_openssl.o
CONFIG_INTERNAL_SHA256=y
endif
ifeq ($(CONFIG_TLS), gnutls)
OBJS += ../src/crypto/crypto_gnutls.o
OBJS_p += ../src/crypto/crypto_gnutls.o
CONFIG_INTERNAL_SHA256=y
endif
ifeq ($(CONFIG_TLS), schannel)
OBJS += ../src/crypto/crypto_cryptoapi.o
OBJS_p += ../src/crypto/crypto_cryptoapi.o
CONFIG_INTERNAL_SHA256=y
endif
ifeq ($(CONFIG_TLS), internal)
ifeq ($(CONFIG_CRYPTO), libtomcrypt)
OBJS += ../src/crypto/crypto_libtomcrypt.o
OBJS_p += ../src/crypto/crypto_libtomcrypt.o
CONFIG_INTERNAL_SHA256=y
endif
ifeq ($(CONFIG_CRYPTO), internal)
CFILES += crypto_internal.c bignum.c
CFLAGS_LOCAL += -DCONFIG_CRYPTO_INTERNAL
ifdef CONFIG_INTERNAL_LIBTOMMATH
CFLAGS_LOCAL += -DCONFIG_INTERNAL_LIBTOMMATH
ifdef CONFIG_INTERNAL_LIBTOMMATH_FAST
CFLAGS_LOCAL += -DLTM_FAST
endif
else
LIBS_LOCAL += -ltommath
endif
CONFIG_INTERNAL_AES=y
CONFIG_INTERNAL_DES=y
CONFIG_INTERNAL_SHA1=y
CONFIG_INTERNAL_MD4=y
CONFIG_INTERNAL_MD5=y
CONFIG_INTERNAL_SHA256=y
endif
ifeq ($(CONFIG_CRYPTO), cryptoapi)
OBJS += ../src/crypto/crypto_cryptoapi.o
OBJS_p += ../src/crypto/crypto_cryptoapi.o
CFLAGS += -DCONFIG_CRYPTO_CRYPTOAPI
CONFIG_INTERNAL_SHA256=y
endif
endif
ifeq ($(CONFIG_TLS), none)
CFILES += crypto_none.c
CONFIG_INTERNAL_SHA256=y
endif
else
CONFIG_INTERNAL_AES=y
CONFIG_INTERNAL_SHA1=y
CONFIG_INTERNAL_MD5=y
endif

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
ifdef CONFIG_INTERNAL_DES
CFLAGS_LOCAL += -DINTERNAL_DES
endif

ifdef CONFIG_IEEE80211R
NEED_SHA256=y
endif

ifdef CONFIG_IEEE80211W
CFLAGS += -DCONFIG_IEEE80211W
NEED_SHA256=y
endif

ifdef NEED_SHA256
CFLAGS += sha256.c
CFLAGS_LOCAL += -DNEED_SHA256
endif

ifdef CONFIG_WIRELESS_EXTENSION
CFILES += driver_wext.c
endif

ifdef CONFIG_CTRL_IFACE
ifeq ($(CONFIG_CTRL_IFACE), y)
ifdef CONFIG_NATIVE_WINDOWS
CONFIG_CTRL_IFACE=named_pipe
else
CONFIG_CTRL_IFACE=unix
endif
endif
CFLAGS_LOCAL += -DCONFIG_CTRL_IFACE
ifeq ($(CONFIG_CTRL_IFACE), unix)
CFLAGS_LOCAL += -DCONFIG_CTRL_IFACE_UNIX
endif
ifeq ($(CONFIG_CTRL_IFACE), udp)
CFLAGS += -DCONFIG_CTRL_IFACE_UDP
endif
ifeq ($(CONFIG_CTRL_IFACE), named_pipe)
CFLAGS += -DCONFIG_CTRL_IFACE_NAMED_PIPE
endif
CFILES += ctrl_iface.c ctrl_iface_$(CONFIG_CTRL_IFACE).c
endif

ifdef CONFIG_CTRL_IFACE_DBUS
CFLAGS += -DCONFIG_CTRL_IFACE_DBUS -DDBUS_API_SUBJECT_TO_CHANGE
OBJS += ctrl_iface_dbus.o ctrl_iface_dbus_handlers.o dbus_dict_helpers.o
ifndef DBUS_LIBS
DBUS_LIBS := $(shell pkg-config --libs dbus-1)
endif
LIBS += $(DBUS_LIBS)
ifndef DBUS_INCLUDE
DBUS_INCLUDE := $(shell pkg-config --cflags dbus-1)
endif
dbus_version=$(subst ., ,$(shell pkg-config --modversion dbus-1))
DBUS_VERSION_MAJOR=$(word 1,$(dbus_version))
DBUS_VERSION_MINOR=$(word 2,$(dbus_version))
ifeq ($(DBUS_VERSION_MAJOR),)
DBUS_VERSION_MAJOR=0
endif
ifeq ($(DBUS_VERSION_MINOR),)
DBUS_VERSION_MINOR=0
endif
DBUS_INCLUDE += -DDBUS_VERSION_MAJOR=$(DBUS_VERSION_MAJOR)
DBUS_INCLUDE += -DDBUS_VERSION_MINOR=$(DBUS_VERSION_MINOR)
CFLAGS += $(DBUS_INCLUDE)
endif

ifdef CONFIG_READLINE
CFLAGS_LOCAL += -DCONFIG_READLINE
LIBS_LOCAL += -lncurses -lreadline
endif

ifdef CONFIG_NATIVE_WINDOWS
CFLAGS += -DCONFIG_NATIVE_WINDOWS
LIBS += -lws2_32 -lgdi32 -lcrypt32
LIBS_c += -lws2_32
LIBS_p += -lws2_32 -lgdi32
ifeq ($(CONFIG_CRYPTO), cryptoapi)
LIBS_p += -lcrypt32
endif
endif

ifdef CONFIG_NO_STDOUT_DEBUG
CFLAGS_LOCAL += -DCONFIG_NO_STDOUT_DEBUG
ifndef CONFIG_CTRL_IFACE
CFLAGS_LOCAL += -DCONFIG_NO_WPA_MSG
endif
endif

ifdef CONFIG_IPV6
# for eapol_test only
CFLAGS += -DCONFIG_IPV6
endif

ifdef CONFIG_PEERKEY
CFLAGS_LOCAL += -DCONFIG_PEERKEY
endif

ifdef CONFIG_IEEE80211R
CFLAGS += -DCONFIG_IEEE80211R
OBJS += ../src/rsn_supp/wpa_ft.o
endif

ifndef CONFIG_NO_WPA
CFILES += wpa.c
CFILES += preauth.c
CFILES += pmksa_cache.c
CFILES += peerkey.c
CFILES += wpa_ie.c
CFILES += wpa_common.c
NEED_AES=y
else
CFLAGS_LOCAL += -DCONFIG_NO_WPA -DCONFIG_NO_WPA2
endif

ifdef CONFIG_NO_WPA2
CFLAGS_LOCAL += -DCONFIG_NO_WPA2
endif

ifdef CONFIG_NO_WPA_PASSPHRASE
CFLAGS_LOCAL += -DCONFIG_NO_PBKDF2
endif

ifdef CONFIG_NO_AES_EXTRAS
CFLAGS_LOCAL += -DCONFIG_NO_AES_WRAP
CFLAGS_LOCAL += -DCONFIG_NO_AES_CTR -DCONFIG_NO_AES_OMAC1
CFLAGS_LOCAL += -DCONFIG_NO_AES_EAX -DCONFIG_NO_AES_CBC
CFLAGS_LOCAL += -DCONFIG_NO_AES_ENCRYPT
CFLAGS_LOCAL += -DCONFIG_NO_AES_ENCRYPT_BLOCK
endif

#ifdef NEED_AES
CFILES += aes_wrap.c aes.c
#endif

ifdef NEED_DH_GROUPS
CFILES += dh_groups.c
ifdef NEED_DH_GROUPS_ALL
CFLAGS_LOCAL += -DALL_DH_GROUPS
endif
endif

ifndef NEED_FIPS186_2_PRF
CFLAGS += -DCONFIG_NO_FIPS186_2_PRF
endif

ifndef NEED_T_PRF
CFLAGS_LOCAL += -DCONFIG_NO_T_PRF
endif

ifndef NEED_TLS_PRF
CFLAGS_LOCAL += -DCONFIG_NO_TLS_PRF
endif

ifdef NEED_BASE64
CFILES += base64.c
endif

ifdef CONFIG_CLIENT_MLME
OBJS += mlme.o ../src/common/ieee802_11_common.o
CFLAGS += -DCONFIG_CLIENT_MLME
endif

ifndef CONFIG_MAIN
CONFIG_MAIN=main
endif

ifdef CONFIG_DEBUG_SYSLOG
CFLAGS_LOCAL += -DCONFIG_DEBUG_SYSLOG
endif

ifdef CONFIG_DEBUG_FILE
CFLAGS_LOCAL += -DCONFIG_DEBUG_FILE
endif

ifdef CONFIG_DELAYED_MIC_ERROR_REPORT
CFLAGS_LOCAL += -DCONFIG_DELAYED_MIC_ERROR_REPORT
endif

CFILES += scan_helpers.c

CFILES += wpa_supplicant.c events.c blacklist.c wpas_glue.c scan.c
CFILES += $(CONFIG_MAIN).c

ifdef CONFIG_PRIVSEP
OBJS_priv += $(OBJS_d) ../src/drivers/drivers.o ../src/drivers/scan_helpers.o
OBJS_priv += $(OBJS_l2)
OBJS_priv += ../src/utils/os_$(CONFIG_OS).o
OBJS_priv += ../src/utils/$(CONFIG_ELOOP).o
OBJS_priv += ../src/utils/common.o
OBJS_priv += ../src/utils/wpa_debug.o
OBJS_priv += ../src/utils/wpabuf.o
OBJS_priv += wpa_priv.o
ifdef CONFIG_DRIVER_TEST
OBJS_priv += ../src/crypto/sha1.o
OBJS_priv += ../src/crypto/md5.o
ifeq ($(CONFIG_TLS), openssl)
OBJS_priv += ../src/crypto/crypto_openssl.o
endif
ifeq ($(CONFIG_TLS), gnutls)
OBJS_priv += ../src/crypto/crypto_gnutls.o
endif
ifeq ($(CONFIG_TLS), internal)
ifeq ($(CONFIG_CRYPTO), libtomcrypt)
OBJS_priv += ../src/crypto/crypto_libtomcrypt.o
else
OBJS_priv += ../src/crypto/crypto_internal.o
endif
endif
endif # CONFIG_DRIVER_TEST
OBJS += ../src/l2_packet/l2_packet_privsep.o
OBJS += ../src/drivers/driver_privsep.o
EXTRA_progs += wpa_priv
else
CFILES += drivers.c
endif

ifdef CONFIG_NDIS_EVENTS_INTEGRATED
CFLAGS += -DCONFIG_NDIS_EVENTS_INTEGRATED
OBJS += ../src/drivers/ndis_events.o
EXTRALIBS += -loleaut32 -lole32 -luuid
ifdef PLATFORMSDKLIB
EXTRALIBS += $(PLATFORMSDKLIB)/WbemUuid.Lib
else
EXTRALIBS += WbemUuid.Lib
endif
endif

endif

include $(ARUBA_SRC)/mk/Makefile.bottom

arubainstall:
	$(INSTALL) wired_supplicant.conf $(DESTDIR)/bin/wired_supplicant.conf
	$(INSTALL) mesh_psk_supplicant.conf $(DESTDIR)/bin
