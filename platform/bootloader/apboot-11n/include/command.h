/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 *  Definitions for Command Processor
 */
#ifndef __COMMAND_H
#define __COMMAND_H

#ifndef NULL
#define NULL	0
#endif

#ifndef	__ASSEMBLY__
/*
 * Monitor Command Table
 */

struct cmd_tbl_s {
	char		*name;		/* Command Name			*/
	int		maxargs;	/* maximum number of arguments	*/
	int		repeatable;	/* autorepeat allowed?		*/
					/* Implementation function	*/
	int		(*cmd)(struct cmd_tbl_s *, int, int, char *[]);
	char		*usage;		/* Usage message	(short)	*/
#ifdef	CFG_LONGHELP
	char		*help;		/* Help  message	(long)	*/
#endif
#ifdef CONFIG_AUTO_COMPLETE
	/* do auto completion on the arguments */
	int		(*complete)(int argc, char *argv[], char last_char, int maxv, char *cmdv[]);
#endif
} __attribute__ ((aligned (8)));

typedef struct cmd_tbl_s	cmd_tbl_t;

extern cmd_tbl_t  __u_boot_cmd_start;
extern cmd_tbl_t  __u_boot_cmd_end;


/* common/command.c */
cmd_tbl_t *find_cmd(const char *cmd);

#ifdef CONFIG_AUTO_COMPLETE
extern void install_auto_complete(void);
extern int cmd_auto_complete(const char *const prompt, char *buf, int *np, int *colp);
#endif

/*
 * Monitor Command
 *
 * All commands use a common argument format:
 *
 * void function (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
 */

enum command_ret_t {
    CMD_RET_SUCCESS,    /* 0 = Success */
    CMD_RET_FAILURE,    /* 1 = Failure */
    CMD_RET_USAGE = -1, /* Failure, please report 'usage' error */
};

typedef	void 	command_t (cmd_tbl_t *, int, int, char *[]);

#endif	/* __ASSEMBLY__ */

/*
 * Command Flags:
 */
#define CMD_FLAG_REPEAT		0x0001	/* repeat last command		*/
#define CMD_FLAG_BOOTD		0x0002	/* command is from bootd	*/

/*
 * Configurable monitor commands definitions have been moved
 * to include/cmd_confdefs.h
 */


#define Struct_Section  __attribute__ ((unused,section (".u_boot_cmd")))

#ifdef  CFG_LONGHELP

#define U_BOOT_CMD(name,maxargs,rep,cmd,usage,help) \
cmd_tbl_t __u_boot_cmd_##name Struct_Section = {#name, maxargs, rep, cmd, usage, help}

#define U_BOOT_CMD_MKENT(name,maxargs,rep,cmd,usage,help) \
{#name, maxargs, rep, cmd, usage, help}

#else	/* no long help info */

#define U_BOOT_CMD(name,maxargs,rep,cmd,usage,help) \
cmd_tbl_t __u_boot_cmd_##name Struct_Section = {#name, maxargs, 0, cmd, usage}

#define U_BOOT_CMD_MKENT(name,maxargs,rep,cmd,usage,help) \
{#name, maxargs, rep, cmd, usage}

#endif	/* CFG_LONGHELP */

#if defined(CONFIG_NEBBIOLO)
/* 
 * Constants for our hijacking of "repeatable". Don't use these in .c's in u-boot/common.
 * By avoiding the use of 0 and 1 to filter Neb./Dolc. cmds, commands in common show up
 * under both platforms "for free".
 */
#define CMD_BOTH        0
#define CMD_NEBBIOLO    2
#define CMD_DOLCETTO    3
#endif

extern int cmd_usage(cmd_tbl_t *cmdtp);
cmd_tbl_t *find_cmd_tbl (const char *cmd, cmd_tbl_t *table, int table_len);

#if defined(CONFIG_NEEDS_MANUAL_RELOC)
void fixup_cmdtable(cmd_tbl_t *cmdtp, int size);
#endif

extern int cmd_get_data_size(char* arg, int default_size);

#endif	/* __COMMAND_H */
