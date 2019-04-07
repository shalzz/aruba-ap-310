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

TINYPROXY=`pwd`/tinyproxy-1.8.2

all: swconfig build

swconfig:
	echo "Copy swarm config header"; 
	cd $(TINYPROXY); cp -f config.h.in.swarm config.h.in; chmod u+w config.h.in
	@if [ X$(TBUILD) = X1 ]; then \
		echo ;\
		echo "============ Experimental build ============="; \
		echo ;\
		echo "#define __EXPERIMENTAL_BUILD__" >> $(TINYPROXY)/config.h.in; \
	fi
	cd $(TINYPROXY);touch aclocal.m4

	cd $(TINYPROXY); cp -f configure.instant configure && chmod 755 configure
	@if [ $(ARUBA_IMAGE_TYPE) = "nebbiolo" ] || [ $(ARUBA_IMAGE_TYPE) = "grenache" ] || [ $(ARUBA_IMAGE_TYPE) = "palomino" ] || [ ${ARUBA_IMAGE_TYPE} = "talisker" ] || [ ${ARUBA_IMAGE_TYPE} = "dalmore" ]; then \
		cd $(TINYPROXY); \
		if [ ! -f .config.mips ]; then \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring tinyproxy for MIPS-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			$(MAKE) -f config.talisker > config.log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				for dirs in src; do \
					$(MAKE) -C $$dirs clean; \
				done; \
				touch  .config.mips ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
			printf "\n- tinyproxy already configured for MIPS-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "arran" ]; then \
		cd $(TINYPROXY); \
		if [ ! -f .config.armeb ]; then \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring tinyproxy for ARMEB-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			$(MAKE) -f config.arran > config.log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				for dirs in src; do \
					$(MAKE) -C $$dirs clean; \
				done; \
				touch  .config.armeb ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
				printf "\n- tinyproxy already configured for ARMEB-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "ardmore" ]; then \
		cd $(TINYPROXY); \
		if [ ! -f .config.ppc ]; then \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring tinyproxy for PPC-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			$(MAKE) -f config.ardmore > config.log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			for dirs in src; do \
				$(MAKE) -C $$dirs clean; \
			done; \
			if [ -n "$$completed_config)" ]; then \
				touch  .config.ppc ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
				printf "\n- tinyproxy already configured for ARM-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "springbank" ]; then \
		cd $(TINYPROXY); \
		if [ ! -f .config.arm ]; then \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring tinyproxy for ARM-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			$(MAKE) -f config.springbank > config.log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				for dirs in src; do \
					$(MAKE) -C $$dirs clean; \
				done; \
				touch  .config.arm ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
				printf "\n- tinyproxy already configured for ARM-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "octomore" ]; then \
		cd $(TINYPROXY); \
		if [ ! -f .config.arm ]; then \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring tinyproxy for ARM-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			$(MAKE) -f config.octomore > config.log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				for dirs in src; do \
					$(MAKE) -C $$dirs clean; \
				done; \
				touch  .config.arm ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
				printf "\n- tinyproxy already configured for ARM-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "x4" ]; then \
		cd $(TINYPROXY); \
		if [ ! -f .config.ppc ]; then \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring tinyproxy for PPC-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			$(MAKE) -f config.x4 > config.log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			for dirs in src; do \
				$(MAKE) -C $$dirs clean; \
			done; \
			if [ -n "$$completed_config)" ]; then \
				touch  .config.ppc ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
				printf "\n- tinyproxy already configured for PPC-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "glenmorangie" ]; then \
		cd $(TINYPROXY); \
		if [ ! -f .config.arm ]; then \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			rm -f .config.armv7le > /dev/null 2>&1; \
			printf "\nConfiguring tinyproxy for ARM-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.glenmorangie > config.log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				for dirs in src; do \
					$(MAKE) -C $$dirs clean; \
				done; \
				touch  .config.arm ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
				printf "\n- tinyproxy already configured for ARM-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	elif [ $(ARUBA_IMAGE_TYPE) = "masterson" ]; then \
		cd $(TINYPROXY); \
		if [ ! -f .config.armv7le ]; then \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.ppc > /dev/null 2>&1; \
			rm -f .config.mips > /dev/null 2>&1; \
			rm -f .config.arm > /dev/null 2>&1; \
			rm -f .config.armeb > /dev/null 2>&1; \
			printf "\nConfiguring tinyproxy for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform - \n" ; \
			make -f config.masterson > config.log; \
			completed_config=`grep "configure: exit 0" config.log`; \
			if [ -n "$$completed_config" ]; then \
				for dirs in src; do \
					$(MAKE) -C $$dirs clean; \
				done; \
				touch  .config.armv7le ; \
				printf "completed \n" ; \
			else \
				printf "configuration failed; Refer to config.log \n" ; \
				exit 1; \
			fi \
		else \
				printf "\n- tinyproxy already configured for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform\n"; \
		fi \
	else \
		printf "\n- tinyproxy do not support ARUBA_IMAGE_TYPE of $(ARUBA_IMAGE_TYPE), skipping";    \
	fi

build:
	echo "Copy swarm config header"; 
	@cd $(TINYPROXY); cp -f config.h.in.swarm config.h.in; chmod u+w config.h.in
	@if [ X$(TBUILD) = X1 ]; then \
		echo ;\
		echo "============ Experimental build ============="; \
		echo ;\
		echo "#define __EXPERIMENTAL_BUILD__" >> $(TINYPROXY)/config.h.in; \
	fi
	@if [ $(ARUBA_IMAGE_TYPE) = "nebbiolo" ] || [ $(ARUBA_IMAGE_TYPE) = "grenache" ] || [ $(ARUBA_IMAGE_TYPE) == "palomino" ] || [ ${ARUBA_IMAGE_TYPE} = "talisker" ] || [ ${ARUBA_IMAGE_TYPE} = "dalmore" ]; then \
		echo "- Building tinyproxy for MIPS-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ ${ARUBA_IMAGE_TYPE} = "arran" ]; then \
		echo "- Building tinyproxy for ARMEB-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ ${ARUBA_IMAGE_TYPE} = "ardmore" ]; then \
		echo "- Building tinyproxy for PPC-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ ${ARUBA_IMAGE_TYPE} = "springbank" ]; then \
		echo "- Building tinyproxy for ARM-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ ${ARUBA_IMAGE_TYPE} = "octomore" ]; then \
		echo "- Building tinyproxy for ARM-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ ${ARUBA_IMAGE_TYPE} = "x4" ]; then \
		echo "- Building tinyproxy for PPC-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ ${ARUBA_IMAGE_TYPE} = "glenmorangie" ]; then \
		echo "- Building tinyproxy for ARM-$(ARUBA_IMAGE_TYPE) platform"; \
	elif [ ${ARUBA_IMAGE_TYPE} = "masterson" ]; then \
		echo "- Building tinyproxy for ARMV7LE-$(ARUBA_IMAGE_TYPE) platform"; \
	else \
        echo "- Skipping unsupported platform"; \
	fi
#	echo "Copy swarm config header"; 
#	cd $(TINYPROXY); cp -f config.h.in.swarm config.h.in && \
#	make -f Makefile
	cd $(TINYPROXY); $(MAKE) -f Makefile
	mkdir -p $(OBJDIR)
	cp -f $(TINYPROXY)/src/tinyproxy $(OBJDIR)/tinyproxy
	cp -f $(TINYPROXY)/etc/tinyproxy.conf $(OBJDIR)/tinyproxy.conf

clean:  
	cd $(TINYPROXY); \
	if [ -f .config.mips ] || [ -f .config.ppc ] || [ -f .config.arm ] || [ -f .config.armeb ] || [ -f .config.armv7le ]; then \
	$(MAKE) -f Makefile clean; \
	rm -f .config.mips > /dev/null 2>&1 ; \
	rm -f .config.arm > /dev/null 2>&1 ; \
	rm -f .config.armeb > /dev/null 2>&1 ; \
	rm -f .config.ppc > /dev/null 2>&1 ; \
	rm -f .config.armv7le > /dev/null 2>&1 ; \
	rm -f config.log > /dev/null 2>&1; \
	fi
	rm -Rf $(OBJDIR)

cleanconfig:
	cd $(TINYPROXY); \
	rm -f .config.mips > /dev/null 2>&1 ; \
	rm -f .config.arm > /dev/null 2>&1 ; \
	rm -f .config.armeb > /dev/null 2>&1 ; \
	rm -f .config.ppc > /dev/null 2>&1 ; \
	rm -f .config.armv7le > /dev/null 2>&1 ; \
	rm -f config.log > /dev/null 2>&1

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
	cp -a $(OBJDIR)/tinyproxy  $(SYMBOLS_DIR)

arubainstall: 
	cp -a $(OBJDIR)/tinyproxy $(DESTDIR)/bin
	$(STRIP) $(DESTDIR)/bin/tinyproxy
	cp -a $(OBJDIR)/tinyproxy.conf $(DESTDIR)/../etc
	mkdir -p $(DESTDIR)/../var/log/tinyproxy
	mkdir -p $(DESTDIR)/../var/run/tinyproxy

cleaninstall:
	rm -rf $(DESTDIR)/bin/tinyproxy
