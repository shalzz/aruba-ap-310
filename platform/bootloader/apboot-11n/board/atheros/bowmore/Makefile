include $(TOPDIR)/config.mk

LIB	= lib$(BOARD).a

OBJS	=			\
	$(BOARD).o		\
	flash.o			\
	../common/init-955x.o	\
	../common/serial.o	\
	../common/ath_pci.o	\
	../common/qca-eth-955x.o \
	../common/ath_watchdog.o \
	../common/ath_misc.o

#
# The files included in EXTRA_OBJS use some functions that are not
# available in the `bootstrap' portion of tuboot.bin resulting in
# compile/link errors. Hence have them separate (i.e. don't include
# into lib$(BOARD).a and include them only in `u-boot' portion of
# tuboot.bin.
#

ETH_CONFIG=_s17
COMPRESSED_UBOOT=0

ifeq ($(strip $(COMPRESSED_UBOOT)),0) #{
OBJS	+= $(BOARD_EXTRA_OBJS_tmp)
else
EXTRA_OBJS = $(BOARD_EXTRA_OBJS_tmp)
endif #}

ifeq ($(ATH_DUAL_FLASH),1)
OBJS	+= ../common/ath_nand.o ../../../drivers/nand/nand_ids.o
endif

ifndef BOOT_FROM_NAND
OBJS	+= ../common/ath_flash.o
endif

ifeq ($(ATH_RGMII_CAL),1)
OBJS	+= ../common/rgmii-cal-955x.o	
OBJS	+= ../common/athrs17_lpbk.o
endif

ifeq ($(ETH_CONFIG2), _s17)
OBJS	+= ../common/athrs17_phy.o
endif

ifeq ($(ETH_CONFIG), _s17)
OBJS	+= ../common/athrs17_phy.o
endif

ifeq ($(ETH_CONFIG2), _s17_hwaccel)
OBJS	+= ../common/athrs17_phy.o
endif

ifeq ($(ETH_CONFIG), _s17_hwaccel)
OBJS	+= ../common/athrs17_phy.o
endif

ifeq ($(ETH_CONFIG), _s16)
OBJS	+= ../common/athrs16_phy.o
endif

ifeq ($(ETH_CONFIG), _f1e)
OBJS	+= ../common/athrsf1_phy.o
endif

ifeq ($(ETH_CONFIG), _f2e)
OBJS	+= ../common/athrsf2_phy.o
endif

ifeq ($(ETH_CONFIG), _vir)
OBJS	+= ../common/athrs_vir_phy.o
endif

ifeq ($(ETH_CONFIG2), _vir)
OBJS	+= ../common/athrs_vir_phy.o
endif

ifeq ($(ETH_CONFIG), _s27)
OBJS	+= ../common/athr_s27_phy.o
endif

ifeq ($(ETH_CONFIG2), _ar8033)
OBJS	+= ../common/athr_ar8033_phy.o
endif


SOBJS	= ../common/955x.o ../common/tap-955x.o

$(LIB):	.depend $(OBJS) $(SOBJS) $(EXTRA_OBJS)
	@echo "  LIB     $@"
	${Q}$(AR) ${ARFLAGS} $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
