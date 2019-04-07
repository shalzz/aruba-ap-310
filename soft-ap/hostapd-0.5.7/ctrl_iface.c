/*
 * hostapd / UNIX domain socket -based control interface
 * Copyright (c) 2004, Jouni Malinen <jkmaline@cc.hut.fi>
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

#ifndef CONFIG_NATIVE_WINDOWS

#include <sys/un.h>
#include <sys/stat.h>

#include "hostapd.h"
#include "eloop.h"
#include "config.h"
#include "eapol_sm.h"
#include "ieee802_1x.h"
#include "wpa.h"
#include "radius_client.h"
#include "ieee802_11.h"
#include "ctrl_iface.h"
#include "sta_info.h"
#include "accounting.h"
#include "../meshd/meshd_wpa_iface.h"
#include "radius.h"

extern struct meshd_params meshd_params;
extern struct meshd_params meshd_recovery_params;
extern struct hostapd_ssid meshd_recovery_ssid;
extern Boolean             meshd_recovery_valid;

struct wpa_ctrl_dst {
	struct wpa_ctrl_dst *next;
	struct sockaddr_un addr;
	socklen_t addrlen;
	int debug_level;
	int errors;
};


static int hostapd_ctrl_iface_attach(struct hostapd_data *hapd,
				     struct sockaddr_un *from,
				     socklen_t fromlen)
{
	struct wpa_ctrl_dst *dst;

	dst = wpa_zalloc(sizeof(*dst));
	if (dst == NULL)
		return -1;
	memcpy(&dst->addr, from, sizeof(struct sockaddr_un));
	dst->addrlen = fromlen;
	dst->debug_level = MSG_INFO;
	dst->next = hapd->ctrl_dst;
	hapd->ctrl_dst = dst;
	wpa_hexdump(MSG_DEBUG, "CTRL_IFACE monitor attached",
		    (u8 *) from->sun_path, fromlen);
	return 0;
}


static int hostapd_ctrl_iface_detach(struct hostapd_data *hapd,
				     struct sockaddr_un *from,
				     socklen_t fromlen)
{
	struct wpa_ctrl_dst *dst, *prev = NULL;

	dst = hapd->ctrl_dst;
	while (dst) {
		if (fromlen == dst->addrlen &&
		    memcmp(from->sun_path, dst->addr.sun_path, fromlen) == 0) {
			if (prev == NULL)
				hapd->ctrl_dst = dst->next;
			else
				prev->next = dst->next;
			free(dst);
			wpa_hexdump(MSG_DEBUG, "CTRL_IFACE monitor detached",
				    (u8 *) from->sun_path, fromlen);
			return 0;
		}
		prev = dst;
		dst = dst->next;
	}
	return -1;
}


static int hostapd_ctrl_iface_level(struct hostapd_data *hapd,
				    struct sockaddr_un *from,
				    socklen_t fromlen,
				    char *level)
{
	struct wpa_ctrl_dst *dst;

	wpa_printf(MSG_DEBUG, "CTRL_IFACE LEVEL %s", level);

	dst = hapd->ctrl_dst;
	while (dst) {
		if (fromlen == dst->addrlen &&
		    memcmp(from->sun_path, dst->addr.sun_path, fromlen) == 0) {
			wpa_hexdump(MSG_DEBUG, "CTRL_IFACE changed monitor "
				    "level", (u8 *) from->sun_path, fromlen);
			dst->debug_level = atoi(level);
			return 0;
		}
		dst = dst->next;
	}

	return -1;
}


static int hostapd_ctrl_iface_sta_mib(struct hostapd_data *hapd,
				      struct sta_info *sta,
				      char *buf, size_t buflen)
{
	int len, res, ret;

	if (sta == NULL) {
		ret = snprintf(buf, buflen, "FAIL\n");
		if (ret < 0 || (size_t) ret >= buflen)
			return 0;
		return ret;
	}

	len = 0;
	ret = snprintf(buf + len, buflen - len, MACSTR "\n",
		       MAC2STR(sta->addr));
	if (ret < 0 || (size_t) ret >= buflen - len)
		return len;
	len += ret;

	res = ieee802_11_get_mib_sta(hapd, sta, buf + len, buflen - len);
	if (res >= 0)
		len += res;
	res = wpa_get_mib_sta(sta->wpa_sm, buf + len, buflen - len);
	if (res >= 0)
		len += res;
	res = ieee802_1x_get_mib_sta(hapd, sta, buf + len, buflen - len);
	if (res >= 0)
		len += res;

	return len;
}


static int hostapd_ctrl_iface_sta_first(struct hostapd_data *hapd,
					char *buf, size_t buflen)
{
	return hostapd_ctrl_iface_sta_mib(hapd, hapd->sta_list, buf, buflen);
}


static int hostapd_ctrl_iface_sta(struct hostapd_data *hapd,
				  const char *txtaddr,
				  char *buf, size_t buflen)
{
	u8 addr[ETH_ALEN];
	int ret;

	if (hwaddr_aton(txtaddr, addr)) {
		ret = snprintf(buf, buflen, "FAIL\n");
		if (ret < 0 || (size_t) ret >= buflen)
			return 0;
		return ret;
	}
	return hostapd_ctrl_iface_sta_mib(hapd, ap_get_sta(hapd, addr),
					  buf, buflen);
}


static int hostapd_ctrl_iface_sta_next(struct hostapd_data *hapd,
				       const char *txtaddr,
				       char *buf, size_t buflen)
{
	u8 addr[ETH_ALEN];
	struct sta_info *sta;
	int ret;

	if (hwaddr_aton(txtaddr, addr) ||
	    (sta = ap_get_sta(hapd, addr)) == NULL) {
		ret = snprintf(buf, buflen, "FAIL\n");
		if (ret < 0 || (size_t) ret >= buflen)
			return 0;
		return ret;
	}		
	return hostapd_ctrl_iface_sta_mib(hapd, sta->next, buf, buflen);
}

static int meshd_process_rsn_ie(struct hostapd_data *hapd, u8 *rsn_ie, struct sta_info *sta)
{
	int ielen, res;

	if (sta->wpa_sm == NULL)
		sta->wpa_sm = wpa_auth_sta_init(hapd->wpa_auth, sta->addr);
	if (sta->wpa_sm == NULL) {
		printf("Failed to initialize WPA state machine\n");
		return -1;
	}
	ielen = rsn_ie[1]+2;
	res = wpa_validate_wpa_ie(hapd->wpa_auth, sta->wpa_sm,
				  rsn_ie, ielen);
	if (res != WPA_IE_OK) {
		printf("WPA/RSN information element rejected? (res %u)\n", res);
		return -1;
	}
	return 0;
}

static int hostapd_ctrl_iface_del_sta(struct hostapd_data *hapd,
				      const char *txtaddr)
{
	u8 addr[ETH_ALEN];
	u8 *p;
	struct sta_info *sta;

	wpa_printf(MSG_DEBUG, "CTRL_IFACE DEL_STA %s", txtaddr);

	if (hwaddr_aton(txtaddr, addr)) {
		wpa_printf(MSG_ERROR, "%s: Bad MAC address %s", __FUNCTION__, txtaddr);
		return -1;
	}
	sta = ap_get_sta(hapd, addr);
	if (sta != NULL) {
		sta->flags &= ~(WLAN_STA_AUTH | WLAN_STA_ASSOC);
		wpa_auth_sm_event(sta->wpa_sm, WPA_DISASSOC);
		sta->acct_terminate_cause = RADIUS_ACCT_TERMINATE_CAUSE_USER_REQUEST;
		ieee802_1x_notify_port_enabled(sta->eapol_sm, 0);
		ap_free_sta(hapd, sta);
	}
	return 0;
}

static int
hostapd_ctrl_iface_peer_key (struct hostapd_data *hapd, char *msg)
{
    u8 me[ETH_ALEN], peer[ETH_ALEN], *ptr;

    ptr = msg;
    if (hwaddr_aton(ptr, me)) {
        wpa_printf(MSG_DEBUG, "peer key inform: invalid address!");
        return -1;
    }
    ptr = msg + 17;
    if (hwaddr_aton(ptr, peer)) {
        wpa_printf(MSG_DEBUG, "peer key inform: invalid address!");
        return -1;
    }
    ptr = msg + 34;
    wpa_printf(MSG_DEBUG, "\nme: " MACSTR "\npeer: " MACSTR, MAC2STR(me), MAC2STR(peer));
    wpa_hexdump_ascii(MSG_INFO, "got peer's key", ptr, PMK_LEN);

    hostapd_set_unique_psk(hapd, peer, ptr, PMK_LEN);

    return 0;
}

static int hostapd_ctrl_iface_new_sta(struct hostapd_data *hapd,
				      const char *txtaddr, Boolean recovery)
{
	u8 addr[ETH_ALEN];
	u8 rsn_ie[128];
	u8 *p;
	struct sta_info *sta;

	wpa_printf(MSG_DEBUG, "CTRL_IFACE NEW_STA %s", txtaddr);

	if (hwaddr_aton(txtaddr, addr)) {
		wpa_printf(MSG_ERROR, "%s: Bad MAC address %s", __FUNCTION__, txtaddr);
		return -1;
	}
	if ((p = strchr(txtaddr, ' ')) == NULL) {
		wpa_printf(MSG_ERROR, "%s:No RSN IE found %s", __FUNCTION__, txtaddr);
		return -1;
	}
	++p;
	if (hexstr2bin(p, rsn_ie, strlen(p)/2)) {
		wpa_printf(MSG_ERROR, "Bad RSN IE %s", p);
		return -1;
	}
	sta = ap_get_sta(hapd, addr);
	if (sta) {
		accounting_sta_stop(hapd, sta);
	}

	wpa_printf(MSG_DEBUG, "Add new STA " MACSTR " based on ctrl_iface "
		   "notification", MAC2STR(addr));
	sta = ap_sta_add(hapd, addr);
	if (sta == NULL)
		return -1;

	if (recovery && meshd_recovery_valid == TRUE) {
		sta->ssid = &meshd_recovery_ssid;
	}

	if (hapd->conf->wpa) {
		if (meshd_process_rsn_ie(hapd, rsn_ie, sta))
			return -1;
	}
	sta->flags |= WLAN_STA_AUTH | WLAN_STA_ASSOC;
	wpa_auth_sm_event(sta->wpa_sm, WPA_ASSOC);

	hostapd_new_assoc_sta(hapd, sta, 0);
	accounting_sta_get_id(hapd, sta);
	ieee802_1x_notify_port_enabled(sta->eapol_sm, 1);
	return 0;
}

/* handler for meshd->hostapd control socket */
extern void handle_reload(int sig, void *eloop_ctx, void *signal_ctx);
static void hostapd_meshd_receive(int sock, void *eloop_ctx,
				  void *sock_ctx)
{
	recv_meshd_configs(sock, &meshd_params, &meshd_recovery_params);
	handle_reload(SIGHUP, eloop_get_user_data(), NULL);
}

static void hostapd_ctrl_iface_receive(int sock, void *eloop_ctx,
				       void *sock_ctx)
{
	struct hostapd_data *hapd = eloop_ctx;
	char buf[256];
	int res;
	struct sockaddr_un from;
	socklen_t fromlen = sizeof(from);
	char *reply;
	const int reply_size = 4096;
	int reply_len;

	res = recvfrom(sock, buf, sizeof(buf) - 1, MSG_WAITALL,
		       (struct sockaddr *) &from, &fromlen);
	if (res < 0) {
		wpa_printf(MSG_ERROR, "recvfrom(ctrl_iface)");
		return;
	}
	buf[res] = '\0';
	wpa_hexdump_ascii(MSG_DEBUG, "RX ctrl_iface", (u8 *) buf, res);

	reply = malloc(reply_size);
	if (reply == NULL) {
		sendto(sock, "FAIL\n", 5, 0, (struct sockaddr *) &from,
		       fromlen);
		return;
	}

	memcpy(reply, "OK\n", 3);
	reply_len = 3;

	if (strcmp(buf, "PING") == 0) {
		memcpy(reply, "PONG\n", 5);
		reply_len = 5;
	} else if (strcmp(buf, "MIB") == 0) {
		reply_len = ieee802_11_get_mib(hapd, reply, reply_size);
		if (reply_len >= 0) {
			res = wpa_get_mib(hapd->wpa_auth, reply + reply_len,
					  reply_size - reply_len);
			if (res < 0)
				reply_len = -1;
			else
				reply_len += res;
		}
		if (reply_len >= 0) {
			res = ieee802_1x_get_mib(hapd, reply + reply_len,
						 reply_size - reply_len);
			if (res < 0)
				reply_len = -1;
			else
				reply_len += res;
		}
		if (reply_len >= 0) {
			res = radius_client_get_mib(hapd->radius,
						    reply + reply_len,
						    reply_size - reply_len);
			if (res < 0)
				reply_len = -1;
			else
				reply_len += res;
		}
	} else if (strcmp(buf, "STA-FIRST") == 0) {
		reply_len = hostapd_ctrl_iface_sta_first(hapd, reply,
							 reply_size);
	} else if (strncmp(buf, "STA ", 4) == 0) {
		reply_len = hostapd_ctrl_iface_sta(hapd, buf + 4, reply,
						   reply_size);
	} else if (strncmp(buf, "STA-NEXT ", 9) == 0) {
		reply_len = hostapd_ctrl_iface_sta_next(hapd, buf + 9, reply,
							reply_size);
	} else if (strcmp(buf, "ATTACH") == 0) {
		if (hostapd_ctrl_iface_attach(hapd, &from, fromlen))
			reply_len = -1;
	} else if (strcmp(buf, "DETACH") == 0) {
		if (hostapd_ctrl_iface_detach(hapd, &from, fromlen))
			reply_len = -1;
	} else if (strncmp(buf, "LEVEL ", 6) == 0) {
		if (hostapd_ctrl_iface_level(hapd, &from, fromlen,
						    buf + 6))
			reply_len = -1;
	} else if (strncmp(buf, "NEW_STA ", 8) == 0) {
		if (hostapd_ctrl_iface_new_sta(hapd, buf + 8, FALSE))
			reply_len = -1;
	} else if (strncmp(buf, "NEW_RECOVERY_STA ", 17) == 0) {
		if (hostapd_ctrl_iface_new_sta(hapd, buf + 17, TRUE))
			reply_len = -1;
	} else if (strncmp(buf, "DEL_STA ", 8) == 0) {
		if (hostapd_ctrl_iface_del_sta(hapd, buf + 8))
			reply_len = -1;
        } else if (strncmp(buf, "PEER_KEY ", 9) == 0) {
            if (hostapd_ctrl_iface_peer_key(hapd, buf + 9)) {
                reply_len = -1;
            }
	} else {
		memcpy(reply, "UNKNOWN COMMAND\n", 16);
		reply_len = 16;
	}

	if (reply_len < 0) {
		memcpy(reply, "FAIL\n", 5);
		reply_len = 5;
	}
	sendto(sock, reply, reply_len, 0, (struct sockaddr *) &from, fromlen);
	free(reply);
}


static char * hostapd_ctrl_iface_path(struct hostapd_data *hapd)
{
	char *buf;
	size_t len;

	if (hapd->conf->ctrl_interface == NULL)
		return NULL;

	len = strlen(hapd->conf->ctrl_interface) + strlen(hapd->conf->iface) +
		2;
	buf = malloc(len);
	if (buf == NULL)
		return NULL;

	snprintf(buf, len, "%s/%s",
		 hapd->conf->ctrl_interface, hapd->conf->iface);
	buf[len - 1] = '\0';
	return buf;
}


int hostapd_ctrl_iface_init(struct hostapd_data *hapd)
{
	struct sockaddr_un addr;
	int s = -1;
	char *fname = NULL;

	hapd->ctrl_sock = -1;

	if (hapd->conf->ctrl_interface == NULL)
		return 0;

	if (mkdir(hapd->conf->ctrl_interface, S_IRWXU | S_IRWXG) < 0) {
		if (errno == EEXIST) {
			wpa_printf(MSG_DEBUG, "Using existing control "
				   "interface directory.");
		} else {
			perror("mkdir[ctrl_interface]");
			goto fail;
		}
	}

	if (hapd->conf->ctrl_interface_gid_set &&
	    chown(hapd->conf->ctrl_interface, 0,
		  hapd->conf->ctrl_interface_gid) < 0) {
		perror("chown[ctrl_interface]");
		return -1;
	}

	if (strlen(hapd->conf->ctrl_interface) + 1 + strlen(hapd->conf->iface)
	    >= sizeof(addr.sun_path))
		goto fail;

	s = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("socket(PF_UNIX)");
		goto fail;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	fname = hostapd_ctrl_iface_path(hapd);
	if (fname == NULL)
		goto fail;
	strncpy(addr.sun_path, fname, sizeof(addr.sun_path));
	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind(PF_UNIX)");
		goto fail;
	}

	if (hapd->conf->ctrl_interface_gid_set &&
	    chown(fname, 0, hapd->conf->ctrl_interface_gid) < 0) {
		perror("chown[ctrl_interface/ifname]");
		goto fail;
	}

	if (chmod(fname, S_IRWXU | S_IRWXG) < 0) {
		perror("chmod[ctrl_interface/ifname]");
		goto fail;
	}
	free(fname);

	hapd->ctrl_sock = s;
	eloop_register_read_sock(s, hostapd_ctrl_iface_receive, hapd,
				 NULL);

	return 0;

fail:
	if (s >= 0)
		close(s);
	if (fname) {
		unlink(fname);
		free(fname);
	}
	return -1;
}

void
hostapd_meshd_register_cb(int s)
{
	eloop_register_read_sock(s, hostapd_meshd_receive, NULL,
				 NULL);
}

void hostapd_ctrl_iface_deinit(struct hostapd_data *hapd)
{
	struct wpa_ctrl_dst *dst, *prev;

	if (hapd->ctrl_sock > -1) {
		char *fname;
		eloop_unregister_read_sock(hapd->ctrl_sock);
		close(hapd->ctrl_sock);
		hapd->ctrl_sock = -1;
		fname = hostapd_ctrl_iface_path(hapd);
		if (fname)
			unlink(fname);
		free(fname);

		if (hapd->conf->ctrl_interface &&
		    rmdir(hapd->conf->ctrl_interface) < 0) {
			if (errno == ENOTEMPTY) {
				wpa_printf(MSG_DEBUG, "Control interface "
					   "directory not empty - leaving it "
					   "behind");
			} else {
				perror("rmdir[ctrl_interface]");
			}
		}
	}

	dst = hapd->ctrl_dst;
	while (dst) {
		prev = dst;
		dst = dst->next;
		free(prev);
	}
}


void hostapd_ctrl_iface_send(struct hostapd_data *hapd, int level,
			     char *buf, size_t len)
{
	struct wpa_ctrl_dst *dst, *next;
	struct msghdr msg;
	int idx, ret=0;
	struct iovec io[2];
	char levelstr[10];

	dst = hapd->ctrl_dst;
	if (hapd->ctrl_sock < 0 || dst == NULL)
		return;

        ret = snprintf(levelstr, sizeof(levelstr), "<%d>", level);
        if (ret > sizeof(levelstr)) {
            wpa_printf(MSG_ERROR, "%s: snprintf() truncation: max:%d, ret:%d", 
                       __func__, sizeof(levelstr), ret);
            return;
        }
	io[0].iov_base = levelstr;
	io[0].iov_len = strlen(levelstr);
	io[1].iov_base = buf;
	io[1].iov_len = len;
	memset(&msg, 0, sizeof(msg));
	msg.msg_iov = io;
	msg.msg_iovlen = 2;

	idx = 0;
	while (dst) {
		next = dst->next;
		if (level >= dst->debug_level) {
			wpa_hexdump(MSG_DEBUG, "CTRL_IFACE monitor send",
				    (u8 *) dst->addr.sun_path, dst->addrlen);
			msg.msg_name = &dst->addr;
			msg.msg_namelen = dst->addrlen;
			if (sendmsg(hapd->ctrl_sock, &msg, 0) < 0) {
				fprintf(stderr, "CTRL_IFACE monitor[%d]: ",
					idx);
				perror("sendmsg");
				dst->errors++;
				if (dst->errors > 10) {
					hostapd_ctrl_iface_detach(
						hapd, &dst->addr,
						dst->addrlen);
				}
			} else
				dst->errors = 0;
		}
		idx++;
		dst = next;
	}
}

int
create_meshd_sync_sock()
{
	int s=-1;
	struct sockaddr_un addr;

	unlink(MESHD_HOSTAPD_SYNC_FILE);
	s = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (s < 0) {
		wpa_printf(MSG_ERROR, "socket(PF_UNIX) %s", strerror(errno));
		return -1;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, MESHD_HOSTAPD_SYNC_FILE, sizeof(addr.sun_path));
	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		wpa_printf(MSG_ERROR, "bind(PF_UNIX): %s", strerror(errno));
		close(s);
		return -1;
	}
	if (chmod(MESHD_HOSTAPD_SYNC_FILE, S_IRWXU | S_IRWXG) < 0) {
		wpa_printf(MSG_ERROR, "chmod[MESHD_HOSTAPD_SOCK]: %s",
			   strerror(errno));
		close(s);
		return -1;
	}
	return s;
}

int
recv_meshd_configs(int s, struct meshd_params *mp, struct meshd_params *mrp)
{
	char cmd[256];
	int rlen;
	struct sockaddr_un addr;
	struct sockaddr_un from;
	socklen_t fromlen = sizeof(from);
	char *cmd_p, *p;
	int idx=0;
	char *p_vec[]={0,0,0,0,0,0,0};
	int got_hkey=0, got_pass=0;
	int got_recovery=0;
    int length=0;

	rlen = recvfrom(s, cmd, sizeof(cmd) - 1, MSG_WAITALL,
			(struct sockaddr *) &from, &fromlen);
	if (rlen <= 0) {
		wpa_printf(MSG_ERROR, "hostapd: meshd sync read error: %s", strerror(errno));
		goto fail;
	}
	cmd[rlen] = '\0';
#if 0
	wpa_printf(MSG_INFO, "%s: recvd cmd=%s", __FUNCTION__, cmd);
#endif
	/* Send ACK */
	sendto(s, "OK", 2, 0, (struct sockaddr *) &from, fromlen);

	memset(p_vec, 0, sizeof(p_vec));
    cmd_p = cmd;
    for(idx=0;idx<7;idx++) {
        if(*cmd_p == '/') {
            p = strtok(cmd_p, "/");
            length = atoi(p);
            cmd_p += strlen(p)+2;
            p_vec[idx] = cmd_p;
            cmd_p += length ;
            *cmd_p = '\0';
            cmd_p +=1;
        }
    }
#if 0
	for (cmd_p = cmd, idx=0; (idx < 9) && (p = strtok(cmd_p, "/")); ++idx) {
		cmd_p = NULL;
		p_vec[idx] = p;
        wpa_printf(MSG_INFO, "%s %s %d",__FUNCTION__,p,idx);
	}
	p_vec[6] = p_vec[5]+strlen(p_vec[5])+1;
#endif
	if (memcmp(p_vec[0], HOSTAPD_CMD_CONFIG, strlen(HOSTAPD_CMD_CONFIG)))
	  return -1;

	memset(mp, 0, sizeof(*mp));
	if (p_vec[1]) strcpy(mp->iface, p_vec[1]);
	if (p_vec[2]) strcpy(mp->ssid, p_vec[2]);
	if (p_vec[3] && strcmp(p_vec[3], "-")) {
		got_hkey = 1;
		strcpy(mp->hexkey, p_vec[3]);
	}
	if (p_vec[6] && strcmp(p_vec[6], "-")) {
		got_pass = 1;
		strcpy(mp->passphrase, p_vec[6]);
	}
#if 0
	if (!got_hkey && !got_pass)
		return -1;
#endif
	wpa_printf(MSG_INFO, "Got " HOSTAPD_CMD_CONFIG ": i=%s s=%s\n",
		   mp->iface, mp->ssid);

	memset(mrp, 0, sizeof(*mrp));
	if (p_vec[4] && strcmp(p_vec[4], "-")) {
		got_recovery = 1;
        meshd_recovery_valid = TRUE;
		strcpy(mrp->ssid, p_vec[4]);
	}
	if (p_vec[5] && got_recovery) strcpy(mrp->hexkey, p_vec[5]);

	if (!got_hkey && !got_pass) {
        if (!got_recovery)
            return -1;
        /*
         * For opensystem MPPs, copy recovery profile as the main profile.
         */
        strcpy(mp->ssid, mrp->ssid);
        strcpy(mp->hexkey, mrp->hexkey);
        meshd_recovery_valid = FALSE;
    }
	return 1;
 fail:
	return -1;
}

int
request_meshd_configs(int s, struct meshd_params *mp, struct meshd_params *mrp)
{
	int rlen, rval = -1;
	struct sockaddr_un addr;
        meshd_wpa_cmd_t cmd = {magic: MESHD_WPA_MAGIC, cmd_id: MESHD_WPA_HOSTAPD_UP};

	/* Create socket to recv configs */
	wpa_printf(MSG_INFO, "%s", __FUNCTION__);
	/* Send request for configs */
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, MESHD_SOCKET_NAME, sizeof(addr.sun_path));
	if (sendto(s, &cmd, sizeof(cmd), 0, (struct sockaddr *)&addr, sizeof(addr)) <= 0) {
		wpa_printf(MSG_ERROR, "%s: sendto failed:%s\n", __FUNCTION__, strerror(errno));
		goto fail;
	}
	while (1) {
		if (recv_meshd_configs(s, mp, mrp) > 0)
			break;
	}
	return 1;
fail:
	if (s >= 0)
		close (s);
	unlink(MESHD_HOSTAPD_SYNC_FILE);
	return -1;
	
}

void
inform_meshd_sta_auth_status(int s, u_int8_t *mac, u_char cmdid)
{
#if !defined(AP_PLATFORM) && !defined(CONFIG_GRENACHE)
        return;
#else
	int 			rlen, rval = -1;
	struct sockaddr_un 	addr;
	char			buf[128];
        meshd_wpa_cmd_t *cmd = (meshd_wpa_cmd_t *)buf;

        if(s <=0) {
            wpa_printf(MSG_INFO, "%s: Control socket (%d) is not valid - no-op.",  __FUNCTION__, s);
            return;
        }

        cmd->magic = MESHD_WPA_MAGIC;
        cmd->cmd_id = cmdid;
	wpa_printf(MSG_DEBUG, "%s, " MACSTR " command-id: %d", __FUNCTION__,  MAC2STR(mac), cmd->cmd_id);
	/* Send request for configs */
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, MESHD_SOCKET_NAME, sizeof(addr.sun_path));
        memcpy(cmd->data, mac, ETH_ALEN);
	if (sendto(s, cmd, (sizeof(meshd_wpa_cmd_t)+ETH_ALEN), 0, (struct sockaddr *)&addr, sizeof(addr)) <= 0) {
		wpa_printf(MSG_ERROR, "%s: sendto failed:%s\n", __FUNCTION__, strerror(errno));
	}
	return;
#endif
}

int
wait_for_meshd(int s, struct meshd_params *mp, struct meshd_params *mrp)
{

	wpa_printf(MSG_INFO, "%s", __FUNCTION__);

	while (1) {
		if (recv_meshd_configs(s, mp, mrp) > 0)
			return 1;
	}
	return -1;
}

#endif /* CONFIG_NATIVE_WINDOWS */
