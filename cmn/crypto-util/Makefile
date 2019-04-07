include $(ARUBA_SRC)/mk/Makefile.top

SECTIONS = ARUBA_CRYPTO_UTIL_LIBRARY

#CFLAGS_LOCAL += ${WERROR}
INCS_LOCAL := $(MINCDIRS)
LIBS_LOCAL_PATH := $(MLIBDIRS)
CFILES      = des_enc.c md5.c

ifndef FAT_AP
ifneq "" "$(FIPS)$(if $(AP_PLATFORM),,1)"
INCS_LOCAL += -I$(OPENSSL_DIR)/include -I$(OPENSSL_DIR)/include/openssl
LIBS_LOCAL := -lcrypto 
LIBS_LOCAL_PATH += -L$(OPENSSL_DIR)/$(OBJDIR)
ifdef FIPS
CFILES      += aes_wrap.c
endif
else
INCS_LOCAL += -I$(XYSSL)/include/xyssl
LIBS_LOCAL := -lxyssl 
CFLAGS_LOCAL += -DUSE_XYSSL
endif

ifdef AP_PLATFORM
ifdef ARDMORE
CFLAGS_LOCAL += -fno-strict-aliasing
endif
CFILES += sha1_nativeimpl.c sha256_nativeimpl.c aes_wrap_nativeimpl.c aes_nativeimpl.c
EXCLUDE_CFILES_${FIPS} += aes_wrap_nativeimpl.c aes_nativeimpl.c
else
CFILES += sha1.c sha256.c sha384.c cryptohash.c aes_wrap.c
endif
endif

ifdef FAT_AP
ifdef NO_OPENSSL

CYASSL_WRAP_FILES = x509_cyassl_wrap.c

INCS_LOCAL += -I$(CYASSL_DIR) -I$(CYASSL_DIR)/cyassl -I$(CYASSL_DIR)/wolfssl
LIBS_LOCAL := -lcyassl
LIBS_LOCAL_PATH += -L$(CYASSL_DIR)/src/.libs
CFILES     := sha1.c sha256.c sha384.c cryptohash_cyassl.c aes_wrap_cyassl.c des_enc_cyassl.c md5.c $(CYASSL_WRAP_FILES)

else
CFILES     += sha1.c sha256.c sha384.c cryptohash.c aes_wrap.c
INCS_LOCAL += -I$(OPENSSL_DIR)/include -I$(OPENSSL_DIR)/include/openssl
LIBS_LOCAL := -lcrypto 
LIBS_LOCAL_PATH += -L$(OPENSSL_DIR)/$(OBJDIR)
endif
endif

BUILD_DSO=1
LIBRARY = aruba-crypto-util

# ------------------------------------------------------------
# below are test app to test the aes wrapping functions
#
# Define SECTIONS as below
#
# SECTIONS = ARUBA_CRYPTO_UTIL_LIBRARY ARUBA_CRYPTO_AES_WRAP_3DES_TEST_APP ARUBA_CRYPTO_AES_WRAP_TEST_APP
#
ifdef ARUBA_CRYPTO_AES_WRAP_3DES_TEST_APP
CFILES      = aes_wrap_3des_test.c
INCS_LOCAL := $(MINCDIRS) -I$(OPENSSL_DIR)/include -I$(OPENSSL_DIR)/include/openssl
LIBS_LOCAL      = -lcrypto -laruba-crypto-util
LIBS_LOCAL_PATH := $(MLIBDIRS) -L$(OPENSSL_DIR)/$(OBJDIR)
APPLICATION     = aeswrap-3des-test
endif

# ------------------------------------------------------------
ifdef ARUBA_CRYPTO_AES_WRAP_TEST_APP
CFILES = aes_wrap_test.c
INCS_LOCAL += $(MINCDIRS) -I$(OPENSSL_DIR)/include -I$(OPENSSL_DIR)/include/openssl -Wall -fno-strict-aliasing 
LIBS_LOCAL = -lcrypto -laruba-crypto-util
LIBS_LOCAL_PATH = -L$(OPENSSL_DIR)/$(OBJDIR)
APPLICATION = aeswrap-test
endif

ifdef NO_OPENSSL
DIRS += polarssl 
endif

include $(ARUBA_SRC)/mk/Makefile.bottom

