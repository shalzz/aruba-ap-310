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
include $(TOPDIR)/board/${BOARD}/config.tmp

LIB	= lib$(CPU).a

OBJS	= asc_serial.o au1x00_serial.o au1x00_eth.o au1x00_usb_ohci.o \
	  interrupts.o incaip_clock.o
OBJS    +=  octeon_serial.o octeon_debugger.o octeon_eth.o octeon_spi4000.o \
	    octeon_pci.o octeon_ipd_bp_verify.o cpu.o
START	= start.o
	  
SOBJS   += octeon_debugger_handler.o octeon_ipd_bp_enable.o octeon_watchdog.o

# SOBJS	= cache.o uart.o boot2.o cpu.o dram_mats.o

all:	.depend $(START) $(LIB)

$(LIB):	$(OBJS) $(SOBJS)
	@echo "  LIB     $@"
	@$(AR) ${ARFLAGS} $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(START:.o=.S) $(OBJS:.o=.c)
		${Q}$(CC) -M $(CFLAGS) $(START:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
