include $(TOPDIR)/config.mk

LIB	= lib$(BOARD).a

OBJS	= $(BOARD).o flash.o ../common/ar7240_pci.o ../common/ar7240_flash.o \
	../common/ar7240_s26_phy.o ../common/ar7240_misc.o \
	../common/ar7240_watchdog.o ../common/ar8021_phy.o

SOBJS	= ../common/lowlevel_init.o

$(LIB):	.depend $(OBJS) $(SOBJS)
	@echo "  LIB     $@"
	@$(AR) ${ARFLAGS} $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c)
		${Q}$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
