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

#include <config.h>
#include <common.h>
#include <command.h>
#include <pci.h>

#include "mvTypes.h"
#include "mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"

#if defined(MV_INC_BOARD_NOR_FLASH)
#include "norflash/mvFlash.h"
#endif

#if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH)
#include "eth-phy/mvEthPhy.h"
#include "eth/gbe/mvEthDebug.h"
#endif

#if defined(MV_INCLUDE_PEX)
#include "pex/mvPex.h"
#endif

#include "eth/gbe/mvEthDebug.h"

#if defined(MV_INCLUDE_IDMA)
#include "idma/mvIdma.h"
#include "sys/mvSysIdma.h"
#endif

#include "ethSwitch/mvSwitch.h"

#if defined(CONFIG_CMD_NAND)
#include <nand.h>

/* references to names in cmd_nand.c */
//#define NANDRW_READ		0x01
//#define NANDRW_WRITE	0x00
//#define NANDRW_JFFS2	0x02
//extern struct nand_chip nand_dev_desc[];
extern nand_info_t nand_info[];       /* info for NAND chips */

/* int nand_rw (struct nand_chip* nand, int cmd,
	    size_t start, size_t len,
	    size_t * retlen, u_char * buf);
 int nand_erase(struct nand_chip* nand, size_t ofs,
				size_t len, int clean);
*/
//extern int nand_erase_opts(nand_info_t *meminfo, const nand_erase_options_t *opts);
//extern int nand_write_opts(nand_info_t *meminfo, const nand_write_options_t *opts);
#endif /* CONFIG_NAND_BOOT */

void mvEthSwitchRegWrite(MV_U32 ethPortNum, MV_U32 phyAddr,
                                 MV_U32 regOffs, MV_U16 data);

void mvEthSwitchRegRead(MV_U32 ethPortNum, MV_U32 phyAddr,
                             MV_U32 regOffs, MV_U16 *data);

void mvEthSwitchPhyRegWrite(MV_U32 ethPortNum, MV_U16 prt,
                                 MV_U16 regOffs, MV_U16 data);

void mvEthSwitchPhyRegRead(MV_U32 ethPortNum, MV_U16 prt,
                             MV_U16 regOffs, MV_U16 *data);

#ifdef CONFIG_SPI_FLASH
#include <spi_flash.h>

extern struct spi_flash *flash;
#endif

#ifdef MV_NOR_BOOT
extern flash_info_t flash_info[];       /* info for FLASH chips */
#endif

#if defined(CONFIG_CMD_FLASH) || defined(CONFIG_CMD_NAND) || defined(CONFIG_CMD_SF)
#if 1
#ifdef MV_NOR_BOOT
static unsigned int flash_in_which_sec(flash_info_t *fl,unsigned int offset)
{
	unsigned int sec_num;
	if(NULL == fl)
		return 0xFFFFFFFF;

	for( sec_num = 0; sec_num < fl->sector_count ; sec_num++){
		/* If last sector*/
		if (sec_num == fl->sector_count -1)
		{
			if((offset >= fl->start[sec_num]) && 
			   (offset <= (fl->size + fl->start[0] - 1)) )
			{
				return sec_num;
			}

		}
		else
		{
			if((offset >= fl->start[sec_num]) && 
			   (offset < fl->start[sec_num + 1]) )
			{
				return sec_num;
			}

		}
	}
	/* return illegal sector Number */
	return 0xFFFFFFFF;

}

#endif /* defined(MV_NOR_BOOT) */
#endif

/*******************************************************************************
burn a u-boot.bin on the Boot Flash
********************************************************************************/
#include <net.h>
#include "bootstrap_def.h"
#if defined(CONFIG_CMD_NET)
/* 
 * 8 bit checksum 
 */
MV_U8 checksum8(MV_U32 start, MV_U32 len, MV_U8 csum)
{
	register MV_U8 sum = csum;
	volatile MV_U8* startp = (volatile MV_U8*)start;

	do {
		sum += *startp;
		startp++;
	} while(--len);

	return (sum);
} /* end of checksum8 */

#if 0
/*
 * Check the extended header and execute the image
 */
static MV_U32 verify_extheader(ExtBHR_t *extBHR)
{
	MV_U8	chksum;


	/* Caclulate abd check the checksum to valid */
	chksum = checksum8((MV_U32)extBHR , EXT_HEADER_SIZE -1, 0);
	if (chksum != (*(MV_U8*)((MV_U32)extBHR + EXT_HEADER_SIZE - 1)))
	{
		printf("Error! invalid extende header checksum\n");
		return MV_FAIL;
	}
	
    return MV_OK;
}
#endif
#ifndef CONFIG_APBOOT
/*
 * Check the CSUM8 on the main header
 */
static MV_U32 verify_main_header(BHR_t *pBHR, MV_U8 headerID)
{
	MV_U8	chksum,chksumtemp;

	/* Verify Checksum */
	chksumtemp = pBHR->checkSum;
	pBHR->checkSum = 0;
	chksum = checksum8((MV_U32)pBHR, MAIN_HDR_GET_LEN(pBHR), 0);
	pBHR->checkSum = chksumtemp;
	if (chksum != chksumtemp)
	{
		printf("\t[Fail] invalid image header checksum\n");
		return MV_FAIL;
	}

	/* Verify Header */
	if (pBHR->blockID != headerID)
	{
		printf("\t[Fail] invalid image header ID\n");
		return MV_FAIL;
	}
	
	/* Verify Alignment */
	if (pBHR->blockSize & 0x3)
	{
		printf("\t[Fail] invalid image header alignment\n");
		return MV_FAIL;
	}

	if ((cpu_to_le32(pBHR->destinationAddr) & 0x3) && (cpu_to_le32(pBHR->destinationAddr) != 0xffffffff))
	{
		printf("\t[Fail] invalid image header destination\n");
		return MV_FAIL;
	}

	if ((cpu_to_le32(pBHR->sourceAddr) & 0x3) && (pBHR->blockID != IBR_HDR_SATA_ID))
	{
		printf("\t[Fail] invalid image header source\n");
		return MV_FAIL;
	}

    return MV_OK;
}
#endif


#if defined(MV_NAND_BOOT)
/* Boot from NAND flash */
/* Write u-boot image into the nand flash */
int nand_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	nand_info_t *nand = &nand_info[0];
	uint64_t len = CONFIG_SYS_MONITOR_LEN;
	load_addr = CONFIG_SYS_LOAD_ADDR; 

	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else { 
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}
	filesize = NetLoop(TFTP);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t[Fail]\n");
		return 0;
	}
	printf("\t[Done]\n");
#ifdef MV_BOOTROM
	printf("Checking header cksum:");
	BHR_t* tmpBHR = (BHR_t*) load_addr;

	/* Verify Main header checksum */
	if (verify_main_header(tmpBHR, IBR_HDR_NAND_ID))
		return 0;

	printf("\t[Done]\n");
#endif

	printf("Override Env parameters to default? [y/N]");
	readline(" ");
	if( strcmp(console_buffer,"Y") == 0 || 
	    strcmp(console_buffer,"yes") == 0 ||
	    strcmp(console_buffer,"y") == 0 ) {

		printf("Erasing sector 0x%x:",CONFIG_ENV_OFFSET);
		nand_erase(nand, CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE);
		printf("\t[Done]\n");
	}

	printf("Erasing 0x%x - 0x%x: ",CONFIG_SYS_MONITOR_BASE, CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN);
	nand_erase(nand, CONFIG_SYS_MONITOR_BASE, CONFIG_SYS_MONITOR_LEN);
	printf("\t[Done]\n");

	printf("Writing image to NAND:");
	ret = nand_write(nand, CONFIG_SYS_MONITOR_BASE, &len, (u_char*)load_addr);
	if (ret)
		printf("\t[Fail]\n");
	else
		printf("\t[Done]\n");	

	return 1;
}

U_BOOT_CMD(
        bubt,      2,     1,      nand_burn_uboot_cmd,
        "bubt	- Burn an image on the Boot Nand Flash.\n",
        " file-name \n"
        "\tBurn a binary image on the Boot Nand Flash, default file-name is u-boot.bin .\n"
);

/* Write nboot loader image into the nand flash */
int nand_burn_nboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_U32 ret = 0;
	extern char console_buffer[];
	nand_info_t *nand = &nand_info[0];
	unsigned int filesize;
	size_t len = MV_NBOOT_LEN;

	load_addr = CONFIG_SYS_LOAD_ADDR; 
	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else { 
		copy_filename (BootFile, "nboot.bin", sizeof(BootFile));
		printf("using default file \"nboot.bin\" \n");
	}
 
	if ((filesize = NetLoop(TFTP)) < 0)
		return 0;
 
	printf("Erase %d - %d ... ",MV_NBOOT_BASE, MV_NBOOT_LEN);
	nand_erase(nand, MV_NBOOT_BASE, MV_NBOOT_LEN);

	printf("\nCopy to Nand Flash... ");
	ret = nand_write(nand, MV_NBOOT_BASE, (uint64_t*)&len, (u_char*)load_addr);
	if (ret)
		printf("Error - NAND burn faild!\n");
	else
		printf("\ndone\n");	

	return 1;
}

U_BOOT_CMD(
        nbubt,      2,     1,      nand_burn_nboot_cmd,
        "nbubt	- Burn a boot loader image on the Boot Nand Flash.\n",
        " file-name \n"
        "\tBurn a binary boot loader image on the Boot Nand Flash, default file-name is nboot.bin .\n"
);
#endif /* defined(CONFIG_NAND_BOOT) */

#if defined(MV_SPI_BOOT)
#ifndef CONFIG_APBOOT
/* Boot from SPI flash */
/* Write u-boot image into the SPI flash */
int spi_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	size_t len = CONFIG_SYS_MONITOR_LEN;
	load_addr = CONFIG_SYS_LOAD_ADDR; 

	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else { 
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}
	filesize = NetLoop(TFTP);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t[ERR!]\n");
		return 0;
	}
	printf("\t[Done]\n");
#ifdef MV_BOOTROM
	printf("Checking header cksum:");
	BHR_t* tmpBHR = (BHR_t*) load_addr;

	/* Verify Main header checksum */
	if (verify_main_header(tmpBHR, IBR_HDR_SPI_ID))
		return 0;

	printf("\t[Done]\n");
#endif

	printf("Override Env parameters to default? [y/N]");
	readline(" ");
	if( strcmp(console_buffer,"Y") == 0 || 
	    strcmp(console_buffer,"yes") == 0 ||
	    strcmp(console_buffer,"y") == 0 ) {

		printf("Erasing sector 0x%x:",CONFIG_ENV_OFFSET);
		spi_flash_erase(flash, CONFIG_ENV_OFFSET, CONFIG_ENV_SECT_SIZE);
		printf("\t[Done]\n");
	}

	printf("Erasing 0x%x - 0x%x: ",0, 0 + CONFIG_SYS_MONITOR_LEN);
	spi_flash_erase(flash, 0, CONFIG_SYS_MONITOR_LEN);
	printf("\t[Done]\n");

	printf("Writing image to SPI:");
	ret = spi_flash_write(flash, 0, len, (const void *)load_addr);

	if (ret)
		printf("\t[Err!]\n");
	else
		printf("\t[Done]\n");	

	return 1;
}

U_BOOT_CMD(
        bubt,      2,     1,      spi_burn_uboot_cmd,
        "bubt	- Burn an image on the Boot SPI Flash.\n",
        " file-name \n"
        "\tBurn a binary image on the Boot SPI Flash, default file-name is u-boot.bin .\n"
);
#endif
#endif

#if defined(MV_NOR_BOOT)

/* Boot from Nor flash */
int nor_burn_uboot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int filesize;
	MV_U32 ret = 0;
	extern char console_buffer[];
	size_t len = CONFIG_SYS_MONITOR_LEN;
	load_addr = CONFIG_SYS_LOAD_ADDR; 
	MV_U32 s_first,s_end,env_sec;

	ulong stop_addr;
	ulong start_addr;

	s_first = flash_in_which_sec(&flash_info[BOOT_FLASH_INDEX], CONFIG_SYS_MONITOR_BASE);
	s_end = flash_in_which_sec(&flash_info[BOOT_FLASH_INDEX], CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN -1);
	env_sec = flash_in_which_sec(&flash_info[BOOT_FLASH_INDEX], CONFIG_ENV_ADDR);

	if(argc == 2) {
		copy_filename (BootFile, argv[1], sizeof(BootFile));
	}
	else { 
		copy_filename (BootFile, "u-boot.bin", sizeof(BootFile));
		printf("Using default filename \"u-boot.bin\" \n");
	}

	filesize = NetLoop(TFTP);
	printf("Checking file size:");
	if (filesize == -1)
	{
		printf("\t[ERR!]\n");
		return 0;
	}
	printf("\t[Done]\n");
#ifdef MV_BOOTROM
	printf("Checking header cksum:");
	BHR_t* tmpBHR = (BHR_t*) load_addr;

	/* Verify Main header checksum */
	if (verify_main_header(tmpBHR, IBR_HDR_SPI_ID))
		return 0;

	printf("\t[Done]\n");
#endif

	printf("Override Env parameters to default? [y/N]");
	readline(" ");
	if( strcmp(console_buffer,"Y") == 0 || 
	    strcmp(console_buffer,"yes") == 0 ||
	    strcmp(console_buffer,"y") == 0 ) {

		start_addr = CONFIG_ENV_ADDR;
		stop_addr = start_addr + CONFIG_ENV_SIZE - 1;

		printf("Erasing sector 0x%x:",CONFIG_ENV_OFFSET);
		flash_sect_protect (0, start_addr, stop_addr);

		flash_sect_erase (start_addr, stop_addr);

		flash_sect_protect (1, start_addr, stop_addr);
		printf("\t[Done]\n");
	}

	start_addr = NOR_CS_BASE;
	stop_addr = start_addr + CONFIG_SYS_MONITOR_LEN - 1;

	flash_sect_protect (0, start_addr, stop_addr);

	printf("Erasing 0x%x - 0x%x: ", start_addr, start_addr + CONFIG_SYS_MONITOR_LEN);
	flash_sect_erase (start_addr, stop_addr);
	printf("\t[Done]\n");

	printf("Writing image to NOR:");
	ret = flash_write((char *)CONFIG_SYS_LOAD_ADDR, start_addr, filesize);

	if (ret)
		printf("\t[Err!]\n");
	else
		printf("\t[Done]\n");
	
	flash_sect_protect (1, start_addr, stop_addr);
#if 0
	flash_protect (FLAG_PROTECT_CLEAR,
			CONFIG_SYS_MONITOR_BASE,
			CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN - 1,
			&flash_info[BOOT_FLASH_INDEX]);

	printf("Erase %d - %d sectors... ",s_first,s_end);
	flash_erase(&flash_info[BOOT_FLASH_INDEX], s_first, s_end);

	printf("Copy to Flash... ");
	flash_write ( (char *)(CONFIG_SYS_LOAD_ADDR),
				  (ulong)CONFIG_SYS_FLASH_BASE,
				  (ulong)filesize);

	if (ret)
		printf("\t[Err!]\n");
	else
		printf("\t[Done]\n");	

	flash_protect (FLAG_PROTECT_SET,
			CONFIG_SYS_MONITOR_BASE,
			CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN - 1,
			&flash_info[BOOT_FLASH_INDEX]);
#endif
	return 1;
}

U_BOOT_CMD(
        bubt,      2,     1,      nor_burn_uboot_cmd,
        "bubt	- Burn an image on the Boot Flash.\n",
        " file-name \n"
        "\tBurn a binary image on the Boot Flash, default file-name is u-boot.bin .\n"
);
#endif /* MV_NOR_BOOT */

#endif /* (CONFIG_CMD_NET) */

#ifndef CONFIG_APBOOT
/*******************************************************************************
Reset environment variables.
********************************************************************************/
int resetenv_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if defined(CONFIG_ENV_IS_IN_NAND)
	nand_erase_options_t nand_erase_options;
	size_t offset = 0;

	
#if defined(CONFIG_SKIP_BAD_BLOCK)
	int i = 0;
	size_t blocksize;
	blocksize = nand_info[0].erasesize;
	while(i * blocksize < nand_info[0].size) {
		if (!nand_block_isbad(&nand_info[0], offset))
			offset += blocksize;
		if (offset >= CONFIG_ENV_OFFSET)
			break;
		i++;
	}
#else
	offset = CONFIG_ENV_OFFSET;
#endif
	nand_erase_options.length = CONFIG_ENV_RANGE;
	nand_erase_options.quiet = 0;
	nand_erase_options.jffs2 = 0;
	nand_erase_options.scrub = 0;
	nand_erase_options.offset = offset;

	puts ("Erasing Nand:\n");
	if (nand_erase_opts(&nand_info[0], &nand_erase_options))
		return 1;
	puts ("[Done]\n");
#elif defined(CONFIG_ENV_IS_IN_SPI_FLASH)
	u32 sector = 1;

	if (CONFIG_ENV_SIZE > CONFIG_ENV_SECT_SIZE) {
		sector = CONFIG_ENV_SIZE / CONFIG_ENV_SECT_SIZE;
		if (CONFIG_ENV_SIZE % CONFIG_ENV_SECT_SIZE)
			sector++;
	}

	puts("Erasing SPI flash:\n");
	if (spi_flash_erase(flash, CONFIG_ENV_OFFSET, sector * CONFIG_ENV_SECT_SIZE))
		return 1;
	puts("[Done]\n");
#elif defined(CONFIG_ENV_IS_IN_FLASH)
	ulong stop_addr;
	ulong start_addr;

	start_addr = CONFIG_ENV_ADDR;
	stop_addr = start_addr + CONFIG_ENV_SIZE - 1;

	printf("Erasing sector 0x%x:",CONFIG_ENV_OFFSET);
	flash_sect_protect (0, start_addr, stop_addr);

	flash_sect_erase (start_addr, stop_addr);

	flash_sect_protect (1, start_addr, stop_addr);
	printf("\t[Done]\n");

#endif
	printf("Warning: Default Environment Variables will take effect Only after RESET\n");
	return 0;
}

U_BOOT_CMD(
        resetenv,      1,     1,      resetenv_cmd,
        "resetenv	- erase environment sector to reset all variables to default.\n",
        " \n"
        "\t Erase the environemnt variable sector.\n"
);

#endif	/* CONFIG_APBOOT */
#endif
#if defined(CONFIG_CMD_BSP)

/******************************************************************************
* Category     - General
* Functionality- The commands allows the user to view the contents of the MV
*                internal registers and modify them.
* Need modifications (Yes/No) - no
*****************************************************************************/
int ir_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[] )
{
	MV_U32 regNum = 0x0, regVal, regValTmp, res;
	MV_8 regValBin[40];
	MV_8 cmd[40];
	int i,j = 0, flagm = 0;
	extern MV_8 console_buffer[];

	if( argc == 2 ) {
		regNum = simple_strtoul( argv[1], NULL, 16 );
	}
	else { 
		printf( "Usage:\n%s\n", cmdtp->usage );
		return 0;
	}                                                                                                        

	regVal = MV_REG_READ( regNum );
	regValTmp = regVal;
	printf( "Internal register 0x%x value : 0x%x\n ",regNum, regVal );
	printf( "\n    31      24        16         8         0" );
	printf( "\n     |       |         |         |         |\nOLD: " );

	for( i = 31 ; i >= 0 ; i-- ) {
		if( regValTmp > 0 ) {
			res = regValTmp % 2;
			regValTmp = (regValTmp - res) / 2;
			if( res == 0 )
				regValBin[i] = '0';
			else
				regValBin[i] = '1';
		}
		else
			regValBin[i] = '0';
	}

	for( i = 0 ; i < 32 ; i++ ) {
		printf( "%c", regValBin[i] );
		if( (((i+1) % 4) == 0) && (i > 1) && (i < 31) )
			printf( "-" );
	}

	readline( "\nNEW: " );
	strcpy(cmd, console_buffer);
	if( (cmd[0] == '0') && (cmd[1] == 'x') ) {
		regVal = simple_strtoul( cmd, NULL, 16 );
		flagm=1;
	}
	else {
		for( i = 0 ; i < 40 ; i++ ) {
			if(cmd[i] == '\0')
				break;
			if( i == 4 || i == 9 || i == 14 || i == 19 || i == 24 || i == 29 || i == 34 )
				continue;
			if( cmd[i] == '1' ) {
				regVal = regVal | (0x80000000 >> j);
				flagm = 1;
			}
			else if( cmd[i] == '0' ) {
				regVal = regVal & (~(0x80000000 >> j));
				flagm = 1;
			}
			j++;
		}
	}

	if( flagm == 1 ) {
		MV_REG_WRITE( regNum, regVal );
		printf( "\nNew value = 0x%x\n\n", MV_REG_READ(regNum) );
	}
	return 1;
}

U_BOOT_CMD(
	ir,      2,     1,      ir_cmd,
	"ir	- reading and changing MV internal register values.\n",
	" address\n"
	"\tDisplays the contents of the internal register in 2 forms, hex and binary.\n"
	"\tIt's possible to change the value by writing a hex value beginning with \n"
	"\t0x or by writing 0 or 1 in the required place. \n"
    	"\tPressing enter without any value keeps the value unchanged.\n"
);

/******************************************************************************
* Category     - General
* Functionality- Display the auto detect values of the TCLK and SYSCLK.
* Need modifications (Yes/No) - no
*****************************************************************************/
int clk_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    	printf( "TCLK %dMhz, SYSCLK %dMhz (UART baudrate %d)\n",
		mvTclkGet()/1000000, mvSysClkGet()/1000000, CONFIG_BAUDRATE);
	return 1;
}

U_BOOT_CMD(
	dclk,      1,     1,      clk_cmd,
	"dclk	- Display the MV device CLKs.\n",
	" \n"
	"\tDisplay the auto detect values of the TCLK and SYSCLK.\n"
);

/******************************************************************************
* Functional only when using Lauterbach to load image into DRAM
* Category     - DEBUG
* Functionality- Display the array of registers the u-boot write to.
*
*****************************************************************************/
#if defined(REG_DEBUG)
int reg_arry[REG_ARRAY_SIZE][2];
int reg_arry_index = 0;
int print_registers( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	printf("Register display\n");

	for (i=0; i < reg_arry_index; i++)
		printf("Index %d 0x%x=0x%08x\n", i, (reg_arry[i][0] & 0x000fffff), reg_arry[i][1]);

	/* Print DRAM registers */	
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1500, MV_REG_READ(0x1500));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1504, MV_REG_READ(0x1504));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1508, MV_REG_READ(0x1508));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x150c, MV_REG_READ(0x150c));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1510, MV_REG_READ(0x1510));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1514, MV_REG_READ(0x1514));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1518, MV_REG_READ(0x1518));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x151c, MV_REG_READ(0x151c));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1400, MV_REG_READ(0x1400));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1404, MV_REG_READ(0x1404));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1408, MV_REG_READ(0x1408));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x140c, MV_REG_READ(0x140c));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1410, MV_REG_READ(0x1410));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x141c, MV_REG_READ(0x141c));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1420, MV_REG_READ(0x1420));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1424, MV_REG_READ(0x1424));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1428, MV_REG_READ(0x1428));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x147c, MV_REG_READ(0x147c));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1494, MV_REG_READ(0x1494));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x1498, MV_REG_READ(0x1498));
	printf("Index %d 0x%x=0x%08x\n", i++, 0x149c, MV_REG_READ(0x149c));

	printf("Number of Reg %d \n", i);

	return 1;
}

U_BOOT_CMD(
	printreg,      1,     1,      print_registers,
	"printreg	- Display the register array the u-boot write to.\n",
	" \n"
	"\tDisplay the register array the u-boot write to.\n"
);
#endif

#if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH)
/******************************************************************************
* Category     - Etherent
* Functionality- Display PHY ports status (using SMI access).
* Need modifications (Yes/No) - No
*****************************************************************************/
int sg_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if defined(MV_INC_BOARD_QD_SWITCH)
		printf( "Switch status not supported\n");
#else
	MV_U32 port;
	for( port = 0 ; port < mvCtrlEthMaxPortGet(); port++ ) {

		printf( "PHY %d :\n", port );
		printf( "---------\n" );

		mvEthPhyPrintStatus( mvBoardPhyAddrGet(port) );

		printf("\n");
	}
#endif
	return 1;
}

U_BOOT_CMD(
	sg,      1,     1,      sg_cmd,
	"sg	- scanning the PHYs status\n",
	" \n"
	"\tScan all the Gig port PHYs and display their Duplex, Link, Speed and AN status.\n"
);
#endif /* #if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH) */

#if defined(MV_INCLUDE_IDMA)

/******************************************************************************
* Category     - DMA
* Functionality- Perform a DMA transaction
* Need modifications (Yes/No) - No
*****************************************************************************/
int mvDma_cmd( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[] )
{
	MV_8 cmd[20], c;
	extern MV_8 console_buffer[];
	MV_U32 chan, src, dst, byteCount, ctrlLo;
	MV_DMA_DEC_WIN win;
	MV_BOOL err;

	/* IDMA channel */
	if( argc == 2 ) 
		chan = simple_strtoul( argv[1], NULL, 16 );
	else
		chan = 0;

	/* source address */
	while(1) {
		readline( "Source Address: " );
		strcpy( cmd, console_buffer );
		src = simple_strtoul( cmd, NULL, 16 );
		if( src == 0xffffffff ) printf( "Bad address !!!\n" );
		else break;
	}

	/* desctination address */
	while(1) {
		readline( "Destination Address: " );
		strcpy(cmd, console_buffer);
		dst = simple_strtoul( cmd, NULL, 16 );
		if( dst == 0xffffffff ) printf("Bad address !!!\n");
		else break;
	}

	/* byte count */
	while(1) {
		readline( "Byte Count (up to 16M (0xffffff-1)): " );
		strcpy( cmd, console_buffer );
		byteCount = simple_strtoul( cmd, NULL, 16 );
		if( (byteCount > 0xffffff) || (byteCount == 0) ) printf("Bad value !!!\n");
		else break;
	}

	/* compose the command */
	ctrlLo = ICCLR_BLOCK_MODE | ICCLR_NON_CHAIN_MODE | ICCLR_SRC_INC | ICCLR_DST_INC;


	if (byteCount > _64K)
	{
		ctrlLo |= ICCLR_DESC_MODE_16M;
	}

	/* set data transfer limit */
	while(1) {
		printf( "Data transfer limit:\n" );
		printf( "(1) 8   bytes at a time.\n" );
		printf( "(2) 16  bytes at a time.\n" );
		printf( "(3) 32  bytes at a time.\n" );
		printf( "(4) 64  bytes at a time.\n" );
		printf( "(5) 128 bytes at a time.\n" );

		c = getc(); 
		printf( "%c\n", c );

		err = MV_FALSE;

		switch( c ) {
			case 13: /* Enter */
				ctrlLo |= (ICCLR_DST_BURST_LIM_32BYTE | ICCLR_SRC_BURST_LIM_32BYTE);
				printf( "32 bytes at a time.\n" );
				break;
			case '1':
				ctrlLo |= (ICCLR_DST_BURST_LIM_8BYTE | ICCLR_SRC_BURST_LIM_8BYTE);
				break;
			case '2':
				ctrlLo |= (ICCLR_DST_BURST_LIM_16BYTE | ICCLR_SRC_BURST_LIM_16BYTE);
				break;
			case '3':
				ctrlLo |= (ICCLR_DST_BURST_LIM_32BYTE | ICCLR_SRC_BURST_LIM_32BYTE);
				break;
			case '4':
				ctrlLo |= (ICCLR_DST_BURST_LIM_64BYTE | ICCLR_SRC_BURST_LIM_64BYTE);
				break;
			case '5':
				ctrlLo |= (ICCLR_DST_BURST_LIM_128BYTE | ICCLR_SRC_BURST_LIM_128BYTE);
				break;
			default:
				printf( "Bad value !!!\n" );
				err = MV_TRUE;
		}

		if( !err ) break;
	}

	/* set ovveride source option */
	while(1) {
		printf( "Override Source:\n" ); 
		printf( "(0) - no override\n" );
		mvDmaWinGet( 1, &win );
		printf( "(1) - use Win1 (%s)\n",mvCtrlTargetNameGet(win.target));
		mvDmaWinGet( 2, &win );
		printf( "(2) - use Win2 (%s)\n",mvCtrlTargetNameGet(win.target));
		mvDmaWinGet( 3, &win );
		printf( "(3) - use Win3 (%s)\n",mvCtrlTargetNameGet(win.target));

		c = getc(); 
		printf( "%c\n", c );

		err = MV_FALSE;

		switch( c ) {
			case 13: /* Enter */
			case '0':
				printf( "No override\n" );
				break;
			case '1':
				ctrlLo |= ICCLR_OVRRD_SRC_BAR(1);
				break;
			case '2':
				ctrlLo |= ICCLR_OVRRD_SRC_BAR(2);
				break;
			case '3':
				ctrlLo |= ICCLR_OVRRD_SRC_BAR(3);
				break;
			default:
				printf("Bad value !!!\n");
				err = MV_TRUE;
		}

		if( !err ) break;
	}

	/* set override destination option */
	while(1) {
		printf( "Override Destination:\n" ); 
		printf( "(0) - no override\n" );
		mvDmaWinGet( 1, &win );
		printf( "(1) - use Win1 (%s)\n",mvCtrlTargetNameGet(win.target));
		mvDmaWinGet( 2, &win );
		printf( "(2) - use Win2 (%s)\n",mvCtrlTargetNameGet(win.target));
		mvDmaWinGet( 3, &win );
		printf( "(3) - use Win3 (%s)\n",mvCtrlTargetNameGet(win.target));

		c = getc(); 
		printf( "%c\n", c );

		err = MV_FALSE;

	        switch( c ) {
			case 13: /* Enter */
			case '0':
				printf( "No override\n" );
				break;
			case '1':
				ctrlLo |= ICCLR_OVRRD_DST_BAR(1);
				break;
			case '2':
				ctrlLo |= ICCLR_OVRRD_DST_BAR(2);
				break;
			case '3':
				ctrlLo |= ICCLR_OVRRD_DST_BAR(3);
				break;
			default:
				printf("Bad value !!!\n");
				err = MV_TRUE;
		}

		if( !err ) break;
	}

	/* wait for previous transfer completion */
	while( mvDmaStateGet(chan) != MV_IDLE );

	/* issue the transfer */
	mvDmaCtrlLowSet( chan, ctrlLo ); 
	mvDmaTransfer( chan, src, dst, byteCount, 0 );

	/* wait for completion */
	while( mvDmaStateGet(chan) != MV_IDLE );

	printf( "Done...\n" );
	return 1;
}

U_BOOT_CMD(
	dma,      2,     1,      mvDma_cmd,
	"dma	- Perform DMA\n",
	" \n"
	"\tPerform DMA transaction with the parameters given by the user.\n"
);

#endif /* #if defined(MV_INCLUDE_IDMA) */

/******************************************************************************
* Category     - Memory
* Functionality- Displays the MV's Memory map
* Need modifications (Yes/No) - Yes
*****************************************************************************/
int displayMemoryMap_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	mvCtrlAddrDecShow();
	return 1;
}

U_BOOT_CMD(
	map,      1,     1,      displayMemoryMap_cmd,
	"map	- Display address decode windows\n",
	" \n"
	"\tDisplay controller address decode windows: CPU, PCI, Gig, DMA, XOR and COMM\n"
);



#include "ddr2/spd/mvSpd.h"
#if defined(MV_INC_BOARD_DDIM)

/******************************************************************************
* Category     - Memory
* Functionality- Displays the SPD information for a givven dimm
* Need modifications (Yes/No) - 
*****************************************************************************/
              
int dimminfo_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        int num = 0;
 
        if (argc > 1) {
                num = simple_strtoul (argv[1], NULL, 10);
        }
 
        printf("*********************** DIMM%d *****************************\n",num);
 
        dimmSpdPrint(num);
 
        printf("************************************************************\n");
         
        return 1;
}
 
U_BOOT_CMD(
        ddimm,      2,     1,      dimminfo_cmd,
        "ddimm  - Display SPD Dimm Info\n",
        " [0/1]\n"
        "\tDisplay Dimm 0/1 SPD information.\n"
);

/******************************************************************************
* Category     - Memory
* Functionality- Copy the SPD information of dimm 0 to dimm 1
* Need modifications (Yes/No) - 
*****************************************************************************/
              
int spdcpy_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
 
        printf("Copy DIMM 0 SPD data into DIMM 1 SPD...");
 
        if (MV_OK != dimmSpdCpy())
        	printf("\nDIMM SPD copy fail!\n");
 	else
        	printf("Done\n");
         
        return 1;
}
 
U_BOOT_CMD(
        spdcpy,      2,     1,      spdcpy_cmd,
        "spdcpy  - Copy Dimm 0 SPD to Dimm 1 SPD \n",
        ""
        ""
);
#endif /* #if defined(MV_INC_BOARD_DDIM) */

/******************************************************************************
* Functionality- Go to an address and execute the code there and return,
*    defualt address is 0x40004
*****************************************************************************/
extern void cpu_dcache_flush_all(void);
extern void cpu_icache_flush_invalidate_all(void);

void mv_go(unsigned long addr,int argc, char *argv[])
{
	int rc;
	addr = MV_CACHEABLE(addr);
	char* envCacheMode = getenv("cacheMode");
 
	/*
	 * pass address parameter as argv[0] (aka command name),
	 * and all remaining args
	 */

    if(envCacheMode && (strcmp(envCacheMode,"write-through") == 0))
	{	
		int i=0;

		/* Flush Invalidate I-Cache */
		cpu_icache_flush_invalidate_all();

		/* Drain write buffer */
		asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i));
		

	}
	else /*"write-back"*/
	{
		int i=0;

		/* Flush Invalidate I-Cache */
		cpu_icache_flush_invalidate_all();

		/* Drain write buffer */
		asm ("mcr p15, 0, %0, c7, c10, 4": :"r" (i));
		

		/* Flush invalidate D-cache */
		cpu_dcache_flush_all();


    }


	rc = ((ulong (*)(int, char *[]))addr) (--argc, &argv[1]);
 
        return;
}

int g_cmd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        ulong   addr;

	if(!enaMonExt()){
		printf("This command can be used only if enaMonExt is set!\n");
		return 0;
	}

	addr = 0x40000;

        if (argc > 1) {
		addr = simple_strtoul(argv[1], NULL, 16);
        }
	mv_go(addr,argc,&argv[0]);
	return 1;
}                                                                                                                     

U_BOOT_CMD(
	g,      CONFIG_SYS_MAXARGS,     1,      g_cmd,
        "g	- start application at cached address 'addr'(default addr 0x40000)\n",
        " addr [arg ...] \n"
	"\tStart application at address 'addr'cachable!!!(default addr 0x40004/0x240004)\n"
	"\tpassing 'arg' as arguments\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

/******************************************************************************
* Functionality- Searches for a value
*****************************************************************************/
int fi_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    MV_U32 s_address,e_address,value,i,tempData;
    MV_BOOL  error = MV_FALSE;

    if(!enaMonExt()){
	printf("This command can be used only if enaMonExt is set!\n");
	return 0;}

    if(argc == 4){
	value = simple_strtoul(argv[1], NULL, 16);
	s_address = simple_strtoul(argv[2], NULL, 16);
	e_address = simple_strtoul(argv[3], NULL, 16);
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
    	return 0;
    }     

    if(s_address == 0xffffffff || e_address == 0xffffffff) error = MV_TRUE;
    if(s_address%4 != 0 || e_address%4 != 0) error = MV_TRUE;
    if(s_address > e_address) error = MV_TRUE;
    if(error)
    {
	printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    for(i = s_address; i < e_address ; i+=4)
    {
        tempData = (*((volatile unsigned int *)i));
        if(tempData == value)
        {
            printf("Value: %x found at ",value);
            printf("address: %x\n",i);
            return 1;
        }
    }
    printf("Value not found!!\n");
    return 1;
}

U_BOOT_CMD(
	fi,      4,     1,      fi_cmd,
	"fi	- Find value in the memory.\n",
	" value start_address end_address\n"
	"\tSearch for a value 'value' in the memory from address 'start_address to\n"
	"\taddress 'end_address'.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

/******************************************************************************
* Functionality- Compare the memory with Value.
*****************************************************************************/
int cmpm_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    MV_U32 s_address,e_address,value,i,tempData;
    MV_BOOL  error = MV_FALSE;

    if(!enaMonExt()){
	printf("This command can be used only if enaMonExt is set!\n");
	return 0;}

    if(argc == 4){
	value = simple_strtoul(argv[1], NULL, 16);
	s_address = simple_strtoul(argv[2], NULL, 16);
	e_address = simple_strtoul(argv[3], NULL, 16);
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
    	return 0;
    }     

    if(s_address == 0xffffffff || e_address == 0xffffffff) error = MV_TRUE;
    if(s_address%4 != 0 || e_address%4 != 0) error = MV_TRUE;
    if(s_address > e_address) error = MV_TRUE;
    if(error)
    {
	printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    for(i = s_address; i < e_address ; i+=4)
    {
        tempData = (*((volatile unsigned int *)i));
        if(tempData != value)
        {
            printf("Value: %x found at address: %x\n",tempData,i);
        }
    }
    return 1;
}

U_BOOT_CMD(
	cmpm,      4,     1,      cmpm_cmd,
	"cmpm	- Compare Memory\n",
	" value start_address end_address.\n"
	"\tCompare the memory from address 'start_address to address 'end_address'.\n"
	"\twith value 'value'\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);



#if 1
/******************************************************************************
* Category     - Etherent
* Functionality- Display PHY ports status (using SMI access).
* Need modifications (Yes/No) - No
*****************************************************************************/
extern void ethRegs(int port);
extern void ethPortRegs(int port);
extern void ethPortStatus(int port);
extern void ethPortQueues(int port, int rxQueue, int txQueue, int mode);
int eth_show_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        int port = simple_strtoul(argv[1], (char **)0, 10);

	printf("Regs:\n");
	ethRegs(port);
	printf("Port Regs:\n");
	ethPortRegs(port);
	printf("Status:\n");
	ethPortStatus(port);
	printf("Port Queues:\n");
	ethPortQueues(port,0,0,1);

	ethPortCounters(port);
	ethPortRmonCounters(port);
	ethPortUcastShow(port);
	ethPortMcast(port);

	return 1;
}

U_BOOT_CMD(
	ethShow,      2,    2,      eth_show_cmd,
	"ethShow	- scanning the PHYs status\n",
	" \n"
	"\tScan all the Gig port PHYs and display their Duplex, Link, Speed and AN status.\n"
);

/******************************************************************************
* Category     - Etherent
* Functionality- Display ports counters
* Need modifications (Yes/No) - No
*****************************************************************************/
int eth_count_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int	port_num = eth_get_dev_index();

#ifdef CONFIG_APBOOT
	if (strncmp(eth_get_name(), "eth", 3) != 0)
		return 0;
#else
	if (strncmp(eth_get_name(), "egiga", 5) != 0)
		return 0;
#endif

	ethPortCounters(port_num);
	return 1;
}

U_BOOT_CMD(
	ethCounters,      2,    2,      eth_count_cmd,
	"ethShow	- show the current ETH port counters\n",
	" \n"
	"\tShow the current Gig port (egigaX) MIB counters.\n"
	);

#if defined (MV_RT_DEBUG)
/******************************************************************************
* Category     - Etherent
* Functionality- Init current ETH adapter
* Need modifications (Yes/No) - No
*****************************************************************************/
int eth_init_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	struct eth_device *dev = eth_get_dev();

#ifdef CONFIG_APBOOT
	if (strncmp(eth_get_name(), "eth", 3) != 0)
		return 0;
#else
	if (strncmp(eth_get_name(), "egiga", 5) != 0)
		return 0;
#endif

	dev->init(dev, 0);
	return 1;
}

U_BOOT_CMD(
	ethInit,      2,    2,      eth_init_cmd,
	"ethInit	- init current ETH port\n",
	" \n"
	"\tInitialize current Gig port (egigaX).\n"
	  );

/******************************************************************************
* Category     - Etherent
* Functionality- Send dummy packet from ETH adapter
* Need modifications (Yes/No) - No
*****************************************************************************/
int eth_send_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	struct eth_device *dev = eth_get_dev();
	char	buf[100] = {
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x50,
			0x43, 0xC0, 0xFF, 0xEE, 0x08, 0x06, 0x00, 0x01,
			0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x50,
			0x43, 0xC0, 0xFF, 0xEE, 0x32, 0x01, 0x01, 0x0A,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x01,
			0x01, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
			};

	if (strncmp(eth_get_name(), "egiga", 5) != 0)
		return 0;

	dev->send(dev, buf, sizeof(buf));
	return 1;
}

U_BOOT_CMD(
	ethSend,      2,    2,      eth_send_cmd,
	"ethSend	- send dummy frame from current ETH port\n",
	" \n"
	"Send dummy frame from current Gig port (egigaX).\n"
	  );
#endif /* MV_RT_DEBUG */
#endif

#if defined(MV_INCLUDE_PEX)

//#include "pci/mvPci.h"

int pcie_phy_read_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    	MV_U16 phyReg;

    	mvPexPhyRegRead(simple_strtoul( argv[1], NULL, 16 ),
	                simple_strtoul( argv[2], NULL, 16), &phyReg);

	printf ("0x%x\n", phyReg);

	return 1;
}

U_BOOT_CMD(
	pciePhyRead,      3,     3,      pcie_phy_read_cmd,
	"phyRead	- Read PCI-E Phy register\n",
	" PCI-E_interface Phy_offset. \n"
	"\tRead the PCI-E Phy register. \n"
);


int pcie_phy_write_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	mvPexPhyRegWrite(simple_strtoul( argv[1], NULL, 16 ),
					 simple_strtoul( argv[2], NULL, 16 ),
					 simple_strtoul( argv[3], NULL, 16 ));

	return 1;
}

U_BOOT_CMD(
	pciePhyWrite,      4,     4,      pcie_phy_write_cmd,
	"pciePhyWrite	- Write PCI-E Phy register\n",
	" PCI-E_interface Phy_offset value.\n"
	"\tWrite to the PCI-E Phy register.\n"
);

#endif /* #if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH) */
#if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH)

#include "eth-phy/mvEthPhy.h"

int phy_read_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    	MV_U16 phyReg;

    	mvEthPhyRegRead(simple_strtoul( argv[1], NULL, 16 ),
	                simple_strtoul( argv[2], NULL, 16), &phyReg);

	printf ("0x%x\n", phyReg);

	return 1;
}

U_BOOT_CMD(
	phyRead,      3,     3,      phy_read_cmd,
	"phyRead	- Read Phy register\n",
	" Phy_address Phy_offset. \n"
	"\tRead the Phy register. \n"
);


int phy_write_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	mvEthPhyRegWrite(simple_strtoul( argv[1], NULL, 16 ),
					 simple_strtoul( argv[2], NULL, 16 ),
					 simple_strtoul( argv[3], NULL, 16 ));

	return 1;
}

U_BOOT_CMD(
	phyWrite,      4,     4,      phy_write_cmd,
	"phyWrite	- Write Phy register\n",
	" Phy_address Phy_offset value.\n"
	"\tWrite to the Phy register.\n"
);


int switch_read_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    	MV_U16 phyReg;

    	mvEthSwitchRegRead(simple_strtoul( argv[1], NULL, 16 ),
	                simple_strtoul( argv[2], NULL, 16), simple_strtoul( argv[3], NULL, 16 ), &phyReg);

	printf ("0x%x\n", phyReg);

	return 1;
}

U_BOOT_CMD(
	switchRegRead,      4,     4,      switch_read_cmd,
	"switchRegRead	- Read switch register\n",
	" Port_number Phy_address Phy_offset. \n"
	"\tRead the switch register. \n"
);


int switch_write_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	mvEthSwitchRegWrite(simple_strtoul( argv[1], NULL, 16 ),
                        simple_strtoul( argv[2], NULL, 16 ),
                        simple_strtoul( argv[3], NULL, 16 ),
                        simple_strtoul( argv[4], NULL, 16 ));

	return 1;
}

U_BOOT_CMD(
	switchRegWrite,      5,     5,      switch_write_cmd,
	"switchRegWrite	- Write switch register\n",
	" Port_number Phy_address Phy_offset value.\n"
	"\tWrite to the switch register.\n"
);

int switch_phy_read_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_U16 phyReg;

	mvEthSwitchPhyRegRead(simple_strtoul( argv[1], NULL, 16 ),
                          simple_strtoul( argv[2], NULL, 16 ),
                          simple_strtoul( argv[3], NULL, 16 ),
                          &phyReg);

	printf ("0x%x\n", phyReg);
	return 1;
}

U_BOOT_CMD(
	switchPhyRegRead,      4,     4,      switch_phy_read_cmd,
	"switchPhyRegRead - Read switch register\n",
	" SW_on_port Port_number Phy_offset. \n"
	"\tRead the switch register. \n"
);


int switch_phy_write_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	mvEthSwitchPhyRegWrite(simple_strtoul( argv[1], NULL, 16 ),
                        simple_strtoul( argv[2], NULL, 16 ),
                        simple_strtoul( argv[3], NULL, 16 ),
                        simple_strtoul( argv[4], NULL, 16 ));

	return 1;
}

U_BOOT_CMD(
	switchPhyRegWrite,      5,     4,      switch_phy_write_cmd,
	"switchPhyRegWrite - Write switch register\n",
	" SW_on_port Port_number Phy_offset value.\n"
	"\tWrite to the switch register.\n"
);

#if 0
int switch_cntread_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    	MV_U16 data;
	MV_U32 port;
	MV_U32 i;

	port = simple_strtoul(argv[1], NULL, 16);
	printf("Switch on port = %d.\n", port);
	for(i = 0; i < 7; i++) {
		/* Set egress counter */
		mvEthSwitchRegWrite(port, 0x1B, 0x1D, 0xC400 | ((i + 1) << 5) | 0xE);
		do {
		    	mvEthSwitchRegRead(port, 0x1B, 0x1D, &data);
		} while(data & 0x8000);
		/* Read egress counter */
	    	mvEthSwitchRegRead(port, 0x1B, 0x1F, &data);
		printf("Port %d: Egress 0x%x, Ingress ", i, data);
		/* Set ingress counter */
		mvEthSwitchRegWrite(port, 0x1B, 0x1D, 0xC400 | ((i + 1) << 5) | 0x0);
		do {
		    	mvEthSwitchRegRead(port, 0x1B, 0x1D, &data);
		} while(data & 0x8000);
		/* Read egress counter */
	    	mvEthSwitchRegRead(port, 0x1B, 0x1F, &data);
		printf("0x%x.\n", data);
	}

	/* Clear all counters */
	mvEthSwitchRegWrite(port, 0x1B, 0x1D, 0x94C0);
	do {
	    	mvEthSwitchRegRead(port, 0x1B, 0x1D, &data);
	} while(data & 0x8000);

	return 1;
}

U_BOOT_CMD(
	switchCountersRead,      2,     2,      switch_cntread_cmd,
	"switchCountersRead Read switch port counters.\n",
	" MAC_Port. \n"
	"\tRead the switch ports counters. \n"
);

#endif /* #if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH) */
#endif /* 0 */

#endif

#if defined(CONFIG_CMD_DIAG)

#include "../diag/diag.h"
extern diag_func_t *diag_sequence[];

int mv_diag (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
        int test_no = 0, no_of_tests = 0; 
        diag_func_t **diag_func_ptr;


        for (diag_func_ptr = diag_sequence; *diag_func_ptr; ++diag_func_ptr)
                no_of_tests++;

        if (argc > 1) 
        {
                test_no = simple_strtoul(argv[1], NULL, 10); 
                if (test_no > no_of_tests)
                {
                        printf("There are only %d tests\n", no_of_tests);
                        printf("Usage: %s\n", cmdtp->help);
                        return 0;
                }

                test_no--;
                (*diag_sequence[test_no])();
                return 0;
        }

        for (diag_func_ptr = diag_sequence; *diag_func_ptr; ++diag_func_ptr)
        {
                printf("\n");
                if((*diag_func_ptr)())
                        break;
        }

        if(*diag_func_ptr == NULL)
                printf("\nDiag completed\n");
        else
                printf("\nDiag FAILED\n");

        return 0;
}

U_BOOT_CMD(
        mv_diag, 2, 0, mv_diag,
        "mv_diag - perform board diagnostics\n"
        "mv_diag - run all available tests\n"
        "mv_diag [1|2|...]\n"
        "        - run specified test number\n",
        "mv_diag - perform board diagnostics\n"
        "mv_diag - run all available tests\n"
        "mv_diag [1|2|...]\n"
        "        - run specified test number\n"
);
#endif /*CFG_DIAG*/


static int do_mux(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	const char *cmd;
	int env;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strcmp(cmd, "help") == 0)
			goto usage;
	if (strcmp(cmd, "set") == 0)
	{
		env = simple_strtoul(argv[2],NULL, 10);
		printf("Setting mux to %d\n",env);
		mvBoardTwsiMuxChannelSet(env);
		return 0;
	}
	else
		goto usage;
usage:
	cmd_usage(cmdtp);
	return 1;
}
U_BOOT_CMD(
	mux,	3,	1,	do_mux,
	"mux set",
	"help                - prints this help\n"
	"mux set 1/0         - prints the current configuration values\n"
);
static int do_mpp(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	const char *cmd;
	int i,j;
	MV_U32 regNum = 0xf1018000, regVal, regValTemp;
	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strcmp(cmd, "help") == 0)
			goto usage;
	if (strcmp(cmd, "print") == 0)
	{
		for (i=0;i<=0x30;i+=0x4)
		{
			regVal = MV_REG_READ(regNum + i);
			for (j=0;j<8;j++)
			{
				regValTemp = regVal & 0xf;
				printf("0x%x\n", regValTemp);
				regVal = regVal >> 4;
			}
		}
		return 0;
	}
	else
		goto usage;
usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(
	mpp,	3,	1,	do_mpp,
	"mpp",
	"help                - prints this help\n"
	"mpp print           - prints the current configuration values\n"
);

typedef struct m65xx_stat_t {
    int ms_64;
    int ms_stat1;
    int ms_stat2;
    int ms_egress;
    char *ms_name;
} m65xx_stat_t;

static m65xx_stat_t m65xx_stats[] = {
    {1,  0x1, 0x0, 0, "good octets received" },
    {0,  0x2, 0x0, 0, "bad octets received" },
    {0,  0x4, 0x0, 0, "unicast frames received" },
    {0,  0x6, 0x0, 0, "broadcast frames received" },
    {0,  0x7, 0x0, 0, "multicast frames received" },
    {0, 0x16, 0x0, 0, "pause frames received" },
    {0, 0x18, 0x0, 0, "undersize frames received" },
    {0, 0x19, 0x0, 0, "fragments received" },
    {0, 0x1a, 0x0, 0, "oversize frames received" },
    {0, 0x1b, 0x0, 0, "jabber frames received" },
    {0, 0x1c, 0x0, 0, "receive errors" },
    {0, 0x1d, 0x0, 0, "received with FCS error" },
#if 0
    {0,  0x8, 0x0, 0, "64 octets" },
    {0,  0x9, 0x0, 0, "65-127 octets" },
    {0,  0xa, 0x0, 0, "128-255 octets" },
    {0,  0xb, 0x0, 0, "256-511 octets" },
    {0,  0xc, 0x0, 0, "512-1023 octets" },
    {0,  0xd, 0x0, 0, "1024-max octets" },
#endif
    {1,  0xf, 0xe, 0, "octets transmitted" },
    {0, 0x10, 0x0, 0, "unicast frames transmitted" },
    {0, 0x13, 0x0, 0, "broadcast frames transmitted" },
    {0, 0x12, 0x0, 0, "multicast frames transmitted" },
    {0, 0x15, 0x0, 0, "pause frames transmitted" },
    {0, 0x14, 0x0, 0, "single collisions" },
    {0, 0x17, 0x0, 0, "multiple collisions" },
    {0, 0x11, 0x0, 0, "excessive collisions" },
    {0,  0x5, 0x0, 0, "frames deferred" },
    {0, 0x1e, 0x0, 0, "collisions" },
    {0, 0x1f, 0x0, 0, "late collisions" },
    {0, 0x03, 0x0, 0, "transmit FCS errors" },
#if 0
    {0,  0x8, 0x0, 1, "64 octets" },
    {0,  0x9, 0x0, 1, "65-127 octets" },
    {0,  0xa, 0x0, 1, "128-255 octets" },
    {0,  0xb, 0x0, 1, "256-511 octets" },
    {0,  0xc, 0x0, 1, "512-1023 octets" },
    {0,  0xd, 0x0, 1, "1024-max octets" },
#endif
};
#define M65xx_NSTAT (sizeof(m65xx_stats) / sizeof(m65xx_stats[0]))

static void 
aruba_dump_switch_counters_for_port(int port)
{
    	MV_U16 data;
	MV_U32 i, j;
	unsigned long long val;

	for (i = 0; i < 7; i++) {
		if (i > 1 && i != 4 && i != 5) {
		    continue;
		}
		printf("port %u\n", i);
		for (j = 0; j < M65xx_NSTAT; j++) {
			/* Set counter */
			/* XXX; 64-bit */
			int mode = 0xc400;
			if (m65xx_stats[j].ms_egress) {
				mode = 0xc800;
			}
			mvEthSwitchRegWrite(port, 0x1B, 0x1D, mode | ((i + 1) << 5) | m65xx_stats[j].ms_stat1);
			do {
				mvEthSwitchRegRead(port, 0x1B, 0x1D, &data);
			} while(data & 0x8000);
			/* Read data */
			mvEthSwitchRegRead(port, 0x1B, 0x1F, &data);
			val = data;
			mvEthSwitchRegRead(port, 0x1B, 0x1E, &data);
			val |= (data << 16);

			if (m65xx_stats[j].ms_64) {
				val <<= 32;
				mvEthSwitchRegWrite(port, 0x1B, 0x1D, mode | ((i + 1) << 5) | m65xx_stats[j].ms_stat2);
				do {
					mvEthSwitchRegRead(port, 0x1B, 0x1D, &data);
				} while(data & 0x8000);
				/* Read data */
				mvEthSwitchRegRead(port, 0x1B, 0x1F, &data);
				val |= data;
				mvEthSwitchRegRead(port, 0x1B, 0x1E, &data);
				val |= (data << 16);
			}
			printf("\t%32s: %Lu\n", m65xx_stats[j].ms_name, val);
		}
	}

	/* Clear all counters */
	mvEthSwitchRegWrite(port, 0x1B, 0x1D, 0x94C0);
	do {
	    	mvEthSwitchRegRead(port, 0x1B, 0x1D, &data);
	} while(data & 0x8000);
}

static int aruba_dump_switch_counters(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	aruba_dump_switch_counters_for_port(0);
	return 0;
}

U_BOOT_CMD(
	switchCountersRead,      1,     2,      aruba_dump_switch_counters,
	"switchCountersRead Read switch port counters.\n",
	" MAC_Port. \n"
	"\tRead the switch ports counters. \n"
);
