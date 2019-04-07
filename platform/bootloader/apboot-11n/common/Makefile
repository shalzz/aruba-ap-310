#
# (C) Copyright 2004
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk
include $(TOPDIR)/include/config.mk

LIB	= libcommon.a

AOBJS	=

COBJS	= main.o ACEX1K.o altera.o bedbug.o circbuf.o \
	  cmd_ace.o cmd_autoscript.o \
	  cmd_bdinfo.o cmd_bedbug.o cmd_bmp.o cmd_boot.o cmd_bootm.o \
	  cmd_cache.o cmd_console.o rngs.o \
	  cmd_date.o cmd_dcr.o cmd_diag.o cmd_display.o cmd_doc.o cmd_dtt.o cmd_poe.o \
	  cmd_eeprom.o cmd_elf.o cmd_ext2.o \
	  cmd_fat.o cmd_fdc.o cmd_fdos.o cmd_flash.o cmd_fpga.o \
	  cmd_i2c.o cmd_ide.o cmd_immap.o cmd_itest.o cmd_jffs2.o \
	  cmd_load.o cmd_log.o cmd_flash_octeon.o cmd_flash_atheros.o \
	  cmd_flash_marvell.o cmd_flash_fsl.o cmd_i2c_marvell.o cmd_sf.o \
	  cmd_flash_qca.o cmd_mem.o cmd_mii.o cmd_misc.o cmd_mmc.o \
	  cmd_nand.o cmd_net.o cmd_nvedit.o cmd_mtdparts.o cmd_ubi.o \
	  cmd_pci.o cmd_pcmcia.o cmd_portio.o \
	  cmd_reginfo.o cmd_reiser.o \
	  cmd_usb.o cmd_vfd.o cmd_ethreg.o cmd_usb_fsl.o \
	  command.o console.o devices.o dlmalloc.o docecc.o \
	  environment.o env_common.o \
	  env_nand.o env_dataflash.o env_flash.o env_eeprom.o \
	  env_nvram.o env_nowhere.o env_sf.o \
	  exports.o getopt.o \
	  flash.o fpga.o ft_build.o \
	  hush.o kgdb.o lcd.o lists.o lynxkdi.o \
	  memsize.o miiphybb.o miiphyutil.o \
	  s_record.o serial.o soft_i2c.o soft_spi.o spartan2.o spartan3.o \
	  usb.o usb_kbd.o usb_storage.o usb_fsl.o usb_storage_fsl.o \
	  virtex2.o xilinx.o crc16.o xyzModem.o image.o fdt_support.o \
	  hwconfig.o cmd_fdt.o cmd_bootm_fsl.o cmd_i2c_fsl.o \
	  aruba-fdt.o ddr_spd.o bcmutils.o bcmnvram.o cmd_nvram.o

ifeq (${FIPS_TEST},1)
COBJS += cmd_fips.o
endif

ifeq (${ARCH},octeon)
COBJS += cmd_octeon.o cmd_octeon_linux.o cmd_lan_bypass.o
endif

ifeq (${APBOOT},true)
COBJS += aruba_manuf.o aruba_post.o md5.o aruba-inventory.o apboot-util.o
COBJS += cmd_tpm.o
ifeq (${MESHOS_SUPPORT},true)
COBJS += cmd_meshos.o 
else
COBJS += cmd_apboot.o
endif
endif

ifeq ($(VXWORKS_UBOOT),1)
COBJS += cmd_elf.o
endif

OBJS	= $(AOBJS) $(COBJS)

CPPFLAGS += -I..

all:	$(LIB) $(AOBJS)

$(LIB): .depend $(OBJS)
	@echo "  LIB     $@"
	${Q}$(AR) ${ARFLAGS} $@ $(OBJS)

environment.o: environment.c ../tools/envcrc
	$(CC) $(AFLAGS) -Wa,--no-warn \
		-DENV_CRC=$(shell ../tools/envcrc) \
		-c -o $@ environment.c

../tools/envcrc:
	$(MAKE) -C ../tools

#########################################################################

.depend:	Makefile $(AOBJS:.o=.S) $(COBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(AOBJS:.o=.S) $(COBJS:.o=.c) > $@

sinclude .depend

#########################################################################
