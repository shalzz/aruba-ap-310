include $(ARUBA_SRC)/mk/Makefile.top

LIBUSB=libusb-0.1.12
CFLAGS_LOCAL += -Werror -L.
INCS_LOCAL := -I${LIBUSB}
LIBS_LOCAL_PATH := -L${LIBUSB}/${OBJDIR} 
ifneq ($(OEM), siemens)
LIBS_LOCAL_PATH += -L$(ARUBA_SRC)/cmn/crypto-util/
endif
LIBS_LOCAL += -lusb 
ifneq ($(OEM), siemens)
LIBS_LOCAL += -laruba-crypto-util
endif

ifndef AP_PLATFORM
LIBS_LOCAL_PATH += -L$(OPENSSL_DIR)/$(OBJDIR)
LIBS_LOCAL += -lcrypto
endif

ifdef FIPS
ifdef NO_OPENSSL
ifneq ($(OEM), siemens)
INCS_LOCAL += -I$(CYASSL_DIR) -I$(CYASSL_DIR)/cyassl
LIBS_LOCAL += -lcyassl
LIBS_LOCAL_PATH += -L$(CYASSL_DIR)/src/.libs
endif
else#ifdef NO_OPENSSL
ifneq ($(OEM), siemens)
INCS_LOCAL += -I$(ARUBA_SRC)/openssl-f/include
endif
CFLAGS_LOCAL += -I$(OPENSSL_DIR)/include
LIBS_LOCAL_PATH += -L$(INSTALL_LIBS_DIR) -L${OPENSSL_DIR}/${OBJDIR}
endif#ifdef NO_OPENSSL
endif


CFILES= usb_modeswitch.c

APPLICATION= usb_modeswitch

include $(ARUBA_SRC)/mk/Makefile.bottom

arubainstall:
	$(INSTALL) usb_modeswitch.conf $(DESTDIR)/../etc
