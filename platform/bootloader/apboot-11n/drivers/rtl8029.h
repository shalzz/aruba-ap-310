/*
 * Realtek 8029AS Ethernet
 * (C) Copyright 2002-2003
 * Xue Ligong(lgxue@hotmail.com),Wang Kehao, ESLAB, whut.edu.cn
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
 */

/*
 * This code works in 8bit mode.
 * If you need to work in 16bit mode, PLS change it!
 */

#include <asm/types.h>
#include <config.h>


#ifdef CONFIG_DRIVER_RTL8029

#define RTL8029_BASE rtl8029_dev->iobase

#define		RTL8029_REG_00        		(RTL8029_BASE + 0x00)
#define 	RTL8029_REG_01        		(RTL8029_BASE + 0x01)
#define 	RTL8029_REG_02        		(RTL8029_BASE + 0x02)
#define 	RTL8029_REG_03        		(RTL8029_BASE + 0x03)
#define 	RTL8029_REG_04        		(RTL8029_BASE + 0x04)
#define 	RTL8029_REG_05        		(RTL8029_BASE + 0x05)
#define 	RTL8029_REG_06        		(RTL8029_BASE + 0x06)
#define 	RTL8029_REG_07        		(RTL8029_BASE + 0x07)
#define 	RTL8029_REG_08        		(RTL8029_BASE + 0x08)
#define 	RTL8029_REG_09        		(RTL8029_BASE + 0x09)
#define 	RTL8029_REG_0a        		(RTL8029_BASE + 0x0a)
#define 	RTL8029_REG_0b        		(RTL8029_BASE + 0x0b)
#define 	RTL8029_REG_0c        		(RTL8029_BASE + 0x0c)
#define 	RTL8029_REG_0d        		(RTL8029_BASE + 0x0d)
#define 	RTL8029_REG_0e       	 	(RTL8029_BASE + 0x0e)
#define 	RTL8029_REG_0f        		(RTL8029_BASE + 0x0f)
#define 	RTL8029_REG_10        		(RTL8029_BASE + 0x10)
#define 	RTL8029_REG_1f        		(RTL8029_BASE + 0x1f)

#define		RTL8029_COMMAND			RTL8029_REG_00
#define		RTL8029_PAGESTART		RTL8029_REG_01
#define		RTL8029_PAGESTOP		RTL8029_REG_02
#define		RTL8029_BOUNDARY		RTL8029_REG_03
#define		RTL8029_TRANSMITSTATUS		RTL8029_REG_04
#define		RTL8029_TRANSMITPAGE		RTL8029_REG_04
#define		RTL8029_TRANSMITBYTECOUNT0	RTL8029_REG_05
#define		RTL8029_NCR 			RTL8029_REG_05
#define		RTL8029_TRANSMITBYTECOUNT1 	RTL8029_REG_06
#define		RTL8029_INTERRUPTSTATUS		RTL8029_REG_07
#define		RTL8029_CURRENT 		RTL8029_REG_07
#define		RTL8029_REMOTESTARTADDRESS0 	RTL8029_REG_08
#define		RTL8029_CRDMA0  		RTL8029_REG_08
#define		RTL8029_REMOTESTARTADDRESS1 	RTL8029_REG_09
#define		RTL8029_CRDMA1 			RTL8029_REG_09
#define		RTL8029_REMOTEBYTECOUNT0	RTL8029_REG_0a
#define		RTL8029_REMOTEBYTECOUNT1	RTL8029_REG_0b
#define		RTL8029_RECEIVESTATUS		RTL8029_REG_0c
#define		RTL8029_RECEIVECONFIGURATION	RTL8029_REG_0c
#define		RTL8029_TRANSMITCONFIGURATION	RTL8029_REG_0d
#define		RTL8029_FAE_TALLY 		RTL8029_REG_0d
#define		RTL8029_DATACONFIGURATION	RTL8029_REG_0e
#define		RTL8029_CRC_TALLY 		RTL8029_REG_0e
#define		RTL8029_INTERRUPTMASK		RTL8029_REG_0f
#define		RTL8029_MISS_PKT_TALLY		RTL8029_REG_0f
#define		RTL8029_PHYSICALADDRESS0	RTL8029_REG_01
#define 	RTL8029_PHYSICALADDRESS1	RTL8029_REG_02
#define		RTL8029_PHYSICALADDRESS2	RTL8029_REG_03
#define		RTL8029_PHYSICALADDRESS3	RTL8029_REG_04
#define		RTL8029_PHYSICALADDRESS4	RTL8029_REG_05
#define		RTL8029_PHYSICALADDRESS5	RTL8029_REG_06
#define		RTL8029_MULTIADDRESS0		RTL8029_REG_08
#define		RTL8029_MULTIADDRESS1		RTL8029_REG_09
#define		RTL8029_MULTIADDRESS2		RTL8029_REG_0a
#define		RTL8029_MULTIADDRESS3		RTL8029_REG_0b
#define		RTL8029_MULTIADDRESS4		RTL8029_REG_0c
#define		RTL8029_MULTIADDRESS5		RTL8029_REG_0d
#define		RTL8029_MULTIADDRESS6		RTL8029_REG_0e
#define		RTL8029_MULTIADDRESS7		RTL8029_REG_0f
#define		RTL8029_DMA_DATA		RTL8029_REG_10
#define		RTL8029_RESET			RTL8029_REG_1f


#define 	RTL8029_PAGE0               	0x22
#define   	RTL8029_PAGE1               	0x62
#define   	RTL8029_PAGE0DMAWRITE       	0x12
#define   	RTL8029_PAGE2DMAWRITE       	0x92
#define   	RTL8029_REMOTEDMAWR         	0x12
#define   	RTL8029_REMOTEDMARD         	0x0A
#define   	RTL8029_ABORTDMAWR          	0x32
#define   	RTL8029_ABORTDMARD          	0x2A
#define   	RTL8029_PAGE0STOP           	0x21
#define   	RTL8029_PAGE1STOP           	0x61
#define   	RTL8029_TRANSMIT            	0x26
#define   	RTL8029_TXINPROGRESS        	0x04
#define   	RTL8029_SEND		    	0x1A

#define		RTL8029_PSTART			0x4c
#define		RTL8029_PSTOP			0x80
#define		RTL8029_TPSTART			0x40


#endif /*end of CONFIG_DRIVER_RTL8029*/
