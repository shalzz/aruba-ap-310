include $(TOPDIR)/config.mk

LIB	= lib$(SOC).a

START	=
#OBJS	= ar7240_serial.o ag7240.o ar7240_flash.o meminit.o
OBJS	= ar7240_serial.o meminit.o
SOBJS	=

BOARD_OBJ = ag7240.o
ifeq ($(BOARD), db12x)
BOARD_OBJ = ag934x.o
endif
ifeq ($(BOARD), cardhu)
BOARD_OBJ = ag934x.o
endif
ifeq ($(BOARD), dalmore)
BOARD_OBJ = ag934x.o
endif
ifeq ($(BOARD), dalmore_lte)
BOARD_OBJ = ag934x.o
endif
ifeq ($(BOARD), hazelburn)
BOARD_OBJ = ag934x.o
endif
ifeq ($(BOARD), hazelburn_h)
BOARD_OBJ = ag934x.o
endif
OBJS    += $(BOARD_OBJ)

all:	.depend $(START) $(LIB)

$(LIB):	$(OBJS) $(SOBJS) $(OBJS-0)
	@echo "  LIB     $@"
	@$(AR) ${ARFLAGS} $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(START:.o=.S) $(OBJS:.o=.c)
		${Q}$(CC) -M $(CFLAGS) $(START:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
