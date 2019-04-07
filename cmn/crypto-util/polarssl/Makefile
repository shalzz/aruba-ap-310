include $(ARUBA_SRC)/mk/Makefile.top

CFLAGS_LOCAL += -Werror -L.

INCS_LOCAL += -I$(CYASSL_DIR) -I$(CYASSL_DIR)/cyassl -Iinclude/
LIBS_LOCAL += -lcyassl
LIBS_LOCAL_PATH += -L$(CYASSL_DIR)/src/.libs

ifdef FAT_AP
CFILES= convert_cert.c
else
CFILES= base64.c bignum.c md5.c sha1.c sha2.c sha4.c des.c aes.c rsa.c md.c md_wrap.c convert_cert.c
endif

APPLICATION= convert_cert 

include $(ARUBA_SRC)/mk/Makefile.bottom

arubainstall:
	    $(INSTALL) $(OBJDIR)$(APPLICATION) $(DESTDIR)/../aruba/bin 
		$(STRIP) $(DESTDIR)/../aruba/bin/$(APPLICATION)
