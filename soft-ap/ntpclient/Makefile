include $(ARUBA_SRC)/mk/Makefile.top

ifdef FAT_AP
SECTIONS= NTPC
else
SECTIONS= NTPC ADJ
endif

ifdef NTPC
INCS_LOCAL += -I$(ARUBA_SRC)/cmn	
CFILES=  ntpclient.c phaselock.c

APPLICATION= ntpclient
endif

ifdef ADJ
CFILES= adjtimex.c

APPLICATION= adjtimex
endif

include $(ARUBA_SRC)/mk/Makefile.bottom

