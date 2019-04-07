include $(TOPDIR)/config.mk

LIB	= lib$(BOARD).a

# Cardhu ethernet
ETH_CONFIG = _s27

OBJS	= $(BOARD).o flash.o ../common/ar7240_pci.o ../common/ar7240_misc.o ../common/ar7240_watchdog.o

ifdef BOOT_FROM_NAND
OBJS	+= ../common/ath_nand_flash.o
else
OBJS	+= ../common/ar7240_flash.o
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

ifeq ($(ETH_CONFIG), _s27)
OBJS	+= ../common/athr_s27_phy.o
endif

SOBJS	= ../common/lowlevel_init_934x.o
SOBJS	+= ../common/lowlevel_init_934x-1.1.o

$(LIB):	.depend $(OBJS) $(SOBJS)
	@echo "  LIB     $@"
	@$(AR) ${ARFLAGS} $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c)
		${Q}$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
