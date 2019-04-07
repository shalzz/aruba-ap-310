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
    *  Filename: mde_proto.h
    *
    *  Function: 
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */
#ifndef MDE_PROTO_H
#define MDE_PROTO_H
/// defined in mde common src files

extern void mde_entry (unsigned info );

extern void verifyNewMde ( sMde_t *mde );
extern void setupNewMdeWithCommonInfo ( sMde_t *mde );

extern MDE_CMD *mdeParseCmds ( sMde_t *mde);
extern MDE_CMD *getMdeCommonCommands ( unsigned *uCommandCount );

extern int dbg_printf(const char *templat,...);
extern void get_user_input ( sMde_t *mde);
extern void mdeScanf ( sMde_t *mde, char *ctl, ...);
extern void mdeInit ( sMde_t *mde );

#if PLATFORM != HOST_LINUX

extern char *strcpy(char *dest,const char *src);
extern char *strncpy(char *dest,const char *src, int cnt);
extern char *strchr(const char *dest,int c);
#endif

/// Common MDE commands

extern unsigned mdeMemRead    ( sMde_t *mde );
extern unsigned mdeMemWrite   ( sMde_t *mde );
extern unsigned mdeShowMemory ( sMde_t *mde );
extern unsigned mdeMemFill    ( sMde_t *mde );
extern unsigned mdeMemVerify  ( sMde_t *mde );

extern unsigned mdeRandomFill   ( sMde_t *mde );
extern unsigned mdeRandomVerify ( sMde_t *mde );

extern unsigned mdeMemSearch  ( sMde_t *mde );
extern unsigned mdeMemCompare ( sMde_t *mde );
extern unsigned mdeMemCmpBufs ( sMde_t *mde );
extern unsigned mdeGetChecksum( sMde_t *mde );

extern unsigned mdeShowInfo   ( sMde_t *mde );

extern unsigned mdeRegRead    ( sMde_t *mde );
extern unsigned mdeRegWrite   ( sMde_t *mde );

extern unsigned mdeGetTime    ( sMde_t *mde );

////

extern unsigned mdeVerifyBlk (unsigned addr, unsigned count, unsigned data, unsigned incr, sMde_t *mde);
extern void     mdeFillBlk (unsigned *addr, int count, unsigned data, unsigned incr, sMde_t *mde);
extern void     mdeShowBlk (sMde_t *mde, int count);
extern unsigned mdeQuery ( sMde_t *mde, char *question, unsigned original);
/// memory test related

extern unsigned mtEditConfig ( sMde_t *mde );
extern unsigned mtShowConfig ( sMde_t *mde );
extern unsigned mtAutoRun    ( sMde_t *mde );
extern unsigned mtRunTest    ( sMde_t *mde );

//// mips_rd_eye_plot needs to be upgraded to use sMde_t
extern unsigned mips_rd_eye_plot ( char *line );
extern unsigned mde_rd_eye_plot ( sMde_t *mde );
extern unsigned mde_test_srdi (sMde_t *mde );
extern unsigned mde_test_mtm (sMde_t *mde );

extern unsigned mde_test_flush (sMde_t *mde );


extern void mde_fdiv (unsigned val, unsigned divr, int place, sMde_t *mde );
extern void find_dq_init_all (struct MDE0 *, int);

extern inline int find_all_errors_all    (struct MDE0 *, int);
extern inline int find_all_errors_all_16 (struct MDE0 *, int);

extern int find_dq_show_result_all    (struct MDE0 *, int);
extern int find_dq_show_result_all_16 (struct MDE0 *, int);

/// defined in mde_<local>.c

//extern sMde_t *setupNewMde ( sMde_t *newMde );
extern sMde_t *setupNewMdeWithLocalSpecificInfo ( sMde_t *newMde );

#endif // MDE_PROTO_H
