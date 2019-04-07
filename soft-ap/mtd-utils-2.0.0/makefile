SHELL = /bin/sh

include $(ARUBA_SRC)/mk/Makefile.rules
include $(ARUBA_SRC)/mk/Makefile.toolsdefs
export ARUBA_SYSROOT

TOP = ..

ifdef AP_PLATFORM
TOOL_PREFIX = $(patsubst %-,%,$(notdir $(ARUBA_TOOLS_PREFIX)))
TOOL_PATH = $(dir $(ARUBA_TOOLS_PREFIX))
PATH := $(PATH):$(TOOL_PATH)
CONFIG = ./configure --host=$(TOOL_PREFIX) --without-jffs --without-xattr --without-lzo --disable-tests --without-ubifs 
endif

CFLAGS := $(ARUBA_GCC_BASE)
export CFLAGS
##################################################
# targets
#
# all       -- compile 
# clean     -- remove object files
# depend    -- create dependency files

ARUBAINSTALL_${SPRINGBANK} = arubainstall_real
ARUBAINSTALL_${OCTOMORE} = arubainstall_real
ARUBAINSTALL_${GLENMORANGIE} = arubainstall_real
ARUBAINSTALL_${X4} = arubainstall_real
ARUBAINSTALL_${MASTERSON} = arubainstall_real
BUILD_${SPRINGBANK} = build_real
BUILD_${OCTOMORE} = build_real
BUILD_${GLENMORANGIE} = build_real
BUILD_${X4} = build_real
BUILD_${MASTERSON} = build_real

all: swconfig  build

swconfig:
	@if [ $(ARUBA_IMAGE_TYPE) = "springbank" ]; then \
		if [ ! -f .config.armle ]; then \
			rm -f .config.armv7 > /dev/null 2>&1; \
			rm -f .config.ppc-e5500 > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring mtd-utils for ARMLE-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			$(CONFIG) > config-$(ARUBA_IMAGE_TYPE).log 2>&1 ; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				[[ -e Makefile ]] && $(MAKE) -f Makefile clean; \
				touch  .config.armle ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- mtd-utils already configured for ARMLE-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "octomore" ]|| [ ${ARUBA_IMAGE_TYPE} = "glenmorangie" ]; then \
		if [ ! -f .config.armv7 ]; then \
			rm -f .config.armle > /dev/null 2>&1; \
			rm -f .config.ppc-e5500 > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring mtd-utils for ARMv7-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			$(CONFIG) > config-$(ARUBA_IMAGE_TYPE).log 2>&1 ; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				[[ -e Makefile ]] && $(MAKE) -f Makefile clean; \
				touch  .config.armv7 ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- mtd-utils already configured for ARMv7-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "x4" ]; then \
		if [ ! -f .config.ppc-e5500 ]; then \
			rm -f .config.armv7 > /dev/null 2>&1; \
			rm -f .config.armle > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring mtd-utils for PPC-E5500-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			$(CONFIG) > config-$(ARUBA_IMAGE_TYPE).log 2>&1 ; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				[[ -e Makefile ]] && $(MAKE) -f Makefile clean; \
				touch  .config.ppc-e5500 ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- mtd-utils already configured for PPC-E5500-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "masterson" ]; then \
		if [ ! -f .config.armv7le ]; then \
			rm -f .config.armv7 > /dev/null 2>&1; \
			rm -f .config.armle > /dev/null 2>&1; \
			rm -f .config.ppc-e5500 > /dev/null 2>&1; \
			printf "\nConfiguring mtd-utils for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			$(CONFIG) > config-$(ARUBA_IMAGE_TYPE).log 2>&1 ; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				[[ -e Makefile ]] && $(MAKE) -f Makefile clean; \
				touch  .config.armv7le ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- mtd-utils already configured for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	else \
        echo "Not support machine type: $(ARUBA_IMAGE_TYPE)";    \
		exit 0; \
	fi

build_real:
	@if [ $(ARUBA_IMAGE_TYPE) = "springbank" ]; then \
		echo "- Building mtd-utils for ARMLE-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ $(ARUBA_IMAGE_TYPE) = "octomore" ] || [ $(ARUBA_IMAGE_TYPE) = "glenmorangie" ]; then \
		echo "- Building mtd-utils for ARMv7-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ $(ARUBA_IMAGE_TYPE) = "x4" ]; then \
		echo "- Building mtd-utils for PPC-E5500-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ $(ARUBA_IMAGE_TYPE) = "masterson" ]; then \
		echo "- Building mtd-utils for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform"; \
	else \
        echo "Not support machine type: $(ARUBA_IMAGE_TYPE)";    \
	fi
	@echo ""
	$(MAKE) -f Makefile
	mkdir -p $(OBJDIR)
	cp -f flash_erase $(OBJDIR)
	if [ $(ARUBA_IMAGE_TYPE) = "masterson" ]; then \
	cp -f mtdinfo $(OBJDIR) ; \
	cp -f mtdpart $(OBJDIR) ; \
	cp -f mtd_debug $(OBJDIR) ; \
	cp -f nanddump $(OBJDIR) ; \
	cp -f nandtest $(OBJDIR) ; \
	cp -f nandwrite $(OBJDIR) ; \
	cp -f ubiattach $(OBJDIR) ; \
	cp -f ubicrc32 $(OBJDIR) ; \
	cp -f ubidetach $(OBJDIR) ; \
	cp -f ubinfo $(OBJDIR) ; \
	cp -f ubirmvol $(OBJDIR) ; \
	fi

build: ${BUILD_yes}
	@true

links:
	make -f Makefile links;

clean:
	if [ -f .config.armv7 ] || [ -f .config.armle ] || [ -f .config.ppc-e5500 ] || [ -f .config.armv7le ]; then \
	[[ -e Makefile ]] && $(MAKE) -f Makefile clean; \
	rm -f .config.armv7 > /dev/null 2>&1 ; \
	rm -f .config.armle > /dev/null 2>&1 ; \
	rm -f .config.ppc-e5500 > /dev/null 2>&1 ; \
	rm -f .config.armv7le > /dev/null 2>&1 ; \
	rm -f config.log config.status libtool Makefile; \
	fi
	rm -Rf $(OBJDIR)

cleanconfig:
	rm -f .config.armv7 > /dev/null 2>&1 ; \
	rm -f .config.armle > /dev/null 2>&1 ; \
	rm -f .config.ppc-e5500 > /dev/null 2>&1 ; \
	rm -f .config.armv7le > /dev/null 2>&1;

depend:
	make -f Makefile depend

.PHONY: all clean depend 

Makefile:
	$(CONFIG)

# we do not do DSOs on ZMASTER yet.
arubainstall_real: 
	$(STRIP) -o $(DESTDIR)/bin/flash_erase $(OBJDIR)flash_erase
	if [ $(ARUBA_IMAGE_TYPE) = "masterson" ]; then \
	$(STRIP) -o $(DESTDIR)/bin/mtdinfo $(OBJDIR)mtdinfo ; \
	$(STRIP) -o $(DESTDIR)/bin/mtdpart $(OBJDIR)mtdpart ; \
	$(STRIP) -o $(DESTDIR)/bin/mtd_debug $(OBJDIR)mtd_debug ; \
	$(STRIP) -o $(DESTDIR)/bin/nanddump $(OBJDIR)nanddump ; \
	$(STRIP) -o $(DESTDIR)/bin/nandtest $(OBJDIR)nandtest ; \
	$(STRIP) -o $(DESTDIR)/bin/nandwrite $(OBJDIR)nandwrite ; \
	$(STRIP) -o $(DESTDIR)/bin/ubiattach $(OBJDIR)ubiattach ; \
	$(STRIP) -o $(DESTDIR)/bin/ubicrc32 $(OBJDIR)ubicrc32 ; \
	$(STRIP) -o $(DESTDIR)/bin/ubidetach $(OBJDIR)ubidetach ; \
	$(STRIP) -o $(DESTDIR)/bin/ubinfo $(OBJDIR)ubinfo ; \
	$(STRIP) -o $(DESTDIR)/bin/ubirmvol $(OBJDIR)ubirmvol ; \
	fi

arubainstall: ${ARUBAINSTALL_yes}
	@true

cleaninstall:
	rm -Rf $(DESTDIR)/bin/flash_erase
	if [ $(ARUBA_IMAGE_TYPE) = "masterson" ]; then \
	rm -Rf $(DESTDIR)/bin/mtdinfo ; \
	rm -Rf $(DESTDIR)/bin/mtdpart ; \
	rm -Rf $(DESTDIR)/bin/mtd_debug ; \
	rm -Rf $(DESTDIR)/bin/nanddump ; \
	rm -Rf $(DESTDIR)/bin/nandtest ; \
	rm -Rf $(DESTDIR)/bin/nandwrite ; \
	rm -Rf $(DESTDIR)/bin/ubiattach ; \
	rm -Rf $(DESTDIR)/bin/ubicrc32 ; \
	rm -Rf $(DESTDIR)/bin/ubidetach ; \
	rm -Rf $(DESTDIR)/bin/ubinfo ; \
	rm -Rf $(DESTDIR)/bin/ubirmvol  ; \
	fi

include $(ARUBA_SRC)/mk/Makefile.bottom
