include $(TOPDIR)/config.mk

LIB	= lib$(BOARD).a

OBJS	= $(BOARD).o ../common/ar7100_flash.o athrs26_phy.o pb44_pci.o ../common/generic_spi.o ../common/ar7100_watchdog.o ../common/ar7100_misc.o ../common/vsc8601_phy.o
SOBJS	= lowlevel_init.o

$(LIB):	.depend $(OBJS) $(SOBJS)
	@echo "  LIB     $@"
	${Q}$(AR) ${ARFLAGS} $@ $(OBJS) ${SOBJS}

#########################################################################

.depend:	Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c)
		${Q}$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
