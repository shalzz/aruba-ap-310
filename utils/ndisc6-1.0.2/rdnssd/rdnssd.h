/**
 * rdnssd.h - daemon for DNS configuration from ICMPv6 RA
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

#ifndef NDISC6_RDNSSD_H
# define NDISC6_RDNSSD_H 1

enum {
	NDUSEROPT_SUCCESS = 1,
	PREFIX_SUCCESS
};

#define RA_PREFIX_FILE "/tmp/provision_record_info/ra_prefix_file"
#define RA_DNS_SERVER_FILE "/tmp/provision_record_info/ra_dns_server_file"

typedef struct rdnss_src
{
	int (*setup) (void);
	int (*process) (int fd);
} rdnss_src_t;

extern const rdnss_src_t rdnss_netlink, rdnss_icmp;
extern unsigned M_O_flags[2];
#define M_f M_O_flags[0]
#define O_f M_O_flags[1]

/* Belongs in <netinet/icmp6.h> */
#define ND_OPT_RDNSS 25
#define ND_OPT_DNSSL 31

struct nd_opt_rdnss
{
	uint8_t nd_opt_rdnss_type;
	uint8_t nd_opt_rdnss_len;
	uint16_t nd_opt_rdnss_reserved;
	uint32_t nd_opt_rdnss_lifetime;
	/* followed by one or more IPv6 addresses */
};

struct nd_opt_dnssl
{
    uint8_t nd_opt_dnssl_type;
    uint8_t nd_opt_dnssl_len;
    uint16_t nd_opt_dnssl_reserved;
    uint32_t nd_opt_dnssl_lifetime;
    /* followed by one or more domain names */
};

# ifdef __cplusplus
extern "C" {
# endif

int parse_nd_opts (const struct nd_opt_hdr *opt, size_t opts_len, unsigned int ifindex);

# ifdef __cplusplus
}
# endif

#endif /* !NDISC6_RDNSSD_H */

