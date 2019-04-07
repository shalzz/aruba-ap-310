/*Commands for Lan Bypass function on Octeon.
 *By Oscar Ko, Flextronics Taiwan
 */

#include <common.h>

#ifdef CONFIG_OCTEON_LANBYPASS

#include <command.h>
#if 0
#include <ds1374.h>
#endif

#define CLR_PULSE 10 //Oscar: approx. 100ns

extern void octeon_gpio_clr(int bit);
extern void octeon_gpio_set(int bit);
int bypass_init(void);

int do_setbypass (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    char c = argv[1][0];

    if(argc > 2) {
        printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }

    if( c=='g' || !strncmp(argv[1], "gpio", 4) ) {
        /* make lan bypass enable */
        bypass_init();
        octeon_gpio_set(6);
        printf("Enable LAN bypass by GPIO. \n");
    }
#if 0
    else if( c=='w' || !strncmp(argv[1], "wdt", 3) ) {
        /* make lan bypass enable */
        bypass_init();

        //Set WDT parameters and turn it on
        SetAlarmCounter(0x00, 0x10, 0x00);	//1 sec
        SetAlarmPara(1,1,1);
        SetAlarmOn();

        printf("Enable LAN bypass by WDT. \n");
    }
#endif
    else if ( c=='o' || !strncmp(argv[1], "off", 2) ) {
        /* make lan bypass disable */
	bypass_init();
        printf("Disable LAN bypass. \n");
    } else {
        printf("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }
    return 0;
}

//Disable LAN BYPASS to an init. stage
int bypass_init(void)
{
	int i;
	//Clear GPIO 6
	octeon_gpio_clr(6);
#if 0
	//Disable WDT
	SetAlarmOff();
#endif	
	//GPIO(7) Send a low pulse
	octeon_gpio_clr(7);
	for(i=0;i<CLR_PULSE;i++);
	octeon_gpio_set(7);
	return 0;
}

U_BOOT_CMD(
	setbypass, CFG_MAXARGS, 1,	do_setbypass,
	"setbypass <mode> - Set LAN ByPass mode\n",
	"<mode> - Set LAN ByPass mode\n"
	"    g[pio] - enable lan bypass by GPIO\n"
#if 0
	"    w[dt]  - enable lan bypass by WDT\n"
#endif
	"    o[ff]  - disable lan bypass\n"
);
#endif /*CONFIG_OCTEON_LANBYPASS*/
