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
    *  Broadcom Common Diagnostics Environment (CDE)
    *********************************************************************
    *  Filename: mde_cmd.c
    *
    *  Function:
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
    * 
    * $Id::                                                       $:
    * $Rev:: : Global SVN Revision = 1950                         $:
    * 
 */

#ifdef PLATFORM 
#if PLATFORM == HOST_LINUX
#warning: PLATFORM is LINUX
#include <linux/string.h>
#endif
#endif


#include <mde_common_headers.h>

static unsigned mdeHelp ( sMde_t *mde );

static unsigned mdeNoSuchCommand (  sMde_t *pMde )
{
  host_printf ("\nUnsupported user commnad [%s]\n\n", pMde -> user_line );
  return DIAG_RET_OK;
}

static unsigned mdeNoAction (  sMde_t *pMde )
{
  return DIAG_RET_OK;
}

static const MDE_CMD mdeNoSuch = { NULL, NULL, NULL, NULL, mdeNoSuchCommand };
static const MDE_CMD mdeNoAct  = { NULL, NULL, NULL, NULL, mdeNoAction      };

MDE_CMD mdeCommonCmds [] = {
  ///// MEMORY access
  {"memory-display",      "md",   "<address> of memory to display", 
   "   Display a block of memory\n\n"
   "\t<address>: The address of the memory to show\n", 
   mdeShowMemory },

  {"memory-read",         "mr",   "<address>", 
   "   Read one 32-bit data from the specified address\n"
   "\t<address>: The address to read\n",
   mdeMemRead },
  {"memory-write",        "mw",   "<address of memory to write> <data>", 
   "   Write one 32-bit data to the specified location\n"
   "\t<address>: The address to write to\n"
   "\t<data>: The data to write\n",
   mdeMemWrite },

  {"memory-fill",         "mf",   "<addr> <count> <data> <incrementer>", 
   "   Fill a block of memory with incrementing data patterns\n\n"
   "\t<addr>: The address of the memory block to fill\n"
   "\t<count>: Number of 32-bit INTS to fill\n"
   "\t<data>: The first data pattern to use\n"
   "\t<incrementer>: The value to be added to subsequent data patterns to use\n", 
   mdeMemFill    },

  {"memory-verify",       "mv",   "<addr> <count> <data> <incrementer>", 
   "   Verify a block of memory pre-filled with incrementing data patterns\n\n"
   "\t<addr>: The address of the memory block to verify\n"
   "\t<count>: Number of 32-bit INTS to verify\n"
   "\t<data>: The first data pattern to use\n"
   "\t<incrementer>: The value to be added to subsequent data patterns to use\n", 
   mdeMemVerify  },

  {"memory-compare-blocks",       "cmp",   "<from_addr> <to_addr><size_ints>", 
   "   Compare two blocks of memory\n\n"
   "\t<from_addr>: The address of the 1st memory block\n"
   "\t<to_addr>:   The address of the 2nd memory block\n"
   "\t<size_ints>: Number of 32-bit INTS to compare\n",
   mdeMemCmpBufs },

  {"memory-random-fill",         "rf",   "<addr> <count> <seed>", 
   "   Fill a block of memory with random data patterns\n\n"
   "\t<addr>: The address of the memory block to verify\n"
   "\t<count>:Number of 32-bit INTS to verify\n"
   "\t<seed>: The value to initialize the random generator\n",
    mdeRandomFill},
  
  {"memory-random-verify",       "rv",   "<addr> <count><seed>", 
   "   Verify a block of memory pre-filled with random data patterns\n\n"
   "\t<addr>: The address of the memory block to verify\n"
   "\t<count>:Number of 32-bit INTS to verify\n"
   "\t<seed>: The value to initialize the random generator\n",
   mdeRandomVerify  },

  ////// REGISTER access 

  {"register-read",         "rr",   "<phy-id><address of register to read>", 
   "   Read a DDR PHY register\n"
   "\t<phy-id>:  which PHY ? (0, 1, 2...)\n"
   "\t<address>: The address to read (0 based offset)\n",
   mdeRegRead },

  {"register-write",        "rw",   "<phy-id><address of register to write> <data>", 
   "   Write data to the specified DDR PHY register\n"
   "\t<phy-id>:  which PHY ? (0, 1, 2...)\n"
   "\t<address>: The address to read (0 based offset)\n"
   "\t<data>: The data to write\n",
   mdeRegWrite },


  {"mtest-run-test", "mtrt", "<mode><pass><run-time><seed>", 
   "   Start memory test with default arguments (enter 'mtsc' for details)\n\n"
   "\t<mode> 0 - write once, then each loop do read; 1 - each loop do write and read\n"
   "\t       2 - bitflip; 3 - write and read cached per loop; 4 - bitflip in address chunk\n"
   "\t       5 - write and read cached in address chunk per loop\n"
   "\t<pass> is the number of passes to run. Enter decimal count (or 0 to run contnuously)\n"
   "\t<run_time> The time to run test in seconds (dec)\n"
   "\nNOTE: To run non-stop: set pass=0 and run_time=0\n"
   "\t<seed> A value to seed the random number generator. Enter 0 to get a value provided by the system\n",
     mtRunTest },


  {"rdi-rd-eye-mips",    "mrdi",   "<mode><MEMC base><DIS start><DIS size><mt start><mt size><wl 32|16><vdl><vref><plot><clear><vref><sta_vref><octal_eye_memc_id>",
   "   Do a RD EYE SHMOO using MIPS for memory test and plot the result\n\n"
   "\t<mode>: 0 - single eye; 1 - dual eye; 2 - STA dual eye; 3 - dual eye without DIS; 4 - octal eye\n"
   "\t<MEMC base>: offset of MEMC (do not include RBUS base)\n"
   "\t<DIS start>: for DIS, top address of memory to test\n"
   "\t<DIS size>:  for DIS, size of memory in bytes (split for DIS0 and DIS1)\n"
   "\t<mt start>: for MIPS, top address of memory to test\n"
   "\t<mt size>:  for MIPS, size of memory in bytes (divided into source and dest halves)\n"
   "\t<data width>: 32 or 16\n"
   "\t<start_vdl>: starting vdl to use: 0 to 63 only\n"
   "\t<start_vref>: starting vref to use: 0 to 63 only\n"
   "\t<plot>: set to 1 to enable plot, 0 to disable \n"
   "\t<clear>: set to 1 to clear result array, 0 to retain previous result\n"
   "\t<vref>: set to 1 to change vref and vdl, 0 to retain current values\n"
   "\t<sta_vref>: in mode 2, specify the vref for STA collection\n"
   "\t<octal_eye_memc_id>: in mode 4, specify the memc to test\n"
   ,mde_rd_eye_plot        },

  {"mtest-show-config", "mtsc", "Display memory test configuration table", 
   "   Display the memory test configuration\n\n",
   mtShowConfig },
  
  {"mtest-edit-config", "mtec", "Edit memory test configuration <id>", 
   "   Edit the memory test configuration\n\n"
   "\t<id>\tWhich config to edit (0, 1, 2,...)\n",
   mtEditConfig },
  
  {"system-info", "info", "Display system related information\n", 
   "   Show MDE and local specific informations\n",
   mdeShowInfo },

  {"system-time", "time",   "Show seconds elapsed since start", 
   "   Show seconds and millsec since start\n"
   "\tNo argument needed\n",
   mdeGetTime },

  // HELP MUST BE THE LAST COMMAND
  {"help", "?", "Print this help text\n", 
   "   Print usage text and detail help text if available\n"
   "\tEnter help <command> to see full help text of the command\n",
   mdeHelp },
  
};
#define MDE_COMMON_CMD_COUNT (sizeof(mdeCommonCmds)/sizeof( MDE_CMD ))

MDE_CMD *getMdeCommonCommands ( unsigned *uCommandCount )
{
  *uCommandCount = MDE_COMMON_CMD_COUNT;
  return mdeCommonCmds;
}

const char mdeHelpMsg0 [] = "\n\nAll input numbers accepted as HEX (Do not include the prefix '0x')\n\n"
  "Enter 'help <command>' to see more details (if available)\n\n";

static void prettyStr (char *line, int count, sMde_t *mde)
{
  int size = strlen (line);
  int inx;

  host_printf (line);
  if (size > count) return;
  for (inx = 0; inx < (count - size); inx ++ ) host_console_write (SPACE);
}
#define NAME_SIZE 24
#define ALIAS_SIZE 12
char dashes [] = "--------------------------------------------------------------------------";

static unsigned show_cmd_set ( MDE_CMD *this, int count, sMde_t *mde )
{
#define CHAR_WANT 4
  int inx;
  char last [DIAG_SIZE_32];

  last [0] = 0;
  last [CHAR_WANT] = 0;
  host_printf ("Total %d commands available\n\n", count);
  host_printf ("%s\n", dashes);
  prettyStr ("Name",  NAME_SIZE, mde);
  prettyStr ("Alias", ALIAS_SIZE, mde);
  host_printf ("\tUsage\n");
  host_printf ("%s\n", dashes);
  strncpy (last, this[0].name, CHAR_WANT);

  // Print all commands including help
  for (inx = 0; inx < count; inx ++, this ++ ) {
    if (strstr (this->name, last) == NULL) {
      strncpy (last, this->name, CHAR_WANT);
	  host_printf ("\n");
    }
    prettyStr (this->name,  NAME_SIZE, mde);
    prettyStr (this->alias, ALIAS_SIZE, mde);
    host_printf ("%s\n", this->usage);
  }
  host_printf ("%s\n", dashes);

  return DIAG_RET_OK;
}
static unsigned show_all_cmds ( sMde_t *mde )
{
  host_printf ("\nMDE common commands information\n");
  show_cmd_set ( mde->common.cmds, mde->common.cmd_count, mde);
  if ( mde -> local.cmd_count != 0 ) {
	host_printf ("\nMDE local commands information\n");
	show_cmd_set ( mde->local.cmds, mde->local.cmd_count, mde);
  }
  host_printf (mdeHelpMsg0);
  return DIAG_RET_OK;
}


static unsigned show_cmd_detail (sMde_t *mde, MDE_CMD *this, int count, char name [])
{
  unsigned inx;//, found = 0;
  char *p_detail;

  for (inx = 0; inx < count; inx ++, this ++) {
    if ((strcmp (name, this->alias) == STR_EQU) || (strcmp (name, this->name)  == STR_EQU)) {
	  p_detail = this->details;

	  host_printf ("%s\n", dashes);
	  prettyStr ("Name",  NAME_SIZE, mde);
	  prettyStr ("Alias", ALIAS_SIZE, mde);
	  host_printf ("\tUsage\n");
	  host_printf ("%s\n", dashes);
	  host_printf ("%s\t\%s\t\t%s\n\n", this->name, this->alias, this->usage);

	  if (p_detail != NULL)  host_printf ("%s\n", p_detail);
	  return DIAG_RET_OK;
	}
  }
  return DIAG_RET_BAD;
}

static void show_help_detail (sMde_t *mde, char name [])
{
  if (show_cmd_detail ( mde, mde->common.cmds, mde->common.cmd_count, name ) == DIAG_RET_OK)
	return;
  if (show_cmd_detail ( mde, mde->local.cmds, mde->local.cmd_count, name ) == DIAG_RET_OK)
	return;
  host_printf ("\nNo info for command %s\n", name);
}

static unsigned mdeHelp ( sMde_t *mde )
{
  // help
  // help <name>
  char name    [DIAG_SIZE_32];
  char discard [DIAG_SIZE_32];
  char *line = &mde -> user_line [0];

  if (strchr (line, SPACE) == NULL) return show_all_cmds ( mde );

  mdeScanf (mde, "%s%s", discard, name);

  if (name == NULL) return show_all_cmds ( mde );

  show_help_detail (mde, name);
  return DIAG_RET_OK;
}


MDE_CMD *mdeParseCmds ( sMde_t *mde)
{
  char name [DIAG_SIZE_32];
  unsigned inx;
  const MDE_CMD *this;

  // check common first
  if (mde -> user_line [0] == 0) return (MDE_CMD*)&mdeNoAct;
  mdeScanf ( mde, "%s", name);

  for (inx = 0, this = mde -> common.cmds; inx < mde -> common.cmd_count; inx ++, this ++) {
    if ((strcmp (name, this->alias) == STR_EQU) || (strcmp (name, this->name)  == STR_EQU)) {
	  return (MDE_CMD*)this;
	}
  }
  // check local command lits
  for (inx = 0, this = mde -> local.cmds; inx < mde -> local.cmd_count; inx ++, this ++) {
    if ((strcmp (name, this->alias) == STR_EQU) || (strcmp (name, this->name)  == STR_EQU)) return (MDE_CMD*)this;
  }
  return (MDE_CMD*)&mdeNoSuch;
}

