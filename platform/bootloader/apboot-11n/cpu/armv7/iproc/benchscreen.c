#include <common.h>
#include <config.h>
#include <asm/arch/iproc.h>
#include <asm/system.h>
#include "asm/iproc/reg_utils.h"

/*
** Test tests the DCO code for LCPLL(IPROC_XGPLL) at IPROC wrapper in SOC
** Applicable to Helix4 and Hurricane2
**
*/
#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2))
void bench_screen_test1(void)
{ 
	uint32_t val, i;
	printf("BENCH SCREENING TEST1\n");
	printf("=========================================\n");
	val = reg32_read(IPROC_WRAP_IPROC_XGPLL_CTRL_3);
	printf("IPROC_XGPLL_CTRL_3: 0x%08x\n", val);
	reg32_clear_bits(IPROC_WRAP_IPROC_XGPLL_CTRL_3, 1 << 17);
	reg32_write_masked(IPROC_WRAP_IPROC_XGPLL_CTRL_3, 7 << 14, 4 << 14);
	reg32_set_bits(IPROC_WRAP_IPROC_XGPLL_CTRL_3, 1<<17);
	for(i =0; i < 1000; i++)
		val = reg32_read(IPROC_WRAP_IPROC_XGPLL_STATUS);
	printf("IPROC_XGPLL_STATUS: 0x%08x\n", val);
	val = ((val >> 4) & 0xFF); /* bits 11:4 */
	printf("DCO code: %d\n", val);
	if(val > 14)
		printf("PASS\n");
	else
		printf("FAIL\n");
	printf("=========================================\n");
}
#endif


