include $(ARUBA_SRC)/mk/Makefile.top

CFLAGS_LOCAL += -DNO_GETTEXT -Wall -W -c -I$(ARUBA_SRC)

CFILES= cache.c rfc1035.c util.c option.c forward.c log.c network.c \
       dnsmasq.c dhcp.c lease.c rfc2131.c netlink.c dbus.c bpf.c helper.c tftp.c \
       raw.c unescape_c_string.c

NO_INSTALL= 1

APPLICATION = dnsmasq

include $(ARUBA_SRC)/mk/Makefile.bottom

INSTALL = /usr/bin/install

arubainstall:
	$(INSTALL) $(OBJDIR)$(APPLICATION)  $(DESTDIR)/../usr/sbin
	$(STRIP) $(DESTDIR)/../usr/sbin/$(APPLICATION)
ifdef SWITCH_PLATFORM 
	$(INSTALL) dnsmasq.conf.switch  $(DESTDIR)/../etc/dnsmasq.conf
else
	$(INSTALL) dnsmasq.conf  $(DESTDIR)/../etc
	$(INSTALL) dnsmasq_172.conf  $(DESTDIR)/../etc/dnsmasq_172.conf
endif

cleaninstall:
	-rm -f $(DESTDIR)/../usr/sbin/$(APPLICATION)
