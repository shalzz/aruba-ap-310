#include <config.h>
#include <common.h>
#include <asm/arch/iproc_regs.h>
#include "asm/arch/socregs.h"
#include "asm/iproc/reg_utils.h"

#ifdef CONFIG_APBOOT
#pragma GCC optimize("-O0")
#endif

/*
To power down NEON 1 in CPU1:
1. Set the logic_clamp_on_neon1 bit(bit 12) of CRU_ARMCore_pwrdwn_en register (address: 0x1800b004) to 1.
2. Wait for 20ns
3. Set the logic_pwrdown_neon1 bit(bit 13) of CRU_ARMCore_pwrdwn_en register (address: 0x1800b004) to 1.

To power down CPU1:
1. Power down Neon1 by following the steps in “Powering Down NEON 1 in CPU 1”.
2. Wait for 100ns.
3. Set the logic_clamp_on_cpu1 bit(bit 8) of CRU_ARMCore_pwrdwn_en register (address: 0x1800b004) to 1.
4. Set the ram_clamp_on_cpu1 bit(bit 10) of CRU_ARMCore_pwrdwn_en register (address: 0x1800b004) to 1.
5. Wait approximately 20ns.
6. Set logic_pwrdown_cpu1 bit(bit 9) of CRU_ARMCore_pwrdwn_en register (address: 0x1800b004) to 1.
7. Set ram_pwrdown_cpu1 bit(bit 11) of CRU_ARMCore_pwrdwn_en register (address: 0x1800b004) to 1.
*/

void ihost_pwrdown_neon(uint32_t neon_num)
{
	uint32_t i, val;
#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_NS_PLUS) || defined(CONFIG_KATANA2))
		for(i = 0; i < 100; i++)
			val  = reg32_read((volatile uint32_t *)CRU_ihost_pwrdwn_status);


	if(neon_num == 0) {
		reg32_set_bits((volatile uint32_t *)CRU_ihost_pwrdwn_en, 1 << CRU_ihost_pwrdwn_en__logic_clamp_on_neon0);
		for(i = 0; i < 30; i++)
			val  = reg32_read((volatile uint32_t *)CRU_ihost_pwrdwn_status);
		reg32_set_bits((volatile uint32_t *)CRU_ihost_pwrdwn_en, 1 << CRU_ihost_pwrdwn_en__logic_pwrdown_neon0);
	}
	else if(neon_num == 1) {
		reg32_set_bits((volatile uint32_t *)CRU_ihost_pwrdwn_en, 1 << CRU_ihost_pwrdwn_en__logic_clamp_on_neon1);
		for(i = 0; i < 30; i++)
			val  = reg32_read((volatile uint32_t *)CRU_ihost_pwrdwn_status);
		reg32_set_bits((volatile uint32_t *)CRU_ihost_pwrdwn_en, 1 << CRU_ihost_pwrdwn_en__logic_pwrdown_neon1);
	}
#elif defined(CONFIG_NORTHSTAR)
	if(((reg32_read((volatile uint32_t *)ChipcommonA_ChipID)) & 0x0000ffff) == 0xcf19) {
		/* costar */
		/* No neon support in the Northstar */
		for(i = 0; i < 100; i++)
			val  = reg32_read((volatile uint32_t *)0x1800b008);


		if(neon_num == 0) {
			reg32_set_bits((volatile uint32_t *)0x1800b004, 1 << 4);
			for(i = 0; i < 30; i++)
				val  = reg32_read((volatile uint32_t *)0x1800b008);
			reg32_set_bits((volatile uint32_t *)0x1800b004, 1 << 5);
		}
		else if(neon_num == 1) {
			reg32_set_bits((volatile uint32_t *)0x1800b004, 1 << 12);
			for(i = 0; i < 30; i++)
				val  = reg32_read((volatile uint32_t *)0x1800b008);
			reg32_set_bits((volatile uint32_t *)0x1800b004, 1 << 13);
		}
	}
#endif

}

void ihost_pwrdown_cpu(uint32_t cpu_num)
{
	uint32_t i, val;
	ihost_pwrdown_neon(cpu_num);
#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_NS_PLUS) || defined(CONFIG_KATANA2))
	if(cpu_num == 0) {
		reg32_write_masked((volatile uint32_t *)CRU_cpu0_powerdown, 0xffff, 1000);
		reg32_set_bits((volatile uint32_t *)CRU_cpu0_powerdown, 1 << CRU_cpu0_powerdown__start_cpu0_powerdown_seq);
		asm ("WFI");
	}
	else if(cpu_num == 1) {
		reg32_set_bits((volatile uint32_t *)CRU_ihost_pwrdwn_en, 1 << CRU_ihost_pwrdwn_en__logic_clamp_on_cpu1 || 
		1 << CRU_ihost_pwrdwn_en__ram_clamp_on_cpu1	);
		for(i = 0; i < 30; i++)
			val  = reg32_read((volatile uint32_t *)CRU_ihost_pwrdwn_status);
		reg32_set_bits((volatile uint32_t *)CRU_ihost_pwrdwn_en, 1 << CRU_ihost_pwrdwn_en__logic_pwrdown_cpu1 || 
		1 << CRU_ihost_pwrdwn_en__ram_pwrdown_cpu1);

	}
#elif defined(CONFIG_NORTHSTAR)
	if(((reg32_read((volatile uint32_t *)ChipcommonA_ChipID)) & 0x0000ffff) == 0xcf19) {
		if(cpu_num == 0) {
			reg32_write_masked((volatile uint32_t *)0x1800b00c, 0xffff, 1000);
			reg32_set_bits((volatile uint32_t *)0x1800b00c, 1 << 31);
			asm ("WFI");
		}
		else if(cpu_num == 1) {
			reg32_set_bits((volatile uint32_t *)0x1800b004, 1 << 8 || 1 << 10);
			for(i = 0; i < 30; i++)
				val  = reg32_read((volatile uint32_t *)0x1800b008);
			reg32_set_bits((volatile uint32_t *)0x1800b004, 1 << 9 || 1 << 11);
		}
	}
#endif

}
