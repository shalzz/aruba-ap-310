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

#ifndef CTRL_IFACE_H
#define CTRL_IFACE_H

int hostapd_ctrl_iface_init(struct hostapd_data *hapd);
void hostapd_ctrl_iface_deinit(struct hostapd_data *hapd);
void hostapd_ctrl_iface_send(struct hostapd_data *hapd, int level,
			     char *buf, size_t len);
int wait_for_meshd(int s, struct meshd_params *mp, struct meshd_params *mrp);
int create_meshd_sync_sock();
int request_meshd_configs(int s, struct meshd_params *mp,
        struct meshd_params *mrp);
void hostapd_meshd_register_cb(int s);
int recv_meshd_configs(int s, struct meshd_params *mp,
        struct meshd_params *mrp);
void inform_meshd_sta_auth_status(int s, u_int8_t *mac, unsigned char cmdid);

#endif /* CTRL_IFACE_H */
