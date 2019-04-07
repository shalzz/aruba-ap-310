include $(ARUBA_SRC)/mk/Makefile.top

CFLAGS_LOCAL += -DNO_GETTEXT -DCONFIG_ARUBA_NET -O0 -g -Wall -W -c -I$(ARUBA_SRC) \
			-I$(ARUBA_SRC)/utils/dnsmasq-2.55

CFILES= dhcp-helper_ch.c raw.c

NO_INSTALL= 1

APPLICATION = dhcp-helper

include $(ARUBA_SRC)/mk/Makefile.bottom

INSTALL = /usr/bin/install

arubainstall:
	$(INSTALL) $(OBJDIR)$(APPLICATION)  $(DESTDIR)/../usr/sbin
	$(STRIP) $(DESTDIR)/../usr/sbin/$(APPLICATION)

cleaninstall:
	-rm -f $(DESTDIR)/../usr/sbin/$(APPLICATION)
