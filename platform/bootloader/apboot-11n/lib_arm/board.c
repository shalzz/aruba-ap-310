/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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
#ifdef CFG_ENV_IS_IN_SPI_FLASH
#include <spi_flash.h>
#endif

#ifdef __FIPS_BUILD__
#include "../include/xyssl/rsa.h"
#endif

#ifdef CFG_LATE_BOARD_INIT
extern void late_board_init(void);
#endif
#ifdef CONFIG_APBOOT
extern int early_board_init(void);
#endif
#if defined(CONFIG_MARVELL)
 extern int PTexist(void);
// extern unsigned long mvFlash_init (void);
 extern unsigned int whoAmI(void);
 extern int cpuMapInit (void);
#if defined(MV78200)
 extern void second_cpu_realloc_and_load(void);
#include "../board/mv_feroceon/common/mvTypes.h"
#include "../board/mv_feroceon/mv_dd/dd_family/mv78200/mvSocUnitMap.h"
#endif
#endif
#ifdef CONFIG_DRIVER_SMC91111
#include "../drivers/smc91111.h"
#endif
#ifdef CONFIG_DRIVER_LAN91C96
#include "../drivers/lan91c96.h"
#endif

#ifdef __FIPS_BUILD__
int fips_post_status = 0;
#endif

#if (CONFIG_COMMANDS & CFG_CMD_NAND)
void nand_init (void);
#endif

ulong monitor_flash_len;

#ifdef CONFIG_HAS_DATAFLASH
extern int  AT91F_DataflashInit(void);
extern void dataflash_print_info(void);
#endif

#ifndef CONFIG_IDENT_STRING
#define CONFIG_IDENT_STRING ""
#endif

#ifdef __BOARD_HAS_POWER_INFO__
extern void board_print_power_status(void);
#endif

const char version_string[] =
#ifdef CONFIG_APBOOT
	U_BOOT_VERSION;
#else
	U_BOOT_VERSION" (" __DATE__ " - " __TIME__ ")"CONFIG_IDENT_STRING;
#endif

#ifdef CONFIG_DRIVER_CS8900
extern void cs8900_get_enetaddr (uchar * addr);
#endif

#ifdef CONFIG_DRIVER_RTL8019
extern void rtl8019_get_enetaddr (uchar * addr);
#endif

#if (CONFIG_COMMANDS & CFG_CMD_RCVR) 
extern void recoveryCheck(void);
#endif

#ifdef __TLV_INVENTORY__
extern void aruba_manuf_init_tlv(void);
#endif

/*
 * Begin and End of memory area for malloc(), and current "brk"
 */
static ulong mem_malloc_start = 0;
static ulong mem_malloc_end = 0;
static ulong mem_malloc_brk = 0;

static
void mem_malloc_init (ulong dest_addr)
{
#ifndef CONFIG_MARVELL
	mem_malloc_start = dest_addr;
	mem_malloc_end = dest_addr + CFG_MALLOC_LEN;
	mem_malloc_brk = mem_malloc_start;

	memset ((void *) mem_malloc_start, 0,
			mem_malloc_end - mem_malloc_start);
#else
        unsigned int malloc_len;
#ifndef CONFIG_APBOOT
        char *env;
#endif

#if defined(CONFIG_MARVELL) && defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
		unsigned int cfg_malloc_len = CFG_MALLOC_LEN/2;
		unsigned int cfg_malloc_base = CFG_MALLOC_BASE+(whoAmI()*cfg_malloc_len); 
#else
#ifndef CONFIG_APBOOT
		unsigned int cfg_malloc_len = CFG_MALLOC_LEN;
#endif
		unsigned int cfg_malloc_base = CFG_MALLOC_BASE;
#endif

#ifdef CONFIG_APBOOT
	malloc_len = 1 << 20;
#else
        env = getenv("MALLOC_len");
        malloc_len =  simple_strtoul(env, NULL, 10) << 20;
        if(malloc_len == 0)
                malloc_len = cfg_malloc_len;
#endif
  
		mem_malloc_end = cfg_malloc_base + malloc_len;
#ifndef CONFIG_APBOOT
        printf("Addresses %ldM - 0M are saved for the U-Boot usage.\n",mem_malloc_end >> 20);
#endif

        mem_malloc_start = cfg_malloc_base;
        mem_malloc_brk = mem_malloc_start;
  
#ifndef CONFIG_APBOOT
        printf("Mem malloc Initialization (%dM - %dM):",mem_malloc_end >> 20,
                                                            mem_malloc_start >>20 );
#endif
        memset ((void *) mem_malloc_start,0,mem_malloc_end - mem_malloc_start);
  
#ifndef CONFIG_APBOOT
        printf(" Done\n");
#endif

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

/************************************************************************
 * Init Utilities							*
 ************************************************************************
 * Some of this code should be moved into the core functions,
 * or dropped completely,
 * but let's get it working (again) first...
 */

static int init_baudrate (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	uchar tmp[64];	/* long enough for environment variables */
	int i = getenv_r ("baudrate", tmp, sizeof (tmp));
	gd->bd->bi_baudrate = gd->baudrate = (i > 0)
			? (int) simple_strtoul (tmp, NULL, 10)
			: CONFIG_BAUDRATE;

	return (0);
}

static int display_banner (void)
{
	extern char build_num[], build_time[];
	printf ("\n\n%s (build %s)\n", version_string, build_num);
	printf ("Built: %s\n\n", build_time);

#ifdef __MODEL_STR__
	printf("Model: %s\n", __MODEL_STR__);
#endif

#ifdef DEBUG
	printf ("U-Boot code: %08lX -> %08lX  BSS: -> %08lX\n",
		_armboot_start, _bss_start, _bss_end);
#endif
#ifdef CONFIG_MODEM_SUPPORT
	puts ("Modem Support enabled\n");
#endif
#ifdef CONFIG_USE_IRQ
	printf ("IRQ Stack: %08lx\n", IRQ_STACK_START);
	printf ("FIQ Stack: %08lx\n", FIQ_STACK_START);
#endif

	return (0);
}

/*
 * WARNING: this code looks "cleaner" than the PowerPC version, but
 * has the disadvantage that you either get nothing, or everything.
 * On PowerPC, you might see "DRAM: " before the system hangs - which
 * gives a simple yet clear indication which part of the
 * initialization if failing.
 */
#ifndef CONFIG_MARVELL

static int display_dram_config (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	int i;

	puts ("RAM Configuration:\n");

	for(i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		printf ("Bank #%d: %08lx ", i, gd->bd->bi_dram[i].start);
		print_size (gd->bd->bi_dram[i].size, "\n");
	}

	return (0);
}

#endif

static void display_flash_config (ulong size)
{
	puts ("Flash: ");
	print_size (size, "\n");
}


/*
 * Breathe some life into the board...
 *
 * Initialize a serial port as console, and carry out some hardware
 * tests.
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependent #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t) (void);

init_fnc_t *init_sequence[] = {
	cpu_init,		/* basic cpu dependent setup */
	board_init,		/* basic board dependent setup */
	interrupt_init,		/* set up exceptions */
	env_init,		/* initialize environment */
#ifdef CONFIG_APBOOT
	early_board_init,
#endif
	init_baudrate,		/* initialze baudrate settings */
	serial_init,		/* serial communications setup */
	console_init_f,		/* stage 1 init of console */
	display_banner,		/* say that we are here */
#if defined(CONFIG_MARVELL) && defined(MV78XX0)
	cpuMapInit,
#endif
	dram_init,		/* configure available RAM banks */
#ifndef CONFIG_MARVELL
	display_dram_config,
#endif
#if defined(CONFIG_VCMA9) || defined (CONFIG_CMC_PU2) || defined(CONFIG_APBOOT)
	checkboard,
#endif
	NULL,
};

#if defined(CONFIG_MARVELL) && defined(MV78200)
init_fnc_t *init_sequence_slave[] = {
	cpu_init,		/* basic cpu dependent setup */
	board_init,		/* basic board dependent setup */
	interrupt_init,		/* set up exceptions */
	env_init,
	serial_init,		/* serial communications setup */
	console_init_f,		/* stage 1 init of console */
	display_banner,		/* say that we are here */
	NULL,			/* Terminate this list */
};
#endif
#ifdef CFG_ENV_IS_IN_SPI_FLASH
extern struct spi_flash *flash;
#endif

void start_armboot (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	ulong size;
	init_fnc_t **init_fnc_ptr;
	char *s;
#if defined(MV78200) && (defined(DUAL_OS_78200) || defined(DUAL_OS_SHARED_MEM_78200))
	char *env;
#endif
#if defined(CONFIG_MARVELL) && defined(MV78200)
	volatile unsigned int cpu = 0;
#endif
//	int nand_access = 0;
#if defined(CONFIG_VFD) || defined(CONFIG_LCD)
	unsigned long addr;
#endif
	/* Pointer is writable since we allocated a register for it */
#ifndef	CONFIG_MARVELL
	gd = (gd_t*)(_armboot_start - CFG_MALLOC_LEN - sizeof(gd_t));
#else
#if defined(CONFIG_MARVELL) && defined(MV78200)
   /* Marvell Master CPU Boot */
    cpu = whoAmI();
#if !defined(DUAL_OS_78200)
   if(cpu == 0)
#endif
#endif
	gd = (gd_t*)(_armboot_start - sizeof(gd_t));
#endif
	/* compiler optimization barrier needed for GCC >= 3.4 */
	__asm__ __volatile__("": : :"memory");

#if defined(CONFIG_MARVELL) && defined(MV78200)
   /* Marvell Master CPU Boot */
#if !defined(DUAL_OS_78200)
   if(cpu == 0)
	{
#endif
#endif
	memset ((void*)gd, 0, sizeof (gd_t));
	gd->bd = (bd_t*)((char*)gd - sizeof(bd_t));
	memset (gd->bd, 0, sizeof (bd_t));

	monitor_flash_len = _bss_start - _armboot_start;

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}
	/* armboot_start is defined in the board-specific linker script */
	mem_malloc_init (_armboot_start - CFG_MALLOC_LEN);


#if !(defined(MV78200) && (defined(DUAL_OS_78200) || \
	(defined(DUAL_OS_SHARED_MEM_78200) && !defined(MV_NAND_BOOT))))	
#if (CONFIG_COMMANDS & CFG_CMD_NAND)
    puts ("NAND:");
    nand_init();		/* go init the NAND */
#endif
#endif

#ifndef CFG_NO_FLASH
	/* configure available FLASH banks */
	size = flash_init ();
	/* initialize environment */
#ifdef __TLV_INVENTORY__
	aruba_manuf_init_tlv();
#endif
	env_relocate ();
#if defined(CONFIG_MARVELL)
//	size += mvFlash_init ();
#endif
#ifdef CFG_ENV_IS_IN_SPI_FLASH
	size = flash->size;
#endif
	display_flash_config (size);
#endif /* CFG_NO_FLASH */

#ifdef __BOARD_HAS_POWER_INFO__
	board_print_power_status();
#endif

#if defined(MV78200) && (defined(DUAL_OS_78200) || defined(DUAL_OS_SHARED_MEM_78200))
	env = getenv("cpu0_res");
	if (env)
		mvSocUnitMapFillTable(env, MASTER_CPU, strstr);
	env = getenv("cpu1_res");
	if (env)
		mvSocUnitMapFillTable(env, SLAVE_CPU, strstr);
#endif

#ifdef CONFIG_VFD
#	ifndef PAGE_SIZE
#	  define PAGE_SIZE 4096
#	endif
	/*
	 * reserve memory for VFD display (always full pages)
	 */
	/* bss_end is defined in the board-specific linker script */
	addr = (_bss_end + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
	size = vfd_setmem (addr);
	gd->fb_base = addr;
#endif /* CONFIG_VFD */

#ifdef CONFIG_LCD
#	ifndef PAGE_SIZE
#	  define PAGE_SIZE 4096
#	endif
	/*
	 * reserve memory for LCD display (always full pages)
	 */
	/* bss_end is defined in the board-specific linker script */
	addr = (_bss_end + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
	size = lcd_setmem (addr);
	gd->fb_base = addr;
#endif /* CONFIG_LCD */

#if defined(MV78200) && (defined(DUAL_OS_78200) || \
	(defined(DUAL_OS_SHARED_MEM_78200) && !defined(MV_NAND_BOOT)))
#if (CONFIG_COMMANDS & CFG_CMD_NAND)
    /* Check in dual CPU system which CPU use nand */
    if (mvSocUnitIsMappedToThisCpu(NAND_FLASH))
    {
    	puts ("NAND:");
    	nand_init();		/* go init the NAND */
    }
#endif
#endif

#ifdef CONFIG_HAS_DATAFLASH
	AT91F_DataflashInit();
	dataflash_print_info();
#endif

#ifdef CFG_DIAG
#define DIAGNOSTICS
#ifdef DIAGNOSTICS
       if( !strcmp(getenv("run_diag"), "yes") ||
           !strcmp(getenv("run_diag"), "YES"))
               run_diag();
#endif
#endif

#ifdef CONFIG_VFD
	/* must do this after the framebuffer is allocated */
	drv_vfd_init();
#endif /* CONFIG_VFD */

	/* IP Address */
#if defined(CONFIG_MARVELL) && defined(DUAL_OS_78200)
	if(cpu == 0)
	gd->bd->bi_ip_addr = getenv_IPaddr ("ipaddr");
	else
	    gd->bd->bi_ip_addr = getenv_IPaddr ("ipaddr2");
#else
	gd->bd->bi_ip_addr = getenv_IPaddr ("ipaddr");
#endif

	/* MAC Address */
	{
		int i;
		ulong reg;
		char *s, *e;
		uchar tmp[64];

		i = getenv_r ("ethaddr", tmp, sizeof (tmp));
		s = (i > 0) ? tmp : NULL;

		for (reg = 0; reg < 6; ++reg) {
			gd->bd->bi_enetaddr[reg] = s ? simple_strtoul (s, &e, 16) : 0;
			if (s)
				s = (*e) ? e + 1 : e;
		}
	}

	devices_init ();	/* get the devices list going. */

#ifdef CONFIG_CMC_PU2
	load_sernum_ethaddr ();
#endif /* CONFIG_CMC_PU2 */

	jumptable_init ();

	console_init_r ();	/* fully init console as a device */

#if defined(CONFIG_MISC_INIT_R)
	/* miscellaneous platform dependent initialisations */
	misc_init_r ();
#endif
	/* enable exceptions */
	enable_interrupts ();

	/* Perform network card initialisation if necessary */
#ifdef CONFIG_DRIVER_CS8900
	cs8900_get_enetaddr (gd->bd->bi_enetaddr);
#endif

#if defined(CONFIG_DRIVER_SMC91111) || defined (CONFIG_DRIVER_LAN91C96)
	if (getenv ("ethaddr")) {
		smc_set_mac_addr(gd->bd->bi_enetaddr);
	}
#endif /* CONFIG_DRIVER_SMC91111 || CONFIG_DRIVER_LAN91C96 */

	/* Initialize from environment */
	if ((s = getenv ("loadaddr")) != NULL) {
		load_addr = simple_strtoul (s, NULL, 16);
	}
#if (CONFIG_COMMANDS & CFG_CMD_NET)
	if ((s = getenv ("bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof (BootFile));
	}
#endif	/* CFG_CMD_NET */

#ifdef BOARD_LATE_INIT
	board_late_init ();
#endif
#if (CONFIG_COMMANDS & CFG_CMD_SCSI)
	puts ("SCSI:  ");
	scsi_init ();
#endif
#if (CONFIG_COMMANDS & CFG_CMD_NET)
#if defined(CONFIG_NET_MULTI)
	puts ("Net:   ");
#endif
	eth_initialize(gd->bd);
#endif
#if (CONFIG_COMMANDS & CFG_CMD_RCVR)
	recoveryCheck(); 
#endif
/* Cancle SILENT mode for prompt only */
#if defined(CONFIG_MARVELL) && defined(CONFIG_SILENT_CONSOLE)
	DECLARE_GLOBAL_DATA_PTR;
	gd->flags &= ~GD_FLG_SILENT;
#endif
#if defined(CONFIG_MARVELL) && defined(MV78200)
#if !defined(DUAL_OS_78200)
   /* Marvell Second CPU Boot */
    }
    else
    {
	/* Master CPU global data */
	gd_t *gd_master;	
	gd_master = (gd_t*)(_armboot_start - sizeof(gd_t));	
	/* Slave CPU global data */
	gd = (gd_t*)(_armboot_start - _1M - sizeof(gd_t));
	memset ((void *) gd, 0, sizeof (gd_t));
	memcpy ((void *)gd, (const void *)gd_master, sizeof (gd_t));
	gd->bd = (bd_t*)((char*)gd - sizeof(bd_t));
	memset ((void *) gd->bd, 0, sizeof (bd_t));

	/*copy the Global Data from the Master. */
	memcpy ((void *)gd->bd, (const void *)gd_master->bd, sizeof (bd_t));
	for (init_fnc_ptr = init_sequence_slave; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}
	gd->env_addr = gd_master->env_addr;
#if defined(DUAL_OS_SHARED_MEM_78200)
	/* armboot_start is defined in the board-specific linker script */
	mem_malloc_init (_armboot_start - CFG_MALLOC_LEN);
	env = getenv("cpu0_res");
	if (env)
		mvSocUnitMapFillTable(env, MASTER_CPU, strstr);
	env = getenv("cpu1_res");
	if (env)
		mvSocUnitMapFillTable(env, SLAVE_CPU, strstr);
	gd->bd->bi_ip_addr = getenv_IPaddr ("ipaddr2");
#endif
	misc_init_r ();
#if (CONFIG_COMMANDS & CFG_CMD_SCSI)
	puts ("SCSI:  ");
	scsi_init ();
#endif

#if (CONFIG_COMMANDS & CFG_CMD_NET)
#if defined(CONFIG_NET_MULTI)
	puts ("Net:   ");
#endif
	eth_initialize(gd->bd);
#endif

    }
#endif
	if (cpu == 0) {
		second_cpu_realloc_and_load();
	}	
#endif
#ifdef CFG_LATE_BOARD_INIT
	late_board_init();
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

#ifdef CONFIG_MODEM_SUPPORT
/* called from main loop (common/main.c) */
extern void  dbg(const char *fmt, ...);
int mdm_init (void)
{
	char env_str[16];
	char *init_str;
	int i;
	extern char console_buffer[];
	static inline void mdm_readline(char *buf, int bufsiz);
	extern void enable_putc(void);
	extern int hwflow_onoff(int);

	enable_putc(); /* enable serial_putc() */

#ifdef CONFIG_HWFLOW
	init_str = getenv("mdm_flow_control");
	if (init_str && (strcmp(init_str, "rts/cts") == 0))
		hwflow_onoff (1);
	else
		hwflow_onoff(-1);
#endif

	for (i = 1;;i++) {
		sprintf(env_str, "mdm_init%d", i);
		if ((init_str = getenv(env_str)) != NULL) {
			serial_puts(init_str);
			serial_puts("\n");
			for(;;) {
				mdm_readline(console_buffer, CFG_CBSIZE);
				dbg("ini%d: [%s]", i, console_buffer);

				if ((strcmp(console_buffer, "OK") == 0) ||
					(strcmp(console_buffer, "ERROR") == 0)) {
					dbg("ini%d: cmd done", i);
					break;
				} else /* in case we are originating call ... */
					if (strncmp(console_buffer, "CONNECT", 7) == 0) {
						dbg("ini%d: connect", i);
						return 0;
					}
			}
		} else
			break; /* no init string - stop modem init */

		udelay(100000);
	}

	udelay(100000);

	/* final stage - wait for connect */
	for(;i > 1;) { /* if 'i' > 1 - wait for connection
				  message from modem */
		mdm_readline(console_buffer, CFG_CBSIZE);
		dbg("ini_f: [%s]", console_buffer);
		if (strncmp(console_buffer, "CONNECT", 7) == 0) {
			dbg("ini_f: connected");
			return 0;
		}
	}

	return 0;
}

/* 'inline' - We have to do it fast */
static inline void mdm_readline(char *buf, int bufsiz)
{
	char c;
	char *p;
	int n;

	n = 0;
	p = buf;
	for(;;) {
		c = serial_getc();

		/*		dbg("(%c)", c); */

		switch(c) {
		case '\r':
			break;
		case '\n':
			*p = '\0';
			return;

		default:
			if(n++ > bufsiz) {
				*p = '\0';
				return; /* sanity check */
			}
			*p = c;
			p++;
			break;
		}
	}
}
#endif	/* CONFIG_MODEM_SUPPORT */
