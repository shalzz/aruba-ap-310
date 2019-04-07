SHELL = /bin/sh

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

MODULE_NAME= libwebsockets-1.23-chrome32-firefox24
MODULE_DIR=`pwd`/$(MODULE_NAME)
MODULE_STATIC_OBJ_NAME=libwebsockets.a
MODULE_DYNAMIC_OBJ_NAME=libwebsockets.so

ARUBA_OBJ_PREFIX=$(ARUBA_SRC)/usr/lib/aruba

.PHONY: all
all: swconfig build

.PHONY: swconfig
swconfig:
	@mkdir -p $(ARUBA_OBJ_PREFIX)/$(OBJDIR)
	@if [ $(ARUBA_IMAGE_TYPE) = "nebbiolo" ] || [ $(ARUBA_IMAGE_TYPE) = "grenache" ] || [ $(ARUBA_IMAGE_TYPE) = "palomino" ] || [ ${ARUBA_IMAGE_TYPE} = "talisker" ] || [ ${ARUBA_IMAGE_TYPE} = "dalmore" ]; then \
		cd $(MODULE_DIR); \
		if [ ! -f .config.mips ]; then \
			touch ./configure; \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring $(MODULE_NAME) for MIPS-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.talisker > config-$(ARUBA_IMAGE_TYPE).log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				touch  .config.mips ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config-$(ARUBA_IMAGE_TYPE).log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- $(MODULE_NAME) already configured for MIPS-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "arran" ]; then \
		cd $(MODULE_DIR); \
		if [ ! -f .config.armeb ]; then \
			touch ./configure; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring $(MODULE_NAME) for ARMEB-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.arran > config-$(ARUBA_IMAGE_TYPE).log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				touch  .config.armeb ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config-$(ARUBA_IMAGE_TYPE).log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- $(MODULE_NAME) already configured for ARMEB-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "ardmore" ]; then \
		cd $(MODULE_DIR); \
		if [ ! -f .config.ppc ]; then \
			touch ./configure; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring $(MODULE_NAME) for PPC-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.ardmore > config-$(ARUBA_IMAGE_TYPE).log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				touch  .config.ppc ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config-$(ARUBA_IMAGE_TYPE).log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- $(MODULE_NAME) already configured for PPC-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "springbank" ]; then \
		cd $(MODULE_DIR); \
		if [ ! -f .config.arm ]; then \
			touch ./configure; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring $(MODULE_NAME) for ARM-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.springbank > config-$(ARUBA_IMAGE_TYPE).log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				touch  .config.arm ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config-$(ARUBA_IMAGE_TYPE).log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- $(MODULE_NAME) already configured for ARM-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "octomore" ]; then \
		cd $(MODULE_DIR); \
		if [ ! -f .config.arm ]; then \
			touch ./configure; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring $(MODULE_NAME) for ARM-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.octomore > config-$(ARUBA_IMAGE_TYPE).log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				touch  .config.arm ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config-$(ARUBA_IMAGE_TYPE).log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- $(MODULE_NAME) already configured for ARM-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "x4" ]; then \
		cd $(MODULE_DIR); \
		if [ ! -f .config.ppc ]; then \
			touch ./configure; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring $(MODULE_NAME) for PPC-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.x4 > config-$(ARUBA_IMAGE_TYPE).log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				touch  .config.ppc ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config-$(ARUBA_IMAGE_TYPE).log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- $(MODULE_NAME) already configured for PPC-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "glenmorangie" ]; then \
		cd $(MODULE_DIR); \
		if [ ! -f .config.arm ]; then \
			touch ./configure; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring $(MODULE_NAME) for ARM-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.glenmorangie > config-$(ARUBA_IMAGE_TYPE).log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				touch  .config.arm ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config-$(ARUBA_IMAGE_TYPE).log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- $(MODULE_NAME) already configured for ARM-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "masterson" ]; then \
		cd $(MODULE_DIR); \
		if [ ! -f .config.armv7le ]; then \
			touch ./configure; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			printf "\nConfiguring $(MODULE_NAME) for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.masterson > config-$(ARUBA_IMAGE_TYPE).log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				touch  .config.armv7le ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config-$(ARUBA_IMAGE_TYPE).log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- $(MODULE_NAME) already configured for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "kilchoman" ]; then \
		cd $(MODULE_DIR); \
		if [ ! -f .config.armv7le ]; then \
			touch ./configure; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			printf "\nConfiguring $(MODULE_NAME) for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.kilchoman > config-$(ARUBA_IMAGE_TYPE).log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				touch  .config.armv7le ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config-$(ARUBA_IMAGE_TYPE).log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- $(MODULE_NAME) already configured for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "lagavulin" ]; then \
		cd $(MODULE_DIR); \
		if [ ! -f .config.armv7le ]; then \
			touch ./configure; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			printf "\nConfiguring $(MODULE_NAME) for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.lagavulin > config-$(ARUBA_IMAGE_TYPE).log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				touch  .config.armv7le ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config-$(ARUBA_IMAGE_TYPE).log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- $(MODULE_NAME) already configured for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	else \
        echo "$(MODULE_NAME) do not support ARUBA_IMAGE_TYPE of $(ARUBA_IMAGE_TYPE), skipping";    \
	fi

.PHONY: build
build: swconfig
	@mkdir -p $(ARUBA_OBJ_PREFIX)/$(OBJDIR)
	@if [ ${ARUBA_IMAGE_TYPE} = "talisker" ] || [ ${ARUBA_IMAGE_TYPE} = "dalmore" ]; then \
		echo "- Building $(MODULE_NAME) for MIPS-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ $(ARUBA_IMAGE_TYPE) = "arran" ]; then \
		echo "- Building $(MODULE_NAME) for ARMEB-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ $(ARUBA_IMAGE_TYPE) = "ardmore" ]; then \
		echo "- Building $(MODULE_NAME) for PPC-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ $(ARUBA_IMAGE_TYPE) = "springbank" ]; then \
		echo "- Building $(MODULE_NAME) for ARM-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ $(ARUBA_IMAGE_TYPE) = "octomore" ]; then \
		echo "- Building $(MODULE_NAME) for ARM-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ $(ARUBA_IMAGE_TYPE) = "glenmorangie" ]; then \
		echo "- Building $(MODULE_NAME) for ARM-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ $(ARUBA_IMAGE_TYPE) = "masterson" ] || [ ${ARUBA_IMAGE_TYPE} = "kilchoman" ] || [ ${ARUBA_IMAGE_TYPE} = "lagavulin" ]; then \
		echo "- Building $(MODULE_NAME) for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform"; \
	else \
		echo "- Skipping unsupported platform"; \
	fi
	cd $(MODULE_DIR) && $(MAKE) -f Makefile 
	cp -f $(MODULE_DIR)/lib/.libs/$(MODULE_STATIC_OBJ_NAME) $(ARUBA_OBJ_PREFIX)/$(OBJDIR)/$(MODULE_STATIC_OBJ_NAME) 
	cp -f $(MODULE_DIR)/lib/libwebsockets.h $(ARUBA_OBJ_PREFIX)/$(OBJDIR) 

.PHONY: clean
clean:  
	cd $(MODULE_DIR); \
	if [ -f .config.mips ] || [ -f .config.arm ] || [ -f .config.ppc ] || [ -f .config.armeb ] || [ -f .config.armv7le ]; then \
	make -f Makefile clean; \
	rm -f .config.mips > /dev/null 2>&1 ; \
	rm -f .config.ppc > /dev/null 2>&1 ; \
	rm -f .config.arm > /dev/null 2>&1 ; \
	rm -f .config.armeb > /dev/null 2>&1 ; \
	rm -f .config.armv7le > /dev/null 2>&1 ; \
	rm -f config-$(ARUBA_IMAGE_TYPE).log > /dev/null 2>&1; \
	fi
	rm -Rf $(OBJDIR)

.PHONY: cleanconfig
cleanconfig:
	cd $(MODULE_DIR); \
	rm -f .config.mips > /dev/null 2>&1 ; \
	rm -f .config.ppc > /dev/null 2>&1 ; \
	rm -f .config.arm > /dev/null 2>&1 ; \
	rm -f .config.armeb > /dev/null 2>&1 ; \
	rm -f .config.armv7le > /dev/null 2>&1 ; \
	rm -f config-$(ARUBA_IMAGE_TYPE).log > /dev/null 2>&1

.PHONY: depend
depend:
	$(MAKE) -f Makefile depend

ifneq "$(findstring savesymbols,$(MAKECMDGOALS))" ""
DEST_LIB_DIR=$(SYMBOLS_DIR)
else
DEST_LIB_DIR=$(DESTDIR)/../lib
endif

.PHONY: savesymbols
savesymbols: 
	@mkdir -p $(SYMBOLS_DIR)
	cp -a $(ARUBA_OBJ_PREFIX)/$(OBJDIR)/$(MODULE_STATIC_OBJ_NAME) ${SYMBOLS_DIR}/$(MODULE_STATIC_OBJ_NAME)

.PHONY: arubainstall
arubainstall:
	#@cd $(MODULE_DIR) \
	#cp -a ./libwebsockets-test-server.pem         $(DESTDIR)/bin \
	#cp -a ./libwebsockets-test-server.key.pem     $(DESTDIR)/bin \
	#cp -a $(OBJDIR)/libwebsockets-test-server $(DESTDIR)/bin \
	#cp -a $(OBJDIR)/libwebsockets-test-client $(DESTDIR)/bin 

.PHONY: cleaninstall
cleaninstall:
	#rm -rf $(DESTDIR)/bin/libwebsockets-test-server
	#rm -rf $(DESTDIR)/bin/libwebsockets-test-client
	#rm -rf $(DESTDIR)/bin/libwebsockets-test-server.pem
	#rm -rf $(DESTDIR)/bin/libwebsockets-test-server.key.pem
