/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
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
 *
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
 
/* PCI.c - PCI functions */
#include "mvCommon.h"
#include <common.h>
#include <config.h>
#include <command.h>
#if defined(CONFIG_CMD_PCI)


#include <pci.h>
#include "boardEnv/mvBoardEnvLib.h"
#include "mvSysPexApi.h"
#include "gpp/mvGpp.h"
//#include "pci-if/mvPciIf.h"
#include "mvCpuIf.h"
#include "pci-if/pci_util/mvPciUtils.h"


#ifdef DEBUG
#define DB(x) x
#else
#define DB(x)
#endif /* DEBUG */

/* global definetion */
#define REG_NUM_MASK	(0x3F << 2)
/* global indicate wether we are in the scan process */
unsigned int bus_scan = 0;
extern unsigned int whoAmI(void);


#if defined(CONFIG_CMD_BSP)

/******************************************************************************
* Category     - PCI0
* Functionality- Scans PCI0 for devices and prints relevant information
* Need modifications (Yes/No) - No
*****************************************************************************/
MV_BOOL scanPci(MV_U32 host)
{
    MV_U32    index,numOfElements=4*8,barIndex;
    MV_PCI_DEVICE      pciDevices[4*8]; //3 slots and us,Max 8 functions per slot

    memset (&pciDevices,0,12*sizeof(MV_PCI_DEVICE));

    if (mvPciScan(host, pciDevices , &numOfElements) != MV_OK )
    {
	DB(printf("scanPci:mvPciScan failed for host %d \n",host));
	return MV_FALSE;
    }

    
    for(index = 0; index < numOfElements ; index++)
    {
	    printf("\nBus: %x Device: %x Func: %x Vendor ID: %x Device ID: %x\n",
	    pciDevices[index].busNumber,
            pciDevices[index].deviceNum,
	    pciDevices[index].function,
            pciDevices[index].venID,
            pciDevices[index].deviceID);

		printf("-------------------------------------------------------------------\n");

		printf("Class: %s\n",pciDevices[index].type);

		/* check if we are bridge*/
		if ((pciDevices[index].baseClassCode == PCI_BRIDGE_CLASS)&&
			(pciDevices[index].subClassCode == P2P_BRIDGE_SUB_CLASS_CODE))
		{
			printf("Primary Bus:0x%x \tSecondary Bus:0x%x \tSubordinate Bus:0x%x\n",
							pciDevices[index].p2pPrimBusNum,
							pciDevices[index].p2pSecBusNum,
							pciDevices[index].p2pSubBusNum);
			
			printf("IO Base:0x%x \t\tIO Limit:0x%x",pciDevices[index].p2pIObase,
							pciDevices[index].p2pIOLimit);

			(pciDevices[index].bIO32)? (printf(" (32Bit IO)\n")):
								(printf(" (16Bit IO)\n"));

			printf("Memory Base:0x%x \tMemory Limit:0x%x\n",pciDevices[index].p2pMemBase,
							pciDevices[index].p2pMemLimit);

			printf("Pref Memory Base:0x%x \tPref Memory Limit:0x%x",
				   pciDevices[index].p2pPrefMemBase,
							pciDevices[index].p2pPrefMemLimit);

			(pciDevices[index].bPrefMem64)? (printf(" (64Bit PrefMem)\n")):
								(printf(" (32Bit PrefMem)\n"));
			if (pciDevices[index].bPrefMem64)
			{
				printf("Pref Base Upper 32bit:0x%x \tPref Limit Base Upper32 bit:0x%x\n",
								pciDevices[index].p2pPrefBaseUpper32Bits,
								pciDevices[index].p2pPrefLimitUpper32Bits);
			}
		}

	for (barIndex = 0 ; barIndex < pciDevices[index].barsNum ; barIndex++)
        {

	   if (pciDevices[index].pciBar[barIndex].barType == PCI_64BIT_BAR)
	   {
           printf("PCI_BAR%d (%s-%s) base: %x%08x%s",barIndex,
                  (pciDevices[index].pciBar[barIndex].barMapping == PCI_MEMORY_BAR)?"Mem":"I/O",
                  "64bit",
                  pciDevices[index].pciBar[barIndex].barBaseHigh,
                  pciDevices[index].pciBar[barIndex].barBaseLow,
                  (pciDevices[index].pciBar[barIndex].barBaseLow == 0)?"\t\t":"\t");
	   }
	   else if (pciDevices[index].pciBar[barIndex].barType == PCI_32BIT_BAR)
	   {
             printf("PCI_BAR%d (%s-%s) base: %x%s",barIndex,
                (pciDevices[index].pciBar[barIndex].barMapping == PCI_MEMORY_BAR)?"Mem":"I/O",
                "32bit",
                pciDevices[index].pciBar[barIndex].barBaseLow,
                (pciDevices[index].pciBar[barIndex].barBaseLow == 0)?"\t\t\t":"\t\t");
           }
	
         if(pciDevices[index].pciBar[barIndex].barSizeHigh != 0)
            printf("size: %d%08d bytes\n",pciDevices[index].pciBar[barIndex].barSizeHigh,
                                         pciDevices[index].pciBar[barIndex].barSizeLow);
         else
            printf("size: %d bytes\n", pciDevices[index].pciBar[barIndex].barSizeLow);
        }
    }
    return MV_TRUE;
}


#ifdef ARUBA_ARM
MV_BOOL
aruba_scan_pex(MV_U32 host)
{
    MV_U32    index,numOfElements=4*8; //,barIndex;
    MV_PCI_DEVICE      pciDevices[4*8]; //3 slots and us,Max 8 functions per slot

    memset (&pciDevices,0,12*sizeof(MV_PCI_DEVICE));

    if (mvPciScan(host, pciDevices , &numOfElements) != MV_OK )
    {
	DB(printf("scanPci:mvPciScan failed for host %d \n",host));
	return MV_FALSE;
    }

//    printf(" scanning...\n");
    printf("       bus.dev fn venID devID class  rev    MBAR0    MBAR1    MBAR2    MBAR3\n");
    
    for(index = 0; index < numOfElements ; index++) {
	printf("       %02d.%02d   %02x  %04x  %04x",
	    pciDevices[index].busNumber,
            pciDevices[index].deviceNum,
	    pciDevices[index].function,
            pciDevices[index].venID,
            pciDevices[index].deviceID);

        printf(" %05x   %02x",pciDevices[index].baseClassCode,
		pciDevices[index].revisionID);

	printf(" %08Lx %08Lx %08Lx %08Lx\n", 
		pciDevices[index].pciBar[0].barBaseAddr,
		pciDevices[index].pciBar[1].barBaseAddr,
		pciDevices[index].pciBar[2].barBaseAddr,
		pciDevices[index].pciBar[3].barBaseAddr);

#if 0
		/* check if we are bridge*/
		if ((pciDevices[index].baseClassCode == PCI_BRIDGE_CLASS)&&
			(pciDevices[index].subClassCode == P2P_BRIDGE_SUB_CLASS_CODE))
		{
			printf("Primary Bus:0x%x \tSecondary Bus:0x%x \tSubordinate Bus:0x%x\n",
							pciDevices[index].p2pPrimBusNum,
							pciDevices[index].p2pSecBusNum,
							pciDevices[index].p2pSubBusNum);
			
			printf("IO Base:0x%x \t\tIO Limit:0x%x",pciDevices[index].p2pIObase,
							pciDevices[index].p2pIOLimit);

			(pciDevices[index].bIO32)? (printf(" (32Bit IO)\n")):
								(printf(" (16Bit IO)\n"));
			printf("Memory Base:0x%x \tMemory Limit:0x%x\n",pciDevices[index].p2pMemBase,
							pciDevices[index].p2pMemLimit);

			printf("Pref Memory Base:0x%x \tPref Memory Limit:0x%x",
				   pciDevices[index].p2pPrefMemBase,
							pciDevices[index].p2pPrefMemLimit);

			(pciDevices[index].bPrefMem64)? (printf(" (64Bit PrefMem)\n")):
								(printf(" (32Bit PrefMem)\n"));
			if (pciDevices[index].bPrefMem64)
			{
				printf("Pref Base Upper 32bit:0x%x \tPref Limit Base Upper32 bit:0x%x\n",
								pciDevices[index].p2pPrefBaseUpper32Bits,
								pciDevices[index].p2pPrefLimitUpper32Bits);
			}
		}

	for (barIndex = 0 ; barIndex < pciDevices[index].barsNum ; barIndex++)
        {

	   if (pciDevices[index].pciBar[barIndex].barType == PCI_64BIT_BAR)
	   {
           printf("PCI_BAR%d (%s-%s) base: %x%08x%s",barIndex,
                  (pciDevices[index].pciBar[barIndex].barMapping == PCI_MEMORY_BAR)?"Mem":"I/O",
                  "64bit",
                  pciDevices[index].pciBar[barIndex].barBaseHigh,
                  pciDevices[index].pciBar[barIndex].barBaseLow,
                  (pciDevices[index].pciBar[barIndex].barBaseLow == 0)?"\t\t":"\t");
	   }
	   else if (pciDevices[index].pciBar[barIndex].barType == PCI_32BIT_BAR)
	   {
             printf("PCI_BAR%d (%s-%s) base: %x%s",barIndex,
                (pciDevices[index].pciBar[barIndex].barMapping == PCI_MEMORY_BAR)?"Mem":"I/O",
                "32bit",
                pciDevices[index].pciBar[barIndex].barBaseLow,
                (pciDevices[index].pciBar[barIndex].barBaseLow == 0)?"\t\t\t":"\t\t");
           }
	
         if(pciDevices[index].pciBar[barIndex].barSizeHigh != 0)
            printf("size: %d%08d bytes\n",pciDevices[index].pciBar[barIndex].barSizeHigh,
                                         pciDevices[index].pciBar[barIndex].barSizeLow);
         else
            printf("size: %d bytes\n", pciDevices[index].pciBar[barIndex].barSizeLow);
        }
#endif
    }
    return MV_TRUE;
}

void
aruba_pex_report(int bus)
{
	if (bus >= mvCtrlPexMaxIfGet()) {
		printf("PEX %d doesn't exist\n",bus);
		return;
	}
	aruba_scan_pex(bus);
}
#endif
int sp_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	MV_U32 host = 0;


        if (argc > 1) {
                host = simple_strtoul (argv[1], NULL, 10);
        }
	if(host >= mvCtrlPexMaxIfGet()){ 
		printf("PCI %d doesn't exist\n",host);		
		return 1;
	}
	if( scanPci(host) == MV_FALSE)
		printf("PCI %d Scan - FAILED!!.\n",host);
	return 1;
}
	
U_BOOT_CMD(
	sp,      2,     1,      sp_cmd,
	"sp	- Scan PCI bus.\n",
	" [0/1] \n"
	"\tScan and detecect all devices on mvPCI bus 0/1 \n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

int me_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        MV_U32 host = 0;
        if (argc > 1) 
	{
                host = simple_strtoul (argv[1], NULL, 10);
        }

		if(host >= mvCtrlPexMaxIfGet()) 
		{
			printf("Master %d doesn't exist\n",host); 
			return 1;
		} 

        if(mvPexMasterEnable(host,MV_TRUE)  == MV_OK)
                printf("PCI %d Master enabled.\n",host);
        else
                printf("PCI %d Master enabled -FAILED!!\n",host);
 
        return 1;
}
 
U_BOOT_CMD(
        me,      2,      1,      me_cmd,
        "me	- PCI master enable\n",
        " [0/1] \n"
        "\tEnable the MV device as Master on PCI 0/1. \n"
);

int se_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        MV_U32 host=0,dev = 0,bus=0;

	if(argc != 4)
	{
                printf ("Usage:\n%s\n", cmdtp->usage);
                return 1;
	}
        host = simple_strtoul (argv[1], NULL, 10);
        bus = simple_strtoul (argv[2], NULL, 16);
        dev = simple_strtoul (argv[3], NULL, 16);

        if(host >= mvCtrlPexMaxIfGet())
		{ 
			printf("PCI %d doesn't exist\n",host); 
			return 1;
		} 
        if(mvPexSlaveEnable(host,bus,dev,MV_TRUE) == MV_OK )
                printf("PCI %d Bus %d Slave 0x%x enabled.\n",host,bus,dev);
        else
                printf("PCI %d Bus %d Slave 0x%x enabled - FAILED!!.\n",host,bus,dev);
        return 1;
}
         
U_BOOT_CMD(
        se,      4,     1,      se_cmd,
        "se	- PCI Slave enable\n",
        " [0/1] bus dev \n"
        "\tEnable the PCI device as Slave on PCI 0/1. \n"
);

/******************************************************************************
* Functionality- The commands changes the pci remap register and displays the
*                address to be used in order to access PCI 0.
*****************************************************************************/
int mapPci_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    MV_ADDR_WIN pciWin;
	MV_TARGET target=0;
    MV_U32 host=0,effectiveBaseAddress=0;

    pciWin.baseLow=0;
    pciWin.baseHigh=0;
 
    if (argc > 1) {
        host = simple_strtoul(argv[1], NULL, 10);
    }
    if(argc > 2) {
        pciWin.baseLow = simple_strtoul(argv[2], NULL, 16);
    }
    if(host >= mvCtrlPexMaxIfGet()){
 	printf("PCI %d doesn't exist\n",host);
	return 1;
    }

    target = PCI0_MEM0 + (2 * host);

    printf("mapping pci %x to address 0x%x\n",host,pciWin.baseLow);
	
#if defined(MV_INCLUDE_PEX) || defined(MV_INCLUDE_PCI)
	effectiveBaseAddress = mvCpuIfPexRemap(target,&pciWin);
#endif

	if ( effectiveBaseAddress == 0xffffffff)
	{
		printf("Error remapping\n");
		return 1;
	}

    printf("PCI %x Access base address : %x\n",host,effectiveBaseAddress);
    return 1;
}
 
U_BOOT_CMD(
        mp,      3,     1,      mapPci_cmd,
        "mp	- map PCI BAR\n",
        " [0/1] address \n"
        "\tChange the remap of PCI 0/1 window 0 to address 'addrress'.\n"
        "\tIt also displays the new access address, since the remap is not always\n"
        "\tthe same as requested. \n"
);

#endif

MV_U32 mv_mem_ctrl_dev(MV_U32 pexIf, MV_U32 bus,MV_U32 dev)
{
	MV_U32 ven, class;


	ven =    mvPexConfigRead(pexIf,bus,dev,0,PCI_VENDOR_ID) & 0xffff;
	class = (mvPexConfigRead(pexIf,bus,dev,0,PCI_REVISION_ID) >> 16 ) & 0xffff;
	/* if we got any other Marvell PCI cards ignore it. */
	if(((ven == 0x11ab) && (class == PCI_CLASS_MEMORY_OTHER))||
	  ((ven == 0x11ab) && (class == PCI_CLASS_BRIDGE_HOST)))
	{
		return 1;
	}
	return 0;
}

static int mv_read_config_dword(struct pci_controller *hose,
				pci_dev_t dev,
				int offset, u32* value)
{
	MV_U32 bus,func,regOff,dev_no;
	char *env;

	bus = PCI_BUS(dev);
	dev_no = PCI_DEV(dev);

	func = (MV_U32)PCI_FUNC(dev);
	regOff = (MV_U32)offset & REG_NUM_MASK;

	/*  We will scan only ourselves and the PCI slots that exist on the 
		board, because we may have a case that we have one slot that has
		a Cardbus connector, and because CardBus answers all IDsels we want
		to scan only this slot and ourseleves.
	*/
	
	if( bus_scan == 1 )
	{
		env = getenv("disaMvPnp");
		if(env && ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
		{
			if( mv_mem_ctrl_dev((MV_U32)hose->cfg_addr, bus, dev_no) )
			{
				*value = 0xffffffff;
				return 0;
			}
		}
	}
	DB(printf("mv_read_config_dword hose->cfg_addr %x\n",hose->cfg_addr);)
	DB(printf("mv_read_config_dword bus %x\n",bus);)
	DB(printf("mv_read_config_dword dev_no %x\n",dev_no);)
	DB(printf("mv_read_config_dword func %x\n",func);)
	DB(printf("mv_read_config_dword regOff %x\n",regOff);)

	*value = (u32) mvPexConfigRead((MV_U32)hose->cfg_addr,bus,dev_no,func,regOff);
	
	DB(printf("mv_read_config_dword value %x\n",*value);)

	return 0;
}

static int mv_write_config_dword(struct pci_controller *hose,
				 pci_dev_t dev,
				 int offset, u32 value)
{
	MV_U32 bus,func,regOff,dev_no;

	bus = PCI_BUS(dev);
	dev_no = PCI_DEV(dev);
	func = (MV_U32)PCI_FUNC(dev);
	regOff = offset & REG_NUM_MASK;
	mvPexConfigWrite((MV_U32)hose->cfg_addr,bus,dev_no,func,regOff,value);

	return 0;
}



static void mv_setup_ide(struct pci_controller *hose,
			 pci_dev_t dev, struct pci_config_table *entry)
{
    static const int ide_bar[]={8,4,8,4,16,1024};
    u32 bar_response, bar_value;
    int bar;

    for (bar=0; bar<6; bar++)
    {
	/*ronen different function for 3rd bank.*/
	unsigned int offset = (bar < 2)? bar*8: 0x100 + (bar-2)*8;

	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0 + offset, 0x0);
	pci_read_config_dword(dev, PCI_BASE_ADDRESS_0 + offset, &bar_response);

	pciauto_region_allocate(bar_response & PCI_BASE_ADDRESS_SPACE_IO ?
				hose->pci_io : hose->pci_mem, ide_bar[bar], &bar_value);

	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0 + bar*4, bar_value);
    }
}

static void mv_setup_host(struct pci_controller *hose,
			 pci_dev_t dev, struct pci_config_table *entry)
{
	//skip our host
	DB(printf("skipping :bus=%x dev=%x fun=%x\n",
			  (unsigned int)PCI_BUS(dev),
			  (unsigned int)PCI_DEV(dev),
			  (unsigned int)PCI_FUNC(dev)));
	return;
}


static void mv_pci_bus_mode_display(MV_U32 host)
{
	
	#if defined(MV_INCLUDE_PEX)
	
	MV_PEX_MODE pexMode;
	if (mvPexModeGet(host,&pexMode) != MV_OK)
	{
		printf("mv_pci_bus_mode_display: mvPexModeGet failed\n");
	}
	switch (pexMode.pexType)
	{
	case MV_PEX_ROOT_COMPLEX:
		printf("PEX %d: RC",host);
		break;
	case MV_PEX_END_POINT:
		printf("PEX %d: EP",host);
		break;
	}

	if (!(pexMode.pexLinkUp))
	{
		printf(", no link.\n");
	}
	else
	{
		if (MV_PEX_WITDH_X1 ==  pexMode.pexWidth)
		{
		printf(", link up, x1\n");
		}
		else
		{
		printf(", link up, x4\n");
		}
	}
	
	return ;

	#endif /* MV_INCLUDE_PEX */
}

struct pci_config_table mv_config_table[] = {
    { PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_STORAGE_IDE,
      PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, mv_setup_ide},

    { PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID,
      PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, mv_setup_host}, //PCI host

    {}

};


/* Defines for more modularity of the pci_init_board function */

struct pci_controller  pci_hose[8];


//#if (MV_PEX_MAX_IF == 2)

#define PCI_IF_MEM(pexIf)	((pexIf==0)?PCI_IF0_MEM0:PCI_IF1_MEM0)
#define PCI_IF_REMAPED_MEM_BASE(pexIf) ((pexIf==0)?PCI_IF0_REMAPED_MEM_BASE:PCI_IF1_REMAPED_MEM_BASE)
#define PCI_IF_MEM_BASE(pexIf)	((pexIf==0)?PCI_IF0_MEM0_BASE:PCI_IF1_MEM0_BASE)
#define PCI_IF_MEM_SIZE(pexIf)	((pexIf==0)?PCI_IF0_MEM0_SIZE:PCI_IF1_MEM0_SIZE)
#define PCI_IF_IO_BASE(pexIf)	((pexIf==0)?PCI_IF0_IO_BASE:PCI_IF1_IO_BASE)
#define PCI_IF_IO_SIZE(pexIf)	((pexIf==0)?PCI_IF0_IO_SIZE:PCI_IF1_IO_SIZE)
/*
#else

#define PCI_IF_MEM(pexIf)	(PCI_IF0_MEM0)
#define PCI_IF_REMAPED_MEM_BASE(pexIf) (PCI_IF0_REMAPED_MEM_BASE)
#define PCI_IF_MEM_BASE(pexIf)	(PCI_IF0_MEM0_BASE)
#define PCI_IF_MEM_SIZE(pexIf)	(PCI_IF0_MEM0_SIZE)
#define PCI_IF_IO_BASE(pexIf)	(PCI_IF0_IO_BASE)
#define PCI_IF_IO_SIZE(pexIf)	(PCI_IF0_IO_SIZE)

#endif*/

/* because of CIV team needs we are gonna do a remap to PCI memory */
#define PCI_IF0_REMAPED_MEM_BASE	PEX0_MEM_BASE
#define PCI_IF1_REMAPED_MEM_BASE	PEX1_MEM_BASE


void
pci_init_board(void)
{
	MV_U32 pexIfNum = mvCtrlPexMaxIfGet();
	MV_U32 pexIf=0;
	MV_ADDR_WIN rempWin;
	char *env;
	int first_busno=0;
	int status;
    	MV_CPU_DEC_WIN  cpuAddrDecWin;
	PCI_IF_MODE	pexIfMode = PCI_IF_MODE_HOST;
	int pexIfStart = 0;
	MV_U32 gppGrp, gppBit;
	MV_32 gppNum;

	if(pexIfNum == 0)
		return;
	env = getenv("disaMvPnp");

	if(env && ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
		printf("Warning: skip configuration of Marvell devices!!!\n");

	gppNum = mvBoarGpioPinNumGet(BOARD_GPP_PEX_RESET, 0);
	if (gppNum > 0) {
		gppGrp = gppNum >> 5;
		gppBit = 1 << (gppNum & 0x1F);

		/* walk around for PEX power failure */
		mvGppTypeSet(gppGrp, gppBit, (MV_GPP_OUT & gppBit));
		mvGppValueSet(gppGrp, gppBit, gppBit);
		mvOsDelay(100);
		mvGppValueSet(gppGrp, gppBit, 0);
		mvOsDelay(100);
		mvGppValueSet(gppGrp, gppBit, gppBit);
		/* Wait for some time to allow card to reset before initializing the
		 ** Pex interface.
		 */
		mvOsDelay(1000);
	}

#if defined(MV_INCLUDE_PEX) && defined(MV78XX0)
	/* Power down the none lanes 0.1, 0.2, and 0.3 if PEX0 is X4 */
	if ( !(PCI0_IS_QUAD_X1) )
	{
	    for (pexIf = 1; pexIf < 4; pexIf++)
		mvCtrlPwrClckSet(PEX_UNIT_ID, pexIf, MV_FALSE);
	}

	/* Power down the none lanes 1.1, 1.2, and 1.3 if PEX1 is X4 */
	if ( !(PCI1_IS_QUAD_X1) )
	{
	    for (pexIf = 5; pexIf < 8; pexIf++)
		mvCtrlPwrClckSet(PEX_UNIT_ID, pexIf, MV_FALSE);
	}
#endif

	/* start pci scan */
	for (pexIf = pexIfStart; pexIf < pexIfNum; pexIf++)
	{
		pci_hose[pexIf].config_table = mv_config_table;
		if (MV_FALSE == mvCtrlPwrClckGet(PEX_UNIT_ID, pexIf)) 
		{
			continue;
		}
		/* device or host ? */
#if defined(MV_INCLUDE_PEX)
#if !defined(MV_88F6183) && !defined(MV_88F6183L) && !defined(MV_88F6082) && !defined(MV88F6281) && \
            !defined(MV88F6192) && !defined(MV88F6180)  && !defined(MV88F6190) && !defined(MV88F6282) && \
	    !defined(MV88F6510) && !defined(MV88F6530) && !defined(MV88F6550) && !defined(MV88F6560)
		MV_PEX_MODE pexMode;
		if (mvPexModeGet(pexIf,&pexMode) != MV_OK)
		{
			printf("pci_init_board: mvPexModeGet failed (%d)\n", pexIf);
		}
		if (MV_PEX_ROOT_COMPLEX == pexMode.pexType)	
		    pexIfMode = MV_PEX_ROOT_COMPLEX;
		else
		    pexIfMode = MV_PEX_END_POINT;

#else
		/* Set pex mode incase S@R not exist */
		env = getenv("pexMode");
		if( env && ( ((strcmp(env,"EP") == 0) || (strcmp(env,"ep") == 0) )))
		    pexIfMode = MV_PEX_END_POINT;
		else
		    pexIfMode = MV_PEX_ROOT_COMPLEX;
#endif
#endif

		if ((status = mvSysPexInit(pexIf, pexIfMode)) == MV_ERROR) {
			printf("pci_init_board:Error calling mvPexIfInit for pexIf %d\n",pexIf);
		} else {
			if (status == MV_OK)
				mv_pci_bus_mode_display(pexIf);
			else
			{
				/* Interface with no link */
				printf("PEX %d: Detected no link.\n", pexIf);
//				mvCtrlPwrClckSet(PEX_UNIT_ID, pexIf, MV_FALSE);
				continue;
			}
		}

		/* start Uboot PCI scan */
		if (pexIf == 0)
			pci_hose[pexIf].first_busno = 0;
		else
			pci_hose[pexIf].first_busno = pci_hose[pexIf-1].last_busno + 1;

		/* start Uboot PCI scan */
		pci_hose[pexIf].current_busno = pci_hose[pexIf].first_busno;
		pci_hose[pexIf].last_busno = 0xff;

		if (mvPexLocalBusNumSet(pexIf,pci_hose[pexIf].first_busno) != MV_OK)
		{
			printf("pci_init_board:Error calling mvPexLocalBusNumSet for pexIf %d\n",pexIf);	
		}

		/* If no link on the interface it will not be scan */
		if (status == MV_NO_SUCH)
		{
			pci_hose[pexIf].last_busno =pci_hose[pexIf].first_busno;
			continue;
		}

#ifdef PCI_DIS_INTERFACE
		/* The disable interface will not be scan */
		if (pexIf == PCI_DIS_INTERFACE)
		{
			printf("***Interface is disable***\n");	
			pci_hose[pexIf].last_busno =pci_hose[pexIf].first_busno;
			continue;
		}
#endif
		if (MV_OK != mvCpuIfTargetWinGet(PCI_MEM(pexIf, 0), &cpuAddrDecWin))
		{
			printf("%s: ERR. mvCpuIfTargetWinGet failed\n", __FUNCTION__);
			return ;
		}

		rempWin.baseLow = ((cpuAddrDecWin.addrWin.baseLow & 0x0fffffff) | PCI_IF_REMAPED_MEM_BASE(pexIf));
		rempWin.baseHigh = 0;

		/* perform a remap for the PEX0 interface*/
		if (0xffffffff == mvCpuIfPexRemap(PCI_MEM(pexIf, 0),&rempWin)) {
			printf("%s:mvCpuIfPexRemap failed\n",__FUNCTION__);
			return;
		}

		/* PCI memory space */
		pci_set_region(pci_hose[pexIf].regions + 0,
				rempWin.baseLow, /* bus address */
				cpuAddrDecWin.addrWin.baseLow,
				cpuAddrDecWin.addrWin.size,
				PCI_REGION_MEM);

		if (MV_OK != mvCpuIfTargetWinGet(PCI_IO(pexIf), &cpuAddrDecWin))
		{
			/* No I/O space */
			pci_hose[pexIf].region_count = 1;
		}
		else
		{
			/* PCI I/O space */
			pci_set_region(pci_hose[pexIf].regions + 1,
				    cpuAddrDecWin.addrWin.baseLow,
				    cpuAddrDecWin.addrWin.baseLow,
				    cpuAddrDecWin.addrWin.size,
				    PCI_REGION_IO);
			pci_hose[pexIf].region_count = 2;
		}
		pci_set_ops(&pci_hose[pexIf],
				pci_hose_read_config_byte_via_dword,
				pci_hose_read_config_word_via_dword,
				mv_read_config_dword,
				pci_hose_write_config_byte_via_dword,
				pci_hose_write_config_word_via_dword,
				mv_write_config_dword);

		pci_hose[pexIf].cfg_addr = (unsigned int*) pexIf;

		pci_hose[pexIf].config_table[1].bus = mvPexLocalBusNumGet(pexIf);
		pci_hose[pexIf].config_table[1].dev = mvPexLocalDevNumGet(pexIf);

		pci_register_hose(&pci_hose[pexIf]);

		if (pexIfMode == PCI_IF_MODE_HOST)
		{
			MV_U32 pciData=0,baseClassCode=0,subClassCode;
			
			bus_scan = 1;

			pci_hose[pexIf].last_busno = pci_hose_scan(&pci_hose[pexIf]);
			bus_scan = 0;

			pciData = mvPexConfigRead(pexIf,pci_hose[pexIf].first_busno,1,0, PCI_CLASS_CODE_AND_REVISION_ID);
	
			baseClassCode = (pciData & PCCRIR_BASE_CLASS_MASK) >> PCCRIR_BASE_CLASS_OFFS;
			subClassCode = 	(pciData & PCCRIR_SUB_CLASS_MASK) >> PCCRIR_SUB_CLASS_OFFS;

			if ((baseClassCode == PCI_BRIDGE_CLASS) && 
			   (subClassCode == P2P_BRIDGE_SUB_CLASS_CODE))
			{
				mvPexConfigWrite(pexIf,pci_hose[pexIf].first_busno,1,0,P2P_MEM_BASE_LIMIT,pciData);
				/* In rthe bridge : We want to open its memory and
				IO to the maximum ! after the u-boot Scan */
				/* first the IO */
				pciData  = mvPexConfigRead(pexIf,pci_hose[pexIf].first_busno,1,0,P2P_IO_BASE_LIMIT_SEC_STATUS);
				/* keep the secondary status */
				pciData  &= PIBLSS_SEC_STATUS_MASK;
				/* set to the maximum - 0 - 0xffff */
				pciData  |= 0xff00;
				mvPexConfigWrite(pexIf,pci_hose[pexIf].first_busno,1,0,P2P_IO_BASE_LIMIT_SEC_STATUS,pciData);

				/* the the Memory */
				pciData  = mvPexConfigRead(pexIf,pci_hose[pexIf].first_busno,1,0,P2P_MEM_BASE_LIMIT);
				/* set to the maximum - PCI_IF_REMAPED_MEM_BASE(pexIf) - 0xf000000 */
				pciData  = 0xEFF00000 | (PCI_IF_REMAPED_MEM_BASE(pexIf) >> 16);
				mvPexConfigWrite(pexIf,pci_hose[pexIf].first_busno,1,0,P2P_MEM_BASE_LIMIT,pciData);
			}
		}
		else /* PCI_IF_MODE_HOST */
		{
			pci_hose[pexIf].last_busno = pci_hose[pexIf].first_busno;
		}

		first_busno = pci_hose[pexIf].last_busno + 1;
#ifdef CONFIG_APBOOT
		aruba_pex_report(pexIf);
#endif
	}
#ifdef DB_FPGA
	MV_REG_BIT_RESET(PCI_BASE_ADDR_ENABLE_REG(0), BIT10);
#endif
}

#endif /* CONFIG_PCI */

