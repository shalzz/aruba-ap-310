include $(ARUBA_SRC)/mk/Makefile.top

# - local defintions.
ifdef APSIM
CFLAGS_LOCAL += -DSHELLPATH=\"$(run_prefix)/bin/sh\"
else
ifdef APSIM_LITE
CFLAGS_LOCAL += -DSHELLPATH=\"$(run_prefix)/bin/sh\"
else
CFLAGS_LOCAL += -DSHELLPATH=\"$(run_prefix)/bin/sh\" -Werror
endif
endif
ifneq ("","$(DEBUG)")
CFLAGS_LOCAL  += -DDEBUG -g
endif

# Disabling the Dynamic Support Password feature with the flag
ifdef PRODUCTION_IMAGE 
CFLAGS_LOCAL += -DPRODUCTION_IMAGE 
endif
ifdef MANUFACTURING_IMAGE 
CFLAGS_LOCAL += -DMANUFACTURING_IMAGE 
endif
ifdef FIPS
CFLAGS_LOCAL += -DFIPS
endif

INCS_LOCAL := -I$(ARUBA_SRC)/cmn/util -I$(ARUBA_SRC)/cmn -I$(OPENSSL_DIR)/include -I$(ARUBA_SRC)/cmn/include \
			 $(MINCDIRS) -I$(ARUBA_SRC)

ifndef AP_PLATFORM
ifndef SOFTAP
LIBS_LOCAL_PATH += -L$(OPENSSL_DIR)/$(OBJDIR)
LIBS_LOCAL += -lcmnutil -lcrypto -lph -lssl -lcliauth
#dependencies for cliauth
LIBS_LOCAL += -ldispatcher -lMHApi -lutilmgmt

endif
endif

# - files
#CFILES= utelnetd_ssh.c
CFILES= utelnetd.c

NO_INSTALL= 1
APPLICATION = utelnetd

include $(ARUBA_SRC)/mk/Makefile.bottom

ifndef APSIM
ifdef AP_PLATFORM
INSTALL = /usr/bin/install

arubainstall:
	$(INSTALL) $(OBJDIR)$(APPLICATION)  $(DESTDIR)/../sbin
	$(STRIP) $(DESTDIR)/../sbin/$(APPLICATION)

cleaninstall:
	-rm -f $(DESTDIR)/../sbin/$(APPLICATION)
else
ifdef SOFTAP
DESTDIR = $(ARUBA_SRC)/soft-ap/Image_final
INSTALL = /usr/bin/install

arubainstall:
	$(INSTALL) $(OBJDIR)$(APPLICATION)  $(DESTDIR)/sbin
	$(STRIP) $(DESTDIR)/sbin/$(APPLICATION)

cleaninstall:
	-rm -f $(DESTDIR)/sbin/$(APPLICATION)
else
arubainstall:
	$(INSTALL) $(OBJDIR)$(APPLICATION)  $(DESTDIR)/../sbin
	$(STRIP) $(DESTDIR)/../sbin/$(APPLICATION)

cleaninstall:
	-rm -f $(DESTDIR)/../sbin/$(APPLICATION)
endif
endif
else
arubainstall:
	$(INSTALL) $(OBJDIR)$(APPLICATION)  $(DESTDIR)/../sbin
	$(STRIP) $(DESTDIR)/../sbin/$(APPLICATION)

cleaninstall:
	-rm -f $(DESTDIR)/../sbin/$(APPLICATION)
endif
