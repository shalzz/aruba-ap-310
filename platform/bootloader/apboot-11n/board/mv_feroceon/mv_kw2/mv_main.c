/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
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
#include "mvTypes.h"
#include "mvBoardEnvLib.h"
#include "mvCpuIf.h"
#include "mvCtrlEnvLib.h"
#include "mv_mon_init.h"
#include "mvDebug.h"
#include "device/mvDevice.h"
#include "twsi/mvTwsi.h"
#include "eth/mvEth.h"
#include "pex/mvPex.h"
#include "gpp/mvGpp.h"
#include "gpp/mvGppRegs.h"
#include "mvSysHwConfig.h"
#include "mv_phy.h"
#include "ctrlEnv/mvCtrlEthCompLib.h"

#ifdef MV_INCLUDE_RTC
#include "rtc/integ_rtc/mvRtc.h"
#include "rtc.h"
#elif CONFIG_RTC_DS1338_DS1339
#include "rtc/ext_rtc/mvDS133x.h"
#endif

#if defined(MV_INCLUDE_XOR)
#include "xor/mvXor.h"
#include "mvSysXorApi.h"
#endif
#if defined(MV_INCLUDE_IDMA)
#include "sys/mvSysIdma.h"
#include "idma/mvIdma.h"
#endif
#if defined(MV_INCLUDE_USB) || defined(ARUBA_ARM)
#include "usb/mvUsb.h"
#include "mv_hal_if/mvSysUsbApi.h"
#endif

#include "cpu/mvCpu.h"
#include "nand.h"
#include "spi_flash.h"
#ifdef CONFIG_PCI
	#include <pci.h>
#endif
//#include "pci/mvPciRegs.h"

#include <asm/arch-arm926ejs/vfpinstr.h>
#include <asm/arch-arm926ejs/vfp.h>
//#include <asm/arch/vfpinstr.h>
//#include <asm/arch/vfp.h>

#include <net.h>
//#include <netdev.h>
#include <command.h>

/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/* CPU address decode table. */
MV_CPU_DEC_WIN mvCpuAddrWinMap[] = MV_CPU_IF_ADDR_WIN_MAP_TBL;
#if 0
static void mvHddPowerCtrl(void);
#endif
#if defined(CONFIG_CMD_RCVR)
extern void recoveryDetection(void);
#endif
void mv_cpu_init(void);
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
int mv_set_power_scheme(void);
#endif

#ifdef	CONFIG_FLASH_CFI_DRIVER
MV_VOID mvUpdateNorFlashBaseAddrBank(MV_VOID);
int mv_board_num_flash_banks;
extern flash_info_t	flash_info[]; /* info for FLASH chips */
extern unsigned long flash_add_base_addr (uint flash_index, ulong flash_base_addr);
#endif	/* CONFIG_FLASH_CFI_DRIVER */

#if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH)
#include "mv_egiga.h"
//extern MV_VOID mvBoardEgigaPhySwitchInit(void);
#endif 

#if defined(CONFIG_CMD_NAND) || defined(ARUBA_ARM)
/* Define for SDK 2.0 */
int __aeabi_unwind_cpp_pr0(int a,int b,int c) {return 0;}
int __aeabi_unwind_cpp_pr1(int a,int b,int c) {return 0;}
#endif

extern nand_info_t nand_info[];       /* info for NAND chips */

extern struct spi_flash *flash;
//MV_VOID mvMppModuleTypePrint(MV_VOID);

#ifdef MV_NAND_BOOT
extern MV_U32 nandEnvBase;
#endif

/* Define for SDK 2.0 */
//int raise(void) {return 0;}

void print_mvBanner(void)
{
#ifdef CONFIG_SILENT_CONSOLE
	DECLARE_GLOBAL_DATA_PTR;
	gd->flags |= GD_FLG_SILENT;
#endif
#ifndef ARUBA_ARM
	printf("\n");
	printf(" __   __                      _ _\n");
	printf("|  \\/  | __ _ _ ____   _____| | |\n");
	printf("| |\\/| |/ _` | '__\\ \\ / / _ \\ | |\n");
	printf("| |  | | (_| | |   \\ V /  __/ | |\n");
	printf("|_|  |_|\\__,_|_|    \\_/ \\___|_|_|\n");
	printf("         _   _     ____              _\n");
	printf("        | | | |   | __ )  ___   ___ | |_ \n");
	printf("        | | | |___|  _ \\ / _ \\ / _ \\| __| \n");
	printf("        | |_| |___| |_) | (_) | (_) | |_ \n");
	printf("         \\___/    |____/ \\___/ \\___/ \\__| \n");
//#if !defined(MV_NAND_BOOT)
#if defined(MV_INCLUDE_MONT_EXT)
    //mvMPPConfigToSPI();
	if(!enaMonExt())
		printf(" ** LOADER **\n"); 
	else
		printf(" ** MONITOR **\n");
    //mvMPPConfigToDefault();
#else

	printf(" ** LOADER **\n"); 
#endif /* MV_INCLUDE_MONT_EXT */
//#endif
#endif
	return;
}

void maskAllInt(void)
{
        /* mask all external interrupt sources */
        MV_REG_WRITE(CPU_MAIN_IRQ_MASK_REG, 0);
        MV_REG_WRITE(CPU_MAIN_FIQ_MASK_REG, 0);
        MV_REG_WRITE(CPU_ENPOINT_MASK_REG, 0);
        MV_REG_WRITE(CPU_MAIN_IRQ_MASK_HIGH_REG, 0);
        MV_REG_WRITE(CPU_MAIN_FIQ_MASK_HIGH_REG, 0);
        MV_REG_WRITE(CPU_ENPOINT_MASK_HIGH_REG, 0);
}

/* init for the Master*/
void misc_init_r_dec_win(void)
{
#if defined(MV_INCLUDE_USB)
	{
		char *env;
		extern MV_STATUS mvSysUsbInit(MV_U32, MV_BOOL);

		env = getenv("usb0Mode");
#ifdef CONFIG_APBOOT
		printf("USB 0: Host mode\n");	
		mvSysUsbInit(0, MV_TRUE);
#else
		if((!env) || (strcmp(env,"device") == 0) || (strcmp(env,"Device") == 0) )
		if((!env) || (strcmp(env,"device") == 0) || (strcmp(env,"Device") == 0) )
		{
			printf("USB 0: Device Mode\n");	
			mvSysUsbInit(0, MV_FALSE);
		}
		else
		{
			printf("USB 0: Host Mode\n");	
			mvSysUsbInit(0, MV_TRUE);
		}
#endif
	}
#endif/* #if defined(MV_INCLUDE_USB) */

#if defined(MV_INCLUDE_XOR)
	mvSysXorInit();
#endif

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	mv_set_power_scheme();
#endif

    return;
}


/*
 * Miscellaneous platform dependent initialisations
 */

extern	MV_STATUS mvEthPhyRegRead(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 *data);
extern	MV_STATUS mvEthPhyRegWrite(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 data);

/* golabal mac address for yukon EC */
unsigned char yuk_enetaddr[6];
extern int interrupt_init (void);
extern int timer_init(void );
extern void i2c_init(int speed, int slaveaddr);


int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

#ifdef	CONFIG_FLASH_CFI_DRIVER
	int portwidth;
	MV_U32 devParam;
#endif
#if defined(MV_INCLUDE_TWSI)
	MV_TWSI_ADDR slave;
#endif
	MV_GPP_HAL_DATA gppHalData;

	unsigned int i;

	maskAllInt();

	/* must initialize the int in order for udelay to work */
	interrupt_init();
#if 0
	timer_init();
#endif

	/* Init the Board environment module (device bank params init) */
	mvBoardEnvInit();

#if defined(MV_INCLUDE_TWSI) && defined(CONFIG_CMD_I2C)
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);
#endif

	/* Init the Controlloer environment module (MPP init) */
	mvCtrlEnvInit();

#ifndef CONFIG_APBOOT
	mvBoardDebugLed(3);
#endif

	/* Init the Controller CPU interface */
	mvCpuIfInit(mvCpuAddrWinMap);

	/* Init the GPIO sub-system */
	memset(&gppHalData, 0, sizeof(gppHalData));
	gppHalData.ctrlRev = mvCtrlRevGet();
	mvGppInit(&gppHalData);

	/* arch number of Integrator Board */
	gd->bd->bi_arch_number = 529; //KW2 arch number

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x00000100;

	/* relocate the exception vectors */
	/* U-Boot is running from DRAM at this stage */
	for(i = 0; i < 0x100; i+=4)
	{
		*(unsigned int *)(0x0 + i) = *(unsigned int*)(TEXT_BASE + i);
	}

#ifndef CONFIG_APBOOT
	mvBoardDebugLed(4);
#endif
	
	return 0;
}

void misc_init_r_env(void){
	char *env;
	char tmp_buf[10];
	unsigned int malloc_len;
	DECLARE_GLOBAL_DATA_PTR;
//	char buff[256];
#if 0
	unsigned int flashSize =0 , secSize =0, ubootSize =0;


#if defined(MV_BOOTSIZE_4M)
	flashSize = _4M;
#elif defined(MV_BOOTSIZE_8M)
	flashSize = _8M;
#elif defined(MV_BOOTSIZE_16M)
	flashSize = _16M;
#elif defined(MV_BOOTSIZE_32M)
	flashSize = _32M;
#elif defined(MV_BOOTSIZE_64M)
	flashSize = _64M;
#endif

#if defined(MV_SEC_64K)
	secSize = _64K;
	ubootSize = _512K;
#elif defined(MV_SEC_128K)
	secSize = _128K;
	ubootSize = _128K * 5;
#elif defined(MV_SEC_256K)
	secSize = _256K;
	ubootSize = _256K * 3;
#endif

	if ((0 == flashSize) || (0 == secSize) || (0 == ubootSize))
	{
		env = getenv("console");
		if(!env)
			setenv("console","console=ttyS0,115200");
	}
	else
#endif
	env = getenv("console");
	if(!env) {
		if (mvBoardIdGet() == RD_88F6510_SFU_ID)
			setenv("console","console=ttyS0,115200 mv_port1_config=disconnected");
		else
			setenv("console","console=ttyS0,115200");
	}
/*#if defined(MV_SPI_BOOT)

	sprintf(buff,"console=ttyS0,115200 mtdparts=spi_flash:0x%x@0(uboot)ro,0x%x@0x%x(root)",
		 0x100000, flash->size - 0x100000, 0x100000);
	env = getenv("console");
	if(!env)
		setenv("console",buff);
#endif*/
// #if defined(MV_NAND_BOOT)
// 	sprintf(buff,"console=ttyS0,115200 mtdparts=nand_mtd:0x%x@0(uboot)ro,0x%x@0x%x(root)",
// 			0x100000, nand_info[0].size - 0x100000, 0x100000);
// 	env = getenv("console");
// 	if(!env)
// 		setenv("console",buff);

	//env = getenv("nandEnvBase");
	//strcpy(env, "");
// 	sprintf(buff, "0x%x", nandEnvBase);
// 	setenv("nandEnvBase", buff);
// #endif
#if 0
	/* Linux open port support */
	env = getenv("mainlineLinux");
	if(env && ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0)))
		setenv("mainlineLinux","yes");
	else
		setenv("mainlineLinux","no");

	env = getenv("mainlineLinux");
	if(env && ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0)))
	{
		/* arch number for open port Linux */
		env = getenv("arcNumber");
		if(!env )
		{
			/* arch number according to board ID */
			int board_id = mvBoardIdGet();	
			switch(board_id){
				case(DB_88F6281A_BP_ID):
				sprintf(tmp_buf,"%d", DB_88F6281_BP_MLL_ID);
				board_id = DB_88F6281_BP_MLL_ID; 
				break;
				case(RD_88F6192A_ID):
				sprintf(tmp_buf,"%d", RD_88F6192_MLL_ID);
				board_id = RD_88F6192_MLL_ID; 
				break;
				case(RD_88F6281A_ID):
				sprintf(tmp_buf,"%d", RD_88F6281_MLL_ID);
				board_id = RD_88F6281_MLL_ID; 
				break;
				case(DB_CUSTOMER_ID):
				break;
				default:
				sprintf(tmp_buf,"%d", board_id);
				board_id = board_id; 
				break;
			}
			gd->bd->bi_arch_number = board_id;
			setenv("arcNumber", tmp_buf);
		}
		else
		{
			gd->bd->bi_arch_number = simple_strtoul(env, NULL, 10);
		}
	}
#endif
	/* update the CASset env parameter */
	env = getenv("CASset");
	if(!env )
	{
#ifdef MV_MIN_CAL
		setenv("CASset","min");
#else
		setenv("CASset","max");
#endif
	}
        /* Monitor extension */
#ifdef MV_INCLUDE_MONT_EXT
	env = getenv("enaMonExt");
	if(/* !env || */ ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
		setenv("enaMonExt","yes");
	else
#endif
	setenv("enaMonExt","no");

#if defined (MV_INC_BOARD_NOR_FLASH)
	env = getenv("enaFlashBuf");
	if (((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0)))
		setenv("enaFlashBuf","no");
	else
		setenv("enaFlashBuf","yes");
#endif

	/* CPU streaming */
	env = getenv("enaCpuStream");
	if(!env || ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
		setenv("enaCpuStream","no");
	else
		setenv("enaCpuStream","yes");

	/* Write allocation */
	env = getenv("enaWrAllo");
	if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
		setenv("enaWrAllo","no");
	else
		setenv("enaWrAllo","yes");

	/* Pex mode */
	env = getenv("pexMode");
	if( env && ( ((strcmp(env,"EP") == 0) || (strcmp(env,"ep") == 0) )))
		setenv("pexMode","EP");
	else
		setenv("pexMode","RC");

	env = getenv("disL2Cache");
	if((mvCpuL2Exists() == MV_TRUE) && (!env || (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0)))
		setenv("disL2Cache","no"); 
	else
		setenv("disL2Cache","yes");

	env = getenv("setL2CacheWT");
	if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
		setenv("setL2CacheWT","yes"); 
	else
		setenv("setL2CacheWT","no");

	env = getenv("disL2Prefetch");
	if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) ) {
		setenv("disL2Prefetch","yes"); 
	
		/* ICache Prefetch */
		env = getenv("enaICPref");
		if( env && ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
			setenv("enaICPref","no");
		else
			setenv("enaICPref","yes");
		
		/* DCache Prefetch */
		env = getenv("enaDCPref");
		if( env && ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
			setenv("enaDCPref","no");
		else
			setenv("enaDCPref","yes");
	} else {
		setenv("disL2Prefetch","no");
		setenv("enaICPref","no");
		setenv("enaDCPref","no");
	}


	env = getenv("sata_dma_mode");
	if( env && ((strcmp(env,"No") == 0) || (strcmp(env,"no") == 0) ) )
		setenv("sata_dma_mode","no");
	else
		setenv("sata_dma_mode","yes");

	/* Malloc length */
	env = getenv("MALLOC_len");
	malloc_len =  simple_strtoul(env, NULL, 10) << 20;
	if(malloc_len == 0) {
		sprintf(tmp_buf,"%d",CONFIG_SYS_MALLOC_LEN>>20);
		setenv("MALLOC_len",tmp_buf);
	}

	/* primary network interface */
	env = getenv("ethprime");
	if(!env)
		setenv("ethprime",ENV_ETH_PRIME);
 
	/* netbsd boot arguments */
	env = getenv("netbsd_en");
	if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
		setenv("netbsd_en","no");
	else {
		setenv("netbsd_en","yes");
		env = getenv("netbsd_gw");
		if(!env)
			setenv("netbsd_gw","192.168.0.254");
		env = getenv("netbsd_mask");
		if(!env)
			setenv("netbsd_mask","255.255.255.0");

		env = getenv("netbsd_fs");
		if(!env)
			setenv("netbsd_fs","nfs");

		env = getenv("netbsd_server");
		if(!env)
			setenv("netbsd_server","192.168.0.1");

		env = getenv("netbsd_ip");
		if(!env)
		{
			env = getenv("ipaddr");
			setenv("netbsd_ip",env);
		}

		env = getenv("netbsd_rootdev");
		if(!env)
			setenv("netbsd_rootdev","mgi0");

		env = getenv("netbsd_add");
		if(!env)
			setenv("netbsd_add","0x800000");

		env = getenv("netbsd_get");
		if(!env)
			setenv("netbsd_get","tftpboot $(netbsd_add) $(image_name)");

		env = getenv("netbsd_set_args");
		if(!env)
			setenv("netbsd_set_args","setenv bootargs nfsroot=$(netbsd_server):$(rootpath) fs=$(netbsd_fs) \
ip=$(netbsd_ip) serverip=$(netbsd_server) mask=$(netbsd_mask) gw=$(netbsd_gw) rootdev=$(netbsd_rootdev) \
ethaddr=$(ethaddr) eth1addr=$(eth1addr) ethmtu=$(ethmtu) eth1mtu=$(eth1mtu) $(netbsd_netconfig)");

		env = getenv("netbsd_boot");
		if(!env)
			setenv("netbsd_boot","bootm $(netbsd_add) $(bootargs)");

		env = getenv("netbsd_bootcmd");
		if(!env)
			setenv("netbsd_bootcmd","run netbsd_get ; run netbsd_set_args ; run netbsd_boot");
	}

	/* vxWorks boot arguments */
	env = getenv("vxworks_en");
	if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
		setenv("vxworks_en","no");
	else {
		char* buff = (char *)0x1100;
		setenv("vxworks_en","yes");
		
		sprintf(buff,"mgi(0,0) host:vxWorks.st");
		env = getenv("serverip");
		strcat(buff, " h=");
		strcat(buff,env);
		env = getenv("ipaddr");
		strcat(buff, " e=");
		strcat(buff,env);
		strcat(buff, ":ffff0000 u=anonymous pw=target ");

		setenv("vxWorks_bootargs",buff);
	}

	/* linux boot arguments */
	env = getenv("bootargs_root");
	if(!env)
		setenv("bootargs_root","root=/dev/nfs rw");

	/* For open Linux we set boot args differently */
	env = getenv("mainlineLinux");
	if(env && ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0)))
	{
		env = getenv("bootargs_end");
		if(!env)
			setenv("bootargs_end",":::orion:eth0:none");
	}
	else
	{
		env = getenv("bootargs_end");
		if(!env)
			setenv("bootargs_end",MV_BOOTARGS_END);
	}
	
	env = getenv("image_name");
	if(!env)
		setenv("image_name","uImage");

#if (CONFIG_BOOTDELAY >= 0)
	env = getenv("bootcmd");
	if(!env)
#if defined(MV_INCLUDE_TDM) && defined(MV_INC_BOARD_QD_SWITCH)
		setenv("bootcmd","tftpboot 0x2000000 $(image_name);\
setenv bootargs $(console) $(bootargs_root) nfsroot=$(serverip):$(rootpath) \
ip=$(ipaddr):$(serverip)$(bootargs_end) $(mvNetConfig) $(mvPhoneConfig);  bootm 0x2000000; ");
#elif defined(MV_INC_BOARD_QD_SWITCH)
		setenv("bootcmd","tftpboot 0x2000000 $(image_name);\
setenv bootargs $(console) $(bootargs_root) nfsroot=$(serverip):$(rootpath) \
ip=$(ipaddr):$(serverip)$(bootargs_end) $(mvNetConfig);  bootm 0x2000000; ");
#elif defined(MV_INCLUDE_TDM)
		setenv("bootcmd","tftpboot 0x2000000 $(image_name);\
setenv bootargs $(console) $(bootargs_root) nfsroot=$(serverip):$(rootpath) \
ip=$(ipaddr):$(serverip)$(bootargs_end) $(mvNetConfig) $(mvPhoneConfig);  bootm 0x2000000; ");
#else

		setenv("bootcmd","tftpboot 0x2000000 $(image_name);\
setenv bootargs $(console) $(bootargs_root) nfsroot=$(serverip):$(rootpath) \
ip=$(ipaddr):$(serverip)$(bootargs_end);  bootm 0x2000000; ");
#endif
#endif /* (CONFIG_BOOTDELAY >= 0) */

	env = getenv("standalone");
	if(!env)
#if defined(MV_INCLUDE_TDM) && defined(MV_INC_BOARD_QD_SWITCH)
		setenv("standalone","fsload 0x2000000 $(image_name);setenv bootargs $(console) root=/dev/mtdblock0 rw \
ip=$(ipaddr):$(serverip)$(bootargs_end) $(mvNetConfig) $(mvPhoneConfig); bootm 0x2000000;");
#elif defined(MV_INC_BOARD_QD_SWITCH)
		setenv("standalone","fsload 0x2000000 $(image_name);setenv bootargs $(console) root=/dev/mtdblock0 rw \
ip=$(ipaddr):$(serverip)$(bootargs_end) $(mvNetConfig); bootm 0x2000000;");
#elif defined(MV_INCLUDE_TDM)
		setenv("standalone","fsload 0x2000000 $(image_name);setenv bootargs $(console) root=/dev/mtdblock0 rw \
ip=$(ipaddr):$(serverip)$(bootargs_end) $(mvPhoneConfig); bootm 0x2000000;");
#else
		setenv("standalone","fsload 0x2000000 $(image_name);setenv bootargs $(console) root=/dev/mtdblock0 rw \
ip=$(ipaddr):$(serverip)$(bootargs_end); bootm 0x2000000;");
#endif

	/* Set boodelay to 3 sec, if Monitor extension are disabled */
	if(!enaMonExt()){
		setenv("bootdelay","3");
		setenv("disaMvPnp","no");
	}

	/* Disable PNP config of Marvel memory controller devices. */
	env = getenv("disaMvPnp");
	if(!env)
		setenv("disaMvPnp","no");

#if (defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH))
	/* Generate random ip and mac address */
	/* Read RTC to create pseudo-random data for enc */
	struct rtc_time tm;
	unsigned int xi, xj, xk, xl;
	char ethaddr_0[30];
	char ethaddr_1[30];
	char pon_addr[30];

	rtc_get(&tm);
	xi = ((tm.tm_yday + tm.tm_sec)% 254);
	/* No valid ip with one of the fileds has the value 0 */
	if (xi == 0)
		xi+=2;

	xj = ((tm.tm_yday + tm.tm_min)%254);
	/* No valid ip with one of the fileds has the value 0 */
	if (xj == 0)
		xj+=2;

	/* Check if the ip address is the same as the server ip */
	if ((xj == 1) && (xi == 11))
		xi+=2;

	xk = (tm.tm_min * tm.tm_sec)%254;
	xl = (tm.tm_hour * tm.tm_sec)%254;

	sprintf(ethaddr_0,"00:50:43:%02x:%02x:%02x",xk,xi,xj);
	sprintf(ethaddr_1,"00:50:43:%02x:%02x:%02x",xl,xi,xj);
	sprintf(pon_addr,"00:50:43:%02x:%02x:%02x",xj,xk,xl);

	/* MAC addresses */
	env = getenv("ethaddr");
	if(!env)
		setenv("ethaddr",ethaddr_0);

	env = getenv("eth1addr");
	if(!env)
		setenv("eth1addr",ethaddr_1);

	env = getenv("mv_pon_addr");
	if(!env)
		setenv("mv_pon_addr",pon_addr);

	env = getenv("ethmtu");
	if(!env)
		setenv("ethmtu","1500");

	env = getenv("eth1mtu");
	if(!env)
		setenv("eth1mtu","1500");

	/* Set mvNetConfig env parameter */
	env = getenv("mvNetConfig");
	if(!env ) {
		switch (mvBoardIdGet()) {
			case RD_88F6510_SFU_ID:
			case RD_88F6530_MDU_ID:
				setenv("mvNetConfig","mv_net_config=0");
			break;
			case RD_88F6560_GW_ID:
			case DB_88F6535_BP_ID:
			default:
				setenv("mvNetConfig","mv_net_config=4,(00:50:43:11:11:11,0:1:2:3),mtu=1500");
			break;
		}
	}
#endif /*  (MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH) */

#if defined(MV_INCLUDE_TDM)
	/* Set mvPhoneConfig env parameter */
	env = getenv("mvPhoneConfig");
	if(!env )
		setenv("mvPhoneConfig","mv_phone_config=dev0:fxs,dev1:fxs");
#endif

#if defined(MV_INCLUDE_USB)
	/* USB Host */
	env = getenv("usb0Mode");
	if(!env)
		setenv("usb0Mode",ENV_USB0_MODE);
#endif  /* (MV_INCLUDE_USB) */

#if defined(YUK_ETHADDR)
	env = getenv("yuk_ethaddr");
	if(!env)
		setenv("yuk_ethaddr",YUK_ETHADDR);

	{
		int i;
		char *tmp = getenv ("yuk_ethaddr");
		char *end;

		for (i=0; i<6; i++) {
			yuk_enetaddr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
			if (tmp)
				tmp = (*end) ? end+1 : end;
		}
	}
#endif /* defined(YUK_ETHADDR) */

#if defined(MTD_NAND_LNC)
	env = getenv("nandEcc");
	if(!env)
		setenv("nandEcc", "1bit");
#endif

#if defined(CONFIG_CMD_RCVR)
	env = getenv("netretry");
	if (!env)
		setenv("netretry","no");

	env = getenv("rcvrip");
	if (!env)
		setenv("rcvrip",RCVR_IP_ADDR);

	env = getenv("loadaddr");
	if (!env)
		setenv("loadaddr",RCVR_LOAD_ADDR);

	env = getenv("autoload");
	if (!env)
		setenv("autoload","no");

	/* Check the recovery trigger */
	recoveryDetection();
#endif
	env = getenv("eeeEnable");
	if (!env)
		setenv("eeeEnable","no");

	return;
}

#ifdef BOARD_LATE_INIT
int board_late_init (void)
{
	/* Check if to use the LED's for debug or to use single led for init and Linux heartbeat */
#ifndef CONFIG_APBOOT
	mvBoardDebugLed(0);
#endif
	return 0;
}
#endif

void pcie_tune(void)
{
	MV_REG_WRITE(0xF1041AB0, 0x100);
	MV_REG_WRITE(0xF1041A20, 0x78000801);
	MV_REG_WRITE(0xF1041A00, 0x4014022F);
	MV_REG_WRITE(0xF1040070, 0x18110008);

	return;
}

int board_eth_init(bd_t *bis)
{
#ifdef  CONFIG_MARVELL
#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH)
        extern int mv_eth_initialize(bd_t *);
	/* move to the begining so in case we have a PCI NIC it will
	read the env mac addresses correctlly. */
	mv_eth_initialize(bis);
#endif
#endif
#if defined(CONFIG_SK98)
	skge_initialize(bis);
#endif
#if defined(CONFIG_E1000)
	e1000_initialize(bis);
#endif
	return 0;
}

int print_cpuinfo (void)
{
	char name[50];

	mvBoardNameGet(name);
	printf("Board: %s\n",  name);
	mvCtrlModelRevNameGet(name);
	printf("SoC:   %s\n",  name);
	mvCpuNameGet(name);
	printf("CPU:   %s - ",  name);
#ifdef MV_CPU_BE
	printf("BE\n");
#else
	printf("LE\n");
#endif
	printf("       CPU @ %dMhz, ",  mvCpuPclkGet()/1000000);
	if (mvCpuL2Exists() == MV_TRUE)
		printf("L2 @ %dMhz\n", mvCpuL2ClkGet()/1000000);
	else
		printf("No L2\n");

	printf("       DDR%s @ %dMhz, TClock @ %dMhz\n", ((mvDramIfIsTypeDdr3() == MV_TRUE) ? "3" : "2"),
			CONFIG_SYS_BUS_CLK/1000000, mvTclkGet()/1000000);
	return 0;
}
int misc_init_r (void)
{
#ifndef CONFIG_APBOOT
	char *env;
#endif

#ifndef ARUBA_ARM
	mvBoardDebugLed(5);

	/* init special env variables */
	misc_init_r_env();
#endif

	mv_cpu_init();

#if defined(MV_INCLUDE_MONT_EXT)
	if(enaMonExt()){
		printf("Marvell monitor extension:\n");
		mon_extension_after_relloc();
	}
#endif /* MV_INCLUDE_MONT_EXT */

	/* init the units decode windows */
	misc_init_r_dec_win();

#ifdef CONFIG_EEE
#if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH)
	/* Init the PHY or Switch of the board */
	mvBoardEgigaPhyInit();
#endif /* #if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH) */
#endif	/* EEE */

#ifdef CONFIG_PCI
#if !defined(MV_MEM_OVER_PCI_WA) && !defined(MV_MEM_OVER_PEX_WA)
	pci_init();
#endif
#endif

#ifndef ARUBA_ARM
	mvBoardDebugLed(6);
	/* Prints the modules detected */
	mvBoardMppModuleTypePrint();

	mvBoardDebugLed(7);
#endif

#ifndef CONFIG_APBOOT
	/* pcie fine tunning */
	env = getenv("pcieTune");
	if(env && ((strcmp(env,"yes") == 0) || (strcmp(env,"yes") == 0)))
		pcie_tune();
	else
		setenv("pcieTune","no");
#endif

#ifndef CONFIG_EEE
#if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH)
	/* Init the PHY or Switch of the board */
	mvBoardEgigaPhyInit();
#endif /* #if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH) */
#endif	/* EEE */
	
	return 0;
}

MV_U32 mvTclkGet(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	/* get it only on first time */
	if(gd->tclk == 0)
		gd->tclk = mvBoardTclkGet();

	return gd->tclk;
}

MV_U32 mvSysClkGet(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	/* get it only on first time */
	if(gd->bus_clk == 0)
		gd->bus_clk = mvBoardSysClkGet();

	return gd->bus_clk;
}

/* exported for EEMBC */
MV_U32 mvGetRtcSec(void)
{
	MV_RTC_TIME time;
#ifdef MV_INCLUDE_RTC
	mvRtcTimeGet(&time);
#elif CONFIG_RTC_DS1338_DS1339
	mvRtcDS133xTimeGet(&time);
#endif
	return (time.minutes * 60) + time.seconds;
}

void reset_cpu(void)
{
#ifdef GPIO_HW_RESET
	extern void gpio_set_as_output(unsigned);
	extern void gpio_out(unsigned, unsigned);
	gpio_set_as_output(GPIO_HW_RESET);
	gpio_out(GPIO_HW_RESET, 0);
#endif
	mvBoardReset();
}

void mv_cpu_init(void)
{
	char *env;
	volatile unsigned int temp;

#ifndef CONFIG_APBOOT
	/*CPU streaming & write allocate */
	env = getenv("enaWrAllo");
	if(env && ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0)))  
	{
		__asm__ __volatile__("mrc    p15, 1, %0, c15, c1, 0" : "=r" (temp));
		temp |= BIT28;
		__asm__ __volatile__("mcr    p15, 1, %0, c15, c1, 0" :: "r" (temp));
		
	}
	else
	{
		__asm__ __volatile__("mrc    p15, 1, %0, c15, c1, 0" : "=r" (temp));
		temp &= ~BIT28;
		__asm__ __volatile__("mcr    p15, 1, %0, c15, c1, 0" :: "r" (temp));
	}

	env = getenv("enaCpuStream");
	if(!env || (strcmp(env,"no") == 0) ||  (strcmp(env,"No") == 0) )
	{
		__asm__ __volatile__("mrc    p15, 1, %0, c15, c1, 0" : "=r" (temp));
		temp &= ~BIT29;
		__asm__ __volatile__("mcr    p15, 1, %0, c15, c1, 0" :: "r" (temp));
	}
	else
	{
		__asm__ __volatile__("mrc    p15, 1, %0, c15, c1, 0" : "=r" (temp));
		temp |= BIT29;
		__asm__ __volatile__("mcr    p15, 1, %0, c15, c1, 0" :: "r" (temp));
	}
	
	#if 0
	/* Verifay write allocate and streaming */
	printf("\n");
	__asm__ __volatile__("mrc    p15, 1, %0, c15, c1, 0" : "=r" (temp));
	if (temp & BIT29)
		printf("Streaming enabled \n");
	else
		printf("Streaming disabled \n");
	if (temp & BIT28)
		printf("Write allocate enabled\n");
	else
		printf("Write allocate disabled\n");
	#endif

	/* DCache Pref  */
	env = getenv("enaDCPref");
	if(env && ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0)))
	{
		MV_REG_BIT_SET( CPU_CONFIG_REG , CCR_DCACH_PREF_BUF_ENABLE);
	}

	if(env && ((strcmp(env,"no") == 0) ||  (strcmp(env,"No") == 0)))
	{
		MV_REG_BIT_RESET( CPU_CONFIG_REG , CCR_DCACH_PREF_BUF_ENABLE);
	}

	/* ICache Pref  */
	env = getenv("enaICPref");
	if(env && ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0)))
	{
		MV_REG_BIT_SET( CPU_CONFIG_REG , CCR_ICACH_PREF_BUF_ENABLE);
	}
	
	if(env && ((strcmp(env,"no") == 0) ||  (strcmp(env,"No") == 0)))
	{
		MV_REG_BIT_RESET( CPU_CONFIG_REG , CCR_ICACH_PREF_BUF_ENABLE);
	}

	/* Set L2C WT mode - Set bit 4 */
	temp = MV_REG_READ(CPU_L2_CONFIG_REG);
	env = getenv("setL2CacheWT");
	if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
	{
		temp |= BIT4;
	}
	else
		temp &= ~BIT4;
	MV_REG_WRITE(CPU_L2_CONFIG_REG, temp);
#else
	/* disable write allocations per Marvell */
	__asm__ __volatile__("mrc    p15, 1, %0, c15, c1, 0" : "=r" (temp));
	temp &= ~BIT28;
	__asm__ __volatile__("mcr    p15, 1, %0, c15, c1, 0" :: "r" (temp));

	/* disable streaming per Marvell */
	__asm__ __volatile__("mrc    p15, 1, %0, c15, c1, 0" : "=r" (temp));
	temp &= ~BIT29;
	__asm__ __volatile__("mcr    p15, 1, %0, c15, c1, 0" :: "r" (temp));
 
	/* enable I- and D-cache prefetching */
	MV_REG_BIT_SET( CPU_CONFIG_REG , CCR_DCACH_PREF_BUF_ENABLE);
	MV_REG_BIT_SET( CPU_CONFIG_REG , CCR_ICACH_PREF_BUF_ENABLE);
	
	/*
	 * Set L2C WT mode - Set bit 4; 
	 * this is for U-boot only; Linux will override
	 */
	temp = MV_REG_READ(CPU_L2_CONFIG_REG);
	temp |= BIT4;
	MV_REG_WRITE(CPU_L2_CONFIG_REG, temp);
#endif

	/* L2Cache settings */
	asm ("mrc p15, 1, %0, c15, c1, 0":"=r" (temp));

	/* Disable L2C pre fetch - Set bit 24 */
	env = getenv("disL2Prefetch");
	if(env && ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
		temp &= ~BIT24;
	else
		temp |= BIT24;

	/* enable L2C - Set bit 22 */
	env = getenv("disL2Cache");
	if(!env || ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
		temp |= BIT22;
	else
		temp &= ~BIT22;
	
	asm ("mcr p15, 1, %0, c15, c1, 0": :"r" (temp));

	/* Enable i cache */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (temp));
	temp |= BIT12;
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (temp));
	
	/* Change reset vector to address 0x0 */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (temp));
	temp &= ~BIT13;
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (temp));
}

/* Set unit in power off mode acording to the detection of MPP */
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
int mv_set_power_scheme(void)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_U16 devId = mvCtrlModelGet();
	MV_U32 ethCompOpt;
	MV_U32 ponOpt;
	MV_BOOL integSwitch, gePhy, fePhy;

#ifndef CONFIG_APBOOT
	mvOsOutput("Shutting down unused interfaces:\n");
#endif
	/* GPON */
	ponOpt = mvBoardPonConfigGet();
	if(ponOpt == BOARD_PON_NONE) {
#ifdef ARUBA_ARM
		{
			MV_U32 val;
			/*
			 * sequence provided by Sandeep @ Marvell:
			 * - first set bits 4:3 to 0
			 * - then set bits 2:0 to 0
			 */
			val = MV_REG_READ(0xf10184f4);
			val &= ~(3 << 3);
			MV_REG_WRITE(0xf10184f4, val);

			val = MV_REG_READ(0xf10184f4);
			val &= ~7;
			MV_REG_WRITE(0xf10184f4, val);
		}
#endif

#ifndef CONFIG_APBOOT
		mvOsOutput("       PON\n");
#endif
		mvCtrlPwrClckSet(XPON_UNIT_ID, 0, MV_FALSE);
		mvCtrlPwrMemSet(XPON_UNIT_ID, 0, MV_FALSE);
	}

	/* Sata & Ethernet Complex */
	ethCompOpt = mvBoardEthComplexConfigGet();

	/* Sata */
	if (!(ethCompOpt & ESC_OPT_SATA)) {
#ifndef CONFIG_APBOOT
		mvOsOutput("       SATA\n");
#endif
		mvCtrlPwrClckSet(SATA_UNIT_ID, 0, MV_FALSE);
		mvCtrlPwrMemSet(SATA_UNIT_ID, 0, MV_FALSE);
	}

	/* Ethernet complex */
#if 0
	/* MAC0*/
 	if (!(ethCompOpt & (ESC_OPT_RGMIIA_MAC0 | ESC_OPT_RGMIIB_MAC0 |
 					ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_SGMII))) {
 		mvOsOutput("       MAC0\n");
 		mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, 0, MV_FALSE);
 	}

	/* MAC1 */
	if(!(ethCompOpt & (ESC_OPT_RGMIIA_MAC1 | ESC_OPT_MAC1_2_SW_P5 | 
			ESC_OPT_GEPHY_MAC1))) {
#ifndef CONFIG_APBOOT
		mvOsOutput("       MAC1\n");
#endif
		mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, 1, MV_FALSE);
	}
#endif
	/* Switch */
 	integSwitch = MV_FALSE;
 	if (!(ethCompOpt & (ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5))) {
 		integSwitch = MV_TRUE;
#ifndef CONFIG_APBOOT
 		mvOsOutput("       Switch\n");
#endif
 	}

#if 0
	/* 3xFE */
#ifdef CONFIG_ARRAN
	/* power off FE PHYs */
	{
		MV_U32 val;
		/*
		 * sequence provided by Sandeep @ Marvell:
		 * - first set bit 15 to 1
		 * - then set power down bits for all 3 PHYs to 1
		 */
		val = MV_REG_READ(0xf1018880);
		val |= (1 << 15);
		MV_REG_WRITE(0xf1018880, val);

		val = MV_REG_READ(0xf1018880);
		val |= (7 << 16);
		MV_REG_WRITE(0xf1018880, val);
	}
#endif

#ifdef CONFIG_ARRAN
	/* power off GE PHY */
	{
		MV_U32 val;
		/*
		 * sequence provided by Sandeep @ Marvell:
		 * - first set bit 25 to 1 and bit 5 to 0
		 * - then set bit 25 to 1 and bit 5 to 1
		 */
		val = MV_REG_READ(0xf10188A0);
		val |= (1 << 25);
		val &= ~(1 << 5);
		MV_REG_WRITE(0xf10188A0, val);

		val = MV_REG_READ(0xf10188A0);
		val |= (1 << 5) | (1 << 25);
		MV_REG_WRITE(0xf10188A0, val);
	}
#endif
#endif

	/* 3xFE */
 	fePhy = MV_FALSE;
 	if (!(ethCompOpt & ESC_OPT_FE3PHY)) {
#ifndef CONFIG_APBOOT
 		mvOsOutput("       3xFE-PHY\n");
#endif
 		fePhy = MV_TRUE;
 	}
 
 	/* GE Phy */
 	gePhy = MV_FALSE;
 	if (!(ethCompOpt & (ESC_OPT_GEPHY_MAC1 | ESC_OPT_GEPHY_SW_P0 |
 				ESC_OPT_GEPHY_SW_P5))) {
#ifndef CONFIG_APBOOT
 		mvOsOutput("       GE-PHY\n");
#endif
 		gePhy = MV_TRUE;
 	}
  
 	mvEthernetComplexShutdownIf(integSwitch, gePhy, fePhy);

	/* SDIO */
	if ((boardId == RD_88F6510_SFU_ID) || (boardId == RD_88F6530_MDU_ID) ||
	    (mvBoardIsSdioEnabled() == MV_FALSE)) {
#ifndef CONFIG_APBOOT
		mvOsOutput("       SDIO\n");
#endif
		mvCtrlPwrClckSet(SDIO_UNIT_ID, 0, MV_FALSE);
	}

#ifdef CONFIG_ARRAN
	mvUsbPowerDown(0);

	/* disable unused memories per Marvell */
	{ 
		MV_U32 val;
		val = MV_REG_READ(0xf1018218);
		val |= (1 << 0); /* comm */
		val |= (1 << 2); /* USB */
		val |= (1 << 6); /* SATA */
		val |= (1 << 18); /* GP */
		val |= (1 << 21); /* GPON MAC */
		val |= (1 << 22); /* GPON ROM  */
		MV_REG_WRITE(0xf1018218, val);
	}

	/* ensure all unused clocks are off */
	{ 
		MV_U32 val;
		val = MV_REG_READ(0xf101821c);
		val &= ~(1 << 0); /* TDM V clock */
		val &= ~(1 << 3); /* USB */
		val &= ~(1 << 4); /* SDIO */
		val &= ~(1 << 5); /* comm */
		val &= ~(1 << 14); /* SATA */
		val &= ~(1 << 28); /* GPON */
		MV_REG_WRITE(0xf101821c, val);
	}
#endif

	/* Shutdown other interfaces depending on device type. */
	if (devId == MV_6510_DEV_ID) {
		/* PCI-E */
		mvOsOutput("       PCI-E 0/1\n");
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
		mvCtrlPwrClckSet(PEX_UNIT_ID, 1, MV_FALSE);
		mvCtrlPwrMemSet(PEX_UNIT_ID, 0, MV_FALSE);
		mvCtrlPwrMemSet(PEX_UNIT_ID, 1, MV_FALSE);

		/* Crypto engine */
		mvOsOutput("       Crypto\n");
		mvCtrlPwrClckSet(CESA_UNIT_ID, 0, MV_FALSE);
		mvCtrlPwrMemSet(CESA_UNIT_ID, 0, MV_FALSE);

		/* Xor engine */
		mvOsOutput("       XOR engine 0/1\n");
		mvCtrlPwrClckSet(XOR_UNIT_ID, 0, MV_FALSE);
		mvCtrlPwrClckSet(XOR_UNIT_ID, 1, MV_FALSE);
		mvCtrlPwrMemSet(XOR_UNIT_ID, 0, MV_FALSE);
		mvCtrlPwrMemSet(XOR_UNIT_ID, 1, MV_FALSE);
	}

	return MV_OK;
}

#endif /* defined(MV_INCLUDE_CLK_PWR_CNTRL) */

/*******************************************************************************
* mvUpdateNorFlashBaseAddrBank - 
*
* DESCRIPTION:
*       This function update the CFI driver base address bank with on board NOR
*       devices base address.
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*       None
*
*******************************************************************************/
#if 0
#ifdef	CONFIG_FLASH_CFI_DRIVER
MV_VOID mvUpdateNorFlashBaseAddrBank(MV_VOID)
{
	MV_U32 devBaseAddr;
	MV_U32 devNum = 0;
	int i;

	/* Update NOR flash base address bank for CFI flash init driver */
	for (i = 0 ; i < CONFIG_SYS_MAX_FLASH_BANKS_DETECT; i++)
	{
		devBaseAddr = mvBoardGetDeviceBaseAddr(i,BOARD_DEV_NOR_FLASH);
		if (devBaseAddr != 0xFFFFFFFF)
		{
			flash_add_base_addr (devNum, devBaseAddr);
			devNum++;
		}
	}
	mv_board_num_flash_banks = devNum;

	/* Update SPI flash count for CFI flash init driver */
	/* Assumption only 1 SPI flash on board */
	for (i = 0 ; i < CONFIG_SYS_MAX_FLASH_BANKS_DETECT; i++)
	{
		devBaseAddr = mvBoardGetDeviceBaseAddr(i,BOARD_DEV_SPI_FLASH);
		if (devBaseAddr != 0xFFFFFFFF)
		mv_board_num_flash_banks += 1;
	}
}
#endif	/* CONFIG_FLASH_CFI_DRIVER */
#endif

#if 0
#ifdef MV_INC_BOARD_SPI_FLASH
#include <environment.h>
#include "norflash/mvFlash.h"

void memcpyFlash(env_t *env_ptr, void* buffer, MV_U32 size)
{
    MV_FLASH_INFO *pFlash;
    pFlash = getMvFlashInfo(BOOT_FLASH_INDEX);

    mvFlashBlockRd(pFlash,(MV_U32 *)env_ptr - mvFlashBaseAddrGet(pFlash),
                    size, (MV_U8 *)buffer);
}
#endif
#endif

static char buf[2048];
static int
do_cpu_regs(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	mvCpuIfPrintSystemConfig(buf, 0);
	printf("%s\n", buf);
	return 0;
}

U_BOOT_CMD(
    cpuRegs,    7,    1,     do_cpu_regs,
    "cpuRegs    - display CPU registers\n",                   
    " Usage: cpuRegs \n"
);
