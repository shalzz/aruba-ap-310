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
#include <exports.h>

#pragma GCC optimize("-O0")

DECLARE_GLOBAL_DATA_PTR;

extern unsigned gpio_value(unsigned);
extern void gpio_set_as_input(unsigned);
extern void gpio_set_as_output(unsigned);
extern void gpio_out(unsigned, unsigned);

extern int bcmiproc_eth_register(u8 dev_num);
extern void l2cc_unlock(void);
extern void iproc_clk_enum(void);

#if defined(CONFIG_RESET_BUTTON)

#define REG_OFFSET             1
#define UART_SCRATCH_PAD       (7*REG_OFFSET)
#define UART16550_READ(unit, x)      reg8_read((uint8_t *)(CONFIG_SYS_NS16550_COM1 + x + (unit * 0x100)))
#define UART16550_WRITE(unit, x, z)  reg8_write((uint8_t *)(CONFIG_SYS_NS16550_COM1 + x + (unit * 0x100)), z)

static void
flash_status_led(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    int i;

    for (i = 0; i < 6; i++) {
        gpio_out(GPIO_GREEN_STATUS_LED, 1);
#ifdef CONFIG_HW_WATCHDOG
        gd->watchdog_blink = 1;
#endif
        udelay(100 * 1000);
        gpio_out(GPIO_GREEN_STATUS_LED, 0);
#ifdef CONFIG_HW_WATCHDOG
        gd->watchdog_blink = 0;
#endif
        WATCHDOG_RESET();
        udelay(100 * 1000);
    }
}

static void
check_reset_button(void)
{
    char *reset_env_var = 0;

    if (!gpio_value(GPIO_CONFIG_CLEAR)) {
        /* unleash hell */
#if 0 /* XXX; this doesn't work pre relocation */
        reset_env_var = getenv("disable_reset_button");
#endif
        if (reset_env_var && (!strcmp(reset_env_var, "yes")))  {
            UART16550_WRITE(0, UART_SCRATCH_PAD, 'D');
            UART16550_WRITE(1, UART_SCRATCH_PAD, 'D');
        } else {
            UART16550_WRITE(0, UART_SCRATCH_PAD, 'R');
            UART16550_WRITE(1, UART_SCRATCH_PAD, 'B');
            flash_status_led();
        }
    } else {
        /* no config clear */
        UART16550_WRITE(0, UART_SCRATCH_PAD, 'N');
        UART16550_WRITE(1, UART_SCRATCH_PAD, 'R');
    }
}

static void
execute_config_clear(void)
{
    extern int do_factory_reset(cmd_tbl_t *, int, int, char **);
    char *av[3];
    char *reset_env_var = 0;
    int uart0, uart1;

    uart0 = UART16550_READ(0, UART_SCRATCH_PAD);
    uart1 = UART16550_READ(1, UART_SCRATCH_PAD);

    WATCHDOG_RESET();

    if (uart0 == 'N' && uart1 == 'R') {
        return;
    } else if (uart0 == 'D' && uart1 == 'D') {
        printf("**** Reset Request Disabled by Environment ****\n");
    } else if (uart0 != 'R' || uart1 != 'B') {
        printf("**** Unrecognized Reset State (0x%02x/0x%02x) -- Ignoring ****\n",
            uart0, uart1);
        return;
    }

    reset_env_var = getenv("disable_reset_button");
    if (reset_env_var && !strcmp(reset_env_var, "yes")) {
        printf("**** Reset Request Disabled by Environment ****\n");
        return;
    }

    printf("**** Configuration Reset Requested by User ****\n");

    av[0] = "factory_reset";
    av[1] = 0;
    do_factory_reset(0, 0, 1, av);
    flash_status_led();
}
#endif

static void enable_smbus(int onoff)
{
    unsigned gpio_mask;

    gpio_mask = 0xffff;
    if (onoff) {
	    gpio_mask &= ~((1<<GPIO_TPM_CLK) | (1 << GPIO_TPM_DATA));
    }
    reg32_write((volatile unsigned int *)CRU_GPIO_CONTROL0_OFFSET, gpio_mask);
}

static void
springbank_gpio_init(void)
{
    extern void iprocGPIOInit(void);
    unsigned gpio_mask;

    /* set all 16 CRU MUXes to GPIO */
    gpio_mask = 0xffff;
//    gpio_mask &= ~((1<<GPIO_TPM_CLK) | (1 << GPIO_TPM_DATA));
    reg32_write((volatile unsigned int *)CRU_GPIO_CONTROL0_OFFSET, gpio_mask);

    gpio_set_as_output(GPIO_RED_STATUS_LED);
    /* turn off red LED */
    gpio_out(GPIO_RED_STATUS_LED, 1);

    gpio_set_as_output(GPIO_GREEN_STATUS_LED);
    gpio_set_as_output(GPIO_2G_AMBER_LED);
    gpio_set_as_output(GPIO_5G_AMBER_LED);
    gpio_set_as_output(GPIO_2G_GREEN_LED);
    gpio_set_as_output(GPIO_5G_GREEN_LED);

    // drive reset pin high, but don't enable it as an output
#ifdef GPIO_HW_RESET
    gpio_out(GPIO_HW_RESET, 1);
#endif

    /* turn red/green radio signals off as early as possible */
    gpio_out(GPIO_5G_AMBER_LED, 1);
    gpio_out(GPIO_2G_AMBER_LED, 1);
    gpio_out(GPIO_5G_GREEN_LED, 1);
    gpio_out(GPIO_2G_GREEN_LED, 1);

#ifdef GPIO_POWER_DC
    gpio_set_as_input(GPIO_POWER_DC);
#endif
#ifdef GPIO_POWER_AT
    gpio_set_as_input(GPIO_POWER_AT);
#endif
#ifdef GPIO_POWER_POE_PLUS
    gpio_set_as_input(GPIO_POWER_POE_PLUS);
#endif
    gpio_set_as_input(GPIO_CONFIG_CLEAR);
}

int checkboard(void)
{
#ifdef CONFIG_HW_WATCHDOG
    extern void hw_watchdog_init(void);

    hw_watchdog_init();
    WATCHDOG_RESET();
#endif

    springbank_gpio_init();

#ifdef CONFIG_RESET_BUTTON
    check_reset_button();
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
springbank_radio_callback(int n)
{
   n_radios = n;
}

void
late_board_init(void)
{
    extern void ar5416_install_callback(void *);
    extern int ar5416_initialize(void);
    unsigned val32;
    extern void srab_wreg(void *, unsigned, unsigned, void *, unsigned);

    ar5416_install_callback(springbank_radio_callback);
    ar5416_initialize();

#ifdef CONFIG_RESET_BUTTON
    execute_config_clear();
#endif

    /* power down ports 1-4 */
    val32 = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);
    srab_wreg(0, 0x0, 0x4a, &val32, 4);
}
#endif	/* CFG_LATE_BOARD_INIT */

void
board_print_power_status(void)
{
#ifdef GPIO_POWER_DC
    int dc = gpio_value(GPIO_POWER_DC);                 /* active high */

    if (dc) {
        printf("Power: DC\n");
    } else {
	printf("Power: POE\n");
    }
#endif
}

int
do_gpio_pins(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#ifdef GPIO_POWER_DC
    printf("DC:              %s\n", gpio_value(GPIO_POWER_DC) ? "high" : "low");
//    printf("POE+:            %s\n", gpio_value(GPIO_POWER_POE_PLUS) ? "high" : "low");
//    printf("AT:              %s\n", gpio_value(GPIO_POWER_AT) ? "high" : "low");
#endif
#ifdef GPIO_CONFIG_CLEAR
    printf("Config reset:    %s\n", gpio_value(GPIO_CONFIG_CLEAR) ? "inactive" : "pending");
#endif
#ifdef GPIO_PHY_0_INT
    printf("PHY interrupt:   %s\n", gpio_value(GPIO_PHY_0_INT) ? "inactive" : "pending");
#endif
    return 0;
}

U_BOOT_CMD(
    gpio_pins,    1,    1,     do_gpio_pins,
    "gpio_pins    - show GPIO pin status\n",
    " Usage: gpio_pins \n"
);

extern int i2c_write_raw(u8, u8 *, int);
extern int i2c_read_raw(u8, u8 *, int);
int
tpm_i2c_read_bb(int bus, int dev, unsigned char *buf, int count)
{
//	int bnum = i2c_get_bus_num();
	int r;

//	i2c_set_bus_num(0);
	r = i2c_read_raw(dev, buf, count);
//	r = i2c_read(dev, 0, 0, buf, count);
//	i2c_set_bus_num(bnum);
	enable_smbus(0);
	return r;
}
int
tpm_i2c_write_bb(int bus, int dev, unsigned char *buf, int count)
{
//	int bnum = i2c_get_bus_num();
	int r;

	enable_smbus(1);
//	i2c_set_bus_num(0);
	r = i2c_write_raw(dev, buf, count);
//	r = i2c_write(dev, 0, 0, buf, count);
//	i2c_set_bus_num(bnum);
	return r;
}
