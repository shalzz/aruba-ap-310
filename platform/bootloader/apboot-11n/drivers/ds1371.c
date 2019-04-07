/*
 * (C) Copyright 2001, 2002, 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 * Keith Outwater, keith_outwater@mvis.com`
 * Steven Scholz, steven.scholz@imc-berlin.de
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Support for Dallas Semiconductor (now Maxim)
 * DS1371 watchdog function.
 *
 * based on ds1374.c
 */

#include <common.h>
#include <command.h>
#include <i2c.h>
#include <exports.h>

#if defined(CONFIG_DS1371)

#ifndef CFG_I2C_DS1371_ADDR
# define CFG_I2C_DS1371_ADDR	0x68
#endif

/* in seconds */
#ifndef CFG_I2C_DS1371_WDOG_VAL
# define CFG_I2C_DS1371_WDOG_VAL 30
#endif

#define DS1371_WDOG_VAL (CFG_I2C_DS1371_WDOG_VAL << 12)

/*
 * DS1371 register addresses
 */
#define DS1371_TOD_CNT_BYTE0_ADDR		0x00
#define DS1371_TOD_CNT_BYTE1_ADDR		0x01
#define DS1371_TOD_CNT_BYTE2_ADDR		0x02
#define DS1371_TOD_CNT_BYTE3_ADDR		0x03

#define DS1371_WD_ALM_CNT_BYTE0_ADDR	0x04
#define DS1371_WD_ALM_CNT_BYTE1_ADDR	0x05
#define DS1371_WD_ALM_CNT_BYTE2_ADDR	0x06

#define DS1371_CTL_ADDR			0x07 /* DS1371-CoNTrol-register */
#define DS1371_SR_ADDR			0x08 /* DS1371-StatusRegister */

#define DS1371_CTL_BIT_AIE		(1<<0) /* Bit 0 - Alarm Interrupt enable */
#define DS1371_CTL_BIT_RS1		(1<<1) /* Bit 1/2 - Rate Select square wave output */
#define DS1371_CTL_BIT_RS2		(1<<2) /* Bit 2/2 - Rate Select square wave output */
#define DS1371_CTL_BIT_INTCN		(1<<3) /* Bit 3 - interrupt control */
#define DS1371_CTL_BIT_WD_ALM		(1<<5) /* Bit 5 - Watchdoc/Alarm Counter Select */
#define DS1371_CTL_BIT_WACE		(1<<6) /* Bit 6 - Watchdog/Alarm Counter Enable WACE*/
#define DS1371_CTL_BIT_EN_OSC		(1<<7) /* Bit 7 - Enable Oscilator */

#define DS1371_SR_BIT_AF		0x01 /* Bit 0 = Alarm Flag */
#define DS1371_SR_BIT_OSF		0x80 /* Bit 7 - Osc Stop Flag */

typedef unsigned char boolean_t;

#ifndef TRUE
#define TRUE ((boolean_t)(0==0))
#endif
#ifndef FALSE
#define FALSE (!TRUE)
#endif

static uchar ds1371_read (uchar reg);
static void ds1371_write (uchar reg, uchar val, boolean_t set);
#if 0
static void ds1371_write_raw (uchar reg, uchar val);
#endif

/*
 * Reset the chip.
 */
void ds1371_reset (void)
{
	/* clear status flags */
	ds1371_write (DS1371_SR_ADDR, (DS1371_SR_BIT_AF|DS1371_SR_BIT_OSF), FALSE); /* clearing OSF and AF */

	/* stop oscillator */
	ds1371_write(DS1371_CTL_ADDR, DS1371_CTL_BIT_EN_OSC, TRUE);

	/* disable WACE, clear AIE - set to 0 */
	ds1371_write (DS1371_CTL_ADDR, (DS1371_CTL_BIT_WACE
				 |DS1371_CTL_BIT_AIE), FALSE);

	/* write watchdog timer value */
	ds1371_write(DS1371_WD_ALM_CNT_BYTE0_ADDR, DS1371_WDOG_VAL & 0xff , TRUE);
	ds1371_write(DS1371_WD_ALM_CNT_BYTE1_ADDR, (DS1371_WDOG_VAL >> 8) & 0xff, TRUE);
	ds1371_write(DS1371_WD_ALM_CNT_BYTE2_ADDR, (DS1371_WDOG_VAL >> 16) & 0xff, TRUE);

	/* enable watchdog and enable interrupt strobe */
	ds1371_write (DS1371_CTL_ADDR, 
		(DS1371_CTL_BIT_WD_ALM
		 |DS1371_CTL_BIT_INTCN
		 |DS1371_CTL_BIT_WACE
		 |DS1371_CTL_BIT_AIE), TRUE);

	/* Start clock */
	ds1371_write(DS1371_CTL_ADDR, DS1371_CTL_BIT_EN_OSC, FALSE);

}

void ds1371_off (void)
{
	/* Stop clock */
	ds1371_write(DS1371_CTL_ADDR, DS1371_CTL_BIT_EN_OSC, TRUE);
}

void ds1371_show (void)
{
	unsigned val = 0;

	printf("Control register: 0x%x\n", ds1371_read(DS1371_CTL_ADDR));
	printf("Status register: 0x%x\n", ds1371_read(DS1371_SR_ADDR));

	val |= ds1371_read(DS1371_WD_ALM_CNT_BYTE0_ADDR);
	val |= (ds1371_read(DS1371_WD_ALM_CNT_BYTE1_ADDR) << 8);
	val |= (ds1371_read(DS1371_WD_ALM_CNT_BYTE2_ADDR) << 16);

	printf("Counter: %u\n", val);
}

/*
 * Helper functions
 */
static uchar ds1371_read (uchar reg)
{
	return (i2c_reg_read (CFG_I2C_DS1371_ADDR, reg));
}

static void ds1371_write (uchar reg, uchar val, boolean_t set)
{
	if (set == TRUE) {
		val |= i2c_reg_read (CFG_I2C_DS1371_ADDR, reg);
		i2c_reg_write (CFG_I2C_DS1371_ADDR, reg, val);
	} else {
		val = i2c_reg_read (CFG_I2C_DS1371_ADDR, reg) & ~val;
		i2c_reg_write (CFG_I2C_DS1371_ADDR, reg, val);
	}
}

#if 0
static void ds1371_write_raw (uchar reg, uchar val)
{
	i2c_reg_write (CFG_I2C_DS1371_ADDR, reg, val);
}
#endif

int
do_ds1371(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    if (argc == 1) {
        ds1371_show();
    } else {
        ds1371_off();
    }
    return 0;
}

U_BOOT_CMD(
    ds1371,     2,     1,      do_ds1371,
    "ds1371 - DS1371 commands\n",
    "- manage the DS1317 watchdog\n"
);
#endif /* CONFIG_DS1371 */
