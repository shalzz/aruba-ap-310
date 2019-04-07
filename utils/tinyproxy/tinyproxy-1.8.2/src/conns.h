/* tinyproxy - A fast light-weight HTTP proxy
 * Copyright (C) 2001 Robert James Kaes <rjkaes@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/* See 'conns.c' for detailed information. */

#ifndef TINYPROXY_CONNS_H
#define TINYPROXY_CONNS_H

#include "main.h"
#include "hashmap.h"

#define  ECP_COOKIE     "ecp_session"

#ifdef __FAT_AP__
#include "util/util_instant.h"

enum {
    BUILD_AUTO_WL_NONE = 0,
    BUILD_AUTO_WL_ALL,
    BUILD_AUTO_WL_DOC_ONLY
};
#endif

/*
 * Connection Definition
 */
struct conn_s {
        int client_fd;
        int server_fd;

        struct buffer_s *cbuffer;
        struct buffer_s *sbuffer;

        /* The request line (first line) from the client */
        char *request_line;

        /* Booleans */
        unsigned int connect_method;
        unsigned int show_stats;
        unsigned int build_auto_wl;      /* build auto WL from response */

        /*
         * This structure stores key -> value mappings for substitution
         * in the error HTML files.
         */
        hashmap_t error_variables;

        int error_number;
        char *error_string;

        /* A Content-Length value from the remote server */
        struct {
                long int server;
                long int client;
        } content_length;

        /*
         * Store the server's IP (for BindSame)
         */
        char *server_ip_addr;
        int   server_port;

        /*
         * Store the client's IP and hostname information
         */
        char *client_ip_addr;
        char *client_string_addr;
        int client_port;

        /*
         * Store the incoming request's HTTP protocol.
         */
        struct {
                unsigned int major;
                unsigned int minor;
        } protocol;

#ifdef REVERSE_SUPPORT
        /*
         * Place to store the current per-connection reverse proxy path
         */
        char *reversepath;
#endif

        /*
         * Pointer to upstream proxy.
         */
        struct upstream *upstream_proxy;

#ifdef __FAT_AP__
        char doc_base[256];
        char filter_chunk_encoding;
        char insert_doc_base;
        char prefix[2048];
        char mangle_ecp_headers;
        char user_agent[64];
        char user_authenticated;
        swarm_info_user_t user;
        struct upstream *current_up;
        char cp_type;
        char is_wispr;
#endif
};

/*
 * Functions for the creation and destruction of a connection structure.
 */
extern struct conn_s *initialize_conn (int client_fd, const char *ipaddr,
                                       const char *string_addr,
                                       const char *sock_ipaddr);
extern void destroy_conn (struct conn_s *connptr);

#endif
