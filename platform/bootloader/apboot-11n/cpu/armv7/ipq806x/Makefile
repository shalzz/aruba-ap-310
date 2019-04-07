# Copyright (c) 2012 The Linux Foundation. All rights reserved.

include $(TOPDIR)/config.mk

LIB	=  $(obj)lib$(SOC).a

SOBJS-y = timer.o
COBJS-y += clock.o
COBJS-y += gpio.o
COBJS-y += smem.o
COBJS-y += ipq806x-watchdog.o
ifeq ($(CONFIG_IPQ_APPSBL_DLOAD),y)
COBJS-y += cmd_dumpipq_data.o
endif

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS-y) $(SOBJS-y))

all:	 $(obj).depend $(LIB)

$(LIB):	$(OBJS)
	$(AR) ${ARFLAGS} $@ $(OBJS) 
#	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################

