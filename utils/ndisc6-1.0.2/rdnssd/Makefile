include $(ARUBA_SRC)/mk/Makefile.top

LIBCOMPAT=compat
CFLAGS_LOCAL += -Werror -L.
CFLAGS_LOCAL += -DHAVE_CONFIG_H -D__aruba_strverscmp__
INCS_LOCAL := -I.. -I../include -I${LIBCOMPAT}
LIBS_LOCAL_PATH := -L${LIBCOMPAT}/${OBJDIR}
LIBS_LOCAL = -lcompat

CFILES = icmp.c netlink.c rdnssd.c 

APPLICATION = rdnssd

include $(ARUBA_SRC)/mk/Makefile.bottom

arubainstall:
	@mkdir -p ${DESTDIR}/../var/run/rdnssd
	${INSTALL} merge-hook ${DESTDIR}/bin/merge-hook

cleaninstall:
	rm -f ${DESTDIR}/bin/${APPLICATION}
