#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <asm/immap_85xx.h>

#ifdef CONFIG_APBOOT

/* 
 * Due to FSL bit numbering, the pin must be subtracted from 31.
 * For example: pin 4 becomes 0x8000000 in the register.
 */
void
gpio_set_as_output(unsigned pin)
{
    volatile ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);

    setbits_be32(&pgpio->gpdir, (1 << (31 - pin)));
}

void
gpio_set_as_input(unsigned pin)
{
    volatile ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);

    clrbits_be32(&pgpio->gpdir, (1 << (31 - pin)));
}

void
gpio_out(unsigned pin, unsigned val)
{
    volatile ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);

    if (val) {
        setbits_be32(&pgpio->gpdat, (1 << (31 - pin)));
    } else {
        clrbits_be32(&pgpio->gpdat, (1 << (31 - pin)));
    }
}

int
gpio_value(unsigned pin)
{
    volatile ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);
    unsigned val;

    val = in_be32(&pgpio->gpdat);

    if (val & (1 << (31 - pin))) {
        return 1;
    }

    return 0;
}

int
do_gpio(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int pin, val;

    if (argc != 3) {
        printf("Usage: \n%s\n", cmdtp->usage);
        return 1;
    }

    pin = simple_strtoul(argv[1], (char **)0, 10);
    val = simple_strtoul(argv[2], (char **)0, 10);

    if (val) {
    	val = 1;
    }

    printf("Driving pin %u as output %s\n", pin, val & 1 ? "high" : "low");
    gpio_set_as_output(pin);
    gpio_out(pin, val);
    return 0;
}

U_BOOT_CMD(
    gpio,    3,    1,     do_gpio,
    "gpio    - poke GPIO pin\n",                   
    " Usage: gpio \n"
);
#endif
