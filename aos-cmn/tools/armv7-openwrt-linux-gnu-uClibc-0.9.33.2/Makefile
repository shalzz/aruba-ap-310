include ${ARUBA_SRC}/mk/Makefile.top

include ${ARUBA_SRC}/mk/Makefile.bottom

#
# Kludge.   The savesymbols target reuses the arubainstall 
# target to save the DSOs and symlinks.  The STRIP is redefined
# to echo to keep symbols.
#
ifneq "$(findstring savesymbols,$(MAKECMDGOALS))" ""
STRIP:=echo
DEST_LIB_DIR=$(SYMBOLS_DIR)/lib
else
DEST_LIB_DIR=$(DESTDIR)/../lib
endif
SDIR=lib
PDIR=lib

savesymbols: arubainstall

arubainstall:
	rm -f ${DEST_LIB_DIR}/ld-uClibc-0.9.33.2.so 
	cp -f ${SDIR}/ld-uClibc-0.9.33.2.so  ${DEST_LIB_DIR}/ld-uClibc-0.9.33.2.so 
	chmod u+w ${DEST_LIB_DIR}/ld-uClibc-0.9.33.2.so; $(STRIP) ${DEST_LIB_DIR}/ld-uClibc-0.9.33.2.so
	rm -f ${DEST_LIB_DIR}/ld-uClibc.so.0
	rm -f ${DEST_LIB_DIR}/ld-uClibc.so
	rm -f ${DEST_LIB_DIR}/ld.so
	rm -f ${DEST_LIB_DIR}/ld.so.0
	rm -f ${DEST_LIB_DIR}/ld.so.1
	cd ${DEST_LIB_DIR};  ln -s ld-uClibc-0.9.33.2.so ld-uClibc.so.0 
	cd ${DEST_LIB_DIR};  ln -s ld-uClibc-0.9.33.2.so ld-uClibc.so
	cd ${DEST_LIB_DIR};  ln -s ld-uClibc-0.9.33.2.so ld.so
	cd ${DEST_LIB_DIR};  ln -s ld-uClibc-0.9.33.2.so ld.so.0
	cd ${DEST_LIB_DIR};  ln -s ld-uClibc-0.9.33.2.so ld.so.1
#ifdef EIGER
	rm -f ${DEST_LIB_DIR}/libstdc++.so.6.0.16
	cp -f ${SDIR}/libstdc++.so.6.0.16  ${DEST_LIB_DIR}/libstdc++.so.6.0.16
	chmod u+w ${DEST_LIB_DIR}/libstdc++.so.6.0.16; $(STRIP) ${DEST_LIB_DIR}/libstdc++.so.6.0.16
	rm -f ${DEST_LIB_DIR}/libstdc++.so
	rm -f ${DEST_LIB_DIR}/libstdc++.so.6
	cd ${DEST_LIB_DIR};  ln -s libstdc++.so.6.0.16 libstdc++.so
	cd ${DEST_LIB_DIR};  ln -s libstdc++.so.6.0.16 libstdc++.so.6
#endif

	rm -f ${DEST_LIB_DIR}/libcrypt-0.9.33.2.so  
	cp -f ${SDIR}/libcrypt-0.9.33.2.so ${DEST_LIB_DIR}/libcrypt-0.9.33.2.so
	chmod u+w ${DEST_LIB_DIR}/libcrypt-0.9.33.2.so; $(STRIP) ${DEST_LIB_DIR}/libcrypt-0.9.33.2.so
	rm -f ${DEST_LIB_DIR}/libcrypt.so.0
	rm -f ${DEST_LIB_DIR}/libcrypt.so
	cd ${DEST_LIB_DIR}; ln -s libcrypt-0.9.33.2.so libcrypt.so.0 
	cd ${DEST_LIB_DIR}; ln -s libcrypt-0.9.33.2.so libcrypt.so

	rm -f ${DEST_LIB_DIR}/libuClibc-0.9.33.2.so 
	cp -f ${SDIR}/libuClibc-0.9.33.2.so ${DEST_LIB_DIR}/libuClibc-0.9.33.2.so 
	chmod u+w ${DEST_LIB_DIR}/libuClibc-0.9.33.2.so; $(STRIP) ${DEST_LIB_DIR}/libuClibc-0.9.33.2.so 
	rm -f ${DEST_LIB_DIR}/libc.so
	rm -f ${DEST_LIB_DIR}/libc.so.0
	cd ${DEST_LIB_DIR}; ln -s libuClibc-0.9.33.2.so libc.so 
	cd ${DEST_LIB_DIR}; ln -s libuClibc-0.9.33.2.so libc.so.0

	rm -f ${DEST_LIB_DIR}/libpthread-0.9.33.2.so 
	cp -f ${SDIR}/libpthread-0.9.33.2.so ${DEST_LIB_DIR}/libpthread-0.9.33.2.so 
	chmod u+w ${DEST_LIB_DIR}/libpthread-0.9.33.2.so; $(STRIP) ${DEST_LIB_DIR}/libpthread-0.9.33.2.so 
	rm -f ${DEST_LIB_DIR}/libpthread.so
	rm -f ${DEST_LIB_DIR}/libpthread.so.0
	cd ${DEST_LIB_DIR}; ln -s libpthread-0.9.33.2.so libpthread.so 
	cd ${DEST_LIB_DIR}; ln -s libpthread-0.9.33.2.so libpthread.so.0

	rm -f ${DEST_LIB_DIR}/libgcc_s.so.1  
#ifdef REGULATORY_TESTING
	cp -f ${PDIR}/libgcc_s.so.1 ${DEST_LIB_DIR}/libgcc_s.so.1  
	chmod u+w ${DEST_LIB_DIR}/libgcc_s.so.1; $(STRIP) ${DEST_LIB_DIR}/libgcc_s.so.1
#endif

	rm -f ${DEST_LIB_DIR}/libgcc_s.so
#ifdef REGULATORY_TESTING
	cd ${DEST_LIB_DIR}; ln -s libgcc_s.so.1 libgcc_s.so
#endif
#ifdef REGULATORY_TESTING
	rm -f ${DEST_LIB_DIR}/librt-0.9.33.2.so 
	rm -f ${DEST_LIB_DIR}/librt.so.0
	cp -f ${SDIR}/librt-0.9.33.2.so  ${DEST_LIB_DIR}/librt-0.9.33.2.so 
	chmod u+w ${DEST_LIB_DIR}/librt-0.9.33.2.so; $(STRIP) ${DEST_LIB_DIR}/librt-0.9.33.2.so
	cd ${DEST_LIB_DIR}; ln -s librt-0.9.33.2.so librt.so.0
#endif

	rm -f ${DEST_LIB_DIR}/libm-0.9.33.2.so   
	cp -f ${SDIR}/libm-0.9.33.2.so ${DEST_LIB_DIR}/libm-0.9.33.2.so   
	chmod u+w ${DEST_LIB_DIR}/libm-0.9.33.2.so; $(STRIP) ${DEST_LIB_DIR}/libm-0.9.33.2.so
	rm -f ${DEST_LIB_DIR}/libm.so
	rm -f ${DEST_LIB_DIR}/libm.so.0
	cd ${DEST_LIB_DIR}; ln -s libm-0.9.33.2.so libm.so 
	cd ${DEST_LIB_DIR}; ln -s libm-0.9.33.2.so libm.so.0

	rm -f ${DEST_LIB_DIR}/libdl-0.9.33.2.so
#ifdef REGULATORY_TESTING
	cp -f ${SDIR}/libdl-0.9.33.2.so ${DEST_LIB_DIR}/libdl-0.9.33.2.so
	chmod u+w ${DEST_LIB_DIR}/libdl-0.9.33.2.so; $(STRIP) ${DEST_LIB_DIR}/libdl-0.9.33.2.so
#endif
	rm -f ${DEST_LIB_DIR}/libdl.so 
	rm -f ${DEST_LIB_DIR}/libdl.so.0
#ifdef REGULATORY_TESTING
	cd ${DEST_LIB_DIR}; ln -s libdl-0.9.33.2.so libdl.so
	cd ${DEST_LIB_DIR}; ln -s libdl-0.9.33.2.so libdl.so.0
#endif

ifdef FAT_AP
	rm -f ${DEST_LIB_DIR}/libutil-0.9.33.2.so
	cp -f ${SDIR}/libutil-0.9.33.2.so ${DEST_LIB_DIR}/libutil-0.9.33.2.so
	chmod u+w ${DEST_LIB_DIR}/libutil-0.9.33.2.so; $(STRIP) ${DEST_LIB_DIR}/libutil-0.9.33.2.so
	rm -f ${DEST_LIB_DIR}/libutil.so
	rm -f ${DEST_LIB_DIR}/libutil.so.0
	cd ${DEST_LIB_DIR}; ln -s libutil-0.9.33.2.so libutil.so
	cd ${DEST_LIB_DIR}; ln -s libutil-0.9.33.2.so libutil.so.0
endif

ifdef AP_DEV_UTILS
	cp -f usr/arm-buildroot-linux-uclibcgnueabi/target_utils/gdbserver ${DESTDIR}/../usr/sbin/
	cp -f usr/arm-buildroot-linux-uclibcgnueabi/target_utils/strace ${DESTDIR}/../usr/sbin/ 
	cp -f usr/arm-buildroot-linux-uclibcgnueabi/target_utils/tcpdump ${DESTDIR}/../usr/sbin/ 
	chmod u+w ${DESTDIR}/../usr/sbin/{gdbserver,strace,tcpdump}
	$(STRIP) ${DESTDIR}/../usr/sbin/{gdbserver,strace,tcpdump}
else
	rm -f ${DESTDIR}/../usr/sbin/{strace,tcpdump}
ifndef FAT_AP
	rm -f ${DESTDIR}/../usr/sbin/gdbserver
endif
endif

ifdef DO_LIBEFENCE
	rm -f ${DEST_LIB_DIR}/libefence.so.0.0
	cp -f usr/arm-buildroot-linux-uclibcgnueabi/sysroot/usr/lib/libefence.so.0.0 ${DEST_LIB_DIR}/libefence.so.0.0
	chmod u+w ${DEST_LIB_DIR}/libefence.so.0.0; 
	rm -f ${DEST_LIB_DIR}/libefence.so.0
	rm -f ${DEST_LIB_DIR}/libefence.so
	cd ${DEST_LIB_DIR}; ln -s libefence.so.0.0 libefence.so.0
	cd ${DEST_LIB_DIR}; ln -s libefence.so.0.0 libefence.so
	echo "Installed libefence from <${UDIR}/libefence.so.0.0> under <${DEST_LIB_DIR}>! TODO-rem"
else
	rm -f ${DEST_LIB_DIR}/libefence.so*
endif


#
# libelf for perf
# 
	rm -f ${DEST_LIB_DIR}/libelf*
	cp -f ${SDIR}/libelf.so.0.8.13 ${DEST_LIB_DIR}/libelf.so.0.8.13
	chmod u+w ${DEST_LIB_DIR}/libelf.so.0.8.13; 
	$(STRIP) ${DEST_LIB_DIR}/libelf.so.0.8.13
	cd ${DEST_LIB_DIR}; ln -s libelf.so.0.8.13 libelf.so.0
	cd ${DEST_LIB_DIR}; ln -s libelf.so.0.8.13 libelf.so
	cd ${DEST_LIB_DIR}; ln -s libelf.so libelf.so.1

	cd ${DEST_LIB_DIR}; chmod +x *.so
#
# Blow that all off!
# 

cleaninstall:
	-rm -f $(DEST_LIB_DIR)/*.so*

#
