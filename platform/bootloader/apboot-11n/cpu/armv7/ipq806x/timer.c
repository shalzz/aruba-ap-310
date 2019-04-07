/*
 * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.
 * Source : APQ8064 LK boot
 *
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <asm/arch-ipq806x/iomap.h>
#include <asm/io.h>
#include <common.h>
#include <asm/types.h>
#include <asm/arch/systimer.h>
#include <watchdog.h>
#include <asm/arch/timer.h>

static ulong timestamp;
static ulong lastinc;

#define GPT_FREQ_KHZ    32
#define GPT_FREQ	(GPT_FREQ_KHZ * 1000)	/* 32 KHz */

unsigned long long get_ticks(void)
{
    return get_timer(0);
}

ulong get_tbclk (void)
{
    return 1000;
}

/**
 * timer_init - initialize timer
 */
int timer_init(void)
{
	writel(0, GPT_ENABLE);
	writel(GPT_ENABLE_EN, GPT_ENABLE);
	return 0;
}

/**
 * get_timer - returns time lapsed
 * @base: base/start time
 *
 * Returns time lapsed, since the specified base time value.
 */
ulong get_timer(ulong base)
{
	return get_timer_masked() - base;
}

#ifndef CONFIG_WD_PERIOD
# define CONFIG_WD_PERIOD	(10 * 1000 * 1000)	/* 10 seconds default*/
#endif

extern void __udelay(unsigned long usec);
/* ------------------------------------------------------------------------- */

void udelay(unsigned long usec)
{
	ulong kv;

	do {
		WATCHDOG_RESET();
		kv = usec > CONFIG_WD_PERIOD ? CONFIG_WD_PERIOD : usec;
		__udelay (kv);
		usec -= kv;
	} while(usec);
}

void mdelay(unsigned long msec)
{
	while (msec--)
		udelay(1000);
}

/**
 * __udelay -  generates micro second delay.
 * @usec: delay duration in microseconds
 *
 * With 32KHz clock, minimum possible delay is 31.25 Micro seconds and
 * its multiples. In Rumi GPT clock is 32 KHz
 */
void __udelay(unsigned long usec)
{
	unsigned int val;
	ulong now, last;
	ulong runcount;

	usec = (usec + GPT_FREQ_KHZ - 1) / GPT_FREQ_KHZ;
	last = readl(GPT_COUNT_VAL);
	runcount = last;
	val = usec + last;

	do {
		now = readl(GPT_COUNT_VAL);
		if (last > now)
			runcount += ((GPT_FREQ - last) + now);
		else
			runcount += (now - last);
		last = now;
	} while (runcount < val);
}


inline ulong gpt_to_sys_freq(unsigned int gpt)
{
	/*
	 * get_timer() expects the timer increments to be in terms
	 * of CONFIG_SYS_HZ. Convert GPT timer values to CONFIG_SYS_HZ
	 * units.
	 */
	return (((ulong)gpt) / (GPT_FREQ / CONFIG_SYS_HZ));
}

/**
 * get_timer_masked - returns current ticks
 *
 * Returns the current timer ticks, since boot.
 */
ulong get_timer_masked(void)
{
	ulong now = gpt_to_sys_freq(readl(GPT_COUNT_VAL));

	if (lastinc <= now) {	/* normal mode (non roll) */
		/* normal mode */
		timestamp += now - lastinc;
		/* move stamp forward with absolute diff ticks */
	} else {		/* we have overflow of the count down timer */
		timestamp += now + (TIMER_LOAD_VAL - lastinc);
	}

	lastinc = now;

	return timestamp;
}

void reset_timer_masked(void)
{
	lastinc = 0;
	timestamp = 0;
}

void reset_timer(void)
{
	reset_timer_masked();
}

