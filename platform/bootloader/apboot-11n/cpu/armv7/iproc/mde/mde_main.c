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
    *     WARRANTIES OF MRCHANTABILITY, FITNESS FOR A PARTICULAR
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
    *  Broadcom Memory Diagnostics Environment (MDE)
    *********************************************************************
    *  Filename: mde_main.c
    *
    *  Function:
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */

#include "mde_common_headers.h"


const char mde_banner [] = \
"\n"
"##############################################\n\n"
"     CDE version %d.%d %08x\n"
"     Built on %s at %s\n\n"
"     Platform: %s:%d\n\n"
"##############################################\n\n"
"Enter ? for help\n"
"Enter 'help <command | alias>' for details\n\n";
/////

static unsigned hdeHandleNewKey (unsigned new, unsigned last, sMde_t *mde, int which, int limit)
{
  if (((last == SPACE) && (new == SPACE)) || (which >= limit)) return NO_ACTION;

  if ((new >= SPACE) && (new <= DEL)) {
    // regular character entered
    mde -> user_line [which] = (char) new;
    host_console_write (new);
    return ADD_ONE;    
  }
  // handle special keys

  switch (new) {
  case ESC:  return NO_ACTION;
  case DEL:
  case BS:
    if (which == 0) return NO_ACTION;
    host_printf ("\b \b");
    return SUB_ONE;    
    break;
  case CR:
    host_printf ("\n");
    mde -> user_line [which] = 0;
    return ADD_ONE;

  default:
    return NO_ACTION;
    break;
  }
}

void get_user_input ( sMde_t *mde)
{
  unsigned new_key;
  int inx, result;
//  unsigned val ;

//  int count;
//  int take;

  char /*this,*/ last = 0;

  inx = 0;
  while (CONTINUE_UNTIL_DONE) {
	//    while ( mde -> console_rx_ready () == 0) {
	//	}
    new_key = host_console_read ();
    result = hdeHandleNewKey (new_key, last, mde, inx, DIAG_SIZE_256);
    inx += result;
    if ((new_key == CR) || (new_key == LF)) break;
	last = new_key;
  }
  mde -> user_line [inx] = 0; // terminate the raw input line
}

void getuserline ( sMde_t *mde)
{
  host_printf ("CDE-%d >> ", mde->chip_id );
  
  get_user_input ( mde );
}

#define DBG(val)    host_printf ("step %08x\n\n", val)

extern unsigned host_get_elapsed_msec(void);
extern unsigned host_local_init(void);

void mde_entry (unsigned info )
{
  sMde_t mde;
  sMde_t *pThisMde;
  unsigned elapse_ms;
  MT_TIME start;
  MT_TIME done;
  unsigned retval;

  DBG(0x1000);
  setupNewMdeWithCommonInfo ( &mde );

  DBG(0x1001); 
  pThisMde = setupNewMdeWithLocalSpecificInfo ( &mde );
  DBG(0x1002);

  // do local chip specific init
  if ((retval = host_local_init ()) != 0) {
	host_printf ("\nWARNING: host_local_init returned status %08x\n", retval );
  }

  DBG(0x1003);

  // do common mde init
  mdeInit ( &mde );
  DBG(0x1004);
  // for debug and verifying the newly setup mde struct
  mdeShowInfo ( &mde );
  DBG(0x1005);
  // print banner 
  host_printf (mde_banner,  MDE_VER_MAJ, MDE_VER_MIN, MDE_DATE_CODE,  __DATE__, __TIME__,
			  mde.chip_name, mde.chip_id );
  DBG(0x1006);
  // read to go into CLI

  while (1) {
	getuserline (&mde);
	if (mde.user_line [0] == 'q') break;
	mde.current_command = mdeParseCmds ( &mde );
	host_printf ("\n");
	start.msec = host_get_elapsed_msec ();
	mde.current_command -> handler ( &mde );
	if (strchr (mde.user_line, '$') != 0) {
	  done.msec = host_get_elapsed_msec ();
	  elapse_ms = done.msec - start.msec;
	  host_printf ("\n\tLast command took %d ms to complete\n", elapse_ms);
	}
  }
  host_printf ("\nExit from CDE now (HALT)\n");
}
