include $(ARUBA_SRC)/mk/Makefile.top
CFILES  =  tr6.c
APPLICATION = traceroute6
include $(ARUBA_SRC)/mk/Makefile.bottom
arubainstall:
	$(INSTALL) $(OBJDIR)/traceroute6 $(DESTDIR)/bin;
	$(STRIP) $(DESTDIR)/bin/traceroute6;
cleaninstall:
	-rm -f $(DESTDIR)/bin/traceroute6;
