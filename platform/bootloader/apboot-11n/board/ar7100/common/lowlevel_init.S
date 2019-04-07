#include <config.h>
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <ar7100_soc.h>

/*
 * Helper macros.
 * These Clobber t7, t8 and t9
 */
#define clear_mask(_reg, _mask)                     \
    li  t7, _reg;                                   \
    lw  t8, 0(t7);                                  \
    li  t9, ~_mask;                                 \
    and t8, t8, t9;                                 \
    sw  t8, 0(t7)            

#define set_val(_reg, _mask, _val)                  \
    li  t7, _reg;                                   \
    lw  t8, 0(t7);                                  \
    li  t9, ~_mask;                                 \
    and t8, t8, t9;                                 \
    li  t9, _val;                                   \
    or  t8, t8, t9;                                 \
    sw  t8, 0(t7)            

#define get_val(_reg, _mask, _shift, _res_reg)      \
    li  t7, _reg;                                   \
    lw  t8, 0(t7);                                  \
    li  t9, _mask;                                  \
    and t8, t8, t9;                                 \
    srl _res_reg, t8, _shift                        \

#define pll_clr(_mask)                              \
    clear_mask(AR7100_CPU_PLL_CONFIG, _mask)

#define pll_set(_mask, _val)                        \
    set_val(AR7100_CPU_PLL_CONFIG,  _mask, _val)

#define pll_get(_mask, _shift, _res_reg)            \
    get_val(AR7100_CPU_PLL_CONFIG, _mask, _shift, _res_reg)

#define clk_clr(_mask)                               \
    clear_mask(AR7100_CPU_CLOCK_CONTROL, _mask)

#define clk_set(_mask, _val)                         \
    set_val(AR7100_CPU_CLOCK_CONTROL,  _mask, _val)

#define clk_get(_mask, _shift, _res_reg)              \
    get_val(AR7100_CPU_CLOCK_CONTROL, _mask, _shift, _res_reg)


/*
#define PLL_CONFIG_CPU_DIV_VAL  (0x3 << 16)
#define PLL_CONFIG_AHB_DIV_VAL  (0x0 << 20)
#define PLL_CONFIG_DDR_DIV_VAL  (0x3 << 18)
#define PLL_CONFIG_PLL_FB_VAL   (0x1d << 3)
#define PLL_CONFIG_PLL_LOOP_BW_VAL  (0x0 << 12)
*/

#define PLL_CONFIG_SW_UPDATE_VAL (1 << 31)
#define CLOCK_CTRL_SWITCH_VAL (1 << 1)

/******************************************************************************
 * first level initialization:
 * 
 * 0) If clock cntrl reset switch is already set, we're recovering from 
 *    "divider reset"; goto 3.
 * 1) Setup divide ratios.
 * 2) Reset.
 * 3) Setup pll's, wait for lock.
 * 
 *****************************************************************************/

.globl lowlevel_init

lowlevel_init:

    /*
     * The code below is for the real chip. Wont work on FPGA
     */
    /* jr ra  */

    clk_get(CLOCK_CONTROL_RST_SWITCH_MASK, CLOCK_CONTROL_RST_SWITCH_SHIFT, t6)
    bne zero, t6, initialize_pll
    nop 

    pll_set(PLL_CONFIG_CPU_DIV_MASK, PLL_CONFIG_CPU_DIV_VAL)
    pll_set(PLL_CONFIG_AHB_DIV_MASK, PLL_CONFIG_AHB_DIV_VAL)
    pll_set(PLL_CONFIG_DDR_DIV_MASK, PLL_CONFIG_DDR_DIV_VAL)

    pll_set(PLL_CONFIG_SW_UPDATE_MASK, PLL_CONFIG_SW_UPDATE_VAL)

    /*
     * Will cause a reset
     */
    clk_set(CLOCK_CONTROL_RST_SWITCH_MASK, CLOCK_CTRL_SWITCH_VAL)
    clk_set(CLOCK_CONTROL_CLOCK_SWITCH_MASK, 1)
    
initialize_pll:
    pll_set(PLL_CONFIG_SW_UPDATE_MASK, PLL_CONFIG_SW_UPDATE_VAL)
    clk_clr(CLOCK_CONTROL_RST_SWITCH_MASK)
    pll_set(PLL_CONFIG_PLL_FB_MASK, PLL_CONFIG_PLL_FB_VAL)
    pll_set(PLL_CONFIG_PLL_LOOP_BW_MASK, PLL_CONFIG_PLL_LOOP_BW_VAL)

    pll_clr(PLL_CONFIG_PLL_POWER_DOWN_MASK);
    pll_clr(PLL_CONFIG_PLL_BYPASS_MASK);

wait_for_pll_lock:
    pll_get(PLL_CONFIG_LOCKED_MASK, PLL_CONFIG_LOCKED_SHIFT, t6)
    beq zero, t6, wait_for_pll_lock
    nop 


pll_locked:
    clk_set(CLOCK_CONTROL_CLOCK_SWITCH_MASK, 1)   

    jr ra

