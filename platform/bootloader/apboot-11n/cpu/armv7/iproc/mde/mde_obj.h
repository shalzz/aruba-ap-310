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
    *  Broadcom Memoroy Diagnostics Environment (MDE)
    *********************************************************************
    *  Filename: mde_obj.h
    *
    *  Function: 
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */
#ifndef MDE_OBJ_H
#define MDE_OBJ_H

// memory access support


// prepare struct MDE to be referenced before it is fully defined
struct MDE0;

// prepare MDE_CMD so that it can use the struct MDE before it is fully defined
struct SMDE_CMD {
  /* Properties */
  char *name;
  char *alias;
  char *usage;
  char *details;
  /* Methods */
  unsigned (*handler)(struct MDE0 *);
};

typedef struct SMDE_CMD MDE_CMD;

typedef struct {
  MDE_CMD *cmds;
  int cmd_count;
} MDE_CMD_GROUP;

//#define MEMBUF_SIZE_BYTES 256
#define MEMBUF_SIZE_BYTES 256
#define MEMBUF_SIZE_INTS  (MEMBUF_SIZE_BYTES/4)

typedef struct {
  unsigned address;
  unsigned size_ints;
  unsigned data [MEMBUF_SIZE_INTS];
} MEM_ACCESS_BUF;

typedef struct {
  unsigned id;
  unsigned start;
  unsigned bsize;
  unsigned physical_addr;
} MEM_REGION;

typedef struct {
  unsigned sec;
  unsigned msec;
} MT_TIME;

/// Memory tester

typedef struct {
  unsigned id;
  char     name [BSIZE_64];
  unsigned enabled;       // for locking out the test if necessary
  unsigned done;          // for exit condition check during main test loop
  // memory range - need both cached and uncached address
  // if this is not appropriate - then set both addresses the same
  unsigned start_addr_cached;
  unsigned start_addr_uncached;
  unsigned test_bsize;    // size in bytes
  unsigned test_wsize;    // size in INTS

  unsigned *p_read;
  unsigned seed;
  unsigned uPeriodStartInSec;
  unsigned uPeriodStartInMilliSec;

  // random number generator
  tinymt32_t tiny;
  // error handling support

  unsigned uAddxFail[ADDR_FAIL_CNT];  // failed address array 
  unsigned uAddrFailCnt;
  unsigned uTotalErr;
  unsigned uTotalErrMap;

  unsigned addx;
  unsigned act_data;
  unsigned exp_data;
  // DQ, cyc for this tester
  unsigned result [32][16]; // accomodates 16bit PHY

  // interface stressor info
  // The stressor performs write/read test in the background
  unsigned stressor_present; // 1 = present 0 = no stressor
  unsigned stressor_address; // The address of memory to test
  unsigned stressor_bsize;    // size in bytes
  unsigned stressor_enabled;
  // methods
  unsigned (*stressor_init)  ( struct MDE0 *);
  unsigned (*stressor_start) ( struct MDE0 *);
  unsigned (*stressor_status)( struct MDE0 *);
  unsigned (*stressor_stop)  ( struct MDE0 *);
} MEM_TESTER;

typedef struct {
  unsigned id;
  char name [DIAG_SIZE_64];
  unsigned (*handler) ( struct MDE0 *);
} MT_MODE_HANDLER;


// fill in the details of struct MDE
typedef struct MDE0 {
  // common properties
  unsigned version;
  unsigned date;
  char     name [DIAG_SIZE_64];
  // CLI related support
  char     user_line [DIAG_SIZE_256];
  char     history   [HISTORY_DEPTH][DIAG_SIZE_256];
  unsigned cli_history_depth;
  MDE_CMD_GROUP common;
  MDE_CMD_GROUP local;

  MDE_CMD  *current_command;
  // Memory access support
  MEM_ACCESS_BUF bread;
  MEM_ACCESS_BUF bwrit;

  // Local memory region: LOCAL_MEM_BLOCK_COUNT is defined in mde_local_defs.h
  // The size of MDE is dependent of how many memory blocks are defined on the target chip
  unsigned mem_blks_count; // should be the same as LOCAL_MEM_BLOCK_COUNT
  unsigned mem_total_bytes;
  MEM_REGION mem_blks  [ LOCAL_MEM_BLOCK_COUNT ];


  // memory tester related support
  unsigned memtester_count;
  unsigned memtester_pass;
  unsigned memtester_time_seconds;

  // This is the memory test mode ( 0, 1, 2, 3, 5 )
  unsigned memtester_mode;
  unsigned memtester_loop;
  unsigned memtester_exit;

  MEM_TESTER memtester [ LOCAL_MEM_TESTER_COUNT];

  // test mode related - one handler per mode
  unsigned mt_mode_count;
  unsigned mt_total_mb_tested;
  unsigned mt_total_mb_per_loop;
  unsigned mt_total_read_time_ms;

  // This is the handler per the mode specified
  // This is filled in at runtime
  MT_MODE_HANDLER *mt;

  // random generator related
  unsigned random_seed;

  // time related

  MT_TIME time_start;
  MT_TIME time_now;

  // chip specific properties
  char     chip_name [DIAG_SIZE_64];
  unsigned chip_id;
  unsigned phy_data_width; // 32 or 16

  // 
  unsigned reg_access_base;

  // build specific - DDR frequency is NOT a build option
  // The actual DDR frequency is set by the host at boot up time
  // 
  unsigned ddr_frequency;
  // methods
  // initialization related

  // memory test related 
  // This routine checks and show via printf any local information
  // The output from this routine is shown in the post-loop status report
  void     (*mt_local_status_report)(struct MDE0 *);

  // tester - fill in with a handler according to the mode specified
  unsigned (*memory_test) ( struct MDE0 * );

  // system hardware specific support
  unsigned (*get_chip_temperature) (void);
  unsigned (*get_avs_voltage) (void);
  unsigned (*get_chip_id)(void);

  // find_dq support
  void     (*fdq_init)       (struct MDE0 *, int which);
  int      (*fdq_log_error)  (struct MDE0 *, int which);
  int      (*fdq_show_result)(struct MDE0 *, int which);
} MDE;


typedef MDE sMde_t;

// For accessing bytes within an INT
typedef union {
  unsigned ival;
  unsigned char bval [4];
} ITOB;

#endif // MDE_OBJ_H
