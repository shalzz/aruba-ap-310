// vim:set ts=4 sw=4 expandtab:
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
 * Date & Time support (no alarms) for Seiko S35390A RTC
 *
 * based on ds1374.c, and a lot of ideas plundered from Byron Bradley's Linux driver
 */

#include <common.h>
#include <command.h>
#include <rtc.h>
#include <i2c.h>
#include <exports.h>

#if (defined(CONFIG_RTC_S35390)) && (CONFIG_COMMANDS & CFG_CMD_DATE)

/*---------------------------------------------------------------------*/
#undef DEBUG_RTC
//#define DEBUG_RTC

#ifdef DEBUG_RTC
#define DEBUGR(fmt,args...) printf(fmt ,##args)
#else
#define DEBUGR(fmt,args...)
#endif
/*---------------------------------------------------------------------*/
#if defined(CONFIG_RTC_S35390) && (CFG_I2C_SPEED > 400000)
# error The S35390 is specified up to 400kHz in fast mode!
#endif

//
// This part does not have internal registers; instead each register shows up as
// its own I2C device
//
#define S35390_BASE_ADDRESS	0x30
#define	S35390_STATUS_1 (S35390_BASE_ADDRESS + 0x0)
#define	S35390_STATUS_2 (S35390_BASE_ADDRESS + 0x1)
#define	S35390_RTC_1    (S35390_BASE_ADDRESS + 0x2)
#define	S35390_RTC_2    (S35390_BASE_ADDRESS + 0x3)
#define	S35390_INT_1    (S35390_BASE_ADDRESS + 0x4)
#define	S35390_INT_2    (S35390_BASE_ADDRESS + 0x5)
#define	S35390_CLK_ADJ  (S35390_BASE_ADDRESS + 0x6)
#define	S35390_FREE_REG (S35390_BASE_ADDRESS + 0x7)

// note that everything is bit-swapped in this fine part
#define STATUS1_POC    0x01
#define STATUS1_BLD    0x02
#define STATUS1_24     0x40
#define STATUS1_RESET  0x80

#define S35390_BYTE_YEAR	0
#define S35390_BYTE_MONTH	1
#define S35390_BYTE_DAY	    2
#define S35390_BYTE_WDAY	3
#define S35390_BYTE_HOURS	4
#define S35390_BYTE_MINS	5
#define S35390_BYTE_SECS	6

#define BCD2BIN(val)	(((val) & 0x0f) + ((val)>>4)*10)
#define BIN2BCD(val)	((((val)/10)<<4) + (val)%10)

static uchar rtc_read(uchar reg);
static void rtc_write(uchar reg, uchar val);

const u8 byte_rev_table[256] = {
	0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
	0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
	0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
	0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
	0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
	0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
	0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
	0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
	0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
	0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
	0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
	0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
	0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
	0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
	0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
	0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
	0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

static uchar bitrev8(uchar b)
{
    return byte_rev_table[b];
}

/*
 * Get the current time from the RTC
 */
void
rtc_get(struct rtc_time *tm)
{
    unsigned char tbuf[7];
    int i;

    i2c_read(S35390_RTC_1, 0, 0, tbuf, sizeof(tbuf));

    for (i = 0; i < 7; ++i) {
        tbuf[i] = bitrev8(tbuf[i]);
    }

    tm->tm_sec = BCD2BIN(tbuf[S35390_BYTE_SECS]);
    tm->tm_min = BCD2BIN(tbuf[S35390_BYTE_MINS]);
    // assume 24 hour; if we use 12 hour, this will need work
    tm->tm_hour = BCD2BIN(tbuf[S35390_BYTE_HOURS] & 0x3f);
    tm->tm_wday = BCD2BIN(tbuf[S35390_BYTE_WDAY]);
    tm->tm_mday = BCD2BIN(tbuf[S35390_BYTE_DAY]);
    tm->tm_mon = BCD2BIN(tbuf[S35390_BYTE_MONTH])/* - 1*/;
    tm->tm_year = BCD2BIN(tbuf[S35390_BYTE_YEAR]) + 2000;

	DEBUGR ("Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_wday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
}

/*
 * Set the RTC
 */
void
rtc_set(struct rtc_time *tmp)
{
    unsigned char tbuf[7];
    int i;

	DEBUGR ("Set DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

	if (tmp->tm_year < 1970 || tmp->tm_year > 2069)
		printf("WARNING: year should be between 1970 and 2069!\n");


    tmp->tm_year -= 2000;
    tbuf[S35390_BYTE_YEAR] = BIN2BCD(tmp->tm_year);
//    tmp->tm_mon += 1;
    tbuf[S35390_BYTE_MONTH] = BIN2BCD(tmp->tm_mon);
    tbuf[S35390_BYTE_DAY] = BIN2BCD(tmp->tm_mday);
    tbuf[S35390_BYTE_WDAY] = BIN2BCD(tmp->tm_wday);
    // assume 24 hour
    tbuf[S35390_BYTE_HOURS] = BIN2BCD(tmp->tm_hour);
    tbuf[S35390_BYTE_MINS] = BIN2BCD(tmp->tm_min);
    tbuf[S35390_BYTE_SECS] = BIN2BCD(tmp->tm_sec);

    for (i = 0; i < 7; ++i) {
        tbuf[i] = bitrev8(tbuf[i]);
    }

    i2c_write(S35390_RTC_1, 0, 0, tbuf, sizeof(tbuf));
}

void
s35390_reset(void)
{
    unsigned char val;
	val = rtc_read(S35390_STATUS_1);
#if 0
    if (!(val & (STATUS1_POC|STATUS1_BLD))) {
        return;
    }
    
    if (!(val & (STATUS1_POC | STATUS1_BLD))) {
        return;
    }
#endif
    val |= (STATUS1_24 /*| STATUS1_RESET*/);
    val &= 0xf0;
    rtc_write(S35390_STATUS_1, val);

    // clear all interrupt functions
    val = 0;
    rtc_write(S35390_STATUS_2, val);
}

/*
 * Reset the RTC. We set the date back to 1970-01-01.
 */
void
rtc_reset(void)
{
    struct rtc_time tmp;

    s35390_reset();

	tmp.tm_year = 1970;
	tmp.tm_mon = 1;
	tmp.tm_mday = 1;
	tmp.tm_wday = 4; // 1/1/70 was a Thursday
	tmp.tm_hour = 0;
	tmp.tm_min = 0;
	tmp.tm_sec = 0;

	rtc_set(&tmp);

	printf("RTC:   %4d-%02d-%02d %2d:%02d:%02d UTC\n",
		tmp.tm_year, tmp.tm_mon, tmp.tm_mday,
		tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
}

/*
 * Helper functions
 */
static uchar
rtc_read(uchar addr)
{
    unsigned char val;
	i2c_read(addr, 0, 0, &val, 1);
    return val;
}

static void 
rtc_write(uchar addr, uchar val)
{
    i2c_write(addr, 0, 0, &val, 1);
}
#endif /* (CONFIG_RTC_S35390) && (CFG_COMMANDS & CFG_CMD_DATE) */
