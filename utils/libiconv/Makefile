SHELL = /bin/sh

include $(ARUBA_SRC)/mk/Makefile.top

#!/bin/sh
include $(ARUBA_SRC)/mk/Makefile.rules
include $(ARUBA_SRC)/mk/Makefile.toolsdefs

TOP = ..

ifdef UC_CROSS
ifndef EMS
ifdef MIPS_XLR_PLATFORM
CONFIG = ./Configure no-dso linux-mips-xlr shared
else
CONFIG = ./Configure no-dso linux-x86-ppc-uclibc-0.9.26 shared
endif
else
CONFIG = ./Configure no-dso linux-x86-uclibc-0.9.26 shared
endif
else
CONFIG = ./config no-dso
endif

ifdef AP_PLATFORM
CONFIG = ./Configure no-dso linux-${MAIN_ARCH} shared
endif

##################################################
# targets
#
# all       -- compile 
# clean     -- remove object files
# depend    -- create dependency files

TOOL_PREFIX = $(patsubst %-,%,$(notdir $(ARUBA_TOOLS_PREFIX)))
TOOL_PATH = $(dir $(ARUBA_TOOLS_PREFIX))

VER_2=2
VER_2_5=2.5
VER_2_5_1=2.5.1
LINK_LIBICONV=libiconv.so
LIBICONV_NAME=$(LINK_LIBICONV).$(VER_2_5_1)

MODULE_NAME=libiconv-1.14
MODULE_DIR=$(ARUBA_SRC)/utils/libiconv/libiconv-1.14

ifdef NO_OPENSSL
	SSL_INC=$(CYASSL_DIR)
	SSL_LIB_DIR=$(CYASSL_DIR)/src/.libs
	SSL_OPTION=--with-cyassl
else
	SSL_INC=$(OPENSSL_DIR)/include
	SSL_LIB_DIR=$(OPENSSL_DIR)/$(OBJDIR)
	SSL_OPTION=--enable-openssl
endif

CC=$(ARUBA_TOOLS_PREFIX)gcc
NM=$(ARUBA_TOOLS_PREFIX)nm
CFLAGS=-B$(ARUBA_TOOLS)/lib -B$(ARUBA_TOOLS)/usr/lib --sysroot=${ARUBA_SYSROOT} -I$(ARUBA_TOOLS)/include -I$(ARUBA_TOOLS)/usr/include -I$(SSL_INC)
#CPP=$(ARUBA_TOOLS_PREFIX)cpp -I$(ARUBA_TOOLS)/usr/include -I$(SSL_INC)
CPP=$(ARUBA_TOOLS_PREFIX)cpp
CXXCPP=${CPP}
CXXCPP=g++ -E
CPPFLAGS=-B$(ARUBA_TOOLS)/lib -B$(ARUBA_TOOLS)/usr/lib -I$(SSL_INC) -I$(ARUBA_TOOLS)/include -I$(ARUBA_TOOLS)/usr/include -I$(ARUBA_SRC)/utils/zlib --sysroot=${ARUBA_SYSROOT} 
LDFLAGS=-B$(ARUBA_TOOLS)/lib -B$(ARUBA_TOOLS)/usr/lib --sysroot=${ARUBA_SYSROOT}

export CC
export NM
export CFLAGS
export CPP
export CXXCPP
export CPPFLAGS
export LDFLAGS
export ARUBA_SYSROOT

MYCONFIG =./configure \
        --host=$(TOOL_PREFIX) \
		--with-gnu-ld


all: swconfig build

swconfig:
	@cd $(MODULE_DIR); \
	if [ ! -f .config.$(ARUBA_IMAGE_TYPE) ]; then \
		echo "Configuring $(MODULE_NAME) for $(ARUBA_IMAGE_TYPE) platform" ; \
		$(MYCONFIG) > config-$(ARUBA_IMAGE_TYPE).log; \
		cat config-$(ARUBA_IMAGE_TYPE).log; \
		completed_config=`grep "configure: exit 0" config.log`; \
		if [ -n "$$completed_config" ]; then \
			touch  .config.$(ARUBA_IMAGE_TYPE) ; \
			echo "completed" ; \
		else \
			echo "configuration failed; Refer to config.log" ; \
			exit 1; \
		fi \
	else \
		echo "$(MODULE_NAME) already configured for $(ARUBA_IMAGE_TYPE) platform"; \
	fi \

build: swconfig
	@mkdir -p $(OBJDIR)
	@echo "Building $(MODULE_NAME) for $(ARUBA_IMAGE_TYPE) platform"
	@cd $(MODULE_DIR) && $(MAKE) -f Makefile
	@cp -f $(MODULE_DIR)/lib/.libs/$(LIBICONV_NAME) $(OBJDIR)/$(LIBICONV_NAME)
	@cd $(OBJDIR) && ln -sf $(LIBICONV_NAME) $(LINK_LIBICONV) && ln -sf $(LIBICONV_NAME) $(LINK_LIBICONV).$(VER_2) && ln -sf $(LIBICONV_NAME) $(LINK_LIBICONV).$(VER_2_5)

clean:  
	cd $(MODULE_DIR); \
	if [ -f Makefile ]; then \
		$(MAKE) -f Makefile clean; \
		for i in $(ARUBA_IMAGES); do \
			if [ $$i != $(ARUBA_IMAGE_TYPE) ]; then \
				rm -f .config.$$i > /dev/null 2>&1; \
			fi \
		done \
	fi
	rm -Rf $(OBJDIR)

cleanconfig:
	cd $(MODULE_DIR); \
	rm -f .config.* > /dev/null 2>&1 ;

depend:
	$(MAKE) -f Makefile depend

.PHONY: all clean depend 

ifneq "$(findstring savesymbols,$(MAKECMDGOALS))" ""
DEST_LIB_DIR=$(SYMBOLS_DIR)
else
DEST_LIB_DIR=$(DESTDIR)/../lib
endif

savesymbols: 
	mkdir -p $(SYMBOLS_DIR)
	cp -a $(OBJDIR)/$(LIBICONV_NAME)  $(SYMBOLS_DIR)/$(LIBICONV_NAME)

arubainstall: 
	cp -a $(OBJDIR)/$(LIBICONV_NAME) $(DEST_LIB_DIR)
	$(STRIP) $(DEST_LIB_DIR)/$(LIBICONV_NAME)
	cd $(DEST_LIB_DIR) && ln -sf $(LIBICONV_NAME) $(LINK_LIBICONV) && ln -sf $(LIBICONV_NAME) $(LINK_LIBICONV).$(VER_2) && ln -sf $(LIBICONV_NAME) $(LINK_LIBICONV).$(VER_2_5)
#	cp -a $(OBJDIR)/libiconv.conf $(DESTDIR)/../etc
#	mkdir -p $(DESTDIR)/../var/log/libiconv
#	mkdir -p $(DESTDIR)/../var/run/libiconv

cleaninstall:
	rm -rf $(DEST_LIB_DIR)/lib/$(LIBICONV_NAME)
