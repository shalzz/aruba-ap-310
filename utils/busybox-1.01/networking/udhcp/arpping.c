/*
 * arpping.c
 *
 * Mostly stolen from: dhcpcd - DHCP client daemon
 * by Yoichi Hariguchi <yoichi@fore.com>
 */

#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

#include "dhcpd.h"
#include "arpping.h"
#include "common.h"

#ifndef CONFIG_ARUBA_AP
/* args:	yiaddr - what IP to ping
 *		ip - our ip
 *		mac - our arp address
 *		interface - interface to use
 * retn: 	1 addr free
 *		0 addr used
 *		-1 error
 */

/* FIXME: match response against chaddr */
int arpping(uint32_t yiaddr, uint32_t ip, uint8_t *mac, char *interface)
{

	int	timeout = 2;
	int 	optval = 1;
	int	s;			/* socket */
	int	rv = 1;			/* return value */
	struct sockaddr addr;		/* for interface name */
	struct arpMsg	arp;
	fd_set		fdset;
	struct timeval	tm;
	time_t		prevTime;


	if ((s = socket (PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) == -1) {
#ifdef IN_BUSYBOX
		LOG(LOG_ERR, bb_msg_can_not_create_raw_socket);
#else
		LOG(LOG_ERR, "Could not open raw socket");
#endif
		return -1;
	}

	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
		LOG(LOG_ERR, "Could not setsocketopt on raw socket");
		close(s);
		return -1;
	}

	/* send arp request */
	memset(&arp, 0, sizeof(arp));
	memcpy(arp.h_dest, MAC_BCAST_ADDR, 6);		/* MAC DA */
	memcpy(arp.h_source, mac, 6);			/* MAC SA */
	arp.h_proto = htons(ETH_P_ARP);			/* protocol type (Ethernet) */
	arp.htype = htons(ARPHRD_ETHER);		/* hardware type */
	arp.ptype = htons(ETH_P_IP);			/* protocol type (ARP message) */
	arp.hlen = 6;					/* hardware address length */
	arp.plen = 4;					/* protocol address length */
	arp.operation = htons(ARPOP_REQUEST);		/* ARP op code */
	memcpy(arp.sInaddr, &ip, sizeof(ip));		/* source IP address */
	memcpy(arp.sHaddr, mac, 6);			/* source hardware address */
	memcpy(arp.tInaddr, &yiaddr, sizeof(yiaddr));	/* target IP address */

	memset(&addr, 0, sizeof(addr));
	strcpy(addr.sa_data, interface);
	if (sendto(s, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0)
		rv = 0;

	/* wait arp reply, and check it */
	tm.tv_usec = 0;
	prevTime = uptime();
	while (timeout > 0) {
		FD_ZERO(&fdset);
		FD_SET(s, &fdset);
		tm.tv_sec = timeout;
		if (select(s + 1, &fdset, (fd_set *) NULL, (fd_set *) NULL, &tm) < 0) {
			DEBUG(LOG_ERR, "Error on ARPING request: %m");
			if (errno != EINTR) rv = 0;
		} else if (FD_ISSET(s, &fdset)) {
			if (recv(s, &arp, sizeof(arp), 0) < 0 ) rv = 0;
			if (arp.operation == htons(ARPOP_REPLY) &&
			    bcmp(arp.tHaddr, mac, 6) == 0 &&
//			    *((uint32_t *) arp.sInaddr) == yiaddr) {
			    memcmp(arp.sInaddr, &yiaddr, sizeof(yiaddr)) == 0) {
//			    ip_to_u32(arp.sInaddr) == yiaddr) {
				DEBUG(LOG_INFO, "Valid arp reply receved for this address");
				rv = 0;
				break;
			}
		}
		timeout -= uptime() - prevTime;
		prevTime = uptime();
	}
	close(s);
	DEBUG(LOG_INFO, "%salid arp replies for this address", rv ? "No v" : "V");	
	return rv;
}

#else
/* Bug182480: using busybox-1.29's code to work on newer version Linux */

/* Wrapper which restarts poll on EINTR or ENOMEM.
 * On other errors does perror("poll") and returns.
 * Warning! May take longer than timeout_ms to return! */
static int safe_poll(struct pollfd *ufds, nfds_t nfds, int timeout)
{
	while (1) {
		int n = poll(ufds, nfds, timeout);
		if (n >= 0)
			return n;
		/* Make sure we inch towards completion */
		if (timeout > 0)
			timeout--;
		/* E.g. strace causes poll to return this */
		if (errno == EINTR)
			continue;
		/* Kernel is very low on memory. Retry. */
		/* I doubt many callers would handle this correctly! */
		if (errno == ENOMEM)
			continue;
//		bb_perror_msg("poll");
		return n;
	}
}

enum {
	ARP_MSG_SIZE = 0x2a
};

/* Returns 1 if no reply received */
int arpping(uint32_t test_nip, uint32_t from_ip, uint8_t *from_mac, char *interface)
{
    unsigned timeo = 2 * 1000;
    uint8_t *safe_mac = from_mac;
	int timeout_ms;
	struct pollfd pfd[1];
#define s (pfd[0].fd)           /* socket */
	int rv = 1;             /* "no reply received" yet */
	struct sockaddr addr;   /* for interface name */
	struct arpMsg arp;
	int 	optval = 1;

	if (!timeo)
		return 1;

	s = socket(PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP));
	if (s == -1) {
		bb_perror_msg(bb_msg_can_not_create_raw_socket);
		return -1;
	}

	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
		LOG(LOG_ERR, "Could not setsocketopt on raw socket");
		close(s);
		return -1;
	}

	/* send arp request */
	memset(&arp, 0, sizeof(arp));
	memset(arp.h_dest, 0xff, 6);                    /* MAC DA */
	memcpy(arp.h_source, from_mac, 6);              /* MAC SA */
	arp.h_proto = htons(ETH_P_ARP);                 /* protocol type (Ethernet) */
	arp.htype = htons(ARPHRD_ETHER);                /* hardware type */
	arp.ptype = htons(ETH_P_IP);                    /* protocol type (ARP message) */
	arp.hlen = 6;                                   /* hardware address length */
	arp.plen = 4;                                   /* protocol address length */
	arp.operation = htons(ARPOP_REQUEST);           /* ARP op code */
	memcpy(arp.sHaddr, from_mac, 6);                /* source hardware address */
	memcpy(arp.sInaddr, &from_ip, sizeof(from_ip)); /* source IP address */
	/* tHaddr is zero-filled */                     /* target hardware address */
	memcpy(arp.tInaddr, &test_nip, sizeof(test_nip));/* target IP address */

	memset(&addr, 0, sizeof(addr));
	strncpy(addr.sa_data, interface, sizeof(addr.sa_data));
	if (sendto(s, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0) {
		// TODO: error message? caller didn't expect us to fail,
		// just returning 1 "no reply received" misleads it.
		goto ret;
	}

	/* wait for arp reply, and check it */
	timeout_ms = (int)timeo;
	do {
		typedef uint32_t aliased_uint32_t __attribute__((__may_alias__));
		int r;
		unsigned prevTime = uptime() * 1000;

		pfd[0].events = POLLIN;
		r = safe_poll(pfd, 1, timeout_ms);
		if (r < 0)
			break;
		if (r) {
			r = safe_read(s, &arp, sizeof(arp));
			if (r < 0)
				break;

			//log3("sHaddr %02x:%02x:%02x:%02x:%02x:%02x",
			//	arp.sHaddr[0], arp.sHaddr[1], arp.sHaddr[2],
			//	arp.sHaddr[3], arp.sHaddr[4], arp.sHaddr[5]);

			if (r >= ARP_MSG_SIZE
			 && arp.operation == htons(ARPOP_REPLY)
			 /* don't check it: Linux doesn't return proper tHaddr (fixed in 2.6.24?) */
			 /* && memcmp(arp.tHaddr, from_mac, 6) == 0 */
			 && *(aliased_uint32_t*)arp.sInaddr == test_nip
			) {
				/* if ARP source MAC matches safe_mac
				 * (which is client's MAC), then it's not a conflict
				 * (client simply already has this IP and replies to ARPs!)
				 */
				if (!safe_mac || memcmp(safe_mac, arp.sHaddr, 6) != 0)
					rv = 0;
				//else log2("sHaddr == safe_mac");
				break;
			}
		}
		timeout_ms -= (unsigned)uptime() * 1000 - prevTime + 1;

		/* We used to check "timeout_ms > 0", but
		 * this is more under/overflow-resistant
		 * (people did see overflows here when system time jumps):
		 */
	} while ((unsigned)timeout_ms <= timeo);

 ret:
	close(s);
	DEBUG(LOG_INFO, "%salid arp replies for this address", rv ? "No v" : "V");	
	return rv;
}

#endif /* CONFIG_ARUBA_AP */
