#
# Copyright 2009-2010 Freescale Semiconductor, Inc.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# Version 2 as published by the Free Software Foundation.
#

include $(TOPDIR)/config.mk

LIB	= $(obj)lib8xxx.a

ifneq ($(CPU),mpc83xx)
COBJS	+= cpu.o
endif

COBJS += fdt.o
COBJS-$(CONFIG_FSL_IFC) += fsl_ifc.o
COBJS += fsl_lbc.o
COBJS-$(CONFIG_SYS_SRIO) += srio.o

SRCS	:= $(START:.o=.S) $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))

all:	$(obj).depend $(LIB)

$(LIB):	$(OBJS)
	@echo "  LIB     $@"
	${Q}$(AR) ${ARFLAGS} $@ $(OBJS)

include $(SRCTREE)/rules.mk

#.depend:	Makefile $(START:.o=.S) $(OBJS:.o=.c)
#		$(CC) -M $(CFLAGS) $(START:.o=.S) $(OBJS:.o=.c) > $@

sinclude $(obj).depend
