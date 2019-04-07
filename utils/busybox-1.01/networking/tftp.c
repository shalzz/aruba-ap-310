/* ------------------------------------------------------------------------- */
/* tftp.c                                                                    */
/*                                                                           */
/* A simple tftp client for busybox.                                         */
/* Tries to follow RFC1350.                                                  */
/* Only "octet" mode supported.                                              */
/* Optional blocksize negotiation (RFC2347 + RFC2348)                        */
/*                                                                           */
/* Copyright (C) 2001 Magnus Damm <damm@opensource.se>                       */
/*                                                                           */
/* Parts of the code based on:                                               */
/*                                                                           */
/* atftp:  Copyright (C) 2000 Jean-Pierre Lefebvre <helix@step.polymtl.ca>   */
/*                        and Remi Lefebvre <remi@debian.org>                */
/*                                                                           */
/* utftp:  Copyright (C) 1999 Uwe Ohse <uwe@ohse.de>                         */
/*                                                                           */
/* This program is free software; you can redistribute it and/or modify      */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation; either version 2 of the License, or         */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          */
/* General Public License for more details.                                  */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA   */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <net/if.h>
#ifdef __FAT_AP__
#include <sys/ioctl.h>
#include <util/dns_res.h>
#endif

#include "busybox.h"

#ifdef AP_PLATFORM
#ifndef __FAT_AP__
#include "syslog/syslogutil.h"
#include "syslog/arubalog.h"
#endif
#endif

//#define CONFIG_FEATURE_TFTP_DEBUG

#define TFTP_BLOCKSIZE_DEFAULT 512 /* according to RFC 1350, don't change */
#define TFTP_TIMEOUT 5             /* seconds */

/* opcodes we support */

#define TFTP_RRQ   1
#define TFTP_WRQ   2
#define TFTP_DATA  3
#define TFTP_ACK   4
#define TFTP_ERROR 5
#define TFTP_OACK  6

static const char *tftp_bb_error_msg[] = {
	"Undefined error",
	"File not found",
	"Access violation",
	"Disk full or allocation error",
	"Illegal TFTP operation",
	"Unknown transfer ID",
	"File already exists",
	"No such user"
};

#ifdef __FAT_AP__
const static char *interface_name = NULL;
#endif

#ifdef AP_PLATFORM
#ifndef __FAT_AP__
#define SYSTEM_MSG_TFTP_GENERAL_WARN(function, file, line, msg) \
    do { \
        arubaNewLog(LOG_APDEBUG, LOG_WARNING, 36001, " ^[function %s] [file %s] [line %d] [msg %s]", function, file, line, msg); \
    } while(0)
#endif
#endif

const int tftp_cmd_get = 1;
const int tftp_cmd_put = 2;

#ifdef CONFIG_FEATURE_TFTP_BLOCKSIZE

static int tftp_blocksize_check(int blocksize, int bufsize)
{
        /* Check if the blocksize is valid:
	 * RFC2348 says between 8 and 65464,
	 * but our implementation makes it impossible
	 * to use blocksizes smaller than 22 octets.
	 */

        if ((bufsize && (blocksize > bufsize)) ||
	    (blocksize < 8) || (blocksize > 65464)) {
	        bb_error_msg("bad blocksize");
	        return 0;
	}

	return blocksize;
}

static char *tftp_option_get(char *buf, int len, char *option)
{
        int opt_val = 0;
	int opt_found = 0;
	int k;

	while (len > 0) {

	        /* Make sure the options are terminated correctly */

	        for (k = 0; k < len; k++) {
		        if (buf[k] == '\0') {
			        break;
			}
		}

		if (k >= len) {
		        break;
		}

		if (opt_val == 0) {
			if (strcasecmp(buf, option) == 0) {
			        opt_found = 1;
			}
		}
		else {
		        if (opt_found) {
				return buf;
			}
		}

		k++;

		buf += k;
		len -= k;

		opt_val ^= 1;
	}

	return NULL;
}

#endif

static inline int tftp(const int cmd, const struct hostent *host,
	const char *remotefile, int localfd, const unsigned short port, int tftp_bufsize)
{
	const int cmd_get = cmd & tftp_cmd_get;
	const int cmd_put = cmd & tftp_cmd_put;
	const int bb_tftp_num_retries = 5;
    struct sockaddr_in6 sa;
    struct sockaddr_in6 from;
	struct timeval tv;
	socklen_t fromlen;
	fd_set rfds;
	char *cp;
	unsigned short tmp;
	int socketfd;
	int len;
	int opcode = 0;
	int finished = 0;
	int timeout = bb_tftp_num_retries;
	unsigned short block_nr = 1;
        int datalen = 0;
        int resends = 0;

#ifdef CONFIG_FEATURE_TFTP_BLOCKSIZE
	int want_option_ack = 0;
#endif

#ifdef AP_PLATFORM
    int tftp_is_strap;
	int tftp_is_fatap;
	struct ifreq interface;
#endif
	/* Can't use RESERVE_CONFIG_BUFFER here since the allocation
	 * size varies meaning BUFFERS_GO_ON_STACK would fail */
	char *buf=xmalloc(tftp_bufsize + 4);

	tftp_bufsize += 4;

    if ((socketfd = socket(PF_INET6, SOCK_DGRAM, 0)) < 0) {
        bb_perror_msg("socket");
        return EXIT_FAILURE;
    }

#ifdef __FAT_AP__
        memset(&interface,0,sizeof(interface));
        if (interface_name != NULL) {
            int ret = 0;
            int _socket = socket(PF_INET, SOCK_DGRAM, 0);

            if (_socket) {
            memset(&interface,0,sizeof(interface));
            strncpy(interface.ifr_name, interface_name, IFNAMSIZ);
            ret = ioctl (_socket, SIOCGIFADDR, &interface);
            if (ret < 0) {
                //bb_perror_msg("ioctl get if addr");
            }

            close(_socket);
            }

        }
#endif

	len = sizeof(sa);

	memset(&sa, 0, len);
#ifdef __FAT_AP__
    sa.sin6_family = AF_INET6;
    if (interface_name != NULL) {
        sa.sin6_addr.s6_addr16[5] = 0xffff;
        sa.sin6_addr.s6_addr32[3] = (((struct sockaddr_in *)&(interface.ifr_netmask))->sin_addr.s_addr);
    }
#endif
	bind(socketfd, (struct sockaddr *)&sa, len);

    sa.sin6_family = AF_INET6;
    sa.sin6_port = port;
    memcpy(&sa.sin6_addr, host->h_addr, sizeof(sa.sin6_addr));

#ifdef AP_PLATFORM

#define PAPUSER_PATH           "/tmp/papuser"
#define PAPPASSWD_PATH         "/tmp/pappasswd"
#define IKEPSK_PATH            "/tmp/ikepsk"
#define REMOTEAP_PATH          "/tmp/remote_ap"
#define FATAP_FILE             "/tmp/fatap"
#define BRIDGE_DEVNAME         "br0"

    // Check if fatap mode or not
	tftp_is_fatap = 0;
    {
        FILE *fp1 = NULL;
        if ((fp1 = fopen(FATAP_FILE, "r")) != NULL) {
            tftp_is_fatap = 1;
        }
    }

	if ( ! tftp_is_fatap) {
	    tftp_is_strap   = 0; // initialize strap = 0
        {
            FILE *fp1 = NULL, *fp2 = NULL, *fp3 = NULL;

            if (((fp1 = fopen(PAPUSER_PATH, "r")) != NULL) &&
                ((fp2 = fopen(PAPPASSWD_PATH, "r")) != NULL) &&
                ((fp3 = fopen(IKEPSK_PATH, "r")) != NULL)) {
                tftp_is_strap = 1;
            }
            if (fp1) { fclose(fp1); }
            if (fp2) { fclose(fp2); }
            if (fp3) { fclose(fp3); }
        }
    
        /* Only do if ikepsk/etc are not present, try to figure out is
         * this ap wants to be a rap or not */
        if(tftp_is_strap == 0) {
            char rapbuf[2];
            int is_rap = 0;
            FILE *fp = NULL;
            if ((fp = fopen(REMOTEAP_PATH, "r")) != NULL) {
                if (fscanf(fp, "%1s", rapbuf) == 1) {
                    is_rap = atoi(rapbuf);
                    if(is_rap == 1) {
                        tftp_is_strap = 1;
                    }
                }
            }
            if (fp) { fclose(fp); }
        }

	    if ( ! tftp_is_strap){
		    strncpy(interface.ifr_ifrn.ifrn_name, BRIDGE_DEVNAME, IFNAMSIZ);
		    (void)setsockopt(socketfd, SOL_SOCKET,
			    	SO_BINDTODEVICE, &interface, sizeof(interface));
	    }
	}
#endif

	/* build opcode */

	if (cmd_get) {
		opcode = TFTP_RRQ;
	}

	if (cmd_put) {
		opcode = TFTP_WRQ;
	}

	while (1) {

		cp = buf;

		/* first create the opcode part */

		*((unsigned short *) cp) = htons(opcode);

		cp += 2;

		/* add filename and mode */

		if ((cmd_get && (opcode == TFTP_RRQ)) ||
			(cmd_put && (opcode == TFTP_WRQ))) {
                        int too_long = 0;

			/* see if the filename fits into buf */
			/* and fill in packet                */

			len = strlen(remotefile) + 1;

			if ((cp + len) >= &buf[tftp_bufsize - 1]) {
			        too_long = 1;
			}
			else {
			        safe_strncpy(cp, remotefile, len);
				cp += len;
			}

			if (too_long || ((&buf[tftp_bufsize - 1] - cp) < 6)) {
				bb_error_msg("too long remote-filename");
				break;
			}

			/* add "mode" part of the package */

			memcpy(cp, "octet", 6);
			cp += 6;

#ifdef CONFIG_FEATURE_TFTP_BLOCKSIZE

			len = tftp_bufsize - 4; /* data block size */

			if (len != TFTP_BLOCKSIZE_DEFAULT) {

			        if ((&buf[tftp_bufsize - 1] - cp) < 15) {
				        bb_error_msg("too long remote-filename");
					break;
				}

				/* add "blksize" + number of blocks  */

				memcpy(cp, "blksize", 8);
				cp += 8;

				cp += snprintf(cp, 6, "%d", len) + 1;

				want_option_ack = 1;
			}
#endif
		}

		/* add ack and data */

		if ((cmd_get && (opcode == TFTP_ACK)) ||
			(cmd_put && (opcode == TFTP_DATA))) {

			*((unsigned short *) cp) = htons(block_nr);

			cp += 2;

			block_nr++;

			if (cmd_put && (opcode == TFTP_DATA)) {
				len = bb_full_read(localfd, cp, tftp_bufsize - 4);

				if (len < 0) {
					bb_perror_msg("read");
					break;
				}

				datalen = len;
				if (len != (tftp_bufsize - 4)) {
					finished++;
				}

				cp += len;
			}
		}


		/* send packet */


		timeout = bb_tftp_num_retries;  /* re-initialize */
		do {

			len = cp - buf;

#ifdef CONFIG_FEATURE_TFTP_DEBUG
			fprintf(stderr, "sending %u bytes\n", len);
			for (cp = buf; cp < &buf[len]; cp++)
				fprintf(stderr, "%02x ", (unsigned char)*cp);
			fprintf(stderr, "\n");
#endif
			if (sendto(socketfd, buf, len, 0,
					(struct sockaddr *) &sa, sizeof(sa)) < 0) {
				bb_perror_msg("send");
				len = -1;
				break;
			}


			if (finished && (opcode == TFTP_ACK)) {
				break;
			}

			/* receive packet */

			memset(&from, 0, sizeof(from));
			fromlen = sizeof(from);

			tv.tv_sec = TFTP_TIMEOUT;
			tv.tv_usec = 0;

			FD_ZERO(&rfds);
			FD_SET(socketfd, &rfds);

			switch (select(socketfd + 1, &rfds, NULL, NULL, &tv)) {
			case 1:
				len = recvfrom(socketfd, buf, tftp_bufsize, 0,
						(struct sockaddr *) &from, &fromlen);

				if (len < 0) {
					bb_perror_msg("recvfrom");
					break;
				}

				timeout = 0;

                if (sa.sin6_port == port) {
                    sa.sin6_port = from.sin6_port;
                }
                if (sa.sin6_port == from.sin6_port) {
                    break;
                }

				/* fall-through for bad packets! */
				/* discard the packet - treat as timeout */
				timeout = bb_tftp_num_retries;

			case 0:
				bb_error_msg("timeout");

				timeout--;
				if (timeout == 0) {
					len = -1;
					bb_error_msg("last timeout");
				}
				break;

			default:
				bb_perror_msg("select");
				len = -1;
			}

		} while (timeout && (len >= 0));

		if ((finished) || (len < 0)) {
			break;
		}

		/* process received packet */


		opcode = ntohs(*((unsigned short *) buf));
		tmp = ntohs(*((unsigned short *) &buf[2]));

#ifdef CONFIG_FEATURE_TFTP_DEBUG
		fprintf(stderr, "received %d bytes: %04x %04x\n", len, opcode, tmp);
#endif

		if (opcode == TFTP_ERROR) {
			char *msg = NULL;

			if (buf[4] != '\0') {
				msg = &buf[4];
				buf[tftp_bufsize - 1] = '\0';
			} else if (tmp < (sizeof(tftp_bb_error_msg)
					  / sizeof(char *))) {

				msg = (char *) tftp_bb_error_msg[tmp];
			}

			if (msg) {
				bb_error_msg("server says: %s", msg);
			}

			break;
		}

#ifdef CONFIG_FEATURE_TFTP_BLOCKSIZE
		if (want_option_ack) {

			 want_option_ack = 0;

		         if (opcode == TFTP_OACK) {

			         /* server seems to support options */

			         char *res;

				 res = tftp_option_get(&buf[2], len-2,
						       "blksize");

				 if (res) {
				         int blksize = atoi(res);
			
					 if (tftp_blocksize_check(blksize,
							   tftp_bufsize - 4)) {

					         if (cmd_put) {
				                         opcode = TFTP_DATA;
						 }
						 else {
				                         opcode = TFTP_ACK;
						 }
#ifdef CONFIG_FEATURE_TFTP_DEBUG
						 fprintf(stderr, "using blksize %u\n", blksize);
#endif
					         tftp_bufsize = blksize + 4;
						 block_nr = 0;
						 continue;
					 }
				 }
				 /* FIXME:
				  * we should send ERROR 8 */
				 bb_error_msg("bad server option");
				 break;
			 }

			 bb_error_msg("warning: blksize not supported by server"
				   " - reverting to 512");

			 tftp_bufsize = TFTP_BLOCKSIZE_DEFAULT + 4;
		}
#endif

		if (cmd_get && (opcode == TFTP_DATA)) {

			if (tmp == block_nr) {
				len = bb_full_write(localfd, &buf[4], len - 4);

				if (len < 0) {
					bb_perror_msg("write");
					break;
				}

				if (len != (tftp_bufsize - 4)) {
					finished++;
				}

				opcode = TFTP_ACK;
				continue;
			}
			/* in case the last ack disappeared into the ether */
			if ( tmp == (block_nr - 1) ) {
				--block_nr;
				opcode = TFTP_ACK;
				continue;
			} else if (tmp + 1 == block_nr) {
				/* Server lost our TFTP_ACK.  Resend it */
				block_nr = tmp;
				opcode = TFTP_ACK;
				continue;
			}
		}

		if (cmd_put && (opcode == TFTP_ACK)) {

			if (tmp == (unsigned short)(block_nr - 1)) {
				if (finished) {
					break;
				}

				opcode = TFTP_DATA;
				resends = 0;
				continue;
			} else {
			        /*
                                 * Resend last block.
                                 */
                                finished = 0;
                                if (++resends > bb_tftp_num_retries) {
					bb_error_msg("max retries exceeded");
                                        break;
                                }
                                --block_nr;
				lseek(localfd, -datalen, SEEK_CUR);
				opcode = TFTP_DATA;
				continue;
			}
		}
	}

#ifdef CONFIG_FEATURE_CLEAN_UP
	close(socketfd);

        free(buf);
#endif

	return finished ? EXIT_SUCCESS : EXIT_FAILURE;
}

struct hostent * aruba_aos_gethostbyname(char *host_name)
{
    struct hostent *host = NULL;

    host = gethostbyname2(host_name, AF_INET);
    if (!host) {
        host = gethostbyname2(host_name, AF_INET6);
    }
    return host;
}

int tftp_main(int argc, char **argv)
{
	struct hostent *host = NULL;
	struct hostent host6;
    struct in6_addr ipv4_mapped_v6 = {{{0}}};
#ifdef CONFIG_FEATURE_TFTP_DEBUG
    char str[INET6_ADDRSTRLEN];
#endif
	const char *localfile = NULL;
	const char *remotefile = NULL;
	int port;
	int cmd = 0;
	int fd = -1;
	int flags = 0;
	int opt;
	int result;
	int blocksize = TFTP_BLOCKSIZE_DEFAULT;
	/* figure out what to pass to getopt */

#ifdef CONFIG_FEATURE_TFTP_BLOCKSIZE
#define BS "b:"
#else
#define BS
#endif

#ifdef CONFIG_FEATURE_TFTP_GET
#define GET "g"
#else
#define GET
#endif

#ifdef CONFIG_FEATURE_TFTP_PUT
#define PUT "p"
#else
#define PUT
#endif

#ifdef __FAT_AP__
#define INTF "i:"    
#else
#define INTF
#endif

	while ((opt = getopt(argc, argv, BS GET PUT INTF "l:r:")) != -1) {
		switch (opt) {
#ifdef CONFIG_FEATURE_TFTP_BLOCKSIZE
		case 'b':
			blocksize = atoi(optarg);
			if (!tftp_blocksize_check(blocksize, 0)) {
                                return EXIT_FAILURE;
			}
			break;
#endif
#ifdef CONFIG_FEATURE_TFTP_GET
		case 'g':
			cmd = tftp_cmd_get;
			flags = O_WRONLY | O_CREAT | O_TRUNC;
			break;
#endif
#ifdef CONFIG_FEATURE_TFTP_PUT
		case 'p':
			cmd = tftp_cmd_put;
			flags = O_RDONLY;
			break;
#endif
		case 'l':
			localfile = optarg;
			break;
		case 'r':
			remotefile = optarg;
			break;
#ifdef __FAT_AP__
        case 'i':
            interface_name = optarg;
            break;
#endif
		}
	}

	if ((cmd == 0) || (optind == argc)) {
		bb_show_usage();
	}
	if(localfile && strcmp(localfile, "-") == 0) {
	    fd = fileno((cmd==tftp_cmd_get)? stdout : stdin);
	}
	if(localfile == NULL)
	    localfile = remotefile;
	if(remotefile == NULL)
	    remotefile = localfile;
	if (fd==-1) {
	    fd = open(localfile, flags, 0644);
	}

	if (fd < 0) {
		bb_perror_msg_and_die("local file");
	}


    host = aruba_aos_gethostbyname(argv[optind]);
    if (!host) return EXIT_FAILURE;
    if (host->h_addrtype == AF_INET) {
        unsigned int ipv4_ip = *(unsigned int *) (host->h_addr); 
        ipv4_mapped_v6.s6_addr16[5] = 0xffff;
        ipv4_mapped_v6.s6_addr32[3] = ipv4_ip;
        memcpy(&host6, host, sizeof(host6));
        host6.h_addrtype = AF_INET6;
        host6.h_addr = (char*) &ipv4_mapped_v6;
    } else {
        memcpy(&host6, host, sizeof(host6));
    }
    port = bb_lookup_port(argv[optind + 1], "udp", 69);

#ifdef CONFIG_FEATURE_TFTP_DEBUG
    if (inet_ntop(AF_INET6, (struct in6_addr*)(host6.h_addr), str, INET6_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "using server \"%s\", remotefile \"%s\", "
            "localfile \"%s\".\n",str, remotefile, localfile);
#endif
	result = tftp(cmd, &host6, remotefile, fd, port, blocksize);


#ifdef CONFIG_FEATURE_CLEAN_UP
	if (!(fd == STDOUT_FILENO || fd == STDIN_FILENO)) {
	    close(fd);
	}
#endif
	return(result);
}
