include $(TOPDIR)/config.mk

LIB = $(obj)lib$(BOARD).a

COBJS	:= $(BOARD).o ddr_init_table.o

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))
SOBJS	:= $(addprefix $(obj),$(SOBJS))

$(LIB): $(obj).depend $(OBJS) $(SOBJS)
	@echo "  LIB     $@"
	${Q}$(AR) ${ARFLAGS} $@ $(OBJS) $(SOBJS)
clean:
	rm -f $(SOBJS) $(OBJS)

distclean:	clean
	rm -f $(LIB) core *.bak .depend

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk
#include /home/lab/uboot_from_denx_tot/uboot/rules.mk

sinclude $(obj).depend

#########################################################################
