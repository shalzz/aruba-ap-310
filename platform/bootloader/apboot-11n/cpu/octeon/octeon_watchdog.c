#include <config.h>

#ifdef CONFIG_HW_WATCHDOG

#include <common.h>
#include <command.h>
#include "octeon_boot.h"
#include "octeon_hal.h"

/**
 * cvmx_ciu_wdog#
 */
typedef union
{
    uint64_t u64;
    struct cvmx_ciu_wdogx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_46_63          : 18;
        uint64_t gstopen                 : 1;       /**< GSTOPEN */
        uint64_t dstop                   : 1;       /**< DSTOP */
        uint64_t cnt                     : 24;      /**< Number of 256-cycle intervals until next watchdog
                                                         expiration.  Cleared on write to associated
                                                         CIU_PP_POKE register. */
        uint64_t len                     : 16;      /**< Watchdog time expiration length
                                                         The 16 bits of LEN represent the most significant
                                                         bits of a 24 bit decrementer that decrements
                                                         every 256 cycles.
                                                         LEN must be set > 0 */
        uint64_t state                   : 2;       /**< Watchdog state
                                                         number of watchdog time expirations since last
                                                         PP poke.  Cleared on write to associated
                                                         CIU_PP_POKE register. */
        uint64_t mode                    : 2;       /**< Watchdog mode
                                                         0 = Off
                                                         1 = Interrupt Only
                                                         2 = Interrupt + NMI
                                                         3 = Interrupt + NMI + Soft-Reset */
#else
        uint64_t mode                    : 2;
        uint64_t state                   : 2;
        uint64_t len                     : 16;
        uint64_t cnt                     : 24;
        uint64_t dstop                   : 1;
        uint64_t gstopen                 : 1;
        uint64_t reserved_46_63          : 18;
#endif
    } s;
    struct cvmx_ciu_wdogx_s              cn3020;
    struct cvmx_ciu_wdogx_s              cn30xx;
    struct cvmx_ciu_wdogx_s              cn31xx;
    struct cvmx_ciu_wdogx_s              cn36xx;
    struct cvmx_ciu_wdogx_s              cn38xx;
    struct cvmx_ciu_wdogx_s              cn38xxp2;
    struct cvmx_ciu_wdogx_s              cn56xx;
    struct cvmx_ciu_wdogx_s              cn58xx;
} cvmx_ciu_wdogx_t;

#define  CVMX_CIU_PP_POKEX(offset)                           (CVMX_ADD_IO_SEG(0x0001070000000580ull+((offset)*8)))
#define  CVMX_CIU_WDOGX(offset)                              (CVMX_ADD_IO_SEG(0x0001070000000500ull+((offset)*8)))

/* Watchdog defines, to be moved.... */
typedef enum {
   CVMX_CIU_WDOG_MODE_OFF = 0,
   CVMX_CIU_WDOG_MODE_INT = 1,
   CVMX_CIU_WDOG_MODE_INT_NMI = 2,
   CVMX_CIU_WDOG_MODE_INT_NMI_SR = 3
} cvmx_ciu_wdog_mode_t;

static int pet_the_dog = 1;

void
hw_watchdog_reset(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	if (!gd->watchdog_inited) {
		gd->watchdog_start = get_timer(0);
		gd->watchdog_blink = 1;
		gd->watchdog_inited = 1;
	}
	if (pet_the_dog) {
		uint64_t val = 0x534341;
		cvmx_write_csr(CVMX_CIU_PP_POKEX(0), val);
	}
#ifdef GPIO_GREEN_STATUS_LED
	if (gd->watchdog_blink) {
		octeon_gpio_set(GPIO_GREEN_STATUS_LED);
	} else {
		octeon_gpio_clr(GPIO_GREEN_STATUS_LED);
	}
#endif
	if (get_timer(gd->watchdog_start) > CFG_HZ) {
		gd->watchdog_start = get_timer(0);
		gd->watchdog_blink = !gd->watchdog_blink;
	}
}

void
hw_watchdog_init(void)
{
	cvmx_ciu_wdogx_t val;

	val.u64 = 0;

	//
	// 'len' * 65536 is the count in cycles
	// 7630 gives a one-second watchdog (roughly)
	// the longest we can do on a 500 Mhz part is
	// ~8.6 seconds
	// XXX; empirical testing doesn't quite match these calculations.  It
	// looks more like 5 seconds.
	//
	val.cn30xx.len = 7630 * 7;	// XXX 7 seconds is debug value

	//
	// we enable both the interrupt & the NMI, but since u-boot has
	// no interrupts turned on, we'll just get the NMI, which is fine
	// when in the boot; this should not happen much
	//
	val.cn30xx.mode = CVMX_CIU_WDOG_MODE_INT_NMI;
	cvmx_write_csr(CVMX_CIU_WDOGX(0), val.u64);
}

void
hw_watchdog_off(void)
{
	cvmx_ciu_wdogx_t val;

	val.cn30xx.mode = CVMX_CIU_WDOG_MODE_OFF;
	cvmx_write_csr(CVMX_CIU_WDOGX(0), val.u64);
}

int
do_nodog(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    hw_watchdog_off();
    printf("Watchdog disabled\n");
    return 0;
}

int
do_dog(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("Watchdog will fire momentarily.\n");
	pet_the_dog = 0;
	return 0;
}

U_BOOT_CMD(
	wdog,     1,     1,      do_dog,
	"wdog - stop petting the watchdog\n",
	"- stop petting the watchdog\n"
);

U_BOOT_CMD(
    nodog,     1,     1,      do_nodog,
    "nodog - disable watchdog\n",
    "- disable watchdog\n"
);
#endif
