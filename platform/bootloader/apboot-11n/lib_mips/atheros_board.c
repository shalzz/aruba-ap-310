//#define DEBUG
/*
 * (C) Copyright 2003
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

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <devices.h>
#include <version.h>
#include <net.h>
#include <environment.h>
#ifdef CONFIG_AR7100
#include <ar7100_soc.h>
#elif defined(CONFIG_ATHEROS)
#include <atheros.h>
#else
#include <ar7240_soc.h>
#endif
#include <watchdog.h>
#ifdef __FIPS_BUILD__
#include "../include/xyssl/rsa.h"
#endif

DECLARE_GLOBAL_DATA_PTR;

#if ( ((CFG_ENV_ADDR+CFG_ENV_SIZE) < CFG_MONITOR_BASE) || \
      (CFG_ENV_ADDR >= (CFG_MONITOR_BASE + CFG_MONITOR_LEN)) ) || \
    defined(CFG_ENV_IS_IN_NVRAM)
#define	TOTAL_MALLOC_LEN	(CFG_MALLOC_LEN + CFG_ENV_SIZE)
#else
#define	TOTAL_MALLOC_LEN	CFG_MALLOC_LEN
#endif

//#undef DEBUG

#ifdef __FIPS_BUILD__
int fips_post_status = 0;
#endif

#ifdef CONFIG_APBOOT
extern int ar7100_testdram(void);
#endif

int early_board_init(void);
extern int timer_init(void);

extern int incaip_set_cpuclk(void);

#if defined(CONFIG_WASP_SUPPORT) || defined(CONFIG_MACH_QCA955x)
void ath_set_tuning_caps(void);
#endif


extern ulong uboot_end_data;
extern ulong uboot_end;

ulong monitor_flash_len;

const char version_string[] = U_BOOT_VERSION;
#ifdef CFG_LATE_BOARD_INIT
extern int late_board_init(void);
#endif

static char *failed = "*** failed ***\n";
#ifdef __TLV_INVENTORY__
extern void aruba_manuf_init_tlv(void);
#endif

#ifdef __BOARD_HAS_POWER_INFO__
extern void board_print_power_status(void);
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
	ulong dest_addr = CFG_MONITOR_BASE + gd->reloc_off;

	mem_malloc_end = dest_addr;
	mem_malloc_start = dest_addr - TOTAL_MALLOC_LEN;
	mem_malloc_brk = mem_malloc_start;

	memset ((void *) mem_malloc_start,
		0,
		mem_malloc_end - mem_malloc_start);
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
#ifdef	CONFIG_BOARD_TYPES
	int board_type = gd->board_type;
#else
	int board_type = 0;	/* use dummy arg */
#endif
	puts ("DRAM:  ");

	if ((gd->ram_size = initdram (board_type)) > 0) {
		print_size (gd->ram_size, "\n");
#ifdef CONFIG_APBOOT
		ar7100_testdram();
#endif
		return (0);
	}
	puts (failed);
	return (1);
}

static int display_banner(void)
{
#ifdef CONFIG_HW_WATCHDOG
#ifdef CONFIG_AR7100
	extern void ar7100_watchdog_check(void);
#elif defined(CONFIG_ATHEROS)
	extern void ath_watchdog_check(void);
#else
	extern void ar7240_watchdog_check(void);
#endif
#endif
	extern char build_num[], build_time[];

#ifdef CONFIG_HW_WATCHDOG
#ifdef CONFIG_AR7100
	ar7100_watchdog_check();
#elif defined(CONFIG_ATHEROS)
	ath_watchdog_check();
#else
	ar7240_watchdog_check();
#endif
#endif

	printf ("\n\n%s (build %s)\n", version_string, build_num);
	printf ("Built: %s\n\n", build_time);

#ifdef __MODEL_STR__
	printf("Model: %s\n", __MODEL_STR__);
#endif

#ifdef __CPU_ID__
#ifdef CONFIG_AR7100
	ar7100_cpu_id();
#elif defined(CONFIG_ATHEROS)
	ath_cpu_id();
#else
	ar7240_cpu_id();
#endif
#endif
	return (0);
}

static void display_flash_config(ulong size)
{
	if (size < (10 * 1024 * 1024)) {
		puts ("Flash:");
	} else {
		puts ("Flash: ");
	}
#if AP_PROVISIONING_IN_BANK2
	print_size (size * 2, "\n");
#else
	print_size (size, "\n");
#endif
}


static int init_baudrate (void)
{
	char tmp[64];	/* long enough for environment variables */
	int i = getenv_r ("baudrate", tmp, sizeof (tmp));

	gd->baudrate = (i > 0)
			? (int) simple_strtoul (tmp, NULL, 10)
			: CONFIG_BAUDRATE;

	return (0);
}


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
typedef int (init_fnc_t) (void);

init_fnc_t *init_sequence[] = {
	timer_init,
	env_init,		/* initialize environment */
#ifdef CONFIG_INCA_IP
	incaip_set_cpuclk,	/* set cpu clock according to environment variable */
#endif
#if defined(CONFIG_AR7100) || defined(CONFIG_AR7240) || defined(CONFIG_ATHEROS)
	early_board_init,
#endif
	init_baudrate,		/* initialze baudrate settings */
	serial_init,		/* serial communications setup */
	console_init_f,
	display_banner,		/* say that we are here */
	checkboard,
	init_func_ram,
	NULL,
};


void board_init_f(ulong bootflag)
{
	gd_t gd_data, *id;
	bd_t *bd;
	init_fnc_t **init_fnc_ptr;
	ulong addr, addr_sp, len = (ulong)&uboot_end - CFG_MONITOR_BASE;
	ulong *s;
#ifdef CONFIG_PURPLE
	void copy_code (ulong);
#endif

	/* Pointer is writable since we allocated a register for it.
	 */
	gd = &gd_data;
	/* compiler optimization barrier needed for GCC >= 3.4 */
	__asm__ __volatile__("": : :"memory");

	memset ((void *)gd, 0, sizeof (gd_t));

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}

	/*
	 * Now that we have DRAM mapped and working, we can
	 * relocate the code and continue running from DRAM.
	 */
	addr = CFG_SDRAM_BASE + gd->ram_size;

	/* We can reserve some RAM "on top" here.
	 */

	/* round down to next 4 kB limit.
	 */
	addr &= ~(4096 - 1);
	debug ("Top of RAM usable for U-Boot at: %08lx\n", addr);

	/* Reserve memory for U-Boot code, data & bss
	 * round down to next 16 kB limit
	 */
	addr -= len;
	addr &= ~(16 * 1024 - 1);

	debug ("Reserving %ldk for U-Boot at: %08lx\n", len >> 10, addr);

	 /* Reserve memory for malloc() arena.
	 */
	addr_sp = addr - TOTAL_MALLOC_LEN;
	debug ("Reserving %dk for malloc() at: %08lx\n",
			TOTAL_MALLOC_LEN >> 10, addr_sp);

	/*
	 * (permanently) allocate a Board Info struct
	 * and a permanent copy of the "global" data
	 */
	addr_sp -= sizeof(bd_t);
	bd = (bd_t *)addr_sp;
	gd->bd = bd;
	debug ("Reserving %d Bytes for Board Info at: %08lx\n",
			sizeof(bd_t), addr_sp);

	addr_sp -= sizeof(gd_t);
	id = (gd_t *)addr_sp;
	debug ("Reserving %d Bytes for Global Data at: %08lx\n",
			sizeof (gd_t), addr_sp);

 	/* Reserve memory for boot params.
	 */
	addr_sp -= CFG_BOOTPARAMS_LEN;
	bd->bi_boot_params = addr_sp;
	debug ("Reserving %dk for boot params() at: %08lx\n",
			CFG_BOOTPARAMS_LEN >> 10, addr_sp);

	/*
	 * Finally, we set up a new (bigger) stack.
	 *
	 * Leave some safety gap for SP, force alignment on 16 byte boundary
	 * Clear initial stack frame
	 */
	addr_sp -= 16;
	addr_sp &= ~0xF;
	s = (ulong *)addr_sp;
	*s-- = 0;
	*s-- = 0;
	addr_sp = (ulong)s;
	debug ("Stack Pointer at: %08lx\n", addr_sp);

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

#if 0
        { int i;
	for (i = 0; i < 16; i++) {
	     debug ("Memory register %08lx: %08lx\n", 0xb8000000 + (4 * i),
	     	*(volatile unsigned *)(0xb8000000 + (4 * i)));
	}
	for (i = 0; i < 8; i++) {
	     debug ("PLL register %08lx: %08lx\n", 0xb8050000 + (4 * i),
	     	*(volatile unsigned *)(0xb8050000 + (4 * i)));
	}
	}
#endif
        debug ("Copy to addr %08lx %08lx %08lx\n", addr, addr_sp, id);
#ifdef ARUBA_AR7100
	printf("Copy:  ");
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

void board_init_r (gd_t *id, ulong dest_addr)
{
	cmd_tbl_t *cmdtp;
	ulong size;
	extern void malloc_bin_reloc (void);
#ifndef CFG_ENV_IS_NOWHERE
	extern char * env_name_spec;
#endif
	char *s, *e;
	bd_t *bd;
	int i;

	gd = id;
	gd->flags |= GD_FLG_RELOC;	/* tell others: relocation done */

#ifdef CONFIG_HW_WATCHDOG
	WATCHDOG_RESET();
#endif

#ifdef ARUBA_AR7100
	printf("\b \b\b \b\b \b\b \bdone\n");
#endif
	debug ("Now running in RAM - U-Boot at: %08lx; GD %08lx\n", dest_addr, gd);

	gd->reloc_off = dest_addr - CFG_MONITOR_BASE;
	debug ("A %08lx\n", gd->reloc_off);

	monitor_flash_len = (ulong)&uboot_end_data - dest_addr;
	debug ("B %08lx\n", monitor_flash_len);

	/*
	 * We have to relocate the command table manually
	 */
	debug ("C %08lx/%08lx\n", &__u_boot_cmd_start, &__u_boot_cmd_end);
 	for (cmdtp = (cmd_tbl_t *)(void *)&__u_boot_cmd_start; cmdtp !=  (cmd_tbl_t *)(void *)&__u_boot_cmd_end; cmdtp++) {
		ulong addr;

		addr = (ulong) (cmdtp->cmd) + gd->reloc_off;
		cmdtp->cmd =
			(int (*)(struct cmd_tbl_s *, int, int, char *[]))addr;

		addr = (ulong)(cmdtp->name) + gd->reloc_off;
#ifdef DEBUG
		printf ("Command \"%p\": 0x%08lx => 0x%08lx\n",
				cmdtp->name, (ulong) cmdtp, addr);
#endif
		cmdtp->name = (char *)addr;

		if (cmdtp->usage) {
			addr = (ulong)(cmdtp->usage) + gd->reloc_off;
			cmdtp->usage = (char *)addr;
		}
#ifdef	CFG_LONGHELP
		if (cmdtp->help) {
			addr = (ulong)(cmdtp->help) + gd->reloc_off;
			cmdtp->help = (char *)addr;
		}
#endif
#ifdef CONFIG_HW_WATCHDOG
	        WATCHDOG_RESET();
#endif
	}
	debug ("D\n");
	/* there are some other pointer constants we must deal with */
#ifndef CFG_ENV_IS_NOWHERE
	env_name_spec += gd->reloc_off;
#endif

	/* configure available FLASH banks */
	size = flash_init();
	display_flash_config (size);

	bd = gd->bd;
	bd->bi_flashstart = CFG_FLASH_BASE;
	bd->bi_flashsize = size;
#if CFG_MONITOR_BASE == CFG_FLASH_BASE
	bd->bi_flashoffset = monitor_flash_len;	/* reserved area for U-Boot */
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

	/* board MAC address */
	s = getenv ("ethaddr");
	for (i = 0; i < 6; ++i) {
		bd->bi_enetaddr[i] = s ? simple_strtoul (s, &e, 16) : 0;
		if (s)
			s = (*e) ? e + 1 : e;
	}

#ifdef __BOARD_HAS_POWER_INFO__
	board_print_power_status();
#endif

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
	if ((s = getenv ("loadaddr")) != NULL) {
		load_addr = simple_strtoul (s, NULL, 16);
	}
#if (CONFIG_COMMANDS & CFG_CMD_NET)
	if ((s = getenv ("bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof (BootFile));
	}
#endif	/* CFG_CMD_NET */

#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r ();
#endif

#if (CONFIG_COMMANDS & CFG_CMD_NET)
#if defined(CONFIG_NET_MULTI)
	puts ("Net:   ");
#endif
	eth_initialize(gd->bd);
#endif

#if CFG_LATE_BOARD_INIT
        late_board_init();
#endif

#if defined(CONFIG_WASP_SUPPORT) || defined(CONFIG_MACH_QCA955x)
        ath_set_tuning_caps(); /* Needed here not to mess with Ethernet clocks */
#endif

#ifdef __FIPS_BUILD__
    fips_post_status = fips_post_process(0); // Perform FIPS POST
    if(0 == fips_post_status)
        printf("FIPS: passed \n");
#endif
	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;) {
		main_loop ();
	}

	/* NOTREACHED - no way out of command loop except booting */
}

void hang (void)
{
	puts ("### ERROR ### Please RESET the board ###\n");
	for (;;);
}
