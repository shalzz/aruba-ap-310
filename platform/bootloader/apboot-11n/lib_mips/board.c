//#define DEBUG
/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 * Copyright 2004,2005 Cavium Networks
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

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <devices.h>
#include <net.h>
#include <flash.h>
#include <environment.h>
#include "octeon_boot.h"
#include <lib_octeon.h>
#include <cvmx-bootmem-shared.h>
#include <octeon_eeprom_types.h>
#include <lib_octeon_shared.h>
#include <version.h>

#ifdef __FIPS_BUILD__
#include "../include/xyssl/rsa.h"
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#if (!defined(ENV_IS_EMBEDDED) ||  defined(CFG_ENV_IS_IN_NVRAM))
#define	TOTAL_MALLOC_LEN	(CFG_MALLOC_LEN + CFG_ENV_SIZE)
#else
#define	TOTAL_MALLOC_LEN	CFG_MALLOC_LEN
#endif

#define	SLOP_K	(576 * 1024)

#define DRAM_LATE_ZERO_OFFSET  0x100000ull
//#define DEBUG

#ifdef __FIPS_BUILD__
int fips_post_status = 0;
#endif

extern int timer_init(void);

extern int incaip_set_cpuclk(void);

extern ulong uboot_end_data;
extern ulong uboot_end;

ulong monitor_flash_len;
#define SET_K0(val)                 asm volatile ("add $26, $0, %[rt]" : : [rt] "d" (val):"memory")

const char version_string[] = U_BOOT_VERSION;

static char *failed = "*** failed ***\n";
#ifdef __TLV_INVENTORY__
extern void aruba_manuf_init_tlv(void);
#endif

/*
 * Begin and End of memory area for malloc(), and current "brk"
 */
static ulong mem_malloc_start;
static ulong mem_malloc_end;
static ulong mem_malloc_brk;


/*
 * The Malloc area is immediately below the monitor copy in DRAM
 */
static void mem_malloc_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	ulong dest_addr = CFG_MONITOR_BASE + gd->reloc_off;

	mem_malloc_end = dest_addr;
	mem_malloc_start = dest_addr - TOTAL_MALLOC_LEN;
	mem_malloc_brk = mem_malloc_start;

#if !CONFIG_OCTEON_SIM
	memset ((void *) mem_malloc_start,
		0,
		mem_malloc_end - mem_malloc_start);
#endif
}

void *sbrk (ptrdiff_t increment)
{
	ulong old = mem_malloc_brk;
	ulong new = old + increment;

	if ((new < mem_malloc_start) || (new > mem_malloc_end)) {
		return (NULL);
	}
	mem_malloc_brk = new;
	return ((void *) old);
}


static int init_func_ram (void)
{
    DECLARE_GLOBAL_DATA_PTR;

    puts ("DRAM:  ");

#if CONFIG_OCTEON_SIM
    if ((gd->ram_size = (uint64_t)(*((uint32_t *)0x9ffffff8) * 1024 * 1024)) > 0) {
        print_size (gd->ram_size, "\n");
        return (0);
    }
#else
    if ((gd->ram_size) > 0) {
        print_size (gd->ram_size, "\n");
        return (0);
    }
#endif

    puts (failed);
    return (1);
}

static int display_banner(void)
{
	extern char build_num[], build_time[];

	printf ("\n\n%s (build %s)\n", version_string, build_num);
	printf ("Built: %s\n\n", build_time);
	return (0);
}

static void display_flash_config(ulong size)
{
	puts ("Flash: ");
	print_size (size, "\n");
}

#if !CONFIG_OCTEON_SIM
static const dimm_odt_config_t dimm_odt_cn31xx_1rank_config_table[] = {
    CN31XX_DRAM_ODT_1RANK_CONFIGURATION
};
static const dimm_odt_config_t dimm_odt_cn31xx_2rank_config_table[] = {
    CN31XX_DRAM_ODT_2RANK_CONFIGURATION
};
static const dimm_odt_config_t dimm_odt_cn38xx_1rank_config_table[] = {
    CN38XX_DRAM_ODT_1RANK_CONFIGURATION
};
static const dimm_odt_config_t dimm_odt_cn38xx_2rank_config_table[] = {
    CN38XX_DRAM_ODT_2RANK_CONFIGURATION
};
static const dimm_odt_config_t dimm_odt_cn58xx_1rank_config_table[] = {
    CN58XX_DRAM_ODT_1RANK_CONFIGURATION
};
static const dimm_odt_config_t dimm_odt_cn58xx_2rank_config_table[] = {
    CN58XX_DRAM_ODT_2RANK_CONFIGURATION
};
static const dimm_odt_config_t dimm_odt_cn50xx_1rank_config_table[] = {
    CN50XX_DRAM_ODT_1RANK_CONFIGURATION
};
static const dimm_odt_config_t dimm_odt_cn50xx_2rank_config_table[] = {
    CN50XX_DRAM_ODT_2RANK_CONFIGURATION
};

static int init_dram(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    int memsize = 0;
    int cpu_rev = octeon_get_proc_id() & 0xff;
    char *eptr;
    ddr_configuration_t ddr_configuration;

#if !(CONFIG_RAM_RESIDENT)
    /* Init the L2C, must be done before DRAM access so that we know L2 is empty */
    eptr = getenv("disable_l2_index_aliasing");
    if (eptr)
    {
        printf("L2 index aliasing disabled.\n");
    }
    else
    {
        /* Enable L2C index aliasing */
        uint64_t tmp = octeon_read64(CVMX_L2C_CFG);
#ifdef notyet
	// SCA; favor I/O in the arbitrator
	tmp &= ~7ull;
#endif
        octeon_write64(CVMX_L2C_CFG,  tmp | (1ull << 7));
    }
#endif

    /* Check to see if we should limit the number of L2 ways. */
    eptr = getenv("limit_l2_ways");
    if (eptr)
    {
        uint32_t valid_mask = 0xff;
        int ways_max = 7;
        if (!octeon_is_model(OCTEON_CN38XX))
        {
            valid_mask = 0xf;
            ways_max = 3;
        }
        
        int ways = simple_strtoul(eptr, NULL, 10);
        if (ways >= 1 && ways <= ways_max)
        {
            printf("Limiting L2 to %d ways\n", ways);
            uint64_t mask = (0xff << ways) & valid_mask;

            if (!octeon_is_model(OCTEON_CN30XX))
            {
                mask |= mask << 8;
                if (octeon_is_model(OCTEON_CN38XX))
                {
                    mask |= mask << 8;
                    mask |= mask << 8;
                }
            }
            cvmx_write_csr(CVMX_L2C_SPAR0, mask);
            if (octeon_is_model(OCTEON_CN38XX))
            {
                cvmx_write_csr(CVMX_L2C_SPAR1, mask);
                cvmx_write_csr(CVMX_L2C_SPAR2, mask);
                cvmx_write_csr(CVMX_L2C_SPAR3, mask);
            }
            cvmx_write_csr(CVMX_L2C_SPAR4, mask & valid_mask);
        }
        else
            printf("Invalid limit_l2_ways value: %d, must be between 1 and 7\n", ways);
    }


    uint32_t ddr_hertz = gd->ddr_clock_mhz * 1000000;
    uint32_t cpu_hertz = gd->cpu_clock_mhz * 1000000;
    uint32_t ddr_ref_hertz = cpu_hertz / 10; // was CN5020_FORCED_DDR_AND_CPU_REF_HZ;

    char *s;

    if ((s = getenv("ddr_clock_hertz")) != NULL) {
        ddr_hertz = simple_strtoul(s, NULL, 0);
        gd->ddr_clock_mhz = ddr_hertz / 1000000;
        printf("Parameter found in environment. ddr_clock_hertz = %d\n", ddr_hertz);
    }

    if ((s = getenv("ddr_ref_hertz")) != NULL) {
        ddr_ref_hertz = simple_strtoul(s, NULL, 0);
        gd->ddr_ref_hertz = ddr_ref_hertz;
        printf("Parameter found in environment. ddr_ref_hertz = %d\n", ddr_ref_hertz);
    }

#if defined(CONFIG_ARUBA_OCTEON) || defined(CONFIG_OCTEON_BBGW_REF)
    if (__octeon_is_model_runtime__(OCTEON_CN50XX)) {
    	// SCA; ddr_clock_hertz is not set here, and I don't know why yet;
	// we need it to be, so kludge it for now
	gd->ddr_clock_mhz = ARUBA_5010_DDR_MHZ;

	ddr_hertz = gd->ddr_clock_mhz * 1000000;
	gd->ddr_ref_hertz = CN5020_FORCED_DDR_AND_CPU_REF_HZ;
	ddr_ref_hertz = gd->ddr_ref_hertz;
    }
#endif
    /* Except for Octeon pass1 we can count DDR clocks, and we use
    ** this to correct the DDR clock that we are passed. For Octeon
    ** pass1 the measured value will always be returned as 0. */
    uint32_t calc_ddr_hertz = measure_octeon_ddr_clock(octeon_get_proc_id(), cpu_hertz, ddr_hertz, ddr_ref_hertz);

#ifdef DEBUG
    int calc_ddr_megahertz = calc_ddr_hertz / 1000000;
    printf("Measured DDR clock %d.%02d MHz\n", calc_ddr_megahertz, (calc_ddr_hertz - calc_ddr_megahertz * 1000000) / CALC_SCALER);
#endif

#if defined(CONFIG_ARUBA_OCTEON) || defined(CONFIG_OCTEON_BBGW_REF)
    if (!__octeon_is_model_runtime__(OCTEON_CN50XX)) {
#endif
    if (calc_ddr_hertz != 0)
    {
        if (!gd->ddr_clock_mhz)
        {
            /* If ddr_clock not set, use measured clock and don't warn */
            gd->ddr_clock_mhz = calc_ddr_hertz/1000000;
        }
        else if ((calc_ddr_hertz > ddr_hertz + 3000000) || (calc_ddr_hertz <  ddr_hertz - 3000000))
        {
            printf("\nWARNING:\n");
            printf("WARNING: Measured DDR clock mismatch! expected: %d MHz, measured: %d MHz, cpu clock: %d MHz\n",
                   ddr_hertz/1000000, calc_ddr_hertz/1000000, cpu_hertz/1000000);
            printf("WARNING: Using measured clock for configuration.\n");
            printf("WARNING:\n\n");
            gd->ddr_clock_mhz = calc_ddr_hertz/1000000;
        }
    }
#if defined(CONFIG_ARUBA_OCTEON) || defined(CONFIG_OCTEON_BBGW_REF)
    }
#endif


    /* Enable L2 ECC */
    octeon_write64(CVMX_L2D_ERR, 1);
    octeon_write64(CVMX_L2T_ERR, 1);


    if (gd->flags & GD_FLG_CLOCK_DESC_MISSING)
    {
        printf("Warning: Clock descriptor tuple not found in eeprom, using defaults\n");
        if (gd->board_desc.rev_major == 1)
        {
            printf("\n\nWARNING: memory configured for %d mhz clock.  \nIf this is not the actual memory clock\n", gd->ddr_clock_mhz);
            printf("poor performance and memory instability may result.  \nThe memory speed must be specified in the board eeprom\n\n\n");
        }
    }
#if !defined(CONFIG_APBOOT)
    if (gd->flags & GD_FLG_BOARD_DESC_MISSING)
    {
        printf("Warning: Board descriptor tuple not found in eeprom, using defaults\n");
    }

    printf("%s board revision major:%d, minor:%d, serial #: %s\n",
           cvmx_board_type_to_string(gd->board_desc.board_type),
           gd->board_desc.rev_major, gd->board_desc.rev_minor,
           gd->board_desc.serial_str);
#endif

#if defined(__MODEL_STR__)
    printf("Model: %s\n", __MODEL_STR__);
#endif
    printf("CPU:   ");
    printf("OCTEON %s-%s revision: %d\nClock: %d MHz, DDR clock: %d MHz (%d Mhz data rate)\n",
           octeon_get_model_name(), octeon_get_submodel_name(),
           cpu_rev & ~0x10,
           gd->cpu_clock_mhz,
           gd->ddr_clock_mhz, gd->ddr_clock_mhz *2);
#ifdef CONFIG_PALOMINO
    printf("Power: %s\n", !octeon_gpio_value(GPIO_POE_ACTIVE) ? "POE" : "DC");
    /* Read CPU clock multiplier */
    uint64_t data = octeon_read64((uint64_t)0x80011F00000001E8ull);
    data = data >> 18;
    data &= 0x1f;
#endif

    dimm_config_t dimm_config_table[] = {
#ifdef DRAM_SOCKET_CONFIGURATION
        DRAM_SOCKET_CONFIGURATION,
#endif /* DRAM_SOCKET_CONFIGURATION */
        {
            0,
            0
        }
    };


    /* Some boards need different ddr delay based on revision */
#if CONFIG_OCTEON_EBH3100
    if (gd->board_desc.rev_major == 1)
        ddr_configuration.ddr_board_delay = DDR_BOARD_DELAY_REV_1;
    else
        ddr_configuration.ddr_board_delay = DDR_BOARD_DELAY_REV_2;
#else
#ifdef DDR_BOARD_DELAY
    /* Board delay can vary based on configuration.  A default
       configuration can be described by DDR_BOARD_DELAY. */
    ddr_configuration.ddr_board_delay = DDR_BOARD_DELAY;
#endif
#endif
    uint32_t cpu_id = cvmx_get_proc_id();

    /* Check for special case of mismarked 3005 samples, and adjust cpuid */
    if (cpu_id == OCTEON_CN3010_PASS1 && (cvmx_read_csr(0x80011800800007B8ull) & (1ull << 34)))
        cpu_id |= 0x10;

    if (octeon_is_model(OCTEON_CN31XX) || octeon_is_model(OCTEON_CN30XX))
    {
        ddr_configuration.odt_1rank_config = (dimm_odt_config_t *)dimm_odt_cn31xx_1rank_config_table;
        ddr_configuration.odt_2rank_config = (dimm_odt_config_t *)dimm_odt_cn31xx_2rank_config_table;
    }
    else if (octeon_is_model(OCTEON_CN58XX))
    {
        ddr_configuration.odt_1rank_config = (dimm_odt_config_t *)dimm_odt_cn58xx_1rank_config_table;
        ddr_configuration.odt_2rank_config = (dimm_odt_config_t *)dimm_odt_cn58xx_2rank_config_table;
    }
#ifndef CONFIG_ARUBA_OCTEON
    else if (octeon_is_model(OCTEON_CN50XX))
    {
        ddr_configuration.odt_1rank_config = (dimm_odt_config_t *)dimm_odt_cn50xx_1rank_config_table;
        ddr_configuration.odt_2rank_config = (dimm_odt_config_t *)dimm_odt_cn50xx_2rank_config_table;
#if 0
        memcpy(ddr_config_array->odt_1rank_config, dimm_odt_cn50xx_1rank_config_table, sizeof(ddr_config_array->odt_1rank_config));
        memcpy(ddr_config_array->odt_2rank_config, dimm_odt_cn50xx_2rank_config_table, sizeof(ddr_config_array->odt_2rank_config));
#endif
    }
#endif
    else
    {
        ddr_configuration.odt_1rank_config = (dimm_odt_config_t *)dimm_odt_cn38xx_1rank_config_table;
        ddr_configuration.odt_2rank_config = (dimm_odt_config_t *)dimm_odt_cn38xx_2rank_config_table;
    }

    ddr_configuration.dimm_config_table = dimm_config_table;

    memsize = init_octeon_dram_interface(cpu_id,
                                         &ddr_configuration,
                                         gd->ddr_clock_mhz * 1000000,
                                         gd->cpu_clock_mhz*1000000,
                                         ddr_ref_hertz,
                                         gd->board_desc.board_type,
                                         gd->board_desc.rev_major,
                                         gd->board_desc.rev_minor
                                         );

    eptr = getenv("limit_dram_mbytes");
    if (eptr)
    {
        unsigned int mbytes = simple_strtol(eptr, NULL, 10);
        if (mbytes > 0)
        {
            memsize = mbytes;
            printf("Limiting DRAM size to %d MBytes based on limit_dram_mbytes env. variable\n", mbytes);
        }
    }


    if (memsize > 0)
    {
        gd->ram_size = (uint64_t)memsize * 1024 * 1024;

#if !defined(CONFIG_NO_RELOCATION) /* If already running in memory don't zero it. */
    /* Zero the low Megabyte of DRAM used by bootloader.  The rest is zeroed later.
    ** when running from DRAM*/
#ifdef CONFIG_ARUBA_OCTEON
	{
		extern int aruba_testdram(unsigned long, unsigned long, int);
		aruba_testdram(CFG_SDRAM_BASE + 0, 
			CFG_SDRAM_BASE + DRAM_LATE_ZERO_OFFSET, 1);
	}
#endif
        octeon_bzero64_pfs(0, DRAM_LATE_ZERO_OFFSET);
#endif /* !CONFIG_NO_RELOCATION */
        return(0);
    }
    else
    {
        return -1;
    }

}
#endif


static int init_baudrate (void)
{
	DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_APBOOT
	gd->baudrate = CONFIG_BAUDRATE;
#else
	char tmp[64];	/* long enough for environment variables */
	int i = getenv_r ("baudrate", tmp, sizeof (tmp));

	gd->baudrate = (i > 0)
			? (int) simple_strtoul (tmp, NULL, 10)
			: CONFIG_BAUDRATE;
#endif
	return (0);
}

#define DO_SIMPLE_DRAM_TEST_FROM_FLASH  0
#if DO_SIMPLE_DRAM_TEST_FROM_FLASH
/* Very simple DRAM test to run from flash for cases when bootloader boot
** can't complete.
*/
static int dram_test(void)
{
    uint32_t *addr;
    uint32_t *start = (void *)0x80000000;
    uint32_t *end = (void *)0x80200000;
    uint32_t val, incr, readback, pattern;
    int error_limit;

    pattern = 0;
    printf("Performing simple DDR test from flash.\n");

    #define DRAM_TEST_ERROR_LIMIT 200

    incr = 1;
    for (;;) {
        error_limit = DRAM_TEST_ERROR_LIMIT;

        printf ("\rPattern %08lX  Writing..."
                "%12s"
                "\b\b\b\b\b\b\b\b\b\b",
                pattern, "");

        for (addr=start,val=pattern; addr<end; addr++) {
                *addr = val;
                val  += incr;
        }

        puts ("Reading...");

        for (addr=start,val=pattern; addr<end; addr++) {
                readback = *addr;
                if (readback != val && error_limit-- > 0) {
                    if (error_limit + 1 == DRAM_TEST_ERROR_LIMIT)
                        puts("\n");
                    printf ("Mem error @ 0x%08X: "
                            "found %08lX, expected %08lX\n",
                            (uint)addr, readback, val);
                }
                val += incr;
        }

        /*
         * Flip the pattern each time to make lots of zeros and
         * then, the next time, lots of ones.  We decrement
         * the "negative" patterns and increment the "positive"
         * patterns to preserve this feature.
         */
        if(pattern & 0x80000000) {
                pattern = -pattern;	/* complement & increment */
        }
        else {
                pattern = ~pattern;
        }
        if (error_limit <= 0)
        {
            printf("Too many errors, printing stopped\n");
        }
        incr = -incr;
    }

}
#endif
/*
 * Breath some life into the board...
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependend #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */

int early_board_init(void);
int octeon_boot_bus_init_board(void);
#if CFG_LATE_BOARD_INIT
int late_board_init(void);
#endif

typedef int (init_fnc_t) (void);

init_fnc_t *init_sequence[] = {
#if CONFIG_OCTEON && (!CONFIG_OCTEON_SIM)
#if CONFIG_BOOT_BUS_BOARD
    octeon_boot_bus_init_board,  // board specific boot bus init
#else
    octeon_boot_bus_init,  // need to map PAL to read clocks, speed up flash
#endif
#endif
	timer_init,
	env_init,		/* initialize environment */
#if CONFIG_OCTEON
    early_board_init,
#endif
	init_baudrate,		/* initialze baudrate settings */
	serial_init,		/* serial communications setup */
	console_init_f,
	display_banner,		/* say that we are here */
#if !CONFIG_OCTEON_SIM
        init_dram,
#endif
#if !defined(CONFIG_RAM_RESIDENT)
#if DO_SIMPLE_DRAM_TEST_FROM_FLASH
        dram_test,
#endif
#if defined(CFG_DRAM_TEST)
	testdram,
#endif /* CFG_DRAM_TEST */
#endif	// ram-resident
	checkboard,
	init_func_ram,
	NULL,
};

void board_init_f(ulong bootflag)
{
	DECLARE_GLOBAL_DATA_PTR;

	gd_t gd_data, *id;
	bd_t *bd;
	init_fnc_t **init_fnc_ptr;
	ulong addr, addr_sp;
#if defined(CONFIG_APBOOT) || (!defined(CONFIG_NO_RELOCATION) || !defined(CONFIG_RAM_RESIDENT))
	ulong len = (ulong)&uboot_end - CFG_MONITOR_BASE;
#endif
	uint32_t u_boot_mem_top; /* top of memory, as used by u-boot */
#ifdef CONFIG_PURPLE
	void copy_code (ulong);
#endif

        SET_K0(&gd_data);
        /* Pointer is writable since we allocated a register for it.  */
	memset ((void *)gd, 0, sizeof (gd_t));

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			printf("hanging...\n");
			hang ();
		}
	}
	/*
	 * Now that we have DRAM mapped and working, we can
	 * relocate the code and continue running from DRAM.
	 */
#if defined(CONFIG_NO_RELOCATION) && defined(CONFIG_RAM_RESIDENT)
        /* If loaded into ram, we can skip relocation , and run from the spot we were loaded into */
#ifdef CONFIG_APBOOT
	addr = CFG_SDRAM_BASE + MIN(gd->ram_size, (1*1024*1024));
	/* Reserve memory for U-Boot code, data & bss
	 */
	addr -= MAX(len, (512*1024));
	/* round down to next 64k (allow us to create image at same addr for debugging)*/
	addr &= ~(64 * 1024 - 1);

#ifdef DEBUG
	printf ("Reserving %ldk for U-Boot at: %08lx\n", MAX(len, (512*1024)) >> 10, addr);
#endif

#else
	addr = CFG_MONITOR_BASE;
#endif
	u_boot_mem_top = CFG_SDRAM_BASE + MIN(gd->ram_size, (1*1024*1024));
#else
    /* Locate at top of first Megabyte */
	addr = CFG_SDRAM_BASE + MIN(gd->ram_size, MAX((1*1024*1024), (TOTAL_MALLOC_LEN + SLOP_K)));	// SLOP_K should only kick in if MALLOC length > 1M
	u_boot_mem_top = addr;


		/* We can reserve some RAM "on top" here.
		 */

		/* round down to next 4 kB limit.
		 */
	addr &= ~(4096 - 1);
#ifdef DEBUG
	printf ("Top of RAM usable for U-Boot at: %08lx\n", addr);
#endif


		/* Reserve memory for U-Boot code, data & bss
		 */
	addr -= MAX(len, (512*1024));
    /* round down to next 64k (allow us to create image at same addr for debugging)*/
	addr &= ~(64 * 1024 - 1);

#ifdef DEBUG
	printf ("Reserving %ldk for U-Boot at: %08lx\n", MAX(len, (512*1024)) >> 10, addr);
#endif

#endif  // CONFIG_RAM_RESIDENT


	 	/* Reserve memory for malloc() arena.
		 */
	addr_sp = addr - TOTAL_MALLOC_LEN;
#ifdef DEBUG
	printf ("Reserving %dk for malloc() at: %08lx\n",
			TOTAL_MALLOC_LEN >> 10, addr_sp);
#endif

	/*
	 * (permanently) allocate a Board Info struct
	 * and a permanent copy of the "global" data
	 */
	addr_sp -= sizeof(bd_t);
	bd = (bd_t *)addr_sp;
	gd->bd = bd;
#ifdef DEBUG
	printf ("Reserving %d Bytes for Board Info at: %08lx\n",
			sizeof(bd_t), addr_sp);
#endif
	addr_sp -= sizeof(gd_t);
	id = (gd_t *)addr_sp;
#ifdef DEBUG
	printf ("Reserving %d Bytes for Global Data at: %08lx\n",
			sizeof (gd_t), addr_sp);
#endif

	 	/* Reserve memory for boot params.
		 */
	addr_sp -= CFG_BOOTPARAMS_LEN;
	bd->bi_boot_params = addr_sp;
#ifdef DEBUG
	printf ("Reserving %dk for boot params at: %08lx\n",
			CFG_BOOTPARAMS_LEN >> 10, addr_sp);
#endif

	/*
	 * Finally, we set up a new (bigger) stack.
	 *
	 * Leave some safety gap for SP, force alignment on 16 byte boundary
	 * Clear initial stack frame
	 */
	addr_sp -= 16;
	addr_sp &= ~0xF;
    /* Changed to avoid lvalue cast warnings */
    uint32_t *tmp_ptr = (void *)addr_sp;
    *tmp_ptr-- = 0;
    *tmp_ptr-- = 0;
    addr_sp = (uint32_t)addr_sp;

#define STACK_SIZE  (16*1024UL)
    bd->bi_uboot_ram_addr = (addr_sp - STACK_SIZE) & ~(STACK_SIZE - 1);
    bd->bi_uboot_ram_used_size = u_boot_mem_top - bd->bi_uboot_ram_addr;

#ifdef DEBUG
	printf ("Stack Pointer at: %08lx, stack size: 0x%08x\n", addr_sp, STACK_SIZE);
    printf("Base DRAM address used by u-boot: 0x%08x, size: 0x%x\n", bd->bi_uboot_ram_addr, bd->bi_uboot_ram_used_size);
#endif
	/*
	 * Save local variables to board info struct
	 */
	bd->bi_memstart	= CFG_SDRAM_BASE;	/* start of  DRAM memory */
	bd->bi_memsize	= gd->ram_size;		/* size  of  DRAM memory in bytes */
	bd->bi_baudrate	= gd->baudrate;		/* Console Baudrate */

	memcpy (id, (void *)gd, sizeof (gd_t));

	/* On the purple board we copy the code in a special way
	 * in order to solve flash problems
	 */
#ifdef CONFIG_PURPLE
	copy_code(addr);
#endif
#ifdef DEBUG
        printf("relocating and jumping to code in DRAM\n");
#endif
	relocate_code (addr_sp, id, addr);

	/* NOTREACHED - relocate_code() does not return */
}
/************************************************************************
 *
 * This is the next part if the initialization sequence: we are now
 * running from RAM and have a "normal" C environment, i. e. global
 * data can be written, BSS has been cleared, the stack size in not
 * that critical any more, etc.
 *
 ************************************************************************
 */

void board_init_r (gd_t *id, ulong dest_addr, ulong gp_addr)
{
    DECLARE_GLOBAL_DATA_PTR;
    cmd_tbl_t *cmdtp;
    ulong size = 0;
    extern void malloc_bin_reloc (void);
#ifndef CFG_ENV_IS_NOWHERE
    extern char * env_name_spec;
#endif
    char *s, *e;
    bd_t *bd;
    int i;

/* Convince GCC to really do what we want, otherwise the old value is still used......*/
    SET_K0(id);

    gd->flags |= GD_FLG_RELOC;  /* tell others: relocation done */

#ifdef DEBUG
    /* This is the address that u-boot has relocated itself to.  Use this 
    ** address for RELOCATED_TEXT_BASE in the makefile to enable the relocated
    ** build for debug info. */
    printf ("Now running in RAM - U-Boot at: 0x%x\n", dest_addr);
#endif

    gd->reloc_off = dest_addr - CFG_MONITOR_BASE;

    monitor_flash_len = (ulong)&uboot_end_data - dest_addr;

    /*
     * We have to relocate the command table manually
     */
    if (gd->reloc_off)
    {
        for (cmdtp = (cmd_tbl_t *)(void *)&__u_boot_cmd_start; cmdtp !=  (cmd_tbl_t *)(void *)&__u_boot_cmd_end; cmdtp++)
        {
            ulong addr;

            addr = (ulong) (cmdtp->cmd) + gd->reloc_off;
            cmdtp->cmd =
            (int (*)(struct cmd_tbl_s *, int, int, char *[]))addr;

            addr = (ulong)(cmdtp->name) + gd->reloc_off;
            cmdtp->name = (char *)addr;

            if (cmdtp->usage)
            {
                addr = (ulong)(cmdtp->usage) + gd->reloc_off;
                cmdtp->usage = (char *)addr;
            }
#ifdef	CFG_LONGHELP
            if (cmdtp->help)
            {
                addr = (ulong)(cmdtp->help) + gd->reloc_off;
                cmdtp->help = (char *)addr;
            }
#endif
        }
        /* there are some other pointer constants we must deal with */
#ifndef CFG_ENV_IS_NOWHERE
        env_name_spec += gd->reloc_off;
#endif

    }  /* if (gd-reloc_off) */


    size = flash_init();
    display_flash_config (size);

    bd = gd->bd;
    bd->bi_flashstart = CFG_FLASH_BASE;
    bd->bi_flashsize = size;
#if CFG_MONITOR_BASE == CFG_FLASH_BASE
    bd->bi_flashoffset = monitor_flash_len; /* reserved area for U-Boot */
#else
    bd->bi_flashoffset = 0;
#endif

    /* initialize malloc() area */
    mem_malloc_init();
    malloc_bin_reloc();

#ifdef __TLV_INVENTORY__
    aruba_manuf_init_tlv();
#endif

    /* relocate environment function pointers etc. */
    env_relocate();

#if !CONFIG_OCTEON_SIM
    if (octeon_is_model(OCTEON_CN38XX))
    {
        extern int octeon_ipd_bp_enable(void);
        extern int octeon_ipd_bp_verify(void);
        int loops = octeon_ipd_bp_enable();
        if (loops < 37)
        {
            if (octeon_ipd_bp_verify())
                printf("IPD backpressure workaround verified, took %d loops\n", loops);
            else
                printf("IPD backpressure workaround FAILED verfication. Adjust octeon_ipd_bp_enable() for this chip!\n");
        }
        else
            printf("IPD backpressure workaround FAILED. Adjust octeon_ipd_bp_enable() for this chip!\n");
    }
#endif

#if !CONFIG_OCTEON_SIM 
    /* Flush and unlock L2 cache (soft reset does not unlock locked lines) */
    octeon_flush_l2_cache();
#endif

#if !CONFIG_OCTEON_SIM && !defined(CONFIG_NO_RELOCATION) && !defined(CONFIG_NO_CLEAR_DDR)
    /* Zero all of memory above DRAM_LATE_ZERO_OFFSET */
    printf("Clear:");
    uint64_t start_addr = DRAM_LATE_ZERO_OFFSET | (1ull << 63ull);
    uint64_t skip = DRAM_LATE_ZERO_OFFSET;

    if (TOTAL_MALLOC_LEN > DRAM_LATE_ZERO_OFFSET) {
	    start_addr = (TOTAL_MALLOC_LEN + (SLOP_K)) | (1ull << 63ull);
	    skip = (TOTAL_MALLOC_LEN + (SLOP_K));
    }
    octeon_bzero64_pfs(start_addr, (uint64_t)(MIN(256*1024*1024, gd->ram_size) - skip));
//    puts(".");

    if (gd->ram_size > 256*1024*1024)
    {
        octeon_bzero64_pfs(0x8000000410000000ull, (uint64_t)MIN((gd->ram_size - 256*1024*1024), 256*1024*1024));
        puts(".");
    }


    if (gd->ram_size > 512*1024*1024)
    {
        octeon_bzero64_pfs(0x8000000020000000ull, gd->ram_size - 512*1024*1024ull);
        puts(".");
    }
    printf(" done\n");
#endif

    if (!octeon_phy_mem_list_init(gd->ram_size, OCTEON_RESERVED_LOW_MEM_SIZE))
    {
        printf("FATAL: Error initializing free memory list\n");
        while (1);
    }
#ifdef DEBUG
    octeon_phy_mem_list_print(glob_phy_desc_ptr);
#endif

#if OCTEON_BOOTLOADER_RESERVED_MEM_SIZE && !CONFIG_OCTEON_SIM
    /* Here we will reserve some memory for bootloader use, freed just before app starts */
    octeon_phy_mem_named_block_alloc(glob_phy_desc_ptr, OCTEON_BOOTLOADER_RESERVED_MEM_SIZE,
                                     OCTEON_BOOTLOADER_RESERVED_MEM_BASE, OCTEON_BOOTLOADER_RESERVED_MEM_BASE + OCTEON_BOOTLOADER_RESERVED_MEM_SIZE,
                                     0, OCTEON_BOOTLOADER_RESERVED_MEM_NAME);
#endif

#if OCTEON_LINUX_RESERVED_MEM_SIZE && !CONFIG_OCTEON_SIM
    /* Reserve memory for Linux kernel.  The kernel requires specific physical addresses,
    ** so by reserving this here we can load simple exec applications and do other memory
    ** allocation without interfering with loading a kernel image.  This is freed and used
    ** when loading a Linux image.  If a Linux image is not loaded, then this is freed just
    ** before the applications start.
    */
    octeon_phy_mem_named_block_alloc(glob_phy_desc_ptr, OCTEON_LINUX_RESERVED_MEM_SIZE,
                                     OCTEON_LINUX_RESERVED_MEM_BASE, OCTEON_LINUX_RESERVED_MEM_BASE + OCTEON_LINUX_RESERVED_MEM_SIZE, 0, OCTEON_LINUX_RESERVED_MEM_NAME);
#endif

#ifdef DEBUG
    octeon_phy_mem_list_print(glob_phy_desc_ptr);
#endif


#if (CONFIG_OCTEON_EEPROM_TUPLES)
    {
        /* Read coremask from eeprom, and set environment variable if present.
        ** Print warning if not present in EEPROM.
        ** Currently ignores voltage/freq fields, and just uses first capability tuple
        */
        int addr;
        uint8_t ee_buf[OCTEON_EEPROM_MAX_TUPLE_LENGTH];
        octeon_eeprom_chip_capability_t *cc_ptr = (void *)ee_buf;
        addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_CHIP_CAPABILITY_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
        if (addr >= 0)
        {
            char tmp[10];
            sprintf(tmp,"0x%04x", cc_ptr->coremask);
            setenv("coremask_override", tmp);
            coremask_from_eeprom = cc_ptr->coremask;  /* Save copy for later verification */
        }
        else
        {
            /* No chip capability tuple found, so we need to check if we expect one.
            ** CN31XX chips will all have fuses blown appropriately.
            ** CN38XX chips may have fuses blown, but may not.  We will check to see if
            ** we need a chip capability tuple and only warn if we need it but don't have it.
            */

            if (OCTEON_CN38XX_PASS1 == octeon_get_proc_id())
            {
                /* always need tuple for pass 1 chips */
                printf("Warning: No chip capability tuple found in eeprom, coremask_override may be set incorrectly\n");
            }
            else if (octeon_is_model(OCTEON_CN38XX))
            {
                /* Here we only need it if no core fuses are blown and the lockdown fuse is not blown.
                ** In all other cases the cores fuses are definitive and we don't need a coremask override
                */
                if ((octeon_read64(CVMX_CIU_FUSE) & 0xffff) == 0xffff && !cvmx_octeon_fuse_locked())
                {
                    printf("Warning: No chip capability tuple found in eeprom, coremask_override may be set incorrectly\n");
                }
                else
                {
                    /* Clear coremask_override as we have a properly fused part, and don't need it */
                    setenv("coremask_override", NULL);
                }
            }
            else
            {
                /* 31xx and 30xx will always have core fuses blown appropriately */

                setenv("coremask_override", NULL);
            }
        }
    }
#endif


    /* Set numcores env variable to indicate the number of cores available */
    char tmp[10];
    sprintf(tmp,"%d", octeon_get_available_core_count());
    setenv("numcores", tmp);

#if (!CONFIG_OCTEON_SIM)
    octeon_bist();
#endif


    /* board MAC address */
    s = getenv ("ethaddr");
    for (i = 0; i < 6; ++i)
    {
        bd->bi_enetaddr[i] = s ? simple_strtoul (s, &e, 16) : 0;
        if (s)
            s = (*e) ? e + 1 : e;
    }

    /* IP Address */
    bd->bi_ip_addr = getenv_IPaddr("ipaddr");

#if defined(CONFIG_PCI)
    /*
     * Do pci configuration
     */
    pci_init();
#endif

/** leave this here (after malloc(), environment and PCI are working) **/
    /* Initialize devices */
    devices_init ();

    jumptable_init ();

    /* Initialize the console (after the relocation and devices init) */
    console_init_r ();
/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

    /* Initialize from environment */
    if ((s = getenv ("loadaddr")) != NULL)
    {
        load_addr = simple_strtoul (s, NULL, 16);
    }
#if (CONFIG_COMMANDS & CFG_CMD_NET)
    if ((s = getenv ("bootfile")) != NULL)
    {
        copy_filename (BootFile, s, sizeof (BootFile));
    }
#endif	/* CFG_CMD_NET */

#if defined(CONFIG_MISC_INIT_R)
    /* miscellaneous platform dependent initialisations */
    misc_init_r ();
#endif

#if CONFIG_OCTEON_LANBYPASS
    /* Set the LAN bypass defaults for Thunder */
    octeon_gpio_cfg_output(6); /* GPIO 6 controls the bypass enable */
    octeon_gpio_cfg_output(7); /* GPIO 7 controls the watchdog clear */
    octeon_gpio_cfg_input(5);  /* GPIO 5 tells you the bypass status */
    
    /* Startup with bypass disabled and watchdog cleared */
    octeon_gpio_clr(6);
    octeon_gpio_set(7);
#endif 

#if (CONFIG_COMMANDS & CFG_CMD_NET) && defined(CONFIG_NET_MULTI)
    puts ("Net:   ");
    eth_initialize(gd->bd);
#endif


#if CONFIG_OCTEON && !CONFIG_OCTEON_SIM && (CONFIG_COMMANDS & CFG_CMD_IDE)
#ifndef CONFIG_PCI  
    if (octeon_cf_present())
#endif
    {
        /* Always do ide_init if PCI enabled to allow for PCI IDE devices */
        ide_init();
    }
#endif

#if CONFIG_OCTEON
    /* verify that boot_init_vector type is the correct size */
    if (BOOT_VECTOR_NUM_WORDS*4 != sizeof(boot_init_vector_t))
    {
        printf("ERROR: boot_init_vector_t size mismatch: define: %d, type: %d\n",BOOT_VECTOR_NUM_WORDS*8,  sizeof(boot_init_vector_t));
        while (1)
            ;
    }
    /* DEBUGGER setup  */

#if !defined(CONFIG_ARUBA_OCTEON) && !defined(CONFIG_OCTEON_BBGW_REF)
    extern void cvmx_debugger_handler2(void);
    extern void cvmx_debugger_handler_pci(void);
    extern void cvmx_debugger_handler_pci_profiler(void);
    /* put relocated address of 2nd stage of debug interrupt
    ** at BOOTLOADER_DEBUG_TRAMPOLINE (word value)
    ** This only needs to be done once during initial boot
    */
    {
        /* Put address of cvmx_debugger_handler2() into fixed DRAM
        ** location so that minimal exception handler in flash
        ** can jump to this easily. Also put a list of the available handlers
        ** in a location accessible from PCI at a fixed address. Host PCI
        ** routines can then use this to replace the normal debug handler
        ** with one supporting other stuff.
        */
        uint32_t *ptr = (void *)BOOTLOADER_DEBUG_TRAMPOLINE;
        *ptr = (uint32_t)cvmx_debugger_handler2;
        ptr = (void*)BOOTLOADER_DEBUG_HANDLER_LIST;
        *ptr++ = (uint32_t)cvmx_debugger_handler2;
        *ptr++ = (uint32_t)cvmx_debugger_handler_pci;
        *ptr++ = (uint32_t)cvmx_debugger_handler_pci_profiler;
        *ptr = 0;

        /* Save the GP value currently used (after relocation to DRAM)
        ** so that debug stub can restore it before calls cvmx_debugger_handler2.
        */
        uint32_t tmp = 0x80000000 | BOOTLOADER_DEBUG_GP_REG;
        asm volatile ("sw    $28, 0(%[tmp])" : :[tmp] "r" (tmp) :"memory");
        /* Save K0 (global data pointer) so that debug stub can set up the correct
        ** context to use bootloader functions. */
        tmp = 0x80000000 | BOOTLOADER_DEBUG_KO_REG_SAVE;
        asm volatile ("sw    $26, 0(%[tmp])" : :[tmp] "r" (tmp) :"memory");
       
    }
#endif
#endif


#if CFG_LATE_BOARD_INIT
    late_board_init();
#endif

#ifdef ATH_DUAL_FLASH
	ath_nand_flash_init();
#endif

#ifdef __FIPS_BUILD__
    fips_post_status = fips_post_process(0); // Perform FIPS POST
    if(0 == fips_post_status)
        printf("FIPS: passed \n");
#endif

    /* main_loop() can return to retry autoboot, if so just run it again. */
    for (;;)
    {
        main_loop ();
    }

    /* NOTREACHED - no way out of command loop except booting */
}

void hang (void)
{
	puts ("### ERROR ### Please RESET the board ###\n");
	for (;;);
}
