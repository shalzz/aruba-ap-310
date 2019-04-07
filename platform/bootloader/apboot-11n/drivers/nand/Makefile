include $(TOPDIR)/config.mk

LIB := libnand.a

OBJS := nand.o nand_base.o nand_ids.o nand_ecc.o nand_bbt.o ipq_nand.o
all:	$(LIB)

$(LIB):	$(OBJS)
	@echo "  LIB     $@"
	${Q}$(AR) ${ARFLAGS} $@ $(OBJS)

#########################################################################

.depend:	Makefile $(OBJS:.o=.c)
		${Q}$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend
