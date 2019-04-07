// vim:set ts=4 sw=4:
#include <config.h>
#if defined(CONFIG_APBOOT) && !defined(CONFIG_POST)
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 * Aruba AP POST functions
 */

#include <common.h>

#if defined(CONFIG_WASP) || defined(CONFIG_ATHEROS)
#include <command.h>
extern int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#endif

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_APBOOT) && defined(CONFIG_HW_WATCHDOG)
extern void hw_watchdog_reset(void);
extern void hw_watchdog_init(void);
#endif

static int
dram_test(
    int test, unsigned long *base, unsigned long step,
	unsigned long length, unsigned long pattern
)
{
#ifdef ARUBA_ARM
	extern void cpu_dcache_flush_all(void);
#endif
    unsigned long *ptr = base;
    unsigned long i;

    printf("mem %d ", test);
#if defined(CONFIG_APBOOT) && defined(CONFIG_HW_WATCHDOG)
	hw_watchdog_reset();
#endif
    for (i = 0; i < length; i += step, ptr += step) {
        *ptr = pattern ^ (unsigned long) ptr;
    }
#ifdef ARUBA_ARM
	/* Drain write buffer */
	asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i));
		
	/* Flush invalidate D-cache */
//	cpu_dcache_flush_all();
#endif
    ptr = base;
    for (i = 0; i < length; i += step, ptr += step) {
        if (*ptr != (pattern ^ (unsigned long) ptr)) {
			printf("(Mismatch at %08lx: Expected %08lx got %08lx) ",
			   (unsigned long) ptr, pattern ^ (unsigned long) ptr, *ptr);
#if defined(CONFIG_WASP) || defined(CONFIG_ATHEROS)
			do_reset(NULL, 0, 0, NULL);
#else
			return 1;
#endif
		}
    }
#ifdef CONFIG_ARUBA_OCTEON
	hw_watchdog_reset();
#endif
    printf("\b\b\b\b\b\b");
    return 0;
}

int 
aruba_testdram(unsigned long mem_start, unsigned long mem_end, int post)
{
    int test;
    unsigned long *ptr;

#if defined(CONFIG_APBOOT) && defined(CONFIG_HW_WATCHDOG)
    hw_watchdog_init();
#endif

    printf("POST%d: ", post);

#ifndef ARUBA_ARM
#ifndef CONFIG_OCTEON
    ptr = (unsigned long *)(mem_start | 0x20000000); /* uncached */
#else
    ptr = (unsigned long *)(mem_start /*| 0x20000000*/); /* uncached */
#endif
#else
    ptr = (unsigned long *)(mem_start /*| 0x20000000*/); /* uncached */
#endif
    test = 0;
    /* test entire first 128KB */
    if (dram_test(test++, ptr, 1, (128 * 1024) / sizeof(*ptr),
		  0x55555555)) {
        goto failure;
    }
    if (dram_test(test++, ptr, 1, (128 * 1024) / sizeof(*ptr),
		  0xaaaaaaaa)) {
        goto failure;
    }
    /* test each 128KB region */
    if (dram_test(test++, ptr, (128 * 1024) / sizeof(*ptr),
		  (mem_end - mem_start)/sizeof(*ptr), 0xff00ff00)) {
        goto failure;
    }
    if (dram_test(test++, ptr, (128 * 1024) / sizeof(*ptr),
		  (mem_end - mem_start)/sizeof(*ptr), 0x00ff00ff)) {
        goto failure;
    }

    printf("passed\n");

    return 0;

 failure:
    printf("FAILED!\n");
    return 1;
}

#ifdef ARUBA_FREESCALE
int
testdram(void)
{
	/* test everything below the part where u-boot will run */
	return aruba_testdram(CFG_MEMTEST_START, CFG_MEMTEST_END, 2);
}
#else
int
testdram(void)
{
	// test everything above the part where u-boot will run
	return aruba_testdram(CFG_MEMTEST_START, CFG_SDRAM_BASE + gd->ram_size, 2);
}

#endif

#ifdef ARUBA_AR7100
int
ar7100_testdram(void)
{
	// test everything above the part where u-boot will run
	return aruba_testdram(CFG_MEMTEST_START, CFG_SDRAM_BASE + gd->ram_size, 1);
}
#endif

#ifdef ARUBA_ARM
int
mv_testdram(void)
{
	unsigned int start, end;
	extern unsigned int _TEXT_BASE;
	extern unsigned mvDramIfSizeGet(void);
	extern unsigned mvDramIfBankBaseGet(unsigned);

	/* Get the base address of memory bank 0 */
	end = start = mvDramIfBankBaseGet(0);

	/* Start from the location where u-boot ends
         * FIXME: Additional _1M is added for heap size
	 */
	start += _TEXT_BASE + _1M + _1M;

	/* TODO: If we are using same DRAM register init file for base and 
	 * client, we need to hard-code end-offset for base to be 256M */
	end += mvDramIfSizeGet();
	// test everything above the part where u-boot will run

	return aruba_testdram(start, end, 1);
}
#endif
#endif
