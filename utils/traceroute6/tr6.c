#include <sys/param.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <net/if.h>
#if __linux__
#include <endian.h>
#endif
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <linux/types.h>
#ifdef CAPABILITIES
#include <sys/capability.h>
#endif
#ifdef USE_IDN
#include <idna.h>
#include <locale.h>
#endif
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static char SNAPSHOT[] = "s20121221";

#ifndef SOL_IPV6
#define SOL_IPV6 IPPROTO_IPV6
#endif
#define MAXPACKET   65535
#define MAX_HOSTNAMELEN NI_MAXHOST
#ifndef FD_SET
#define NFDBITS         (8*sizeof(fd_set))
#define FD_SETSIZE      NFDBITS
#define FD_SET(n, p)    ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)    ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p)  ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)      memset((char *)(p), 0, sizeof(*(p)))
#endif
#define Fprintf (void)fprintf
#define Printf (void)printf
u_char  packet[512];        /* last inbound (icmp) packet */
int wait_for_reply(int, struct sockaddr_in6 *, struct in6_addr *, int);
int packet_ok(u_char *buf, int cc, struct sockaddr_in6 *from,
          struct in6_addr *to, int seq, struct timeval *);
void    send_probe(int seq, int ttl);
double  deltaT (struct timeval *, struct timeval *);
void    print(unsigned char *buf, int cc, struct sockaddr_in6 *from);
void    tvsub (struct timeval *, struct timeval *);
void    usage(void);
int icmp_sock;          /* receive (icmp) socket file descriptor */
int sndsock;            /* send (udp) socket file descriptor */
struct timezone tz;     /* leftover */
struct sockaddr_in6 whereto;    /* Who to try to reach */
struct sockaddr_in6 saddr;
struct sockaddr_in6 firsthop;
char *source = NULL;
char *device = NULL;
char *hostname;
int nprobes = 3;
int max_ttl = 30;
pid_t ident;
u_short port = 32768+666;   /* start udp dest port # for probe packets */
int options;            /* socket options */
int verbose;
int waittime = 5;       /* time to wait for response (in seconds) */
int nflag;          /* print addresses numerically */
struct pkt_format
{
    __u32 ident;
    __u32 seq;
    struct timeval tv;
};
char *sendbuff;
int datalen = sizeof(struct pkt_format);
int main(int argc, char *argv[])
{
    char pa[MAX_HOSTNAMELEN];
    extern char *optarg;
    extern int optind;
    struct hostent *hp;
    struct sockaddr_in6 from, *to;
    int ch, i, on, probe, seq, tos, ttl;
    int socket_errno;
    icmp_sock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
    socket_errno = errno;
    if (setuid(getuid())) {
        perror("traceroute6: setuid");
        exit(-1);
    }
#ifdef CAPABILITIES
    {
        cap_t caps = cap_init();
        if (cap_set_proc(caps)) {
            perror("traceroute6: cap_set_proc");
            exit(-1);
        }
        cap_free(caps);
    }
#endif
#ifdef USE_IDN
    setlocale(LC_ALL, "");
#endif
    on = 1;
    seq = tos = 0;
    to = (struct sockaddr_in6 *)&whereto;
    while ((ch = getopt(argc, argv, "dm:np:q:rs:t:w:vi:g:V")) != EOF) {
        switch(ch) {
        case 'd':
            options |= SO_DEBUG;
            break;
        case 'm':
            max_ttl = atoi(optarg);
            if (max_ttl <= 1) {
                Fprintf(stderr,
                    "traceroute: max ttl must be >1.\n");
                exit(1);
            }
            break;
        case 'n':
            nflag++;
            break;
        case 'p':
            port = atoi(optarg);
            if (port < 1) {
                Fprintf(stderr,
                    "traceroute: port must be >0.\n");
                exit(1);
            }
            break;
        case 'q':
            nprobes = atoi(optarg);
            if (nprobes < 1) {
                Fprintf(stderr,
                    "traceroute: nprobes must be >0.\n");
                exit(1);
            }
            break;
        case 'r':
            options |= SO_DONTROUTE;
            break;
        case 's':
            /*
             * set the ip source address of the outbound
             * probe (e.g., on a multi-homed host).
             */
            source = optarg;
            break;
        case 'i':
            device = optarg;
            break;
        case 'g':
            Fprintf(stderr, "Sorry, rthdr is not yet supported\n");
            break;
        case 'v':
            verbose++;
            break;
        case 'w':
            waittime = atoi(optarg);
            if (waittime <= 1) {
                Fprintf(stderr,
                    "traceroute: wait must be >1 sec.\n");
                exit(1);
            }
            break;
        case 'V':
            printf("traceroute6 utility, iputils-%s\n", SNAPSHOT);
            exit(0);
        default:
            usage();
        }
    }
    argc -= optind;
    argv += optind;
    if (argc < 1)
        usage();
    setlinebuf (stdout);
    (void) memset((char *)&whereto, 0, sizeof(whereto));
    to->sin6_family = AF_INET6;
    to->sin6_port = htons(port);
    if (inet_pton(AF_INET6, *argv, &to->sin6_addr) > 0) {
        hostname = *argv;
    } else {
        char *idn = NULL;
#ifdef USE_IDN
        if (idna_to_ascii_lz(*argv, &idn, 0) != IDNA_SUCCESS)
            idn = NULL;
#endif
        hp = gethostbyname2(idn ? idn : *argv, AF_INET6);
        if (hp) {
            memmove((caddr_t)&to->sin6_addr, hp->h_addr, sizeof(to->sin6_addr));
            hostname = (char *)hp->h_name;
        } else {
            (void)fprintf(stderr,
                "traceroute: unknown host %s\n", *argv);
            exit(1);
        }
    }
    firsthop = *to;
    if (*++argv) {
        datalen = atoi(*argv);
        /* Message for rpm maintainers: have _shame_. If you want
         * to fix something send the patch to me for sanity checking.
         * "datalen" patch is a shit. */
        if (datalen == 0)
            datalen = sizeof(struct pkt_format);
        else if (datalen < (int)sizeof(struct pkt_format) ||
             datalen >= MAXPACKET) {
            Fprintf(stderr,
                "traceroute: packet size must be %d <= s < %d.\n",
                (int)sizeof(struct pkt_format), MAXPACKET);
            exit(1);
        }
    }
    ident = getpid();
    sendbuff = malloc(datalen);
    if (sendbuff == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    if (icmp_sock < 0) {
        errno = socket_errno;
        perror("traceroute6: icmp socket");
        exit(1);
    }
#ifdef IPV6_RECVPKTINFO
    setsockopt(icmp_sock, SOL_IPV6, IPV6_RECVPKTINFO, &on, sizeof(on));
    setsockopt(icmp_sock, SOL_IPV6, IPV6_2292PKTINFO, &on, sizeof(on));
#else
    setsockopt(icmp_sock, SOL_IPV6, IPV6_PKTINFO, &on, sizeof(on));
#endif
    if (options & SO_DEBUG)
        setsockopt(icmp_sock, SOL_SOCKET, SO_DEBUG,
               (char *)&on, sizeof(on));
    if (options & SO_DONTROUTE)
        setsockopt(icmp_sock, SOL_SOCKET, SO_DONTROUTE,
               (char *)&on, sizeof(on));
#ifdef __linux__
    on = 2;
    if (setsockopt(icmp_sock, SOL_RAW, IPV6_CHECKSUM, &on, sizeof(on)) < 0) {
        /* checksum should be enabled by default and setting this
         * option might fail anyway.
         */
        fprintf(stderr, "setsockopt(RAW_CHECKSUM) failed - try to continue.");
    }
#endif
    if ((sndsock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        perror("traceroute: UDP socket");
        exit(5);
    }
#ifdef SO_SNDBUF
    if (setsockopt(sndsock, SOL_SOCKET, SO_SNDBUF, (char *)&datalen,
               sizeof(datalen)) < 0) {
        perror("traceroute: SO_SNDBUF");
        exit(6);
    }
#endif /* SO_SNDBUF */
    if (options & SO_DEBUG)
        (void) setsockopt(sndsock, SOL_SOCKET, SO_DEBUG,
                  (char *)&on, sizeof(on));
    if (options & SO_DONTROUTE)
        (void) setsockopt(sndsock, SOL_SOCKET, SO_DONTROUTE,
                  (char *)&on, sizeof(on));
    if (source == NULL) {
        socklen_t alen;
        int probe_fd = socket(AF_INET6, SOCK_DGRAM, 0);
        if (probe_fd < 0) {
            perror("socket");
            exit(1);
        }
        if (device) {
            if (setsockopt(probe_fd, SOL_SOCKET, SO_BINDTODEVICE, device, strlen(device)+1) == -1)
                perror("WARNING: interface is ignored");
        }
        firsthop.sin6_port = htons(1025);
        if (connect(probe_fd, (struct sockaddr*)&firsthop, sizeof(firsthop)) == -1) {
            perror("connect");
            exit(1);
        }
        alen = sizeof(saddr);
        if (getsockname(probe_fd, (struct sockaddr*)&saddr, &alen) == -1) {
            perror("getsockname");
            exit(1);
        }
        saddr.sin6_port = 0;
        close(probe_fd);
    } else {
        (void) memset((char *)&saddr, 0, sizeof(saddr));
        saddr.sin6_family = AF_INET6;
        if (inet_pton(AF_INET6, source, &saddr.sin6_addr) <= 0)
        {
            Printf("traceroute: unknown addr %s\n", source);
            exit(1);
        }
    }
    if (bind(sndsock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror ("traceroute: bind sending socket");
        exit (1);
    }
    if (bind(icmp_sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror ("traceroute: bind icmp6 socket");
        exit (1);
    }
    Fprintf(stderr, "traceroute to %s (%s)", hostname,
        inet_ntop(AF_INET6, &to->sin6_addr, pa, sizeof(pa)));
    Fprintf(stderr, " from %s",
        inet_ntop(AF_INET6, &saddr.sin6_addr, pa, sizeof(pa)));
    Fprintf(stderr, ", %d hops max, %d byte packets\n", max_ttl, datalen);
    (void) fflush(stderr);
    for (ttl = 1; ttl <= max_ttl; ++ttl) {
        struct in6_addr lastaddr = {{{0,}}};
        int got_there = 0;
        int unreachable = 0;
        Printf("%2d ", ttl);
        for (probe = 0; probe < nprobes; ++probe) {
            int cc, reset_timer;
            struct timeval t1, t2;
            struct timezone tz;
            struct in6_addr to;
            gettimeofday(&t1, &tz);
            send_probe(++seq, ttl);
            reset_timer = 1;
            while ((cc = wait_for_reply(icmp_sock, &from, &to, reset_timer)) != 0) {
                gettimeofday(&t2, &tz);
                if ((i = packet_ok(packet, cc, &from, &to, seq, &t1))) {
                    reset_timer = 1;
                    if (memcmp(&from.sin6_addr, &lastaddr, sizeof(from.sin6_addr))) {
                        print(packet, cc, &from);
                        memcpy(&lastaddr,
                               &from.sin6_addr,
                               sizeof(lastaddr));
                    }
                    Printf("  %g ms", deltaT(&t1, &t2));
                    switch(i - 1) {
                    case ICMP6_DST_UNREACH_NOPORT:
                        ++got_there;
                        break;
                    case ICMP6_DST_UNREACH_NOROUTE:
                        ++unreachable;
                        Printf(" !N");
                        break;
                    case ICMP6_DST_UNREACH_ADDR:
                        ++unreachable;
                        Printf(" !H");
                        break;
                    case ICMP6_DST_UNREACH_ADMIN:
                        ++unreachable;
                        Printf(" !S");
                        break;
                    }
                    break;
                } else
                    reset_timer = 0;
            }
            if (cc <= 0)
                Printf(" *");
            (void) fflush(stdout);
        }
        putchar('\n');
        if (got_there ||
            (unreachable > 0 && unreachable >= nprobes-1))
            exit(0);
    }
    return 0;
}
int
wait_for_reply(sock, from, to, reset_timer)
    int sock;
    struct sockaddr_in6 *from;
    struct in6_addr *to;
    int reset_timer;
{
    fd_set fds;
    static struct timeval wait;
    int cc = 0;
    char cbuf[512];
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    if (reset_timer) {
        /*
         * traceroute could hang if someone else has a ping
         * running and our ICMP reply gets dropped but we don't
         * realize it because we keep waking up to handle those
         * other ICMP packets that keep coming in.  To fix this,
         * "reset_timer" will only be true if the last packet that
         * came in was for us or if this is the first time we're
         * waiting for a reply since sending out a probe.  Note
         * that this takes advantage of the select() feature on
         * Linux where the remaining timeout is written to the
         * struct timeval area.
         */
        wait.tv_sec = waittime;
        wait.tv_usec = 0;
    }
    if (select(sock+1, &fds, (fd_set *)0, (fd_set *)0, &wait) > 0) {
        struct iovec iov;
        struct msghdr msg;
        iov.iov_base = packet;
        iov.iov_len = sizeof(packet);
        msg.msg_name = (void *)from;
        msg.msg_namelen = sizeof(*from);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_flags = 0;
        msg.msg_control = cbuf;
        msg.msg_controllen = sizeof(cbuf);
        cc = recvmsg(icmp_sock, &msg, 0);
        if (cc >= 0) {
            struct cmsghdr *cmsg;
            struct in6_pktinfo *ipi;
            for (cmsg = CMSG_FIRSTHDR(&msg);
                 cmsg;
                 cmsg = CMSG_NXTHDR(&msg, cmsg)) {
                if (cmsg->cmsg_level != SOL_IPV6)
                    continue;
                switch (cmsg->cmsg_type) {
                case IPV6_PKTINFO:
#ifdef IPV6_2292PKTINFO
                case IPV6_2292PKTINFO:
#endif
                    ipi = (struct in6_pktinfo *)CMSG_DATA(cmsg);
                    memcpy(to, ipi, sizeof(*to));
                }
            }
        }
    }
    return(cc);
}
void send_probe(int seq, int ttl)
{
    struct pkt_format *pkt = (struct pkt_format *) sendbuff;
    int i;
    pkt->ident = htonl(ident);
    pkt->seq = htonl(seq);
    gettimeofday(&pkt->tv, &tz);
    i = setsockopt(sndsock, SOL_IPV6, IPV6_UNICAST_HOPS, &ttl, sizeof(ttl));
    if (i < 0)
    {
        perror("setsockopt");
        exit(1);
    }
    do {
        i = sendto(sndsock, sendbuff, datalen, 0,
               (struct sockaddr *)&whereto, sizeof(whereto));
    } while (i<0 && errno == ECONNREFUSED);
    if (i < 0 || i != datalen)  {
        if (i<0)
            perror("sendto");
        Printf("traceroute: wrote %s %d chars, ret=%d\n", hostname,
            datalen, i);
        (void) fflush(stdout);
    }
}
double deltaT(struct timeval *t1p, struct timeval *t2p)
{
    register double dt;
    dt = (double)(t2p->tv_sec - t1p->tv_sec) * 1000.0 +
         (double)(t2p->tv_usec - t1p->tv_usec) / 1000.0;
    return (dt);
}
/*
 * Convert an ICMP "type" field to a printable string.
 */
char * pr_type(unsigned char t)
{
    switch(t) {
    /* Unknown */
    case 0:
        return "Error";
    case 1:
        /* ICMP6_DST_UNREACH: */
        return "Destination Unreachable";
    case 2:
        /* ICMP6_PACKET_TOO_BIG: */
        return "Packet Too Big";
    case 3:
        /* ICMP6_TIME_EXCEEDED */
        return "Time Exceeded in Transit";
    case 4:
        /* ICMP6_PARAM_PROB */
        return "Parameter Problem";
    case 128:
        /* ICMP6_ECHO_REQUEST */
        return "Echo Request";
    case 129:
        /* ICMP6_ECHO_REPLY */
        return "Echo Reply";
    case 130:
        /* ICMP6_MEMBERSHIP_QUERY */
        return "Membership Query";
    case 131:
        /* ICMP6_MEMBERSHIP_REPORT */
        return "Membership Report";
    case 132:
        /* ICMP6_MEMBERSHIP_REDUCTION */
        return "Membership Reduction";
    case 133:
        /* ND_ROUTER_SOLICIT */
        return "Router Solicitation";
    case 134:
        /* ND_ROUTER_ADVERT */
        return "Router Advertisement";
    case 135:
        /* ND_NEIGHBOR_SOLICIT */
        return "Neighbor Solicitation";
    case 136:
        /* ND_NEIGHBOR_ADVERT */
        return "Neighbor Advertisement";
    case 137:
        /* ND_REDIRECT */
        return "Redirect";
    }
    return("OUT-OF-RANGE");
}
int packet_ok(u_char *buf, int cc, struct sockaddr_in6 *from,
          struct in6_addr *to, int seq,
          struct timeval *tv)
{
    struct icmp6_hdr *icp;
    u_char type, code;
    icp = (struct icmp6_hdr *) buf;
    type = icp->icmp6_type;
    code = icp->icmp6_code;
    if ((type == ICMP6_TIME_EXCEEDED && code == ICMP6_TIME_EXCEED_TRANSIT) ||
        type == ICMP6_DST_UNREACH)
    {
        struct ip6_hdr *hip;
        struct udphdr *up;
        int nexthdr;
        hip = (struct ip6_hdr *) (icp + 1);
        up = (struct udphdr *)(hip+1);
        nexthdr = hip->ip6_nxt;
        if (nexthdr == 44) {
            nexthdr = *(unsigned char*)up;
            up++;
        }
        if (nexthdr == IPPROTO_UDP)
        {
            struct pkt_format *pkt;
            pkt = (struct pkt_format *) (up + 1);
            if (ntohl(pkt->ident) == ident &&
                ntohl(pkt->seq) == seq)
            {
                *tv = pkt->tv;
                return (type == ICMP6_TIME_EXCEEDED ? -1 : code+1);
            }
        }
    }
    if (verbose) {
        unsigned char *p;
        char pa1[MAX_HOSTNAMELEN];
        char pa2[MAX_HOSTNAMELEN];
        int i;
        p = (unsigned char *) (icp + 1);
        Printf("\n%d bytes from %s to %s", cc,
               inet_ntop(AF_INET6, &from->sin6_addr, pa1, sizeof(pa1)),
               inet_ntop(AF_INET6, to, pa2, sizeof(pa2)));
        Printf(": icmp type %d (%s) code %d\n", type, pr_type(type),
               icp->icmp6_code);
        cc -= sizeof(struct icmp6_hdr);
        for (i = 0; i < cc ; i++) {
            if (i % 16 == 0)
                Printf("%04x:", i);
            if (i % 4 == 0)
                Printf(" ");
            Printf("%02x", 0xff & (unsigned)p[i]);
            if (i % 16 == 15 && i + 1 < cc)
                Printf("\n");
        }
        Printf("\n");
    }
    return(0);
}
void print(unsigned char *buf, int cc, struct sockaddr_in6 *from)
{
    char pa[MAX_HOSTNAMELEN];
    if (nflag)
        Printf(" %s", inet_ntop(AF_INET6, &from->sin6_addr,
                    pa, sizeof(pa)));
    else
    {
        const char *hostname;
        struct hostent *hp;
        char *s = NULL;
        hostname = inet_ntop(AF_INET6, &from->sin6_addr, pa, sizeof(pa));
        if ((hp = gethostbyaddr((char *)&from->sin6_addr,
                    sizeof(from->sin6_addr), AF_INET6))) {
#ifdef USE_IDN
            if (idna_to_unicode_lzlz(hp->h_name, &s, 0) != IDNA_SUCCESS)
                s = NULL;
#endif
        }
        Printf(" %s (%s)", hp ? (s ? s : hp->h_name) : hostname, pa);
        free(s);
    }
}
/*
 * Subtract 2 timeval structs:  out = out - in.
 * Out is assumed to be >= in.
 */
void
tvsub(out, in)
    register struct timeval *out, *in;
{
    if ((out->tv_usec -= in->tv_usec) < 0)   {
        out->tv_sec--;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}
void usage(void)
{
    fprintf(stderr,
"Usage: traceroute6 [-dnrvV] [-m max_ttl] [-p port#] [-q nqueries]\n\t\
[-s src_addr] [-t tos] [-w wait] host [data size]\n");
    exit(1);
}
