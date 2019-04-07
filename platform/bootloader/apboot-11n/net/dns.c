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

#include <common.h>
#ifdef CONFIG_APBOOT
#include <command.h>
#include "net.h"
#include "bootp.h"
#include "tftp.h"
#include "dns.h"

extern void ArpRequest(void);

#if (CONFIG_COMMANDS & CFG_CMD_NET)
#ifdef CONFIG_APBOOT
#define DEBUG
int dns_debug = 0;
#endif

#define TIMEOUT		2		/* Seconds before trying DNS again */
#define TIMEOUT_COUNT	4		/* # of timeouts before giving up  */

#define DNS_HDR_SIZE   16

#define RESOLV
#define PROM
#ifdef RESOLV
#include "nameser.h"
#include "resolv.h"
#endif

#ifdef PROM
#define	MAXALIASES	2
#define	MAXADDRS	2
#else
#define	MAXALIASES	35
#define	MAXADDRS	35
#endif

static int querylen = 0;

#define	MAXPACKET	1024

static char *h_addr_ptrs[MAXADDRS + 1];

static void DnsHandler(uchar *pkt, unsigned dest, unsigned src, unsigned len);
static void DnsTimeout(void);

int total_dns_tries = 0;
int	DnsTry = 0;
int	DnsRetry = 0;
int     DnsLen;

typedef union {
    HEADER hdr;
    u_char buf[MAXPACKET];
} querybuf;

typedef union {
    long al;
    char ac;
} align;

/*
 * Skip over a compressed domain name. Return the size or -1.
 */
int
__dn_skipname(const u_char *comp_dn, const u_char *eom)
{
	register u_char *cp;
	register int n;

	cp = (u_char *)comp_dn;
	while (cp < eom && (n = *cp++)) {
		/*
		 * check for indirection
		 */
		switch (n & INDIR_MASK) {
		case 0:		/* normal case, n == len */
			cp += n;
			continue;
		default:	/* illegal type */
			return (-1);
		case INDIR_MASK:	/* indirection */
			cp++;
		}
		break;
	}
	return (cp - comp_dn);
}

/*
 * Routines to insert/extract short/long's. Must account for byte
 * order and non-alignment problems. This code at least has the
 * advantage of being portable.
 *
 * used by sendmail.
 */

u_short
_getshort(u_char *msgp)
{
	register u_char *p = (u_char *) msgp;
#ifdef vax
	/*
	 * vax compiler doesn't put shorts in registers
	 */
	register u_long u;
#else
	register u_short u;
#endif

	u = *p++ << 8;
	return ((u_short)(u | *p));
}
/*
 * Expand compressed domain name 'comp_dn' to full domain name.
 * 'msg' is a pointer to the begining of the message,
 * 'eomorig' points to the first location after the message,
 * 'exp_dn' is a pointer to a buffer of size 'length' for the result.
 * Return size of compressed name or -1 if there was an error.
 */
int
dn_expand(
	const u_char *msg, const u_char *eomorig, const u_char *comp_dn, 
	u_char *exp_dn, int length
)
{
	register u_char *cp, *dn;
	register int n, c;
	u_char *eom;
	int len = -1, checked = 0;

	dn = exp_dn;
	cp = (u_char *)comp_dn;
	eom = exp_dn + length;
	/*
	 * fetch next label in domain name
	 */
	while ((n = *cp++)) {
		/*
		 * Check for indirection
		 */
		switch (n & INDIR_MASK) {
		case 0:
			if (dn != exp_dn) {
				if (dn >= eom)
					return (-1);
				*dn++ = '.';
			}
			if (dn+n >= eom)
				return (-1);
			checked += n + 1;
			while (--n >= 0) {
				if ((c = *cp++) == '.') {
					if (dn + n + 2 >= eom)
						return (-1);
					*dn++ = '\\';
				}
				*dn++ = c;
				if (cp >= eomorig)	/* out of range */
					return(-1);
			}
			break;

		case INDIR_MASK:
			if (len < 0)
				len = cp - comp_dn + 1;
			cp = (u_char *)msg + (((n & 0x3f) << 8) | (*cp & 0xff));
			if (cp < msg || cp >= eomorig)	/* out of range */
				return(-1);
			checked += 2;
			/*
			 * Check for loops in the compressed name;
			 * if we've looked at the whole message,
			 * there must be a loop.
			 */
			if (checked >= eomorig - msg)
				return (-1);
			break;

		default:
			return (-1);			/* flag error */
		}
	}
	*dn = '\0';
	if (len < 0)
		len = cp - comp_dn;
	return (len);
}
static char hostbuf[MAXPACKET + 1];
static char *host_aliases[MAXALIASES];

static IPaddr_t
getanswer(querybuf *answer, int anslen, int iquery)
{
	register HEADER *hp;
	register u_char *cp;
	register int n;
	u_char *eom;
	char *bp, **ap;
	int type, class, buflen, ancount, qdcount;
	int haveanswer, getclass = C_ANY;
	char **hap;
	IPaddr_t addr;

	eom = answer->buf + anslen;
	/*
	 * find first satisfactory answer
	 */
	hp = &answer->hdr;
	ancount = ntohs(hp->ancount);
	qdcount = ntohs(hp->qdcount);
	bp = hostbuf;
	buflen = sizeof(hostbuf);
	cp = answer->buf + sizeof(HEADER);
	if (dns_debug) {
		printf("qdcount %u, ancount %u, iquery %u\n", qdcount, ancount, iquery);
	}
	if (qdcount) {
		if (iquery) {
			if ((n = dn_expand((u_char *)answer->buf,
			    (u_char *)eom, (u_char *)cp, (u_char *)bp,
			    buflen)) < 0) {
//				h_errno = NO_RECOVERY;
				if (dns_debug) {
					printf("non-recoverable error\n");
				}
				return 0;
			}
			cp += n + QFIXEDSZ;
//			host.h_name = bp;
			n = strlen(bp) + 1;
			bp += n;
			buflen -= n;
		} else
			cp += __dn_skipname(cp, eom) + QFIXEDSZ;
		while (--qdcount > 0)
			cp += __dn_skipname(cp, eom) + QFIXEDSZ;
	} else if (iquery) {
		if (hp->aa) {
//			h_errno = HOST_NOT_FOUND;
			if (dns_debug) {
				printf("host not found (authoratative)\n");
			}
		} else {
//			h_errno = TRY_AGAIN;
			if (dns_debug) {
				printf("host not found (try again)\n");
			}
		}
		return 0;
	}
	ap = host_aliases;
	*ap = NULL;
//	host.h_aliases = host_aliases;
	hap = h_addr_ptrs;
	*hap = NULL;
#if 0 //BSD >= 43 || defined(h_addr)	/* new-style hostent structure */
	host.h_addr_list = h_addr_ptrs;
#endif
	haveanswer = 0;
	while (--ancount >= 0 && cp < eom) {
		if ((n = dn_expand((u_char *)answer->buf, (u_char *)eom,
		    (u_char *)cp, (u_char *)bp, buflen)) < 0)
			break;
		cp += n;
		type = _getshort(cp);
 		cp += sizeof(u_short);
		class = _getshort(cp);
 		cp += sizeof(u_short) + sizeof(u_long);
		n = _getshort(cp);
		cp += sizeof(u_short);
		if (dns_debug) {
			printf("type %u, class %u\n", type, class);
		}
		if (type == T_CNAME) {
			cp += n;
			if (ap >= &host_aliases[MAXALIASES-1])
				continue;
			*ap++ = bp;
			n = strlen(bp) + 1;
			bp += n;
			buflen -= n;
			continue;
		}
		if (iquery && type == T_PTR) {
			if ((n = dn_expand((u_char *)answer->buf,
			    (u_char *)eom, (u_char *)cp, (u_char *)bp,
			    buflen)) < 0) {
				cp += n;
				continue;
			}
			cp += n;
//			host.h_name = bp;
			if (dns_debug) {
				printf("unexpected iquery\n");
			}
			return 0;	// XXX should not happen
		}
		if (iquery || type != T_A)  {
#ifdef DEBUG
			if (dns_debug) {
				printf("unexpected answer type %d, size %d\n",
					type, n);
			}
#endif
			cp += n;
			continue;
		}
		if (haveanswer) {
			if (n != 4 /*host.h_length*/) {
				cp += n;
				continue;
			}
			if (class != getclass) {
				cp += n;
				continue;
			}
		} else {
//			host.h_length = n;
			getclass = class;
//			host.h_addrtype = (class == C_IN) ? AF_INET : AF_UNSPEC;
			if (!iquery) {
//				host.h_name = bp;
				bp += strlen(bp) + 1;
			}
		}

		bp += sizeof(align) - ((u_long)bp % sizeof(align));

		if (bp + n >= &hostbuf[sizeof(hostbuf)]) {
#ifdef DEBUG
			if (dns_debug) {
				printf("size (%d) too big\n", n);
			}
#endif
			break;
		}
		memcpy(*hap++ = bp, cp, n);
		bp +=n;
		cp += n;
		haveanswer++;
	}
	if (haveanswer) {
		*ap = NULL;
#if 0
#if BSD >= 43 || defined(h_addr)	/* new-style hostent structure */
		*hap = NULL;
#else
		host.h_addr = h_addr_ptrs[0];
#endif
#else
		addr = *(IPaddr_t *)h_addr_ptrs[0];
#endif
		return addr;
	} else {
		return 0;
	}
}

/*
 *	Handle a Dns received packet.
 */
static void
DnsHandler(uchar *pkt, unsigned dest, unsigned src, unsigned len)
{
    int namelen;
    int i;
    ushort *p;
#if 0 //def DEBUG
	ushort id;
#endif

    if (dest != 1025) {
    	if (dns_debug) {
            printf("dropped; destination port is incorrect: %u\n", dest);
        }
        return;
    }

#ifdef DEBUG
	if (dns_debug) {
		printf("DNS receive:");
		for (i = 0; i < len; i++) {
                        if (!(i & 0x7)) printf("\n");
			printf(" %02x", pkt[i]);
                }
		printf("\n");
	}
#endif
    namelen = querylen;
    if (len < DNS_HDR_SIZE + namelen + 16) {
    	if (dns_debug) {
                printf("dropped; length incorrect: %u\n", len);
        }
        return;
    }
    p = (ushort *)pkt;
    NetServerIP = getanswer((querybuf *)pkt, len, 0);
    if (NetServerIP == 0) {
        if (dns_debug) {
            printf("no address\n");
    	}
	return;
    }

    TftpStart();
}


/*
 *	Timeout on ARP request.
 */
static void
DnsTimeout(void)
{
	extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
	int try = DnsRetry % 5;

	if (total_dns_tries > 60) {
		puts ("\nToo many retries; resetting\n");
		do_reset(0, 0, 0, 0);
	} else if (try >= TIMEOUT_COUNT) {
		puts ("\nRetry count exceeded; starting again\n");
		NetStartAgain ();
	} else {
		puts ("T ");
		NetSetTimeout ((TIMEOUT << (try + 1)) * CFG_HZ, DnsTimeout);
		DnsSend ();
	}
}

static int DnsLength(char *str)
{
    int len = 0;
    while (*str && *str++ != '.') {
        len++;
    }
    return len;
}


char server_name_copy[64];
char domain_name_copy[255];

void
DnsStart (void)
{
	int len;
#ifdef DEBUG
	int i;
	uchar *opkt;
#endif
	uchar *pkt;
	ushort *p;
	char *name;
	char *cp;
	int trailingdot = 0;
	char abuf[20];

	memset(server_name_copy, 0, sizeof(server_name_copy));
	memset(domain_name_copy, 0, sizeof(domain_name_copy));

	strcpy(server_name_copy, NetServerName);
	strcpy(domain_name_copy, NetOurDomainName);

	DnsTry = 0;
	DnsRetry = 0;

	if (getenv("dns_debug")) {
		dns_debug = 1;
	}

	cp = server_name_copy + strlen(server_name_copy) - 1;
	if (*cp == '.') {
	    trailingdot = 1;
	}

	ip_to_string(NetOurDNSIP, abuf);
	printf("DNS request %d for %s%s%s to %s\n", ++DnsTry,
	       server_name_copy, 
	       trailingdot ? "" : ".",
	       trailingdot ? "" : domain_name_copy, abuf);
	total_dns_tries++;

	pkt = (uchar *)NetTxPacket + NetEthHdrSize() + IP_HDR_SIZE;
	len = strlen(server_name_copy);
	DnsLen = DNS_HDR_SIZE;

	p = (ushort *)pkt;
#ifdef DEBUG
	opkt = pkt;
#endif

	*p++ = htons(0x0000); /* id */
	*p++ = htons(0x0100); /* flags */
	*p++ = htons(0x0001); /* questions */
	*p++ = htons(0x0000); /* answers */
	*p++ = htons(0x0000); /* authority */
	*p++ = htons(0x0000); /* additional */
	pkt = (uchar *)p;

	name = server_name_copy;

	while ((len = DnsLength(name))) {
	    *pkt++  = len;         /* query length */
	    strncpy((char *)pkt, name, len);  /* name */
	    name += len + 1;                  /* skip '.' */
	    pkt += len;
	    DnsLen += len + 1;
	}

	querylen = strlen(server_name_copy) + !trailingdot;

	if (!trailingdot) {
	    name = domain_name_copy;
	    querylen += strlen(domain_name_copy) + 1;
	    while ((len = DnsLength(name))) {
		*pkt++  = len;         /* query length */
		strncpy((char *)pkt, name, len);  /* name */
		name += len + 1;                  /* skip '.' */
		pkt += len;
		DnsLen += len + 1;
	    }
	}
	*pkt++  = 0;             /* query length */
	DnsLen++;
	p = (ushort *)pkt;
	NetPutShort(p, 0x0001);	/* host address */
	p++;
	NetPutShort(p, 0x0001);	/* inet*/
	p++;

#ifdef DEBUG
	if (dns_debug) {
		printf("DNS send:");
		for (i = 0; i < DnsLen; i++) {
			if (!(i & 0x7)) printf("\n");
			printf(" %02x", opkt[i]);
		}
		printf("\n");
	}
#endif

	memset(NetServerEther, 0, 6);

	NetSetTimeout(TIMEOUT * CFG_HZ, DnsTimeout);
	NetSetHandler(DnsHandler);

	DnsSend();

}

void
DnsSend (void)
{
	uchar *pkt;

	pkt = (uchar *)NetTxPacket + NetEthHdrSize() + IP_HDR_SIZE;
	*(ushort *)pkt = htons(DnsRetry); /* id */

	NetSendUDPPacket(NetServerEther, NetOurDNSIP, 53, 1025, DnsLen);

	DnsRetry++;
}
#endif /* CFG_CMD_NET */
#endif
