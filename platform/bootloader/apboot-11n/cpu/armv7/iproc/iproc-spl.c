#include <common.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;


u32 spl_boot_device(void)
{
#ifdef CONFIG_NAND_IPROC_BOOT
   return BOOT_DEVICE_NAND;
#else
   return BOOT_DEVICE_SPI;
#endif
}

/*
 * In the context of SPL, board_init_f must ensure that any clocks/etc for
 * DDR are enabled, ensure that the stack pointer is valid, clear the BSS
 * and call board_init_f.  We provide this version by default but mark it
 * as __weak to allow for platforms to do this in their own way if needed.
 */
void board_init_f(ulong dummy)
{

    /* Set the stack pointer. */
    asm volatile("mov sp, %0\n" : : "r"(CONFIG_SPL_STACK));

    /* Clear the BSS. */
    memset(__bss_start, 0, __bss_end__ - __bss_start);

    gd = &gdata;
    memset((void *)gd, 0, sizeof(gd_t));

    /* Console initialization */
    preloader_console_init();
    timer_init();
    dram_init();
    board_init_r(NULL, 0);
}
