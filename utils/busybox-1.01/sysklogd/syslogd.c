/* vi: set sw=4 ts=4: */
/*
 * Mini syslogd implementation for busybox
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Copyright (C) 2000 by Karl M. Hegbloom <karlheg@debian.org>
 *
 * "circular buffer" Copyright (C) 2001 by Gennady Feldman <gfeldman@gena01.com>
 *
 * Maintainer: Gennady Feldman <gfeldman@gena01.com> as of Mar 12, 2001
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>
#include <paths.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef AP_PLATFORM 
#include <mesh/mesh_config.h>
#endif

#include "busybox.h"

/* SYSLOG_NAMES defined to pull some extra junk from syslog.h */
#define SYSLOG_NAMES
#include <sys/syslog.h>
#include <sys/uio.h>

/* Path for the file where all log messages are written */
#define __LOG_FILE "/var/log/messages"

/* Path to the unix socket */
static char lfile[MAXPATHLEN];

static const char *logFilePath = __LOG_FILE;

#ifdef CONFIG_FEATURE_ROTATE_LOGFILE
/* max size of message file before being rotated */
static int logFileSize = 200 * 1024;

/* number of rotated message files */
static int logFileRotate = 1;
#endif

/* interval between marks in seconds */
static int MarkInterval = 20 * 60;

/* localhost's name */
static char LocalHostName[64];

#ifdef CONFIG_FEATURE_REMOTE_LOG
#include <netinet/in.h>
/* udp socket for logging to remote host */
static int remotefd = -1;
static int v6_remote =  0;
static struct sockaddr_in remoteaddr;
static struct sockaddr_in6 remoteaddrv6;

#ifndef __FAT_AP__
static int v6_remote_in_msg =  0;
static struct sockaddr_in remoteaddr_in_msg;
static struct sockaddr_in6 remoteaddrv6_in_msg;
#endif

/* where do we log? */
static char *RemoteHost;

/* what port to log to? */
static int RemotePort = 514;

/* To remote log or not to remote log, that is the question. */
static int doRemoteLog = FALSE;
static int local_logging = FALSE;
#endif


#ifdef __FAT_AP__
static int default_priority = LOG_DEBUG;
#endif

/* Make loging output smaller. */
static bool small = false;

#define MAXLINE         1024	/* maximum line length */

/* circular buffer variables/structures */
#ifdef CONFIG_FEATURE_IPC_SYSLOG

#if CONFIG_FEATURE_IPC_SYSLOG_BUFFER_SIZE < 4
#error Sorry, you must set the syslogd buffer size to at least 4KB.
#error Please check CONFIG_FEATURE_IPC_SYSLOG_BUFFER_SIZE
#endif

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

/* our shared key */
static const long KEY_ID = 0x414e4547;	/*"GENA" */

// Semaphore operation structures
static struct shbuf_ds {
	int size;			// size of data written
	int head;			// start of message list
	int tail;			// end of message list
	char data[1];		// data/messages
} *buf = NULL;			// shared memory pointer

static struct sembuf SMwup[1] = { {1, -1, IPC_NOWAIT} };	// set SMwup
static struct sembuf SMwdn[3] = { {0, 0}, {1, 0}, {1, +1} };	// set SMwdn

static int shmid = -1;	// ipc shared memory id
static int s_semid = -1;	// ipc semaphore id
static int shm_size = ((CONFIG_FEATURE_IPC_SYSLOG_BUFFER_SIZE)*1024);	// default shm size
static int circular_logging = FALSE;

/*
 * sem_up - up()'s a semaphore.
 */
static inline void sem_up(int semid)
{
	if (semop(semid, SMwup, 1) == -1) {
		bb_perror_msg_and_die("semop[SMwup]");
	}
}

/*
 * sem_down - down()'s a semaphore
 */
static inline void sem_down(int semid)
{
	if (semop(semid, SMwdn, 3) == -1) {
		bb_perror_msg_and_die("semop[SMwdn]");
	}
}


void ipcsyslog_cleanup(void)
{
	printf("Exiting Syslogd!\n");
	if (shmid != -1) {
		shmdt(buf);
	}

	if (shmid != -1) {
		shmctl(shmid, IPC_RMID, NULL);
	}
	if (s_semid != -1) {
		semctl(s_semid, 0, IPC_RMID, 0);
	}
}

void ipcsyslog_init(void)
{
	if (buf == NULL) {
		if ((shmid = shmget(KEY_ID, shm_size, IPC_CREAT | 1023)) == -1) {
			bb_perror_msg_and_die("shmget");
		}

		if ((buf = shmat(shmid, NULL, 0)) == NULL) {
			bb_perror_msg_and_die("shmat");
		}

		buf->size = shm_size - sizeof(*buf);
		buf->head = buf->tail = 0;

		// we'll trust the OS to set initial semval to 0 (let's hope)
		if ((s_semid = semget(KEY_ID, 2, IPC_CREAT | IPC_EXCL | 1023)) == -1) {
			if (errno == EEXIST) {
				if ((s_semid = semget(KEY_ID, 2, 0)) == -1) {
					bb_perror_msg_and_die("semget");
				}
			} else {
				bb_perror_msg_and_die("semget");
			}
		}
	} else {
		printf("Buffer already allocated just grab the semaphore?");
	}
}

/* write message to buffer */
void circ_message(const char *msg)
{
	int l = strlen(msg) + 1;	/* count the whole message w/ '\0' included */

	sem_down(s_semid);

	/*
	 * Circular Buffer Algorithm:
	 * --------------------------
	 *
	 * Start-off w/ empty buffer of specific size SHM_SIZ
	 * Start filling it up w/ messages. I use '\0' as separator to break up messages.
	 * This is also very handy since we can do printf on message.
	 *
	 * Once the buffer is full we need to get rid of the first message in buffer and
	 * insert the new message. (Note: if the message being added is >1 message then
	 * we will need to "remove" >1 old message from the buffer). The way this is done
	 * is the following:
	 *      When we reach the end of the buffer we set a mark and start from the beginning.
	 *      Now what about the beginning and end of the buffer? Well we have the "head"
	 *      index/pointer which is the starting point for the messages and we have "tail"
	 *      index/pointer which is the ending point for the messages. When we "display" the
	 *      messages we start from the beginning and continue until we reach "tail". If we
	 *      reach end of buffer, then we just start from the beginning (offset 0). "head" and
	 *      "tail" are actually offsets from the beginning of the buffer.
	 *
	 * Note: This algorithm uses Linux IPC mechanism w/ shared memory and semaphores to provide
	 *       a threasafe way of handling shared memory operations.
	 */
	if ((buf->tail + l) < buf->size) {
		/* before we append the message we need to check the HEAD so that we won't
		   overwrite any of the message that we still need and adjust HEAD to point
		   to the next message! */
		if (buf->tail < buf->head) {
			if ((buf->tail + l) >= buf->head) {
				/* we need to move the HEAD to point to the next message
				 * Theoretically we have enough room to add the whole message to the
				 * buffer, because of the first outer IF statement, so we don't have
				 * to worry about overflows here!
				 */
				int k = buf->tail + l - buf->head;	/* we need to know how many bytes
													   we are overwriting to make
													   enough room */
				char *c =
					memchr(buf->data + buf->head + k, '\0',
						   buf->size - (buf->head + k));
				if (c != NULL) {	/* do a sanity check just in case! */
					buf->head = c - buf->data + 1;	/* we need to convert pointer to
													   offset + skip the '\0' since
													   we need to point to the beginning
													   of the next message */
					/* Note: HEAD is only used to "retrieve" messages, it's not used
					   when writing messages into our buffer */
				} else {	/* show an error message to know we messed up? */
					printf("Weird! Can't find the terminator token??? \n");
					buf->head = 0;
				}
			}
		}

		/* in other cases no overflows have been done yet, so we don't care! */
		/* we should be ok to append the message now */
		strncpy(buf->data + buf->tail, msg, l);	/* append our message */
		buf->tail += l;	/* count full message w/ '\0' terminating char */
	} else {
		/* we need to break up the message and "circle" it around */
		char *c;
		int k = buf->tail + l - buf->size;	/* count # of bytes we don't fit */

		/* We need to move HEAD! This is always the case since we are going
		 * to "circle" the message.
		 */
		c = memchr(buf->data + k, '\0', buf->size - k);

		if (c != NULL) {	/* if we don't have '\0'??? weird!!! */
			/* move head pointer */
			buf->head = c - buf->data + 1;

			/* now write the first part of the message */
			strncpy(buf->data + buf->tail, msg, l - k - 1);

			/* ALWAYS terminate end of buffer w/ '\0' */
			buf->data[buf->size - 1] = '\0';

			/* now write out the rest of the string to the beginning of the buffer */
			strcpy(buf->data, &msg[l - k - 1]);

			/* we need to place the TAIL at the end of the message */
			buf->tail = k + 1;
		} else {
			printf
				("Weird! Can't find the terminator token from the beginning??? \n");
			buf->head = buf->tail = 0;	/* reset buffer, since it's probably corrupted */
		}

	}
	sem_up(s_semid);
}
#endif							/* CONFIG_FEATURE_IPC_SYSLOG */

/* Note: There is also a function called "message()" in init.c */
/* Print a message to the log file. */
static void message(char *fmt, ...) __attribute__ ((format(printf, 1, 2)));
static void message(char *fmt, ...)
{
	int fd;
	struct flock fl;
	va_list arguments;

	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 1;

#ifdef CONFIG_FEATURE_IPC_SYSLOG
	if ((circular_logging == TRUE) && (buf != NULL)) {
		char b[1024];

		va_start(arguments, fmt);
		vsnprintf(b, sizeof(b) - 1, fmt, arguments);
		va_end(arguments);
		circ_message(b);

	} else
#endif
	if ((fd =
			 device_open(logFilePath,
							 O_WRONLY | O_CREAT | O_NOCTTY | O_APPEND |
							 O_NONBLOCK)) >= 0) {
		fl.l_type = F_WRLCK;
		fcntl(fd, F_SETLKW, &fl);
#ifdef CONFIG_FEATURE_ROTATE_LOGFILE
		if ( logFileSize > 0 ) {
			struct stat statf;
			int r = fstat(fd, &statf);
			if( !r && (statf.st_mode & S_IFREG)
				&& (lseek(fd,0,SEEK_END) > logFileSize) ) {
				if(logFileRotate > 0) {
					int i;
					char oldFile[(strlen(logFilePath)+3)], newFile[(strlen(logFilePath)+3)];
					for(i=logFileRotate-1;i>1;i--) {
						sprintf(oldFile, "%s.%d", logFilePath, i-1);
						sprintf(newFile, "%s.%d", logFilePath, i);
						rename(oldFile, newFile);
					}
					sprintf(newFile, "%s.%d", logFilePath, 1);
					fl.l_type = F_UNLCK;
					fcntl (fd, F_SETLKW, &fl);
					close(fd);
					rename(logFilePath, newFile);
					fd = device_open (logFilePath,
						   O_WRONLY | O_CREAT | O_NOCTTY | O_APPEND |
						   O_NONBLOCK);
					fl.l_type = F_WRLCK;
					fcntl (fd, F_SETLKW, &fl);
				} else {
					ftruncate( fd, 0 );
				}
			}
		}
#endif
		va_start(arguments, fmt);
		vdprintf(fd, fmt, arguments);
		va_end(arguments);
		fl.l_type = F_UNLCK;
		fcntl(fd, F_SETLKW, &fl);
		close(fd);
	} else {
		/* Always send console messages to /dev/console so people will see them. */
		if ((fd =
			 device_open(_PATH_CONSOLE,
						 O_WRONLY | O_NOCTTY | O_NONBLOCK)) >= 0) {
			va_start(arguments, fmt);
			vdprintf(fd, fmt, arguments);
			va_end(arguments);
			close(fd);
		} else {
			fprintf(stderr, "Bummer, can't print: ");
			va_start(arguments, fmt);
			vfprintf(stderr, fmt, arguments);
			fflush(stderr);
			va_end(arguments);
		}
	}
}

#ifdef CONFIG_FEATURE_REMOTE_LOG
#ifdef AP_PLATFORM 
static void mesh_nonblocking(int rmfd)
{
        int role;
        if (rmfd <0)
            return;

        MESH_GET_ROLE(role);
        if (role != MESH_POINT) {
                return;
        }
        if (fcntl(rmfd, F_SETFL, O_NONBLOCK) <0) { // To avoid blocking meshd and hanging MP (Bug 35084).
                bb_error_msg("mesh_nonblocking: fcntl error in setting O_NONBLOCK");
        }
}
#endif

static void init_RemoteLog(void)
{
    struct addrinfo * result;
    struct addrinfo * res;
    int ret;

	memset(&remoteaddr, 0, sizeof(remoteaddr));
    memset(&remoteaddrv6, 0, sizeof(remoteaddrv6));

    ret = getaddrinfo(RemoteHost, NULL, NULL, &result);
    if (ret) { 
        bb_error_msg("getaddrinfo error: %s",gai_strerror(ret));
        return;
    } 
    
    for (res = result; res != NULL; res = res->ai_next) {
        switch (res->ai_addr->sa_family) {
        case AF_INET6:
            remoteaddrv6.sin6_family = AF_INET6;
            memcpy(remoteaddrv6.sin6_addr.s6_addr, ((struct sockaddr_in6 *)res->ai_addr)->sin6_addr.s6_addr, 16);
            remoteaddrv6.sin6_port = htons(RemotePort);            
            v6_remote = 1;
            break;
        case AF_INET:
            remoteaddr.sin_family = AF_INET;
            remoteaddr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
            remoteaddr.sin_port = htons(RemotePort);
            break;
        default:
            freeaddrinfo(result);
            bb_error_msg("cannot get address info");
            return;
        }
    }
    freeaddrinfo(result);

    if (!v6_remote)
	    remotefd = socket(AF_INET, SOCK_DGRAM, 0);
    else 
        remotefd = socket(AF_INET6, SOCK_DGRAM, 0); 

	if (remotefd < 0) {
		bb_error_msg("cannot create socket");
	}
#ifdef AP_PLATFORM 
        else {
                mesh_nonblocking(remotefd);
        }
#endif
}
#endif

#ifdef __FAT_AP__
#define __LOG_FAC_TYPE__ unsigned char
#else
#define __LOG_FAC_TYPE__ int
#endif

static void logMessage(__LOG_FAC_TYPE__ pri, char *msg, int use_alt_log_dest)
{
	time_t now;
	char *timestamp;
	static char res[20] = "";
#ifdef CONFIG_FEATURE_REMOTE_LOG	
	static char line[MAXLINE + 1];
#endif
	CODE *c_pri, *c_fac;
#ifdef AP_PLATFORM
    struct stat st;
    /*
     * Suppress syslog in maintenance mode.
     */
    if (stat("/tmp/maint_mode", &st) == 0) {
        return;
    }

#endif
#ifdef __FAT_AP__
    if (LOG_PRI(pri) > default_priority)
        return;
#endif
	if (pri != 0) {
		for (c_fac = facilitynames;
			 c_fac->c_name && !(c_fac->c_val == LOG_FAC(pri) << 3); c_fac++);
		for (c_pri = prioritynames;
			 c_pri->c_name && !(c_pri->c_val == LOG_PRI(pri)); c_pri++);
		if (c_fac->c_name == NULL || c_pri->c_name == NULL) {
			snprintf(res, sizeof(res), "<%d>", pri);
		} else {
			snprintf(res, sizeof(res), "%s.%s", c_fac->c_name, c_pri->c_name);
		}
	}

	if (strlen(msg) < 16 || msg[3] != ' ' || msg[6] != ' ' ||
		msg[9] != ':' || msg[12] != ':' || msg[15] != ' ') {
		time(&now);
		timestamp = ctime(&now) + 4;
		timestamp[15] = '\0';
	} else {
		timestamp = msg;
		timestamp[15] = '\0';
		msg += 16;
	}

	/* todo: supress duplicates */

#ifdef CONFIG_FEATURE_REMOTE_LOG

	if (doRemoteLog == TRUE) {

		/* trying connect the socket */
		if (-1 == remotefd) {
			init_RemoteLog();
		}

		/* if we have a valid socket, send the message */
		if (-1 != remotefd) {
                        int is_v6=0;
                        static struct sockaddr_in to_addr;
                        static struct sockaddr_in6 to_addrv6;

			now = 1;
			snprintf(line, sizeof(line), "<%d>%s", pri, msg);

		retry:
#ifndef __FAT_AP__
                        if (use_alt_log_dest) {
                                if (v6_remote_in_msg) {
                                        to_addrv6 = remoteaddrv6_in_msg;
                                        is_v6 = 1;
                                } else
                                        to_addr = remoteaddr_in_msg;
                        } else
#endif
                        {
                                if (v6_remote) {
                                        to_addrv6 = remoteaddrv6;
                                        is_v6 = 1;
                                } else
                                        to_addr = remoteaddr;
                        }
			if(( -1 == sendto(remotefd, line, strlen(line), 0,
					 (is_v6 ? ((struct sockaddr *) &to_addrv6) : ((struct sockaddr *) &to_addr)),
					 (is_v6 ? sizeof(to_addrv6) : sizeof(to_addr))) && (errno == EINTR))) {
				/* sleep now seconds and retry (with now * 2) */
				sleep(now);
				now *= 2;
				goto retry;
			}
		}
	}

	if (local_logging == TRUE)
#endif
	{
		/* now spew out the message to wherever it is supposed to go */
		if (small)
			message("%s %s\n", timestamp, msg);
		else
			message("%s %s %s %s\n", timestamp, LocalHostName, res, msg);
	}
}

static void quit_signal(int sig)
{
	logMessage(LOG_SYSLOG | LOG_INFO, "System log daemon exiting.", 0);
	unlink(lfile);
#ifdef CONFIG_FEATURE_IPC_SYSLOG
	ipcsyslog_cleanup();
#endif

	exit(TRUE);
}

#ifdef __FAT_AP__
static void remote_server(int sig)
{
    FILE * fp;

    if (remotefd >= 0) {
        close (remotefd);
        remotefd = -1;
        doRemoteLog = FALSE;
    }
    if (RemoteHost) {
        free (RemoteHost);
        RemoteHost = NULL;
    }
       
    fp = fopen ("/tmp/syslog_host", "r");
    if (fp) {
        char host[128];

        host[0] = 0;
        fscanf (fp, "%s", host);
        if (host[0]) {
            doRemoteLog = TRUE;
            RemoteHost = bb_xstrdup (host);
            RemotePort = 514;
        }
        fclose (fp);
    }
    else {
        doRemoteLog = FALSE;
    }
}


static void logging_level(int sig)
{
    FILE * fp;

    fp = fopen ("/tmp/syslog_level", "r");
    if (fp) {
        int level = 0;

        
        fscanf (fp, "%d", &level);
        if (level <= LOG_DEBUG)
            default_priority = level;
        fclose (fp);
    }
}

#endif

static void domark(int sig)
{
	if (MarkInterval > 0) {
		logMessage(LOG_SYSLOG | LOG_INFO, "-- MARK --", 0);
		alarm(MarkInterval);
	}
}

#if !defined(__FAT_AP__)
static int
check_if_ip_ok(char *ip, int len)
{
        struct addrinfo * result;
        struct addrinfo * res;
        struct addrinfo   hint;
        int ret;
        struct in6_addr zeroin6 = IN6ADDR_ANY_INIT;
        struct sockaddr_in6 *sin6;
        struct sockaddr_in *sin;
        int count=0;

        if (!strcmp(ip, "0.0.0.0"))
                return 0;
        memset(&remoteaddr_in_msg, 0, sizeof(remoteaddr_in_msg));
        memset(&remoteaddrv6_in_msg, 0, sizeof(remoteaddrv6_in_msg));
        memset(&hint, 0, sizeof(hint));
        hint.ai_family = AF_UNSPEC;
        hint.ai_flags = (AI_NUMERICHOST | AI_ADDRCONFIG);

        ret = getaddrinfo(ip, NULL, &hint, &result);
        if (ret) { 
                return -1; /* error */
        }
    
        for (res = result; res != NULL; res = res->ai_next) {
                ++count;
                switch (res->ai_addr->sa_family) {
                case AF_INET6:
                        sin6 = (struct sockaddr_in6 *)res->ai_addr;
                        if (!memcmp(sin6->sin6_addr.s6_addr, zeroin6.s6_addr, sizeof(zeroin6))) {
                                ret = 0;
                                goto done;
                        }
                        remoteaddrv6_in_msg.sin6_family = AF_INET6;
                        memcpy(remoteaddrv6_in_msg.sin6_addr.s6_addr,
                               ((struct sockaddr_in6 *)res->ai_addr)->sin6_addr.s6_addr, 16);
                        remoteaddrv6_in_msg.sin6_port = htons(RemotePort);            
                        v6_remote_in_msg = 1;
                        ret = 1;
                        break;
                case AF_INET:
                        sin = (struct sockaddr_in *)res->ai_addr;
                        if (sin->sin_addr.s_addr == INADDR_ANY) {
                                ret = 0;
                                goto done;
                        }
                        remoteaddr_in_msg.sin_family = AF_INET;
                        remoteaddr_in_msg.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
                        remoteaddr_in_msg.sin_port = htons(RemotePort);
                        ret = 1;
                        break;
                default:
                        freeaddrinfo(result);
                        return -1;
                }
                if (ret == 1) break;
        }
done:
        freeaddrinfo(result);
        return ret; /* valid addr */
}
#endif

/* This must be a #define, since when CONFIG_DEBUG and BUFFERS_GO_IN_BSS are
 * enabled, we otherwise get a "storage size isn't constant error. */
static int serveConnection(char *tmpbuf, int n_read)
{
#if !defined(__FAT_AP__)
        char log_dest[128];
        int ip_idx=0, saw_ip_begin=0, count=0;
        int got_log_dest_in_msg, ret;
#endif
        int use_log_dest_in_msg = 0;
	char *p = tmpbuf;
        /* extract log destination IP */
#if !defined(__FAT_AP__)
        got_log_dest_in_msg = 0;
#endif
	while (p < tmpbuf + n_read) {

		__LOG_FAC_TYPE__ pri = (LOG_USER | LOG_NOTICE);
		int num_lt = 0;
		char line[MAXLINE + 1];
		unsigned char c;
		char *q = line;

#if !defined(__FAT_AP__)
                if (p == tmpbuf) { /* look for IP only at start of msg */
                        ip_idx = count = 0;
                        while ((count++ < n_read) && (c = *p) && (ip_idx < sizeof(log_dest))) {
                                if (c == '{') saw_ip_begin = 1;
                                else if (c == '}') {
                                        log_dest[ip_idx] = '\0';
                                        if (-1 != (ret = check_if_ip_ok(log_dest, ip_idx))) {
                                                got_log_dest_in_msg = 1;
                                                if (1 == ret) use_log_dest_in_msg = 1;
                                                ++p;
                                        }
                                        break;
                                } else if (saw_ip_begin)
                                        log_dest[ip_idx++] = c;
                                ++p;
                        }
                        if (!got_log_dest_in_msg) p = tmpbuf;
                }
#endif
                while ((c = *p) && q < &line[sizeof(line) - 1]) {
			if (c == '<' && num_lt == 0) {
				/* Parse the magic priority number. */
				num_lt++;
				pri = 0;
				while (isdigit(*(++p))) {
					pri = 10 * pri + (*p - '0');
				}
				if (pri & ~(LOG_FACMASK | LOG_PRIMASK)) {
					pri = (LOG_USER | LOG_NOTICE);
				}
			} else if (c == '\n') {
				*q++ = ' ';
			} else if (iscntrl(c) && (c < 0177)) {
				*q++ = '^';
				*q++ = c ^ 0100;
			} else {
				*q++ = c;
			}
			p++;
		}
		*q = '\0';
		p++;
		/* Now log it */
		logMessage(pri, line, use_log_dest_in_msg);
	}
	return n_read;
}

#ifdef AP_PLATFORM
#ifndef __FAT_AP__
static void
syslogd_restart(int sig)
{
    /*
     * 0xaa tells nanny to not complain about syslogd exiting.
     */

    exit(0xaa);
}
#endif
#endif

static void doSyslogd(void) __attribute__ ((noreturn));
static void doSyslogd(void)
{
	struct sockaddr_un sunx;
	socklen_t addrLength;

	int sock_fd;
	fd_set fds;

	/* Set up signal handlers. */
	signal(SIGINT, quit_signal);
	signal(SIGTERM, quit_signal);
	signal(SIGQUIT, quit_signal);
	signal(SIGHUP, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
#ifdef SIGCLD
	signal(SIGCLD, SIG_IGN);
#endif
#ifdef __FAT_AP__
        signal (SIGUSR1, remote_server);
        signal (SIGUSR2, logging_level);
#endif
	signal(SIGALRM, domark);
	alarm(MarkInterval);

#ifdef AP_PLATFORM
    /*
     * When the AP changes from one LMS to another, syslogd is restarted
     * with SIGUSR1. The syslogd_restart function exits with code 0xaa
     * so that nanny doesn't report an error.
     */
#ifndef __FAT_AP__
    signal(SIGUSR1, syslogd_restart);
#endif
#endif

	/* Create the syslog file so realpath() can work. */
	if (realpath(_PATH_LOG, lfile) != NULL) {
		unlink(lfile);
	}

	memset(&sunx, 0, sizeof(sunx));
	sunx.sun_family = AF_UNIX;
	strncpy(sunx.sun_path, lfile, sizeof(sunx.sun_path));
	if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
		bb_perror_msg_and_die("Couldn't get file descriptor for socket "
						   _PATH_LOG);
	}

	addrLength = sizeof(sunx.sun_family) + strlen(sunx.sun_path);
	if (bind(sock_fd, (struct sockaddr *) &sunx, addrLength) < 0) {
		bb_perror_msg_and_die("Could not connect to socket " _PATH_LOG);
	}

	if (chmod(lfile, 0666) < 0) {
		bb_perror_msg_and_die("Could not set permission on " _PATH_LOG);
	}
#ifdef CONFIG_FEATURE_IPC_SYSLOG
	if (circular_logging == TRUE) {
		ipcsyslog_init();
	}
#endif

#ifdef CONFIG_FEATURE_REMOTE_LOG
	if (doRemoteLog == TRUE) {
		init_RemoteLog();
	}
#endif

// SCA; removed
//	logMessage(LOG_SYSLOG | LOG_INFO, "syslogd started: " BB_BANNER);

	for (;;) {

		FD_ZERO(&fds);
		FD_SET(sock_fd, &fds);

		if (select(sock_fd + 1, &fds, NULL, NULL, NULL) < 0) {
			if (errno == EINTR) {
				/* alarm may have happened. */
				continue;
			}
			bb_perror_msg_and_die("select error");
		}

		if (FD_ISSET(sock_fd, &fds)) {
			int i;

			RESERVE_CONFIG_BUFFER(tmpbuf, MAXLINE + 1);

			memset(tmpbuf, '\0', MAXLINE + 1);
			if ((i = recv(sock_fd, tmpbuf, MAXLINE, 0)) > 0) {
				serveConnection(tmpbuf, i);
			} else {
				bb_perror_msg_and_die("UNIX socket error");
			}
			RELEASE_CONFIG_BUFFER(tmpbuf);
		}				/* FD_ISSET() */
	}					/* for main loop */
}

extern int syslogd_main(int argc, char **argv)
{

	int opt;

	int doFork = TRUE;

	char *p;

	/* do normal option parsing */
	while ((opt = getopt(argc, argv, "m:nO:s:Sb:R:P:LC::")) > 0) {
		switch (opt) {
		case 'm':
			MarkInterval = atoi(optarg) * 60;
			break;
		case 'n':
			doFork = FALSE;
			break;
		case 'O':
			logFilePath = optarg;
			break;
#ifdef CONFIG_FEATURE_ROTATE_LOGFILE
		case 's':
			logFileSize = atoi(optarg) * 1024;
			break;
		case 'b':
			logFileRotate = atoi(optarg);
			if( logFileRotate > 99 ) logFileRotate = 99;
			break;
#endif
#ifdef CONFIG_FEATURE_REMOTE_LOG
		case 'R':
			RemoteHost = bb_xstrdup(optarg);
			doRemoteLog = TRUE;
			break;
        case 'P': 
            RemotePort = atoi(optarg);
            break;
		case 'L':
			local_logging = TRUE;
			break;
#endif
#ifdef CONFIG_FEATURE_IPC_SYSLOG
		case 'C':
			if (optarg) {
				int buf_size = atoi(optarg);
				if (buf_size >= 4) {
					shm_size = buf_size * 1024;
				}
			}
			circular_logging = TRUE;
			break;
#endif
		case 'S':
			small = true;
			break;
		default:
			bb_show_usage();
		}
	}

#ifdef CONFIG_FEATURE_REMOTE_LOG
	/* If they have not specified remote logging, then log locally */
	if (doRemoteLog == FALSE)
		local_logging = TRUE;
#endif


	/* Store away localhost's name before the fork */
	gethostname(LocalHostName, sizeof(LocalHostName));
	if ((p = strchr(LocalHostName, '.'))) {
		*p = '\0';
	}

	umask(0);

	if (doFork == TRUE) {
#if defined(__uClinux__)
		vfork_daemon_rexec(0, 1, argc, argv, "-n");
#else /* __uClinux__ */
		if(daemon(0, 1) < 0)
			bb_perror_msg_and_die("daemon");
#endif /* __uClinux__ */
	}
	doSyslogd();

	return EXIT_SUCCESS;
}

/*
Local Variables
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
