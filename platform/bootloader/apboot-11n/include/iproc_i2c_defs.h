/*****************************************************************************
Copyright 2012 Broadcom Corporation.  All rights reserved.
This program is the proprietary software of Broadcom Corporation and/or its 
licensors, and may only be used, duplicated, modified or distributed pursuant to
the terms and conditions of a separate, written license agreement executed 
between you and Broadcom (an "Authorized License").  Except as set forth in an 
Authorized License, Broadcom grants no license (express or implied), right to 
use, or waiver of any kind with respect to the Software, and Broadcom expressly
reserves all rights in and to the Software and all intellectual property rights
therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL
USE OF THE SOFTWARE.  
 
Except as expressly set forth in the Authorized License,
 
1.     This program, including its structure, sequence and organization, 
constitutes the valuable trade secrets of Broadcom, and you shall use all 
reasonable efforts to protect the confidentiality thereof, and to use this 
information only in connection with your use of Broadcom integrated circuit 
products.
 
2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR 
CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
PERFORMANCE OF THE SOFTWARE.

3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE 
OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY
PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED 
REMEDY.
*****************************************************************************/
#ifndef __IPROC_SMBUS_DEFS_H__
#define __IPROC_SMBUS_DEFS_H__

/* Transaction error codes defined in Master command register (0x30) */
#define MSTR_STS_XACT_SUCCESS          0
#define MSTR_STS_LOST_ARB              1
#define MSTR_STS_NACK_FIRST_BYTE       2
#define MSTR_STS_NACK_NON_FIRST_BYTE   3 /* NACK on a byte other than 
                                            the first byte */
#define MSTR_STS_TTIMEOUT_EXCEEDED     4
#define MSTR_STS_TX_TLOW_MEXT_EXCEEDED 5
#define MSTR_STS_RX_TLOW_MEXT_EXCEEDED 6

/* SMBUS protocol values defined in register 0x30 */
#define SMBUS_PROT_QUICK_CMD               0 
#define SMBUS_PROT_SEND_BYTE               1 
#define SMBUS_PROT_RECV_BYTE               2 
#define SMBUS_PROT_WR_BYTE                 3 
#define SMBUS_PROT_RD_BYTE                 4 
#define SMBUS_PROT_WR_WORD                 5 
#define SMBUS_PROT_RD_WORD                 6 
#define SMBUS_PROT_BLK_WR                  7 
#define SMBUS_PROT_BLK_RD                  8 
#define SMBUS_PROT_PROC_CALL               9 
#define SMBUS_PROT_BLK_WR_BLK_RD_PROC_CALL 10 

#define BUS_BUSY_COUNT                 100000 /* Number can be changed later */

#endif /* __IPROC_SMBUS_DEFS_H__ */
