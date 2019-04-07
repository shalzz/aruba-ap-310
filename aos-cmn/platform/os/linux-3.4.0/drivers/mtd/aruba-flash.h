#ifndef __aruba_flash_h__
#define __aruba_flash_h__
/**************************************************************************
* Copyright (C)2007 Spansion LLC. All Rights Reserved . 
*
* This software is owned and published by: 
* Spansion LLC, 915 DeGuigne Dr. Sunnyvale, CA  94088-3453 ("Spansion").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND 
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software constitutes driver source code for use in programming Spansion's 
* Flash memory components. This software is licensed by Spansion to be adapted only 
* for use in systems utilizing Spansion's Flash memories. Spansion is not be 
* responsible for misuse or illegal use of this software for devices not 
* supported herein.  Spansion is providing this source code "AS IS" and will 
* not be responsible for issues arising from incorrect user implementation 
* of the source code herein.  
*
* SPANSION MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE, 
* REGARDING THE SOFTWARE, ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED 
* USE, INCLUDING, WITHOUT LIMITATION, NO IMPLIED WARRANTY OF MERCHANTABILITY, 
* FITNESS FOR A  PARTICULAR PURPOSE OR USE, OR NONINFRINGEMENT.  SPANSION WILL 
* HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT, NEGLIGENCE OR 
* OTHERWISE) FOR ANY DAMAGES ARISING FROM USE OR INABILITY TO USE THE SOFTWARE, 
* INCLUDING, WITHOUT LIMITATION, ANY DIRECT, INDIRECT, INCIDENTAL, 
* SPECIAL, OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA, SAVINGS OR PROFITS, 
* EVEN IF SPANSION HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  
*
* This software may be replicated in part or whole for the licensed use, 
* with the restriction that this Copyright notice must be included with 
* this software, whether used in part or whole, at all times.  
*/
typedef enum {
    LLD_P_POLL_NONE = 0,        /* pull program status */
    LLD_P_POLL_PGM,             /* pull program status */
    LLD_P_POLL_WRT_BUF_PGM,     /* Poll write buffer   */
    LLD_P_POLL_SEC_ERS,         /* Poll sector erase   */
    LLD_P_POLL_CHIP_ERS,        /* Poll chip erase     */
    LLD_P_POLL_RESUME,
    LLD_P_POLL_BLANK            /* Poll device sector blank check */
} POLLING_TYPE;

typedef enum {
    DEV_STATUS_UNKNOWN = 0,
    DEV_NOT_BUSY,
    DEV_BUSY,
    DEV_EXCEEDED_TIME_LIMITS,
    DEV_SUSPEND,
    DEV_WRITE_BUFFER_ABORT,
    DEV_STATUS_GET_PROBLEM,
    DEV_VERIFY_ERROR,
    DEV_BYTES_PER_OP_WRONG,
    DEV_ERASE_ERROR,
    DEV_PROGRAM_ERROR,
    DEV_SECTOR_LOCK
} DEVSTATUS;

#ifdef CONFIG_ARDMORE
/* x16 as x16 */
#define LLD_DEV_MULTIPLIER 0x00000001
#define LLD_DB_READ_MASK   0x0000FFFF
#define LLD_DEV_READ_MASK  0x0000FFFF
#define LLD_UNLOCK_ADDR1   0x00000AAA
#define LLD_UNLOCK_ADDR2   0x00000554
#define LLD_BYTES_PER_OP   0x00000002
typedef unsigned short FLASHDATA;
#else
/* x8 as x8 */
#define LLD_DEV_MULTIPLIER 0x00000001
#define LLD_DB_READ_MASK   0x000000FF
#define LLD_DEV_READ_MASK  0x000000FF
#define LLD_UNLOCK_ADDR1   0x00000555
#define LLD_UNLOCK_ADDR2   0x000002AA
#define LLD_BYTES_PER_OP   0x00000001
typedef unsigned char FLASHDATA;
#endif

typedef unsigned char BYTE;     /* 8 bits wide */
typedef unsigned short LLD_UINT16;      /* 16 bits wide */
typedef unsigned long LLD_UINT32;       /* 32 bits wide */
typedef LLD_UINT32 ADDRESS;     /* Used for system level addressing */
typedef unsigned int WORDCOUNT; /* used for multi-byte operations */

extern DEVSTATUS lld_ProgramOp
(
FLASHDATA * base_addr,      /* device base address is system */
ADDRESS offset,         /* address offset from base address */
FLASHDATA write_data    /* variable containing data to program */
);

extern DEVSTATUS lld_memcpy(
    FLASHDATA * base_addr,      /* device base address is system */
    ADDRESS offset,             /* address offset from base address */
    WORDCOUNT word_cnt,         /* number of words to program */
    FLASHDATA * data_buf        /* buffer containing data to program */
);

extern DEVSTATUS lld_SectorEraseOp(
    FLASHDATA * base_addr,      /* device base address is system */
    ADDRESS offset              /* address offset from base address */
);

extern void lld_set_buffer_size(unsigned);

#endif /* __aruba_flash_h__ */
