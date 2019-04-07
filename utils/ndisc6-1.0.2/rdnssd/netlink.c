/**
 * netlink.c - NetLink source for ICMPv6 RDNSS
 */

/*************************************************************************
 *  Copyright © 2007 Pierre Ynard, Rémi Denis-Courmont.                  *
 *  This program is free software: you can redistribute and/or modify    *
 *  it under the terms of the GNU General Public License as published by *
 *  the Free Software Foundation, versions 2 or 3 of the license.        *
 *                                                                       *
 *  This program is distributed in the hope that it will be useful,      *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *  GNU General Public License for more details.                         *
 *                                                                       *
 *  You should have received a copy of the GNU General Public License    *
 *  along with this program. If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef __linux__

#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/icmp6.h>
#include <sys/utsname.h>
#include <linux/rtnetlink.h>
#include <arpa/inet.h>

#include "rdnssd.h"
#include "gettext.h"


#ifndef RTNLGRP_ND_USEROPT
#if 0
# warning You need to update your Linux kernel headers (>= 2.6.24)
#endif
/* Belongs in <linux/rtnetlink.h> */

struct nduseroptmsg
{
	unsigned char	nduseropt_family;
	unsigned char	nduseropt_pad1;
	unsigned short	nduseropt_opts_len; /* Total length of options */
	int		nduseropt_ifindex;
	__u8		nduseropt_icmp_type;
	__u8		nduseropt_icmp_code;
	unsigned short	nduseropt_pad2;
	unsigned int	nduseropt_pad3;
	/* Followed by one or more ND options */
};

# define RTNLGRP_ND_USEROPT 20

#define RTM_NEWNDUSEROPT 68

#endif

#ifdef __aruba_strverscmp__
#include <string.h>
#include <ctype.h>

/* states: S_N: normal, S_I: comparing integral part, S_F: comparing
           fractional parts, S_Z: idem but with leading Zeroes only */
#define S_N    0x0
#define S_I    0x4
#define S_F    0x8
#define S_Z    0xC

/* result_type: CMP: return diff; LEN: compare using len_diff/diff */
#define CMP    2
#define LEN    3


/* ISDIGIT differs from isdigit, as follows:
   - Its arg may be any int or unsigned int; it need not be an unsigned char.
   - It's guaranteed to evaluate its argument exactly once.
   - It's typically faster.
   POSIX says that only '0' through '9' are digits.  Prefer ISDIGIT to
   ISDIGIT_LOCALE unless it's important to use the locale's definition
   of `digit' even when the host does not conform to POSIX.  */
#define ISDIGIT(c) ((unsigned int) (c) - '0' <= 9)

int strverscmp (const char *s1, const char *s2)
{
    const unsigned char *p1 = (const unsigned char *) s1;
    const unsigned char *p2 = (const unsigned char *) s2;
    unsigned char c1, c2;
    int state;
    int diff;

    /* Symbol(s)    0       [1-9]   others  (padding)
     Transition   (10) 0  (01) d  (00) x  (11) -   */
    static const unsigned int next_state[] =
    {
      /* state    x    d    0    - */
      /* S_N */  S_N, S_I, S_Z, S_N,
      /* S_I */  S_N, S_I, S_I, S_I,
      /* S_F */  S_N, S_F, S_F, S_F,
      /* S_Z */  S_N, S_F, S_Z, S_Z
    };

    static const int result_type[] =
    {
      /* state   x/x  x/d  x/0  x/-  d/x  d/d  d/0  d/-
                 0/x  0/d  0/0  0/-  -/x  -/d  -/0  -/- */

      /* S_N */  CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
                 CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
      /* S_I */  CMP, -1,  -1,  CMP,  1,  LEN, LEN, CMP,
                  1,  LEN, LEN, CMP, CMP, CMP, CMP, CMP,
      /* S_F */  CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
                 CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
      /* S_Z */  CMP,  1,   1,  CMP, -1,  CMP, CMP, CMP,
                 -1,  CMP, CMP, CMP
    };

    if (p1 == p2)
        return 0;

    c1 = *p1++;
    c2 = *p2++;
    /* Hint: '0' is a digit too.  */
    state = S_N | ((c1 == '0') + (ISDIGIT (c1) != 0));

    while ((diff = c1 - c2) == 0 && c1 != '\0')
    {
        state = next_state[state];
        c1 = *p1++;
        c2 = *p2++;
        state |= (c1 == '0') + (ISDIGIT (c1) != 0);
    }

    state = result_type[state << 2 | ((c2 == '0') + (ISDIGIT (c2) != 0))];

    switch (state)
    {
        case CMP:
            return diff;
        
        case LEN:
            while (ISDIGIT (*p1++))
                if (!ISDIGIT (*p2++))
                    return 1;
            return ISDIGIT (*p2) ? -1 : diff;
        default:
            return state;
    }

}
#endif

static void write_provision_ra_prefix_file(uint8_t *prefix, uint8_t	prefix_len, uint32_t lease)
{
	FILE *fp = NULL;

	if (!(fp = fopen(RA_PREFIX_FILE, "w"))) {
		return;
	}

	fprintf(fp, "ipv6 prefix address: %s\n" \
			"ipv6 prefix len: %d\n" \
			"ipv6 lease: %d\n",
			prefix, prefix_len, lease
		   );

	fclose(fp);
}

static int nl_recv_nduser(uint8_t *msg, size_t msg_size)
{
	struct nduseroptmsg *ndmsg = NULL;

	ndmsg = (struct nduseroptmsg*) NLMSG_DATA((struct nlmsghdr*) msg);

	if (msg_size < NLMSG_SPACE(sizeof(struct nduseroptmsg))) {
		return -1;
	}

	if (ndmsg->nduseropt_family != AF_INET6
			|| ndmsg->nduseropt_icmp_type != ND_ROUTER_ADVERT
			|| ndmsg->nduseropt_icmp_code != 0) {
		return -1;
	}

	/* Aruba: Parses RA flags */
	{
		unsigned flags;

		flags = ndmsg->nduseropt_pad1;
		M_f = O_f = 0;
		if (flags & ND_RA_FLAG_MANAGED) {
			M_f = 1;
		}
		if (flags & ND_RA_FLAG_OTHER) {
			O_f = 1;
		}
	}

	if (msg_size < NLMSG_SPACE(sizeof(struct nduseroptmsg) + ndmsg->nduseropt_opts_len)) {
		return -1;
	}

	return parse_nd_opts((struct nd_opt_hdr *) (ndmsg + 1), ndmsg->nduseropt_opts_len, ndmsg->nduseropt_ifindex);
}

static int nl_recv_prefix(uint8_t *msg, size_t msg_size)
{
	uint8_t	prefix_len = 0;
	uint8_t prefix_buf[INET6_ADDRSTRLEN];
	struct nlattr *attr = NULL;
	struct prefix_cacheinfo *ci = NULL;
	struct in6_addr	*prefix = NULL;
	struct prefixmsg *pfmsg = NULL;

	if (msg_size < NLMSG_SPACE(sizeof(struct prefixmsg))) {
		return -1;
	}

	pfmsg = (struct prefixmsg*) NLMSG_DATA((struct nlmsghdr*) msg);

	if (pfmsg->prefix_family != AF_INET6) {
		return -1;
	}

	prefix_len = pfmsg->prefix_len;

	attr = (struct nlattr*)(pfmsg + 1);
	if (attr->nla_type != PREFIX_ADDRESS) {
		return -1;
	}

	prefix = (struct in6_addr*)(attr + 1);

	memset(prefix_buf, 0, INET6_ADDRSTRLEN);
	inet_ntop(AF_INET6, prefix, prefix_buf, INET6_ADDRSTRLEN);

	attr = (struct nlattr*)(prefix + 1);
	if (attr->nla_type != PREFIX_CACHEINFO) {
		return -1;
	}

	ci = (struct prefix_cacheinfo*)(attr + 1);

	write_provision_ra_prefix_file(prefix_buf, prefix_len, ci->valid_time);

	return PREFIX_SUCCESS;
}

static int nl_recv(int fd)
{
	unsigned int buf_size = NLMSG_SPACE(65536 - sizeof(struct icmp6_hdr));
	uint8_t buf[buf_size];
	int res = 0;
	size_t msg_size;
	struct nlmsghdr *nlh = NULL;

	memset(buf, 0, buf_size);
	msg_size = recv(fd, buf, buf_size, 0);
	if (msg_size == (size_t)(-1)) {
		return -1;
	}

	if (msg_size < NLMSG_HDRLEN) {
		return -1;
	}

	nlh = (struct nlmsghdr*)buf;

	switch (nlh->nlmsg_type)
	{
		case RTM_NEWNDUSEROPT:
			res = nl_recv_nduser(buf, msg_size);
			break;

		case RTM_NEWPREFIX:
			res = nl_recv_prefix(buf, msg_size);
			break;
		default:
			return -1;
	}

	return res;
}

static int nl_socket (void)
{
	struct sockaddr_nl saddr;
	struct utsname uts;
	int fd;

	fd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
	if (fd == -1)
	{
		syslog(LOG_CRIT, _("cannot open netlink socket"));
		return fd;
	}

	memset(&saddr, 0, sizeof(struct sockaddr_nl));
	saddr.nl_family = AF_NETLINK;
	saddr.nl_pid = getpid();
	saddr.nl_groups = (1 << (RTNLGRP_ND_USEROPT - 1)) | (1 << (RTNLGRP_IPV6_PREFIX - 1));
    
    if (bind (fd, (struct sockaddr *) &saddr, sizeof (struct sockaddr_nl))) {
        close(fd);
        return -1;
    }
    
    return fd;
}

const rdnss_src_t rdnss_netlink = { nl_socket, nl_recv };

#endif /* __linux__ */
