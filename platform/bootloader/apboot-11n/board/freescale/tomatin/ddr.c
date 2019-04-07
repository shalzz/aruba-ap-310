/*
 * Copyright 2008-2010 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#include <common.h>
#include <asm/mmu.h>
#include <asm/immap_85xx.h>
#include <asm/processor.h>
#include <asm/fsl_ddr_sdram.h>
#include <asm/io.h>
#include <asm/fsl_law.h>

DECLARE_GLOBAL_DATA_PTR;

extern void fsl_ddr_set_memctl_regs(const fsl_ddr_cfg_regs_t *regs,
				   unsigned int ctrl_num);

#define DATARATE_400MHZ 400000000
#define DATARATE_533MHZ 533333333
#define DATARATE_667MHZ 666666666
#define DATARATE_800MHZ 800000000

#if (__TOMATIN_DRAM__ == 256)
#define CONFIG_SYS_DDR_CS0_BNDS		0x0000000f /* 256MB */
#define CONFIG_SYS_DDR_CS0_CONFIG	0x80014102 /* 13 row bits */
#define CONFIG_SYS_DDR_TIMING_1_800	0x6f6B4644
#elif (__TOMATIN_DRAM__ == 512)
#define CONFIG_SYS_DDR_CS0_BNDS		0x0000001f /* 512MB */
#define CONFIG_SYS_DDR_CS0_CONFIG	0x80014202 /* 14 row bits */
#define CONFIG_SYS_DDR_TIMING_1_800	0x5d59d544
#else
#error "Unsupported memory size for Ardmore"
#endif

#define CONFIG_SYS_DDR_CS0_CONFIG_2	0x00000000
#define CONFIG_SYS_DDR_INIT_ADDR	0x00000000
#define CONFIG_SYS_DDR_INIT_EXT_ADDR	0x00000000
#define CONFIG_SYS_DDR_MODE_CONTROL	0x00000000

#define CONFIG_SYS_DDR_ZQ_CONTROL	0x89080600
#define CONFIG_SYS_DDR_WRLVL_CONTROL	0x8655A605
#define CONFIG_SYS_DDR_SR_CNTR		0x00000000
#define CONFIG_SYS_DDR_RCW_1		0x00000000
#define CONFIG_SYS_DDR_RCW_2		0x00000000
#define CONFIG_SYS_DDR_CONTROL		0xC70C0008
#define CONFIG_SYS_DDR_CONTROL_2	0x04401010
#define CONFIG_SYS_DDR_TIMING_4		0x00000001
#define CONFIG_SYS_DDR_TIMING_5		0x03402400

#define CONFIG_SYS_DDR_TIMING_3_800	0x00020000
#define CONFIG_SYS_DDR_TIMING_0_800	0x00330004
#define CONFIG_SYS_DDR_TIMING_2_800	0x0FA888CF
#define CONFIG_SYS_DDR_CLK_CTRL_800	0x02000000
#define CONFIG_SYS_DDR_MODE_1_800	0x00041420
#define CONFIG_SYS_DDR_MODE_2_800	0x8000c000
#define CONFIG_SYS_DDR_INTERVAL_800	0x0C300100

#define CONFIG_SYS_DDR_CDR1		0x80040000
#define CONFIG_SYS_DDR_CDR2		0x00000000

#define CONFIG_MEM_INIT_VALUE	0xdeadbeef

fsl_ddr_cfg_regs_t ddr_cfg_regs_800 = {
	.cs[0].bnds = CONFIG_SYS_DDR_CS0_BNDS,
	.cs[0].config = CONFIG_SYS_DDR_CS0_CONFIG,
	.cs[0].config_2 = CONFIG_SYS_DDR_CS0_CONFIG_2,
	.timing_cfg_3 = CONFIG_SYS_DDR_TIMING_3_800,
	.timing_cfg_0 = CONFIG_SYS_DDR_TIMING_0_800,
	.timing_cfg_1 = CONFIG_SYS_DDR_TIMING_1_800,
	.timing_cfg_2 = CONFIG_SYS_DDR_TIMING_2_800,
	.ddr_sdram_cfg = CONFIG_SYS_DDR_CONTROL,
	.ddr_sdram_cfg_2 = CONFIG_SYS_DDR_CONTROL_2,
	.ddr_sdram_mode = CONFIG_SYS_DDR_MODE_1_800,
	.ddr_sdram_mode_2 = CONFIG_SYS_DDR_MODE_2_800,
	.ddr_sdram_md_cntl = CONFIG_SYS_DDR_MODE_CONTROL,
	.ddr_sdram_interval = CONFIG_SYS_DDR_INTERVAL_800,
	.ddr_data_init = CONFIG_MEM_INIT_VALUE,	
	.ddr_sdram_clk_cntl = CONFIG_SYS_DDR_CLK_CTRL_800,
	.ddr_init_addr = CONFIG_SYS_DDR_INIT_ADDR,
	.ddr_init_ext_addr = CONFIG_SYS_DDR_INIT_EXT_ADDR,
	.timing_cfg_4 = CONFIG_SYS_DDR_TIMING_4,
	.timing_cfg_5 = CONFIG_SYS_DDR_TIMING_5,
	.ddr_zq_cntl = CONFIG_SYS_DDR_ZQ_CONTROL,
	.ddr_wrlvl_cntl = CONFIG_SYS_DDR_WRLVL_CONTROL,
	.ddr_sr_cntr = CONFIG_SYS_DDR_SR_CNTR,
	.ddr_sdram_rcw_1 = CONFIG_SYS_DDR_RCW_1,
	.ddr_sdram_rcw_2 = CONFIG_SYS_DDR_RCW_2,
	.ddr_cdr1 = CONFIG_SYS_DDR_CDR1,
	.ddr_cdr2 = CONFIG_SYS_DDR_CDR2,
};

/*
 * Fixed sdram init -- doesn't use serial presence detect.
 */

phys_size_t fixed_sdram (void)
{
	sys_info_t sysinfo;
	char buf[32];
	fsl_ddr_cfg_regs_t ddr_cfg_regs;
	size_t ddr_size;
	struct cpu_type *cpu;

	get_sys_info(&sysinfo);
	printf("Configuring DDR for %s MT/s data rate\n",
				strmhz(buf, sysinfo.freqDDRBus));

	memcpy(&ddr_cfg_regs, &ddr_cfg_regs_800, sizeof(ddr_cfg_regs));

	cpu = gd->cpu;

	ddr_size = CONFIG_SYS_SDRAM_SIZE * 1024 * 1024;

	fsl_ddr_set_memctl_regs(&ddr_cfg_regs, 0);

	if (set_ddr_laws(CONFIG_SYS_DDR_SDRAM_BASE,
				ddr_size, LAW_TRGT_IF_DDR_1) < 0) {
		printf("ERROR setting Local Access Windows for DDR\n");
		return 0;
	};

	return ddr_size;
}

#if 0
phys_size_t initdram(int board_type)
{
	phys_size_t dram_size = 0;

#if defined(CONFIG_SYS_RAMBOOT) && defined(CONFIG_SYS_FSL_BOOT_DDR)
	return CONFIG_SYS_SDRAM_SIZE * 1024 * 1024;
#endif
	dram_size = fixed_sdram();
	set_ddr_laws(0, dram_size, LAW_TRGT_IF_DDR_1);

	dram_size = setup_ddr_tlbs(dram_size / 0x100000);
	dram_size *= 0x100000;

	puts("DDR:   ");
	return dram_size;
}
#endif

