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
#include "ddr2/mvDramIf.h"
#include "mvOs.h"
#include "mvBoardEnvLib.h"
#include "ddr2/mvDramIfRegs.h"
#include "mvCpuIfRegs.h"
#include "mvCpuIf.h"
#include "mvCpu.h"

#ifdef DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

extern void i2c_init(int speed, int slaveaddr);
extern void _start(void);
extern unsigned int  mvCpuPclkGet(void);
extern void reset_cpu(void);
extern int dramBoot;


#ifdef MV_INC_DRAM_MFG_TEST
static MV_VOID mvDramMfgTrst(void);
static MV_STATUS mv_mem_test(MV_U32* pMem, MV_U32 pattern, MV_U32 count);
static MV_STATUS mv_mem_cmp(MV_U32* pMem, MV_U32 pattern, MV_U32 count);
#endif

MV_VOID mvIntrfaceWidthPrint(MV_VOID)
{
	printf(" 16bit width");
}

MV_VOID mvIntrfaceParamPrint(MV_VOID)
{
    MV_U32 temp;

    printf("DRAM");
    switch((MV_REG_READ(0x141c) >> 4) & 0x7)
    {
	case 0x3: printf(" CAS Latency = 3");
	    break;
        case 0x4: printf(" CAS Latency = 4");
	    break;
        case 0x5: printf(" CAS Latency = 5");
            break;
        case 0x6: printf(" CAS Latency = 6");
            break;
        default: printf(" unknown CAL ");
            break;
    }

    temp = MV_REG_READ(0x1408);
    printf(" tRP = %d tRAS = %d tRCD=%d\n",
           ((temp >> 8) & 0xf) + 1, ((temp >> 16) & 0x10) + (temp & 0xf) + 1, ((temp >> 4) & 0xf) + 1);
}

#ifdef CONFIG_APBOOT
extern void mv_testdram(void);
#endif

int dram_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	unsigned int i;
#if defined(MV_INC_BOARD_DDIM)
	unsigned int dramTotalSize=0;
#endif
        MV_32 memBase, temp;
        char name[20];

	mvCtrlModelRevNameGet(name);
	printf("CPU:   %s",  name);

	temp = MV_REG_READ(0x1400);
	if (temp & BIT14) {
	    printf(" (DDR3)\n");
	} else {
	    printf(" (DDR2)\n");
	}

	printf("Clock: CPU %dMHz, L2 %dMHz, ",  mvCpuPclkGet()/1000000, mvCpuL2ClkGet()/1000000);
	printf("SysClock %dMHz, TClock %dMHz\n", CONFIG_SYS_BUS_HZ/1000000, mvTclkGet()/1000000);

#if defined(MV_INC_BOARD_DDIM)
	/* Call dramInit */
	if (0 == (dramTotalSize = initdram(0)))
	{
		printf("DRAM Initialization Failed\n");
		reset_cpu();
		return (1);
	}
#endif

	//mvIntrfaceParamPrint();

	for(i = 0; i< MV_DRAM_MAX_CS; i++)
	{
		MV_CPU_DEC_WIN addrDecWin;
		if((mvCpuIfTargetWinGet(SDRAM_CS0 + i, &addrDecWin) == MV_OK) &&
		   (addrDecWin.enable == MV_TRUE)){
			memBase = addrDecWin.addrWin.baseLow;
			gd->bd->bi_dram[i].start = memBase;
			gd->bd->bi_dram[i].size = addrDecWin.addrWin.size;
		}
		//dramTotalSize += gd->bd->bi_dram[i].size;
//		if (gd->bd->bi_dram[i].size)
//		{
			//printf("DRAM CS[%d] base 0x%08x   ",i, gd->bd->bi_dram[i].start);
			//mvSizePrint(gd->bd->bi_dram[i].size);
			//printf("\n");
//		}
	}
 
//	printf("DRAM Total ");
//	mvSizePrint(dramTotalSize);
//	mvIntrfaceWidthPrint();
//	printf("\n");

	printf("DRAM:  ");
	mvSizePrint(mvDramIfSizeGet());

#ifdef CONFIG_APBOOT
	printf("\n");
	mv_testdram();
#endif

#ifdef MV_INC_DRAM_MFG_TEST
	mvDramMfgTrst();
#endif
        return 0;
}

#if defined(MV_INC_BOARD_DDIM)

/* u-boot interface function to SDRAM init - this is where all the
 * controlling logic happens */
long int initdram(int board_type)
{
    MV_VOIDFUNCPTR pRom; 
    MV_U32 forcedCl;    /* Forced CAS Latency */
    MV_U32 totalSize;
    char * env;
    MV_TWSI_ADDR slave;

    /* r0 <- current position of code   */
    /* test if we run from flash or RAM */
    if(dramBoot != 1)
    {
    slave.type = ADDR7_BIT;
    slave.address = 0;
    mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);

    /* Calculating MIN/MAX CAS latency according to user settings */
    env = getenv("CASset");
	
    if(env && (strcmp(env,"1.5") == 0))
    {
        forcedCl = 15;
    }
    else if(env && (strcmp(env,"2") == 0))
    {
        forcedCl = 20;
    }
    else if(env && (strcmp(env,"2.5") == 0))
    {
        forcedCl = 25;
    }
    else if(env && (strcmp(env,"3") == 0))
    {
        forcedCl = 30;
    }
    else if(env && (strcmp(env,"4") == 0))
    {
        forcedCl = 40;
    }
    else if(env && (strcmp(env,"5") == 0))
    {
        forcedCl = 50;
    }
    else if(env && (strcmp(env,"6") == 0))
    {
        forcedCl = 60;
    }
    else
    {
        forcedCl = 0;
    }
    
    /* detect the dram configuartion parameters */
    if (MV_OK != mvDramIfDetect(forcedCl,1))
    {
        printf("DRAM Auto Detection Failed! System Halt!\n");
        return 0;
    }

    /* set the dram configuration */
    /* Calculate jump address of _mvDramIfConfig() */

#if defined(MV_BOOTROM)
    pRom = (MV_VOIDFUNCPTR)(((MV_VOIDFUNCPTR)_mvDramIfConfig - (MV_VOIDFUNCPTR)_start) +
		(MV_VOIDFUNCPTR)CONFIG_SYS_MONITOR_BASE + (MV_VOIDFUNCPTR)MONITOR_HEADER_LEN);
#else
    pRom = (MV_VOIDFUNCPTR)(((MV_VOIDFUNCPTR)_mvDramIfConfig - (MV_VOIDFUNCPTR)_start) +
					(MV_VOIDFUNCPTR)CONFIG_SYS_MONITOR_BASE);
#endif

    
    (*pRom) (); /* Jump to _mvDramIfConfig*/
    }
    
    totalSize = mvDramIfSizeGet();


    return(totalSize);
}

#endif /* #if defined(MV_INC_BOARD_DDIM) */

#ifdef MV_INC_DRAM_MFG_TEST
static MV_VOID mvDramMfgTrst(void)
{

	/* Memory test */
	DECLARE_GLOBAL_DATA_PTR;
	unsigned int mem_len,i,j, pattern;
	unsigned int *mem_start;
        char *env;
        
	env = getenv("enaPost");
        if(!env || ( (strcmp(env,"Yes") == 0) || (strcmp(env,"yes") == 0) ) )
	{
	    printf("Memory test pattern: ");

	    for (j = 0 ; j<2 ; j++)
	    {

		switch(j){
		case 0:
		    pattern=0x55555555;
		    printf("0x%X, ",pattern);
		    break;
		case 1:
		    pattern=0xAAAAAAAA;
		    printf("0x%X, ",pattern);
		    break;
		default:
		    pattern=0x0;
		    printf("0x%X, ",pattern);
		    break;
		}

		for(i = 0; i< MV_DRAM_MAX_CS; i++)
		{
		    mem_start = (unsigned int *)gd->bd->bi_dram[i].start;
		    mem_len = gd->bd->bi_dram[i].size;
		    if (i == 0)
		    {
			mem_start+= _4M;
			mem_len-= _4M;
		    }
		    mem_len/=4;
		    if (MV_OK != mv_mem_test(mem_start, pattern, mem_len))
		    {
			printf(" Fail!\n");
			while(1);
		    }
		}
	    }
	    printf(" Pass\n");
	}
}


static MV_STATUS mv_mem_test(MV_U32* pMem, MV_U32 pattern, MV_U32 count)
{
	int i;
	for (i=0 ; i< count ; i+=1)
		*(pMem + i) = pattern;
	
	if (MV_OK != mv_mem_cmp(pMem, pattern, count))
	{
		return MV_ERROR;
	}
	return MV_OK;
}

static MV_STATUS mv_mem_cmp(MV_U32* pMem, MV_U32 pattern, MV_U32 count)
{
	int i;
	for (i=0 ; i< count ; i+=1)
	{
		if (*(pMem + i) != pattern)
		{
			printf("Fail\n");
			printf("Test failed at 0x%x\n",(pMem + i));
			return MV_ERROR;
		}
	}

	return MV_OK;
}
#endif /* MV_INC_DRAM_MFG_TEST */
