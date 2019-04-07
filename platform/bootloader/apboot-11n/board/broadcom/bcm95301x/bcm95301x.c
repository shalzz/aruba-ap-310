#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <configs/iproc_board.h>
#include <asm/arch/iproc.h>
#include <asm/arch/socregs.h>
#include <asm/arch/reg_utils.h>
#include <asm/system.h>
#include <watchdog.h>

DECLARE_GLOBAL_DATA_PTR;

extern int bcmiproc_eth_register(u8 dev_num);
extern void l2cc_unlock(void);
extern void iproc_clk_enum(void);

int checkboard(void)
{
#ifdef CONFIG_HW_WATCHDOG
    extern void hw_watchdog_init(void);

    hw_watchdog_init();
    WATCHDOG_RESET();
#endif

    return 0;
}

/*****************************************
 * board_init -early hardware init
 *****************************************/
int board_init (void)
{
    gd->bd->bi_arch_number = CONFIG_MACH_TYPE;      /* board id for linux */
    gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR; /* adress of boot parameters */

    return 0;
}

/*****************************************************************
 * misc_init_r - miscellaneous platform dependent initializations
 ******************************************************************/
int misc_init_r (void)
{
    return(0);
}

/**********************************************
 * dram_init - sets uboots idea of sdram size
 **********************************************/
int dram_init (void)
{
#ifdef CONFIG_L2C_AS_RAM
	ddr_init();
#endif
    gd->ram_size = CONFIG_PHYS_SDRAM_1_SIZE - CONFIG_PHYS_SDRAM_RSVD_SIZE;

    return 0;
}

#ifdef CONFIG_PWM0_FOR_FAN
/* Fixme: Better to have a generic PWM config function to configure any 
   PWM channel */
void bcm95301x_config_pwm0_4_fan(void)
{
	reg32_set_bits(ChipcommonB_GP_AUX_SEL, 0x01); // Select PWM 0 
	reg32_clear_bits(CRU_GPIO_CONTROL0_OFFSET, 1 << 8); // Selct PWM
	reg32_write_masked(ChipcommonB_PWM_PRESCALE, 0x3F, 24); // prescale = 24 + 1
	reg32_write(ChipcommonB_PWM_PERIOD_COUNT0, 5 * 180); 
	reg32_write(ChipcommonB_PWM_DUTYHI_COUNT0, 5 * 135);
	reg32_set_bits(ChipcommonB_PWMCTL, 0x101); // Enable PWM0
}
#endif

int board_late_init (void) 
{
	extern ulong mmu_table_addr;    /* MMU on flash fix: 16KB aligned */
	int status = 0;
#if 0 // Fixme: To be enabled later,works but linux fails to run
	int i;
#endif
    
	/* unlock the L2 Cache */
#ifndef CONFIG_NO_CODE_RELOC
//	printf("Unlocking L2 Cache ...");
	l2cc_unlock();
//	printf("Done\n");
#endif

	/* MMU on flash fix: 16KB aligned */
	asm volatile ("mcr p15, 0, %0, c2, c0, 0"::"r"(mmu_table_addr)); /*update TTBR0 */
	asm volatile ("mcr p15, 0, r1, c8, c7, 0");  /*invalidate TLB*/
	asm volatile ("mcr p15, 0, r1, c7, c10, 4"); /* DSB */
	asm volatile ("mcr p15, 0, r0, c7, c5, 4"); /* ISB */		

	/* Systick initialization(private timer)*/
	iproc_clk_enum();
	//glb_tim_init();
#ifndef STDK_BUILD
	disable_interrupts();
#if 0 // Fixme: To be enabled later,works but linux fails to run
	/* turn off I/D-cache */
	icache_disable();
	dcache_disable();

	/* Cache flush */
	asm ("mcr p15, 0, %0, c7, c5, 0": :"r" (0));

	/* turn off L2 cache */
	l2cc_disable();
	/* invalidate L2 cache also */
	invalidate_dcache();

	i = 0;
	/* mem barrier to sync up things */
	asm("mcr p15, 0, %0, c7, c10, 4": :"r"(i));

	scu_disable();
#endif
#ifdef CONFIG_PWM0_FOR_FAN
	bcm95301x_config_pwm0_4_fan();
#endif
#else
	gic_disable_interrupt(29);
	irq_install_handler(29, systick_isr, NULL);
	gic_config_interrupt(29, 1, IPROC_INTR_LEVEL_SENSITIVE, 0, IPROC_GIC_DIST_IPTR_CPU0);
	iproc_systick_init(10000);

	/* MMU and cache setup */
	disable_interrupts();
	//printf("Enabling SCU\n");
	scu_enable();

	printf("Enabling icache and dcache\n");
	dcache_enable();
	icache_enable();

	printf("Enabling l2cache\n");
	status = l2cc_enable();
	//printf("Enabling done, status = %d\n", status);

	enable_interrupts();
#endif
	return status;
}

int board_eth_init(bd_t *bis)
{
	int rc = -1;
#ifdef CONFIG_BCMIPROC_ETH
//	printf("Registering eth\n");
	rc = bcmiproc_eth_register(0);
#endif
	return rc;
}

#ifdef CFG_LATE_BOARD_INIT
int n_radios = 0;

static void
bcm95301x_radio_callback(int n)
{
   n_radios = n;
}

extern int ar5416_initialize(void);

void
late_board_init(void)
{
    extern void ar5416_install_callback(void *);

    ar5416_install_callback(bcm95301x_radio_callback);
    ar5416_initialize();

#ifdef CONFIG_RESET_BUTTON
    execute_config_clear();
#endif
}
#endif	/* CFG_LATE_BOARD_INIT */

int
do_ar5416(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    ar5416_initialize();
    return 0;
}

U_BOOT_CMD(
    ar5416,    1,    1,     do_ar5416,
    "gpio_pins    - show GPIO pin status\n",
    " Usage: gpio_pins \n"
);
