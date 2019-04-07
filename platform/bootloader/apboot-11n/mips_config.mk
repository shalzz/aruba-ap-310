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

BOOT1_INFO=0x8c000000

PLATFORM_CPPFLAGS += -DCONFIG_MIPS -D__MIPS__ -D_ARUBA_OPTIMIZATION_  -D__SIGNED_IMAGES__ -D__ENFORCE_SIGNED_IMAGES__ -D__U_BOOT__
ifneq (${VENDOR},ar7100)
ifneq (${VENDOR},ar7240)
PLATFORM_CPPFLAGS += -G0 -embedded-pic -DBOOT1_INFO=$(BOOT1_INFO) -Wa,-KPIC 
endif
endif
PLATFORM_CPPFLAGS += -D__FIPS_BUILD__

ifdef FIPS
PLATFORM_CPPFLAGS += -D__FIPS_BUILD__
endif

#PLATFORM_CPPFLAGS += -DCONFIG_MIPS -D__MIPS__ -G0 -DBOOT1_INFO=$(BOOT1_INFO) -Wa,-KPIC
