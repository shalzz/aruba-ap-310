/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "libbb.h"

#define COMM_LEN 16 /* synchronize with size of comm in struct task_struct
					   in /usr/include/linux/sched.h */


#ifdef PROCPS_30

const char* FAST_FUNC bb_basename(const char *name)
{
	const char *cp = strrchr(name, '/');
	if (cp)
		return cp + 1;
	return name;
}
static int comm_match(procps_status_t *p, const char *procName)
{
	int argv1idx;

	/* comm does not match */
	if (strncmp(p->comm, procName, 15) != 0)
		return 0;

	/* in Linux, if comm is 15 chars, it may be a truncated */
	if (p->comm[14] == '\0') /* comm is not truncated - match */
		return 1;

	/* comm is truncated, but first 15 chars match.
	 * This can be crazily_long_script_name.sh!
	 * The telltale sign is basename(argv[1]) == procName. */

	if (!p->argv0)
		return 0;

	argv1idx = strlen(p->argv0) + 1;
	if (argv1idx >= p->argv_len)
		return 0;

	if (strcmp(bb_basename(p->argv0 + argv1idx), procName) != 0)
		return 0;

	return 1;
}
#endif

/* find_pid_by_name()
 *
 *  Modified by Vladimir Oleynik for use with libbb/procps.c
 *  This finds the pid of the specified process.
 *  Currently, it's implemented by rummaging through
 *  the proc filesystem.
 *
 *  Returns a list of all matching PIDs
 */
extern long* find_pid_by_name( const char* pidName)
{
	long* pidList;
	int i=0;
	procps_status_t * p;

	pidList = xmalloc(sizeof(long));
#ifdef CONFIG_SELINUX
	while ((p = procps_scan(0, 0, NULL)) != 0) {
#else
#ifdef PROCPS_30
	while ((p = procps_scan(0,  PSSCAN_PID|PSSCAN_COMM|PSSCAN_ARGVN)) != 0) {
#else
	while ((p = procps_scan(0)) != 0) {
#endif
#endif
#ifdef PROCPS_30
		if (comm_match(p, pidName)) {
#else
		if (strncmp(p->short_cmd, pidName, COMM_LEN-1) == 0) {
#endif
			pidList=xrealloc( pidList, sizeof(long) * (i+2));
			pidList[i++]=p->pid;
		}
	}

	pidList[i] = i==0 ? -1 : 0;
	return pidList;
}

/* END CODE */
/*
Local Variables:
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
