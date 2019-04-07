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
    *  Filename: mde_common_headers.h
    *
    *  Function: declare all MDE headers
	*            This is included in all MDE C files
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */
#ifndef MDE_COMMON_HEADERS_H
#define MDE_COMMON_HEADERS_H

//// Include local header files

#include "mde_local_headers.h"

#include "tinymt32.h"

#include "mde_types.h"
#include "mde.h"
#include "mde_def.h"
#include "mde_mt.h"
#include "mde_obj.h"

#include "mde_proto.h"
#include "mde_plat.h"

// access for PHY registers
// These two header files provide 0 based offset

#include "mde_phy_word_lane.h"
#include "mde_phy_ctrl_regs.h"
#include "mde_shim_addr_cntl.h"

#include "mde_rd_eye.h"

#endif // MDE_COMMON_HEADERS_H
