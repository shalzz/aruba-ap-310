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
    *  Filename: mde_def.h
    *
    *  Function: 
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */
#ifndef MDE_DEF_H
#define MDE_DEF_H

#define BSIZE_16  16
#define BSIZE_64  64 
#define BSIZE_128 128 
#define BSIZE_256 256 

//// Test result, return code
#define CDEC_PASS 1
#define CDEC_FAIL 2
#define CDEC_OK   3
#define CDEC_BAD  4

#define TRUE      1
#define FALSE     0
#define NO_ACTION 0

//// Loop control
#define NON_STOP            1
#define CONTINUE_UNTIL_DONE 1
#define ADD_ONE             1
#define SUB_ONE            -1

//// ASCII 
#if PLATFORM != HOST_LINUX
#ifndef NULL
#define NULL  0
#endif
#endif

#define BS    8
#define HTAB  9
#define LF    0xa
#define VTAB  0xb
#define CR    0xd
#define ESC   0x1b
#define DEL            0x7e
#define EOS            0

#define SPACE 0x20
#define LAST_PRINTABLE_ASCII 0x7e

#define SIGNATURE 0x72654443
/////
//  Popular sizes
#define DIAG_SIZE_16         16
#define DIAG_SIZE_32         32
#define DIAG_SIZE_64         64
#define DIAG_SIZE_128        128
#define DIAG_SIZE_256        256
#define DIAG_SIZE_512        512
#define DIAG_SIZE_1024       1024
#define DIAG_SIZE_2048       2048

#define DIAG_SIZE_TINY       16
#define DIAG_SIZE_SMALL      64
#define DIAG_SIZE_MEDIUM     256
#define DIAG_SIZE_BIG        1024
#define DIAG_SIZE_HUGE       4096

#define DIAG_CMDBUF_SIZE     128
#define DIAG_SIZE_ERRLOG     256
////
// RETURN values used by CDE
#define DIAG_RET_OK          1
#define DIAG_RET_BAD         2
#define DIAG_TEST_PASSED     3
#define DIAG_TEST_FAILED     4
#define DIAG_ENABLED         5
#define DIAG_DISABLED        6
#define DIAG_NOT_YET         7

#define TEST_NOT_DONE        8
#define TEST_COMPLETED       9

#define DIAG_NO_DICE         -1

// Bit values
#define BIT_0                0x00000001
#define BIT_1                0x00000002
#define BIT_2                0x00000004
#define BIT_3                0x00000008
#define BIT_4                0x00000010
#define BIT_5                0x00000020
#define BIT_6                0x00000040
#define BIT_7                0x00000080
#define BIT_8                0x00000100
#define BIT_9                0x00000200
#define BIT_10               0x00000400
#define BIT_11               0x00000800
#define BIT_12               0x00001000
#define BIT_13               0x00002000
#define BIT_14               0x00004000
#define BIT_15               0x00008000
#define BIT_16               0x00010000
#define BIT_17               0x00020000
#define BIT_18               0x00040000
#define BIT_19               0x00080000
#define BIT_20               0x00100000
#define BIT_21               0x00200000
#define BIT_22               0x00400000
#define BIT_23               0x00800000
#define BIT_24               0x01000000
#define BIT_25               0x02000000
#define BIT_26               0x04000000
#define BIT_27               0x08000000
#define BIT_28               0x10000000
#define BIT_29               0x20000000
#define BIT_30               0x40000000
#define BIT_31               0x80000000
#define ALL_1                0xFFFFFFFF
#define DO_FOREVER           1
#define DO_UNTIL_FOUND       1
#define DO_UNTIL_DONE        1
#define DO_UNTIL_CR          1
#define KEYHIT_OR_TIMEOUT    1

#define STR_EQU              0
#define MAX_TOKEN_SUPPORTED  16

#define ALIGN_32BITS        0xFFFFFFFC
#define ALIGN_16BITS        0xFFFFFFFE

#define INVALID_ADDRESS     ALL_1
#define BYTES_PER_WORD      sizeof(uint32_t)
#define ONE_MB              (1024 * 1024)

#define HISTORY_DEPTH       8

#endif // MDE_DEF_H
