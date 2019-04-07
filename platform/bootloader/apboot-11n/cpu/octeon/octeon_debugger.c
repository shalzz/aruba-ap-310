/*************************************************************************
Copyright (c) 2003-2005 Cavium Networks (support@cavium.com). All rights
reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Cavium Networks' name may not be used to endorse or promote products
derived from this software without specific prior written permission.

This Software, including technical data, may be subject to U.S. export
control laws, including the U.S. Export Administration Act and its
associated regulations, and may be subject to export or import
regulations in other countries. You warrant that You will comply
strictly in all respects with all such regulations and acknowledge that
you have the responsibility to obtain licenses to export, re-export or
import the Software.

TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
AND WITH ALL FAULTS AND CAVIUM MAKES NO PROMISES, REPRESENTATIONS OR
WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY)
WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE
RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.

*************************************************************************/

/**
 * @file octeon-debugger.c
 *
 * Control stub for GDB debugging.
 * - This runs in interrupt context so most C library
 *      functions do not work. Be very careful what you do here.
 *
 * $Id: octeon_debugger.c,v 1.49 2006/12/07 16:20:30 cchavva Exp $ $Name: SDK_1_5_0_build_187 $
 *
 */

#include <common.h>
#ifndef CONFIG_ARUBA_OCTEON
#include <linux/ctype.h>
#include "octeon_boot.h"
#include "octeon_serial.h"
#include "octeon_hal.h"


void cvmx_debugger_finish(void);

#define  OCTEON_CIU_INTX_EN0(offset)                           (OCTEON_ADD_IO_SEG(0x0001070000000200ull+((offset)*16)))

/* Function used for all debug exception handler output */
#define DEBUG_PRINTF(format, ...) printf("[DEBUG STUB]" format, ##__VA_ARGS__)
#define DEBUG_GDB_LOGGING 0         /* Display every command from the debugger */
#define DEBUG_CORE_STATE 0          /* Display the state of each core */

#define HW_INSTRUCTION_BREAKPOINT_STATUS            (0xFFFFFFFFFF301000ull)
#define HW_INSTRUCTION_BREAKPOINT_ADDRESS(num)      (0xFFFFFFFFFF301100ull + 0x100 * (num))
#define HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(num) (0xFFFFFFFFFF301108ull + 0x100 * (num))
#define HW_INSTRUCTION_BREAKPOINT_ASID(num)         (0xFFFFFFFFFF301110ull + 0x100 * (num))
#define HW_INSTRUCTION_BREAKPOINT_CONTROL(num)      (0xFFFFFFFFFF301118ull + 0x100 * (num))

#define HW_DATA_BREAKPOINT_STATUS                   (0xFFFFFFFFFF302000ull)
#define HW_DATA_BREAKPOINT_ADDRESS(num)             (0xFFFFFFFFFF302100ull + 0x100 * (num))
#define HW_DATA_BREAKPOINT_ADDRESS_MASK(num)        (0xFFFFFFFFFF302108ull + 0x100 * (num))
#define HW_DATA_BREAKPOINT_ASID(num)                (0xFFFFFFFFFF302110ull + 0x100 * (num))
#define HW_DATA_BREAKPOINT_CONTROL(num)             (0xFFFFFFFFFF302118ull + 0x100 * (num))

typedef union
{
    uint64_t u64;
    struct
    {
        uint64_t    rsrvd:32;   /**< Unused */
        uint64_t    dbd:1;      /**< Indicates whether the last debug exception or
                                    exception in Debug Mode occurred in a branch or
                                    jump delay slot */
        uint64_t    dm:1;       /**< Indicates that the processor is operating in Debug
                                    Mode: */
        uint64_t    nodcr:1;    /**< Indicates whether the dseg segment is present */
        uint64_t    lsnm:1;     /**< Controls access of loads/stores between the dseg
                                    segment and remaining memory when the dseg
                                    segment is present */
        uint64_t    doze:1;     /**< Indicates that the processor was in a low-power mode
                                    when a debug exception occurred */
        uint64_t    halt:1;     /**< Indicates that the internal processor system bus clock
                                    was stopped when the debug exception occurred */
        uint64_t    countdm:1;  /**< Controls or indicates the Count register behavior in
                                    Debug Mode. Implementations can have fixed
                                    behavior, in which case this bit is read-only (R), or
                                    the implementation can allow this bit to control the
                                    behavior, in which case this bit is read/write (R/W).
                                    The reset value of this bit indicates the behavior after
                                    reset, and depends on the implementation.
                                    Encoding of the bit is:
                                    - 0      Count register stopped in Debug Mode Count register is running in Debug
                                    - 1      Mode
                                    This bit is read-only (R) and reads as zero if not implemented. */
        uint64_t    ibusep:1;   /**< Indicates if a Bus Error exception is pending from an
                                    instruction fetch. Set when an instruction fetch bus
                                    error event occurs or a 1 is written to the bit by
                                    software. Cleared when a Bus Error exception on an
                                    instruction fetch is taken by the processor. If IBusEP
                                    is set when IEXI is cleared, a Bus Error exception on
                                    an instruction fetch is taken by the processor, and
                                    IBusEP is cleared.
                                    In Debug Mode, a Bus Error exception applies to a
                                    Debug Mode Bus Error exception.
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    mcheckp:1;  /**< Indicates if a Machine Check exception is pending.
                                    Set when a machine check event occurs or a 1 is
                                    written to the bit by software. Cleared when a
                                    Machine Check exception is taken by the processor.
                                    If MCheckP is set when IEXI is cleared, a Machine
                                    Check exception is taken by the processor, and
                                    MCheckP is cleared.
                                    In Debug Mode, a Machine Check exception applies
                                    to a Debug Mode Machine Check exception.
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    cacheep:1;  /**< Indicates if a Cache Error is pending. Set when a
                                    cache error event occurs or a 1 is written to the bit by
                                    software. Cleared when a Cache Error exception is
                                    taken by the processor. If CacheEP is set when IEXI
                                    is cleared, a Cache Error exception is taken by the
                                    processor, and CacheEP is cleared.
                                    In Debug Mode, a Cache Error exception applies to a
                                    Debug Mode Cache Error exception.
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    dbusep:1;   /**< Indicates if a Data Access Bus Error exception is
                                    pending. Set when a data access bus error event
                                    occurs or a 1 is written to the bit by software. Cleared
                                    when a Bus Error exception on data access is taken by
                                    the processor. If DBusEP is set when IEXI is cleared,
                                    a Bus Error exception on data access is taken by the
                                    processor, and DBusEP is cleared.
                                    In Debug Mode, a Bus Error exception applies to a
                                    Debug Mode Bus Error exception.
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    iexi:1;     /**< An Imprecise Error eXception Inhibit (IEXI) controls
                                    exceptions taken due to imprecise error indications.
                                    Set when the processor takes a debug exception or an
                                    exception in Debug Mode occurs. Cleared by
                                    execution of the DERET instruction. Otherwise
                                    modifiable by Debug Mode software.
                                    When IEXI is set, then the imprecise error exceptions
                                    from bus errors on instruction fetches or data
                                    accesses, cache errors, or machine checks are
                                    inhibited and deferred until the bit is cleared.
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    ddbsimpr:1; /**< Indicates that a Debug Data Break Store Imprecise
                                    exception due to a store was the cause of the debug
                                    exception, or that an imprecise data hardware break
                                    due to a store was indicated after another debug
                                    exception occurred. Cleared on exception in Debug
                                    Mode.
                                        - 0 No match of an imprecise data hardware breakpoint on store
                                        - 1 Match of imprecise data hardware breakpoint on store
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    ddblimpr:1; /**< Indicates that a Debug Data Break Load Imprecise
                                    exception due to a load was the cause of the debug
                                    exception, or that an imprecise data hardware break
                                    due to a load was indicated after another debug
                                    exception occurred. Cleared on exception in Debug
                                    Mode.
                                        - 0 No match of an imprecise data hardware breakpoint on load
                                        - 1 Match of imprecise data hardware breakpoint on load
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    ejtagver:3; /**< Provides the EJTAG version.
                                        - 0      Version 1 and 2.0
                                        - 1      Version 2.5
                                        - 2      Version 2.6
                                        - 3-7    Reserved */
        uint64_t    dexccode:5; /**< Indicates the cause of the latest exception in Debug
                                    Mode.
                                    The field is encoded as the ExcCode field in the
                                    Cause register for those exceptions that can occur in
                                    Debug Mode (the encoding is shown in MIPS32 and
                                    MIPS64 specifications), with addition of code 30
                                    with the mnemonic CacheErr for cache errors and the
                                    use of code 9 with mnemonic Bp for the SDBBP
                                    instruction.
                                    This value is undefined after a debug exception. */
        uint64_t    nosst:1;    /**< Indicates whether the single-step feature controllable
                                    by the SSt bit is available in this implementation:
                                          - 0      Single-step feature available
                                          - 1      No single-step feature available
                                    A minimum number of hardware instruction
                                    breakpoints must be available if no single-step
                                    feature is implemented in hardware. Refer to Section
                                    4.8.1 on page 69 for more information. */
        uint64_t    sst:1;      /**< Controls whether single-step feature is enabled:
                                          - 0       No enable of single-step feature
                                          - 1       Single-step feature enabled
                                    This bit is read-only (R) and reads as zero if not
                                    implemented due to no single-step feature (NoSSt is
                                    1). */
        uint64_t    rsrvd2:2;   /**< Must be zero */
        uint64_t    dint:1;     /**< Indicates that a Debug Interrupt exception occurred.
                                    Cleared on exception in Debug Mode.
                                          - 0       No Debug Interrupt exception
                                          - 1       Debug Interrupt exception
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    dib:1;      /**< Indicates that a Debug Instruction Break exception
                                    occurred. Cleared on exception in Debug Mode.
                                          - 0       No Debug Instruction Break exception
                                          - 1       Debug Instruction Break exception
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    ddbs:1;     /**< Indicates that a Debug Data Break Store exception
                                    occurred on a store due to a precise data hardware
                                    break. Cleared on exception in Debug Mode.
                                          - 0       No Debug Data Break Store Exception
                                          - 1       Debug Data Break Store Exception
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    ddbl:1;     /**< Indicates that a Debug Data Break Load exception
                                    occurred on a load due to a precise data hardware
                                    break. Cleared on exception in Debug Mode.
                                          - 0       No Debug Data Break Store Exception
                                          - 1       Debug Data Break Store Exception
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
        uint64_t    dbp:1;      /**< Indicates that a Debug Breakpoint exception
                                    occurred. Cleared on exception in Debug Mode.
                                          - 0      No Debug Breakpoint exception
                                          - 1      Debug Breakpoint exception */
        uint64_t    dss:1;      /**< Indicates that a Debug Single Step exception
                                    occurred. Cleared on exception in Debug Mode.
                                          - 0       No debug single-step exception
                                          - 1       Debug single-step exception
                                    This bit is read-only (R) and reads as zero if not
                                    implemented. */
    } s;
} debug_register_format_t;

typedef enum
{
    COMMAND_NOP,                /**< Core doesn't need to do anything. Just stay in exception handler */
    COMMAND_STEP,               /**< Core needs to perform a single instruction step */
    COMMAND_CONTINUE            /**< Core need to start running. Doesn't return until some debug event occurs */
} command_t;

/**
 * The debugger state is implemented as a structure packet
 * into a single 8 byte word. This way accesses to it are
 * fairly atomic. Locking is still necessary during updates.
 * Throughout the debugger a mixture of spinlocks and logic
 * control access to this structure. Be careful.
 */
typedef union
{
    uint64_t u64;
    struct
    {
        uint32_t    command_count   : 8;    /**< Incremented every time cores should start running */
        command_t   command         : 2;    /**< Command for all cores. Only valid when command_count changes */
        uint32_t    send_break      : 1;    /**< True if the focus core should send a break to the debugger */
        uint32_t    step_all        : 1;    /**< True if step and continue should affect all cores. False, only the focus core is affected */
        uint32_t    focus_core      : 4;    /**< Core currently under control of the debugger */
        uint32_t    known_cores     :16;    /**< Bitmask of all cores found during startup */
        uint32_t    active_cores    :16;    /**< Bitmask of cores that should stop on a breakpoint */
        uint32_t    handler_cores   :16;    /**< Bitmask of cores currently running the exception handler */
    } s;
} debug_state_t;

/**
 * Structure of the performance counter control register
 */
typedef union
{
    uint32_t u32;
    struct
    {
        uint32_t                M       : 1;	/* 1 for sel=0, 0 for sel=1 */
        uint32_t                W       : 1;	/* = 1, 64-bit counter */
        uint32_t                reserved: 19;	
        uint32_t                event   : 6;	/* Select the event */
        uint32_t                IE      : 1;	/* Interrupt enable */
        uint32_t                U       : 1;	/* enable events in user mode */
        uint32_t                S       : 1;	/* enable events in superuser mode */
        uint32_t                K       : 1;	/* enable events in kernel mode */
        uint32_t                EX      : 1;	/* enable event counter */
    } s;
} cvmx_perf_control_t;

extern char *strcpy(char *dest, const char *source);/* Only a very few C library functions work here */
extern char *strcat(char *dest, const char *append);
extern void *memcpy(void *dest, const void *src, size_t n);

/*************************************************************************/
/*************************************************************************/
/* Global variables */
/*************************************************************************/
/*************************************************************************/

/* add arrays to make per cpu */
/* Make this a fixed address, as this is needed early in the debug exception before
** the gp pointer is set up.  (gp is needed to resolve 'C' symbols, as the bootloader is relocatable.)
*/
uint64_t    *cvmx_debugger_register_storage = (void *)MAKE_KSEG0(BOOTLOADER_DEBUG_REG_SAVE_BASE);/* Per core */
uint8_t     *cvmx_debugger_stack = (void *)MAKE_KSEG0(BOOTLOADER_DEBUG_STACK_BASE);  /* Per core */

//uint8_t     cvmx_debugger_stack[DEBUG_STACK_SIZE * 16];  /* Per core */

/* Data structures shared by all cores */
static debug_state_t    debug_state_global;         /* Our current state, One global */
static cvmx_spinlock_t  debug_lock = {OCTEON_SPINLOCK_UNLOCKED_VAL};  /* Lock for debug_state_global, One global */
static volatile int     debug_init_complete = 0;    /* Initialization has been done, One global */
static int              debug_uart=1;               /* Which uart to use */

char                    debug_input_buffer[2048];    /* Command from the debugger, One global */
char                    debug_output_buffer[2048];  /* Reply to the debugger, One global */


/*************************************************************************/
/*************************************************************************/
/* Inter core data functions */
/*************************************************************************/
/*************************************************************************/

uint64_t *get_debugger_register_storage_ptr(void)
{
    uint32_t coreid = get_core_num();
    return(cvmx_debugger_register_storage + coreid * DEBUG_NUMREGS);

}

/**
 * Update the state of the debugger. The debugger state is
 * highly volatile and needs great care to prevent race
 * conditions.
 *
 * @param state  New state to save
 */
static inline void debug_update_state(debug_state_t state)
{
    volatile debug_state_t* state_ptr = &debug_state_global;

    OCTEON_SYNC;
    *state_ptr = state;
    OCTEON_SYNCW;
}


/**
 * Get the current debugger state. This access function is
 * used to help make sure the volatile nature of the state
 * doesn't cause trouble. The values you get out of this
 * function are stale immiediately unless there is some
 * other form of locking present.
 *
 * @return The current debugger state
 */
static inline debug_state_t debug_get_state(void)
{
    volatile debug_state_t* state_ptr = &debug_state_global;
    OCTEON_SYNC;
    return *state_ptr;
}

/*************************************************************************/
/*************************************************************************/
/* Generic support routines for I/O */
/*************************************************************************/
/*************************************************************************/

static const unsigned char hexchars[]="0123456789abcdef";


/**
 * Routines to handle hex data
 *
 * @param ch
 * @return
 */
static inline int hex(char ch)
{
    if ((ch >= 'a') && (ch <= 'f'))
        return(ch - 'a' + 10);
    if ((ch >= '0') && (ch <= '9'))
        return(ch - '0');
    if ((ch >= 'A') && (ch <= 'F'))
        return(ch - 'A' + 10);
    return(-1);
}


/**
 * Convert the memory pointed to by mem into hex, placing result in buf.
 * Return a pointer to the last char put in buf (null), in case of mem fault,
 * return 0.
 * If MAY_FAULT is non-zero, then we will handle memory faults by returning
 * a 0, else treat a fault like any other fault in the stub.
 *
 * @param mem
 * @param buf
 * @param count
 * @return
 */
static char *mem2hex (uint64_t mem_addr, char *buf, int count)
{
    unsigned char ch;
    while (count-- > 0)
    {
        ch = octeon_read64_byte(mem_addr++);
        *buf++ = hexchars[ch >> 4];
        *buf++ = hexchars[ch & 0xf];
    }

    *buf = 0;
    return buf;
}


/**
 * convert the hex array pointed to by buf into binary to be placed in mem
 * return a pointer to the character AFTER the last byte written
 *
 * @param buf
 * @param mem
 * @param count
 * @return
 */
uint64_t hex2mem (const char *buf, uint64_t mem_addr, int count)
{
    int i;
    unsigned char ch;
    uint64_t tmp_addr = mem_addr;

    for (i=0; i<count; i++)
    {
        ch = hex(*buf++) << 4;
        ch |= hex(*buf++);
        octeon_write64_byte(tmp_addr++, ch);
    }

    return mem_addr;
}


/**
 * While we find nice hex chars, build an int.
 * Return number of chars processed.
 *
 * @param ptr
 * @param intValue
 * @return
 */
static int hexToInt(const char **ptr, int *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;
    while (**ptr)
    {
        hexValue = hex(**ptr);
        if (hexValue < 0)
            break;

        *intValue = (*intValue << 4) | hexValue;
        numChars ++;

        (*ptr)++;
    }

    return(numChars);
}


/**
 * While we find nice hex chars, build an int.
 * Return number of chars processed.
 *
 * @param ptr
 * @param intValue
 * @return
 */
static int hexToLong(const char **ptr, uint64_t *intValue)
{
    int numChars = 0;
    long hexValue;

    *intValue = 0;
    while (**ptr)
    {
        hexValue = hex(**ptr);
        if (hexValue < 0)
            break;

        *intValue = (*intValue << 4) | hexValue;
        numChars ++;

        (*ptr)++;
    }

    return(numChars);
}


/*************************************************************************/
/*************************************************************************/
/* Uart Routines */
/*************************************************************************/
/*************************************************************************/


/**
 * Get a single byte from serial port.
 *
 * @return
 */
static inline unsigned char getDebugChar(void)
{
    cvmx_uart_lsr_t lsrval;

    /* Spin until data is available */
    do
    {
	    lsrval.u64 = octeon_read64(OCTEON_MIO_UARTX_LSR(debug_uart));
    } while (!lsrval.s.dr);

    /* Read and return the data */
    return octeon_read64(OCTEON_MIO_UARTX_RBR(debug_uart));
}


/**
 * Put a single byte to serial port.
 *
 * @param ch
 */
// param needs to be u64
static inline void putDebugChar(uint64_t ch)
{
    cvmx_uart_lsr_t lsrval;

    /* Spin until there is room */
    do
    {
        lsrval.u64 = octeon_read64(OCTEON_MIO_UARTX_LSR(debug_uart));
    }
    while (lsrval.s.thre == 0);

    /* Write the byte */
    octeon_write64(OCTEON_MIO_UARTX_THR(debug_uart), ch);
}


/**
 *
 * @param str
 */
static inline void putDebugString(const char *str)
{
    while (*str)
         putDebugChar(*str++);
}

int octeon_uart_setup(int uart_index);

/*************************************************************************/
/*************************************************************************/
/* GDB I/O */
/*************************************************************************/
/*************************************************************************/


/**
 * Get the packet from host. Check if the packet is valid or not. Store the
 *   data into a static buffer from the sequence $[data]#[checksum]
 *
 * @param buffer
 * @param bufferSize
 * @return
 */
static char *getpacket(char *buffer, int bufferSize)
{
    unsigned char   checksum;
    unsigned char   xmitcsum;
    int             count;
    char            ch;
    char * result;

    while (1)
    {
        /* wait around for the start character, ignore all other characters */
        while ((ch = getDebugChar ()) != '$')
            ;
        retry:
        checksum = 0;
        xmitcsum = -1;
        count = 0;

        /* now, read until a # or end of buffer is found */
        while (count < bufferSize)
        {
            ch = getDebugChar ();
            if (ch == '$')
                goto retry;
            if (ch == '#')
                break;
            checksum = checksum + ch;
            buffer[count] = ch;
            count = count + 1;
        }
        buffer[count] = 0;

        if (ch == '#')
        {
            ch = getDebugChar ();
            xmitcsum = hex (ch) << 4;
            ch = getDebugChar ();
            xmitcsum += hex (ch);

            if (checksum == xmitcsum)
            {
                result = buffer;
                break;
            }
        }
    }

    if (DEBUG_GDB_LOGGING)
        DEBUG_PRINTF("Request: %s\n", result);

    return result;
}


/**
 * send the packet in buffer.
 *
 * @param buffer
 */
static void putpacket(const char *buffer)
{
    int len;
    unsigned char checksum;
    const unsigned char *ptr;

    /* Calculate the GDB command checksum */
    len = 0;
    checksum = 0;
    ptr = (const unsigned char*)buffer;
    while (*ptr)
    {
        checksum += *ptr++;
        len++;
    }

    if (len > sizeof(debug_output_buffer))
        DEBUG_PRINTF("Output too long: %d\n", len);

    if (DEBUG_GDB_LOGGING)
        DEBUG_PRINTF("Reply: %s\n", buffer);

    /* Send the command */
    putDebugChar('$');
    putDebugString(buffer);
    putDebugChar('#');
    putDebugChar(hexchars[checksum >> 4]);
    putDebugChar(hexchars[checksum & 0xf]);
}


/**
 * Build and send an integer based reply to the debugger.
 *
 * @param command_char
 *                   Command character code to begin the reply with
 * @param replyValue Integer to put in the command in hex.
 */
static void sendIntReply(char command_char, int replyValue)
{
    debug_output_buffer[0] = command_char;
    mem2hex((uint32_t)&replyValue, debug_output_buffer + 1, sizeof(replyValue));
    putpacket(debug_output_buffer);
}


/**
 * Send a message to the debugger operator from a specific core.
 *
 * @param core    Core to send the message from
 * @param message Message to send
 */
static void sendCoreMessage(int core, const char *message)
{
     strcpy(debug_output_buffer, "!Core XX ");
     strcat(debug_output_buffer, message);
     if (core < 10)
     {
         debug_output_buffer[6] = ' ';
         debug_output_buffer[7] = core + '0';
     }
     else
     {
         debug_output_buffer[6] = '1';
         debug_output_buffer[7] = core - 10 + '0';
     }
     putpacket(debug_output_buffer);
}


/**
 * get entryhi register.  Need this wrapper as C code is only 32 bit.
 *
 * @return 64 bit entryhi in two registers (one 64 bit int in C)
 */
uint64_t octeon_get_entryhi(void)
{
    uint32_t tmp_low, tmp_hi;

    /* r2 is high bits */
    asm volatile (
               "   .set push                    \n"
               "   .set mips64                  \n"
               "   .set noreorder               \n"
               "   dmfc0 %[tmpl], $10           \n"
               "   add   %[tmph], %[tmpl], $0   \n"
               "   dsrl  %[tmph], 32            \n"
               "   dsll  %[tmpl], 32            \n"
               "   dsrl  %[tmpl], 32            \n"
               "   .set pop                 \n"
                  : [tmpl] "=&r" (tmp_low), [tmph] "=&r" (tmp_hi) : );

    return(((uint64_t)tmp_hi << 32) + tmp_low);
}

/**
 * get entryhi register.  Need this wrapper as C code is only 32 bit.
 *
 * @param val    64 bit value to set in entryhi register
 */
void octeon_set_entryhi(uint64_t val)
{
    uint32_t val_low  = val & 0xffffffff;
    uint32_t val_high = val  >> 32;

    asm volatile (
        "  .set push                       \n"
        "  .set mips64                       \n"
        "  .set noreorder                    \n"
        /* Standard twin 32 bit -> 64 bit construction */
        "  dsll  %[valh], 32                 \n"
        "  dsll  %[vall], 32          \n"
        "  dsrl  %[vall], 32          \n"
        "  daddu %[valh], %[valh], %[vall]   \n"
        /* Combined value is in valh */
        "  dmtc0 %[valh], $10               \n"
        "  .set pop                         \n"
         :: [valh] "r" (val_high), [vall] "r" (val_low) );

}

/**
  * Initialize the performance counter control registers.
  *
  */
static void initialize_perf_control_reg (int perf_event, int perf_counter)
{
    uint64_t *regs = get_debugger_register_storage_ptr();
    cvmx_perf_control_t control;

    control.u32 = 0;
    control.s.U = 1;
    control.s.S = 1;
    control.s.K = 1;
    control.s.EX = 1;
    control.s.W = 1;

    control.s.event = perf_event;
    if (perf_counter == 1)
      {
	control.s.M = 0;
	regs[43] = control.u32;
      }
    else
      {
	control.s.M = 1;
	regs[42] = control.u32;
      }
}

/*************************************************************************/
/*************************************************************************/
/* Actual Debug stuff */
/*************************************************************************/
/*************************************************************************/

/**
 * Return non zero if the supplied address can be accessed.
 * Basically checks the TLB entries to make sure the supplied
 * address is in them. This is an attempt to hanlde bad address
 * from the debugger without crashing the simulator.
 *
 * @param address Address to check
 * @return Non zero if the address is valid
 */
static int debug_is_address_valid(uint64_t address)
{
    uint64_t    original_entryhi;
    int32_t     original_index;
    int32_t     index;

    /* Assume all XKPHYS and CKSEG0 addresses are valid */
    if ((address >> 62 == 0x2) || (address >> 62 == 0x3))
        return 1;

    /* Store the entryhi register and the index register. The running
        program may need the current values */
    original_entryhi = octeon_get_entryhi();
    asm volatile ("mfc0 %0, $0" : "=r" (original_index));

    /* Set the entryhi to match the supplied address. Use the current
        ASID value */
    address = (address & (-1ll<<13)) | (original_entryhi & 0xff);
    octeon_set_entryhi(address);

    /* Lookup the address entry index in the TLB */
    asm volatile ("tlbp");
    asm volatile ("mfc0 %0, $0" : "=r" (index));

    /* Restore the original entryhi and index */
    octeon_set_entryhi(original_entryhi);
    asm volatile ("dmtc0 %0, $0" : : "r" (original_index));

    return (index >= 0); /* TLB lookup succeeded if index not negative */
}


/**
 * Process a command from the debugger
 *
 * @param debuggerCommand
 *               Debugger command received
 * @return Operation to be performed by the cores
 */
static command_t processDebuggerCommand(const char *debuggerCommand)
{
    const char *    buf     = debuggerCommand;
    command_t       result  = COMMAND_NOP;
    debug_state_t   state   = debug_get_state();

    /* A one letter command code represents what to do. Switch on it */
    switch (*buf++)
    {
        case '?':   /* What protocol do I speak? */
            putpacket("S0A");
            break;

        case '\003':   /* Control-C */
            if (!state.s.send_break)
                putpacket("T9");
            break;

        case 'F':   /* Change the focus core */
            {
                uint64_t core;
                hexToLong(&buf, &core);

                /* Only cores in the exception handler may become the focus.
                    If a core not in the exception handler got focus the
                    debugger would hang since nobody would talk to it */
                if (state.s.handler_cores & (1<<core))
                {
                    /* Focus change reply must be sent before the focus
                        changes. Otherwise the new focus core will eat
                        our ACK from the debugger */
                    sendIntReply('F', core);
                    state.s.focus_core = core;
                    debug_update_state(state);
                    break;
                }
                else
                    putpacket("!Core is not in the exception handler. Focus not changed.");
                /* Nothing changed, so we send back the old value */
                sendIntReply('F', state.s.focus_core);

            }
            break;

        case 'f':   /* Get the focus core */
            sendIntReply('F', state.s.focus_core);
            break;

        case 'I':   /* Set the active cores */
            {
                uint64_t        active_cores;
                hexToLong(&buf, &active_cores);

                /* Limit the active mask to the known to exist cores */
                state.s.active_cores = active_cores & state.s.known_cores;

                /* Lazy user hack to have 0 be all cores */
                if (state.s.active_cores == 0)
                    state.s.active_cores = state.s.known_cores;

                /* The focus core must be in the active_cores mask */
                if ((state.s.active_cores & (1<<state.s.focus_core)) == 0)
                {
                    putpacket("!Focus core was added to the masked.");
                    state.s.active_cores |= 1<<state.s.focus_core;
                }

                debug_update_state(state);
            }
            /* Fall through. The reply to the set active cores command is the
                same as the get active cores command */

        case 'i':   /* Get the active cores */
            sendIntReply('I', state.s.active_cores);
            break;

        case 'A':   /* Setting the step mode all or one */
            {
                if (*buf == '1')
                    state.s.step_all = 1;   /* A step or continue will start all cores */
                else
                    state.s.step_all = 0;   /* A step or continue only affects the focus core */
                debug_update_state(state);
            }
            /* Fall through. The reply to the set step-all command is the
                same as the get step-all command */

        case 'a':   /* Getting the current step mode */
            sendIntReply('A', state.s.step_all);
            break;

        case 'q':   /* Query for processor state */
            {
                int state_type;
                int regno;
                uint64_t *regs = get_debugger_register_storage_ptr();

                hexToInt(&buf, &state_type);
                switch (state_type)
                {
                    case 0: /* All registers and current instruction */
                        for (regno=0; regno<40; regno++)
                            mem2hex ((uint32_t)&(regs[regno]), debug_output_buffer + regno * 16, 8);
                        mem2hex(regs[37], debug_output_buffer + 40 * 16, 4);
                        putpacket (debug_output_buffer);
                        break;
                    default:
                        DEBUG_PRINTF("Query #%d is not valid\n", state_type);
                        break;
                }
            }
            break;
        case 'g':   /* read a register from global place. */
            {
                uint64_t *regs = get_debugger_register_storage_ptr();
                int regno;

                /* Get the register number to read */
                hexToInt(&buf, &regno);

                /* Make sure the register number is valid */
                if ((regno < DEBUG_NUMREGS) && (regno >= 0))
                {
                    regs += regno;
                    mem2hex ((uint32_t)regs, debug_output_buffer, 8);   /* Convert the register value into hex */
                    putpacket (debug_output_buffer);
                }
                else
                    DEBUG_PRINTF("Register #%d is not valid\n", regno);
            }
            break;

        case 'G':   /* set the value of a register. */
            {
                volatile uint64_t *regs = get_debugger_register_storage_ptr();
                int regno;

                /* Get the register number to write. It should be followed by
                    a comma */
                if (hexToInt(&buf, &regno) && (*buf++ == ','))
                {
                    /* Make sure the register number is valid */
                    if (regno < DEBUG_NUMREGS && regno >= 0)
                    {
                        regs += regno;
                        hex2mem (buf, (uint32_t)regs, 8);     /* Write over the saved register value */
                    }
                    else
                        DEBUG_PRINTF("Register #%d is not valid\n", regno);
                }
            }
            break;

        case 'm':   /* Memory read. mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
            {
                uint64_t addr;
                uint64_t length;

                /* Get the memory address, a comma, and the length */
                if (hexToLong(&buf, &addr)
                    && (*buf++ == ',')
                    && hexToLong(&buf, &length))
                {
                    if (debug_is_address_valid(addr))
                    {
                        mem2hex (addr, debug_output_buffer, length);
                    }
                    else
                    {
                        const uint64_t bad = 0x00baddeed00baddeedull;
                        putpacket("!Attempt to read from an invalid memory address.");
                        mem2hex ((uint32_t)&bad, debug_output_buffer, length);
                    }
                    putpacket (debug_output_buffer);
                }
            }
            break;

        case 'M':   /* Memory write. MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
            {
                uint64_t addr;
                uint64_t length;

                /* Get the memory address, a comma, the length, and a colon */
                if (hexToLong(&buf, &addr)
                    && *buf++ == ','
                    && hexToLong(&buf, &length)
                    && *buf++ == ':')
                {
                    if (debug_is_address_valid(addr))
                    {
                        hex2mem (buf, addr, length);
                    }
                    else
                    {
                        putpacket("!Attempt to write to an invalid memory address.");
                    }
                    putpacket("+");
                }
            }
            break;

	case 'e':  /* Initialize the performance counter events. */
	    {
	        char perf_counter = *buf++;
		int perf_event = 0;
		hexToInt(&buf, &perf_event);
		volatile uint64_t *regs = get_debugger_register_storage_ptr();
		
		switch (perf_counter)
		{
		    case '1': /* Set performance counter0 event. */
		      regs[40] = 0;
		      initialize_perf_control_reg (perf_event, 0);
 	  	      break;

		    case '2': /* Set performance counter1 event. */
		      regs[41] = 0;
		      initialize_perf_control_reg (perf_event, 1);
		      break;
		
		    case '3': /* Get performance counter0 event. */
		      { 
			/* Pass performance counter0 event and counter to
			   the debugger.  */
			cvmx_perf_control_t p;
			p.u32 = regs[42];
			uint64_t perf_event = p.s.event;
                        mem2hex ((uint32_t)&(regs[40]), debug_output_buffer, 8);
			debug_output_buffer[16] = ',';
		        mem2hex ((uint32_t)&(perf_event), debug_output_buffer+17, 8);
                        putpacket (debug_output_buffer);
		      }
		      break;

		    case '4': /* Get performance counter1 event. */
		      {
			/* Pass performance counter1 event and counter to
			   the debugger.  */
			cvmx_perf_control_t p ;
			p.u32= regs[43];
			uint64_t perf_event = p.s.event;
                        mem2hex ((uint32_t)&(regs[41]), debug_output_buffer, 8);
			debug_output_buffer[16] = ',';
		        mem2hex ((uint32_t)&(perf_event), debug_output_buffer+17, 8);
                        putpacket (debug_output_buffer);
		      }
		      break;
		}
	     }	
	     break;

#if 0
	case 't': /* Return the trace buffer read data register contents. */
	     {
		uint64_t tra_data;
		uint64_t tra_ctl;
		char tmp[64];
		
		tra_ctl = octeon_read64(OCTEON_TRA_CTL);
		
		/* If trace buffer is disabled no trace data information is available. */
		if ((tra_ctl & 0x1) == 0)
		{
		   putpacket("!Trace buffer not enabled\n");
		   putpacket("t");
		}
		else
		{
		   putpacket("!Trace buffer is enabled\n");
		   tra_data = octeon_read64(OCTEON_TRA_READ_DATA);
		   mem2hex (&tra_data, tmp, 8);
		   strcpy (debug_output_buffer, "t");
		   strcat (debug_output_buffer, tmp);
		   putpacket(debug_output_buffer);
		}
	     }
	     break;
#endif
		
        case 'Z': /* Insert hardware breakpoint */
            {
                uint64_t num;
                uint64_t addr;
	        char bkt_type = *buf++;

                if (hexToLong(&buf, &num)
                    && *buf++ == ','
                    && hexToLong(&buf, &addr))
                {
		    switch (bkt_type)
		    {
			case 'i':	// Instruction hardware breakpoint
                    	    octeon_write64(HW_INSTRUCTION_BREAKPOINT_ADDRESS(num), addr);
                    	    octeon_write64(HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(num), 0);
                    	    octeon_write64(HW_INSTRUCTION_BREAKPOINT_ASID(num), 0);
                    	    octeon_write64(HW_INSTRUCTION_BREAKPOINT_CONTROL(num), 5);
			    break;
			case 'd':	// Data hardware breakpoint
			    {
				uint64_t hw_ctl = 5;
				uint64_t size, type;
				octeon_write64(HW_DATA_BREAKPOINT_ADDRESS(num), addr);
				octeon_write64(HW_DATA_BREAKPOINT_ADDRESS_MASK(num), 0);
				octeon_write64(HW_DATA_BREAKPOINT_ASID(num), 0);
			
				hexToLong(&buf, &size);
				buf++;
				/* type = 2, store, type = 1, load and type = 3, access */
				hexToLong(&buf, &type);
				hw_ctl |= (type == 2) ? 0x1000 : ((type == 1) ? 0x2000 : 0);
				/* Get the bytelane depending on the size. Assuming the
				   address is aligned. */
				hw_ctl |= (size == 2) ? 0x0c0 : ((size == 4) ? 0x0f0 : 0xff0);	
				octeon_write64(HW_DATA_BREAKPOINT_CONTROL(num), hw_ctl);
				break;
			     }
			default:
		 	   break;
		     }
                }
            }
            break;

        case 'z': /* Remove hardware breakpoint */
            {
                uint64_t num;
	        char bpt_type = *buf++;

                if (hexToLong(&buf, &num))
                {
		    switch (bpt_type)
		    {
			case 'i':	// Instruction hardware breakpoint
                    	    octeon_write64(HW_INSTRUCTION_BREAKPOINT_ADDRESS(num), 0);
                    	    octeon_write64(HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(num), 0);
                    	    octeon_write64(HW_INSTRUCTION_BREAKPOINT_ASID(num), 0);
                    	    octeon_write64(HW_INSTRUCTION_BREAKPOINT_CONTROL(num), 0);
			    break;
			case 'd':	// Data hardware breakpoint
			    octeon_write64(HW_DATA_BREAKPOINT_ADDRESS(num), 0);
			    octeon_write64(HW_DATA_BREAKPOINT_ADDRESS_MASK(num), 0);
			    octeon_write64(HW_DATA_BREAKPOINT_ASID(num), 0);
			    octeon_write64(HW_DATA_BREAKPOINT_CONTROL(num), 0);
			    break;
			default:
			    break;
		     }
                }
            }
            break;

        case 's':   /* Single step. sAA..AA Step one instruction from AA..AA (optional) */
            {
                /* We will need to send the break signal when we reenter the
                    exception handler */
                state.s.send_break = 1;
                debug_update_state(state);
                result = COMMAND_STEP;
            }
            break;

        case 'c':   /* Continue. cAA..AA Continue at address AA..AA (optional) */
            {
                /* We will need to send the break signal when we reenter the
                    exception handler */
                state.s.send_break = 1;
                debug_update_state(state);
                result = COMMAND_CONTINUE;
            }
            break;

        case '+':   /* Don't know. I think it is a communications sync */
            /* Ignoring this command */
            break;

        default:
            DEBUG_PRINTF("Unknown debug command: %s\n", debuggerCommand);
            break;
    }

    return result;
}


/**
 * Main debug exception handler. Called from interrupt context
 * when a debug exception occurs. Anything that causes an
 * exception here will be very bad. Remember there isn't a
 * heap. Avoid C library calls.
 */
void cvmx_debugger_exception(void)
{
    const uint32_t          coreid          = get_core_num();
    debug_state_t           state;
    debug_register_format_t debug_register;
    char 		    debug_status_buffer[30];

    /* COP0_MDEBUG_REG |= GSDBBP | Enable MCD0 | Clear MCD0 */
    volatile uint64_t *regs = get_debugger_register_storage_ptr();
    regs[39] |= 0x1100;

    /* The debug register is used to determine why we stopped */
    debug_register.u64 = regs[38];

    /* All cores stop on any exception. If a core isn't in the active core
       mask we need to start him up again. We can only do this if the core
       didn't hit a breakpoint or single step.
       If the core hit CVMX_CIU_DINT interrupt (generally happens when
       while executing _exit() at the end of the program). Remove the 
       core from known cores so that when the cores in active core mask 
       are done executing the program, the focus will not be transfered 
       to this core. */
    state = debug_get_state();
    if (((state.s.active_cores & (1<<coreid)) == 0) &&
        (debug_register.s.dbp == 0) && (debug_register.s.dss == 0)
        && (debug_register.s.dint != 1))
    {
        regs[38] &= ~0x100ull;    /* Clear SSt bit */
        if (DEBUG_CORE_STATE)
            DEBUG_PRINTF("Core #%u not in active cores, continuing.\n", coreid);
        return;
    }

    /* Check to see if the application has exited, if so, process */
    uint32_t *debug_flags_ptr = (uint32_t *)(0x80000000 | BOOTLOADER_DEBUG_FLAGS_BASE);
    if (debug_flags_ptr[coreid] & DEBUG_FLAG_CORE_DONE)
    {
        if (DEBUG_CORE_STATE)
            DEBUG_PRINTF("CORE %d exited application\n", coreid);
        cvmx_debugger_finish();
        return;
    }

    /* Delay the focus core a little if it is likely another core needs to
        steal focus. Once we enter the main loop focus can't be stolen */
    if ((debug_register.s.dss == 0) &&  /* Not single stepping */
        (debug_register.s.dbp == 0) &&  /* Didn't hit a software break */
        (coreid == state.s.focus_core)) /* and we're the focus core */
    {
        volatile int i;
        for (i=0;i< 10000; i++)
        {
            /* Spin giving the breakpoint core time to steal focus */
        }
    }

    /* The following section of code does two critical things. First, it
        populates the handler_cores bitmask of all cores in the exception
        handler. Only one core at a time can update this field. Second it
        changes the focus core if needed. */
    cvmx_spinlock_lock(&debug_lock);

    /* Display the message in the critical section so we can see the exact
        order cores are entering in */
    if (DEBUG_CORE_STATE)
        DEBUG_PRINTF("Core #%u stopped\n", coreid);

    /* Add this core to the in handler bit field */
    state = debug_get_state();
    state.s.handler_cores |= (1 << coreid);

    /* Check how this core got here. We may want to elect him the focus
        core if he hit a breakpoint. Or the core hit a Instruction
	breakpoint exception. */
    if (debug_register.s.dbp || debug_register.s.dib
	|| debug_register.s.ddbs || debug_register.s.ddbl)
    {
	strcpy (debug_status_buffer, "T9");
	
	/* Debug data breakpoint store/load exception occured. */
	if (debug_register.s.ddbs || debug_register.s.ddbl)
	{
	    int status = octeon_read64(HW_DATA_BREAKPOINT_STATUS);
	    octeon_write64(HW_DATA_BREAKPOINT_STATUS, status&(~0xf));
	    regs[38] &= ~0xcull;     // clear ddbs and ddbl bits.
	    debug_status_buffer[1] = '8';
	    debug_status_buffer[2] = ':';
	    mem2hex ((uint32_t)&status, debug_status_buffer+3, sizeof(status));
	}

	if (debug_register.s.dib)
	{
	    int status = octeon_read64(HW_INSTRUCTION_BREAKPOINT_STATUS);
            octeon_write64(HW_INSTRUCTION_BREAKPOINT_STATUS, status&(~0xf));
            regs[38] &= ~0x10ull;   // clear dib bit.
	}
        /* Since he hit a breakpoint, elect this core focus. This can
            only be done if another core isn't already focused in the
            exception handler */
        if ((state.s.handler_cores & (1<<state.s.focus_core)) == 0)
        {
            sendCoreMessage(coreid, "taking focus.");
            state.s.focus_core = coreid;
        }
    }

    /* Push all updates before exiting the critical section */
    debug_update_state(state);
    cvmx_spinlock_unlock(&debug_lock);

    /* If we previously did a step or continue the debugger will be expecting
        a break signal. Send this if the send_break bit is set */
    if ((coreid == state.s.focus_core) && state.s.send_break)
    {
        putpacket(debug_status_buffer);
    }
#if 0
    else
    {
	uint64_t trace_interrupt;

 	trace_interrupt = octeon_read64(OCTEON_TRA_INT_STATUS);
	if (trace_interrupt & 0x8)
	{
	    putpacket("!Interrupt from trace buffer, full threshold occured");
	    putpacket("T9");
	    octeon_write64(OCTEON_TRA_INT_STATUS, 0x8);
	}
	else if (trace_interrupt & 0x4)
	{
	    putpacket("!Interrupt from trace buffer, end trigger occured");
	    putpacket("T9");
	    octeon_write64(OCTEON_TRA_INT_STATUS, 0x4);
	}
     }
#endif

    /* Remember the current value of command_count. This way we know when it
        is incremented and we need to start running. By default we aren't
        running, just waiting for the debugger to tell us what to do */
    uint64_t old_command_count = state.s.command_count;
    command_t command;
    do
    {
        /* Get which core is the focus. It can change while this loop is running */
        state = debug_get_state();
        if (coreid == state.s.focus_core)
        {
            const char *debuggerCommand = getpacket(debug_input_buffer, sizeof(debug_input_buffer));
            if (debuggerCommand)
            {
                command = processDebuggerCommand(debuggerCommand);
            }
            else
                command = COMMAND_NOP;

            /* If the step_all flag is set we need to propagate the command
                to all cores */
            state = debug_get_state();
            if (state.s.step_all && (command != COMMAND_NOP))
            {
                state.s.command = command;
                state.s.command_count++;
                debug_update_state(state);
            }
        }
        else
        {

            cvmx_ciu_intx0_t irq_control;
            irq_control.u64 = octeon_read64(OCTEON_CIU_INTX_EN0(coreid * 2));

            /* Check if this core is the one that processes interrupts */
            if (irq_control.s.uart & (1<<debug_uart))
            {
                /* It is possible the focus core has completed processing
                    and exited the program. When this happens the focus core
                    will not be in known_cores. If this is the case we need
                    to elect a new focus. */
                if ((state.s.known_cores & (1<<state.s.focus_core)) == 0)
                {
                    state = debug_get_state();
                    state.s.focus_core = coreid;
                    debug_update_state(state);

                    sendCoreMessage(state.s.focus_core, "taking focus.");
                    if (state.s.send_break)
                        putpacket("T9");
                }
                else if ((state.s.handler_cores & (1<<state.s.focus_core)) == 0)
                {
                    /* Interrupts are suppressed when we are in the debug
                        exception handler. We need to poll the uart status and see
                        if we need to stop everybody. */
                    cvmx_uart_lsr_t lsrval;
                    lsrval.u64 = octeon_read64(OCTEON_MIO_UARTX_LSR(debug_uart));
                    if (lsrval.s.dr)
                    {
                        register uint64_t tmp;
                        /* Pulse MCD0 signal on Ctrl-C to stop all the cores. */
                        asm volatile (
                            ".set push\n"
                            ".set noreorder\n"
                            ".set mips64\n"
                            "dmfc0 %0, $22\n"
                            "ori   %0, %0, 0x1110\n"
                            "dmtc0 %0, $22\n"
                            ".set pop\n"
                            : "=r" (tmp));
                    }
                }
            }

            /* See if the command issued count has incremented. If it has we
                need to use the new command. Otherwise we're just spinning on
                NOP commands */
            if (old_command_count != state.s.command_count)
            {
                command = state.s.command;
                old_command_count = state.s.command_count;
            }
            else
                command = COMMAND_NOP;
        }
    } while (command == COMMAND_NOP);

    /* If single step then set SSt bit of Debug Register to 1. */
    if (command == COMMAND_STEP)
    {
        /* COP0_DEBUG_REG = reg[38] */
        regs[38] |= 0x100ull;     /* Set SSt bit */
    }
    else
    {
        /* COP0_DEBUG_REG = reg[38] */
        regs[38] &= ~0x100ull;    /* Clear SSt bit */
    }

    if (DEBUG_CORE_STATE)
        DEBUG_PRINTF("Core #%u running\n", coreid);

    cvmx_spinlock_lock(&debug_lock);
    state = debug_get_state();
    state.s.handler_cores ^= (1 << coreid);
    debug_update_state(state);
    cvmx_spinlock_unlock(&debug_lock);
}


/**
 * Initialize the debugger for use
 */
void cvmx_debugger_initialize(void)
{
    const uint32_t  coreid = get_core_num();
    debug_state_t   state;
    int             first_core;

    /* Set the multicore debug register so that pulses on MCD0 cause
        all cores to stop. The register layout is:

        MultiCoreDebug Implemented at reg# = 22, sel = 0

         Used for multi-core debugging. See the debug section for more description.
         Here is the (big-endian bit) description:

         uint64    mbz        :47;
         uint64 DExcCodeVal   : 1; // RO -> set to one when Debug[DExcCode] is valid and should be interpreted
                                   // i.e. is set for Debug Mode Exceptions
         uint64 CountGSTOPEN  : 1; // RW, reset to zero -> if set, Count does not increment when GSTOP is set
         uint64 CvmCountGSTOPEN:1; // RW, reset to zero -> if set, CvmCount does not increment when GSTOP is set
         uint64 CvmCountDM    : 1; // RW, reset to zero -> if set, CvmCount does not increment when Debug[DM] set
         uint64   GSDBBP      : 1; // R/W, reset to zero -> if one, SDBBP execution pulses global MCD0 wire
         uint64    mbz2       : 1;
         uint64   MaskMCD2    : 1; // R/W, reset to zero -> if one, pulses on the global MCD2 wire set MCD2
         uint64   MaskMCD1    : 1; // R/W, reset to zero -> if one, pulses on the global MCD1 wire set MCD1
         uint64   MaskMCD0    : 1; // R/W, reset to zero -> if one, pulses on the global MCD0 wire set MCD0
         uint64    mbz3       : 1;
         uint64   PulseMCD2   : 1; // RAZ/W -> a write of a one pulses global MCD2 wire
         uint64   PulseMCD1   : 1; // RAZ/W -> a write of a one pulses global MCD1 wire
         uint64   PulseMCD0   : 1; // RAZ/W -> a write of a one pulses global MCD0 wire
         uint64    mbz4       : 1;
         uint64    MCD2       : 1; // R/W1C, reset to zero -> If set, enter debug mode
         uint64    MCD1       : 1; // R/W1C, reset to zero -> If set, enter debug mode
         uint64    MCD0       : 1; // R/W1C, reset to zero -> If set, enter debug mode */

    cvmx_spinlock_lock(&debug_lock);

    state = debug_get_state();
    first_core = (state.s.known_cores == 0);
    state.s.known_cores |= (1 << coreid);
    debug_update_state(state);

    cvmx_spinlock_unlock(&debug_lock);

    if (first_core)
    {
        /* Delay so that we are sure we are last */
	octeon_delay_cycles(30000);

        /* Initialize the uart port 1 which is used for debugging. */
	if (debug_uart)
          octeon_uart_setup(debug_uart);

        if (DEBUG_CORE_STATE)
            DEBUG_PRINTF("cvmx_debugger_initialize core: %d\n", coreid);

        /* Initialize the debugger state. We're assuming that all cores that
            should have been added to the known_cores above */
        state = debug_get_state();
        state.s.handler_cores = 0;
        state.s.command = COMMAND_NOP;
        state.s.command_count = 0;
        state.s.focus_core = coreid;
        state.s.step_all = 0;
        state.s.send_break = 0;
        state.s.active_cores = state.s.known_cores;
        if (DEBUG_CORE_STATE)
            DEBUG_PRINTF("Known cores at init: 0x%x\n", state.s.known_cores);

        debug_update_state(state);

        debug_init_complete = 1;
        OCTEON_SYNCW;
    }

    while (!debug_init_complete)
    {
        /* Spin waiting for init to complete */
    }
}


/**
 * Inform debugger about the end of the program. This is
 * called from crt0 after all the C cleanup code finishes.
 * Our current stack is the C one, not the debug exception
 * stack. */
void cvmx_debugger_finish(void)
{
    const uint32_t coreid = get_core_num();
    debug_state_t state;

    if (DEBUG_CORE_STATE)
        DEBUG_PRINTF("Core #%u has finished execution.\n", coreid);

    cvmx_spinlock_lock(&debug_lock);

    state = debug_get_state();
    state.s.known_cores ^= (1 << coreid);
    debug_update_state(state);

    /* Notify the debugger if all cores have completed the program */
    if ((state.s.known_cores == 0) && (state.s.send_break))
    {
        if (DEBUG_CORE_STATE)
            DEBUG_PRINTF("All cores done!\n");
        putpacket("D0");
    }
    else
    {
        /* If this core is the one processing uart interrupts we need to
            select a different core. Once this core finishes it can't answer
            interrupts anymore. Users get upset when Control-C stops working. */
        cvmx_ciu_intx0_t irq_control;
        irq_control.u64 = octeon_read64(OCTEON_CIU_INTX_EN0(coreid * 2));
        if (irq_control.s.uart & (1<<debug_uart))
        {
            /* Loop through cores looking for someone to handle interrupts.
                Since we already check that known_cores is non zero, this
                should always find a core */
            uint32_t cpu;
            for (cpu=0;cpu<16;cpu++)
            {
                if (state.s.known_cores & (1<<cpu))
                {
                    if (DEBUG_CORE_STATE)
                        DEBUG_PRINTF("Routing uart interrupts to Core #%u.\n", cpu);

                    irq_control.u64 = octeon_read64(OCTEON_CIU_INTX_EN0(cpu * 2));
                    irq_control.s.uart|=(1<<debug_uart);
                    octeon_write64(OCTEON_CIU_INTX_EN0(cpu * 2), irq_control.u64);
                    break;
                }
            }

            /* Disable interrupts to this core since he is about to die */
            octeon_write64(OCTEON_CIU_INTX_EN0(coreid * 2), 0);
        }
    }

    cvmx_spinlock_unlock(&debug_lock);
}


/**
 * Change the debug uart port
 *
 * @param uart   New uart port
 */
void cvmx_debugger_set_uart(int uart)
{
    debug_uart = uart;
    OCTEON_SYNCW;
}
#endif
