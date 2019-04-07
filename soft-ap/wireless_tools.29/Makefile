
include $(ARUBA_SRC)/mk/Makefile.top

# Version of the Wireless Tools
WT_VERSION := $(shell sed -ne "/WT_VERSION/{s:\([^0-9]*\)::;p;q;}" < iwlib.h )

# Version of Wireless Extensions.
WE_VERSION := $(shell sed -ne "/WE_VERSION/{s:\([^0-9]*\)::;p;q;}" < iwlib.h )

# Always use local header for wireless extensions
WEXT_HEADER = wireless.$(WE_VERSION).h

#SECTIONS= LIBIW WCONFIG LIST PRIV SPY GETID EVENT
SECTIONS= LIBIW WCONFIG

ifdef QCA_AP
SECTIONS += PRIV
endif

ifdef LIBIW
CFILES= iwlib.c
LIBRARY= iw
endif

ifdef WCONFIG
CFILES= iwconfig.c
ifdef MERLOT
NONIMAGE_APP= iwconfig
else
APPLICATION= iwconfig
endif
LIBS_LOCAL += -liw -lm
endif

ifdef LIST
CFILES= iwlist.c
NONIMAGE_APP= iwlist
LIBS_LOCAL += -liw -lm
endif

ifdef PRIV
CFILES= iwpriv.c
ifdef MERLOT
NONIMAGE_APP= iwpriv
else
APPLICATION= iwpriv
endif
LIBS_LOCAL += -liw -lm
endif

ifdef SPY
CFILES= iwspy.c
NONIMAGE_APP= iwspy
LIBS_LOCAL += -liw -lm
endif

ifdef GETID
CFILES= iwgetid.c
NONIMAGE_APP= iwgetid
LIBS_LOCAL += -liw -lm
endif

ifdef EVENT
CFILES= iwevent.c
NONIMAGE_APP= iwevent
LIBS_LOCAL += -liw -lm
endif


# Do we want to build with or without libm ?
ifdef BUILD_NOLIBM
  LIBS=
  WELIB_FLAG = -DWE_NOLIBM=y
else
  LIBS= -lm
endif

# Other flags
LOCAL_CFLAGS+= -Os -I.
#CFLAGS=-O2 -W -Wall -Wstrict-prototypes -I.

include $(ARUBA_SRC)/mk/Makefile.bottom
