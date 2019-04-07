/********************************************************************
 *
 *		L I N U X   E J E C T	C O M M A N D
 *
 *		  by Jeff Tranter (tranter@pobox.com)
 *
 ********************************************************************
 *
 * Copyright (C) 1994-2001 Jeff Tranter (tranter@pobox.com)
 * Copyright (C) 2004, 2005 Frank Lichtenheld (djpig@debian.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 ********************************************************************
 *
 * See the man page for a description of what this program does and what
 * the requirements to run it are.
 *
 */

#include "i18n.h"
#define VERSION "2.1.5"
#define DEFAULTDEVICE "/dev/sr0"
#ifndef DEFAULTDEVICE
#error DEFAULTDEVICE not set, check Makefile
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>

#ifdef GETOPTLONG
#include <getopt.h>
#endif /* GETOPTLONG */
#include <errno.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/mtio.h>
#include <sys/mount.h>

#if defined(__linux__)
#include <linux/version.h>
/* handy macro found in 2.1 kernels, but not in older ones */
#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif 
#include <linux/types.h>
#include <linux/cdrom.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
#include <linux/ucdrom.h>
#endif
#include <linux/fd.h>
#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <scsi/scsi_ioctl.h>
#include <sys/time.h>

/* Used by the ToggleTray() function. If ejecting the tray takes this
 * time or less, the tray was probably already ejected, so we close it
 * again.
 */
#define TRAY_WAS_ALREADY_OPEN_USECS  200000	/* about 0.2 seconds */


#define CLOSE(fd) if (close(fd)==-1) { \
    perror(programName); \
    exit(1); \
}

#define FCLOSE(fd) if (fclose(fd)==-1) { \
    perror(programName); \
    exit(1); \
}

#define HAVE_EJECT_SCSI

#elif defined(__FreeBSD_kernel__)
#include <sys/cdio.h>
#endif /* if defined(__linux__) */


#define CLOSE(fd) if (close(fd)==-1) { \
    perror(programName); \
    exit(1); \
}

#define FCLOSE(fd) if (fclose(fd)==-1) { \
    perror(programName); \
    exit(1); \
}

/* Global Variables */
static const char *version = VERSION; /* program version */
int a_option = 0; /* command flags and arguments */
int c_option = 0;
int d_option = 0;
int f_option = 0;
int h_option = 0;
int n_option = 0;
int q_option = 0;
int r_option = 0;
int s_option = 0;
int t_option = 0;
int T_option = 0;
int X_option = 0;
int v_option = 0;
int x_option = 0;
int p_option = 0;
int m_option = 0;
int a_arg = 0;
int c_arg = 0;
int x_arg = 0;
static char *programName; /* used in error messages */

/*
 * These are the basenames of devices which can have multiple
 * partitions per device.
 */
static const char *partitionDevice[] = {
	"hd",
	"sd",
	"xd",
	"dos_hd",
	"mfm",
	"ad",
	"ed",
	"ftl",
	"pd",
	0};


/* Display command usage on standard error and exit. */
static void usage()
{
//    perror(_("%s: device is `%s'\n"));
	fprintf(stderr,_(
"Eject version %s by Jeff Tranter (tranter@pobox.com)\n"
"Usage:\n"
"  eject -h				-- display command usage and exit\n"
"  eject -V				-- display program version and exit\n"
"  eject [-vnrsfqpm] [<name>]		-- eject device\n"
"  eject [-vn] -d			-- display default device\n"
"  eject [-vn] -a on|off|1|0 [<name>]	-- turn auto-eject feature on or off\n"
"  eject [-vn] -c <slot> [<name>]	-- switch discs on a CD-ROM changer\n"
"  eject [-vn] -t [<name>]		-- close tray\n"
"  eject [-vn] -T [<name>]		-- toggle tray\n"
"  eject [-vn] -x <speed> [<name>]	-- set CD-ROM max speed\n"
"  eject [-vn] -X [<name>]		-- list CD-ROM available speeds\n"
"Options:\n"
"  -v\t-- enable verbose output\n"
"  -n\t-- don't eject, just show device found\n"
"  -r\t-- eject CD-ROM\n"
#ifdef HAVE_EJECT_SCSI
"  -s\t-- eject SCSI device\n"
#endif
#ifdef HAVE_EJECT_FLOPPY
"  -f\t-- eject floppy\n"
#endif
#ifdef HAVE_EJECT_TAPE
"  -q\t-- eject tape\n"
#endif
"  -p\t-- use /proc/mounts instead of /etc/mtab\n"
"  -m\t-- do not unmount device even if it is mounted\n"
)
, version);
#ifdef GETOPTLONG
	fprintf(stderr,_(
"Long options:\n"
"  -h --help   -v --verbose      -d --default\n"
"  -a --auto   -c --changerslot  -t --trayclose  -x --cdspeed\n"
"  -r --cdrom"
#ifdef HAVE_EJECT_SCSI
"  -s --scsi"
#endif
#ifdef HAVE_EJECT_FLOPPY
"         -f --floppy"
#endif
"     -X --listspeed"
#ifdef HAVE_EJECT_TAPE
"     -q --tape"
#endif
"\n"
"  -n --noop   -V --version\n"
"  -p --proc   -m --no-unmount   -T --traytoggle\n"));
#endif /* GETOPTLONG */
	fprintf(stderr,_(
"Parameter <name> can be a device file or a mount point.\n"
"If omitted, name defaults to `%s'.\n"
"By default tries -r, -s, -f, and -q in order until success.\n"),
			DEFAULTDEVICE);
  exit(1);
}


/* Handle command line options. */
static void parse_args(int argc, char **argv, char **device)
{
	const char *flags = "a:c:x:dfhnqrstTXvVpm";
#ifdef GETOPTLONG
	static struct option long_options[] =
	{
		{"help",	no_argument,	   NULL, 'h'},
		{"verbose",	no_argument,	   NULL, 'v'},
		{"default",	no_argument,	   NULL, 'd'},
		{"auto",	required_argument, NULL, 'a'},
		{"changerslot", required_argument, NULL, 'c'},
		{"trayclose",	no_argument,	   NULL, 't'},
		{"traytoggle",	no_argument,	   NULL, 'T'},
		{"cdspeed",	required_argument, NULL, 'x'},
		{"listspeed",	no_argument,	   NULL, 'X'},
		{"noop",	no_argument,	   NULL, 'n'},
		{"cdrom",	no_argument,	   NULL, 'r'},
		{"scsi",	no_argument,	   NULL, 's'},
		{"floppy",	no_argument,	   NULL, 'f'},
		{"tape",	no_argument,	   NULL, 'q'},
		{"version",	no_argument,	   NULL, 'V'},
		{"proc",	no_argument,	   NULL, 'p'},
		{"no-unmount",	no_argument,	   NULL, 'm'},
		{0, 0, 0, 0}
	};
	int option_index;
#endif /* GETOPTLONG */
	int c;

#ifdef GETOPTLONG
	while ((c = getopt_long(argc, argv, flags, long_options, &option_index)) != EOF) {
#else
	while ((c = getopt(argc, argv, flags)) != EOF) {
#endif /* GETOPTLONG */
		switch (c) {
		  case 'a':
			  a_option = 1;
			  if (!strcmp(optarg, "0"))
				  a_arg = 0;
			  else if (!strcmp(optarg, "off"))
				  a_arg = 0;
			  else if (!strcmp(optarg, "1"))
				  a_arg = 1;
			  else if (!strcmp(optarg, "on"))
				  a_arg = 1;
			  else {
				  fprintf(stderr, _("%s: invalid argument to --auto/-a option\n"), programName);
				  exit(1);
			  }
			  break;
		  case 'c':
			  c_option = 1;
			  /* atoi() returns 0 on error, so "0" must be parsed separately */
			  if (!strcmp(optarg, "0"))
				  c_arg = 0;
			  else {
				  c_arg = atoi(optarg);
				  if (c_arg <= 0) {
					  fprintf(stderr, _("%s: invalid argument to --changerslot/-c option\n"), programName);
					  exit(1);
				  }
			  }
			  break;
		  case 'x':
			  x_option = 1;
			  if (!strcmp(optarg, "0"))
				  x_arg = 0;
			  else {
				  x_arg = atoi(optarg);
				  if (x_arg <= 0) {
					  fprintf(stderr, _("%s: invalid argument to --cdspeed/-x option\n"), programName);
					  exit(1);
				  }
			  }
			  break;
		  case 'd':
			  d_option = 1;
			  break;
		  case 'f':
			  f_option = 1;
			  break;
		  case 'h':
			  usage();
			  exit(0);
			  break;
		  case 'm':
			  m_option = 1;
			  break;
		  case 'n':
			  n_option = 1;
			  break;
		  case 'p':
			  p_option = 1;
			  break;
		  case 'q':
			  q_option = 1;
			  break;
		  case 'r':
			  r_option = 1;
			  break;
		  case 's':
			  s_option = 1;
			  break;
		  case 't':
			  t_option = 1;
			  break;
		  case 'X':
			  X_option = 1;
			  break;
		  case 'T':
			  T_option = 1;
			  break;
		  case 'v':
			  v_option = 1;
			  break;
		  case 'V':
			  printf(_("eject version %s by Jeff Tranter (tranter@pobox.com)\n"), version);
			  exit(0);
			  break;
		  case '?':
			  exit(1);
			  break;
		}
	}
	/* check for a single additional argument */
	if ((argc - optind) > 1) {
		fprintf(stderr, _("%s: too many arguments\n"), programName);
		exit(1);
	}
	if ((argc - optind) == 1) { /* one argument */
		*device = strdup(argv[optind]);
	}
}


/* Return 1 if file/device exists, 0 otherwise. */
static int FileExists(const char *name, const int try, int *found)
{

        if (!found) return -1;
        /*
	 * access() uses the UID, not the EUID. This way a normal user
	 * cannot find out if a file (say, /root/fubar) exists or not, even
	 * if eject is SUID root
	 */
	if (access (name, F_OK) == 0) {
	  (*found)++;
	  if (try <= (*found))
	    return 1;
	  else
	    return 0;
	} else {
	  return 0;
	}
}


/*
 * Given name, such as foo, see if any of the following exist:
 *
 * foo (if foo starts with '.' or '/')
 * /dev/foo
 * /media/foo
 * /mnt/foo
 * /dev/cdroms/foo
 * /dev/cdroms/foo0
 * /dev/dsk/foo
 * /dev/rdsk/foo
 * ./foo
 *
 * If found, return the full path. If not found, return 0.
 * Returns pointer to dynamically allocated string.
 */
static char *FindDevice(const char *name)
{
	char *buf;
	static int try = 0;
	int found = 0;

	buf = (char *) malloc(strlen(name)+14); /* to allow for "/dev/cdroms/ + "0" + null */
	if (buf==NULL) {
		fprintf(stderr, _("%s: could not allocate memory\n"), programName);
		exit(1);
	}

	if (try == INT_MAX) {
	  fprintf(stderr, _("%s: FindDevice called too often\n"), programName );;
	  exit(1);
	} else
	  try++;

	if ((name[0] == '.') || (name[0] == '/')) {
		strcpy(buf, name);
		if (FileExists(buf, try, &found))
			return buf;
	}

	strcpy(buf, "/dev/");
	strcat(buf, name);
	if (FileExists(buf, try, &found))
		return buf;

	strcpy(buf, "/media/");
	strcat(buf, name);
	if (FileExists(buf, try, &found))
		return buf;
	
	strcpy(buf, "/mnt/");
	strcat(buf, name);
	if (FileExists(buf, try, &found))
		return buf;

	/* for devfs under Linux */
	strcpy(buf, "/dev/cdroms/");
	strcat(buf, name);
	if (FileExists(buf, try, &found))
		return buf;

	strcpy(buf, "/dev/cdroms/");
	strcat(buf, name);
	strcat(buf, "0");
	if (FileExists(buf, try, &found))
		return buf;

	/* for devfs under Solaris */
	strcpy(buf, "/dev/rdsk/");
	strcat(buf, name);
	if (FileExists(buf, try, &found))
		return buf;

	strcpy(buf, "/dev/dsk/");
	strcat(buf, name);
	if (FileExists(buf, try, &found))
		return buf;

	strcpy(buf, "./");
	strcat(buf, name);
	if (FileExists(buf, try, &found))
		return buf;

	free(buf);
	buf = 0;
	return 0;
}


/* Set or clear auto-eject mode. */
static void AutoEject(int fd, int onOff)
{
	int status = -1;

#if defined(CDROM_SET_OPTIONS) && defined(CDROM_CLEAR_OPTIONS) 
	if (onOff)
		status = ioctl(fd, CDROM_SET_OPTIONS, CDO_AUTO_EJECT);
	else
		status = ioctl(fd, CDROM_CLEAR_OPTIONS, CDO_AUTO_EJECT);
#else
	errno = ENOSYS;
#endif
	if (status < 0) {
		fprintf(stderr, _("%s: CD-ROM auto-eject command failed: %s\n"), programName, strerror(errno));
		exit(1);
	}
}


/*
 * Changer select. CDROM_SELECT_DISC is preferred, older kernels used
 * CDROMLOADFROMSLOT.
 */
static void ChangerSelect(int fd, int slot)
{
	int status;

#ifdef CDROM_SELECT_DISC
	status = ioctl(fd, CDROM_SELECT_DISC, slot);
	if (status < 0) {
		fprintf(stderr, _("%s: CD-ROM select disc command failed: %s\n"), programName, strerror(errno));
		exit(1);
	}
#elif defined CDROMLOADFROMSLOT
	status = ioctl(fd, CDROMLOADFROMSLOT, slot);
	if (status != 0) {
		fprintf(stderr, _("%s: CD-ROM load from slot command failed: %s\n"), programName, strerror(errno));
		exit(1);
	}
#else
    fprintf(stderr, _("%s: IDE/ATAPI CD-ROM changer not supported by this kernel\n"), programName);
#endif
}


/*
 * Close tray. Not supported by older kernels.
 */
static void CloseTray(int fd)
{
	int status;

#if defined(CDROMCLOSETRAY) || defined(CDIOCCLOSE)
#if defined(CDROMCLOSETRAY)
	status = ioctl(fd, CDROMCLOSETRAY);
#elif defined(CDIOCCLOSE)
	status = ioctl(fd, CDIOCCLOSE);
#endif
	if (status != 0) {
		fprintf(stderr, _("%s: CD-ROM tray close command failed: %s\n"), programName, strerror(errno));
		exit(1);
	}
#else
    fprintf(stderr, _("%s: CD-ROM tray close command not supported by this kernel\n"), programName);
#endif
}

/*
 * Toggle tray.
 *
 * Written by Benjamin Schwenk <benjaminschwenk@yahoo.de> and
 * Sybren Stuvel <sybren@thirdtower.com>
 *
 * Not supported by older kernels because it might use
 * CloseTray().
 *
 */
static void ToggleTray(int fd)
{
	struct timeval time_start, time_stop;
	int time_elapsed;

#ifdef CDROMCLOSETRAY

	/* Try to open the CDROM tray and measure the time therefor
	 * needed.  In my experience the function needs less than 0.05
	 * seconds if the tray was already open, and at least 1.5 seconds
	 * if it was closed.  */
	gettimeofday(&time_start, NULL);
	
	/* Send the CDROMEJECT command to the device. */
	if (ioctl(fd, CDROMEJECT, 0) < 0) {
		perror("ioctl");
		exit(1);
	}

	/* Get the second timestamp, to measure the time needed to open
	 * the tray.  */
	gettimeofday(&time_stop, NULL);

	time_elapsed = (time_stop.tv_sec * 1000000 + time_stop.tv_usec) -
		(time_start.tv_sec * 1000000 + time_start.tv_usec);

	/* If the tray "opened" too fast, we can be nearly sure, that it
	 * was already open. In this case, close it now. Else the tray was
	 * closed before. This would mean that we are done.  */
	if (time_elapsed < TRAY_WAS_ALREADY_OPEN_USECS)
		CloseTray(fd);

#else
    fprintf(stderr, _("%s: CD-ROM tray toggle command not supported by this kernel\n"), programName);
#endif
	
}

/*
 * Select Speed of CD-ROM drive.
 * Thanks to Roland Krivanek (krivanek@fmph.uniba.sk)
 * http://dmpc.dbp.fmph.uniba.sk/~krivanek/cdrom_speed/
 */
static void SelectSpeedCdrom(int fd, int speed)
{
	int status;

#ifdef CDROM_SELECT_SPEED
	status = ioctl(fd, CDROM_SELECT_SPEED, speed);
	if (status != 0) {
		fprintf(stderr, _("%s: CD-ROM select speed command failed: %s\n"), programName, strerror(errno));
		exit(1);
	}
#else
    fprintf(stderr, _("%s: CD-ROM select speed command not supported by this kernel\n"), programName);
#endif
}

/*
 * Read Speed of CD-ROM drive. From Linux 2.6.13, the current speed is correctly reported
 */
static int ReadSpeedCdrom(const char *shortName)
{
	char line[512];
	char *str_speed, *str_name;
	int drive_number = -1, i;
	FILE *f = fopen("/proc/sys/dev/cdrom/info", "r");
	
	if (f == NULL) {
		fprintf(stderr, _("%s: unable to read the speed from /proc/sys/dev/cdrom/info\n"), programName);
		exit(1);
	}
	
	while (!feof(f)) {
		fgets(line, sizeof(line), f);

		/* find drive number from shortName in line "drive name" */
		if (drive_number == -1) {
			if (strncmp(line, "drive name:", 11) == 0) {
				str_name = strtok(&line[11], "\t ");
				drive_number = 0;
				while (strncmp(shortName, str_name, strlen(shortName)) != 0) {
					drive_number++;
					str_name = strtok(NULL, "\t ");
					if (str_name == NULL) {
						fprintf(stderr, _("%s: error while finding CD-ROM name\n"), programName);
						exit(1);
					}
				}
			}
		/* find line "drive speed" and read the correct speed */
		} else {
			if (strncmp(line, "drive speed:", 12) == 0) {
				str_speed = strtok(&line[12], "\t ");
				for (i = 1; i < drive_number; i++)
					str_speed = strtok(NULL, "\t ");

				if (str_speed == NULL) {
					fprintf(stderr, _("%s: error while reading speed\n"), programName);
					exit(1);
				}
				return atoi(str_speed);
			}
		}
	}

	fprintf(stderr, _("%s: error while reading speed\n"), programName);
	exit(1);
	return -1;
}


/*
 * List Speed of CD-ROM drive.
 */
static void ListSpeedCdrom(const char *fullName, int fd)
{
#ifdef CDROM_SELECT_SPEED
	int max_speed, curr_speed = 0, prev_speed;
	char *shortName = rindex(fullName, '/') + 1;
	
	SelectSpeedCdrom(fd, 0);
	max_speed = ReadSpeedCdrom(shortName);
	while (curr_speed < max_speed) {
		prev_speed = curr_speed;
		SelectSpeedCdrom(fd, prev_speed + 1);
		curr_speed = ReadSpeedCdrom(shortName);
		if (curr_speed > prev_speed)
			printf("%d ", curr_speed);
		else
			curr_speed = prev_speed + 1;
	}

	printf("\n");
#else
	fprintf(stderr, _("%s: CD-ROM select speed command not supported by this kernel\n"), programName);
#endif
}

/*
 * Eject using CDROMEJECT ioctl. Return 1 if successful, 0 otherwise.
 */
static int EjectCdrom(int fd)
{
	int status = -1;

#if defined(CDROMEJECT)
	status = ioctl(fd, CDROMEJECT);
#elif defined(CDIOCEJECT)
	status = ioctl(fd, CDIOCEJECT);
#else
/* Some kernels implement cdrom-eject only, but I don't think any kernel in the
   world would implement eject only for non-cdrom drives.  Let's die. */
# error
#endif
	return (status == 0);
}

#ifdef HAVE_EJECT_SCSI
/*
 * Eject using SCSI SG_IO commands. Return 1 if successful, 0 otherwise.
 */
static int EjectScsi(int fd)
{
	int status, k;
	sg_io_hdr_t io_hdr;
	unsigned char allowRmBlk[6] = {ALLOW_MEDIUM_REMOVAL, 0, 0, 0, 0, 0};
	unsigned char startStop1Blk[6] = {START_STOP, 0, 0, 0, 1, 0};
	unsigned char startStop2Blk[6] = {START_STOP, 0, 0, 0, 2, 0};
	unsigned char inqBuff[2];
	unsigned char sense_buffer[32];

	if ((ioctl(fd, SG_GET_VERSION_NUM, &k) < 0) || (k < 30000)) {
	  printf("not an sg device, or old sg driver\n");
	  return 0;
	}

	memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = 6;
	io_hdr.mx_sb_len = sizeof(sense_buffer);
	io_hdr.dxfer_direction = SG_DXFER_NONE;
	io_hdr.dxfer_len = 0;
	io_hdr.dxferp = inqBuff;
	io_hdr.sbp = sense_buffer;
	io_hdr.timeout = 2000;

	io_hdr.cmdp = allowRmBlk;
	status = ioctl(fd, SG_IO, (void *)&io_hdr);
	if (status < 0)
		return 0;

	io_hdr.cmdp = startStop1Blk;
	status = ioctl(fd, SG_IO, (void *)&io_hdr);
	if (status < 0)
		return 0;

	io_hdr.cmdp = startStop2Blk;
	status = ioctl(fd, SG_IO, (void *)&io_hdr);
	if (status < 0)
		return 0;

	/* force kernel to reread partition table when new disc inserted */
	status = ioctl(fd, BLKRRPART);
	return 1;
}
#endif


#ifdef HAVE_EJECT_FLOPPY
/*
 * Eject using FDEJECT ioctl. Return 1 if successful, 0 otherwise.
 */
static int EjectFloppy(int fd)
{
	int status;

	status = ioctl(fd, FDEJECT);
	return (status >= 0);
}
#endif


#ifdef HAVE_EJECT_TAPE
/*
 * Eject using tape ioctl. Return 1 if successful, 0 otherwise.
 */
static int EjectTape(int fd)
{
	int status;
	struct mtop op;

	op.mt_op = MTOFFL; /* rewind and eject */
	op.mt_count = 0;   /* not used */
	status = ioctl(fd, MTIOCTOP, &op);
	return (status >= 0);
}
#endif


/* Unmount a device. */
static void Unmount(const char *fullName)
{
	int status;

	switch (fork()) {
	  case 0: /* child */
		  setuid(getuid()); /* reduce likelyhood of security holes when running setuid */
		  if(p_option) {
			  execlp("pumount", "pumount", fullName, "-n", NULL);
			  execlp("umount", "umount", fullName, "-n", NULL);
		  } else {
			  execlp("pumount", "pumount", fullName, NULL);
			  execlp("umount", "umount", fullName, NULL);
		  }
		  fprintf(stderr, _("%s: unable to exec umount of `%s': %s\n"),
				  programName, fullName, strerror(errno));
		  exit(1);
		  break;
	  case -1:
		  fprintf(stderr, _("%s: unable to fork: %s\n"), programName, strerror(errno));
		  break;
	  default: /* parent */
		  wait(&status);
		  if (WIFEXITED(status) == 0) {
			  fprintf(stderr, _("%s: unmount of `%s' did not exit normally\n"), programName, fullName);
			  exit(1);
		  }
		  if (WEXITSTATUS(status) != 0) {
			  fprintf(stderr, _("%s: unmount of `%s' failed\n"), programName, fullName);
			  exit(1);
		  }
		  break;
	}
}


/* Open a device file. */
static int OpenDevice(const char *fullName)
{
	int fd = open(fullName, O_RDONLY|O_NONBLOCK);
	if (fd == -1) {
		fprintf(stderr, _("%s: unable to open `%s'\n"), programName, fullName);
		exit(1);
	}
	return fd;
}


/*
 * Get major and minor device numbers for a device file name, so we
 * can check for duplicate devices.
 */
static int GetMajorMinor(const char *name, int *maj, int *min)
{
	struct stat sstat;
	if (maj) *maj = -1;
	if (min) *min = -1;
	if (stat(name, &sstat) == -1)
		return -1;
	if (! S_ISBLK(sstat.st_mode) && ! S_ISCHR(sstat.st_mode))
		return -1;
	if (maj) *maj = major(sstat.st_rdev);
	if (min) *min = minor(sstat.st_rdev);
	return 0;
}


/*
 * See if device has been mounted by looking in mount table.  If so, set
 * device name and mount point name, and return 1, otherwise return 0.
 */
static int MountedDevice(const char *name, char **mountName, char **deviceName)
{
	FILE *fp;
	char line[1024];
	char s1[1024];
	char s2[1024];
	int rc;

	int maj;
	int min;

	GetMajorMinor(name, &maj, &min);

	fp = fopen((p_option ? "/proc/mounts" : "/etc/mtab"), "r");
	if (fp == NULL)
	{
		fprintf(stderr, _("unable to open %s: %s\n"), (p_option ? "/proc/mounts" : "/etc/mtab"), strerror(errno));
		exit(1);
	}

	while (fgets(line, sizeof(line), fp) != 0) {
		rc = sscanf(line, "%1023s %1023s", s1, s2);
		if (rc >= 2) {
			int mtabmaj, mtabmin;
			GetMajorMinor(s1, &mtabmaj, &mtabmin);
			if (((strcmp(s1, name) == 0) || (strcmp(s2, name) == 0)) ||
				((maj != -1) && (maj == mtabmaj) && (min == mtabmin))) {
				FCLOSE(fp);
				*deviceName = strdup(s1);
				*mountName = strdup(s2);
				return 1;
			}
		}
	}
	*deviceName = 0;
	*mountName = 0;
	FCLOSE(fp);
	return 0;
}


/*
 * See if device can be mounted by looking in /etc/fstab.
 * If so, set device name and mount point name, and return 1,
 * otherwise return 0.
 */
static int MountableDevice(const char *name, char **mountName, char **deviceName)
{
	FILE *fp;
	char line[1024];
	char s1[1024];
	char s2[1024];
	int rc;

	fp = fopen("/etc/fstab", "r");
	if (fp == NULL) {
/*
 * /etc/fstab may be unreadable in some situations due to passwords in the
 * file.
 */
/*		fprintf(stderr, _("%s: unable to open /etc/fstab: %s\n"), programName, strerror(errno));
		exit(1);*/
		if (v_option) {
			printf( _("%s: unable to open /etc/fstab: %s\n"), programName, strerror(errno));
		}
		return -1;
	}

	while (fgets(line, sizeof(line), fp) != 0) {
		rc = sscanf(line, "%1023s %1023s", s1, s2);
		if (rc >= 2 && s1[0] != '#' && strcmp(s2, name) == 0) {
			FCLOSE(fp);
			*deviceName = strdup(s1);
			*mountName = strdup(s2);
			return 1;
		}
	}
	FCLOSE(fp);
	return 0;
}


/*
 * Step through mount table and unmount all devices that match a regular
 * expression.
 */
static void UnmountDevices(const char *pattern)
{
	regex_t preg;
	FILE *fp;
	char s1[1024];
	char s2[1024];
	char line[1024];
	int status;

	if (regcomp(&preg, pattern, REG_EXTENDED)!=0) {
		perror(programName);
		exit(1);
	}

	fp = fopen((p_option ? "/proc/mounts" : "/etc/mtab"), "r");
	if (fp == NULL)
	{
		fprintf(stderr, _("unable to open %s: %s\n"),(p_option ? "/proc/mounts" : "/etc/mtab"), strerror(errno));
		exit(1);
	}

	while (fgets(line, sizeof(line), fp) != 0) {
		status = sscanf(line, "%1023s %1023s", s1, s2);
		if (status >= 2) {
			status = regexec(&preg, s1, 0, 0, 0);
			if (status == 0) {
				if (v_option)
					printf(_("%s: unmounting `%s'\n"), programName, s2);
				Unmount(s2);
			}
		}
	}
	regfree(&preg);
	FCLOSE(fp);
}


/* Check if name is a symbolic link. If so, return what it points to. */
static char *SymLink(const char *name)
{
	int status;
	char s1[PATH_MAX];
	char s2[PATH_MAX];
	char s4[PATH_MAX];
	char result[PATH_MAX];
	char *s3;

	memset(s1, 0, sizeof(s1));
	memset(s2, 0, sizeof(s2));
	memset(s4, 0, sizeof(s4));
	memset(result, 0, sizeof(result));

	status = readlink(name, s1, sizeof(s1) - 1);

	if (status == -1)
		return 0;

	s1[status] = 0;
	if (s1[0] == '/') { /* absolute link */
		return strdup(s1);
	} else { /* relative link, add base name */
		strncpy(s2, name, sizeof(s2)-1);
		s3 = strrchr(s2, '/');
		if (s3 != 0) {
			s3[1] = 0;
			snprintf(result, sizeof(result)-1, "%s%s", s2, s1);
		}
	}
	realpath(result, s4);
	return strdup(s4);
}


/*
 * Given a name, see if it matches a pattern for a device that can have
 * multiple partitions.  If so, return a regular expression that matches
 * partitions for that device, otherwise return 0.
 */
static char *MultiplePartitions(const char *name)
{
	int i = 0;
	int status;
	regex_t preg;
	char pattern[256];
	char *result = 0;

	for (i = 0; partitionDevice[i] != 0; i++) {
		/* look for ^/dev/foo[a-z]([0-9]?[0-9])?$, e.g. /dev/hda1 */
		strcpy(pattern, "^/dev/");
		strcat(pattern, partitionDevice[i]);
		strcat(pattern, "[a-z]([0-9]?[0-9])?$");
		if (regcomp(&preg, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
			perror(programName);
			exit(1);
		}
		status = regexec(&preg, name, 1, 0, 0);
		regfree(&preg);
		if (status == 0) {
			result = (char *) malloc(strlen(name) + 25);
			if (result == NULL) {
				fprintf(stderr, _("%s: could not allocate memory\n"), programName);
				exit(1);
			}
			strcpy(result, name);
			result[strlen(partitionDevice[i]) + 6] = 0;
			strcat(result, "([0-9]?[0-9])?$");
			if (v_option)
				printf(_("%s: `%s' is a multipartition device\n"), programName, name);
			return result;
		}
	}
	if (v_option)
		printf(_("%s: `%s' is not a multipartition device\n"), programName, name);
	return 0;
}


/* handle -x option */
static void HandleXOption(char *deviceName)
{
	int fd; 	   /* file descriptor for device */
	if (x_option) {
		if (v_option)
		{
			if (x_arg == 0)
				printf(_("%s: setting CD-ROM speed to auto\n"), programName);
			else
				printf(_("%s: setting CD-ROM speed to %dX\n"), programName, x_arg);
		}
		fd = OpenDevice(deviceName);
		SelectSpeedCdrom(fd, x_arg);
		exit(0);
	}
}


/* main program */
int main(int argc, char **argv)
{
	const char *defaultDevice = DEFAULTDEVICE;  /* default if no name passed by user */
	int worked = 0;    /* set to 1 when successfully ejected */
	char *device = 0;  /* name passed from user */
	char *fullName;    /* expanded name */
	char *fullNameOrig;/* expanded name (links not resolved) */
	char *deviceName;  /* name of device */
	char *linkName;    /* name of device's symbolic link */
	char *mountName;   /* name of device's mount point */
	int fd; 	   /* file descriptor for device */
	int mounted = 0;   /* true if device is mounted */
	int mountable = 0; /* true if device is in /etc/fstab */
	int result = 0;    /* store the result of a operation */
	char *pattern;	   /* regex for device if multiple partitions */
	int ld = 6;	   /* symbolic link max depth */

	I18NCODE

	/* program name is global variable used by other procedures */
	programName = strdup(argv[0]);

	/* parse the command line arguments */
	parse_args(argc, argv, &device);


	/* handle -d option */
	if (d_option) {
		printf(_("%s: default device: `%s'\n"), programName, defaultDevice);
		exit(0);
	}

	/* if no device, use default */
	if (device == 0) {
		device = strdup(defaultDevice);
		if (v_option)
			printf(_("%s: using default device `%s'\n"), programName, device);
	}

	/* Strip any trailing slash from name in case user used bash/tcsh
	   style filename completion (e.g. /mnt/cdrom/) */
	if (device[strlen(device)-1] == '/')
		device[strlen(device)-1] = 0;

	if (v_option)
		printf(_("%s: device name is `%s'\n"), programName, device);

	do {
	  /* figure out full device or mount point name */
	  fullName = FindDevice(device);
	  if (fullName == 0) {
	    fprintf(stderr, _("%s: unable to find or open device for: `%s'\n"), 
		    programName, device);
	    exit(1);
	  }
	  if (v_option)
	    printf(_("%s: expanded name is `%s'\n"), programName, fullName);

	  /* check for a symbolic link */
	  /* /proc/mounts doesn't resolve symbolic links */
	  fullNameOrig = strdup(fullName);
	  linkName = strdup(fullName); /* ensure linkName is initialized */
	  if (!p_option) {
	    while ((linkName = SymLink(fullName)) && (ld > 0)) {
	      if (v_option)
		printf(_("%s: `%s' is a link to `%s'\n"), programName,
		       fullName, linkName);
	      free(fullName);
	      fullName = strdup(linkName);
	      free(linkName);
	      linkName = 0;
	      ld--;
	    }
	  }
	  /* handle max depth exceeded option */
	  if (ld <= 0) {
	    printf(_("%s: maximum symbolic link depth exceeded: `%s'\n"), programName, fullName);
	    exit(1);
	  }

	  /* if mount point, get device name */
	  mounted = MountedDevice(fullName, &mountName, &deviceName);
	  if (v_option) {
	    if (mounted)
	      printf(_("%s: `%s' is mounted at `%s'\n"), programName,
		     deviceName, mountName);
	    else
	      printf(_("%s: `%s' is not mounted\n"), programName, fullName);
	  }
	  if (!mounted) {
	    deviceName = strdup(fullName);
	  }

	  /* if not currently mounted, see if it is a possible mount point */
	  if (!mounted) {
	    mountable = MountableDevice(fullName, &mountName, &deviceName);
	    /* if return value -1 then fstab could not be read */
	    if (v_option && mountable >= 0) {
	      if (mountable)
		printf(_("%s: `%s' can be mounted at `%s'\n"), programName, deviceName, mountName);
	      else
		printf(_("%s: `%s' is not a mount point\n"), programName, fullName);
	    }
	  }

	  result = GetMajorMinor(deviceName, NULL, NULL);
	  if (result == -1) {
	      fprintf(stderr,
		      _("%s: tried to use `%s' as device name but it is no block device\n"),
		      programName, deviceName);
	  }

	} while (result == -1);

	/* handle -n option */
	if (n_option) {
		printf(_("%s: device is `%s'\n"), programName, deviceName);
		if (v_option)
			printf(_("%s: exiting due to -n/--noop option\n"), programName);
		exit(0);
	}

	/* handle -a option */
	if (a_option) {
		if (v_option) {
			if (a_arg)
				printf(_("%s: enabling auto-eject mode for `%s'\n"), programName, deviceName);
			else
				printf(_("%s: disabling auto-eject mode for `%s'\n"), programName, deviceName);
		}
		fd = OpenDevice(deviceName);
		AutoEject(fd, a_arg);
		exit(0);
	}

	/* handle -t option */
	if (t_option) {
		if (v_option)
			printf(_("%s: closing tray\n"), programName);
		fd = OpenDevice(deviceName);
		CloseTray(fd);
		HandleXOption(deviceName);
		exit(0);
	}

	/* handle -X option */
	if (X_option) {
		if (v_option)
			printf(_("%s: listing CD-ROM speed\n"), programName);
		fd = OpenDevice(deviceName);
		ListSpeedCdrom(deviceName, fd);
		exit(0);
	}

	/* handle -x option only */
	if (!c_option) HandleXOption(deviceName);

	/* unmount device if mounted */
	if ((m_option != 1) && mounted) {
		if (v_option)
			printf(_("%s: unmounting device `%s' from `%s'\n"), programName, deviceName, mountName);
		Unmount(mountName);
	}

	/* if it is a multipartition device, unmount any other partitions on
	   the device */
	pattern = MultiplePartitions(deviceName);
	if ((m_option != 1) && (pattern != 0))
		UnmountDevices(pattern);

	/* handle -T option */
	if (T_option) {
		if (v_option)
			printf(_("%s: toggling tray\n"), programName);
		fd = OpenDevice(deviceName);
		ToggleTray(fd);
		HandleXOption(deviceName);
		exit(0);
	}
	
	/* handle -c option */
	if (c_option) {
		if (v_option)
			printf(_("%s: selecting CD-ROM disc #%d\n"), programName, c_arg);
		fd = OpenDevice(deviceName);
		ChangerSelect(fd, c_arg);
		HandleXOption(deviceName);
		exit(0);
	}

	/* if user did not specify type of eject, try all four methods */
	if ((r_option + s_option + f_option + q_option) == 0) {
		r_option = s_option = f_option = q_option = 1;
	}

	/* open device */
	fd = OpenDevice(deviceName);

	/* try various methods of ejecting until it works */
	if (r_option) {
		if (v_option)
			printf(_("%s: trying to eject `%s' using CD-ROM eject command\n"), programName, deviceName);
		worked = EjectCdrom(fd);
		if (v_option) {
			if (worked)
				printf(_("%s: CD-ROM eject command succeeded\n"), programName);
			else
				printf(_("%s: CD-ROM eject command failed\n"), programName);
		}
	}

#ifdef HAVE_EJECT_SCSI
	if (s_option && !worked) {
		if (v_option)
			printf(_("%s: trying to eject `%s' using SCSI commands\n"), programName, deviceName);
		worked = EjectScsi(fd);
		if (v_option) {
			if (worked)
				printf(_("%s: SCSI eject succeeded\n"), programName);
			else
				printf(_("%s: SCSI eject failed\n"), programName);
		}
	}
#endif

#ifdef HAVE_EJECT_FLOPPY
	if (f_option && !worked) {
		if (v_option)
			printf(_("%s: trying to eject `%s' using floppy eject command\n"), programName, deviceName);
		worked = EjectFloppy(fd);
		if (v_option) {
			if (worked)
				printf(_("%s: floppy eject command succeeded\n"), programName);
			else
				printf(_("%s: floppy eject command failed\n"), programName);
		}
	}
#endif

#ifdef HAVE_EJECT_TAPE
	if (q_option && !worked) {
		if (v_option)
			printf(_("%s: trying to eject `%s' using tape offline command\n"), programName, deviceName);
		worked = EjectTape(fd);
		if (v_option) {
			if (worked)
				printf(_("%s: tape offline command succeeded\n"), programName);
			else
				printf(_("%s: tape offline command failed\n"), programName);
		}
	}
#endif

	if (!worked) {
		fprintf(stderr, _("%s: unable to eject, last error: %s\n"), programName, strerror(errno));
		exit(1);
	}

	/* cleanup */
	CLOSE(fd);
	free(device);
	free(deviceName);
	free(fullName);
	free(fullNameOrig);
	free(linkName);
	free(mountName);
	free(pattern);
	exit(0);
}
