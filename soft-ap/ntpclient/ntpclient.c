/*
 * ntpclient.c - NTP client
 *
 * Copyright 1997, 1999, 2000, 2003  Larry Doolittle  <larry@doolittle.boa.org>
 * Last hack: July 5, 2003
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (Version 2,
 *  June 1991) as published by the Free Software Foundation.  At the
 *  time of writing, that license was published by the FSF with the URL
 *  http://www.gnu.org/copyleft/gpl.html, and is incorporated herein by
 *  reference.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  Possible future improvements:
 *      - Double check that the originate timestamp in the received packet
 *        corresponds to what we sent.
 *      - Verify that the return packet came from the host we think
 *        we're talking to.  Not necessarily useful since UDP packets
 *        are so easy to forge.
 *      - Write more documentation  :-(
 *
 *  Compile with -D_PRECISION_SIOCGSTAMP if your machine really has it.
 *  There are patches floating around to add this to Linux, but
 *  usually you only get an answer to the nearest jiffy.
 *  Hint for Linux hacker wannabes: look at the usage of get_fast_time()
 *  in net/core/dev.c, and its definition in kernel/time.c .
 *
 *  If the compile gives you any flak, check below in the section
 *  labelled "XXXX fixme - non-automatic build configuration".
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>     /* gethostbyname */
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#ifdef _PRECISION_SIOCGSTAMP
#include <sys/ioctl.h>
#endif
#ifdef __FAT_AP__
#include <syslog.h>
#include <util/dns_res.h>
#endif

#define ENABLE_DEBUG

extern char *optarg;

/* XXXX fixme - non-automatic build configuration */
#ifdef linux
#include <sys/utsname.h>
#include <sys/time.h>
typedef u_int32_t __u32;
#include <sys/timex.h>
#else
extern struct hostent *gethostbyname(const char *name);
extern int h_errno;
#define herror(hostname) \
	fprintf(stderr,"Error %d looking up hostname %s\n", h_errno,hostname)
typedef uint32_t __u32;
#endif

#define JAN_1970        0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */
#define NTP_PORT (123)

/* How to multiply by 4294.967296 quickly (and not quite exactly)
 * without using floating point or greater than 32-bit integers.
 * If you want to fix the last 12 microseconds of error, add in
 * (2911*(x))>>28)
 */
#define NTPFRAC(x) ( 4294*(x) + ( (1981*(x))>>11 ) )

/* The reverse of the above, needed if we want to set our microsecond
 * clock (via settimeofday) based on the incoming time in NTP format.
 * Basically exact.
 */
#define USEC(x) ( ( (x) >> 12 ) - 759 * ( ( ( (x) >> 10 ) + 32768 ) >> 16 ) )

/* Converts NTP delay and dispersion, apparently in seconds scaled
 * by 65536, to microseconds.  RFC1305 states this time is in seconds,
 * doesn't mention the scaling.
 * Should somehow be the same as 1000000 * x / 65536
 */
#define sec2u(x) ( (x) * 15.2587890625 )

struct ntptime {
	unsigned int coarse;
	unsigned int fine;
};

/* prototype for function defined in phaselock.c */
int contemplate_data(unsigned int absolute, double skew, double errorbar, int freq);

/* prototypes for some local routines */
void send_packet(int usd);
int rfc1305print(uint32_t *data, struct ntptime *arrival);
void udp_handle(int usd, char *data, int data_len, struct sockaddr *sa_source, int sa_len);

/* variables with file scope
 * (I know, bad form, but this is a short program) */
static uint32_t incoming_word[325];
#define incoming ((char *) incoming_word)
#define sizeof_incoming (sizeof incoming_word)
static struct timeval time_of_send;
static int live=0;
static int set_clock=0;   /* non-zero presumably needs root privs */

/* when present, debug is a true global, shared with phaselock.c */
#ifdef ENABLE_DEBUG
int debug=0;
#define DEBUG_OPTION "d"
#else
#define debug 0
#define DEBUG_OPTION
#endif

#ifdef __FAT_AP__
static struct sockaddr_in6 ntp_server_dest;
static char *hostname=NULL;          /* must be set */
void setup_transmit(int usd, char *host, short port);
#else
static struct sockaddr_in ntp_server_dest;
#endif

int get_current_freq(void)
{
	/* OS dependent routine to get the current value of clock frequency.
	 */
#ifdef linux
	struct timex txc;
	txc.modes=0;
	if (adjtimex(&txc) < 0) {
		perror("adjtimex"); exit(1);
	}
	return txc.freq;
#else
	return 0;
#endif
}

int set_freq(int new_freq)
{
	/* OS dependent routine to set a new value of clock frequency.
	 */
#ifdef linux
	struct timex txc;
	txc.modes = ADJ_FREQUENCY;
	txc.freq = new_freq;
	if (adjtimex(&txc) < 0) {
		perror("adjtimex"); exit(1);
	}
	return txc.freq;
#else
	return 0;
#endif
}

void send_packet(int usd)
{
	__u32 data[12];
	struct timeval now;
#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4 
#define PREC -6

#ifdef __FAT_AP__
	setup_transmit(usd, hostname, NTP_PORT);
#endif
	if (debug) fprintf(stderr,"Sending ...\n");
	if (sizeof(data) != 48) {
		fprintf(stderr,"size error\n");
		return;
	}
	bzero((char *) data,sizeof(data));
	data[0] = htonl (
		( LI << 30 ) | ( VN << 27 ) | ( MODE << 24 ) |
		( STRATUM << 16) | ( POLL << 8 ) | ( PREC & 0xff ) );
	data[1] = htonl(1<<16);  /* Root Delay (seconds) */
	data[2] = htonl(1<<16);  /* Root Dispersion (seconds) */
	gettimeofday(&now,NULL);
	data[10] = htonl(now.tv_sec + JAN_1970); /* Transmit Timestamp coarse */
	data[11] = htonl(NTPFRAC(now.tv_usec));  /* Transmit Timestamp fine   */
#if 0
	send(usd,data,48,0);
#else
	sendto(usd,data,48,0,(struct sockaddr *)&ntp_server_dest,
	       sizeof(ntp_server_dest));
#endif
	time_of_send=now;
}

void get_packet_timestamp(int usd, struct ntptime *udp_arrival_ntp)
{
	struct timeval udp_arrival;
#ifdef _PRECISION_SIOCGSTAMP
	if ( ioctl(usd, SIOCGSTAMP, &udp_arrival) < 0 ) {
		perror("ioctl-SIOCGSTAMP");
		gettimeofday(&udp_arrival,NULL);
	}
#else
	gettimeofday(&udp_arrival,NULL);
#endif
	udp_arrival_ntp->coarse = udp_arrival.tv_sec + JAN_1970;
	udp_arrival_ntp->fine   = NTPFRAC(udp_arrival.tv_usec);
}

void check_source(int data_len, struct sockaddr *sa_source, int sa_len)
{
	/* This is where one could check that the source is the server we expect */
	if (debug) {
		struct sockaddr_in *sa_in=(struct sockaddr_in *)sa_source;
		printf("packet of length %d received\n",data_len);
		if (sa_source->sa_family==AF_INET) {
			printf("Source: INET Port %d host %s\n",
				ntohs(sa_in->sin_port),inet_ntoa(sa_in->sin_addr));
		} else {
			printf("Source: Address family %d\n",sa_source->sa_family);
		}
	}
}

double ntpdiff( struct ntptime *start, struct ntptime *stop)
{
	int a;
	unsigned int b;
	a = stop->coarse - start->coarse;
	if (stop->fine >= start->fine) {
		b = stop->fine - start->fine;
	} else {
		b = start->fine - stop->fine;
		b = ~b;
		a -= 1;
	}
	
	return a*1.e6 + b * (1.e6/4294967296.0);
}

#ifdef __FAT_AP__
#define NTP_IPC_FILE   "/tmp/ntp.ipc" 
static int is_time_from_image_file()
{
#define AWC_TIME_FILE  "/tmp/awc.time"
    if (!access(AWC_TIME_FILE,F_OK) || !access(NTP_IPC_FILE,F_OK)) {
        return 0;
    }
    return 1;
}
#endif

/* Does more than print, so this name is bogus.
 * It also makes time adjustments, both sudden (-s)
 * and phase-locking (-l).  */
/* return value is number of microseconds uncertainty in answer */
int rfc1305print(uint32_t *data, struct ntptime *arrival)
{
/* straight out of RFC-1305 Appendix A */
	int li, vn, mode, stratum, poll, prec;
	int delay, disp, refid;
	struct ntptime reftime, orgtime, rectime, xmttime;
	double el_time,st_time,skew1,skew2;
	int freq;
    FILE *fp = NULL;

#define Data(i) ntohl(((uint32_t *)data)[i])
	li      = Data(0) >> 30 & 0x03;
	vn      = Data(0) >> 27 & 0x07;
	mode    = Data(0) >> 24 & 0x07;
	stratum = Data(0) >> 16 & 0xff;
	poll    = Data(0) >>  8 & 0xff;
	prec    = Data(0)       & 0xff;
	if (prec & 0x80) prec|=0xffffff00;
	delay   = Data(1);
	disp    = Data(2);
	refid   = Data(3);
	reftime.coarse = Data(4);
	reftime.fine   = Data(5);
	orgtime.coarse = Data(6);
	orgtime.fine   = Data(7);
	rectime.coarse = Data(8);
	rectime.fine   = Data(9);
	xmttime.coarse = Data(10);
	xmttime.fine   = Data(11);
#undef Data

	if (set_clock) {   /* you'd better be root, or ntpclient will crash! */
		struct timeval tv_set;
		/* it would be even better to subtract half the slop */
		tv_set.tv_sec  = xmttime.coarse - JAN_1970;
		/* divide xmttime.fine by 4294.967296 */
		tv_set.tv_usec = USEC(xmttime.fine);
#ifdef __FAT_AP__

        if (!is_time_from_image_file())
        {

            struct timeval now;
            char cmd[1024];

            gettimeofday(&now, NULL);
            syslog(LOG_WARNING, "ntpclient(%s:%d): set time from %lu.%.6lu to %lu.%.6lu"
                                "(reference: %u.%.10u, originate: %u.%.10u, receive: %u.%.10u, transmit: %u.%.10u, our-recv: %u.%.10u).",
                 __FUNCTION__, __LINE__,
                now.tv_sec, now.tv_usec,
                tv_set.tv_sec, tv_set.tv_usec,
                reftime.coarse, reftime.fine,
                orgtime.coarse, orgtime.fine,
                rectime.coarse, rectime.fine,
                xmttime.coarse, xmttime.fine,
                arrival->coarse, arrival->fine);
            if (tv_set.tv_sec + 3600 < now.tv_sec) {
                sprintf(cmd, "reboot \"System clock is too far ahead of NTP sync result: %lu.%.6lu vs %lu.%.6lu\"",
                    now.tv_sec, now.tv_usec,
                    tv_set.tv_sec, tv_set.tv_usec);
                system(cmd);
            }
        }
#endif
		if (settimeofday(&tv_set,NULL)<0) {
			perror("settimeofday");
			exit(1);
		}
		if (debug) {
			printf("set time to %lu.%.6lu\n", tv_set.tv_sec, tv_set.tv_usec);
		}

#ifdef __FAT_AP__
        fp = fopen(NTP_IPC_FILE, "w+");
        if (!fp) {
            syslog(LOG_WARNING, "ntpclient(%s:%d): Failed to create the ipc file \"ntp.ipc\".", __FUNCTION__, __LINE__);                
        } else {
            fclose(fp);
        }
#endif
	}

	if (debug) {
	printf("LI=%d  VN=%d  Mode=%d  Stratum=%d  Poll=%d  Precision=%d\n",
		li, vn, mode, stratum, poll, prec);
	printf("Delay=%.1f  Dispersion=%.1f  Refid=%u.%u.%u.%u\n",
		sec2u(delay),sec2u(disp),
		refid>>24&0xff, refid>>16&0xff, refid>>8&0xff, refid&0xff);
	printf("Reference %u.%.10u\n", reftime.coarse, reftime.fine);
	printf("Originate %u.%.10u\n", orgtime.coarse, orgtime.fine);
	printf("Receive   %u.%.10u\n", rectime.coarse, rectime.fine);
	printf("Transmit  %u.%.10u\n", xmttime.coarse, xmttime.fine);
	printf("Our recv  %u.%.10u\n", arrival->coarse, arrival->fine);
	}
	el_time=ntpdiff(&orgtime,arrival);   /* elapsed */
	st_time=ntpdiff(&rectime,&xmttime);  /* stall */
	skew1=ntpdiff(&orgtime,&rectime);
	skew2=ntpdiff(&xmttime,arrival);
	freq=get_current_freq();
	if (debug) {
	printf("Total elapsed: %9.2f\n"
	       "Server stall:  %9.2f\n"
	       "Slop:          %9.2f\n",
		el_time, st_time, el_time-st_time);
	printf("Skew:          %9.2f\n"
	       "Frequency:     %9d\n"
	       " day   second     elapsed    stall     skew  dispersion  freq\n",
		(skew1-skew2)/2, freq);
	}
	/* Not the ideal order for printing, but we want to be sure
	 * to do all the time-sensitive thinking (and time setting)
	 * before we start the output, especially fflush() (which
	 * could be slow).  Of course, if debug is turned on, speed
	 * has gone down the drain anyway. */
	if (live) {
		int new_freq;
		new_freq = contemplate_data(arrival->coarse, (skew1-skew2)/2,
			el_time+sec2u(disp), freq);
		if (!debug && new_freq != freq) set_freq(new_freq);
	}
	printf("%d %.5d.%.3d  %8.1f %8.1f  %8.1f %8.1f %9d\n",
		arrival->coarse/86400, arrival->coarse%86400,
		arrival->fine/4294967, el_time, st_time,
		(skew1-skew2)/2, sec2u(disp), freq);
	fflush(stdout);
	return(el_time-st_time);
}

#ifdef __FAT_AP__

#define NTP_CONFIG_FILE "/etc/ntpclient.conf"
#define DEFAULT_NTP_SERVER "pool.ntp.org"
static char * get_ntp_server_from_config_file()
{
    FILE *f = NULL;
    static char buf[256] = {0};
    int i = 0;
    f = fopen(NTP_CONFIG_FILE,"r");

    if (!f) {
        return NULL;
    }

    if (fgets(buf,sizeof(buf),f) == 0) {
        fclose(f);
        return NULL;
    }

    /*Check if the end of the string is \n */
    i = strlen(buf);
    if ( i != 0 && buf[i-1] == '\n' ) {
        buf[i-1] = 0;
    }
    fclose(f);
    return buf;
}
void stuff_net_addr(struct in6_addr *p, char *hostname)
{
    struct hostent *ntpserver = NULL;
    char str[INET6_ADDRSTRLEN];
    while (1) {
        ntpserver=aruba_gethostbyname_with_best_effort(hostname);
        if (ntpserver)
            break;

        herror(hostname);
        sleep (60);
    }
    if (ntpserver->h_addrtype == AF_INET) {
        unsigned int ipv4_ip = *(unsigned int *) (ntpserver->h_addr);
        p->s6_addr16[5] = 0xffff;
        p->s6_addr32[3] = ipv4_ip;
    } else {
        memcpy(&(p->s6_addr),ntpserver->h_addr,sizeof(p->s6_addr));
    }
    if (inet_ntop(AF_INET6, p, str, INET6_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "using server \"%s\", hostname \"%s\", "
            "\n",str, hostname);
}

void setup_receive(int usd, struct in6_addr interface, short port)
{
	struct sockaddr_in6 sa_rcvr;
	bzero((char *) &sa_rcvr, sizeof(sa_rcvr));
	sa_rcvr.sin6_family=AF_INET6;
	memcpy(&sa_rcvr.sin6_addr, &interface, sizeof(sa_rcvr.sin6_addr));
	sa_rcvr.sin6_port=htons(port);
	if(bind(usd,(struct sockaddr *) &sa_rcvr,sizeof(sa_rcvr)) == -1) {
		fprintf(stderr,"could not bind to udp port %d\n",port);
		perror("bind");
		exit(1);
	}
	listen(usd,3);
}
#else
void stuff_net_addr(struct in_addr *p, char *hostname)
{
    struct hostent *ntpserver = NULL;
    ntpserver=gethostbyname(hostname);
    if (ntpserver == NULL) {
            herror(hostname);
            exit(1);
    }
    if (ntpserver->h_length != 4) {
            fprintf(stderr,"oops %d\n",ntpserver->h_length);
            exit(1);
    }
    memcpy(&(p->s_addr),ntpserver->h_addr_list[0],4);
}

void setup_receive(int usd, unsigned int interface, short port)
{
	struct sockaddr_in sa_rcvr;
	bzero((char *) &sa_rcvr, sizeof(sa_rcvr));
	sa_rcvr.sin_family=AF_INET;
	sa_rcvr.sin_addr.s_addr=htonl(interface);
	sa_rcvr.sin_port=htons(port);
	if(bind(usd,(struct sockaddr *) &sa_rcvr,sizeof(sa_rcvr)) == -1) {
		fprintf(stderr,"could not bind to udp port %d\n",port);
		perror("bind");
		exit(1);
	}
	listen(usd,3);
}
#endif


void setup_transmit(int usd, char *host, short port)
{
	bzero((char *) &ntp_server_dest, sizeof(ntp_server_dest));
#ifdef __FAT_AP__
	ntp_server_dest.sin6_family=AF_INET6;
	stuff_net_addr(&(ntp_server_dest.sin6_addr),host);
	ntp_server_dest.sin6_port=htons(port);
#else
	ntp_server_dest.sin_family=AF_INET;
	stuff_net_addr(&(ntp_server_dest.sin_addr),host);
	ntp_server_dest.sin_port=htons(port);
#endif
#if 0
	if (connect(usd,(struct sockaddr *)&ntp_server_dest,sizeof(ntp_server_dest))==-1)
		{perror("connect");exit(1);}
#endif
}

void primary_loop(int usd, int num_probes, int interval, int goodness)
{
	fd_set fds;
#ifdef __FAT_AP__
    char str[INET6_ADDRSTRLEN];
	struct sockaddr_in6 sa_xmit;
#else
	struct sockaddr sa_xmit;
#endif
	int i, pack_len, sa_xmit_len, probes_sent, error;
	struct timeval to;
	struct ntptime udp_arrival_ntp;
#ifdef __FAT_AP__
    int timeout = 1;
#endif

	if (debug) printf("Listening...\n");

	probes_sent=0;
	sa_xmit_len=sizeof(sa_xmit);
	to.tv_sec=0;
	to.tv_usec=0;
	for (;;) {
		FD_ZERO(&fds);
		FD_SET(usd,&fds);
		i=select(usd+1,&fds,NULL,NULL,&to);  /* Wait on read or error */
		if ((i!=1)||(!FD_ISSET(usd,&fds))) {
			if (i==EINTR) continue;
			if (i<0) perror("select");
			if (to.tv_sec == 0) {
				if (probes_sent >= num_probes &&
				    num_probes != 0) {
				  break;
				}
				send_packet(usd);
				++probes_sent;
#ifdef __FAT_AP__
				to.tv_sec=timeout;
                if (timeout <= interval / 8) {
                    timeout <<= 1;
                }
#else
				to.tv_sec=interval;
#endif
				to.tv_usec=0;
			}	
			continue;
		}
#ifdef __FAT_AP__
		pack_len=recvfrom(usd,incoming,sizeof_incoming,0,
		                  (struct sockaddr *)&sa_xmit,&sa_xmit_len);
#else
		pack_len=recvfrom(usd,incoming,sizeof_incoming,0,
		                  &sa_xmit,&sa_xmit_len);
#endif
		error = goodness+1;
		if (pack_len<0) {
			perror("recvfrom");
		} else if (pack_len>0 && (unsigned)pack_len<sizeof_incoming){
			get_packet_timestamp(usd, &udp_arrival_ntp);
#ifdef __FAT_AP__
			check_source(pack_len, (struct sockaddr *)&sa_xmit, sa_xmit_len);
            if (inet_ntop(AF_INET6, &(sa_xmit.sin6_addr), str, INET6_ADDRSTRLEN) == NULL) {
                perror("inet_ntop");
                exit(EXIT_FAILURE);
            }
            printf("packet of len %u received from server %s\n",pack_len, str);
            if (!memcmp(&(sa_xmit.sin6_addr), &(ntp_server_dest.sin6_addr), sizeof(struct in6_addr)))
                error = rfc1305print(incoming_word, &udp_arrival_ntp);
            /* error = rfc1305print(incoming_word, &udp_arrival_ntp); */
#else
			check_source(pack_len, &sa_xmit, sa_xmit_len);
            error = rfc1305print(incoming_word, &udp_arrival_ntp);
#endif
			/* udp_handle(usd,incoming,pack_len,&sa_xmit,sa_xmit_len); */
		} else {
			printf("Ooops.  pack_len=%d\n",pack_len);
			fflush(stdout);
		}
		if ( error < goodness && goodness != 0) {
            break;
        }
		if (probes_sent >= num_probes && num_probes != 0) break;
#ifdef __FAT_AP__
        /* Regular interval is as configured */
		to.tv_sec=interval;
        to.tv_usec=0;
        timeout = 1;
#endif
	}
}

void do_replay(void)
{
	char line[100];
	int n, day, freq, absolute;
	float sec, el_time, st_time, disp;
	double skew, errorbar;
	int simulated_freq = 0;
	unsigned int last_fake_time = 0;
	double fake_delta_time = 0.0;

	while (fgets(line,sizeof(line),stdin)) {
		n=sscanf(line,"%d %f %f %f %lf %f %d",
			&day, &sec, &el_time, &st_time, &skew, &disp, &freq);
		if (n==7) {
			fputs(line,stdout);
			absolute=day*86400+(int)sec;
			errorbar=el_time+disp;
			if (debug) printf("contemplate %u %.1f %.1f %d\n",
				absolute,skew,errorbar,freq);
			if (last_fake_time==0) simulated_freq=freq;
			fake_delta_time += (absolute-last_fake_time)*((double)(freq-simulated_freq))/65536;
			if (debug) printf("fake %f %d \n", fake_delta_time, simulated_freq);
			skew += fake_delta_time;
			freq = simulated_freq;
			last_fake_time=absolute;
			simulated_freq = contemplate_data(absolute, skew, errorbar, freq);
		} else {
			fprintf(stderr,"Replay input error\n");
			exit(2);
		}
	}
}

void usage(char *argv0)
{
	fprintf(stderr,
	"Usage: %s [-c count] [-d] [-g goodness] -h hostname [-i interval]\n"
	"\t[-l] [-p port] [-r] [-s] \n",
	argv0);
}

int main(int argc, char *argv[]) {
	int usd;  /* socket */
	int c;
	/* These parameters are settable from the command line
	   the initializations here provide default behavior */
	short int udp_local_port=0;   /* default of 0 means kernel chooses */
	int cycle_time=600;           /* seconds */
	int probe_count=0;            /* default of 0 means loop forever */
	/* int debug=0; is a global above */
	int goodness=0;
#ifndef __FAT_AP__
	char *hostname=NULL;          /* must be set */
#endif
	int replay=0;                 /* replay mode overrides everything */

	for (;;) {
		c = getopt( argc, argv, "c:" DEBUG_OPTION "g:h:i:lp:rs");
		if (c == EOF) break;
		switch (c) {
			case 'c':
				probe_count = atoi(optarg);
				break;
#ifdef ENABLE_DEBUG
			case 'd':
				++debug;
				break;
#endif
			case 'g':
				goodness = atoi(optarg);
				break;
			case 'h':
				hostname = optarg;
				break;
			case 'i':
				cycle_time = atoi(optarg);
				break;
			case 'l':
				live++;
				break;
			case 'p':
				udp_local_port = atoi(optarg);
				break;
			case 'r':
				replay++;
				break;
			case 's':
				set_clock++;
#ifndef __FAT_AP__
				probe_count = 1;
#endif
				break;
			default:
				usage(argv[0]);
				exit(1);
		}
	}
	if (replay) {
		do_replay();
		exit(0);
	}
	if (hostname == NULL) {
#ifdef __FAT_AP__
        /*if hostname is NULL, try to get hostname from configuration file */
		hostname = get_ntp_server_from_config_file();
        if(hostname == NULL) {
            hostname = DEFAULT_NTP_SERVER;
        }
#else
		usage(argv[0]);
		exit(1);
#endif
	}
	if (debug) {
		printf("Configuration:\n"
		"  -c probe_count %d\n"
		"  -d (debug)     %d\n"
		"  -g goodness    %d\n"
		"  -h hostname    %s\n"
		"  -i interval    %d\n"
		"  -l live        %d\n"
		"  -p local_port  %d\n"
		"  -s set_clock   %d\n",
		probe_count, debug, goodness, hostname, cycle_time,
		live, udp_local_port, set_clock );
	}

#ifdef __FAT_AP__
	/* Startup sequence */
	if ((usd=socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP))==-1)
		{perror ("socket");exit(1);}
	setup_receive(usd, in6addr_any, udp_local_port);
#else
	/* Startup sequence */
	if ((usd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
		{perror ("socket");exit(1);}
	setup_receive(usd, INADDR_ANY, udp_local_port);
#endif


#ifndef __FAT_AP__
	setup_transmit(usd, hostname, NTP_PORT);
#endif

	primary_loop(usd, probe_count, cycle_time, goodness);

	close(usd);
	return 0;
}
