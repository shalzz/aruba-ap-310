# vim: set ts=8: */
# -*- c-file-style:"gnu"; indent-tabs-mode:nil; tabstop:8 -*- 
#<S-Insert>
#
# Please go through ${ARUBA_SRC}/tools/README if this is the first
# time you are about to touch this Makefile for a new platform.
#

#
# Top Dir (so that we can pick platform specific tool definitions)
#
include ${ARUBA_SRC}/mk/core/path-vars
include ${ARUBA_SRC}/mk/core/aruba-image-type

export ARUBA_SRC := $(shell cd .. && pwd)
export TOOLS_DIR := $(ARUBA_SRC)/tools

#
# Include all the (various) platforms for which things need to be built
# The convention is first Controllers and then Access Point platforms
#
PLAT_ENV_LIST =	CORVINA GRENACHE MSWITCH NEBBIOLO PORFIDIO MILAGRO

# Get Platform Type
PLATF=${ARUBA_IMAGE_TYPE}


###############################################################
#                                                             #
#                  PLATFORM SPECIFIC SECTION                  #
#                                                             #
###############################################################

#
# Define your specific platform in here (basically invoke 'make' as):
#    make [all | install | clean | distclean ] <platform>=1
#

#
# PLATLIBS: Libraries/Tools that are platform-dependent and may not
# be required/present on all architectures
#
PLATLIBS  +=

#
# Find the Target Platform type:
# Define the Target-type for the platform
# and other associated platform-specific
# options for which the libraries are being
# built. For more information, please visit
# ${ARUBA_SRC}/tools/README
#

PLATFS=${ARUBA_SRC}/${PLATF}fs
ifeq "$(findstring $(PLATF), corvinagrenachenebbioloporfidiomilagro)" "$(PLATF)"
TARGET = mips
ifeq (${PLATF}, milagro)
PLATF_CFLAGS=" -DMIPS_XLP_PLATFORM -fPIC -mabi=32 -march=xlp"
ABI_CFLAGS=" -mabi=32 -march=xlp"
ABI_LDFLAGS=" -mabi=32 -march=xlp"
else 
ifeq (${PLATF}, porfidio)
PLATF_CFLAGS=" -DMIPS_XLP_PLATFORM -fPIC -mabi=32 -march=xlp"
ABI_CFLAGS=" -mabi=32 -march=xlp"
ABI_LDFLAGS=" -mabi=32 -march=xlp"
else
ifeq (${PLATF}, nebbiolo)
PLATFS=${ARUBA_SRC}/nebfs
else
ifeq ($(PLATF), mswitch)
PLATLIBS = 	e2fsprogs-1.36
TARGET = ppc
else
$(error Error: Atleast one platform should be enabled from CORVINA, GRENACHE, MSWITCH, NEBBIOLO, PORFIDIO & MILAGRO);
endif
endif
endif
endif
endif

ifeq ($(PLATF), shumway)
PLATF_CFLAGS=" -fPIC"
TARGET = x86_64
PLATF_CFLAGS=" -DX86_TARGET_PLATFORM -fPIC"
PLATF_CXXFLAGS=" -DX86_TARGET_PLATFORM -fPIC"
endif

ifeq ($(PLATF), opusone)
PLATF_CFLAGS=" -fPIC"
TARGET = x86_64
endif

# Info to the user
ifeq ("$(TARGET)","")
$(error Error: Target is undefined for Platform: ${PLATF});
else
$(info Platform: ${PLATF}, Target: ${TARGET}, Goals: ${MAKECMDGOALS});
endif

#
# Find the Host Platform type
#
HOST_EXTN = -linux-gnu
HOST_ARCH = $(shell uname -i)

ifneq "$(findstring 86, $(HOST_ARCH))" ""
HOST_PLATF = i386
else
HOST_PLATF = $(HOST_ARCH)
endif
HOST = $(HOST_PLATF)$(HOST_EXTN)

ifeq "$(HOST)" ""
$(error Error: Host platform undefined);
endif

#
# FIPS Compliance
#
ifneq "$(FIPS)" ""
OSSL_DIR := "${ARUBA_SRC}/openssl-f"
F_TAG    := "fips."
else
OSSL_DIR := "${ARUBA_SRC}/openssl"
F_TAG    := ""
endif

include  $(ARUBA_SRC)/mk/Makefile.toolsdefs
# handle SYSTEM_LIBS_PATH with multiple directories
Z := $(strip ${SYSTEM_LIBS_PATH})
export Z
ARUBA_LIBS_PATH := $(shell echo -n -L; echo ${Z} | sed 's/ / -L/g')

ifeq (${PLATF}, porfidio)
        export LD_LIBRARY_PATH=${ARUBA_TOOLS}/i686-pc-linux-gnu/mips64-nlm-linux/lib
endif

ifeq (${PLATF}, milagro)
        export LD_LIBRARY_PATH=${ARUBA_TOOLS}/i686-pc-linux-gnu/mips64-nlm-linux/lib
endif

###############################################################
#                                                             #
#                 PLATFORM INDEPENDENT SECTION                #
#                                                             #
###############################################################

#
# COMMONLIBS: Libraries/Tools that are required for all platforms
# and architectures.
#
ifdef GNUTOOLS
COMMONLIBS =		\
        json-c-0.11     \
        rng-tools-4     \
	rsync-3.0.9	\
	curl-7.26.0
#	ncftp-3.1.5	\
	freetds-0.61.2	\
	lsof_4.74_src 	\
	mysql-3.22.32	\
        libxml2-2.7.8   \
	zlib-1.1.4

endif

# NOTE: 
# HTTPD should be built after the zlib as the former depends
# on the latter. Also, HTTPD depends on whole lot of ArubaOS
# stuff. 
# TODO:
# Delink the dependency of HTTPD on ArubaOS libraries (esp.
# syslog libraries)
ifdef ARUBAHTTPD
COMMONLIBS =		\
	zlib-1.1.4	\
	pcre-8.31	\
	httpd-2.4.3
ifdef FIPS
ifeq (${PLATF}, porfidio)
	LD_LIBRARY_PATH=${KERNEL_TOOLS}/i686-pc-linux-gnu/mips64-nlm-linux/lib
endif
ifeq (${PLATF}, milagro)
	LD_LIBRARY_PATH=${KERNEL_TOOLS}/i686-pc-linux-gnu/mips64-nlm-linux/lib
endif
# Abhishek: as a part of sync from services-ctrl to cluster branch
ifeq (${PLATF}, shumway)
	LD_LIBRARY_PATH=${KERNEL_TOOLS}/i686-pc-linux-gnu/mips64-nlm-linux/lib
endif
# AbhishekEnd: as a part of sync from services-ctrl to cluster branch
endif
endif

ifdef LINUXUTILS
COMMONLIBS =		\
	util-linux-2.12q
endif

#
# CONFIGURE: options for GNU TOOLS
#
ifdef GNUTOOLS
e2fsprogs-1.36-conf-opts    =	--enable-elf-shlibs --enable-jbd-debug 	\
			--prefix=$(ARUBA_SRC)/tools/e2fsprogs-1.36/${TARGET}
freetds-0.61.2-conf-opts    =	--with-tdsver=8.0 --build=${HOST} --host=${TARGET} \
			--prefix=$(ARUBA_SRC)/tools/${PLATF}_obj
lsof_4.74_src-conf-opts     =	-n linux
ifeq ($(PLATF), shumway)
mysql-3.22.32-conf-opts     =	--enable-large-files --enable-static 	\
			--enable-shared --host=${HOST} 			\
			--localstatedir=/flash/mysql 			\
			--prefix=${ARUBA_SRC}/tools/${PLATF}_obj/mysql	\
			--target=${TARGET} --without-curses 		\
			--with-client-ldflags=" $(ARUBA_LIBS_PATH) "	\
			--without-termcap
else			
mysql-3.22.32-conf-opts     =	--disable-large-files --disable-static 	\
			--enable-shared --host=${HOST} 			\
			--localstatedir=/flash/mysql 			\
			--prefix=${ARUBA_SRC}/tools/${PLATF}_obj/mysql	\
			--target=${TARGET} --without-curses 		\
			--with-client-ldflags=" $(ARUBA_LIBS_PATH) "	\
			--without-termcap
endif			
libxml2-2.7.8-conf-opts	    =	--host=${TARGET}${HOST_EXTN} 	\
				--target=${TARGET}${HOST_EXTN} 	\
				--with-ssl=${OSSL_DIR} 		\
				--with-random=/dev/urandom 	\
				--without-zlib --enable-shared  \
				--prefix=${ARUBA_SRC}/tools/${PLATF}_obj
curl-7.26.0-conf-opts	    =	--host=${TARGET}${HOST_EXTN} 	\
				--target=${TARGET}${HOST_EXTN} 	\
				--with-ssl=${OSSL_DIR} 		\
				--with-random=/dev/urandom 	\
				--without-zlib --enable-shared  \
				--prefix=${ARUBA_SRC}/tools/${PLATF}_obj
ncftp-3.1.5-conf-opts	    =	--target=${TARGET}		\
				--prefix=${ARUBA_SRC}/tools/${PLATF}_obj
zlib-1.1.4-conf-opts        = 	--prefix=$(ARUBA_SRC)/tools/${PLATF}_obj ##--shared

rng-tools-4-conf-opts	    =	--host=${TARGET}${HOST_EXTN} 	\
				--target=${TARGET}${HOST_EXTN}  \
				--prefix=${ARUBA_SRC}/tools/${PLATF}_obj
rsync-3.0.9-conf-opts	    =	--host=${TARGET}${HOST_EXTN}    \
				--target=${TARGET}${HOST_EXTN}  \
				--prefix=${ARUBA_SRC}/tools/${PLATF}_obj
json-c-0.11-conf-opts	    =	--host=${TARGET}${HOST_EXTN} 	\
				--target=${TARGET}${HOST_EXTN}  \
				--prefix=${ARUBA_SRC}/tools/${PLATF}_obj --enable-shared

endif

#
# CONFIGURE: options for HTTPD
# NOTE: HTTPD has a dependency on the ZLIB. So, in absence of zlib
#       being available, create the same
#
ifdef ARUBAHTTPD
httpd-2.0.52-conf-opts      =	--build=${HOST} --host=${TARGET}-linux-gnu  \
			--target=${TARGET}-linux-gnu --enable-ssl 	    \
			--with-ssl=${OSSL_DIR} --with-mpm=worker 	    \
			--enable-rewrite --enable-proxy-http		    \
			--with-module=aaa:aaa/mod_aruba_auth.c 		    \
			--prefix=/mswitch/apache --enable-modules=deflate   \
			--with-program-name=httpd.${F_TAG}${PLATF}	    \
			--with-z=${ARUBA_SRC}/tools/${PLATF}_obj 	    \
			--enable-proxy --enable-proxy-connect \
			--with-expat=${httpd-2.0.52-path}/httpd-2.0.52/srclib/apr-util/xml/expat

httpd-2.4.3-conf-opts      =	--build=i386-linux-gnu --host=${TARGET}-linux-gnu  \
			--target=${TARGET}-linux-gnu --enable-ssl 	    \
			--with-ssl=${OSSL_DIR} --with-mpm=worker 	    \
			--enable-rewrite --enable-proxy-http \
			--enable-proxy --enable-proxy-connect \
			--enable-mods-static="mime access_compat dir log_config alias negotiation rewrite auth ssl socache_shmcb deflate filter setenvif authz_core unixd aruba_auth cgid status headers proxy proxy_ftp proxy_ajp proxy_http proxy_connect"       \
			--with-included-apr   \
			--with-pcre=${pcre-8.31-path}/pcre-8.31/pcre-config    \
			--with-module=aaa:aaa/mod_aruba_auth.c 		    \
			--prefix=/mswitch/apache   \
			--with-z=${ARUBA_SRC}/tools/${PLATF}_obj

zlib-1.1.4-conf-opts        = 	--prefix=$(ARUBA_SRC)/tools/${PLATF}_obj/

util-linux-2.12q-conf-opts  =	--prefix=$(ARUBA_SRC)/tools/${PLATF}_obj/

pcre-8.31-conf-opts         =   --host=${TARGET}-linux-gnu --target=${TARGET}-linux-gnu --build=i386-linux-gnu --disable-cpp --prefix=$(ARUBA_SRC)/tools/${PLATF}_obj
endif

ifdef LINUXUTILS
util-linux-2.12q-conf-opts  =	--prefix=$(ARUBA_SRC)/tools/${PLATF}_obj/
endif


#
# Environment Variables: EXPORTs
#
ifdef GNUTOOLS
freetds-0.61.2-exports  =  ac_cv_func_getpwuid_r=five LTCONFIG_VERSION=1.4a \
                           CFLAGS=${ABI_CFLAGS}  \
                           LDFLAGS=${ABI_LDFLAGS} 

ifeq ($(PLATF), shumway)
mysql-3.22.32-exports   =  ac_cv_sys_restartable_syscalls=no    \
                           ac_cv_conv_longlong_to_float=yes     \
                           ac_cv_c_bigendian=no                 \
                           CXX=$(ARUBA_TOOLS_PREFIX)g++         \
                           CCLD=$(ARUBA_TOOLS_PREFIX)gcc        \
                           CCC=$(ARUBA_TOOLS_PREFIX)cpp         \
                           CFLAGS="-DHAVE_ERRNO_AS_DEFINE"${PLATF_CFLAGS}${ABI_CFLAGS}\
                           LDFLAGS=${ABI_LDFLAGS}               \
                           CPPFLAGS="-g"${PLATF_CFLAGS}         \
			   CXXFLAGS=${PLATF_CXXFLAGS}
else
mysql-3.22.32-exports   =  ac_cv_sys_restartable_syscalls=no    \
                           ac_cv_conv_longlong_to_float=yes     \
                           ac_cv_c_bigendian=yes                \
                           CXX=$(ARUBA_TOOLS_PREFIX)g++         \
                           CCLD=$(ARUBA_TOOLS_PREFIX)gcc        \
                           CCC=$(ARUBA_TOOLS_PREFIX)cpp         \
                           LD=$(ARUBA_LD)                       \
                           CFLAGS="-DHAVE_ERRNO_AS_DEFINE"${PLATF_CFLAGS}${ABI_CFLAGS}\
                           LDFLAGS=${ABI_LDFLAGS}               \
                           CPPFLAGS="-g"${PLATF_CFLAGS}         \
			   CXXFLAGS=${PLATF_CXXFLAGS}
endif			   

libxml2-2.7.8-exports	= LD=$(ARUBA_LD)\
			  CFLAGS="-I${OSSL_DIR}/include/openssl	\
			  -I${OSSL_DIR}/include -Wno-error 	\
			  -I${OSSL_DIR}"${ABI_CFLAGS}		\
			  LDFLAGS="-L${OSSL_DIR} -lssl -lcrypto"${ABI_LDFLAGS} \
			  ac_cv_func_strtoll=no

curl-7.26.0-exports	= LD=$(ARUBA_LD)\
			  CFLAGS="-I${OSSL_DIR}/include/openssl	\
			  -I${OSSL_DIR}/include -Wno-error 	\
			  -I${OSSL_DIR}"${ABI_CFLAGS}		\
			  LDFLAGS="-L${OSSL_DIR} -lssl -lcrypto"${ABI_LDFLAGS} \
			  ac_cv_func_strtoll=no

ncftp-3.1.5-exports	= CPPFLAGS="-g"${ABI_CFLAGS}		\
			  CFLAGS=${PLATF_CFLAGS}${ABI_CFLAGS}	\
			  LDFLAGS=${ABI_LDFLAGS}		\
			  ac_cv_func_setpgrp_void=no		\
			  ac_cv_func_setvbuf_reversed=no

zlib-1.1.4-exports	= CFLAGS=${PLATF_CFLAGS}${ABI_CFLAGS}	\
			  LDFLAGS=${ABI_LDFLAGS}

json-c-0.11-exports	= CFLAGS=${ABI_CFLAGS} 			\
			  LDFLAGS=${ABI_LDFLAGS}                \
                          ac_cv_func_malloc_0_nonnull=yes

rng-tools-4-exports	= CFLAGS=${ABI_CFLAGS} 			\
			  LDFLAGS=${ABI_LDFLAGS}                \
			  ac_cv_func_strtoll=no

rsync-3.0.9-exports	= CFLAGS=${ABI_CFLAGS}                  \
			  LDFLAGS=${ABI_LDFLAGS}                \
			  ac_cv_func_strlcat=yes		\
			  ac_cv_func_strlcpy=yes  

util-linux-2.12q-exports= CFLAGS=${ABI_CFLAGS} 			\
			  LDFLAGS=${ABI_LDFLAGS}

libxml2-2.7.8-path	=  ${ARUBA_SRC}/utils/

curl-7.26.0-path	=  ${ARUBA_SRC}/utils/

json-c-0.11-path	=  ${ARUBA_SRC}/utils/

rng-tools-4-path	=  ${ARUBA_SRC}/utils/

rsync-3.0.9-path	=  ${ARUBA_SRC}/utils/

ncftp-3.1.5-path	=  ${ARUBA_SRC}/utils/
endif

ifdef ARUBAHTTPD
httpd-2.0.52-exports    =  OPENSSL_DIR="${OSSL_DIR}" 		\
		 CCLD="$(ARUBA_CC)" 	  			\
		 CFLAGS="-I${ARUBA_SRC}/mysql/include 		\
		 -I${ARUBA_SRC}/cmn/syslog -I${ARUBA_SRC}/cmn	\
		 -I${OSSL_DIR}/include/openssl -DHAVE_SYSLOG 	\
		 -I${OSSL_DIR}/include/ -DLOCATION_CODE_DEPRECATED=\
		 -I${ARUBA_SRC}/arci/cgi -Wno-error"${ABI_CFLAGS}\
		 LDFLAGS="${ARUBA_LIBS_PATH}			\
		 -L${ARUBA_SRC}/tools/${PLATF}_obj/lib 		\
		 -lmysqlclient -L${OSSL_DIR} -lm -lz 		\
		 -L${ARUBA_SRC}/tools/${PLATF}_obj/mysql/lib/mysql\
		 -L${ARUBA_SRC}/usr/lib/aruba/${PLATF}_obj"${ABI_LDFLAGS} 

httpd-2.4.3-exports    =  OPENSSL_DIR="${OSSL_DIR}" 		\
		 CCLD="$(ARUBA_CC)" 	  			\
		 CFLAGS="-g -I${ARUBA_SRC}/mysql/include 		\
		 -I${ARUBA_SRC}/cmn/syslog -I${ARUBA_SRC}/cmn	\
		 -I${OSSL_DIR}/include/openssl -DHAVE_SYSLOG 	\
		 -I${OSSL_DIR}/include/ -DLOCATION_CODE_DEPRECATED= \
		 -DDEFAULT_CONNECT_ATTEMPTS=50 \
		 -DDEFAULT_CGID_LISTENBACKLOG=200 \
		 -I${ARUBA_SRC}/arci/cgi -Wno-error"${ABI_CFLAGS}\
		 ac_cv_file__dev_zero=yes \
		 ac_cv_func_setpgrp_void=no \
		 ac_cv_define_PTHREAD_PROCESS_SHARED=no \
		 ac_cv_func_pthread_mutexattr_setpshared=no \
		 apr_cv_tcp_nodelay_with_cork=yes \
		 ac_cv_o_nonblock_inherited=no \
		 ac_cv_sizeof_pid_t=4 \
		 ap_cv_void_ptr_lt_long=no \
		 ac_cv_sizeof_struct_iovec=1 \
		 LDFLAGS="${ARUBA_LIBS_PATH}			\
		 -L${ARUBA_SRC}/tools/${PLATF}_obj/lib 		\
		 -L${ARUBA_SRC}/tools/jansson/jansson-2.5/x86_64/lib \
		 -L$(ARUBA_SRC)/utils/curl-7.26.0/arubainstall/xlr \
		 -L$(ARUBA_SRC)/tools/mongo-c-driver/mongo-c-driver-0.94.2/lib \
		 -lpthread -ltremautils -lmongoc-1.0 -lbson-1.0 -laruba_sdn -lsdnsdk -lofcclisvcsclient -lsdwancmn -lsdwandal -lcurl -lsyslog -loem -larci -ljansson -lcmnutil -lcrypto -lph -lssl -lpdf -lsig -lboc -lMHApi -ldispatcher -lwifi -lpubsub -lmysqlclient  -L${OSSL_DIR} -lm -lz \
		 -L${ARUBA_SRC}/tools/${PLATF}_obj/mysql/lib/mysql\
		 -L${ARUBA_SRC}/tools/mipscross-xlp/lib		\
		 -L${ARUBA_SRC}/usr/lib/aruba/${PLATF}_obj"${ABI_LDFLAGS} 

pcre-8.31-exports    =  OPENSSL_DIR="${OSSL_DIR}" 		\
		 CCLD="$(ARUBA_CC)" 	  			\
		 CFLAGS="-I${ARUBA_SRC}/mysql/include 		\
		 -I${ARUBA_SRC}/cmn/syslog -I${ARUBA_SRC}/cmn	\
		 -I${OSSL_DIR}/include/openssl -DHAVE_SYSLOG 	\
		 -I${OSSL_DIR}/include/ -DLOCATION_CODE_DEPRECATED=\
		 -I${ARUBA_SRC}/arci/cgi -Wno-error"${ABI_CFLAGS}\
		 LDFLAGS="${ARUBA_LIBS_PATH}			\
		 -L${ARUBA_SRC}/tools/${PLATF}_obj/lib 		\
		 -lmysqlclient -L${OSSL_DIR} -lm -lz 		\
		 -L${ARUBA_SRC}/tools/${PLATF}_obj/mysql/lib/mysql\
		 -L${ARUBA_SRC}/tools/mipscross-xlp/lib		\
		 -L${ARUBA_SRC}/usr/lib/aruba/${PLATF}_obj"${ABI_LDFLAGS}

zlib-1.1.4-exports	= CFLAGS=${ABI_CFLAGS} 			\
			  LDFLAGS=${ABI_LDFLAGS}

httpd-2.0.52-path       =  ${ARUBA_SRC}/apache-cp/sources/

pcre-8.31-path       =  ${ARUBA_SRC}/apache-cp/sources/

httpd-2.4.3-path       =  ${ARUBA_SRC}/apache-cp/sources/

endif

#
# Directories 
#
OBJDIR := $(ARUBA_SRC)/tools/${PLATF}_obj

DIRLIST :=		\
	$(COMMONLIBS)   \
	$(PLATLIBS)     


SUBDIRS  := $(DIRLIST)


AS        := $(ARUBA_AS)
AR        := $(ARUBA_AR)
CC        := $(ARUBA_CC) 
CPP       := $(ARUBA_TOOLS_PREFIX)cpp
RANLIB    := $(ARUBA_RANLIB)	
LD        := $(ARUBA_LD) $(ARUBA_GCC_BASE)
NM        := $(ARUBA_TOOLS_PREFIX)nm
STRIP     := $(ARUBA_STRIP)
OBJCOPY   := $(ARUBA_OBJCOPY)




###############################################################
#                                                             #
#                        RULES & TARGETS                      #
#                                                             #
###############################################################



.DEFAULT_GOAL: all

$(OBJDIR):
	@if test ! -d $@; then mkdir -p $@; fi


.EXPORT_ALL_VARIABLES:

$(SUBDIRS): ${$@-preqs}
ifeq "$(findstring clean, $(MAKECMDGOALS))" ""
	@rm -f ${$@-path}$@/config.cache;
	export NULL ${$@-exports} && \
	if test ! -f ${$@-path}$@/Makefile; then \
		echo -en "${$@-path}$@/Configure ${$@-conf-opts}\n";\
		if test -f ${$@-path}$@/Configure; then \
			cd ${$@-path}$@ && ./Configure ${$@-conf-opts}; \
		elif test -f ${$@-path}$@/configure; then \
			cd ${$@-path}$@ && ./configure ${$@-conf-opts}; \
		else \
			echo "No [Cc]onfigure file(s) found in `pwd`"; \
		fi; \
		$(MAKE); \
		if [ "$@" != "httpd-2.4.3" -a $(shell make -q install 2>&1 | grep -i "No rule" | wc -l) -eq 0 ]; then \
			$(MAKE) install; \
		fi;\
		if [ -f ${$@-path}$@/httpd ]; then \
			mv ${$@-path}$@/httpd httpd.${F_TAG}${PLATF};   \
		fi; \
	else \
		echo "Current directory: `pwd`"; \
		echo -en "${$@-path}$@/Configure ${$@-conf-opts}\n";\
		if [ "${PLATF}" = "opusone" ]; then \
			if [ "$@" = "httpd-2.4.3" ]; then \
				echo "Checking mod_jk directory ..."; \
				if test -d ${$@-path}$@/modules/jk; then \
					if test -f ${$@-path}$@/Configure; then \
						echo "Using Configure with mod_jk ..."; \
						cd ${$@-path}$@ && ./Configure ${$@-conf-opts} --with-mod_jk; \
						echo $?; \
					elif test -f ${$@-path}$@/configure; then \
						echo "Using configure with mod_jk ..."; \
						cd ${$@-path}$@ && ./configure ${$@-conf-opts} --with-mod_jk; \
						echo $?; \
					else \
						echo "No [Cc]onfigure file(s) found in `pwd`"; \
					fi; \
				fi; \
				$(MAKE); \
				if [ "$@" != "httpd-2.4.3" -a $(shell make -q install 2>&1 | grep -i "No rule" | wc -l) -eq 0 ]; then \
					$(MAKE) install; \
				fi;\
				if [ -f ${$@-path}$@/httpd ]; then \
					mv ${$@-path}$@/httpd httpd.${F_TAG}${PLATF};   \
				fi; \
			fi; \
		else \
			echo "Platform not opusone ..."; \
		fi; \
		$(MAKE) -C ${$@-path}$@ $(MAKECMDGOALS) ; \
	fi;
else
ifeq "$(findstring clean, $(MAKECMDGOALS))" "clean"
	@echo "Cleaning objects under $@"
	@if [ -f ${$@-path}$@/Makefile ]; then \
		$(MAKE) -C ${$@-path}$@ $(MAKECMDGOALS); \
	fi;
else
	@echo "Cleaning directory: $@"
	@if [ -f ${$@-path}$@/Makefile ]; then \
		$(MAKE) -C ${$@-path}$@ $(MAKECMDGOALS); \
	fi;
endif
endif

all: subdirs
	@echo "SUBDIRS defined as $(SUBDIRS)"

subdirs: ${SUBDIRS}

gnu:
	@$(MAKE) -C $(TOOLS_DIR) all $(shell echo ${PLATF} | tr [a-z] [A-Z])=1 GNUTOOLS=1

apache:
	@chmod +w $(ARUBA_SRC)/apache-cp/sources/httpd-2.4.3/server/util_expr_scan.c;
	chmod +w $(ARUBA_SRC)/apache-cp/sources/httpd-2.4.3/server/util_expr_parse.h; 
	chmod +w $(ARUBA_SRC)/apache-cp/sources/httpd-2.4.3/server/util_expr_parse.c; 
	chmod +w $(ARUBA_SRC)/apache-cp/sources/pcre-8.31/configure;
	$(MAKE) -C $(TOOLS_DIR) all $(shell echo ${PLATF} | tr [a-z] [A-Z])=1 ARUBAHTTPD=1

gnuclean:
	@$(MAKE) -C $(TOOLS_DIR) distclean $(shell echo ${PLATF} | tr [a-z] [A-Z])=1 GNUTOOLS=1

apacheclean:
	@$(MAKE) -C $(TOOLS_DIR) distclean $(shell echo ${PLATF} | tr [a-z] [A-Z])=1 ARUBAHTTPD=1

install: subdirs

clean: subdirs
	@rm -rf $(OBJDIR);

arubainstall: 
	@$(MAKE) -C $(TOOLS_DIR) $(shell echo ${PLATF} | tr [a-z] [A-Z])=1
	@$(MAKE) -C $(TOOLS_DIR) install $(shell echo ${PLATF} | tr [a-z] [A-Z])=1

distclean: subdirs

cleaninstall: 
	@$(MAKE) -C $(TOOLS_DIR) distclean PORFIDIO=1
	@$(MAKE) -C $(TOOLS_DIR) makeinstall PORFIDIO=1

.PHONY: arubainstall clean cleaninstall distclean gnu apache $(SUBDIRS)
