# Makefile for busybox
#
# Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

include $(ARUBA_SRC)/mk/Makefile.rules
include aruba-quiet.mk

ifdef ARUBA_MAKE_VERBOSE
__SILENT=-o
else
__SILENT=-s
endif

# XXX Not sure why we need this, was compiling without it. It tries 
# to also link the library object directory which is unnecessary here
LDFLAGS = 

unexport MAKEFLAGS
ifdef AP_PLATFORM
#CRYPT_LIB = -lcrypt -lc	-lgcc
CRYPT_LIB = -lc	-lgcc
LDFLAGS += $(ARUBA_GCC_BASE) 
#LDFLAGS += $(ARUBA_TOOLS)/lib/ld-uClibc.so.0
INSTALL_DIR = $(ARUBA_SRC)/${ARUBA_IMAGE_TYPE}fs/ucfs/target
DOSTATIC = false
ifdef LTT
SUFF=.ltt
endif

ifdef FAT_AP
CONF=fatap
else
CONF=non-merlot
endif

ifdef MERLOT
CONF=merlot
endif

endif	# AP_PLATFORM

ifneq ($(FILESYSTEM),)
INSTALL_DIR = $(ARUBA_SRC)/$(FILESYSTEM)/target
endif

ifneq ($(TARGET_FILESYSTEM),)
INSTALL_DIR = $(TARGET_FILESYSTEM)
endif

#--------------------------------------------------------------
# You shouldn't need to mess with anything beyond this point...
#--------------------------------------------------------------
noconfig_targets := menuconfig config oldconfig randconfig \
	defconfig allyesconfig allnoconfig clean distclean \
	release tags

ifndef TOPDIR
TOPDIR=$(CURDIR)/
endif
ifndef top_srcdir
top_srcdir=$(CURDIR)
endif
ifndef top_builddir
top_builddir=$(CURDIR)
endif

export srctree=$(top_srcdir)
vpath %/Config.in $(srctree)

include $(top_builddir)/Rules.mak

DIRS:=applets archival archival/libunarchive coreutils console-tools \
	debianutils editors findutils init miscutils modutils networking \
	networking/libiproute networking/udhcp procps loginutils shell \
	sysklogd util-linux libpwdgrp coreutils/libcoreutils libbb

SRC_DIRS:=$(patsubst %,$(top_srcdir)/%,$(DIRS))

# Remove the FORTIFY_SOURCE_OPTION when building busybox
# as the 'tar' command SIGABRT's from a 'buffer overrun'.
ifdef FORTIFY_SOURCE_ENABLE
CFLAGS := $(filter-out $(FORTIFY_SOURCE_OPTION), $(CFLAGS))
endif

ifeq ($(strip $(CONFIG_SELINUX)),y)
CFLAGS += -I/usr/include/selinux
LIBRARIES += -lsecure
endif

ifdef ARUBA_GCOV
LIBRARIES += -lgcov
endif

ifdef SAP_LOGIN_ENABLE
CFLAGS += -DSAP_LOGIN_ENABLE
endif

ifdef AP_PLATFORM
CFLAGS += -DSAP_LOGIN_ENABLE -I$(ARUBA_SRC)/freeswan-1.95/libdes -DHEADER_DES_LOCL_H -I$(ARUBA_TOOLS)/include -I${ARUBA_TOOLS}/usr/include -DSCA_CONSOLE_SHELL_HACK_AP -Werror

ifndef PALOMINO
ifndef FAT_AP
LIBS_LOCAL += -lconprt -laruba-crypto-util -lfl -lcmnutil -loem -lwifi -lsyslog -lMHApi -ldispatcher
endif

ifdef FIPS
ifdef NO_OPENSSL
ifndef FAT_AP
INCS_LOCAL += -I$(CYASSL_DIR) -I$(CYASSL_DIR)/cyassl
LIBS_LOCAL += -lcyassl
LIBS_LOCAL_PATH += -L$(CYASSL_DIR)/.src/libs
else
# Linking with cyassl causes the FIPS KAT constructor to be called any time a command like 'ls' is 
# used. This slows the AP boot process (and runtime functionality using 'system' to do stuff like cat 
# etc.) very slow (upto 4x slow).
# Busybox does not use any ssl or crypto functionality, disabling this does not have any affect on 
# busybox build. So donot link with cyassl.
# Looks like linking with openssl in the else below is also not required, but not sure if it will 
# affect AOS.
endif
else
INCS_LOCAL += -I$(OPENSSL_DIR)/include
LIBS_LOCAL_PATH += -L$(OPENSSL_DIR)/$(OBJDIR)
LIBS_LOCAL += -lcrypto
endif
else#FIPS
ifdef GRENACHE
INCS_LOCAL += -I$(ARUBA_SRC)/openssl-f/include
LIBS_LOCAL_PATH += -L$(OPENSSL_DIR)/$(OBJDIR)
LIBS_LOCAL += -lcrypto
else
INCS_LOCAL += -I$(ARUBA_SRC)/xyssl/xyssl-0.9/include
LIBS_LOCAL_PATH += -L$(ARUBA_SRC)/xyssl/xyssl-0.9/library/$(OBJDIR)
LIBS_LOCAL += -lxyssl
endif # GRENACHE
endif # FIPS
endif # PALOMINO
endif # AP_PLATFORM

ifdef SWITCH_PLATFORM
CFLAGS += $(ARUBA_GCC_BASE) -DSCA_CONSOLE_SHELL_HACK
endif
ifdef REGULATORY_TESTING
CFLAGS += -DREGULATORY_TESTING
endif

ifdef MIPS_XLP_PLATFORM
ifdef MILAGRO
CFLAGS += -mabi=64 -msym32 -fPIC -march=xlp
LDFLAGS += -mabi=64 -msym32 -fPIC -march=xlp
else
CFLAGS += -mabi=32 -march=xlp
LDFLAGS += -mabi=32 -march=xlp
endif
endif

CONFIG_CONFIG_IN = $(top_srcdir)/sysdeps/$(TARGET_OS)/Config.in
CONFIG_DEFCONFIG = $(top_srcdir)/sysdeps/$(TARGET_OS)/defconfig

ALL_DIRS:= $(DIRS) scripts/config
ALL_MAKEFILES:=$(patsubst %,%/Makefile,$(ALL_DIRS))

ifeq ($(KBUILD_SRC),)

ifdef O
  ifeq ("$(origin O)", "command line")
    KBUILD_OUTPUT := $(O)
  endif
endif

# That's our default target when none is given on the command line
.PHONY: _all
_all:

ifneq ($(KBUILD_OUTPUT),)
# Invoke a second make in the output directory, passing relevant variables
# check that the output directory actually exists
saved-output := $(KBUILD_OUTPUT)
KBUILD_OUTPUT := $(shell cd $(KBUILD_OUTPUT) && /bin/pwd)
$(if $(wildcard $(KBUILD_OUTPUT)),, \
     $(error output directory "$(saved-output)" does not exist))

.PHONY: $(MAKECMDGOALS)

$(filter-out _all,$(MAKECMDGOALS)) _all: $(KBUILD_OUTPUT)/Rules.mak $(KBUILD_OUTPUT)/Makefile
	$(MAKE) --no-print-directory -C $(KBUILD_OUTPUT) \
	top_srcdir=$(CURDIR) \
	top_builddir=$(KBUILD_OUTPUT) \
	TOPDIR=$(KBUILD_OUTPUT)	\
	KBUILD_SRC=$(CURDIR) \
	-f $(CURDIR)/Makefile $@

$(KBUILD_OUTPUT)/Rules.mak:
	@echo > $@
	@echo top_srcdir=$(CURDIR) >> $@
	@echo top_builddir=$(KBUILD_OUTPUT) >> $@
	@echo include $(top_srcdir)/Rules.mak >> $@

$(KBUILD_OUTPUT)/Makefile:
	@echo > $@
	@echo top_srcdir=$(CURDIR) >> $@
	@echo top_builddir=$(KBUILD_OUTPUT) >> $@
	@echo KBUILD_SRC='$$(top_srcdir)' >> $@
	@echo include '$$(KBUILD_SRC)'/Makefile >> $@

# Leave processing to above invocation of make
skip-makefile := 1
endif # ifneq ($(KBUILD_OUTPUT),)
endif # ifeq ($(KBUILD_SRC),)

ifeq ($(skip-makefile),)

_all: all

ifeq ($(strip $(HAVE_DOT_CONFIG)),y)

all: aruba_conf busybox busybox.links doc

aruba_conf:
ifdef MIPS_XLR_PLATFORM
	@echo "  CONFIG  .config.nebbiolo"
	${__ARUBA_QUIET}cp -f .config.nebbiolo .config
endif
ifdef MIPS_XLP_PLATFORM
ifdef MILAGRO
	@echo "  CONFIG  .config.milagro"
	${__ARUBA_QUIET}cp -f .config.milagro .config
else
	@echo "  CONFIG  .config.porfidio"
	${__ARUBA_QUIET}cp -f .config.porfidio .config
endif
endif
ifdef X86_TARGET_PLATFORM
	@echo "  CONFIG  .config.x86-target-platform"
	${__ARUBA_QUIET}cp -f .config.x86-target-platform .config
endif        
ifdef AP_PLATFORM
	@echo "  CONFIG  .config.${CONF}${SUFF}"
	${__ARUBA_QUIET}cp -f .config.${CONF}${SUFF} .config
endif

all_tree:	$(ALL_MAKEFILES)

$(ALL_MAKEFILES): %/Makefile: $(top_srcdir)/%/Makefile
	d=`dirname $@`; [ -d "$$d" ] || mkdir -p "$$d"; cp $< $@

# In this section, we need .config
-include $(top_builddir)/.config.cmd
include $(patsubst %,%/Makefile.in, $(SRC_DIRS))
-include $(top_builddir)/.depend

busybox: $(ALL_MAKEFILES) .depend include/config.h $(libraries-y)
	@echo "  EXE     busybox"
	echo $(LIBS_LOCAL_PATH)
	echo $(OPENSSL_DIR)
	echo $(OBJDIR) 
	${__ARUBA_QUIET}$(CC) $(LDFLAGS) -o $@ -Wl,--start-group $(libraries-y) $(INCS_LOCAL) $(LIBRARIES) $(LIBS_LOCAL_PATH) $(LIBS_LOCAL) $(CRYPT_LIB) -Wl,--end-group

busybox.links: $(top_srcdir)/applets/busybox.mkll include/config.h $(top_srcdir)/include/applets.h
	@- $(SHELL) $^ >$@

install: applets/install.sh busybox busybox.links
	$(SHELL) $< $(MYPREFIX)

ifeq ($(strip $(CONFIG_FEATURE_SUID)),y)
	@echo
	@echo
	@echo --------------------------------------------------
	@echo You will probably need to make your busybox binary
	@echo setuid root to ensure all configured applets will
	@echo work properly.
	@echo --------------------------------------------------
	@echo
endif

# The emake parallel build app needs dependencies to determine 
# the proper order in which targets are invoked.
ifdef ARUBA_EMAKE
    arubainstall_deps = busybox
endif

ifndef SOFTAP
arubainstall: applets/install.sh busybox busybox.links
	$(SHELL) $< $(MYPREFIX)
	${__ARUBA_QUIET}$(STRIPCMD) ${DESTDIR}/../bin/busybox
else
arubainstall: $(arubainstall_deps)
	install busybox $(DESTDIR)/../bin
endif


#
# fixMe!
#
cleaninstall:

ifdef X86_TARGET_PLATFORM
savesymbols:
	mkdir -p $(SYMBOLS_DIR)
	cp -a busybox  $(SYMBOLS_DIR)
else
savesymbols:
endif

uninstall: busybox.links
	rm -f $(MYPREFIX)/bin/busybox
	for i in `cat busybox.links` ; do rm -f $(MYPREFIX)$$i; done

install-hardlinks: $(top_srcdir)/applets/install.sh busybox busybox.links
	$(SHELL) $< $(MYPREFIX) --hardlinks

check: busybox
	bindir=$(top_builddir) srcdir=$(top_srcdir)/testsuite \
	$(top_srcdir)/testsuite/runtest

# Documentation Targets
doc: docs/busybox.pod docs/BusyBox.txt docs/BusyBox.1 docs/BusyBox.html

docs/busybox.pod : $(top_srcdir)/docs/busybox_header.pod $(top_srcdir)/include/usage.h $(top_srcdir)/docs/busybox_footer.pod
	@-mkdir -p docs
	@- ( cat $(top_srcdir)/docs/busybox_header.pod; \
	    $(top_srcdir)/docs/autodocifier.pl $(top_srcdir)/include/usage.h; \
	    cat $(top_srcdir)/docs/busybox_footer.pod ) > docs/busybox.pod

docs/BusyBox.txt: docs/busybox.pod
#	@$(SECHO)
#	@$(SECHO) BusyBox Documentation
#	@$(SECHO)
	@-mkdir -p docs
	@-pod2text $< > $@

docs/BusyBox.1: docs/busybox.pod
	@- mkdir -p docs
	@- pod2man --center=BusyBox --release="version $(VERSION)" \
		$< > $@

docs/BusyBox.html: docs/busybox.net/BusyBox.html
	@- mkdir -p docs
	-@ rm -f docs/BusyBox.html
	-@ cp docs/busybox.net/BusyBox.html docs/BusyBox.html

docs/busybox.net/BusyBox.html: docs/busybox.pod
	-@ mkdir -p docs/busybox.net
	@-  pod2html --noindex $< > \
	    docs/busybox.net/BusyBox.html
	-@ rm -f pod2htm*

# The nifty new buildsystem stuff
scripts/mkdep: $(top_srcdir)/scripts/mkdep.c
	@echo "  HOSTCC  mkdep"
	${__ARUBA_QUIET}$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

scripts/split-include: $(top_srcdir)/scripts/split-include.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

.depend: scripts/mkdep include/config.h
	@echo "  MKDEP"
	${__ARUBA_QUIET}rm -f .depend .hdepend;
	${__ARUBA_QUIET}mkdir -p include/config;
	${__ARUBA_QUIET}scripts/mkdep -I include -- \
	  `find $(top_srcdir) -name \*.c -print | sed -e "s,^./,,"` >> .depend;
	${__ARUBA_QUIET}scripts/mkdep -I include -- \
	  `find $(top_srcdir) -name \*.h -print | sed -e "s,^./,,"` >> .hdepend;

depend dep: .depend

include/config/MARKER: depend scripts/split-include
	scripts/split-include include/config.h include/config
	@ touch include/config/MARKER

include/config.h: .config
	@if [ ! -x $(top_builddir)/scripts/config/conf ] ; then \
	    $(MAKE) --no-print-directory -C scripts/config conf; \
	fi;
	${__ARUBA_QUIET}$(top_builddir)/scripts/config/conf ${__SILENT}  $(CONFIG_CONFIG_IN)

finished2:
	$(SECHO)
	$(SECHO) Finished installing...
	$(SECHO)

else # ifeq ($(strip $(HAVE_DOT_CONFIG)),y)

all: oldconfig
	$(MAKE) --no-print-directory

# configuration
# ---------------------------------------------------------------------------

$(ALL_MAKEFILES): %/Makefile: $(top_srcdir)/%/Makefile
	d=`dirname $@`; [ -d "$$d" ] || mkdir -p "$$d"; cp $< $@

scripts/config/conf: scripts/config/Makefile Rules.mak
	$(MAKE) --no-print-directory -C scripts/config conf
	-@if [ ! -f .config ] ; then \
		cp $(CONFIG_DEFCONFIG) .config; \
	fi

scripts/config/mconf: scripts/config/Makefile Rules.mak
	$(MAKE) --no-print-directory -C scripts/config ncurses conf mconf
	-@if [ ! -f .config ] ; then \
		cp $(CONFIG_DEFCONFIG) .config; \
	fi

menuconfig: scripts/config/mconf
	@./scripts/config/mconf $(CONFIG_CONFIG_IN)

config: scripts/config/conf
	@./scripts/config/conf $(CONFIG_CONFIG_IN)

oldconfig: scripts/config/conf
ifdef MIPS_XLR_PLATFORM
	@echo "  CONFIG  .config.nebbiolo"
	${__ARUBA_QUIET}cp -f .config.nebbiolo .config
endif
ifdef MIPS_XLP_PLATFORM
ifdef MILAGRO
	@echo "  CONFIG  .config.milagro"
	${__ARUBA_QUIET}cp -f .config.milagro .config
else
	@echo "  CONFIG  .config.porfidio"
	${__ARUBA_QUIET}cp -f .config.porfidio .config
endif
endif
ifdef X86_TARGET_PLATFORM
	@echo "  CONFIG  .config.x86-target-platform"
	${__ARUBA_QUIET}cp -f .config.x86-target-platform .config
endif        
ifdef AP_PLATFORM
	@echo "  CONFIG  .config.${CONF}"
	${__ARUBA_QUIET}cp -f .config.${CONF}${SUFF} .config
endif
	@./scripts/config/conf ${__SILENT} $(CONFIG_CONFIG_IN)

randconfig: scripts/config/conf
	@./scripts/config/conf -r $(CONFIG_CONFIG_IN)

allyesconfig: scripts/config/conf
	@./scripts/config/conf -y $(CONFIG_CONFIG_IN)
	sed -i -e "s/^CONFIG_DEBUG.*/# CONFIG_DEBUG is not set/" .config
	sed -i -e "s/^USING_CROSS_COMPILER.*/# USING_CROSS_COMPILER is not set/" .config
	sed -i -e "s/^CONFIG_STATIC.*/# CONFIG_STATIC is not set/" .config
	sed -i -e "s/^CONFIG_SELINUX.*/# CONFIG_SELINUX is not set/" .config
	@./scripts/config/conf -o $(CONFIG_CONFIG_IN)

allnoconfig: scripts/config/conf
	@./scripts/config/conf -n $(CONFIG_CONFIG_IN)

defconfig: scripts/config/conf
	@./scripts/config/conf -d $(CONFIG_CONFIG_IN)

cleaninstall:

clean:
	@echo "  CLEAN   busybox"
	${__ARUBA_QUIET}- rm -f docs/busybox.dvi docs/busybox.ps \
	    docs/busybox.pod docs/busybox.net/busybox.html \
	    docs/busybox pod2htm* *.gdb *.elf *~ core .*config.log \
	    docs/BusyBox.txt docs/BusyBox.1 docs/BusyBox.html \
	    docs/busybox.net/BusyBox.html busybox.links libbb/loop.h \
	    .config.old .hdepend busybox
	${__ARUBA_QUIET}- rm -rf _install
	${__ARUBA_QUIET}- find . -name .\*.flags -exec rm -f {} \;
	${__ARUBA_QUIET}- find . -name \*.o -exec rm -f {} \;
	${__ARUBA_QUIET}- find . -name \*.a -exec rm -f {} \;
	${__ARUBA_QUIET}- rm -f Config.h
	${__ARUBA_QUIET}- rm -f scripts/split-include scripts/mkdep
	${__ARUBA_QUIET}- rm -rf include/config include/config.h
	${__ARUBA_QUIET}- find . -name .depend -exec rm -f {} \;
	${__ARUBA_QUIET}- rm -f .config .config.old .config.cmd
	${__ARUBA_QUIET}- $(MAKE) --no-print-directory -C scripts/config clean

distclean: clean
	- rm -f scripts/split-include scripts/mkdep
	- rm -rf include/config include/config.h
	- find . -name .depend -exec rm -f {} \;
	rm -f .config .config.old .config.cmd
	- $(MAKE) --no-print-directory -C scripts/config clean

cleaninstall:

release: distclean #doc
	cd ..; \
	rm -rf $(PROG)-$(VERSION); \
	cp -a busybox $(PROG)-$(VERSION); \
	\
	find $(PROG)-$(VERSION)/ -type d \
		-name .svn \
		-print \
		-exec rm -rf {} \; ; \
	\
	find $(PROG)-$(VERSION)/ -type f \
		-name .\#* \
		-print \
		-exec rm -f {} \; ; \
	\
	tar -cvzf $(PROG)-$(VERSION).tar.gz $(PROG)-$(VERSION)/;

tags:
	ctags -R .


endif # ifeq ($(strip $(HAVE_DOT_CONFIG)),y)

endif # ifeq ($(skip-makefile),)

.PHONY: dummy subdirs release distclean clean config oldconfig \
	menuconfig tags check test depend buildtree
