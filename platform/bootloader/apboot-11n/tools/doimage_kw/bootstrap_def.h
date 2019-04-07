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

#ifndef _INC_BOOTSTRAP__DEF_H
#define _INC_BOOTSTRAP__DEF_H
#include "bootstrap_os.h"

#ifndef MV_ASMLANGUAGE

typedef struct BHR_t
{
//  	type        	name                	byte order
    	MV_U8       	blockID;            	//0
    	MV_U8       	nandEccMode;   		//1
    	MV_U16      	nandPageSize;           //2-3
    	MV_U32      	blockSize;          	//4-7
    	MV_U32      	rsvd1;         		//8-11
    	MV_U32      	sourceAddr;         	//12-15
    	MV_U32      	destinationAddr;    	//16-19
    	MV_U32		executionAddr;      	//20-23
    	MV_U8       	sataPioMode;     	//24
    	MV_U8       	rsvd3;		        //25
    	MV_U16      	ddrInitDelay;      	//26-27
    	MV_U16     	rsvd2;              	//28-29
    	MV_U8       	ext;              	//30
    	MV_U8       	checkSum;           	//31
} BHR_t, * pBHR_t;


typedef struct ExtBHR_t
{
//  	type        	name                	byte order
	MV_U32 		dramRegsOffs;		//0-3
	MV_U32		rsrvd1;			//4-7
	MV_U32 		rsrvd2;			//8-11	
	MV_U32 		rsrvd3;			//12-15	
	MV_U32		rsrvd4;			//16-19	
	MV_U32		rsrvd5;			//20-23	
	MV_U32		rsrvd6;			//24-27	
    	MV_U16      	rsrvd7;			//28-29
    	MV_U8       	rsrvd8;			//30
   	MV_U8       	checkSum;		//31
}ExtBHR_t, *pExtBHR_t;

#define BOOTROM_SIZE			(12 * 1024)
#define HEADER_SIZE			512
#define BHR_HDR_SIZE        		0x20
#define EXT_HEADER_SIZE			(HEADER_SIZE - BHR_HDR_SIZE)

/* Boot Type - block ID */
#define IBR_HDR_I2C_ID			0x4D
#define IBR_HDR_SPI_ID    		0x5A
#define IBR_HDR_NAND_ID     		0x8B
#define IBR_HDR_SATA_ID     		0x78
#define IBR_HDR_PEX_ID			0x9C
#define IBR_HDR_UART_ID     		0x69
#define IBR_DEF_ATTRIB      		0x00

#endif /* MV_ASMLANGUAGE */
#endif /* _INC_BOOTSTRAP_H */

