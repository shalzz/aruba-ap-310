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
#include <linux/kernel.h>
#include "aruba-flash.h"

#define NOR_AUTOSELECT_CMD               ((0x90)*LLD_DEV_MULTIPLIER)
#define NOR_CFI_QUERY_CMD                ((0x98)*LLD_DEV_MULTIPLIER)
#define NOR_CHIP_ERASE_CMD               ((0x10)*LLD_DEV_MULTIPLIER)
#define NOR_ERASE_SETUP_CMD              ((0x80)*LLD_DEV_MULTIPLIER)
#define NOR_PROGRAM_CMD                  ((0xA0)*LLD_DEV_MULTIPLIER)
#define NOR_RESET_CMD                    ((0xF0)*LLD_DEV_MULTIPLIER)
#define NOR_SECSI_SECTOR_ENTRY_CMD       ((0x88)*LLD_DEV_MULTIPLIER)
#define NOR_SECSI_SECTOR_EXIT_SETUP_CMD  ((0x90)*LLD_DEV_MULTIPLIER)
#define NOR_SECSI_SECTOR_EXIT_CMD        ((0x00)*LLD_DEV_MULTIPLIER)
#define NOR_SECTOR_ERASE_CMD             ((0x30)*LLD_DEV_MULTIPLIER)
#define NOR_UNLOCK_BYPASS_ENTRY_CMD      ((0x20)*LLD_DEV_MULTIPLIER)
#define NOR_UNLOCK_BYPASS_PROGRAM_CMD    ((0xA0)*LLD_DEV_MULTIPLIER)
#define NOR_UNLOCK_BYPASS_RESET_CMD1     ((0x90)*LLD_DEV_MULTIPLIER)
#define NOR_UNLOCK_BYPASS_RESET_CMD2     ((0x00)*LLD_DEV_MULTIPLIER)
#define NOR_UNLOCK_DATA1                 ((0xAA)*LLD_DEV_MULTIPLIER)
#define NOR_UNLOCK_DATA2                 ((0x55)*LLD_DEV_MULTIPLIER)
#define NOR_WRITE_BUFFER_ABORT_RESET_CMD ((0xF0)*LLD_DEV_MULTIPLIER)
#define NOR_WRITE_BUFFER_LOAD_CMD        ((0x25)*LLD_DEV_MULTIPLIER)
#define NOR_WRITE_BUFFER_PGM_CONFIRM_CMD ((0x29)*LLD_DEV_MULTIPLIER)
#define NOR_SUSPEND_CMD                  ((0xB0)*LLD_DEV_MULTIPLIER)
#define NOR_RESUME_CMD                   ((0x30)*LLD_DEV_MULTIPLIER)
#define NOR_SET_CONFIG_CMD               ((0xD0)*LLD_DEV_MULTIPLIER)
#define NOR_READ_CONFIG_CMD              ((0xC6)*LLD_DEV_MULTIPLIER)

static inline void
FLASH_WR(void *b, unsigned o, unsigned d)
{
    volatile FLASHDATA *offset = (void *)b + o;
//printk("b %p o %x d %04x, flo %p\n", b, o, d, offset);
    *offset = d;
}

static inline FLASHDATA
FLASH_RD(void *b, unsigned o)
{
        volatile FLASHDATA *offset = (void *)b + o;
	FLASHDATA v;

	v = *offset;
//printk("b %p o %x -> v %04x, flo %p\n", b, o, v, offset);
	return v;
}

#define DQ1_MASK   (0x02 * LLD_DEV_MULTIPLIER)  /* DQ1 mask for all interleave devices */
#define DQ2_MASK   (0x04 * LLD_DEV_MULTIPLIER)  /* DQ2 mask for all interleave devices */
#define DQ5_MASK   (0x20 * LLD_DEV_MULTIPLIER)  /* DQ5 mask for all interleave devices */
#define DQ6_MASK   (0x40 * LLD_DEV_MULTIPLIER)  /* DQ6 mask for all interleave devices */

#define DQ6_TGL_DQ1_MASK (dq6_toggles >> 5)     /* Mask for DQ1 when device DQ6 toggling */
#define DQ6_TGL_DQ5_MASK (dq6_toggles >> 1)     /* Mask for DQ5 when device DQ6 toggling */
unsigned int WriteBufferProgramming = 0;
int enable_status_cmd_g = 0;

/******************************************************************************
* 
* lld_ProgramCmd - Writes a Program Command to Flash Device
*
* This function only issues the Program Command sequence.
* Progran status polling is not implemented in this function.
*
*
* RETURNS: void
*
* ERRNO: 
*/
#ifndef REMOVE_LLD_PROGRAM_CMD
void lld_ProgramCmd
(
FLASHDATA * base_addr,               /* device base address in system */
ADDRESS offset,                  /* address offset from base address */
FLASHDATA *pgm_data_ptr          /* variable containing data to program */
)
{       

  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);

  /* Write Program Command */
  FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_PROGRAM_CMD);
  /* Write Data */
  FLASH_WR(base_addr, offset, *pgm_data_ptr);
  
}
#endif

void lld_WriteBufferAbortResetCmd(
    FLASHDATA * base_addr       /* device base address in system */
) {

    /* Issue Write Buffer Abort Reset Command Sequence */
    FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
    FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);

    /* Write to Buffer Abort Reset Command */
    FLASH_WR(base_addr, LLD_UNLOCK_ADDR1,
             NOR_WRITE_BUFFER_ABORT_RESET_CMD);

}

DEVSTATUS lld_StatusGet(
    FLASHDATA * base_addr,      /* device base address in system */
    ADDRESS offset              /* address offset from base address */
) {

    FLASHDATA dq6_toggles;
    FLASHDATA status_read_1;
    FLASHDATA status_read_2;
    FLASHDATA status_read_3;


    if (enable_status_cmd_g == 0) {     /* Do not use Read Status Command */
        status_read_1 = FLASH_RD(base_addr, offset);
        status_read_2 = FLASH_RD(base_addr, offset);
        status_read_3 = FLASH_RD(base_addr, offset);

        /* Any DQ6 toggles */
        dq6_toggles = ((status_read_1 ^ status_read_2) &        /* Toggles between read1 and read2 */
                       (status_read_2 ^ status_read_3) &        /* Toggles between read2 and read3 */
                       DQ6_MASK);       /* Check for DQ6 only */

        if (dq6_toggles) {
            /* Checking WriteBuffer Abort condition: 
               Check for all devices that have DQ6 toggling also have Write Buffer Abort DQ1 set */
            if (WriteBufferProgramming &&
                ((DQ6_TGL_DQ1_MASK & status_read_1) == DQ6_TGL_DQ1_MASK))
                return DEV_WRITE_BUFFER_ABORT;

            /* Checking Timeout condition: 
               Check for all devices that have DQ6 toggling also have Time Out DQ5 set. */
            if ((DQ6_TGL_DQ5_MASK & status_read_1) == DQ6_TGL_DQ5_MASK)
                return DEV_EXCEEDED_TIME_LIMITS;

            /* No timeout, no WB error */
            return DEV_BUSY;
        } else {                /* no DQ6 toggles on all devices */

            /* Checking Erase Suspend condition */
            status_read_1 = FLASH_RD(base_addr, offset);
            status_read_2 = FLASH_RD(base_addr, offset);

            /* Checking Erase Suspend condition */
            if (((status_read_1 ^ status_read_2) & DQ2_MASK) == 0)
                return DEV_NOT_BUSY;    /* All devices DQ2 not toggling */

            if (((status_read_1 ^ status_read_2) & DQ2_MASK) == DQ2_MASK)
                return DEV_SUSPEND;     /* All devices DQ2 toggling */
            else
                return DEV_BUSY;        /* Wait for all devices DQ2 toggling */
        }
    } else {
        /*..................................................................
           Use Status Register Read command to read the status register. 
           This is for GL-R device only 
           ................................................................... */
#ifdef STATUS_REG
        volatile FLASHDATA status_reg;

        wlld_StatusRegReadCmd(base_addr, offset);       /* Issue status register read command */
        status_reg = FLASH_RD(base_addr, offset);       /* read the status register */

        if ((status_reg & DEV_RDY_MASK) != DEV_RDY_MASK)        /* Are all devices done bit 7 is 1 */
            return DEV_BUSY;

        if (status_reg & DEV_ERASE_MASK)        /* Any erase error */
            return DEV_ERASE_ERROR;

        if (status_reg & DEV_PROGRAM_MASK)      /* Any program error */
            return DEV_PROGRAM_ERROR;

        if (status_reg & DEV_SEC_LOCK_MASK)     /* Any sector lock error */
            return DEV_SECTOR_LOCK;

        return DEV_NOT_BUSY;
#endif
        return DEV_STATUS_UNKNOWN;      /* should never get here */
    }
}
void lld_ResetCmd(
    FLASHDATA * base_addr       /* device base address in system */
) {

    /* Write Software RESET command */
    FLASH_WR(base_addr, 0, NOR_RESET_CMD);
}

DEVSTATUS lld_Poll(
    FLASHDATA * base_addr,      /* device base address in system */
    ADDRESS offset,             /* address offset from base address */
    FLASHDATA * exp_data_ptr,   /* expect data */
    FLASHDATA * act_data_ptr,   /* actual data */
    POLLING_TYPE polling_type   /* type of polling to perform */
) {
    DEVSTATUS dev_status;
    unsigned int polling_counter = 0xFFFFFFFF;

#ifdef WAIT_4us_FOR_DATA_POLLING_BITS_TO_BECOME_ACTIVE
    DELAY_US(4);
#endif
    /* set the WriteBuffer flag if writebuffer operation */
    if (polling_type == LLD_P_POLL_WRT_BUF_PGM)
        WriteBufferProgramming = 1;
    else
        WriteBufferProgramming = 0;

    do {
        polling_counter--;
        dev_status = lld_StatusGet(base_addr, offset);
    }
    while ((dev_status == DEV_BUSY) && polling_counter);

    /* reset the WriteBuffer flag */
    WriteBufferProgramming = 0;

    /* if device returns status other than "not busy" then we
       have a polling error state. */
    if (dev_status != DEV_NOT_BUSY) {
        if (dev_status == DEV_WRITE_BUFFER_ABORT) {
#ifndef REMOVE_WRITE_BUFFER_PROGRAMMING
            lld_WriteBufferAbortResetCmd(base_addr);
#endif
        } else {
            /* Issue software reset. */
            lld_ResetCmd(base_addr);
        }
        /* indicate to caller that there was an error */
        return (dev_status);
    } else {
        /* read the actual data */
        *act_data_ptr = FLASH_RD(base_addr, offset);

        /* Check that polling location verifies correctly */
        if ((*exp_data_ptr & LLD_DB_READ_MASK) ==
            (*act_data_ptr & LLD_DB_READ_MASK)) {
            dev_status = DEV_NOT_BUSY;
            return (dev_status);
        } else {
            dev_status = DEV_VERIFY_ERROR;
            return (dev_status);
        }
    }
}
void lld_SectorEraseCmd(
    FLASHDATA * base_addr,      /* device base address in system */
    ADDRESS offset              /* address offset from base address */
) {

    /* Issue Sector Erase Command Sequence */

    FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
    FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);

    FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_ERASE_SETUP_CMD);

    FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
    FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);

    /* Write Sector Erase Command to Offset */
    FLASH_WR(base_addr, offset, NOR_SECTOR_ERASE_CMD);

}

DEVSTATUS lld_SectorEraseOp(
    FLASHDATA * base_addr,      /* device base address is system */
    ADDRESS offset              /* address offset from base address */
) {
    FLASHDATA expect_data = (FLASHDATA) 0xFFFFFFFF;
    FLASHDATA actual_data = 0;
    DEVSTATUS status;


    lld_SectorEraseCmd(base_addr, offset);

    status =
        lld_Poll(base_addr, offset, &expect_data, &actual_data,
                 LLD_P_POLL_SEC_ERS);

    return (status);
}

#define LLD_BUFFER_SIZE 64 /* (bytes (x8) or words (x16)) */
unsigned lld_buffer_size = LLD_BUFFER_SIZE;

void
lld_set_buffer_size(unsigned n)
{
    lld_buffer_size = n;
}

void lld_WriteToBufferCmd(
    FLASHDATA * base_addr,      /* device base address in system */
    ADDRESS offset              /* address offset from base address */
) {
    /* Issue unlock command sequence */
    FLASH_WR(base_addr, LLD_UNLOCK_ADDR1, NOR_UNLOCK_DATA1);
    FLASH_WR(base_addr, LLD_UNLOCK_ADDR2, NOR_UNLOCK_DATA2);

    /* Write Write Buffer Load Command */
    FLASH_WR(base_addr, offset, NOR_WRITE_BUFFER_LOAD_CMD);

}

void lld_ProgramBufferToFlashCmd(
    FLASHDATA * base_addr,      /* device base address in system */
    ADDRESS offset              /* address offset from base address */
) {

    /* Transfer Buffer to Flash Command */
    FLASH_WR(base_addr, offset, NOR_WRITE_BUFFER_PGM_CONFIRM_CMD);

}

DEVSTATUS lld_WriteBufferProgramOp(
    FLASHDATA * base_addr,      /* device base address is system     */
    ADDRESS offset,             /* address offset from base address  */
    WORDCOUNT word_count,       /* number of words to program        */
    FLASHDATA * data_buf        /* buffer containing data to program */
) {
    DEVSTATUS status;
    FLASHDATA write_data = 0;
    FLASHDATA read_data = 0;
    ADDRESS last_loaded_addr;
    ADDRESS current_offset;
    ADDRESS end_offset;
    FLASHDATA wcount;

    /* Initialize variables */
    current_offset = offset;
    end_offset = offset + word_count - 1;
    last_loaded_addr = offset;

    /* don't try with a count of zero */
    if (!word_count) {
        return (DEV_NOT_BUSY);
    }

    /* Issue Load Write Buffer Command Sequence */
    lld_WriteToBufferCmd(base_addr, offset);

    /* Write # of locations to program */
    wcount = (FLASHDATA) word_count - 1;
    wcount *= LLD_DEV_MULTIPLIER;

    FLASH_WR(base_addr, offset, wcount/ sizeof(FLASHDATA));

    /* Load Data into Buffer */
    while (current_offset <= end_offset) {
        /* Store last loaded address & data value (for polling) */
        last_loaded_addr = current_offset;
        write_data = *data_buf;

        /* Write Data */
        FLASH_WR(base_addr, current_offset, *data_buf++);
	current_offset += sizeof(FLASHDATA);
    }

    /* Issue Program Buffer to Flash command */
    lld_ProgramBufferToFlashCmd(base_addr, offset/*last_loaded_addr*/);

    status = lld_Poll(base_addr, last_loaded_addr, &write_data,
                      &read_data, LLD_P_POLL_WRT_BUF_PGM);
    return (status);
}

DEVSTATUS lld_memcpy(
    FLASHDATA * base_addr,      /* device base address is system */
    ADDRESS offset,             /* address offset from base address */
    WORDCOUNT word_cnt,         /* number of words to program */
    FLASHDATA * data_buf        /* buffer containing data to program */
) {
    ADDRESS mask = lld_buffer_size - 1;
    WORDCOUNT intwc = word_cnt;
    DEVSTATUS status = DEV_NOT_BUSY;

    if (offset & mask) {
        /* program only as much as necessary, so pick the lower of the two numbers */
        if (word_cnt < (lld_buffer_size - (offset & mask)))
            intwc = word_cnt;
        else
            intwc = lld_buffer_size - (offset & mask);

        /* program the first few to get write buffer aligned */
        status =
            lld_WriteBufferProgramOp(base_addr, offset, intwc, data_buf);
        if (status != DEV_NOT_BUSY) {
            return (status);
        }

        offset += intwc;        /* adjust pointers and counter */
        word_cnt -= intwc;
        data_buf += intwc;
        if (word_cnt == 0) {
            return (status);
        }
    }

    while (word_cnt >= lld_buffer_size) {       /* while big chunks to do */
        status =
            lld_WriteBufferProgramOp(base_addr, offset, lld_buffer_size,
                                     data_buf);
        if (status != DEV_NOT_BUSY) {
            return (status);
        }

        offset += lld_buffer_size;      /* adjust pointers and counter */
        word_cnt -= lld_buffer_size;
        data_buf += lld_buffer_size / sizeof(FLASHDATA);
    }
    if (word_cnt == 0) {
        return (status);
    }

    status =
        lld_WriteBufferProgramOp(base_addr, offset, word_cnt, data_buf);
    return (status);
}

/******************************************************************************
* 
* lld_ProgramOp - Performs a standard Programming Operation.
*
* Function programs a single location to the specified data.
* Function issues all required commands and polls for completion.
*
*
* RETURNS: DEVSTATUS
*/
#ifndef REMOVE_LLD_PROGRAM_OP
DEVSTATUS lld_ProgramOp
(
FLASHDATA * base_addr,      /* device base address is system */
ADDRESS offset,         /* address offset from base address */
FLASHDATA write_data    /* variable containing data to program */
)
{       
  FLASHDATA read_data = 0;
  DEVSTATUS status;

  lld_ProgramCmd(base_addr, offset, &write_data);
  
  status = lld_Poll(base_addr, offset, &write_data, &read_data, LLD_P_POLL_PGM);
  
  return(status);
}
#endif
