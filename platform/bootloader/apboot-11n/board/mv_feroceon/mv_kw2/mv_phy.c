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

#include "mvBoardEnvSpec.h"
#include "mvBoardEnvLib.h"
#include "mv_phy.h"
#include "mvSwitch.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"
#include "mvSysEthPhyApi.h"

extern void mvEthSwitchPhyRegWrite(MV_U32 ethPortNum, MV_U16 prt,
                                 MV_U16 regOffs, MV_U16 data);

extern void mvEthSwitchPhyRegRead(MV_U32 ethPortNum, MV_U16 prt,
                             MV_U16 regOffs, MV_U16 *data);

extern MV_STATUS mvSysEthPhyInit(void );

MV_VOID mvEthInternal3FEPhyBasicInit(MV_U32 port)
{
	int i;
	MV_U16 reg;

	//switchPhyRegWrite 0 1 0x1D 9
	mvEthSwitchPhyRegWrite(port, 1, 0x1D, 9);

	//switchPhyRegWrite 0 1 0x1F  bits[6:5] change to 0
	//switchPhyRegWrite 0 2 0x1F  bits[6:5] change to 0
	//switchPhyRegWrite 0 3 0x1F  bits[6:5] change to 0
	for (i=1; i<4; i++) {
		mvEthSwitchPhyRegRead(port, i, 0x1F, &reg);
		reg &= ~(BIT6 | BIT5);
		mvEthSwitchPhyRegWrite(port, i, 0x1F, reg);
	}

	//switchPhyRegWrite 0 1 0x1D 10
	mvEthSwitchPhyRegWrite(port, 1, 0x1D, 0x10);

	//switchPhyRegWrite 0 1 0x1E bit [13] change to 0
	//switchPhyRegWrite 0 2 0x1E bit [13] change to 0
	//switchPhyRegWrite 0 3 0x1E bit [13] change to 0
	for (i=1; i<4; i++) {
		mvEthSwitchPhyRegRead(port, i, 0x1E, &reg);
		reg &= ~(BIT13);
		mvEthSwitchPhyRegWrite(port, i, 0x1E, reg);
	}

	//switchPhyRegWrite 0 1 0x1D 0
	mvEthSwitchPhyRegWrite(port, 1, 0x1D, 0);

	for (i=1; i<4; i++) {
		mvEthSwitchPhyRegWrite(port, i, 0x1C, 0xF03);

		/* soft reset the phy */
		mvEthSwitchPhyRegRead(port, i, 0, &reg);
		reg |= BIT15;
		mvEthSwitchPhyRegWrite(port, i, 0, reg);
	}
}

MV_VOID mvSWE1116PhyBasicInit(MV_U32 port, MV_U32 ethComplex)
{
	MV_U16 reg;
	MV_U16 swPort;

	if (ethComplex & ESC_OPT_RGMIIA_SW_P5)
		swPort = 5;
	else if (ethComplex & ESC_OPT_RGMIIA_SW_P6)
		swPort = 6;
	else
		return;

	/* Leds link and activity*/
	mvEthSwitchPhyRegWrite(port, swPort, 22, 0x3);
	mvEthSwitchPhyRegRead(port, swPort, 16, &reg);
	reg &= ~0xf;
	reg	|= 0x1;
	mvEthSwitchPhyRegWrite(port, swPort, 16, reg);
	mvEthSwitchPhyRegWrite(port, swPort, 22, 0x0);

	/* Set RGMII delay */
	mvEthSwitchPhyRegWrite(port, swPort, 22, 2);
	mvEthSwitchPhyRegRead(port, swPort, 21, &reg);
	reg	|= (BIT5 | BIT4);
	mvEthSwitchPhyRegWrite(port, swPort, 21, reg);
	mvEthSwitchPhyRegWrite(port, swPort, 22, 0);

	/* reset the phy */
	mvEthSwitchPhyRegRead(port, swPort, 0, &reg);
	reg |= BIT15;
	mvEthSwitchPhyRegWrite(port, swPort, 0, reg);
}

MV_VOID mvEthSWInternalGEPhyBasicInit(MV_U32 port, MV_U32 ethComplex)
{
	MV_U16 value;
	MV_U16 swPort;

	if (ethComplex & ESC_OPT_GEPHY_SW_P0)
		swPort = 0;
	else if (ethComplex & ESC_OPT_GEPHY_SW_P5)
		swPort = 5;
	else
		return;

	/* Force 10/100 mode */
	//mvEthSwitchPhyRegRead(port, 4, 9, &value);
	//value &= ~(BIT8 | BIT9);
	//mvEthSwitchPhyRegWrite(port, 4, 9, value);

	mvEthSwitchPhyRegWrite(port, swPort, 0x16, 0x00FF);
	mvEthSwitchPhyRegWrite(port, swPort, 0x11, 0x0FD0);
	mvEthSwitchPhyRegWrite(port, swPort, 0x10, 0x214C);
	mvEthSwitchPhyRegWrite(port, swPort, 0x11, 0x0000);
	mvEthSwitchPhyRegWrite(port, swPort, 0x10, 0x2000);
	mvEthSwitchPhyRegWrite(port, swPort, 0x11, 0x0F16);
	mvEthSwitchPhyRegWrite(port, swPort, 0x10, 0x2146);
	mvEthSwitchPhyRegWrite(port, swPort, 0x16, 0x0);

	/* reset the phy */
	mvEthSwitchPhyRegRead(port, swPort, 0, &value);
	value |= BIT15;
	mvEthSwitchPhyRegWrite(port, swPort, 0, value);
	mvOsDelay(10);
}

/*********************************************************** 
* Init the PHY or Switch of the board 			   *
 ***********************************************************/
void mvBoardEgigaPhyInit(void) 
{
	MV_U32 ethComplex = mvBoardEthComplexConfigGet();
	MV_U32 portEnabled = 0;
	MV_U32 reg;
	MV_U32 i;
	char *env;

	mvSysEthPhyInit();

	if (ethComplex & (ESC_OPT_RGMIIA_MAC0 | ESC_OPT_RGMIIB_MAC0))
		mvEthPhyInit(0, MV_FALSE);

	if (ethComplex & (ESC_OPT_RGMIIA_MAC1 | ESC_OPT_GEPHY_MAC1))
		mvEthPhyInit(1, MV_FALSE);

	if (mvBoardIsInternalSwitchConnected(0) || mvBoardIsInternalSwitchConnected(1)) {
#ifdef CONFIG_APBOOT
		printf("LAN:   ");
#endif
		if (ethComplex & ESC_OPT_RGMIIA_SW_P5)
			portEnabled |= BIT5;
		if (ethComplex & ESC_OPT_RGMIIA_SW_P6)
			portEnabled |= BIT6;		
		if (ethComplex & ESC_OPT_MAC0_2_SW_P4)
			portEnabled |= BIT4;
		if (ethComplex & ESC_OPT_MAC1_2_SW_P5)
			portEnabled |= BIT5;
		if (ethComplex & ESC_OPT_GEPHY_SW_P0)
			portEnabled |= BIT0;
		if (ethComplex & ESC_OPT_GEPHY_SW_P5)
			portEnabled |= BIT5;
		if (ethComplex & ESC_OPT_FE3PHY)
			portEnabled |= BIT1 | BIT2 | BIT3;
		if (ethComplex & ESC_OPT_QSGMII)
			portEnabled |= BIT0 | BIT1 | BIT2 | BIT3;
		mvEthKW2SwitchBasicInit(portEnabled);

#ifdef CONFIG_APBOOT
		printf("done\n");
#endif
		if (ethComplex & ESC_OPT_QSGMII) {
			env = getenv("eeeEnable");
			if((!env) || (strcmp(env,"yes") != 0))
				mvEthPhyInit((MV_U32) -1, MV_FALSE);
			else
				mvEthPhyInit((MV_U32) -1, MV_TRUE);
		}

		if (ethComplex & ESC_OPT_FE3PHY) {
			if (mvBoardIsInternalSwitchConnected(0))
				mvEthInternal3FEPhyBasicInit(0);
			else
				mvEthInternal3FEPhyBasicInit(1);
		}

		if (ethComplex & (ESC_OPT_GEPHY_SW_P0 | ESC_OPT_GEPHY_SW_P5)) {
			if (mvBoardIsInternalSwitchConnected(0))
				mvEthSWInternalGEPhyBasicInit(0, ethComplex);
			else
				mvEthSWInternalGEPhyBasicInit(1, ethComplex);
		}

		if (mvBoardIdGet() == RD_88F6560_GW_ID) {
			/* Config LED Matrix. */
			reg = MV_REG_READ(LED_MATRIX_CTRL_REG(0));
			reg |= 0x3;
			MV_REG_WRITE(LED_MATRIX_CTRL_REG(0), reg);

			/* Set PHY led mode */
			for(i = 0; i < 4; i++) {
				mvEthPhyRegWrite(i, 0x16, 3);
				mvEthPhyRegWrite(i, 0x10, 0x1771);
				mvEthPhyRegWrite(i, 0x16, 0);
			}
			mvEthPhyRegWrite(9, 0x16, 3);
			mvEthPhyRegWrite(9, 0x10, 0x1771);
			mvEthPhyRegWrite(9, 0x16, 0);
		}

		if (ethComplex & (ESC_OPT_RGMIIA_SW_P5 | ESC_OPT_RGMIIA_SW_P6)) {
			if (mvBoardIsInternalSwitchConnected(0))
				mvSWE1116PhyBasicInit(0, ethComplex);
			else
				mvSWE1116PhyBasicInit(1, ethComplex);
		}
	}
}

/*
 * This is currently in 7-LED mode; if we switch to 12-LED mode, this needs work
 */
void
aruba_6560_ge_phy_init(void)
{
    unsigned reg;
    unsigned pid;

    pid = mvBoardPhyAddrGet(1);

#ifdef CONFIG_TAMDHU
    /*
     * experimental value that sort of gets the LEDs to work;
     * set all LED configurations to be driven by an internal signal
     * set LED 6&7 controlled by phy
     */
    MV_REG_WRITE(LED_MATRIX_CTRL_REG(1), 0x00b40000);
#endif
    /* Config LED Matrix. */
    reg = MV_REG_READ(LED_MATRIX_CTRL_REG(0));
#ifdef CONFIG_TAMDHU
    /* 12-LED mode for Tamdhu */
    reg = 0x1;
#else
    reg |= 0x3;
#endif
    MV_REG_WRITE(LED_MATRIX_CTRL_REG(0), reg);

#ifdef CONFIG_TAMDHU
    mvEthPhyRegWrite(pid, 0x16, 3);
    mvEthPhyRegWrite(pid, 0x11, 0x8805);
    mvEthPhyRegWrite(pid, 0x10, 0x7117);
    mvEthPhyRegWrite(pid, 0x16, 0);
#else
    mvEthPhyRegWrite(pid, 0x16, 3);
    mvEthPhyRegWrite(pid, 0x10, 0x1771);
    mvEthPhyRegWrite(pid, 0x16, 0);
#endif
}
