include $(ARUBA_SRC)/mk/Makefile.top

NTPD_DIR=$(ARUBA_SRC)/utils/ntpd
#NTP_DST_FILE=$(NTPD_DIR)/ntpd
NTPATE_DST_FILE=$(NTPD_DIR)/ntpdate
#NTPQ_DST_FILE=$(NTPD_DIR)/ntpq
NTP_LIB_DST_FILE=$(NTPD_DIR)/libintl.so.8
NTP_SERVICES=$(NTPD_DIR)/ntpd_ap/services
NTP_CONF=$(NTPD_DIR)/ntpd_ap/ntp.conf
NTPDATE_LIB_SUPPORT=0

ifdef OCTOMORE
#NTP_FILE=$(NTPD_DIR)/ntpd_ap/ntpd_GLENMORANGIE
NTPDATE_FILE=$(NTPD_DIR)/ntpd_ap/ntpdate_GLENMORANGIE
#NTPQ_FILE=$(NTPD_DIR)/ntpd_ap/ntpq_GLENMORANGIE
endif

ifdef GLENMORANGIE
#NTP_FILE=$(NTPD_DIR)/ntpd_ap/ntpd_GLENMORANGIE
NTPDATE_FILE=$(NTPD_DIR)/ntpd_ap/ntpdate_GLENMORANGIE
#NTPQ_FILE=$(NTPD_DIR)/ntpd_ap/ntpq_GLENMORANGIE
endif

ifdef ARDMORE
#NTP_FILE=$(NTPD_DIR)/ntpd_ap/ntpd_ARDMORE
NTPDATE_FILE=$(NTPD_DIR)/ntpd_ap/ntpdate_ARDMORE
#NTPQ_FILE=$(NTPD_DIR)/ntpd_ap/ntpq_ARDMORE
endif

ifdef SPRINGBANK
#NTP_FILE=$(NTPD_DIR)/ntpd_ap/ntpd_SPRINGBANK
NTPDATE_FILE=$(NTPD_DIR)/ntpd_ap/ntpdate_SPRINGBANK
#NTPQ_FILE=$(NTPD_DIR)/ntpd_ap/ntpq_SPRINGBANK
endif

ifdef GLENFIDDICH
#NTP_FILE=$(NTPD_DIR)/ntpd_ap/ntpd_SPRINGBANK
NTPDATE_FILE=$(NTPD_DIR)/ntpd_ap/ntpdate_SPRINGBANK
#NTPQ_FILE=$(NTPD_DIR)/ntpd_ap/ntpq_SPRINGBANK
endif

ifdef X4
#NTP_FILE=$(NTPD_DIR)/ntpd_ap/ntpd_X4
NTPDATE_FILE=$(NTPD_DIR)/ntpd_ap/ntpdate_X4
#NTPQ_FILE=$(NTPD_DIR)/ntpd_ap/ntpq_X4
NTPDATE_LIB_SUPPORT=1
endif

ifdef MASTERSON
NTPDATE_FILE=$(NTPD_DIR)/ntpd_ap/ntpdate_MASTERSON
NTPDATE_LIB_SUPPORT=1
endif

include $(ARUBA_SRC)/mk/Makefile.bottom


arubainstall:
	echo "  INST    NTPDATE"
	#rm -rf $(NTP_DST_FILE)
	#cp -f $(NTP_FILE) $(NTP_DST_FILE)
	rm -rf $(NTPATE_DST_FILE)
	cp -f $(NTPDATE_FILE) $(NTPATE_DST_FILE)
	#rm -rf $(NTPQ_DST_FILE)
	#cp -f $(NTPQ_FILE) $(NTPQ_DST_FILE)

ifdef X4
	rm -rf $(NTP_LIB_DST_FILE)
	cp -f $(NTPD_DIR)/ntpd_ap/libintl.so.8.1.2 $(NTP_LIB_DST_FILE)
endif

ifdef MASTERSON
	rm -rf $(NTP_LIB_DST_FILE)
	cp -f $(NTPD_DIR)/ntpd_ap/libintl.so.8.1.4 $(NTP_LIB_DST_FILE)
endif

	echo "  INST    NTPDATE FILE"
	$(INSTALL) $(NTPATE_DST_FILE) $(DESTDIR)/../aruba/bin
	$(STRIP) $(DESTDIR)/../aruba/bin/ntpdate
	$(INSTALL) $(NTP_SERVICES) $(NTP_CONF) $(DESTDIR)/../etc

ifeq ($(NTPDATE_LIB_SUPPORT), 1)
	$(INSTALL) $(NTP_LIB_DST_FILE) $(DESTDIR)/../lib
endif

clean:
	#rm -rf $(DESTDIR)/../aruba/bin/ntpd
	rm -rf $(DESTDIR)/../aruba/bin/ntpdate
	#rm -rf $(DESTDIR)/../aruba/bin/ntpq

ifeq ($(NTPDATE_LIB_SUPPORT), 1)
	rm -rf $(DESTDIR)/../lib/libintl.so.8
endif

