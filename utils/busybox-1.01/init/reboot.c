/*
 * Mini reboot implementation for busybox
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
 *
 */

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/reboot.h>
#include "busybox.h"
#include "init_shared.h"

#ifdef __FAT_AP__
#include <sys/file.h>
#define MTD_DEVICE "/dev/mtd0"
#endif

#if defined(AP_PLATFORM)
/* See bug 6296 */
void
del_all_routes(void)
{
	system("ifconfig bond0 0.0.0.0") ;	
}
#endif

extern int reboot_main(int argc, char **argv)
{
	char *delay; /* delay in seconds before rebooting */
#ifdef __FAT_AP__
    int fd;
#endif

	if(bb_getopt_ulflags(argc, argv, "d:", &delay)) {
		sleep(atoi(delay));
	}

#if !defined(AP_PLATFORM)
	/* Sameer Dekate: reboot called intentionally - save this info in NVRAM */
	system("/mswitch/bin/set-reboot-status");
#endif

#if defined(AP_PLATFORM)
#define REBOOTME_CMD "/etc/init.d/rebootme"
	signal(SIGCHLD, SIG_IGN);
	if (optind < argc) {
		char cmdbuf[256];
		snprintf(cmdbuf, sizeof(cmdbuf), "%s '%s'", REBOOTME_CMD, argv[optind]);
		system(cmdbuf);
	} else {
		system(REBOOTME_CMD);
	}
	sleep(3);
	/* I feel this will come and bite us in the back but
	 * bug 6296 needs to be addressed somehow
	 */
	del_all_routes();
#ifdef __FAT_AP__
    // we should confirm that there isn't any writing flash before reboot.
    if ((fd  = open(MTD_DEVICE, O_RDONLY, 0)) != -1) {
        if (flock(fd, LOCK_SH) != -1) {
            sleep(1);
        }
    }
#endif
#endif	/* AP */

#ifndef CONFIG_INIT
#ifndef RB_AUTOBOOT
#define RB_AUTOBOOT		0x01234567
#endif
	return(bb_shutdown_system(RB_AUTOBOOT));
#else
	return kill_init(SIGTERM);
#endif
}

/*
Local Variables:
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
