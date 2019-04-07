
/* * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.* */

#include <common.h>
#ifdef CONFIG_CMD_UBI
#include <linux/compat.h>
#include <nand.h>
#include <onenand_uboot.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <ubi_uboot.h>
#include <jffs2/load_kernel.h>
#endif

#include "ipq_nand.h"
#include <asm/arch/gpio.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/arch/nss/clock.h>
#include <asm/arch/ebi2.h>
#include <asm/arch/smem.h>
#include <asm/errno.h>
#include <asm-armv7/mach-types.h>
#include "./ipq806x_cdp.h"
#include "./ipq806x_board_param.h"
#include <configs/toba.h>
#include <asm/arch/nss/msm_ipq806x_gmac.h>
#include <asm/arch/timer.h>
#include <asm/arch/nand.h>
#include <asm/arch/phy.h>
#include <linux/mtd/compat.h>
#include <nand.h>
#include <watchdog.h>
#include "../../../drivers/ipq_gmac.h"

DECLARE_GLOBAL_DATA_PTR;

static void check_reset_button(void);
/* we use the reserved memory for NSS to store the reset button status */
char *reset_btn = (char *)IPQ_MEM_RESERVE_BASE(nss);

extern int setenv_addr(const char *varname, const void *addr);
extern unsigned gpio_value(unsigned);

// #define GENERATED_IPQ_RESERVE_SIZE  (sizeof(ipq_mem_reserve_t))

/* Watchdog bite time set to default reset value */
#define RESET_WDT_BITE_TIME 0x31F3

/* Watchdog bark time value is ketp larger than the watchdog timeout
 * of 0x31F3, effectively disabling the watchdog bark interrupt
 */
#define RESET_WDT_BARK_TIME (5 * RESET_WDT_BITE_TIME)

/*
 * If SMEM is not found, we provide a value, that will prevent the
 * environment from being written to random location in the flash.
 *
 * NAND: In the case of NAND, we do this by setting ENV_RANGE to
 * zero. If ENV_RANGE < ENV_SIZE, then environment is not written.
 *
 * SPI Flash: In the case of SPI Flash, we do this by setting the
 * flash_index to -1.
 */

loff_t board_env_offset;
loff_t board_env_range;
loff_t board_mfg_offset;
loff_t board_apboot_offset;
extern int nand_env_device;
extern int nand_mfg_device;

char buffer[CONFIG_MTD_MAX_DEVICE][16];

/*
 * Don't have this as a '.bss' variable. The '.bss' and '.rel.dyn'
 * sections seem to overlap.
 *
 * $ arm-none-linux-gnueabi-objdump -h u-boot
 * . . .
 *  8 .rel.dyn      00004ba8  40630b0c  40630b0c  00038b0c  2**2
 *                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 *  9 .bss          0000559c  40630b0c  40630b0c  00000000  2**3
 *                  ALLOC
 * . . .
 *
 * board_early_init_f() initializes this variable, resulting in one
 * of the relocation entries present in '.rel.dyn' section getting
 * corrupted. Hence, when relocate_code()'s 'fixrel' executes, it
 * patches a wrong address, which incorrectly modifies some global
 * variable resulting in a crash.
 *
 * Moral of the story: Global variables that are written before
 * relocate_code() gets executed cannot be in '.bss'
 */
board_ipq806x_params_t *gboard_param = (board_ipq806x_params_t *)0xbadb0ad;
uchar ipq_def_enetaddr[6] = {0x00, 0x03, 0x7F, 0xBA, 0xDB, 0xAD};

/*******************************************************
Function description: Board specific initialization.
I/P : None
O/P : integer, 0 - no error.

********************************************************/
static board_ipq806x_params_t *get_board_param(unsigned int machid)
{
	unsigned int index = 0;
	for (index = 0; index < NUM_IPQ806X_BOARDS; index++) {
		if (MACH_TYPE_IPQ806X_AP148  == board_params[index].machid)
		//if (machid == board_params[index].machid)
		{
			//printf("cdp: machine id is 0x%x\n",machid);
			return &board_params[index];
		}
	}
	BUG_ON(index == NUM_IPQ806X_BOARDS);
	printf("cdp: Invalid machine id 0x%x\n", machid);
	for (;;);
}

int board_init()
{
	int ret;
	uint32_t start_blocks;
	uint32_t size_blocks;
	loff_t board_env_size;
	ipq_smem_flash_info_t *sfi = &ipq_smem_flash_info;

	/*
	 * after relocation gboard_param is reset to NULL
	 * initialize again
	 */
	gd->bd->bi_boot_params = IPQ_BOOT_PARAMS_ADDR;
	gd->bd->bi_arch_number = smem_get_board_machtype();
	gboard_param = get_board_param(gd->bd->bi_arch_number);

	/*
	 * Should be inited, before env_relocate() is called,
	 * since env. offset is obtained from SMEM.
	 */
	ret = smem_ptable_init();
	if (ret < 0) {
		printf("cdp: SMEM init failed\n");
		return ret;
	}

	ret = smem_get_boot_flash(&sfi->flash_type,
				  &sfi->flash_index,
				  &sfi->flash_chip_select,
				  &sfi->flash_block_size);
	if (ret < 0) {
		printf("cdp: get boot flash failed\n");
		return ret;
	}

	if (sfi->flash_type == SMEM_BOOT_NAND_FLASH) {
		nand_env_device = CONFIG_IPQ_NAND_NAND_INFO_IDX;
		nand_mfg_device = CONFIG_IPQ_NAND_NAND_INFO_IDX;
		board_mfg_offset = CFG_MANUF_NAND_BASE;
	} else if (sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
		nand_env_device = CONFIG_IPQ_SPI_NAND_INFO_IDX;
		nand_mfg_device = CONFIG_IPQ_SPI_NAND_INFO_IDX;
		board_mfg_offset = CFG_MANUF_NOR_BASE;
	} else {
//		printf("BUG: unsupported flash type : %d\n", sfi->flash_type);
//		BUG();
	}

	ret = smem_getpart("0:APPSBLENV", &start_blocks, &size_blocks);
	if (ret < 0) {
		printf("cdp: get environment part failed\n");
		return ret;
	}

	flash_chip_select = sfi->flash_chip_select;
	flash_index = sfi->flash_index;
	board_env_offset = ((loff_t) sfi->flash_block_size) * start_blocks;
	board_env_size = ((loff_t) sfi->flash_block_size) * size_blocks;
	board_env_range = CONFIG_ENV_SIZE;
	BUG_ON(board_env_size < CONFIG_ENV_SIZE);

	ret = smem_getpart("0:APPSBL", &start_blocks, &size_blocks);
	if (ret < 0) {
		printf("cdp: get environment part failed\n");
		return ret;
	}

	board_apboot_offset = ((loff_t) sfi->flash_block_size) * start_blocks;

	gpio_tlmm_config(33,0,0,0,0,1);
	gpio_tlmm_config(26,0,0,0,0,1);
	return 0;
}

void enable_caches(void)
{
	icache_enable();
	dcache_enable();
}


/*******************************************************
Function description: DRAM initialization.
I/P : None
O/P : integer, 0 - no error.

********************************************************/

int dram_init(void)
{
	struct smem_ram_ptable rtable;
	int i;
	int mx = ARRAY_SIZE(rtable.parts);

	if (smem_ram_ptable_init(&rtable) > 0) {
		gd->ram_size = 0;
		for (i = 0; i < mx; i++) {
			if (rtable.parts[i].category == RAM_PARTITION_SDRAM
			 && rtable.parts[i].type == RAM_PARTITION_SYS_MEMORY) {
				gd->ram_size += rtable.parts[i].size;
			}
		}
		gboard_param->ddr_size = gd->ram_size;
	} else {
		gd->ram_size = gboard_param->ddr_size;
	}
	return 0;
}

/*******************************************************
Function description: initi Dram Bank size
I/P : None
O/P : integer, 0 - no error.

********************************************************/


void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = IPQ_KERNEL_START_ADDR;
	gd->bd->bi_dram[0].size = gboard_param->ddr_size - GENERATED_IPQ_RESERVE_SIZE;

}

static void gpio_init(void)
{
	ipq_configure_gpio(general_gpio, ARRAY_SIZE(general_gpio));
}
/**********************************************************
Function description: Display board information on console.
I/P : None
O/P : integer, 0 - no error.

**********************************************************/

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	//hw_watchdog_init();
	//WATCHDOG_RESET();

#ifdef CONFIG_RESET_BUTTON
	check_reset_button();
#endif

	return 0;
}
#endif /* CONFIG_DISPLAY_BOARDINFO */

extern void gpio_out(unsigned pin, unsigned value);

void board_gpio_init(void)
{
	
	gpio_out(GPIO_HW_RESET, 1);
	gpio_out(GPIO_WD_LATCH_CLR_L, 1);
	//gpio_out(GPIO_POR_RST_CLK, 1);
	gpio_init();
	/* turn off red LED */
	gpio_out(GPIO_RED_STATUS_LED,0);
	gpio_out(GPIO_TPM_RESET_L,0);
	//mdelay(200);
	//gpio_out(GPIO_TPM_RESET_L,1);
	gpio_out(GPIO_WD_DISABLE, 0);	//Enable external watdchdog 
	//gpio_out(GPIO_PCIE_1_RST_L,1);
	//gpio_out(GPIO_PCIE_2_RST_L,1);
	//gpio_out(GPIO_TPM_RST_L, 1);
	//gpio_out(GPIO_USB_ENABLE, 0);
}
void reset_cpu(ulong addr)
{
	gpio_out(GPIO_HW_RESET, 0);

	printf("\nResetting with watch dog!\n");

	writel(0, APCS_WDT0_EN);
	writel(1, APCS_WDT0_RST);
	writel(RESET_WDT_BARK_TIME, APCS_WDT0_BARK_TIME);
	writel(RESET_WDT_BITE_TIME, APCS_WDT0_BITE_TIME);
	writel(1, APCS_WDT0_EN);
	writel(1, APCS_WDT0_CPU0_WDOG_EXPIRED_ENABLE);

	for(;;);
}

static void configure_nand_gpio(void)
{
	/* EBI2 CS, CLE, ALE, WE, OE */
	gpio_tlmm_config(34, 1, 0, GPIO_NO_PULL, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(35, 1, 0, GPIO_NO_PULL, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(36, 1, 0, GPIO_NO_PULL, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(37, 1, 0, GPIO_NO_PULL, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(38, 1, 0, GPIO_NO_PULL, GPIO_10MA, GPIO_DISABLE);

	/* EBI2 BUSY */
	gpio_tlmm_config(39, 1, 0, GPIO_PULL_UP, GPIO_10MA, GPIO_DISABLE);

	/* EBI2 D7 - D0 */
	gpio_tlmm_config(40, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(41, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(42, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(43, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(44, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(45, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(46, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(47, 1, 0, GPIO_KEEPER, GPIO_10MA, GPIO_DISABLE);
}

void nand_clock_config(void);

void board_nand_init(void)
{
	struct ebi2cr_regs *ebi2_regs;
	extern int ipq_spi_init(void);

	if (gboard_param->flashdesc != NOR_MMC) {

		ebi2_regs = (struct ebi2cr_regs *) EBI2CR_BASE;

		nand_clock_config();
		configure_nand_gpio();

		/* NAND Flash is connected to CS0 */
		clrsetbits_le32(&ebi2_regs->chip_select_cfg0, CS0_CFG_MASK,
				CS0_CFG_SERIAL_FLASH_DEVICE);

		ipq_nand_init(IPQ_NAND_LAYOUT_LINUX);
	}

	//ipq_spi_init();
}

void ipq_get_part_details(void)
{
	int ret, i;
	uint32_t start;		/* block number */
	uint32_t size;		/* no. of blocks */

	ipq_smem_flash_info_t *smem = &ipq_smem_flash_info;

	struct { char *name; ipq_part_entry_t *part; } entries[] = {
		{ "0:HLOS", &smem->hlos },
		{ "0:NSS0", &smem->nss[0] },
		{ "0:NSS1", &smem->nss[1] },
	};

	for (i = 0; i < ARRAY_SIZE(entries); i++) {
		ret = smem_getpart(entries[i].name, &start, &size);
		if (ret < 0) {
			ipq_part_entry_t *part = entries[i].part;
			printf("cdp: get part failed for %s\n", entries[i].name);
			part->offset = 0xBAD0FF5E;
			part->size = 0xBAD0FF5E;
		}
		ipq_set_part_entry(smem, entries[i].part, start, size);
	}

	return;
}

#if defined(CONFIG_CMD_MTDPARTS)

/* partition handling routines */
extern int mtdparts_init(void);
extern int find_dev_and_part(const char *id, struct mtd_device **dev,
		      u8 *part_num, struct part_info **part);
#endif

/*
 * Get the kernel partition details from SMEM and populate the,
 * environment with sufficient information for the boot command to
 * load and execute the kernel.
 */
int board_late_init(void)
{
	unsigned int machid;
#ifdef CONFIG_CMD_UBI
	char mtdparts[128];
	int i;
#endif

#ifndef CONFIG_TOBA
	ipq_get_part_details();
#endif
        /* get machine type from SMEM and set in env */
	machid = gd->bd->bi_arch_number;
	/*VERNDOR used 0x136c,but our code should use 0x1260*/
	machid = 0x1260;
	if (machid != 0) {
		setenv_addr("machid", (void *)machid);
		gd->bd->bi_arch_number = machid;
	}

#ifdef CONFIG_CMD_UBI
	/* init mtd device */
	snprintf(mtdparts, sizeof(mtdparts), "mtdparts=nand0:0x%x@0x%x(%s),0x%x@0x%x(%s),0x%x@0x%x(%s)", \
		AP_PRODUCTION_MTD_SIZE, AP_PRODUCTION_IMAGE, AP_PRODUCTION_IMAGE_NAME, \
		AP_PROVISIONING_MTD_SIZE, AP_PROVISIONING_IMAGE, AP_PROVISIONING_IMAGE_NAME, \
		AP_UBI_FS_SIZE, AP_UBI_FS_OFFSET, AP_UBI_FS_NAME);

	setenv("mtdparts", mtdparts);

	if (mtdparts_init() != 0) {
		printf("Error initializing mtdparts!\n");
		return 1;
	}

	/* add mtd partitions */
	for (i = 0; i < CONFIG_MTD_MAX_DEVICE; i++) {
		struct mtd_device *dev;
		struct part_info *part;
		struct mtd_info *info;
		struct mtd_partition mtd_part;
		u8 pnum;

		snprintf(mtdparts, sizeof(mtdparts), "nand0,%d", i);
		if (find_dev_and_part(mtdparts, &dev, &pnum, &part) != 0) {
			printf("Error: Can't find mtd device %s\n", mtdparts);
		}

		snprintf(buffer[i], sizeof(buffer[i]), "mtd=%d", pnum);
		snprintf(mtdparts, sizeof(mtdparts), "%s%d", MTD_DEV_TYPE(dev->id->type), dev->id->num);

		info = get_mtd_device_nm(mtdparts);
		if (IS_ERR(info)) {
			printf("Partition %s not found on device %s!\n", buffer[i], mtdparts);
			return 1;
		}

		memset(&mtd_part, 0, sizeof(mtd_part));
		mtd_part.name = buffer[i];
		mtd_part.size = part->size;
		mtd_part.offset = part->offset;
		add_mtd_partitions(info, &mtd_part, 1);
		ubi_mtd_param_parse(buffer[i], NULL);
	}

	/* init ubi devices */
	ubi_init();

	/* create ubi volume */
	for (i = 0; i < CONFIG_MTD_MAX_DEVICE; i++) {
		struct ubi_device *ubi;
		struct ubi_mkvol_req req;
		struct mtd_device *dev;
		struct part_info *part;
		u8 pnum;

		snprintf(mtdparts, sizeof(mtdparts), "nand0,%d", i);
		if (find_dev_and_part(mtdparts, &dev, &pnum, &part) != 0) {
			printf("Error: Can't find mtd device %s\n", mtdparts);
		}

		if (ubi_get_volume(part->name)) {
			/* the volume has been created, skip it */
			continue;
		}

		ubi = ubi_get_device(i);

		memset(&req, 0, sizeof(req));
		req.vol_type = UBI_DYNAMIC_VOLUME;
		req.vol_id = UBI_VOL_NUM_AUTO;
		req.alignment = 1;
        /* we reserve 4 more blocks for bad block handling */
        req.bytes = (ubi->avail_pebs - 4) * ubi->leb_size;

		strcpy(req.name, part->name);
		req.name_len = strlen(part->name);
		req.name[req.name_len] = '\0';
		req.padding1 = 0;

		printf("Creating %s volume %s of size %lld\n",
		        "dynamic", part->name, req.bytes);
		/* Call real ubi create volume */
		ubi_create_volume(ubi, &req);
	}
#endif
	return 0;
}

/*
 * This function is called in the very beginning.
 * Retreive the machtype info from SMEM and map the board specific
 * parameters. Shared memory region at Dram address 0x40400000
 * contains the machine id/ board type data polulated by SBL.
 */
int board_early_init_f(void)
{
	gboard_param = get_board_param(smem_get_board_machtype());

	board_gpio_init();

	return 0;
}

/*
 * Gets the ethernet address from the ART partition table and return the value
 */
int get_eth_mac_address(uchar *enetaddr, uint no_of_macs)
{
	s32 ret = 0;
	int j;
	char *addr;
	char *tmp;
	char *end;
	uchar *mac;

	addr = getenv("ethaddr");

	if (addr == NULL)
		return -1;

	tmp = addr;
	mac = enetaddr;
	/* we assume glenfarclas has one ethernet port, and manufacturing
	 * always assigns an even base MAC, which currently has to be
	 * true or the radio driver has issues
	 */
	for (j=0; j<6; j++) {
		*mac = tmp ? simple_strtoul(tmp, &end, 16) : 0;
		if (tmp) {
			tmp = (*end) ? end + 1 : end;
		}
		mac ++;
	}

	return ret;
}

void ipq_configure_gpio(gpio_func_data_t *gpio, uint count)
{
	int i;

	for (i = 0; i < count; i++) {
		gpio_tlmm_config(gpio->gpio, gpio->func, gpio->dir,
				gpio->pull, gpio->drvstr, gpio->enable);
		gpio++;
	}
}

int board_eth_init(bd_t *bis)
{
	int status;

    /* 
        Make sure PHY has a stable reset according hw reqirement. 
        Toba GE Phy reset is active high
    */
    gpio_out(GPIO_PHY_0_RST_L, 1);
    mdelay(100);
    gpio_out(GPIO_PHY_0_RST_L, 0);

	ipq_gmac_common_init(gboard_param->gmac_cfg);

	ipq_configure_gpio(gboard_param->gmac_gpio,
			gboard_param->gmac_gpio_count);

	status = ipq_gmac_init(gboard_param->gmac_cfg);
	return status;
}

void ar8033_reg_init(uint32_t phy_addr)
{
	/* release PHY reset,Toba PHY RST is active high*/
	gpio_out(GPIO_PHY_0_RST_L, 0);
	//gpio_out(GPIO_PHY_1_RST_L, 1);
	mdelay(10);

	/* Disable tx of extend next pages */
	phy_reg_write(0, phy_addr, 0x4, 0xde1);

	/* RGMII pins -> HiZ, restart auto-negotiation */
	phy_reg_write(0, phy_addr, 0x0, 0x1740);
}

#ifdef CONFIG_IPQ806X_PCI

static void ipq_pcie_write_mask(uint32_t addr,
				uint32_t clear_mask, uint32_t set_mask)
{
	uint32_t val;

	val = (readl(addr) & ~clear_mask) | set_mask;
	writel(val, addr);
}

static void ipq_pcie_parf_reset(uint32_t addr, int domain, int assert)

{
	if (assert)
		ipq_pcie_write_mask(addr, 0, domain);
	else
		ipq_pcie_write_mask(addr, domain, 0);
}

static void ipq_pcie_config_controller(int id)
{
	pcie_params_t 	*cfg;
	cfg = &gboard_param->pcie_cfg[id];

	/*
	 * program and enable address translation region 0 (device config
	 * address space); region type config;
	 * axi config address range to device config address range
	 */
	writel(0, cfg->pcie20 + PCIE20_PLR_IATU_VIEWPORT);

	writel(4, cfg->pcie20 + PCIE20_PLR_IATU_CTRL1);
	writel(BIT(31), cfg->pcie20 + PCIE20_PLR_IATU_CTRL2);
	writel(cfg->axi_conf , cfg->pcie20 + PCIE20_PLR_IATU_LBAR);
	writel(0, cfg->pcie20 + PCIE20_PLR_IATU_UBAR);
	writel((cfg->axi_conf + PCIE_AXI_CONF_SIZE - 1),
				cfg->pcie20 + PCIE20_PLR_IATU_LAR);
	writel(MSM_PCIE_DEV_CFG_ADDR,
				cfg->pcie20 + PCIE20_PLR_IATU_LTAR);
	writel(0, cfg->pcie20 + PCIE20_PLR_IATU_UTAR);

	/*
	 * program and enable address translation region 2 (device resource
	 * address space); region type memory;
	 * axi device bar address range to device bar address range
	 */
	writel(2, cfg->pcie20 + PCIE20_PLR_IATU_VIEWPORT);

	writel(0, cfg->pcie20 + PCIE20_PLR_IATU_CTRL1);
	writel(BIT(31), cfg->pcie20 + PCIE20_PLR_IATU_CTRL2);
	writel(cfg->axi_bar_start, cfg->pcie20 + PCIE20_PLR_IATU_LBAR);
	writel(0, cfg->pcie20 + PCIE20_PLR_IATU_UBAR);
	writel((cfg->axi_bar_start + cfg->axi_bar_size
		- PCIE_AXI_CONF_SIZE - 1), cfg->pcie20 + PCIE20_PLR_IATU_LAR);
	writel(cfg->axi_bar_start, cfg->pcie20 + PCIE20_PLR_IATU_LTAR);
	writel(0, cfg->pcie20 + PCIE20_PLR_IATU_UTAR);

	/* 1K PCIE buffer setting */
	writel(0x3, cfg->pcie20 + PCIE20_AXI_MSTR_RESP_COMP_CTRL0);
	writel(0x1, cfg->pcie20 + PCIE20_AXI_MSTR_RESP_COMP_CTRL1);

}


static inline void ipq_pci_gpio_set(uint32_t gpio, uint32_t set)
{
	uint32_t addr;
	uint32_t val;

	addr = GPIO_IN_OUT_ADDR(gpio);
	val = readl(addr);
	val |= (1<<set);
	writel(val,addr);
}
#if 0
{
        unsigned int machid;
        /* get machine type from SMEM and set in env */
        machid = gd->bd->bi_arch_number;

        gpio_func_data_t *gpio_0 = gboard_param->pcie_cfg[0].pci_rst_gpio;
        gpio_func_data_t *gpio_1 = gboard_param->pcie_cfg[1].pci_rst_gpio;
        gpio_func_data_t *gpio_2 = gboard_param->pcie_cfg[2].pci_rst_gpio;

        if (machid == MACH_TYPE_IPQ806X_RUMI3) {
                gpio_0->gpio = -1;
                gpio_1->gpio = -1;
                gpio_2->gpio = -1;
        } else if (machid == MACH_TYPE_IPQ806X_DB147) {
                gpio_1->gpio = -1;
             //   gpio_2->gpio = PCIE_1_RST_GPIO;
        } else if ((machid == MACH_TYPE_IPQ806X_AP148) ) {
                gpio_2->gpio = -1;
        }
}	
#endif	
void board_pci_init()
{
	int i,j;
	pcie_params_t 		*cfg;
	gpio_func_data_t	*gpio_data;
	uint32_t val;

	for (i = 0; i < PCI_MAX_DEVICES; i++) {
		cfg = &gboard_param->pcie_cfg[i];
		gpio_data = cfg->pci_rst_gpio;
		cfg->axi_conf = cfg->axi_bar_start +
					cfg->axi_bar_size - PCIE_AXI_CONF_SIZE;
		if (gpio_data->gpio != -1)
			gpio_tlmm_config(gpio_data->gpio, gpio_data->func,
					gpio_data->dir,	gpio_data->pull,
					gpio_data->drvstr, gpio_data->enable);

		//Sierra, this delay is necessary,else belineer may not link up
		mdelay(1000);
		/* deassert PCIe reset link */
		gpio_out(gpio_data->gpio, 1);
		udelay(3000);

		/* assert PCIe PARF reset while powering the core */
		ipq_pcie_parf_reset(cfg->pcie_rst, BIT(6), 0);

		ipq_pcie_parf_reset(cfg->pcie_rst, BIT(2), 1);
		pcie_clock_config(cfg->pci_clks);
		/*
		* de-assert PCIe PARF reset;
		* wait 1us before accessing PARF registers
		*/
		ipq_pcie_parf_reset(cfg->pcie_rst, BIT(2), 0);
		udelay(1);
		/* Set Tx Termination Offset */
		val = readl(cfg->parf + PCIE20_PARF_PHY_CTRL) |
				PCIE20_PARF_PHY_CTRL_PHY_TX0_TERM_OFFST(7);
		writel(val, cfg->parf + PCIE20_PARF_PHY_CTRL);

		/* PARF programming */
		writel(PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN1(0x28) |
			PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_3_5DB(0x28) |
			PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_6DB(0x28),
			 cfg->parf + PCIE20_PARF_PCS_DEEMPH);

		writel(PCIE20_PARF_PCS_SWING_TX_SWING_FULL(0x7F) |
			PCIE20_PARF_PCS_SWING_TX_SWING_LOW(0x7F),
			cfg->parf + PCIE20_PARF_PCS_SWING);

		writel((4<<24), cfg->parf + PCIE20_PARF_CONFIG_BITS);

		writel(0x11019, cfg->pci_clks->parf_phy_refclk);
		writel(0x10019, cfg->pci_clks->parf_phy_refclk);


		/* enable access to PCIe slave port on system fabric */
		if (i == 0) {
			writel(BIT(4), PCIE_SFAB_AXI_S5_FCLK_CTL);
		}

		udelay(1);
		/* de-assert PICe PHY, Core, POR and AXI clk domain resets */
		ipq_pcie_parf_reset(cfg->pcie_rst, BIT(5), 0);
		ipq_pcie_parf_reset(cfg->pcie_rst, BIT(4), 0);
		ipq_pcie_parf_reset(cfg->pcie_rst, BIT(3), 0);
		ipq_pcie_parf_reset(cfg->pcie_rst, BIT(0), 0);

		/* deassert PCIe reset link */
		//gpio_out(gpio_data->gpio, 1);

		/* enable link training */
		ipq_pcie_write_mask(cfg->elbi + PCIE20_ELBI_SYS_CTRL, 0,
								BIT(0));
		udelay(500);

		for (j = 0; j < 1000; j++) {
			val = readl(cfg->pcie20 + PCIE20_CAP_LINKCTRLSTATUS);
			if (val & BIT(29)) {
				printf("PCIE%d: link up\n", i);
				cfg->linkup = 1;
				break;
			}
			udelay(1000);
		}

	        if (!cfg->linkup)
	            printf("PCIE%d: link down\n", i);

		ipq_pcie_config_controller(i);
	}
}

void board_pci_deinit(void)
{
	int i;
	pcie_params_t 		*cfg;
	gpio_func_data_t	*gpio_data;

	for (i = 0; i < PCI_MAX_DEVICES; i++) {
		cfg = &gboard_param->pcie_cfg[i];
		gpio_data = cfg->pci_rst_gpio;
		gpio_tlmm_config(gpio_data->gpio, 0, GPIO_INPUT,
					GPIO_NO_PULL, GPIO_2MA, GPIO_ENABLE);
		writel(0x7d, cfg->pcie_rst);
		writel(1, cfg->parf + PCIE20_PARF_PHY_CTRL);
		pcie_clock_shutdown(cfg->pci_clks);
	}

}
#endif /* CONFIG_IPQ806X_PCI */
#if defined(CONFIG_RESET_BUTTON)
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
			reset_btn[0] = 'D';
			reset_btn[1] = 'D';
		} else {
			reset_btn[0] = 'R';
			reset_btn[1] = 'B';
			flash_status_led();
		}
	} else {
		/* no config clear */
		reset_btn[0] = 'N';
		reset_btn[1] = 'R';
	}
}

static void
execute_config_clear(void)
{
	extern int do_factory_reset(cmd_tbl_t *, int, int, char **);
	char *av[3];
	char *reset_env_var = 0;

	WATCHDOG_RESET();

	if (reset_btn[0] == 'N' && reset_btn[1] == 'R') {
		return;
	} else if (reset_btn[0] == 'D' && reset_btn[1] == 'D') {
		printf("**** Reset Request Disabled by Environment ****\n");
	} else if (reset_btn[0] != 'R' || reset_btn[1] != 'B') {
		printf("**** Unrecognized Reset State (0x%02x/0x%02x) -- Ignoring ****\n",
		reset_btn[0], reset_btn[1]);
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

#ifdef CFG_LATE_BOARD_INIT
#ifdef CONFIG_CMD_PCI
int n_radios = 0;

static void
radio_callback(int n)
{
	n_radios = n;
}
#endif

void
late_board_init(void)
{
#ifdef CONFIG_CMD_PCI
	extern void ar5416_install_callback(void *);
	extern int ar5416_initialize(void);

	ar5416_install_callback(radio_callback);
	ar5416_initialize();
#endif

	printf("Reset: ");
	if (gpio_value(GPIO_WD_STS_IN)) {
		printf("watchdog\n");
	} else {
		if (gpio_value(GPIO_POR_STS_IN)) {
			printf("warm\n");
		} else {
			printf("cold\n");
		}
	}
#ifdef CONFIG_RESET_BUTTON
	execute_config_clear();
#endif
}
#endif

int allow_usb;


void
board_print_power_status(void)
{
//#ifdef GPIO_POWER_DC
	//int dc = gpio_value(GPIO_POWER_DC);                 /* active high */
	int at = !gpio_value(GPIO_POWER_AT);                /* active low */

	allow_usb = 0;

	 if (!at) {
		printf("Power: 802.3af POE\n");
	} else if (at) {
		printf("Power: 802.3at POE\n");
		//allow_usb = 1;
	} else {
		printf("Power: Unknown\n");
	}
//#endif
}
/*void dump_pci_reg(int id)
{
	int i;
	int size = 10;
	pcie_params_t 		*cfg;
	cfg = &gboard_param->pcie_cfg[id];
	for (i=0;i<size;i++)
	{
		printf("%3d:%08X",i,);
	}
}*/


int
do_gpio_pins(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#ifdef GPIO_POWER_DC
	printf("DC:              %s\n", gpio_value(GPIO_POWER_DC) ? "high" : "low");
	printf("AT:              %s\n", gpio_value(GPIO_POWER_AT) ? "high" : "low");
#endif
#ifdef GPIO_CONFIG_CLEAR
	printf("Config reset:    %s\n", gpio_value(GPIO_CONFIG_CLEAR) ? "inactive" : "pending");
#endif
#ifdef GPIO_PHY0_INT
	printf("PHY0 interrupt:   %s\n", gpio_value(GPIO_PHY0_INT) ? "inactive" : "pending");
#endif
#ifdef GPIO_PHY1_INT
	printf("PHY1 interrupt:   %s\n", gpio_value(GPIO_PHY1_INT) ? "inactive" : "pending");
#endif
#ifdef GPIO_USB_FLAG
	printf("USB overcurrent:  %s\n", gpio_value(GPIO_USB_FLAG) ? "no" : "yes");
#endif
#ifdef GPIO_POR_STS_IN
	printf("Cold reset:       %s\n", gpio_value(GPIO_POR_STS_IN) ? "no" : "yes");
#endif
#ifdef GPIO_WD_STS_IN
	printf("Watchdog reset:   %s\n", gpio_value(GPIO_WD_STS_IN) ? "yes" : "no");
#endif
	return 0;
}
U_BOOT_CMD(
	gpio_pins,    1,    1,     do_gpio_pins,
	"gpio_pins    - show GPIO pin status\n",
	" Usage: gpio_pins \n"
);
