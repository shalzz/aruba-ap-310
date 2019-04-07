#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# 
# $Id:$
# $Rev: : Global SVN Revision = 1950 $
# 
#

include $(TOPDIR)/config.mk

LIB	= $(obj)libmde.a



COBJS	+= mde_main.o mde_cmd.o mde_dev.o mde_find_dq_passthrough.o mde_mem_direct.o
COBJS	+= mde_mt.o mde_platform.o mde_local_cmd.o
COBJS	+= mde_tools.o mde_utils.o mde_rdi.o mde_wr_eye.o
# DDR-EYE-SHMOO: add mde_wr_eye.o

ifeq ($(strip ${MDE_DDR_CLK}),800)
CFLAGS += -DMDE_DDR_CLK=800
else
CFLAGS += -DMDE_DDR_CLK=533
endif

CFLAGS   += -I$(TOPDIR)/cpu/${CPU}/iproc/mde 
CPPFLAGS += -I$(TOPDIR)/cpu/${CPU}/iproc/mde 
CFLAGS += -DFORCING=${MDE_RUNS_AFTER_SHMOO}

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)

OBJS	:= $(addprefix $(obj),$(COBJS) $(SOBJS))

CFLAGS += -DCFG_TMT_RANDOM_MODE=2 
all:   $(obj).depend $(LIB) 

$(LIB):	$(OBJS) 
	@echo "  LIB     $@"
	${Q}$(AR) ${ARFLAGS} $@ $(OBJS) $(SOBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
