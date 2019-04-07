/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

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
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "mvOs.h"
#include "mvSwitch.h"
#include "eth-phy/mvEthPhy.h"
#include "mvSwitchRegs.h"
#include "mvCtrlEnvLib.h"
#include "mvBoardEnvLib.h"
#include <common.h>
#include <command.h>


static void switchVlanInit(MV_U32 ethPortNum,
						   MV_U32 switchCpuPort,
					   MV_U32 switchMaxPortsNum,
					   MV_U32 switchPortsOffset,
					   MV_U32 switchEnabledPortsMask);

void mvEthSwitchRegWrite(MV_U32 ethPortNum, MV_U32 phyAddr,
                                 MV_U32 regOffs, MV_U16 data);

void mvEthSwitchRegRead(MV_U32 ethPortNum, MV_U32 phyAddr,
                             MV_U32 regOffs, MV_U16 *data);

int switchMultiChipMode = 0xdeadbeef;

/*******************************************************************************
* mvEthE6065_61PhyBasicInit - 
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*       
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE6065_61SwitchBasicInit(MV_U32 ethPortNum)
{
	switchVlanInit(ethPortNum,
			   MV_E6065_CPU_PORT,
			   MV_E6065_MAX_PORTS_NUM,
			   MV_E6065_PORTS_OFFSET,
			   MV_E6065_ENABLED_PORTS);
}

/*******************************************************************************
* mvEthE6063PhyBasicInit - 
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*       
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE6063SwitchBasicInit(MV_U32 ethPortNum)
{
	switchVlanInit(ethPortNum,
			   MV_E6063_CPU_PORT,
			   MV_E6063_MAX_PORTS_NUM,
			   MV_E6063_PORTS_OFFSET,
			   MV_E6063_ENABLED_PORTS);
}

/*******************************************************************************
* mvEthE6131PhyBasicInit - 
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*       
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE6131SwitchBasicInit(MV_U32 ethPortNum)
{

	MV_U16 reg;

	/*Enable Phy power up*/
	mvEthPhyRegWrite (0,0,0x9140);
	mvEthPhyRegWrite (1,0,0x9140);
	mvEthPhyRegWrite (2,0,0x9140);


	/*Enable PPU*/
	mvEthPhyRegWrite (0x1b,4,0x4080);


	/*Enable Phy detection*/
	mvEthPhyRegRead (0x13,0,&reg);
	reg &= ~(1<<12);
	mvEthPhyRegWrite (0x13,0,reg);

	mvOsDelay(100);
	mvEthPhyRegWrite (0x13,1,0x33);


	switchVlanInit(ethPortNum,
			    MV_E6131_CPU_PORT,
			   MV_E6131_MAX_PORTS_NUM,
			   MV_E6131_PORTS_OFFSET,
			   MV_E6131_ENABLED_PORTS);

}


/*******************************************************************************
* mvEthE6161PhyBasicInit - 
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*       
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE6161SwitchBasicInit(MV_U32 ethPortNum)
{

    MV_U32 prt;
    MV_U16 reg;
    volatile MV_U32 timeout; 

    /* The 6161/5 needs a delay */
    mvOsDelay(1000);

    /* Init vlan */
    switchVlanInit(ethPortNum,
		    MV_E6161_CPU_PORT,
		    MV_E6161_MAX_PORTS_NUM,
		    MV_E6161_PORTS_OFFSET,
		    MV_E6161_ENABLED_PORTS);

    /* Enable RGMII delay on Tx and Rx for CPU port */
    mvEthSwitchRegWrite (ethPortNum, 0x14,0x1a,0x81e7);
    mvEthSwitchRegRead (ethPortNum, 0x15,0x1a,&reg);
    mvEthSwitchRegWrite (ethPortNum, 0x15,0x1a,0x18);
    mvEthSwitchRegWrite (ethPortNum, 0x14,0x1a,0xc1e7);

    for(prt=0; prt < MV_E6161_MAX_PORTS_NUM; prt++)
    {
	if (prt != MV_E6161_CPU_PORT)
	{
	    /*Enable Phy power up*/
	    mvEthSwitchRegWrite (ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR, 
				MV_E6161_SMI_PHY_DATA, 0x3360);
	    mvEthSwitchRegWrite (ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR, 
				MV_E6161_SMI_PHY_COMMAND, (0x9410 | (prt << 5)));

	    /*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	    timeout = E6161_PHY_TIMEOUT;
	    do
            {
	     	mvEthSwitchRegRead(ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR,
				MV_E6161_SMI_PHY_COMMAND,&reg);
		if(timeout-- == 0)
		{
		 	mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	    }while (reg & E6161_PHY_SMI_BUSY_MASK);   

	    mvEthSwitchRegWrite (ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR,
				MV_E6161_SMI_PHY_DATA,0x1140);
	    mvEthSwitchRegWrite (ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR,
				MV_E6161_SMI_PHY_COMMAND,(0x9400 | (prt << 5)));

	    /*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	    timeout = E6161_PHY_TIMEOUT;
	    do
            {
	     	mvEthSwitchRegRead(ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR,
				MV_E6161_SMI_PHY_COMMAND,&reg);
		if(timeout-- == 0)
		{
		 	mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	    }while (reg & E6161_PHY_SMI_BUSY_MASK);   

	}

	/*Enable port*/
	mvEthSwitchRegWrite (ethPortNum, MV_E6161_PORTS_OFFSET + prt, 4, 0x7f);
    }

    /*Force CPU port to RGMII FDX 1000Base*/
    mvEthSwitchRegWrite (ethPortNum, MV_E6161_PORTS_OFFSET + MV_E6161_CPU_PORT, 1, 0x3e);
}
#if defined(MV_KW2)

/*******************************************************************************
* mvEthKW2SwitchBasicInit - 
*
* DESCRIPTION: KW2 internal SW init.
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID mvEthKW2SwitchBasicInit(MV_U32 enabledPorts)
{
/*    MV_U32 prt;*/
/*    MV_U16 reg;*/
	MV_U32 ethPortNum;
	MV_U32 cpuPort;
	//MV_U32 enabledPorts = ((1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 4)|(1 << 5)); //by default 0-4 are enabled

	/* The 6171 needs a delay */
	mvOsDelay(1000);
	if (mvBoardIsInternalSwitchConnected(0))
	{
		ethPortNum = 0;
		cpuPort = 4;

		/* Force link, speed, duplex for switch port #4. */
		mvEthSwitchRegWrite(0, 0x14, 0x1, 0x3E);
	}
	else 
	{
		//switch is connected in port 1 only
		ethPortNum = 1;
		cpuPort = 5;
		/* Force link, speed, duplex for switch port #4. */
		mvEthSwitchRegWrite(1, 0x15, 0x1, 0x3E);

	}
//	enabledPorts |= (1 << cpuPort);

	/* Init vlan of switch 1 and enable all ports */
#if 1
	switchVlanInit(ethPortNum,
			cpuPort,
			MV_KW2_SW_MAX_PORTS_NUM,
			MV_KW2_SW_PORTS_OFFSET,
			enabledPorts);
#endif
	/* delay only CPU port*/
	/* Enable RGMII delay on Tx and Rx for port 5 switch 1 */
//	mvEthSwitchRegRead (ethPortNum, MV_KW2_SW_PORTS_OFFSET + cpuPort, MV_KW2_SW_SWITCH_PHIYSICAL_CTRL_REG, &reg);
//	mvEthSwitchRegWrite (ethPortNum, MV_KW2_SW_PORTS_OFFSET + cpuPort, MV_KW2_SW_SWITCH_PHIYSICAL_CTRL_REG, (reg|0xC000));
#if 0
	/* Power up PHYs */
	for(prt=0; prt < MV_KW2_SW_MAX_PORTS_NUM; prt++)
	{
		if ((1 << prt) & enabledPorts)
		{
			/*Enable Phy power up for switch 1*/
			mvEthSwitchRegWrite (ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
					MV_KW2_SW_SMI_PHY_DATA, 0x3360);
			mvEthSwitchRegWrite (ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
					MV_KW2_SW_SMI_PHY_COMMAND, (0x9410 | (prt << 5)));

			/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
			timeout = KW2_SW_PHY_TIMEOUT;
			do {
				mvEthSwitchRegRead(ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
						MV_KW2_SW_SMI_PHY_COMMAND,&reg);
				if(timeout-- == 0)
				{
					mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
					return;
				}
			}while (reg & KW2_SW_PHY_SMI_BUSY_MASK);

			mvEthSwitchRegWrite (ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
					MV_KW2_SW_SMI_PHY_DATA,0x1140);
			mvEthSwitchRegWrite (ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
					MV_KW2_SW_SMI_PHY_COMMAND,(0x9400 | (prt << 5)));
	
			/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
			timeout = KW2_SW_PHY_TIMEOUT;
			do {
				mvEthSwitchRegRead(ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
						MV_KW2_SW_SMI_PHY_COMMAND,&reg);
				if(timeout-- == 0)
				{
					mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
					return;
				}
			}while (reg & KW2_SW_PHY_SMI_BUSY_MASK);
		}
	}
#endif
	if (mvBoardIsInternalSwitchConnected(0))
	{
		/* Force link, speed, duplex for switch port #4. */
		mvEthSwitchRegWrite(0, 0x14, 0x1, 0x3E);
	}
	else 
	{
		/* Force link, speed, duplex for switch port #4. */
		mvEthSwitchRegWrite(1, 0x15, 0x1, 0x3E);
	}
}
#endif
static void switchVlanInit(MV_U32 ethPortNum,
						   MV_U32 switchCpuPort,
					   MV_U32 switchMaxPortsNum,
					   MV_U32 switchPortsOffset,
					   MV_U32 switchEnabledPortsMask)
{
	MV_U32 prt;
	MV_U16 reg;
#if 0
	/* be sure all ports are disabled */
    	for(prt=0; prt < switchMaxPortsNum; prt++)
	{
		mvEthSwitchRegRead (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
							  MV_SWITCH_PORT_CONTROL_REG,&reg);
		reg &= ~0x3;
		mvEthSwitchRegWrite (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
							  MV_SWITCH_PORT_CONTROL_REG,reg);
	}

	/* Set CPU port VID to 0x1 */
	mvEthSwitchRegRead (ethPortNum, MV_SWITCH_PORT_OFFSET(switchCpuPort),
						  MV_SWITCH_PORT_VID_REG,&reg);
	reg &= ~0xfff;
	reg |= 0x1;
	mvEthSwitchRegWrite (ethPortNum, MV_SWITCH_PORT_OFFSET(switchCpuPort),
						  MV_SWITCH_PORT_VID_REG,reg);

	/* Setting  Port default priority for all ports to zero */
    	for(prt=0; prt < switchMaxPortsNum; prt++)
	{
		mvEthSwitchRegRead (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
							  MV_SWITCH_PORT_VID_REG,&reg);
		reg &= ~0xc000;
		mvEthSwitchRegWrite (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
							  MV_SWITCH_PORT_VID_REG,reg);
	}

	/* Setting VID and VID map for all ports except CPU port */
    	for(prt=0; prt < switchMaxPortsNum; prt++)
	{
		/* only for enabled ports */
        	if ((1 << prt)& switchEnabledPortsMask)
		{
			/* skip CPU port */
			if (prt== switchCpuPort)
				continue;

			/* 
			*  set Ports VLAN Mapping.
			*	port prt <--> MV_SWITCH_CPU_PORT VLAN #prt+1.
			*/
	
			mvEthSwitchRegRead (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
								  MV_SWITCH_PORT_VID_REG,&reg);
			reg &= ~0x0fff;
			reg |= (prt+1);
			mvEthSwitchRegWrite (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
								  MV_SWITCH_PORT_VID_REG,reg);
	
	
			/* Set Vlan map table for all ports to send only to MV_SWITCH_CPU_PORT */
			mvEthSwitchRegRead (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
								  MV_SWITCH_PORT_VMAP_REG,&reg);
			reg &= ~((1 << switchMaxPortsNum) - 1);
			reg |= (1 << switchCpuPort);
			mvEthSwitchRegWrite (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
								  MV_SWITCH_PORT_VMAP_REG,reg);
		}

	}

	/* Set Vlan map table for MV_SWITCH_CPU_PORT to see all ports*/
	mvEthSwitchRegRead (ethPortNum, MV_SWITCH_PORT_OFFSET(switchCpuPort),
						  MV_SWITCH_PORT_VMAP_REG,&reg);
	reg &= ~((1 << switchMaxPortsNum) - 1);
	reg |= switchEnabledPortsMask & ~(1 << switchCpuPort);
	mvEthSwitchRegWrite (ethPortNum, MV_SWITCH_PORT_OFFSET(switchCpuPort),
						  MV_SWITCH_PORT_VMAP_REG,reg);
#endif


    	/*enable only appropriate ports to forwarding mode - and disable the others*/
    	for(prt=0; prt < switchMaxPortsNum; prt++)
	{
        	if ((1 << prt)& switchEnabledPortsMask)
		{
			mvEthSwitchRegRead (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
								  MV_SWITCH_PORT_CONTROL_REG,&reg);
			reg |= 0x3;
			mvEthSwitchRegWrite (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
								  MV_SWITCH_PORT_CONTROL_REG,reg);
		}
		else
		{
			/* Disable port */
			mvEthSwitchRegRead (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
								  MV_SWITCH_PORT_CONTROL_REG,&reg);
			reg &= ~0x3;
			mvEthSwitchRegWrite (ethPortNum, MV_SWITCH_PORT_OFFSET(prt),
								  MV_SWITCH_PORT_CONTROL_REG,reg);
		}
	}
	return;
}

#ifdef ARUBA_MARVELL_SWITCH
#define SWITCH_PHY_ADDR(x) 0x8
#else
#define SWITCH_PHY_ADDR(x) mvBoardPhyAddrGet(x)
#endif
void mvEthSwitchRegWrite(MV_U32 ethPortNum, MV_U32 phyAddr,
                                 MV_U32 regOffs, MV_U16 data)
{
	MV_U16 reg;

	if(switchMultiChipMode == 0xdeadbeef) {
		mvEthPhyRegRead(SWITCH_PHY_ADDR(ethPortNum) ,0x2, &reg);
		if(reg == 0xffff)
			switchMultiChipMode = SWITCH_PHY_ADDR(ethPortNum);
		else
			switchMultiChipMode = 0xffffffff;
	}

	if(switchMultiChipMode == 0xffffffff)
		mvEthPhyRegWrite(phyAddr, regOffs, data);
	else //If Switch is in multichip mode, need to use indirect register access
	{
		do {
			mvEthPhyRegRead(switchMultiChipMode, 0x0, &reg);
		} while((reg & BIT15));    // Poll till SMIBusy bit is clear
		mvEthPhyRegWrite(switchMultiChipMode, 0x1, data);   // Write data to Switch indirect data register
		mvEthPhyRegWrite(switchMultiChipMode, 0x0, regOffs | (phyAddr << 5) |
				BIT10 | BIT12 | BIT15);   // Write command to Switch indirect command register
	}
}

void mvEthSwitchRegRead(MV_U32 ethPortNum, MV_U32 phyAddr,
                             MV_U32 regOffs, MV_U16 *data)
{
	MV_U16 reg;

	switchMultiChipMode = SWITCH_PHY_ADDR(ethPortNum);

	if(switchMultiChipMode == 0xdeadbeef) {
		mvEthPhyRegRead(SWITCH_PHY_ADDR(ethPortNum),0x2, &reg);
		if(reg == 0xffff)
			switchMultiChipMode = SWITCH_PHY_ADDR(ethPortNum);
		else
			switchMultiChipMode = 0xffffffff;
	}

	if(switchMultiChipMode == 0xffffffff)
		mvEthPhyRegRead(phyAddr, regOffs, data);
	else //If Switch is in multichip mode, need to use indirect register access
	{
		do {
			mvEthPhyRegRead(switchMultiChipMode, 0x0, &reg);
		} while((reg & BIT15));    // Poll till SMIBusy bit is clear
		mvEthPhyRegWrite(switchMultiChipMode, 0x0, regOffs | (phyAddr << 5) |
				BIT11 | BIT12 | BIT15);   // Write command to Switch indirect command register
		do {
			mvEthPhyRegRead(switchMultiChipMode, 0, &reg);
		} while((reg & BIT15));    // Poll till SMIBusy bit is clear
		mvEthPhyRegRead(switchMultiChipMode, 0x1, data);   // Write data to Switch indirect data register
	}
}

extern void mvBoardEthComplexConfigSet(MV_U32 ethConfig); // wnc, 20100914_M_Ellis
void mvEthSwitchPhyRegWrite(MV_U32 ethPortNum, MV_U16 prt,
                                 MV_U16 regOffs, MV_U16 data)
{
	MV_U16 reg;
	volatile MV_U32 timeout;
	// wnc, 20100914_M_Ellis -->
#ifdef ARUBA_MARVELL_SWITCH_notyet
	MV_U32 ethCompOpt;
		ethCompOpt = mvBoardEthComplexConfigGet();
		if(ethCompOpt & ESC_OPT_QSGMII)
		{
			  mvEthPhyRegWrite(prt , regOffs, data);
			  return ; 

		}
#endif	
	// <-- Ellis
	/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	timeout = KW2_SW_PHY_TIMEOUT;
	do {
		mvEthSwitchRegRead(ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
				MV_KW2_SW_SMI_PHY_COMMAND,&reg);
		if(timeout-- == 0)
		{
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	}while (reg & KW2_SW_PHY_SMI_BUSY_MASK);

	mvEthSwitchRegWrite (ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
			MV_KW2_SW_SMI_PHY_DATA, data);

	mvEthSwitchRegWrite (ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
			MV_KW2_SW_SMI_PHY_COMMAND,(0x9400 | (prt << 5) | regOffs));

	/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	timeout = KW2_SW_PHY_TIMEOUT;
	do {
		mvEthSwitchRegRead(ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
				MV_KW2_SW_SMI_PHY_COMMAND,&reg);
		if(timeout-- == 0)
		{
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	}while (reg & KW2_SW_PHY_SMI_BUSY_MASK);

}

void mvEthSwitchPhyRegRead(MV_U32 ethPortNum, MV_U16 prt,
                             MV_U16 regOffs, MV_U16 *data)
{
	MV_U16 reg;
	volatile MV_U32 timeout;
	// wnc, 20100914_M_Ellis -->
#ifdef ARUBA_MARVELL_SWITCH_notyet
	MV_U32 ethCompOpt;

		ethCompOpt = mvBoardEthComplexConfigGet();
		if(ethCompOpt & ESC_OPT_QSGMII)
		{
			  mvEthPhyRegRead(prt , regOffs, data);
			  return ; 
			
		}
#endif
	// <-- Ellis

	/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	timeout = KW2_SW_PHY_TIMEOUT;
	do {
		mvEthSwitchRegRead(ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
				MV_KW2_SW_SMI_PHY_COMMAND,&reg);
		if(timeout-- == 0)
		{
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	}while (reg & KW2_SW_PHY_SMI_BUSY_MASK);

	mvEthSwitchRegWrite (ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
			MV_KW2_SW_SMI_PHY_COMMAND,(0x9800 | (prt << 5) | regOffs));

	/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	timeout = KW2_SW_PHY_TIMEOUT;
	do {
		mvEthSwitchRegRead(ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
				MV_KW2_SW_SMI_PHY_COMMAND,&reg);
		if(timeout-- == 0)
		{
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	}while (reg & KW2_SW_PHY_SMI_BUSY_MASK);

	mvEthSwitchRegRead(ethPortNum, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR,
				MV_KW2_SW_SMI_PHY_DATA, data);
}

#ifdef CONFIG_MV6560_INTERNAL_SWITCH
/* also enables 10K frame MTU */
static void
aruba_6560_disable_translation_on_port(unsigned port)
{
	int switchPortsOffset = MV_KW2_SW_PORTS_OFFSET;
	MV_U16 data;

	mvEthSwitchRegRead(0, MV_SWITCH_PORT_OFFSET(port), 
		MV_SWITCH_PORT_CONTROL2_REG, &data);
	data &= ~(1 << 7);		/* disable address learning */
	data &= ~(3 << 12);		/* clear previous MTU */
	data |= (2 << 12);		/* set 10K MTU */
	mvEthSwitchRegWrite(0, MV_SWITCH_PORT_OFFSET(port), 
		MV_SWITCH_PORT_CONTROL2_REG, data);
}

/* also disables learning on the port */
static void
aruba_6560_set_port_destination(unsigned port, unsigned dest)
{
	int switchPortsOffset = MV_KW2_SW_PORTS_OFFSET;

	mvEthSwitchRegWrite(0, MV_SWITCH_PORT_OFFSET(port), 
		MV_SWITCH_PORT_VMAP_REG, (1 << dest) | (1 << 11));
}
#endif

#ifdef CONFIG_PORTLOOP_TEST
static void
aruba_6560_set_port_loopback(unsigned port)
{
	int switchPortsOffset = MV_KW2_SW_PORTS_OFFSET;
	MV_U16 dest = port;
	MV_U16 data;

	mvEthSwitchRegRead(0, MV_SWITCH_PORT_OFFSET(port), 
		MV_SWITCH_PORT_VMAP_REG, &data);

	printf("orig 0x%x\n", data);
	data &= 0xf780;

	data |= (1 << dest);

	mvEthSwitchRegWrite(0, MV_SWITCH_PORT_OFFSET(port), 
		MV_SWITCH_PORT_VMAP_REG, data);

	mvEthSwitchRegRead(0, MV_SWITCH_PORT_OFFSET(port), 
		MV_SWITCH_PORT_VMAP_REG, &data);

	printf("final 0x%x\n", data);
}
#endif

//#define DUMP_SWITCH_REGS

#ifdef CONFIG_ARRAN
void
arran_6560_switch_core_init(void)
{
#ifdef DUMP_SWITCH_REGS
	int switchPortsOffset = MV_KW2_SW_PORTS_OFFSET;
	MV_U16 data;
	int i, j;
#endif

	aruba_6560_set_port_destination(1, 4);
	aruba_6560_set_port_destination(4, 1);

	aruba_6560_set_port_destination(0, 5);
	aruba_6560_set_port_destination(5, 0);

	aruba_6560_disable_translation_on_port(0);
	aruba_6560_disable_translation_on_port(1);
	aruba_6560_disable_translation_on_port(4);
	aruba_6560_disable_translation_on_port(5);

#ifdef DUMP_SWITCH_REGS
	for (i = 0; i < 0x7; i++) {
		for (j = 0; j < 0x14; j++) {
			mvEthSwitchRegRead(0, MV_SWITCH_PORT_OFFSET(i), j, &data);
			printf("port %u reg %x, val %x\n", i, j, data & 0xffff);
		}
	}
	for (i = 0; i < 0x1f; i++) {
		mvEthSwitchRegRead(0, 0x1b, i, &data);
		printf("global 1 reg %x, val %x\n", i, data & 0xffff);
	}
	for (i = 0; i < 0x1f; i++) {
		mvEthSwitchRegRead(0, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR, i, &data);
		printf("global 2 reg %x, val %x\n", i, data & 0xffff);
	}
#endif
}
#endif

#ifdef CONFIG_BOWMORE
/* XXX; update this for bowmore config */
void
bowmore_6560_switch_core_init(void)
{
#ifdef DUMP_SWITCH_REGS
	int switchPortsOffset = MV_KW2_SW_PORTS_OFFSET;
	MV_U16 data;
	int i, j;
#endif

	aruba_6560_set_port_destination(1, 4);
	aruba_6560_set_port_destination(4, 1);

	aruba_6560_set_port_destination(0, 5);
	aruba_6560_set_port_destination(5, 0);

	aruba_6560_disable_translation_on_port(0);
	aruba_6560_disable_translation_on_port(1);
	aruba_6560_disable_translation_on_port(4);
	aruba_6560_disable_translation_on_port(5);

#ifdef DUMP_SWITCH_REGS
	for (i = 0; i < 0x7; i++) {
		for (j = 0; j < 0x14; j++) {
			mvEthSwitchRegRead(0, MV_SWITCH_PORT_OFFSET(i), j, &data);
			printf("port %u reg %x, val %x\n", i, j, data & 0xffff);
		}
	}
	for (i = 0; i < 0x1f; i++) {
		mvEthSwitchRegRead(0, 0x1b, i, &data);
		printf("global 1 reg %x, val %x\n", i, data & 0xffff);
	}
	for (i = 0; i < 0x1f; i++) {
		mvEthSwitchRegRead(0, MV_KW2_SW_GLOBAL_2_REG_DEV_ADDR, i, &data);
		printf("global 2 reg %x, val %x\n", i, data & 0xffff);
	}
#endif
}
#endif

#ifdef CONFIG_PORTLOOP_TEST
int
portloopcmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int port = simple_strtoul(argv[1], (char **)0, 10);

	aruba_6560_set_port_loopback(port);
	return 0;
}


U_BOOT_CMD(
	portloop,      2,    2,      portloopcmd,
	"portloop	- loop back a switch core port\n",
	" \n"
	"\t\n"
);
#endif
