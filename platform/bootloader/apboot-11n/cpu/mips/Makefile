#
# (C) Copyright 2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk
#include $(TOPDIR)/include/config.mk

LIB	= lib$(CPU).a

START	= start.o
ifeq (${VENDOR},ar7100)
START	= atheros_start.o
endif
ifeq (${VENDOR},ar7240)
START	= atheros_start.o
endif
ifeq (${VENDOR},atheros)
START	= atheros_start.o
endif

OBJS	= asc_serial.o au1x00_serial.o au1x00_eth.o au1x00_usb_ohci.o \
	  interrupts.o incaip_clock.o

SOBJS	= cache.o
ifeq (${VENDOR},ar7100)
SOBJS	= atheros_cache.o cpu.o
endif
ifeq (${VENDOR},ar7240)
SOBJS	= atheros_cache.o cpu.o
endif
ifeq (${VENDOR},atheros)
SOBJS	= atheros_cache.o cpu.o
endif

# SOBJS	= cache.o uart.o boot2.o cpu.o dram_mats.o
all:	.depend $(START) $(LIB)

$(LIB):	$(OBJS) $(SOBJS)
	@echo "  LIB     $@"
	$(AR) ${ARFLAGS} $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(START:.o=.S) $(OBJS:.o=.c)
		${Q}$(CC) -M $(CFLAGS) $(START:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
