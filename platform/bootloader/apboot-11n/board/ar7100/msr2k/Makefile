include $(TOPDIR)/config.mk
include $(TOPDIR)/include/config.mk

LIB	= lib$(BOARD).a

OBJS	= $(BOARD).o ../common/ar7100_flash.o athrs26_phy.o msr2k_pci.o \
	../common/generic_spi.o ../common/ar7100_watchdog.o ../common/ar7100_misc.o \
	../common/generic_i2c.o

ifeq ($(MV88E1XXX_PHY_SUPPORT),true)
OBJS += ../common/marvell_phy.o
else
OBJS += ../common/vsc8601_phy.o 
endif

ifeq ($(MESHOS_DUALFLASH_SUPPORT),true)
OBJS += ../common/ar7100_dualflash.o
endif

SOBJS	= lowlevel_init.o

$(LIB):	.depend $(OBJS) $(SOBJS)
	@echo "  LIB     $@"
	@$(AR) ${ARFLAGS} $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c)
		${Q}$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
