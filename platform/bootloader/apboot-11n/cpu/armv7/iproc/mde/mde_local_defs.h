/*  *********************************************************************
    *  Copyright 2012
    *  Broadcom Corporation. All rights reserved.
    *
    *  This software is furnished under license and may be used and
    *  copied only in accordance with the following terms and
    *  conditions.  Subject to these conditions, you may download,
    *  copy, install, use, modify and distribute modified or unmodified
    *  copies of this software in source and/or binary form.  No title
    *  or ownership is transferred hereby.
    *
    *  1) Any source code used, modified or distributed must reproduce
    *     and retain this copyright notice and list of conditions as
    *     they appear in the source file.
    *
    *  2) No right is granted to use any trade name, trademark, or
    *     logo of Broadcom Corporation. Neither the "Broadcom
    *     Corporation" name nor any trademark or logo of Broadcom
    *     Corporation may be used to endorse or promote products
    *     derived from this software without the prior written
    *     permission of Broadcom Corporation.
    *
    *  3) THIS SOFTWARE IS PROVIDED "AS-IS" AND ANY EXPRESS OR
    *     IMPLIED WARRANTIES, INCLUDING BUT NOT LIMITED TO, ANY IMPLIED
    *     WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    *     PURPOSE, OR NON-INFRINGEMENT ARE DISCLAIMED. IN NO EVENT
    *     SHALL BROADCOM BE LIABLE FOR ANY DAMAGES WHATSOEVER, AND IN
    *     PARTICULAR, BROADCOM SHALL NOT BE LIABLE FOR DIRECT, INDIRECT,
    *     INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    *     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
    *     GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    *     BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
    *     OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    *     TORT (INCLUDING NEGLIGENCE OR OTHERWISE), EVEN IF ADVISED OF
    *     THE POSSIBILITY OF SUCH DAMAGE.
    *********************************************************************
    *  Broadcom Memoroy Diagnostics Environment (CDE)
    *********************************************************************
    *  Filename: mde_local_defs.h
    *
    *  Function: This file defines all end-user related symbols and API
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
    * 
    * $Id::                                                       $:
    * $Rev:: : Global SVN Revision = 1950                         $:
    * 
 */
#ifndef MDE_LOCAL_DEFS_H
#define MDE_LOCAL_DEFS_H


/*
  DDR_PHY_IN_SYSTEM is the number of DDR PHY on the end user's hardware
  PHY_WIDTH can be either 16 or 32
 */
#define DDR_PHY_IN_SYSTEM 1
#define PHY_WIDTH         16
#define CHIP_HAS_L2       1

#define MEMC_MODE         PHY_WIDTH

/*
  RDI_USE_MCP is used by Northstar to increase BW during RDI
 */
#define RDI_USE_MCP       1

/*
  LOCAL_MEM_BLOCK_COUNT is the number of memory regions visible to CDE
  For each DDR_PHY_IN_SYSTEM, the followings are required:

  1. There is a block of DRAM visible to CDE
     Some or all of the memory in this block is tested by CDE
  2. The end-user can specify the amount of DRAM to be tested.

 */
#define LOCAL_MEM_BLOCK_COUNT (DDR_PHY_IN_SYSTEM)

/*
  Define the size of each memory region visible to CDE
 */
#define HOST_MEM_SIZE_BYTES    0x08000000
#define HOST_MEM_SIZE_INTS     (HOST_MEM_SIZE_BYTES/4)
/*
  Define the size of each memory region to be tested by CDE
 */
//#define HOST_MEM_TSIZE_BYTES   HOST_MEM_SIZE_BYTES

// Set Northstar test memory range to 1M for testing purpose
#define HOST_MEM_TSIZE_BYTES   0x01000000  
#define HOST_MEM_TSIZE_INTS    (HOST_MEM_TSIZE_BYTES/4)


#define LOCAL_MEM_TESTER_COUNT 1


#ifndef NULL
#define NULL       0
#endif


/*
  CDE core function requires the following support.

  Each of these can be a macro or a subroutine.
  The API of these functions are as the followings:

  host_console_write - put an ascii chararcter to the console
       void host_console_write (uint32_t address_to_write_to, uint32_t data_to_write);

  host_console_read - read a character from the console
       uint32_t host_console_read (uint32_t address_to_read_from );

  host_console_rx_ready - check if a key is present at the console
       uint32_t host_console_rx_ready ( void);

	       returns 0 if no key presetns at console
		   returns 1 if a key is ready to be read from console

  host_printf - a formated print routine similar to a standard C printf
       int32_t host_printf(const char *templat,...);

  host_get_reg_base(which_phy) - returns the DDR PHY register base for subsequence access
       unsigned host_get_reg_base (which_phy);

  host_get_random_seed - returns a random value
       unsigned host_get_random_seed (void);

 */

/// NORTHSTAR specific implementation
#if 0
extern void     putc (unsigned val);
extern unsigned getc (void);
extern unsigned tstc (void);
extern unsigned printf (const char *fmt, ...);
#else
#include <common.h>
#endif

#define host_console_write    putc
#define host_console_read     getc
#define host_console_rx_ready tstc
#define host_printf           printf

#if 0
extern unsigned get_timer ();
#endif
//define host_get_elapsed_msec() get_timer(0)

#define NORTHSTAR_REG_BASE    0x18010800
/*
  The following three are used by mde_rdi.c which assumes MEMC exists
  Set them to 0 to use the REG_BASE above properly
 */
#define PHY_OFFSET            0
#define PHY_BASE_TYPE_0       0
#define WL_OFFSET             0

extern unsigned host_reg_read( unsigned offset);
extern void     host_reg_write( unsigned offset, unsigned data);

#define host_read_32bit_data(addr)        *(unsigned*)addr
#define host_write_32bit_data(addr, data) *(unsigned*)addr=data

extern unsigned host_block_write_32bit_data (unsigned to_addr, unsigned size_ints, unsigned *pwrite_data);

extern unsigned host_block_read_32bit_data (unsigned from_addr, unsigned size_ints, unsigned *pread_data);


#define HOST_SUPPORTS_BUFFERED_MEM_RW 1
#define DO_NON_MEMC_ERROR             0
#if DO_NON_MEMC_ERROR 
#define METHOD "NON MEMC error check"
#endif

#define DO_FULL_ERROR_REPORT          1
#if DO_FULL_ERROR_REPORT
#define METHOD "FDQ_PASS_THROUGH"
#endif

//#define RDI_PRINT printf
#define RDI_PRINT //

#define NORTHSTAR 1

#endif /// of MDE_LOCAL_DEFS_H
