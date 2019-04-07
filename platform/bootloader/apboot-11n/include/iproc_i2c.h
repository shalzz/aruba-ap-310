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
#ifndef __IPROC_SMBUS_H__
#define __IPROC_SMBUS_H__

#define IPROC_I2C_INVALID_ADDR 0xFF

#define IPROC_SMB_MAX_RETRIES   35

#define I2C_SMBUS_BLOCK_MAX     1024

#define GETREGFLDVAL(regval, mask, startbit) (((regval) & (mask)) >> (startbit))

#define SETREGFLDVAL(regval, fldval, mask, startbit) regval = \
                                                      (regval & ~(mask)) | \
                                                      ((fldval) << (startbit))

/* Enum to specify clock speed. The user will provide it during initialization.
 * If needed, it can be changed dynamically
 */
typedef enum iproc_smb_clk_freq {
    I2C_SPEED_100KHz = 0,
    I2C_SPEED_400KHz = 1,
    I2C_SPEED_INVALID = 255
} smb_clk_freq_t;

/* This enum will be used to notify the user of status of a data transfer
 * request 
 */
typedef enum iproc_smb_error_code {
    I2C_NO_ERR = 0,
    I2C_TIMEOUT_ERR = 1,
    I2C_INVALID_PARAM_ERR = 2, /* Invalid parameter(s) passed to the driver */
    I2C_OPER_IN_PROGRESS = 3, /* The driver API was called before the present
                                 transfer was completed */
    I2C_OPER_ABORT_ERR = 4, /* Transfer aborted unexpectedly, for example a NACK
                               received, before last byte was read/written */
    I2C_FUNC_NOT_SUPPORTED = 5, /* Feature or function not supported 
	                               (e.g., 10-bit addresses, or clock speeds
                                   other than 100KHz, 400KHz) */
} iproc_smb_error_code_t;


/* Structure used to pass information to read/write functions. */
struct iproc_xact_info {
    unsigned char command; /* Passed by caller to send SMBus command code */
    unsigned char *data; /* actual data pased by the caller */
    unsigned int size; /* Size of data buffer passed */
    unsigned short flags; /* Sent by caller specifying PEC, 10-bit addresses */
    unsigned char smb_proto; /* SMBus protocol to use to perform transaction */
    unsigned int cmd_valid; /* true if command field below is valid. Otherwise, false */
};

#endif /* __IPROC_SMBUS_H__ */
