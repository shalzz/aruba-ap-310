/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.

*******************************************************************************/
#include <common.h>
#include <command.h>
#if defined(CONFIG_CMD_SAR)
#include "cpu/mvCpu.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"
#include "boardEnv/mvBoardEnvLib.h"

static int do_sar_list(int argc, char *argv[])
{
	MV_CPU_ARM_CLK cpuDdrL2Tbl[] = MV_CPU_DDR_L2_CLCK_TBL;
	int i = 0;
	const char *cmd;

	if (argc < 1)
		goto usage;
	cmd = argv[0];

	if (strcmp(cmd, "freq") == 0) {
		printf("#\tcpuClk\tddrClk\tl2Clk\n");
		while(cpuDdrL2Tbl[i].satrValue != (MV_U32) -1) {
			printf("%d\t%d\t%d\t%d\n",
					cpuDdrL2Tbl[i].satrValue,
					cpuDdrL2Tbl[i].cpuClk/1000000,
					cpuDdrL2Tbl[i].ddrClk/1000000,
					cpuDdrL2Tbl[i].l2Clk/1000000);
			i++;
		}
	}
	if (strcmp(cmd, "sscg") == 0) {
		printf("SSCG Mode:\n");
		printf("\t0 - SSCG enable\n");
		printf("\t1 - SSCG bypass (disable)\n");
	}
	if (strcmp(cmd, "tclk") == 0) {
		printf("TCLK Modes:\n");
		printf("\t0 - 125 Mhz\n");
		printf("\t1 - 166 Mhz\n");
		printf("\t2 - 200 Mhz\n");
		printf("\t3 - 250 Mhz\n");
	}
	if (strcmp(cmd, "pexclk") == 0) {
		printf("PEX Clock Configuration:\n");
		printf("\t0 - PEX_CLK pins are input pins\n");
		printf("\t1 - PEX_CLK pins are output pins\n");
	}
	if (strcmp(cmd, "pon") == 0) {
		printf("Select EPON or GPON operation mode:\n");
		printf("\t0 - GPON Mode\n");
		printf("\t1 - EPON Mode\n");
	}

	if (strcmp(cmd, "l2exist") == 0) {
		printf("Select L2-Exist mode:\n");
		printf("\t0 - L2 is disabled.\n");
		printf("\t1 - L2 is enabled.\n");
	}

	return 0;
usage:
	printf("Usage: sar list [options] (see help) \n");
	return 1;
}
static int do_sar_read(int argc, char *argv[])
{
	MV_CPU_ARM_CLK cpuDdrL2Tbl[] = MV_CPU_DDR_L2_CLCK_TBL;
	MV_U32 satrValue;

	int i = 0;
	const char *cmd;

	if (argc < 1)
		goto usage;
	cmd = argv[0];

	if (strcmp(cmd, "freq") == 0) {
		satrValue = mvBoardFreqGet();
	
		while(cpuDdrL2Tbl[i].satrValue != (MV_U32) -1) {
			if (satrValue == cpuDdrL2Tbl[i].satrValue)
				printf("Current mode is SatR=%d, CPU=%d MHz, DDR=%d MHz, L2=%d MHz.\n",
						cpuDdrL2Tbl[i].satrValue,
						cpuDdrL2Tbl[i].cpuClk / 1000000,
						cpuDdrL2Tbl[i].ddrClk / 1000000,
						cpuDdrL2Tbl[i].l2Clk / 1000000);
			i++;
		}
	}
	if (strcmp(cmd, "freqval") == 0) {
		satrValue = mvBoardFreqGet();
		
		printf("Current mode is 0x%x.\n", satrValue);
	}

	if (strcmp(cmd, "sscg") == 0) {
		printf("Current SSCG mode is %d\n", mvBoardSSCGGet());
	}
	if (strcmp(cmd, "pon") == 0) {
		printf("Current Pon mode is %d\n", mvBoardPonGet());
	}
	if (strcmp(cmd, "pexclk") == 0) {
		printf("Current PEX-0 Clock configuration is %d\n", mvBoardPexClkGet(0));
		printf("Current PEX-1 Clock configuration is %d\n", mvBoardPexClkGet(1));
	}

	if (strcmp(cmd, "tclk") == 0) {
		printf("Current TCLK mode is %d\n", mvBoardTClkGet());
	}

	if (strcmp(cmd, "l2exist") == 0) {
		printf("Current L2 Exist mode is %d.\n", mvBoardSaRL2ExistGet());
	}

	return 0;
usage:
	printf("Usage: sar read [options] (see help) \n");
	return 1;
}
static int do_sar_write(int argc, char *argv[])
{
	MV_CPU_ARM_CLK cpuDdrL2Tbl[] = MV_CPU_DDR_L2_CLCK_TBL;
	MV_U32 satrValue = (MV_U32) -1;
	MV_U32 cpuClk;
	MV_U32 ddrClk;
	MV_U32 l2Clk;
	MV_U32 temp;
	MV_U32 dev;
	int i = 0;
	const char *cmd;

	if (argc < 2)
		goto usage;
	cmd = argv[0];

	if (strcmp(cmd, "freq") == 0) {
		if (argc < 4)
			goto usage;

		cpuClk = simple_strtoul(argv[1], NULL, 10) * 1000000;
		ddrClk = simple_strtoul(argv[2], NULL, 10) * 1000000;
		l2Clk = simple_strtoul(argv[3], NULL, 10) * 1000000;
	
		while(cpuDdrL2Tbl[i].satrValue != (MV_U32) -1) {
			if ((cpuClk == cpuDdrL2Tbl[i].cpuClk) && (l2Clk == cpuDdrL2Tbl[i].l2Clk) && (ddrClk == cpuDdrL2Tbl[i].ddrClk))
			{
				satrValue = cpuDdrL2Tbl[i].satrValue;
				break;
			}
			i++;
		}
		if (satrValue == (MV_U32) -1)
		{
			printf("Mode not supported!\n");
			return 1;
		}
		if (mvBoardFreqSet(satrValue) != MV_OK)
		{
			printf("Write S@R failed!\n");
			return 1;
		}
	}

	if (strcmp(cmd, "freqval") == 0) {
		if (argc < 2)
			goto usage;

		satrValue = simple_strtoul(argv[1], NULL, 16);
	
		if (satrValue == (MV_U32) -1) {
			printf("Mode not supported!\n");
			return 1;
		}
		if (mvBoardFreqSet(satrValue) != MV_OK) {
			printf("Write S@R failed!\n");
			return 1;
		}
	}

	if (strcmp(cmd, "sscg") == 0) {
		if (argc < 2)
			goto usage;

		temp = simple_strtoul(argv[1], NULL, 10);
		if ((temp != 0) && (temp != 1))
		{
			printf("Mode not supported!\n");
			return 1;
		}
		if (mvBoardSSCGSet(temp) != MV_OK)
		{
			printf("Write S@R failed!\n");
			return 1;
		}
	}
	if (strcmp(cmd, "pexclk") == 0) {
		if (argc < 2)
			goto usage;
		dev = simple_strtoul(argv[1], NULL, 10);

		temp = simple_strtoul(argv[2], NULL, 10);
		if ((temp != 0) && (temp != 1))
		{
			printf("Mode not supported!\n");
			return 1;
		}

		if (mvBoardPexClkSet(dev, temp) != MV_OK)
		{
			printf("Write S@R failed!\n");
			return 1;
		}
	}
	if (strcmp(cmd, "pon") == 0) {
		if (argc < 2)
			goto usage;

		temp = simple_strtoul(argv[1], NULL, 10);
		if ((temp != 0) && (temp != 1))
		{
			printf("Mode not supported!\n");
			return 1;
		}
		if (mvBoardPonSet(temp) != MV_OK)
		{
			printf("Write S@R failed!\n");
			return 1;
		}
	}
	if (strcmp(cmd, "tclk") == 0) {
		if (argc < 2)
			goto usage;

		temp = simple_strtoul(argv[1], NULL, 10);
		if ((temp < 0) || (temp > 3))
		{
			printf("Mode not supported!\n");
			return 1;
		}
		if (mvBoardTClkSet(temp) != MV_OK)
		{
			printf("Write S@R failed!\n");
			return 1;
		}
	}

	if (strcmp(cmd, "l2exist") == 0) {
		if (argc < 2)
			goto usage;
		temp = simple_strtoul(argv[1], NULL, 10);
		if ((temp != 0) && (temp != 1))
		{
			printf("Mode not supported!\n");
			return 1;
		}
		
		if (mvBoardSaRL2ExistSet(temp) != MV_OK) {
			printf("Write S@R failed.\n");
			return 1;
		}
	}

	return 0;
usage:
	printf("Usage: sar write [options] (see help) \n");
	return 1;
}

int do_sar(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	const char *cmd;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strcmp(cmd, "list") == 0)
		return do_sar_list(argc - 2, argv + 2);
	if (strcmp(cmd, "write") == 0) {
		if (do_sar_write(argc - 2, argv + 2) == 0)
			do_sar_read(argc - 2, argv + 2);
		return 0;
	}
	if (strcmp(cmd, "read") == 0)
		return do_sar_read(argc - 2, argv + 2);

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(SatR, 6, 1, do_sar,
	"Sample At Reset sub-system\n",

	"list freq     - prints the S@R modes list\n"
	"SatR list sscg     - prints the SSCG modes list\n"
	"SatR list pexclk   - prints the PEX Clk modes list\n"
	"SatR list tclk     - prints the TClk modes list\n"
	"SatR list pon      - prints the Pon modes list\n"
	"SatR list l2exist  - prints L2-Exist modes list.\n\n"

	"SatR read freq     - read and print the CPU / DDR / L2 frequency\n"
	"SatR read freqval  - read and print the frequency value\n"
	"SatR read sscg     - read and print the SSCG value\n"
	"SatR read pexclk   - read and print the PEX Clk value\n"
	"SatR read tclk     - read and print the TCLK value\n"
	"SatR read pon      - read and print the Pon type\n"
	"SatR read l2exist  - read and print L2-Exist mode.\n\n"

	"SatR write freq cpu ddr l2 - write the S@R with cpu, l2 and ddr values\n"
	"				  cpu: new CPU clock (in MHz)\n"
	"				  ddr: new DDR clock (in MHz(\n"
	"				  l2:  new L2 clock (in MHz)\n"
	"SatR write freqval <frequency value> - Write new value for CPU / DDR / L2 frequency.\n"
	"SatR write sscg <0/1>      - write the SSCG value with 1 or 0\n"
	"SatR write pexclk <unit 0/1> <0/1>    - write the PEX Clk conf\n"
	"SatR write tclk <0/1/2/3>      - write the TCLK value\n"
	"SatR write pon <0/1> - write the Pon type\n"
	"SatR write l2exist <0/1> - Set L2-Exist mode.\n"
	

);
#endif /*defined(CONFIG_CMD_SAR)*/

