/*
 * Copyright (c) 2013 The Linux Foundation. All rights reserved.
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <nand.h>
#include <asm/arch-ipq806x/smem.h>

#define IPQ_IMG_LOAD_TEMP_ADDR (CONFIG_SYS_SDRAM_BASE + (32 << 20))

#define img_addr ((void *)IPQ_IMG_LOAD_TEMP_ADDR)

static int debug = 0;
static ipq_smem_flash_info_t *sfi = &ipq_smem_flash_info;
int rootfs_part_avail = 1;
extern board_ipq806x_params_t *gboard_param;

/**
 * check if the image and its header is valid and move it to
 * load address as specified in the header
 */
static int load_nss_img(const char *runcmd, char *args, int argslen,
						int nsscore)
{
	char cmd[128];
	int ret;

	if (debug)
		printf(runcmd);

	if ((ret = run_command(runcmd, 0)) != CMD_RET_SUCCESS) {
		return ret;
	}

	sprintf(cmd, "bootm start 0x%x; bootm loados", (uint32_t)img_addr);

	if (debug)
		printf(cmd);

	if ((ret = run_command(cmd, 0)) != CMD_RET_SUCCESS) {
		return ret;
	}

	if (args) {
		snprintf(args, argslen, "qca-nss-drv.load%d=0x%x,"
				"qca-nss-drv.entry%d=0x%x,"
				"qca-nss-drv.string%d=\"%.*s\"",
				nsscore, image_get_load(img_addr),
				nsscore, image_get_ep(img_addr),
				nsscore, IH_NMLEN, image_get_name(img_addr));
	}

	return ret;
}

/*
 * Set the root device and bootargs for mounting root filesystem.
 */
static void set_fs_bootargs(void)
{
	char *bootargs;

	if (sfi->flash_type == SMEM_BOOT_SPI_FLASH &&
		(gboard_param->flashdesc == ONLY_NOR ||
		 gboard_param->flashdesc == NOR_MMC)) {
		bootargs = "root=/dev/mtdblock5";
	} else if (sfi->flash_type == SMEM_BOOT_SPI_FLASH && gboard_param->flashdesc == NAND_NOR) {
		bootargs = "root=/dev/mtdblock6";
	} else if (sfi->flash_type == SMEM_BOOT_NAND_FLASH) {
		if (nand_info[CONFIG_IPQ_SPI_NAND_INFO_IDX].size) {
			bootargs = "ubi.mtd=5 rootfstype=squashfs root=/dev/mtdblock7";
		} else {
			bootargs = "ubi.mtd=5 rootfstype=squashfs root=/dev/mtdblock6";
		}
	} else {
		printf("bootipq: unsupported boot flash type\n");
		return;
	}

	if (getenv("fsbootargs") == NULL)
		setenv("fsbootargs", bootargs);

	run_command("setenv bootargs ${bootargs} ${fsbootargs}", 0);
}

/**
 * Inovke the dump routine and in case of failure, do not stop unless the user
 * requested to stop
 */
static int inline do_dumpipq_data(void)
{
	uint64_t etime;

	if (run_command("dumpipq_data", 0) != CMD_RET_SUCCESS) {
		printf("\nAuto crashdump saving failed!"
			"\nPress any key within 10s to take control of U-Boot");

		etime = get_timer_masked() + (10 * CONFIG_SYS_HZ);
		while (get_timer_masked() < etime) {
			if (tstc())
				break;
		}

		if (get_timer_masked() < etime)
			return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

/**
 * Load the NSS images and Kernel image and transfer control to kernel
 */
static int do_bootipq(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char bootargs[IH_NMLEN+32];
	char runcmd[128];
	int nandid = 0;

#ifdef CONFIG_IPQ_APPSBL_DLOAD
	unsigned long * dmagic1 = (unsigned long *) 0x2A03F000;
	unsigned long * dmagic2 = (unsigned long *) 0x2A03F004;
#endif

	if (argc == 2 && strncmp(argv[1], "debug", 5) == 0)
		debug = 1;

#ifdef CONFIG_IPQ_APPSBL_DLOAD
	/* check if we are in download mode */
	if (*dmagic1 == 0xE47B337D && *dmagic2 == 0x0501CAB0) {
		/* clear the magic and run the dump command */
		*dmagic1 = 0;
		*dmagic2 = 0;
		uint64_t etime = get_timer_masked() + (10 * CONFIG_SYS_HZ);

		printf("\nCrashdump magic found."
			"\nHit any key within 10s to stop dump activity...");
		while (!tstc()) {       /* while no incoming data */
			if (get_timer_masked() >= etime) {
				if (do_dumpipq_data() == CMD_RET_FAILURE)
					return CMD_RET_FAILURE;
				/* reset the system before boot */
				run_command("reset", 0);
				break;
			}
		}

	}
#endif

	set_fs_bootargs();

	/* check the smem info to see which flash used for booting */
	if (sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
		nandid = 1;
		if (debug) {
			printf("Using nand device 1\n");
		}
		run_command("nand device 1", 0);
	} else if (sfi->flash_type == SMEM_BOOT_NAND_FLASH) {
		if (debug) {
			printf("Using nand device 0\n");
		}
	} else {
		printf("Unsupported BOOT flash type\n");
		return -1;
	}

	/* check the smem info to see whether the partition size is valid.
	 * refer board/qcom/ipq806x_cdp/ipq806x_cdp.c:ipq_get_part_details
	 * for more details
	 */
	if (sfi->nss[0].size != 0xBAD0FF5E) {
		sprintf(runcmd, "nand read 0x%x 0x%llx 0x%llx",
				img_addr, sfi->nss[0].offset, sfi->nss[0].size);

		if (load_nss_img(runcmd, bootargs, sizeof(bootargs), 0)
				!= CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;

		if (getenv("nssbootargs0") == NULL)
			setenv("nssbootargs0", bootargs);

		run_command("setenv bootargs ${bootargs} ${nssbootargs0}", 0);
	}

	if (sfi->nss[1].size != 0xBAD0FF5E) {
		sprintf(runcmd, "nand read 0x%x 0x%llx 0x%llx",
				img_addr, sfi->nss[1].offset, sfi->nss[1].size);

		if (load_nss_img(runcmd, bootargs, sizeof(bootargs), 1)
				!= CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;

		if (getenv("nssbootargs1") == NULL)
			setenv("nssbootargs1", bootargs);

		run_command("setenv bootargs ${bootargs} ${nssbootargs1}", 0);
	}

	if (debug) {
		run_command("printenv bootargs", 0);
		printf("Booting from flash\n");
	}

	snprintf(runcmd, sizeof(runcmd), "set autostart yes;"
			"nboot 0x%x %d 0x%llx", img_addr, nandid, sfi->hlos.offset);
	if (debug)
		printf(runcmd);

	if (run_command(runcmd, 0) != CMD_RET_SUCCESS)
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

/* fixme_db149 */
#if 1
U_BOOT_CMD(bootipq, 2, 0, do_bootipq,
	   "bootipq from flash device",
	   "bootipq [debug] - Load image(s) and boots the kernel\n");
#endif
