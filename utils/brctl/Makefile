include $(ARUBA_SRC)/mk/Makefile.top

NO_INSTALL=1

# - local defintions.
#CFLAGS_LOCAL += -Wno-error
INCS_LOCAL := -I. 

CFILES= brctl.c brctl_cmd.c libbridge_if.c libbridge_init.c \
		libbridge_misc.c brctl_disp.c libbridge_devif.c

APPLICATION = brctl

LIBS_LOCAL =  -lm
ifndef OCTOMORE
ifndef X4
ifndef GLENMORANGIE
LIBS_LOCAL +=  -lsysfs
endif
endif
endif

include $(ARUBA_SRC)/mk/Makefile.bottom

arubainstall:
	$(INSTALL) $(OBJDIR)brctl $(DESTDIR)/../bin
	$(STRIP) $(DESTDIR)/../bin/brctl

