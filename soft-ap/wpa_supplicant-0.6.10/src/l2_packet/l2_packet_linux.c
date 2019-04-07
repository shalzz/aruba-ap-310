/*
 * WPA Supplicant - Layer2 packet handling with Linux packet sockets
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <net/if.h>

#include "common.h"
#include "eloop.h"
#include "l2_packet.h"
#include <include/compat.h>
#include <net80211/ieee80211.h>
#undef host_to_le32
#undef host_to_le16
#undef le_to_host32
#undef le_to_host16
#include <wifi/wifi.h>

#ifdef __FAT_AP__
#include "../wpa_supplicant/wpa_supplicant_i.h"

#define IS_WIFI_UPLINK(_ctx)    ( 1== ((struct wpa_supplicant *)(_ctx))->drvmode )
#endif

struct l2_packet_data {
	int fd; /* packet socket for EAPOL frames */
	char ifname[IFNAMSIZ + 1];
	int ifindex;
	u8 own_addr[ETH_ALEN];
        int ether_encap;
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len);
	void *rx_callback_ctx;
	int l2_hdr; /* whether to include layer 2 (Ethernet) header data
		     * buffers */
};


int l2_packet_get_own_addr(struct l2_packet_data *l2, u8 *addr)
{
	os_memcpy(addr, l2->own_addr, ETH_ALEN);
	return 0;
}


int l2_packet_send(struct l2_packet_data *l2, const u8 *dst_addr, u16 proto,
		   const u8 *buf, size_t len)
{
	int ret, hlen, alloc=0;
	u8 *bp=NULL;
	struct l2_wlanhdr *wh;
	struct l2_ethhdr *eth;
	struct llc *llc;

	if (l2 == NULL)
		return -1;
        if (l2->ether_encap == 0) {
                hlen = sizeof(struct l2_wlanhdr);
                if ((bp = (u8 *)os_malloc(len+hlen)) == NULL) {
                        wpa_printf(MSG_ERROR, "%s: malloc error. Dropping EAPOL", __FUNCTION__);
                        return -1;
                }
                alloc = 1;
                wh = (struct l2_wlanhdr *) bp;
                memset(wh, 0, sizeof(*wh));
                wh->frame_control = IEEE80211_FC(WLAN_FC_TYPE_DATA,
                                                 WLAN_FC_STYPE_DATA);
                memcpy(wh->da, dst_addr, ETH_ALEN);
                memcpy(wh->sa, l2->own_addr, ETH_ALEN);
                memcpy(wh->bssid, dst_addr, ETH_ALEN);
                llc = (struct llc *)&wh->llc;
                llc->llc_dsap = LLC_SNAP_LSAP;
                llc->llc_ssap = LLC_SNAP_LSAP;
                llc->llc_control = LLC_UI;
                llc->llc_snap.org_code[0] = 0;
                llc->llc_snap.org_code[1] = 0;
                llc->llc_snap.org_code[2] = 0;
                llc->llc_un.type_snap.ether_type=htons(proto);
                memcpy(wh+1, buf, len);
                len = len+hlen;
                buf = bp;
        }
	if (l2->l2_hdr) {
		ret = send(l2->fd, buf, len, 0);
		if (ret < 0)
			wpa_printf(MSG_ERROR, "%s - send:%s", __FUNCTION__, strerror(errno));
	} else {
		struct sockaddr_ll ll;
		os_memset(&ll, 0, sizeof(ll));
		ll.sll_family = AF_PACKET;
		ll.sll_ifindex = l2->ifindex;
		ll.sll_protocol = htons(proto);
		ll.sll_halen = ETH_ALEN;
		os_memcpy(ll.sll_addr, dst_addr, ETH_ALEN);
		ret = sendto(l2->fd, buf, len, 0, (struct sockaddr *) &ll,
			     sizeof(ll));
		wpa_printf(MSG_ERROR, "%s:%d - send out ret %d ", __FUNCTION__, __LINE__, ret);
		if (ret < 0)
			perror("l2_packet_send - sendto");
	}
        if (alloc && (bp != NULL)) os_free(bp);
	return ret;
}

void
supp_dump_pkt_hex(const void *ptr, size_t len)
{
    int i;
	int n =0;
	char str[2048];
	memset(str, 0, sizeof(str));

    if( (ptr == NULL) || (len ==  0) ) {
        return;
    }

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0)
            n += sprintf(str+n, "\n%03d:", i);
        n += sprintf(str+n ," %02x", ((u_int8_t *)ptr)[i]);
    }
wpa_printf(MSG_ERROR, "%s", str);
}

static void l2_packet_receive(int sock, void *eloop_ctx, void *sock_ctx)
{
	struct l2_packet_data *l2 = eloop_ctx;
	u8 buf[2300], *bp;

	int res;
	struct sockaddr_ll ll;
	socklen_t fromlen;
	u8		  * addr;
	struct l2_ethhdr * eth;
        if (l2->ether_encap) {
                os_memset(&ll, 0, sizeof(ll));
                fromlen = sizeof(ll);
                res = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *) &ll,
                               &fromlen);
                if (res < 0) {
                        perror("l2_packet_receive - recvfrom");
                        return;
                }
                
                l2->rx_callback(l2->rx_callback_ctx, ll.sll_addr, buf, res);
        } else {
                res = recv(sock, buf, sizeof(buf), 0);
                if (res > 0) {
#ifdef __FAT_AP__
                if( !IS_WIFI_UPLINK(l2->rx_callback_ctx) ) {
#endif
                        eth = (struct l2_ethhdr *)buf;
                        addr = eth->h_source;
                        bp = buf + sizeof(struct l2_ethhdr);
                        res -= sizeof(struct l2_ethhdr);
#ifdef __FAT_AP__
                l2->rx_callback(l2->rx_callback_ctx, addr, bp, res);
                return;
                }
	l2->rx_callback(l2->rx_callback_ctx, NULL, buf, res);
#endif
                } else
                        return;

		wpa_printf(MSG_ERROR, "%s:%d - recevei packet >>>>>>>>> 000 ", __FUNCTION__, __LINE__);
#ifndef __FAT_AP__
                l2->rx_callback(l2->rx_callback_ctx, addr, bp, res);
#endif
        }
}


struct l2_packet_data * l2_packet_init(
	const char *ifname, const char *wif, const u8 *own_addr, unsigned short protocol,
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len),
	void *rx_callback_ctx, int l2_hdr)
{
	struct l2_packet_data *l2;
	struct ifreq ifr;
	struct sockaddr_ll ll;

wpa_printf(MSG_ERROR, "%s - enter.... if name is %s, wif %p", __FUNCTION__, ifname, wif);
	l2 = os_zalloc(sizeof(struct l2_packet_data));
	if (l2 == NULL)
		return NULL;
	os_strlcpy(l2->ifname, ifname, sizeof(l2->ifname));
        if (wif == NULL)
                l2->ether_encap = 1;
	l2->rx_callback = rx_callback;
	l2->rx_callback_ctx = rx_callback_ctx;
	l2->l2_hdr = l2_hdr;

	l2->fd = socket(PF_PACKET, l2_hdr ? SOCK_RAW : SOCK_DGRAM,
			htons(protocol));
	if (l2->fd < 0) {
		perror("socket(PF_PACKET)");
		os_free(l2);
		return NULL;
	}
	os_memset(&ifr, 0, sizeof(ifr));
        if (wif) {
                os_strlcpy(ifr.ifr_name, "anul0", sizeof(ifr.ifr_name));
		}
        else
                os_strlcpy(ifr.ifr_name, l2->ifname, sizeof(ifr.ifr_name));
wpa_printf(MSG_ERROR, "%s - using the name %s", __FUNCTION__, ifr.ifr_name);
	if (ioctl(l2->fd, SIOCGIFINDEX, &ifr) < 0) {
		perror("ioctl[SIOCGIFINDEX]");
		close(l2->fd);
		os_free(l2);
		return NULL;
	}
	l2->ifindex = ifr.ifr_ifindex;

	os_memset(&ll, 0, sizeof(ll));
	ll.sll_family = PF_PACKET;
	ll.sll_ifindex = ifr.ifr_ifindex;
	ll.sll_protocol = htons(protocol);
	if (bind(l2->fd, (struct sockaddr *) &ll, sizeof(ll)) < 0) {
		perror("bind[PF_PACKET]");
		close(l2->fd);
		os_free(l2);
		return NULL;
	}

	memset(&ifr, 0, sizeof(ifr));
	os_strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(l2->fd, SIOCGIFHWADDR, &ifr) < 0) {
		perror("ioctl[SIOCGIFHWADDR]");
		close(l2->fd);
		os_free(l2);
		return NULL;
	}
	os_memcpy(l2->own_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

	eloop_register_read_sock(l2->fd, l2_packet_receive, l2, NULL);

	return l2;
}


void l2_packet_deinit(struct l2_packet_data *l2)
{
	if (l2 == NULL)
		return;
	if (l2->fd >= 0) {
		eloop_unregister_read_sock(l2->fd);
		close(l2->fd);
	}
		
	os_free(l2);
}


int l2_packet_get_ip_addr(struct l2_packet_data *l2, char *buf, size_t len)
{
	int s;
	struct ifreq ifr;
	struct sockaddr_in *saddr;
	size_t res;

	s = socket(PF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("socket");
		return -1;
	}
	os_memset(&ifr, 0, sizeof(ifr));
	os_strlcpy(ifr.ifr_name, l2->ifname, sizeof(ifr.ifr_name));
	if (ioctl(s, SIOCGIFADDR, &ifr) < 0) {
		if (errno != EADDRNOTAVAIL)
			perror("ioctl[SIOCGIFADDR]");
		close(s);
		return -1;
	}
	close(s);
	saddr = aliasing_hide_typecast(&ifr.ifr_addr, struct sockaddr_in);
	if (saddr->sin_family != AF_INET)
		return -1;
	res = os_strlcpy(buf, inet_ntoa(saddr->sin_addr), len);
	if (res >= len)
		return -1;
	return 0;
}


void l2_packet_notify_auth_start(struct l2_packet_data *l2)
{
}
