#  Copyright (c) 2012 The Linux Foundation. All rights reserved.

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(BOARD).a

COBJS	:= ipq_i2c.o ipq806x_cdp.o ../common/athrs17_phy.o

SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

$(LIB):	$(obj).depend $(OBJS)
	@echo "  LIB     $@"
	echo $(AR) ${ARFLAGS} $@ $(OBJS) 
	$(AR) ${ARFLAGS} $@ $(OBJS) 

#	$(call cmd_link_o_target, $(OBJS))

clean:
	rm -f $(OBJS)

distclean:	clean
	rm -f $(LIB) core *.bak $(obj).depend

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
