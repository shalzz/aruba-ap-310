/* tinyproxy - A fast light-weight HTTP proxy
 * Copyright (C) 1998 Steven Young <sdyoung@miranda.org>
 * Copyright (C) 1999-2005 Robert James Kaes <rjkaes@users.sourceforge.net>
 * Copyright (C) 2000 Chris Lightfoot <chris@ex-parrot.com>
 * Copyright (C) 2002 Petr Lampa <lampa@fit.vutbr.cz>
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

/* This is where all the work in tinyproxy is actually done. Incoming
 * connections have a new child created for them. The child then
 * processes the headers from the client, the response from the server,
 * and then relays the bytes between the two.
 */

#include "main.h"

#include "acl.h"
#include "anonymous.h"
#include "buffer.h"
#include "conns.h"
#include "filter.h"
#include "hashmap.h"
#include "heap.h"
#include "html-error.h"
#include "log.h"
#include "network.h"
#include "reqs.h"
#include "sock.h"
#include "stats.h"
#include "text.h"
#include "utils.h"
#include "vector.h"
#include "reverse-proxy.h"
#include "transparent-proxy.h"
#include "upstream.h"
#include "connect-ports.h"
#include "conf.h"
#ifdef __FAT_AP__
#include "util/util_instant.h"
#include "wg.h"
#include "util/wispr.h"

extern char redirect_domain[128];
extern char vc_ip[64];
#ifdef __FAT_AP__
static char factory_ssid[32] = {0};
static char factory_essid[32] = {0};
#endif
#ifndef __OEM_siemens__
extern char fb_id[128];
extern char fb_key[128];
int send_facebook_http_redirect (struct conn_s *connptr, const char *url, char *cookie, char *redirect_domain);
#endif
int send_http_redirect_timeout (struct conn_s *connptr, const char *url);
int send_wispr_http_redirect (struct conn_s *connptr, char *ip, char *mac, char *url);
inline int
get_cp_user_info(char *client_ip_addr, int client_port, swarm_info_user_t *user_out);
inline int
get_cp_profile_info(struct conn_s *connptr);
extern void ecp_proxy_add(char *cmac, char *cip, int cport, int sport, pid_t pid);
extern void ecp_proxy_remove(int sport, int pid);

inline int
get_cp_user_info(char *client_ip_addr, int client_port, swarm_info_user_t *user_out)
{
    char *cip = client_ip_addr;
    swarm_info_user_t user;
    int cp_user_ret;

    user.essid[0] = 0;
    /* Convert ipv6->ipv4 address */
    if (!strncasecmp(client_ip_addr, "::ffff:", 7))
         cip += 7;
    if (0 != (cp_user_ret = tp_acl_cp_user_get(cip, client_port, &user)) || user.essid[0] == 0) {
        error_log("tinyproxy", "%s: %d: could not find info for client %s:%d, ret=%d, essid='%s'",
            __func__, __LINE__, cip, client_port, cp_user_ret, user.essid);
        return cp_user_ret;
    }
    memcpy(user_out, &user, sizeof(*user_out));
    return 0;
}

inline int
get_cp_profile_info(struct conn_s *connptr)
{
    char cmd[256] = {0};
    char cp_type[64] = {0};
    char profile[64] = {0};
    int cp_user_ret;
    struct upstream *tmp;

    cp_user_ret = read_cp_config(connptr->user.role, "role", "captive-portal", cmd, sizeof(cmd));
    if (cp_user_ret != 0) {
        cp_user_ret = read_cp_config(connptr->user.essid, "ssid", "captive-portal", cmd, sizeof(cmd));
    }

    if (cp_user_ret != 0) {
        error_log("tinyproxy", "%s: %d: could not find info for client, ret=%d, essid='%s'",
            __func__, __LINE__,  cp_user_ret, connptr->user.essid);
        return -1;
    }
    
    if (sscanf(cmd, "%s", cp_type) < 0){
        error_log("tinyproxy", "%s: %d: could not read cp_type from file.",
            __func__, __LINE__);
        return -2;
    } 

    snprintf(profile, sizeof(profile), "%s", cmd + strlen(cp_type) + 1);

    if (strncasecmp(cp_type, "external", 8) == 0) {
        connptr->cp_type = CAPTIVE_PORTAL_EXTERNAL;
        tmp = profile_find (profile, config.auth_host);
        if (!tmp) {
            error_log("tinyproxy", "%s: %d: could not find profile '%s'",
                __func__, __LINE__, profile);
            return -3;
        }
        connptr->current_up = tmp;
    } else if (strncasecmp(cp_type, "internal", 8) == 0) {
        connptr->cp_type = CAPTIVE_PORTAL_INTERNAL;
#ifndef __OEM_siemens__
    //} else if (strncasecmp(cp_type, "facebook", 8) == 0) {
    //    connptr->cp_type = CAPTIVE_PORTAL_FACEBOOK;
#endif
    } else {
        debug_log("tinyproxy", "%s: %d: Get CP type '%s'.",
            __func__, __LINE__, cp_type);
        connptr->cp_type = CAPTIVE_PORTAL_DISABLE;
    }
    connptr->is_wispr = (strstr(cp_type, "wispr") != NULL) ? 1 : 0;
    return 0;
}

static void redirect_ecp_url(struct conn_s *connptr, const char *orig_url);
#endif

/*
 * Maximum length of a HTTP line
 */
#define HTTP_LINE_LENGTH (MAXBUFFSIZE / 6)

/*
 * Macro to help test if the Upstream proxy supported is compiled in and
 * enabled.
 */
#if defined(UPSTREAM_SUPPORT) && !defined(__FAT_AP__)
#  define UPSTREAM_CONFIGURED() (config.upstream_list != NULL)
#  define UPSTREAM_HOST(host) upstream_get(host, config.upstream_list)
#else
#  define UPSTREAM_CONFIGURED() (0)
#  define UPSTREAM_HOST(host) (NULL)
#endif

/*
 * Codify the test for the carriage return and new line characters.
 */
#define CHECK_CRLF(header, len)                                 \
  (((len) == 1 && header[0] == '\n') ||                         \
   ((len) == 2 && header[0] == '\r' && header[1] == '\n'))

/*
 * Codify the test for header fields folded over multiple lines.
 */
#define CHECK_LWS(header, len)                                  \
  ((len) > 0 && (header[0] == ' ' || header[0] == '\t'))

/*
 * Read in the first line from the client (the request line for HTTP
 * connections. The request line is allocated from the heap, but it must
 * be freed in another function.
 */
static int read_request_line (struct conn_s *connptr)
{
        ssize_t len;

retry:
#ifdef __FAT_AP__
        len = readline_w_timeout (connptr->client_fd, &connptr->request_line, 5);
#else
        len = readline (connptr->client_fd, &connptr->request_line);
#endif
        if (len <= 0) {
#ifdef __FAT_AP__
                log_message (LOG_INFO,
                             "read_request_line: Client (file descriptor: %d) "
                             "closed socket before read.", connptr->client_fd);
#else
                log_message (LOG_ERR,
                             "read_request_line: Client (file descriptor: %d) "
                             "closed socket before read.", connptr->client_fd);
#endif
                return -1;
        }

        /*
         * Strip the new line and carriage return from the string.
         */
        if (chomp (connptr->request_line, len) == len) {
                /*
                 * If the number of characters removed is the same as the
                 * length then it was a blank line. Free the buffer and
                 * try again (since we're looking for a request line.)
                 */
                safefree (connptr->request_line);
                goto retry;
        }

        log_message (LOG_CONN, "Request (file descriptor %d): %s",
                     connptr->client_fd, connptr->request_line);

        return 0;
}

/*
 * Free all the memory allocated in a request.
 */
static void free_request_struct (struct request_s *request)
{
        if (!request)
                return;

        safefree (request->method);
        safefree (request->protocol);

        if (request->host)
                safefree (request->host);
        if (request->path)
                safefree (request->path);

        safefree (request);
}

/*
 * Take a host string and if there is a username/password part, strip
 * it off.
 */
static void strip_username_password (char *host)
{
        char *p;

        assert (host);
        assert (strlen (host) > 0);

        if ((p = strchr (host, '@')) == NULL)
                return;

        /*
         * Move the pointer past the "@" and then copy from that point
         * until the NUL to the beginning of the host buffer.
         */
        p++;
        while (*p)
                *host++ = *p++;
        *host = '\0';
}

/*
 * Take a host string and if there is a port part, strip
 * it off and set proper port variable i.e. for www.host.com:8001
 */
static int strip_return_port (char *host)
{
        char *ptr1;
        int port;

        ptr1 = strchr (host, ':');
        if (ptr1 == NULL)
                return 0;

        *ptr1++ = '\0';
        if (sscanf (ptr1, "%d", &port) != 1)    /* one conversion required */
                return 0;
        return port;
}

/*
 * Pull the information out of the URL line.  This will handle both HTTP
 * and FTP (proxied) URLs.
 */
static int extract_http_url (const char *url, struct request_s *request)
{
        char *p;
        int len;
        int port;

        /* Split the URL on the slash to separate host from path */
        p = strchr (url, '/');
        if (p != NULL) {
                len = p - url;
                request->host = (char *) safemalloc (len + 1);
                memcpy (request->host, url, len);
                request->host[len] = '\0';
                request->path = safestrdup (p);
        } else {
                request->host = safestrdup (url);
                request->path = safestrdup ("/");
        }

        if (!request->host || !request->path)
                goto ERROR_EXIT;

        /* Remove the username/password if they're present */
        strip_username_password (request->host);

        /* Find a proper port in www.site.com:8001 URLs */
        port = strip_return_port (request->host);
        request->port = (port != 0) ? port : HTTP_PORT;

        return 0;

ERROR_EXIT:
        if (request->host)
                safefree (request->host);
        if (request->path)
                safefree (request->path);

        return -1;
}

/*
 * Extract the URL from a SSL connection.
 */
static int extract_ssl_url (const char *url, struct request_s *request)
{
        request->host = (char *) safemalloc (strlen (url) + 1);
        if (!request->host)
                return -1;

        if (sscanf (url, "%[^:]:%hu", request->host, &request->port) == 2) ;
        else if (sscanf (url, "%s", request->host) == 1)
                request->port = HTTP_PORT_SSL;
        else {
                log_message (LOG_ERR, "extract_ssl_url: Can't parse URL.");

                safefree (request->host);
                return -1;
        }

        /* Remove the username/password if they're present */
        strip_username_password (request->host);

        return 0;
}

/*
 * Create a connection for HTTP connections.
 */
static int
establish_http_connection (struct conn_s *connptr, struct request_s *request)
{
        char portbuff[7];

        /* Build a port string if it's not a standard port */
        if (request->port != HTTP_PORT && request->port != HTTP_PORT_SSL)
                snprintf (portbuff, 7, ":%u", request->port);
        else
                portbuff[0] = '\0';

        return write_message (connptr->server_fd,
                              "%s %s HTTP/1.0\r\n"
                              "Host: %s%s\r\n"
                              "Connection: close\r\n",
                              request->method, request->path,
                              request->host, portbuff);
}

/*
 * These two defines are for the SSL tunnelling.
 */
#define SSL_CONNECTION_RESPONSE "HTTP/1.0 200 Connection established"
#define PROXY_AGENT "Proxy-agent: " PACKAGE "/" VERSION

/*
 * Send the appropriate response to the client to establish a SSL
 * connection.
 */
static int send_ssl_response (struct conn_s *connptr)
{
        return write_message (connptr->client_fd,
                              "%s\r\n"
                              "%s\r\n"
                              "\r\n", SSL_CONNECTION_RESPONSE, PROXY_AGENT);
}

/*
 * Break the request line apart and figure out where to connect and
 * build a new request line. Finally connect to the remote server.
 */
static struct request_s *process_request (struct conn_s *connptr,
                                          hashmap_t hashofheaders)
{
        char *url;
        struct request_s *request;
        int ret;
        size_t request_len;

        /* NULL out all the fields so frees don't cause segfaults. */
        request =
            (struct request_s *) safecalloc (1, sizeof (struct request_s));
        if (!request)
                return NULL;

        request_len = strlen (connptr->request_line) + 1;

        request->method = (char *) safemalloc (request_len);
        url = (char *) safemalloc (request_len);
        request->protocol = (char *) safemalloc (request_len);

        if (!request->method || !url || !request->protocol) {
                goto fail;
        }

        ret = sscanf (connptr->request_line, "%[^ ] %[^ ] %[^ ]",
                      request->method, url, request->protocol);
        if (ret == 2 && !strcasecmp (request->method, "GET")) {
                request->protocol[0] = 0;

                /* Indicate that this is a HTTP/0.9 GET request */
                connptr->protocol.major = 0;
                connptr->protocol.minor = 9;
        } else if (ret == 3 && !strncasecmp (request->protocol, "HTTP/", 5)) {
                /*
                 * Break apart the protocol and update the connection
                 * structure.
                 */
                ret = sscanf (request->protocol + 5, "%u.%u",
                              &connptr->protocol.major,
                              &connptr->protocol.minor);

                /*
                 * If the conversion doesn't succeed, drop down below and
                 * send the error to the user.
                 */
                if (ret != 2)
                        goto BAD_REQUEST_ERROR;
        } else {
BAD_REQUEST_ERROR:
                log_message (LOG_ERR,
                             "process_request: Bad Request on file descriptor %d",
                             connptr->client_fd);
                indicate_http_error (connptr, 400, "Bad Request",
                                     "detail", "Request has an invalid format",
                                     "url", url, NULL);
                goto fail;
        }

        if (!url) {
                log_message (LOG_ERR,
                             "process_request: Null URL on file descriptor %d",
                             connptr->client_fd);
                indicate_http_error (connptr, 400, "Bad Request",
                                     "detail", "Request has an empty URL",
                                     "url", url, NULL);
                goto fail;
        }
#ifdef REVERSE_SUPPORT
        if (config.reversepath_list != NULL) {
                /*
                 * Rewrite the URL based on the reverse path.  After calling
                 * reverse_rewrite_url "url" can be freed since we either
                 * have the newly rewritten URL, or something failed and
                 * we'll be closing anyway.
                 */
                char *reverse_url;

                reverse_url = reverse_rewrite_url (connptr, hashofheaders, url);

                if (!reverse_url) {
                        goto fail;
                }

                safefree (url);
                url = reverse_url;
        }
#endif

        if (strncasecmp (url, "http://", 7) == 0
            || (UPSTREAM_CONFIGURED () && strncasecmp (url, "ftp://", 6) == 0))
        {
                char *skipped_type = strstr (url, "//") + 2;

                if (extract_http_url (skipped_type, request) < 0) {
                        indicate_http_error (connptr, 400, "Bad Request",
                                             "detail", "Could not parse URL",
                                             "url", url, NULL);
                        goto fail;
                }
        } else if (strcmp (request->method, "CONNECT") == 0) {
                if (extract_ssl_url (url, request) < 0) {
                        indicate_http_error (connptr, 400, "Bad Request",
                                             "detail", "Could not parse URL",
                                             "url", url, NULL);
                        goto fail;
                }

                /* Verify that the port in the CONNECT method is allowed */
                if (!check_allowed_connect_ports (request->port,
                                                  config.connect_ports))
                {
                        indicate_http_error (connptr, 403, "Access violation",
                                             "detail",
                                             "The CONNECT method not allowed "
                                             "with the port you tried to use.",
                                             "url", url, NULL);
                        log_message (LOG_INFO,
                                     "Refused CONNECT method on port %d",
                                     request->port);
                        goto fail;
                }

                connptr->connect_method = TRUE;
        } else {
#ifdef TRANSPARENT_PROXY
                if (!do_transparent_proxy
                    (connptr, hashofheaders, request, &config, &url)) {
                        goto fail;
                }
#else
                indicate_http_error (connptr, 501, "Not Implemented",
                                     "detail",
                                     "Unknown method or unsupported protocol.",
                                     "url", url, NULL);
                log_message (LOG_INFO, "Unknown method (%s) or protocol (%s)",
                             request->method, url);
                goto fail;
#endif
        }

#ifdef FILTER_ENABLE
        /*
         * Filter restricted domains/urls
         */
        if (config.filter) {
                if (config.filter_url)
                        ret = filter_url (url);
                else
                        ret = filter_domain (request->host);

                if (ret) {
                        update_stats (STAT_DENIED);

                        if (config.filter_url)
                                log_message (LOG_NOTICE,
                                             "Proxying refused on filtered url \"%s\"",
                                             url);
                        else
                                log_message (LOG_NOTICE,
                                             "Proxying refused on filtered domain \"%s\"",
                                             request->host);

                        indicate_http_error (connptr, 403, "Filtered",
                                             "detail",
                                             "The request you made has been filtered",
                                             "url", url, NULL);
                        goto fail;
                }
        }
#endif

        /*
         * Check to see if they're requesting the stat host
         */
        if (config.stathost && strcmp (config.stathost, request->host) == 0) {
                log_message (LOG_NOTICE, "Request for the stathost.");
                connptr->show_stats = TRUE;
                goto fail;
        }


        safefree (url);

        return request;

fail:
        safefree (url);
        free_request_struct (request);
        return NULL;
}

/*
 * pull_client_data is used to pull across any client data (like in a
 * POST) which needs to be handled before an error can be reported, or
 * server headers can be processed.
 *	- rjkaes
 */
static int pull_client_data (struct conn_s *connptr, long int length)
{
        char *buffer;
        ssize_t len;

        buffer =
            (char *) safemalloc (min (MAXBUFFSIZE, (unsigned long int) length));
        if (!buffer)
                return -1;

        do {
#ifdef __FAT_AP__
                len = read_w_timeout (connptr->client_fd, buffer,
                                 min (MAXBUFFSIZE, (unsigned long int) length), 5);
#else
                len = safe_read (connptr->client_fd, buffer,
                                 min (MAXBUFFSIZE, (unsigned long int) length));
#endif

                if (len <= 0)
                        goto ERROR_EXIT;

                if (!connptr->error_variables) {
                        if (safe_write (connptr->server_fd, buffer, len) < 0)
                                goto ERROR_EXIT;
                }

                length -= len;
        } while (length > 0);

        /*
         * BUG FIX: Internet Explorer will leave two bytes (carriage
         * return and line feed) at the end of a POST message.  These
         * need to be eaten for tinyproxy to work correctly.
         */
        socket_nonblocking (connptr->client_fd);
        len = recv (connptr->client_fd, buffer, 2, MSG_PEEK);
        socket_blocking (connptr->client_fd);

        if (len < 0 && errno != EAGAIN)
                goto ERROR_EXIT;

        if ((len == 2) && CHECK_CRLF (buffer, len)) {
                ssize_t ret;

                ret = read (connptr->client_fd, buffer, 2);
                if (ret == -1) {
                        log_message
                                (LOG_WARNING,
                                 "Could not read two bytes from POST message");
                }
        }

        safefree (buffer);
        return 0;

ERROR_EXIT:
        safefree (buffer);
        return -1;
}

#ifdef XTINYPROXY_ENABLE
/*
 * Add the X-Tinyproxy header to the collection of headers being sent to
 * the server.
 *	-rjkaes
 */
static int add_xtinyproxy_header (struct conn_s *connptr)
{
        assert (connptr && connptr->server_fd >= 0);
        return write_message (connptr->server_fd,
                              "X-Tinyproxy: %s\r\n", connptr->client_ip_addr);
}
#endif /* XTINYPROXY */

#ifdef __FAT_AP__
static int add_xff_header (struct conn_s *connptr)
{
    struct in_addr in;

    assert (connptr && connptr->server_fd >= 0);
    in.s_addr = connptr->user.v4_ip;
    return write_message (connptr->server_fd,
                          "X-Forwarded-For: %s\r\n", inet_ntoa(in));
}
#endif

/*
 * Take a complete header line and break it apart (into a key and the data.)
 * Now insert this information into the hashmap for the connection so it
 * can be retrieved and manipulated later.
 */
static int
add_header_to_connection (hashmap_t hashofheaders, char *header, size_t len)
{
        char *sep;

        /* Get rid of the new line and return at the end */
        len -= chomp (header, len);

        sep = strchr (header, ':');
        if (!sep)
                return -1;

        /* Blank out colons, spaces, and tabs. */
        while (*sep == ':' || *sep == ' ' || *sep == '\t')
                *sep++ = '\0';

        /* Calculate the new length of just the data */
        len -= sep - header - 1;

        return hashmap_insert (hashofheaders, header, sep, len);
}

/*
 * Read all the headers from the stream
 */
static int get_all_headers (int fd, hashmap_t hashofheaders)
{
        char *line = NULL;
        char *header = NULL;
        char *tmp;
        ssize_t linelen;
        ssize_t len = 0;
        unsigned int double_cgi = FALSE;        /* boolean */

        assert (fd >= 0);
        assert (hashofheaders != NULL);

        for (;;) {
#ifdef __FAT_AP__
                if ((linelen = readline_w_timeout (fd, &line, 5)) <= 0)
#else
                if ((linelen = readline (fd, &line)) <= 0)
#endif
                {
                        safefree (header);
                        safefree (line);
                        return -1;
                }

                /*
                 * If we received a CR LF or a non-continuation line, then add
                 * the accumulated header field, if any, to the hashmap, and
                 * reset it.
                 */
                if (CHECK_CRLF (line, linelen) || !CHECK_LWS (line, linelen)) {
                        if (!double_cgi
                            && len > 0
                            && add_header_to_connection (hashofheaders, header,
                                                         len) < 0) {
                                safefree (header);
                                safefree (line);
                                return -1;
                        }

                        len = 0;
                }

                /*
                 * If we received just a CR LF on a line, the headers are
                 * finished.
                 */
                if (CHECK_CRLF (line, linelen)) {
                        safefree (header);
                        safefree (line);
                        return 0;
                }

                /*
                 * BUG FIX: The following code detects a "Double CGI"
                 * situation so that we can handle the nonconforming system.
                 * This problem was found when accessing cgi.ebay.com, and it
                 * turns out to be a wider spread problem as well.
                 *
                 * If "Double CGI" is in effect, duplicate headers are
                 * ignored.
                 *
                 * FIXME: Might need to change this to a more robust check.
                 */
                if (linelen >= 5 && strncasecmp (line, "HTTP/", 5) == 0) {
                        double_cgi = TRUE;
                }

                /*
                 * Append the new line to the current header field.
                 */
                tmp = (char *) saferealloc (header, len + linelen);
                if (tmp == NULL) {
                        safefree (header);
                        safefree (line);
                        return -1;
                }

                header = tmp;
                memcpy (header + len, line, linelen);
                len += linelen;

                safefree (line);
        }
}

/*
 * Extract the headers to remove.  These headers were listed in the Connection
 * and Proxy-Connection headers.
 */
static int remove_connection_headers (hashmap_t hashofheaders)
{
        static const char *headers[] = {
                "connection",
                "proxy-connection"
        };

        char *data;
        char *ptr;
        ssize_t len;
        int i;

        for (i = 0; i != (sizeof (headers) / sizeof (char *)); ++i) {
                /* Look for the connection header.  If it's not found, return. */
                len =
                    hashmap_entry_by_key (hashofheaders, headers[i],
                                          (void **) &data);
                if (len <= 0)
                        return 0;

                /*
                 * Go through the data line and replace any special characters
                 * with a NULL.
                 */
                ptr = data;
                while ((ptr = strpbrk (ptr, "()<>@,;:\\\"/[]?={} \t")))
                        *ptr++ = '\0';

                /*
                 * All the tokens are separated by NULLs.  Now go through the
                 * token and remove them from the hashofheaders.
                 */
                ptr = data;
                while (ptr < data + len) {
                        hashmap_remove (hashofheaders, ptr);

                        /* Advance ptr to the next token */
                        ptr += strlen (ptr) + 1;
                        while (ptr < data + len && *ptr == '\0')
                                ptr++;
                }

                /* Now remove the connection header it self. */
                hashmap_remove (hashofheaders, headers[i]);
        }

        return 0;
}

/*
 * If there is a Content-Length header, then return the value; otherwise, return
 * a negative number.
 */
static long get_content_length (hashmap_t hashofheaders)
{
        ssize_t len;
        char *data;
        long content_length = -1;

        len =
            hashmap_entry_by_key (hashofheaders, "content-length",
                                  (void **) &data);
        if (len > 0)
                content_length = atol (data);

        return content_length;
}

/*
 * Search for Via header in a hash of headers and either write a new Via
 * header, or append our information to the end of an existing Via header.
 *
 * FIXME: Need to add code to "hide" our internal information for security
 * purposes.
 */
static int
write_via_header (int fd, hashmap_t hashofheaders,
                  unsigned int major, unsigned int minor)
{
        ssize_t len;
        char hostname[512];
        char *data;
        int ret;

        if (config.disable_viaheader) {
                ret = 0;
                goto done;
        }

        if (config.via_proxy_name) {
                strlcpy (hostname, config.via_proxy_name, sizeof (hostname));
        } else if (gethostname (hostname, sizeof (hostname)) < 0) {
                strlcpy (hostname, "unknown", 512);
        }

        /*
         * See if there is a "Via" header.  If so, again we need to do a bit
         * of processing.
         */
        len = hashmap_entry_by_key (hashofheaders, "via", (void **) &data);
        if (len > 0) {
                ret = write_message (fd,
                                     "Via: %s, %hu.%hu %s (%s/%s)\r\n",
                                     data, major, minor, hostname, PACKAGE,
                                     VERSION);

                hashmap_remove (hashofheaders, "via");
        } else {
                ret = write_message (fd,
                                     "Via: %hu.%hu %s (%s/%s)\r\n",
                                     major, minor, hostname, PACKAGE, VERSION);
        }

done:
        return ret;
}

#ifdef __FAT_AP__
/*
 * Extra headers (before sending to our own HTTPD)
 */
static int
write_aruba_headers (struct conn_s *connptr)
{
        int ret;
        char *cip = connptr->client_ip_addr;

        /* pass remote client info to https for user lookup */

        /* Convert ipv6->ipv4 address */
      	if (!strncasecmp(connptr->client_ip_addr, "::ffff:", 7))
            cip += 7;
        ret = write_message (connptr->server_fd, 
                             "X-Remote: %s:%d\r\n",
                             cip, connptr->client_port);
        return ret;
}
#endif

/*
 * Number of buckets to use internally in the hashmap.
 */
#define HEADER_BUCKETS 32

/*
 * Here we loop through all the headers the client is sending. If we
 * are running in anonymous mode, we will _only_ send the headers listed
 * (plus a few which are required for various methods).
 *	- rjkaes
 */
static int
process_client_headers (struct conn_s *connptr, hashmap_t hashofheaders)
{
        static const char *skipheaders[] = {
                "host",
                "keep-alive",
                "proxy-connection",
                "te",
                "trailers",
                "accept-encoding",
                "upgrade",
                "referer"
        };
#ifdef __FAT_AP__
        static const char *ecp_skipheaders[] = {
                "accept",
                "accept-charset",
                "accept-language",
                "accept-datetime"
        };
#endif
        int i;
        hashmap_iter iter;
        int ret = 0;

        char *data, *header;

        /*
         * Don't send headers if there's already an error, if the request was
         * a stats request, or if this was a CONNECT method (unless upstream
         * proxy is in use.)
         */
        if (connptr->server_fd == -1 || connptr->show_stats
            || (connptr->connect_method && (connptr->upstream_proxy == NULL))) {
                log_message (LOG_INFO,
                             "Not sending client headers to remote machine");
                return 0;
        }

        /*
         * See if there is a "Content-Length" header.  If so, again we need
         * to do a bit of processing.
         */
        connptr->content_length.client = get_content_length (hashofheaders);

        /*
         * See if there is a "Connection" header.  If so, we need to do a bit
         * of processing. :)
         */
        remove_connection_headers (hashofheaders);

        /*
         * Delete the headers listed in the skipheaders list
         */
        for (i = 0; i != (sizeof (skipheaders) / sizeof (char *)); i++) {
                hashmap_remove (hashofheaders, skipheaders[i]);
        }

#ifdef __FAT_AP__
        if (connptr->mangle_ecp_headers) {
            for (i = 0; i != (sizeof (ecp_skipheaders) / sizeof (char *)); i++) {
                hashmap_remove (hashofheaders, ecp_skipheaders[i]);
            }
        }
#endif

        /* Send, or add the Via header */
        ret = write_via_header (connptr->server_fd, hashofheaders,
                                connptr->protocol.major,
                                connptr->protocol.minor);
        if (ret < 0) {
                indicate_http_error (connptr, 503,
                                     "Could not send data to remote server",
                                     "detail",
                                     "A network error occurred while "
                                     "trying to write data to the remote web server.",
                                     NULL);
                goto PULL_CLIENT_DATA;
        }

#ifdef __FAT_AP__
        ret = write_aruba_headers (connptr);
        if (ret < 0) {
                indicate_http_error (connptr, 503,
                                     "Could not send remote client info to remote server",
                                     "detail",
                                     "A network error occurred while "
                                     "trying to write data to the remote web server.",
                                     NULL);
                goto PULL_CLIENT_DATA;
        }
#endif 


        /*
         * Output all the remaining headers to the remote machine.
         */
        iter = hashmap_first (hashofheaders);
        if (iter >= 0) {
                for (; !hashmap_is_end (hashofheaders, iter); ++iter) {
                        hashmap_return_entry (hashofheaders,
                                              iter, &data, (void **) &header);

                        if (!is_anonymous_enabled ()
                            || anonymous_search (data) > 0) {
                                ret =
                                    write_message (connptr->server_fd,
                                                   "%s: %s\r\n", data, header);
                                if (ret < 0) {
                                        indicate_http_error (connptr, 503,
                                                             "Could not send data to remote server",
                                                             "detail",
                                                             "A network error occurred while "
                                                             "trying to write data to the "
                                                             "remote web server.",
                                                             NULL);
                                        goto PULL_CLIENT_DATA;
                                }
                        }
                }
        }
#if defined(XTINYPROXY_ENABLE)
        if (config.add_xtinyproxy)
                add_xtinyproxy_header (connptr);
#endif

#ifdef __FAT_AP__
        add_xff_header (connptr);
#endif
        /* Write the final "blank" line to signify the end of the headers */
        if (safe_write (connptr->server_fd, "\r\n", 2) < 0)
                return -1;

        /*
         * Spin here pulling the data from the client.
         */
PULL_CLIENT_DATA:
        if (connptr->content_length.client > 0) {
                ret = pull_client_data (connptr,
                                        connptr->content_length.client);
        }

        return ret;
}

/*
 * Loop through all the headers (including the response code) from the
 * server.
 */
#ifdef __FAT_AP__
static int process_server_headers (struct conn_s *connptr, int in_white_list)
#else
static int process_server_headers (struct conn_s *connptr)
#endif
{
        static const char *skipheaders[] = {
                "keep-alive",
                "proxy-authenticate",
                "proxy-authorization",
                "proxy-connection",
        };

        char *response_line;

        hashmap_t hashofheaders;
        hashmap_iter iter;
        char *data, *header;
        ssize_t len;
        int i;
        int ret;
        int resp_302 = 0;

#ifdef REVERSE_SUPPORT
        struct reversepath *reverse = config.reversepath_list;
#endif

        /* Get the response line from the remote server. */
retry:
#ifdef __FAT_AP__
        len = readline_w_timeout (connptr->server_fd, &response_line, 5);
#else
        len = readline (connptr->server_fd, &response_line);
#endif
        if (len <= 0)
                return -1;

        /*
         * Strip the new line and character return from the string.
         */
        if (chomp (response_line, len) == len) {
                /*
                 * If the number of characters removed is the same as the
                 * length then it was a blank line. Free the buffer and
                 * try again (since we're looking for a request line.)
                 */
                safefree (response_line);
                goto retry;
        }

        /* Do not set ECP session cookie for a new session if response 
         * is redirect. This is to work around IE which continues to go 
         * to original destination if there is a cert warning.
         */
        if (strstr(response_line, "302") != NULL)
            resp_302 = 1;

        hashofheaders = hashmap_create (HEADER_BUCKETS);
        if (!hashofheaders) {
                safefree (response_line);
                return -1;
        }

        /*
         * Get all the headers from the remote server in a big hash
         */
        if (get_all_headers (connptr->server_fd, hashofheaders) < 0) {
                log_message (LOG_WARNING,
                             "Could not retrieve all the headers from the remote server.");
                hashmap_delete (hashofheaders);
                safefree (response_line);

                indicate_http_error (connptr, 503,
                                     "Could not retrieve all the headers",
                                     "detail",
                                     PACKAGE_NAME " "
                                     "was unable to retrieve and process headers from "
                                     "the remote web server.", NULL);
                return -1;
        }

        /*
         * At this point we've received the response line and all the
         * headers.  However, if this is a simple HTTP/0.9 request we
         * CAN NOT send any of that information back to the client.
         * Instead we'll free all the memory and return.
         */
        if (connptr->protocol.major < 1) {
                hashmap_delete (hashofheaders);
                safefree (response_line);
                return 0;
        }

        /* Send the saved response line first */
        ret = write_message (connptr->client_fd, "%s\r\n", response_line);
        safefree (response_line);
        if (ret < 0)
                goto ERROR_EXIT;

        /*
         * If there is a "Content-Length" header, retrieve the information
         * from it for later use.
         */
        connptr->content_length.server = get_content_length (hashofheaders);

        /*
         * See if there is a connection header.  If so, we need to to a bit of
         * processing.
         */
        remove_connection_headers (hashofheaders);

#ifdef __FAT_AP__ 
        {
            char *val;
            int val_len;
            if (hashmap_entry_by_key (hashofheaders, "content-type", (void **) &val) > 0) {
                if (strcasestr(val, "html") != NULL) {
                    if (!in_white_list) {
                        connptr->insert_doc_base = 1;
                        hashmap_remove(hashofheaders, "content-length");
                        if (hashmap_entry_by_key (hashofheaders, "transfer-encoding", (void **) &val) > 0) {
                            debug_log("tinyproxy", "%s: %d: transfer-encoding='%s'", __func__, __LINE__, val);
                            if (strncasecmp(val, "chunked", 7) == 0) {
                                connptr->filter_chunk_encoding = 1;
                            }
                            hashmap_remove(hashofheaders, "transfer-encoding");
                            debug_log("tinyproxy", "%s: %d: filter_chunk_encoding=%d", __func__, __LINE__, connptr->filter_chunk_encoding);
                        }
                    }
                } else {
                    connptr->build_auto_wl = BUILD_AUTO_WL_NONE;
                    debug_log("tinyproxy", "%s: %d: not building auto whitelist due to content-type being '%s'",
                        __func__, __LINE__, val);
                }
            }

            if (resp_302 && !in_white_list && config.auth_host
             && (val_len = hashmap_entry_by_key (hashofheaders, "location", (void **) &val)) > 0) {
                int ll =  strspn(val, " \t\r\n");
                val += ll;
                val_len -= ll;
                if (*val == '/') {
                    int new_len = val_len + 7 + strlen(config.auth_host->host);
                    char *full_url = (char *)(safemalloc(new_len));
                    if (full_url) {
                        char *s = full_url + sprintf(full_url, "http://%s", config.auth_host->host);
                        memcpy(s, val, val_len);
                        hashmap_remove(hashofheaders, "location");
                        hashmap_insert(hashofheaders, "location", full_url, new_len);
                        debug_log("tinyproxy", "%s: %d: rewrote redirect url='%.*s' new_len=%d",
                            __func__, __LINE__, new_len, full_url, new_len);
                        safefree(full_url);
                    }
                }
            }
        }
#endif

        /*
         * Delete the headers listed in the skipheaders list
         */
        for (i = 0; i != (sizeof (skipheaders) / sizeof (char *)); i++) {
                hashmap_remove (hashofheaders, skipheaders[i]);
        }

        /* Send, or add the Via header */
        ret = write_via_header (connptr->client_fd, hashofheaders,
                                connptr->protocol.major,
                                connptr->protocol.minor);

        if (ret < 0)
                goto ERROR_EXIT;

#ifdef __FAT_AP__
        if (connptr->mangle_ecp_headers) {
            write_message (connptr->client_fd, 
                             "Cache-Control: no-cache,no-store,must-revalidate,post-check=0,pre-check=0\r\n");
            hashmap_remove (hashofheaders, "cache-control");
        }
#endif
#ifdef REVERSE_SUPPORT
        /* Write tracking cookie for the magical reverse proxy path hack */
        if (config.reversemagic && connptr->reversepath) {
                ret = write_message (connptr->client_fd,
                                     "Set-Cookie: " REVERSE_COOKIE
                                     "=%s; path=/\r\n", connptr->reversepath);
                if (ret < 0)
                        goto ERROR_EXIT;
        }

        /* Rewrite the HTTP redirect if needed */
        if (config.reversebaseurl &&
            hashmap_entry_by_key (hashofheaders, "location",
                                  (void **) &header) > 0) {

                /* Look for a matching entry in the reversepath list */
                while (reverse) {
                        if (strncasecmp (header,
                                         reverse->url, (len =
                                                        strlen (reverse->
                                                                url))) == 0)
                                break;
                        reverse = reverse->next;
                }

                if (reverse) {
                        ret =
                            write_message (connptr->client_fd,
                                           "Location: %s%s%s\r\n",
                                           config.reversebaseurl,
                                           (reverse->path + 1), (header + len));
                        if (ret < 0)
                                goto ERROR_EXIT;

                        log_message (LOG_INFO,
                                     "Rewriting HTTP redirect: %s -> %s%s%s",
                                     header, config.reversebaseurl,
                                     (reverse->path + 1), (header + len));
                        hashmap_remove (hashofheaders, "location");
                }
        }
#endif

        /*
         * All right, output all the remaining headers to the client.
         */
        iter = hashmap_first (hashofheaders);
        if (iter >= 0) {
                for (; !hashmap_is_end (hashofheaders, iter); ++iter) {
                        hashmap_return_entry (hashofheaders,
                                              iter, &data, (void **) &header);

                        ret = write_message (connptr->client_fd,
                                             "%s: %s\r\n", data, header);
                        if (ret < 0)
                                goto ERROR_EXIT;
                }
        }
        hashmap_delete (hashofheaders);

        /* Write the final blank line to signify the end of the headers */
        if (safe_write (connptr->client_fd, "\r\n", 2) < 0)
                return -1;

        return 0;

ERROR_EXIT:
        hashmap_delete (hashofheaders);
        return -1;
}

#ifdef __FAT_AP__
static int
authenticate_user (struct conn_s *connptr)
{
    int ret;
    if ((ret = authenticate_client("tinyproxy", &(connptr->user), connptr->client_ip_addr, connptr->client_port, NULL)) == 0) {
        connptr->user_authenticated = 1;
    }
    return ret;
}

static void auto_insert_doc_base (struct bufline_s *bl, struct buffer_s *buf, char *doc_base, char *insert_doc_base)
{
    char *s;
    if (!doc_base || !(*doc_base) || !insert_doc_base || !(*insert_doc_base)) {
        return;
    }
    if ((s = strcasestr((char *)(bl->string), "<base")) != NULL) {
        char tmp[256];
        s += 5;
        strlcpy(tmp, s, sizeof(tmp));
        s = strchr(tmp, '>');
        if (s) {
            *s = 0;
        }

        if (isspace(tmp[0]) && strcasestr(tmp, "href") != NULL) {
            /* HTML already contains base href, do not auto insert */
            debug_log("tinyproxy", "%s: %d: HTML already contain base href, no need to insert",
                __func__, __LINE__);
            *insert_doc_base = 0;
            return;
        }
    }
    s = strcasestr((char *)(bl->string), "<head>");
    if (!s) {
        return;
    }
    if (0 == insert_into_line(bl, buf, s + 6 - (char *)(bl->string), doc_base)) {
        debug_log("tinyproxy", "%s: %d: inserted base href into HTML",
            __func__, __LINE__);
        *insert_doc_base = 0;
    }
}
#endif

static void build_auto_wl (struct conn_s *connptr, char *resp)
{
    int len;
        char *p = NULL, *q = NULL;
        char domain[SIZE_WG_NAME];
        char prefix[18];
        char * orig_resp;

        /* only process eligible responses */
        if (connptr->build_auto_wl == BUILD_AUTO_WL_NONE) 
            return;

        orig_resp = resp;
        /* there is no https */
        while ((p = strcasestr(resp, "http://")) != NULL) {
            if (p - resp > (sizeof (prefix) - 1))
                q = p - sizeof (prefix) + 1;
            else
                q = resp;

            memcpy (prefix, q, p - q);
            prefix[p - q] = 0;

            resp = p + 7;

            if (strcasestr (prefix, "href")) {
                if (connptr->build_auto_wl != BUILD_AUTO_WL_ALL) {
                    continue;
                }
            } else if (!strcasestr (prefix, "src") && !strcasestr (prefix, "action")) {
                continue;
            }

            /* extract domain */
            if ((len = strcspn(resp, " \t\"\\\n?'/"))) {
                if (len < (int) sizeof(domain)) {
                    strncpy(domain, resp, len);
                    domain[len] = '\0';
                    auto_wg_add_wl(domain);
                }
                resp += len;
            }
        }
}

/*
 * Switch the sockets into nonblocking mode and begin relaying the bytes
 * between the two connections. We continue to use the buffering code
 * since we want to be able to buffer a certain amount for slower
 * connections (as this was the reason why I originally modified
 * tinyproxy oh so long ago...)
 *	- rjkaes
 */
#ifndef __FAT_AP__
static void relay_connection (struct conn_s *connptr)
#else
static int relay_connection (struct conn_s *connptr)
#endif
{
        fd_set rset, wset;
        struct timeval tv;
        time_t last_access;
        int ret;
        double tdiff;
        int maxfd = max (connptr->client_fd, connptr->server_fd) + 1;
        ssize_t bytes_received;

#ifdef __FAT_AP__
        int token_state = INIT;
        int ret_val = -1;
#endif

        socket_nonblocking (connptr->client_fd);
        socket_nonblocking (connptr->server_fd);

        last_access = time (NULL);

        for (;;) {
                FD_ZERO (&rset);
                FD_ZERO (&wset);

                tv.tv_sec =
                    config.idletimeout - difftime (time (NULL), last_access);
                tv.tv_usec = 0;

                if (buffer_size (connptr->sbuffer) > 0)
                        FD_SET (connptr->client_fd, &wset);
                if (buffer_size (connptr->cbuffer) > 0)
                        FD_SET (connptr->server_fd, &wset);
                if (buffer_size (connptr->sbuffer) < MAXBUFFSIZE)
                        FD_SET (connptr->server_fd, &rset);
                if (buffer_size (connptr->cbuffer) < MAXBUFFSIZE)
                        FD_SET (connptr->client_fd, &rset);

                ret = select (maxfd, &rset, &wset, NULL, &tv);

                if (ret == 0) {
                        tdiff = difftime (time (NULL), last_access);
                        if (tdiff > config.idletimeout) {
                                log_message (LOG_INFO,
                                             "Idle Timeout (after select) as %g > %u.",
                                             tdiff, config.idletimeout);
#ifdef __FAT_AP__
                                return ret_val;
#else
                                return;
#endif
                        } else {
                                continue;
                        }
                } else if (ret < 0) {
                        log_message (LOG_ERR,
                                     "relay_connection: select() error \"%s\". "
                                     "Closing connection (client_fd:%d, server_fd:%d)",
                                     strerror (errno), connptr->client_fd,
                                     connptr->server_fd);
#ifdef __FAT_AP__
                        return ret_val;
#else
                        return;
#endif
                } else {
                        /*
                         * All right, something was actually selected so mark it.
                         */
                        last_access = time (NULL);
                }

                if (FD_ISSET (connptr->server_fd, &rset)) {
                        bytes_received =
#ifdef __FAT_AP__
                            read_buffer_from_server (connptr->server_fd, connptr->sbuffer, connptr->filter_chunk_encoding, &token_state);
#else
                            read_buffer (connptr->server_fd, connptr->sbuffer);
#endif
                        if (bytes_received < 0)
                                break;
                        connptr->content_length.server -= bytes_received;
                        if (connptr->content_length.server == 0)
                                break;
                }
                if (FD_ISSET (connptr->client_fd, &rset)) {
                    if (read_buffer (connptr->client_fd, connptr->cbuffer) < 0)
                        break;
                }
                if (FD_ISSET (connptr->server_fd, &wset)) {
                    if (write_buffer (connptr->server_fd, connptr->cbuffer) < 0)
                        break;
                }
                if (FD_ISSET (connptr->client_fd, &wset)) {
                    struct bufline_s * bl;
                    
                    if (!connptr->connect_method){/*won't do this for HTTPS proxy*/
                    bl = connptr->sbuffer->head;
                    while (bl) {
                        /* check for authentication here. TBD */
                        if (
#ifdef __FAT_AP__
                            !connptr->user_authenticated &&
                            connptr->cp_type == CAPTIVE_PORTAL_EXTERNAL && connptr->current_up != NULL &&
#endif
                            (connptr->current_up->login_success_string) && connptr->current_up->login_success_string[0]) {
                            if (strcasestr ((const char *) bl->string, connptr->current_up->login_success_string))
#ifdef __FAT_AP__
                                if (authenticate_user (connptr) != -1)
                                    ret_val = 0;
#else
                                authenticate_user (connptr);
#endif
                        }
                        auto_insert_doc_base(bl, connptr->sbuffer, connptr->doc_base, &connptr->insert_doc_base);
#ifdef __FAT_AP__
                        if (connptr->cp_type == CAPTIVE_PORTAL_EXTERNAL && connptr->current_up != NULL &&
                            connptr->current_up->autowhitelist)/*This line is used for next line, please note*/
#endif
                        build_auto_wl(connptr, (char *) bl->string);
                            
                        bl = bl->next;
                    }
                    }
                    if (write_buffer (connptr->client_fd, connptr->sbuffer) < 0)
                        break;
                }
        }

        /*
         * Here the server has closed the connection... write the
         * remainder to the client and then exit.
         */
        socket_blocking (connptr->client_fd);
        while (buffer_size (connptr->sbuffer) > 0) {
            struct bufline_s * bl;
            
            bl = connptr->sbuffer->head;
            while (bl) {
                /* check for authentication here. TBD */
                if (
#ifdef __FAT_AP__
                    !connptr->user_authenticated &&
                    connptr->cp_type == CAPTIVE_PORTAL_EXTERNAL && connptr->current_up != NULL &&
#endif
                    connptr->current_up->login_success_string && connptr->current_up->login_success_string[0]) {
                    if (strcasestr ((const char *) bl->string, connptr->current_up->login_success_string))
#ifdef __FAT_AP__
                        if (authenticate_user (connptr) != -1)
                            ret_val = 0;
#else
                        authenticate_user (connptr);
#endif
                }
                
                auto_insert_doc_base(bl, connptr->sbuffer, connptr->doc_base, &connptr->insert_doc_base);
#ifdef __FAT_AP__
                if (connptr->cp_type == CAPTIVE_PORTAL_EXTERNAL && connptr->current_up != NULL &&
                    connptr->current_up->autowhitelist)/*This line is used for next line, please note*/
#endif
                build_auto_wl(connptr, (char *) bl->string);
                bl = bl->next;
            }
            if (write_buffer (connptr->client_fd, connptr->sbuffer) < 0)
                break;
        }
        shutdown (connptr->client_fd, SHUT_WR);

        /*
         * Try to send any remaining data to the server if we can.
         */
        socket_blocking (connptr->server_fd);
        while (buffer_size (connptr->cbuffer) > 0) {
                if (write_buffer (connptr->server_fd, connptr->cbuffer) < 0)
                        break;
        }
#ifdef __FAT_AP__
        return ret_val;
#else
        return;
#endif
}

#ifdef __FAT_AP__
#define SWARM_WIFI_FACTORY_SSID        "/proc/sys/net/fw/factory_ssid"
#define SWARM_WIFI_FACTORY_ESSID       "/proc/sys/net/fw/factory_essid"
//return result of oem_get_instantfactoryname, for example instant
int factory_ssid_read()
{
    FILE *fp;

    if(factory_ssid[0])
        return 0;

    memset(factory_ssid, 0, sizeof(factory_ssid));
    fp = fopen(SWARM_WIFI_FACTORY_SSID, "r");
    if (!fp) {
        return 1;
    }

    fscanf(fp, "%s", factory_ssid);
    fclose (fp);
    return 0;
}
//return result as instant-ddeeff
int factory_essid_read()
{
    FILE *fp;
    
    if(factory_essid[0])
        return 0;

    memset(factory_essid, 0, sizeof(factory_essid));
    fp = fopen(SWARM_WIFI_FACTORY_ESSID, "r");
    if (!fp) {
        factory_ssid_read();
        strlcpy(factory_essid, factory_ssid, sizeof(factory_essid));
        return 0;
    }

    fscanf(fp, "%s", factory_essid);
    fclose (fp);
    return 0;
}
#endif

static int
connect_to_ecp (struct conn_s *connptr, struct request_s *request)
{
    char param_string[512];
    struct upstream *cur_upstream = connptr->current_up;
#ifdef __FAT_AP__
	const char *iap_param = "%s%ccmd=login&mac=%s&essid=%s&ip=%s&apname=%s&apmac=%s&vcname=%s&switchip=%s&url=%s";
    char orig_url[4096];
    char *encoded_ssid;
    char cp_url[1024] = {0};
    char *encoded_url;
    char *encoded_apname;
    char *encoded_apmac;
    char *encoded_vcname;
    int ret = -1;
    char  ecp_server[256] = "";

    {
        char cmd[256];
        char default_wired_ssid[128] = {0};
    
        factory_ssid_read();//instant
        //If factory ssid include $APMAC3, factory essid expand is instant--ddeeff
        factory_essid_read();

        snprintf(default_wired_ssid, sizeof(default_wired_ssid), "wired-%s", factory_ssid);

        if (connptr->cp_type == CAPTIVE_PORTAL_EXTERNAL ) {
            snprintf(ecp_server, sizeof(ecp_server), "%s:%d", cur_upstream->host, cur_upstream->port);
        }

        /* NATTed user */
#ifdef __EXPERIMENTAL_BUILD__
        if (strncmp(connptr->user.essid, "instant-", 8) == 0)
#else
        if (strcmp(connptr->user.essid, factory_essid) == 0)
#endif
        {
            error_log("tinyproxy", "%s, %d: User ssid is %s, factory ssid is %s", __func__, __LINE__, connptr->user.essid, factory_ssid);
local:
        /* Factory SSID */
#ifndef __OEM_siemens__
            snprintf(orig_url, sizeof(orig_url)-1, "https://%s.%s:4343", factory_ssid, iap_get_domainname());
#else
            snprintf(orig_url, sizeof(orig_url)-1, "https://direct.%s:4343", iap_get_domainname());
#endif
            debug_log("tinyproxy", "%s, %d: Redirect client %s to  %s, original url is %s:%d", __func__, __LINE__, mac_to_str(connptr->user.mac), orig_url, request->host, request->port);
            send_http_redirect(connptr, orig_url);
            return -99;
        }
        
        /* Captive portal */
        if (request->port == 80) {
            snprintf(orig_url, sizeof(orig_url)-1, "http://%s%s",
                request->host, request->path);
        } else {
            snprintf(orig_url, sizeof(orig_url)-1, "http://%s:%d%s",
                request->host, request->port, request->path);
        }
        

        /* Check uplink type */
        while (read_cp_config(connptr->user.essid, "ssid", "captive-portal-exclude-uplink", cmd, sizeof(cmd)) == 0) {
            __u32 exclude_types = strtoul(cmd, NULL, 10);
            __u32 type;
            FILE *fp;

            if ((fp = fopen("/tmp/uplink_type", "r")) == NULL)
                break;
            if (fgets(cmd, sizeof(cmd), fp) == NULL) {
                fclose(fp);
                break;
            }
            fclose(fp);
            type = strtoul(cmd, NULL, 10);
            if ((exclude_types & (1 << type)) == 0)
                break;
            log_message (LOG_WARNING, "Couldn't connect to CP server%s%s%sfrom client %s with ESSID %s because uplink type isn't correct.",
                            ecp_server[0] ? " ": "", ecp_server, ecp_server[0] ? " " : "",
                            connptr->client_ip_addr,
                            connptr->user.essid);
            indicate_http_error (connptr, 404,
                                    "Unable to connect to captive portal",
                                    "detail",
                                    "Current uplink type doesn't allow it",
                                    NULL);
            return -1;
        }

#ifndef __OEM_siemens__
        //if (connptr->cp_type == CAPTIVE_PORTAL_FACEBOOK) {
        //    ret = 2;
        //} else 
#endif
        if (connptr->cp_type == CAPTIVE_PORTAL_EXTERNAL) {
            ret = 1;
        } else if (connptr->cp_type == CAPTIVE_PORTAL_INTERNAL) {
            ret = 0;
        } else if (connptr->cp_type == CAPTIVE_PORTAL_DISABLE) {
            debug_log("tinyproxy", "%s, %d: User-[%s], ssid-[%s], role-[%s], disable Captive Portal", __func__, __LINE__, \
                       mac_to_str(connptr->user.mac), connptr->user.essid, connptr->user.role);
            /*There are three case to UI page:
              1. current SSID is 'instant'
              2. default wired SSID
            */

            if (strcmp(connptr->user.essid, factory_essid) == 0 
                || (strncasecmp(connptr->user.essid, "__wired__", 9) == 0 && strcmp(connptr->user.role, default_wired_ssid) == 0)){
                debug_log("tinyproxy", "%s, %d: redirect to UI page", __func__, __LINE__);
                goto local;
            }
            else{
                debug_log("tinyproxy", "%s, %d: redirect to original url [%s]", __func__, __LINE__, orig_url);
                send_http_redirect(connptr, orig_url);
                return -99;
            }
        }
   
        if (connptr->is_wispr && is_wispr_client(connptr->user_agent)){
            encoded_url = encode_url_dup(orig_url);

            if (ret == 0){
                snprintf (cp_url, sizeof (cp_url) - 1,
                    "https://%s/swarm.cgi?opcode=cp_generate&orig_url=%s", redirect_domain, encoded_url);
            }
            else if (ret == 1){
                char *cip;
                cip = safestrdup(ip_to_str(connptr->user.v4_ip));
                encoded_ssid = encode_str_dup(connptr->user.essid);
                encoded_apname = config.ap_name;
                encoded_apmac = config.ap_mac;
                encoded_vcname = config.vc_name;
                snprintf(param_string, sizeof(param_string), iap_param,
                    connptr->current_up->auth_url,
                    connptr->current_up->query_in_auth_url ? '&' : '?',
                    mac_to_str(connptr->user.mac), encoded_ssid,
                    cip, encoded_apname, encoded_apmac, encoded_vcname, (connptr->current_up->switchip && vc_ip[0] != 0 && strcmp(vc_ip, "0.0.0.0")) ? vc_ip : redirect_domain, encoded_url);
                safefree(cip);
                safefree(encoded_ssid);
                snprintf (cp_url, sizeof (cp_url) - 1, "http%s://%s:%d%s",
                    cur_upstream->usehttps ? "s" : "",
                    cur_upstream->host, cur_upstream->port, param_string);
            }

            send_wispr_http_redirect(connptr, ip_to_str(connptr->user.v4_ip), mac_to_str(connptr->user.mac), cp_url);
            safefree(encoded_url);
            return -99;
        }
        
        if (ret < 0) {
            log_message (LOG_WARNING,
                         "Failed to determine configuration of SSID '%s' ROLE '%s'.",
                         connptr->user.essid, connptr->user.role);
            indicate_http_error (connptr, 404,
                                 "Failed to determine configuration of SSID", NULL);
            return -1;
        }
        switch(ret) {
        case 0: {
            /* Internal Captive Portal */
            encoded_url = encode_url_dup(orig_url);
            snprintf (cp_url, sizeof (cp_url) - 1,
                "https://%s/swarm.cgi?opcode=cp_generate&orig_url=%s", redirect_domain, encoded_url);
            send_http_redirect(connptr, cp_url);
            safefree(encoded_url);
            return -99;
        }
        case 1: 
            if (connptr->current_up->useredirect) {
                /* Redirect-based External Captive Portal */

                if (cur_upstream->serverfailthrough
                 && ecp_server_is_down("tinyproxy", cur_upstream->ecp_server_cfg, connptr->server_ip_addr, config.bind_address)) {
                    log_message (LOG_WARNING, "Couldn't connect to ECP server %s from client %s with ESSID %s",
                            ecp_server,
                            connptr->client_ip_addr,
                            connptr->user.essid);
                    redirect_ecp_url(connptr, cur_upstream->redirecturl ? cur_upstream->redirecturl : orig_url);
                    return -99;
                }

                encoded_url = encode_str_dup(orig_url);

                /* Following code copied from the traditional ECP case; should refactor into a function if in the future need to change the parameter string logic */
                {
                    char *cip;
                    cip = safestrdup(ip_to_str(connptr->user.v4_ip));
                    encoded_ssid = encode_str_dup(connptr->user.essid);
                    encoded_apname = config.ap_name;
                    encoded_apmac = config.ap_mac;
                    encoded_vcname = config.vc_name;
                    snprintf(param_string, sizeof(param_string), iap_param,
                        connptr->current_up->auth_url,
                        connptr->current_up->query_in_auth_url ? '&' : '?',
                        mac_to_str(connptr->user.mac), encoded_ssid,
                        cip, encoded_apname, encoded_apmac, encoded_vcname, (connptr->current_up->switchip && vc_ip[0] != 0 && strcmp(vc_ip, "0.0.0.0")) ? vc_ip : redirect_domain, encoded_url);
                    safefree(cip);
                    safefree(encoded_ssid);
                }
                snprintf (cp_url, sizeof (cp_url) - 1, "http%s://%s:%d%s",
                    cur_upstream->usehttps ? "s" : "",
                    cur_upstream->host, cur_upstream->port, param_string);
                log_message (LOG_DEBUG, "Redirect url is %s", cp_url);
                if (cur_upstream->serveroffload){
                    log_message (LOG_DEBUG, "enable server offload for client %s", mac_to_str(connptr->user.mac));
                    send_filtered_http_redirect(connptr, cp_url);
                }
                else
                    send_http_redirect(connptr, cp_url);
                safefree(encoded_url);
                return -99;
            }
            break;

#ifndef __OEM_siemens__
        case 2: 
            {
                char en_url[1024] = {0};
                char redirect_url[1024] = {0};
                char *url;

                log_message (LOG_DEBUG, "The path of client request is %s", request->path);
                if (strstr(request->path, FB_FORWARD_PATH) != NULL){
                    char hash[512] = {0};
                    char c_hash[512] = {0};
                    char cookie[256] = {0};
                    char orig_url[512] = {0};

                    if ((fb_id[0] == 0) || (fb_key[0] == 0)){
                        log_message (LOG_WARNING, "Facebook doesn't register success, please retry later");
                        indicate_http_error (connptr, 404,
                                 "Facebook doesn't register success, please retry later", NULL);
                        return -1;
                    }

                    if ((url = strstr(request->path, "u=")) != NULL){
                        strcpy (orig_url, url + 2);
                        log_message (LOG_DEBUG, "original url is %s", orig_url);
                        decode_orig_url(orig_url, strlen(orig_url), en_url);
                        calc_fb_cookie_hash (mac_to_str (connptr->user.mac), en_url, c_hash);
                        snprintf(cookie, sizeof(cookie), "c_%s=%s", c_hash, orig_url);
                        log_message (LOG_DEBUG, "Cookie %s for client %s", cookie, mac_to_str(connptr->user.mac));
                    }
                    else{
                        log_message (LOG_WARNING, "Couldn't find redirect url");
                        indicate_http_error (connptr, 404,
                                 "Failed to find original url", NULL);
                        return -1;
                    }
                     
                    log_message (LOG_DEBUG, "cookie hash is %s", c_hash);
                    if (calc_fb_hash(fb_id, fb_key, c_hash, hash, 0, redirect_domain) == 0){
                        snprintf(redirect_url, sizeof(redirect_url), "%s%s%s%sc%s%s", FB_EN_INTERNAL_AUTH_URL_HEAD, redirect_domain, FB_EN_INTERNAL_AUTH_URL_TAIL, "%3F", "%3D", c_hash);
                        snprintf(cp_url, sizeof(cp_url), "%s?gw_id=%s&redirect_url=%s&redirect_mac=%s", FB_REDIRECT_URL, fb_id, redirect_url, hash);
                        log_message (LOG_DEBUG, "The redirect url of facebook is %s", cp_url);
                        send_facebook_http_redirect(connptr, cp_url, cookie, redirect_domain);
                        return -99;
                    }
                    else{
                        log_message (LOG_WARNING, "Couldn't calc facebook hash");
                        indicate_http_error (connptr, 404,
                                 "Failed to calc facebook hash", NULL);
                       return -1;
                    }
                }
                else{
                    url = orig_url;
                    encode_str(url, en_url, sizeof(en_url));
                    snprintf (cp_url, sizeof (cp_url) - 1,
                              "http://%s:8080/fbwifi/forward/?u=%s", redirect_domain, en_url);
                    log_message (LOG_DEBUG, "Redirect client %s to %s when receive original request %s", mac_to_str (connptr->user.mac), cp_url, url);
                    send_facebook_http_redirect(connptr, cp_url, NULL, redirect_domain);
                    return -99;
                } 
            }
            break;
#endif
        default:
            log_message (LOG_WARNING,
                         "Failed to determine configuration of SSID '%s'.",
                         connptr->user.essid);
            indicate_http_error (connptr, 404,
                                 "Failed to determine configuration of SSID", NULL);
            return -1;
        }
        /* External Captive Portal */
    }
#endif

    if (!cur_upstream) {
            log_message (LOG_WARNING,
                         "No external server defined for %s.",
                         request->host);
            indicate_http_error (connptr, 404,
                                 "Unable to connect to external captive portal.", NULL);
            return -1;
    }

    connptr->server_fd = opensock (cur_upstream->host, cur_upstream->port,
                      connptr->server_ip_addr);
    if (connptr->server_fd < 0) {
#ifdef __FAT_AP__
            char msg[256];
            if (cur_upstream->serverfailthrough) {
                log_message (LOG_WARNING, "Couldn't connect to ECP server %s from client %s with ESSID %s",
                            ecp_server,
                            connptr->client_ip_addr,
                            connptr->user.essid);
                redirect_ecp_url(connptr, cur_upstream->redirecturl ? cur_upstream->redirecturl : orig_url);
                return -99;
            }
            snprintf(msg, sizeof(msg), "A network error occurred while trying to connect to the captive portal server %s.", ecp_server);
#endif
            log_message (LOG_WARNING,
                         "Could not connect to external server %s.", ecp_server);
            indicate_http_error (connptr, 404,
                                 "Unable to connect to external captive portal",
                                 "detail",
#ifdef __FAT_AP__
                                 msg,
#else
                                 "A network error occurred while trying to connect to the captive portal server.",
#endif
                                 NULL);
            return -1;
    }
 
    log_message (LOG_CONN,
                 "Established connection to external server \"%s\" "
                 "using file descriptor %d.",
                 ecp_server, connptr->server_fd);

#ifndef __FAT_AP__
        /* Convert ipv6->ipv4 address */
    if (!strncasecmp(connptr->client_ip_addr, "::ffff:", 7))
        cip += 7;
#else
    {
        /* Below code copied to redirect-ECP logic above; should refactor into a function if in the future need to change the parameter string logic */
        char *cip;
        cip = safestrdup(ip_to_str(connptr->user.v4_ip));
        encoded_url = encode_str_dup(orig_url);
        encoded_ssid = encode_str_dup(connptr->user.essid);
        encoded_apname = config.ap_name;
        encoded_apmac = config.ap_mac;
        encoded_vcname = config.vc_name;
        snprintf(param_string, sizeof(param_string), iap_param,
            cur_upstream->auth_url,
            cur_upstream->query_in_auth_url ? '&' : '?',
            mac_to_str(connptr->user.mac), encoded_ssid,
                 cip, encoded_apname, encoded_apmac, encoded_vcname, redirect_domain, encoded_url);
        safefree(cip);
        safefree(encoded_ssid);
        safefree(encoded_url);

        if (request->path)
            safefree (request->path);
        request->path = safestrdup(param_string);    /* login page */
    }
    connptr->build_auto_wl = BUILD_AUTO_WL_ALL;
#endif
    debug_log("tinyproxy", "%s: %d: request for %s proxied to %s:%d%s",
                __func__, __LINE__, orig_url, cur_upstream->host, cur_upstream->port, request->path);

    if (request->host) {
#ifdef __FAT_AP__
        if (cur_upstream->port == 80) {
             snprintf(connptr->doc_base, sizeof(connptr->doc_base), "\r\n<base href=\"http://%s%s\" />\r\n",
                cur_upstream->host,
                 cur_upstream->auth_url);
        } else {
            snprintf(connptr->doc_base, sizeof(connptr->doc_base), "\r\n<base href=\"http://%s:%d%s\" />\r\n",
                cur_upstream->host, cur_upstream->port,
                 cur_upstream->auth_url);
        }
#endif
            safefree (request->host);
    }
    request->host = safestrdup(cur_upstream->host);

    return establish_http_connection (connptr, request);
}


/*
 * Establish a connection to the upstream proxy server.
 */
static int
connect_to_upstream (struct conn_s *connptr, struct request_s *request)
{
#ifndef UPSTREAM_SUPPORT
        /*
         * This function does nothing if upstream support was not compiled
         * into tinyproxy.
         */
        return -1;
#else
        char *combined_string;
        int len;

        struct upstream *cur_upstream = connptr->upstream_proxy;

        if (!cur_upstream) {
                log_message (LOG_WARNING,
                             "No upstream proxy defined for %s.",
                             request->host);
                indicate_http_error (connptr, 404,
                                     "Unable to connect to upstream proxy.", NULL);
                return -1;
        }

        connptr->server_fd =
            opensock (cur_upstream->host, cur_upstream->port,
                      connptr->server_ip_addr);

        if (connptr->server_fd < 0) {
                log_message (LOG_WARNING,
                             "Could not connect to upstream proxy.");
                indicate_http_error (connptr, 404,
                                     "Unable to connect to upstream proxy",
                                     "detail",
                                     "A network error occurred while trying to "
                                     "connect to the upstream web proxy.",
                                     NULL);
                return -1;
        }

        log_message (LOG_CONN,
                     "Established connection to upstream proxy \"%s\" "
                     "using file descriptor %d.",
                     cur_upstream->host, connptr->server_fd);

        /*
         * We need to re-write the "path" part of the request so that we
         * can reuse the establish_http_connection() function. It expects a
         * method and path.
         */
        if (connptr->connect_method) {
                len = strlen (request->host) + 7;
                combined_string = (char *) safemalloc (len);
                if (!combined_string) {
                    return -1;
                }
                snprintf (combined_string, len, "http://%s:%d",
                          request->host, request->port);
        } else {
                len = strlen (request->host) + strlen (request->path) + 14;
                combined_string = (char *) safemalloc (len);
                if (!combined_string) {
                    return -1;
                }
                snprintf (combined_string, len, "http://%s:%d%s", request->host,
                          request->port, request->path);
                    
        }

        if (request->path)
                safefree (request->path);
        request->path = combined_string;

        return establish_http_connection (connptr, request);
#endif
}

static int
get_request_entity(struct conn_s *connptr)
{
        int ret;
        fd_set rset;
        struct timeval tv;

        FD_ZERO (&rset);
        FD_SET (connptr->client_fd, &rset);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        ret = select (connptr->client_fd + 1, &rset, NULL, NULL, &tv);

        if (ret == -1) {
                log_message (LOG_ERR,
                             "Error calling select on client fd %d: %s",
                             connptr->client_fd, strerror(errno));
        } else if (ret == 0) {
               log_message (LOG_INFO, "no entity");
        } else if (ret == 1 && FD_ISSET (connptr->client_fd, &rset)) {
                ssize_t nread;
                nread = read_buffer (connptr->client_fd, connptr->cbuffer);
                if (nread < 0) {
                        log_message (LOG_ERR,
                                     "Error reading readble client_fd %d",
                                     connptr->client_fd);
                        ret = -1;
                } else {
                        log_message (LOG_INFO,
                                     "Read request entity of %d bytes",
                                     nread);
                        ret = 0;
                }
        } else {
                log_message (LOG_ERR, "strange situation after select: "
                             "ret = %d, but client_fd (%d) is not readable...",
                             ret, connptr->client_fd);
                ret = -1;
        }

        return ret;
}

#ifdef __FAT_AP__
static void
redirect_ecp_url(struct conn_s *connptr, const char *orig_url)
{
    log_message (LOG_WARNING, "Redirect client URL %s", orig_url);
    send_http_redirect_timeout(connptr, orig_url);
    authenticate_user (connptr);
}
#endif

/*
 * This is the main drive for each connection. As you can tell, for the
 * first few steps we are using a blocking socket. If you remember the
 * older tinyproxy code, this use to be a very confusing state machine.
 * Well, no more! :) The sockets are only switched into nonblocking mode
 * when we start the relay portion. This makes most of the original
 * tinyproxy code, which was confusing, redundant. Hail progress.
 * 	- rjkaes
 */
void handle_connection (int fd)
{
        ssize_t i;
        struct conn_s *connptr;
        struct request_s *request = NULL;
        hashmap_t hashofheaders = NULL;
        int   client_port = 0;
        int   server_port = 0;

        char sock_ipaddr[IP_LENGTH];
        char peer_ipaddr[IP_LENGTH];
        char peer_string[HOSTNAME_LENGTH];

#ifdef __FAT_AP__
         int sport = 0;

        if (getpeer_information (fd, peer_ipaddr, peer_string, &client_port) == -1) {
            log_message (LOG_CONN, "Connect (file descriptor %d): failed to get peer information", fd);
            close (fd);
            return;
        }
#else
        getpeer_information (fd, peer_ipaddr, peer_string, &client_port);
#endif

        if (config.bindsame)
            getsock_ip (fd, sock_ipaddr, &server_port);

        connptr = initialize_conn (fd, peer_ipaddr, peer_string,
                                   config.bindsame ? sock_ipaddr : NULL);

        if (!connptr) {
                close (fd);
                return;
        }
        connptr->client_port = client_port;
        connptr->server_port = server_port;

#ifdef __FAT_AP__
        connptr->user_authenticated = 0;
        memset(&(connptr->user), 0, sizeof(connptr->user));
        if (0 != get_cp_user_info(connptr->client_ip_addr, connptr->client_port, &connptr->user)) {
            //if (strcmp(request->host, redirect_domain) != 0 || request->port != 8080) {
                {
                indicate_http_error (connptr, 403, "Access denied",
                                     "detail",
                                     "The administrator of this proxy has not configured "
                                     "it to service requests from your host.",
                                     NULL);
                goto fail;
            }
        }
#endif

        if (config.bindsame) {
            log_message (LOG_CONN, "Connect (file descriptor %d): %s [%s %d] at [%s %d]",
                         fd, peer_string, peer_ipaddr, client_port, sock_ipaddr, server_port);

        }
        else {
            log_message (LOG_CONN, "Connect (file descriptor %d): %s [%s %d]",
                         fd, peer_string, peer_ipaddr, client_port);
        }

#if 0
        if (check_acl (peer_ipaddr, peer_string, config.access_list) <= 0) {
                update_stats (STAT_DENIED);
                indicate_http_error (connptr, 403, "Access denied",
                                     "detail",
                                     "The administrator of this proxy has not configured "
                                     "it to service requests from your host.",
                                     NULL);
                goto fail;
        }
#endif

        if (read_request_line (connptr) < 0) {
                update_stats (STAT_BADCONN);
                indicate_http_error (connptr, 408, "Timeout",
                                     "detail",
                                     "Server timeout waiting for the HTTP request "
                                     "from the client.", NULL);
                goto fail;
        }

#ifdef __FAT_AP__
        {
            /* Test the request line before proceeding further */
            struct conn_s conn_tmp;
            struct request_s *request_tmp;
            hashmap_t hashofheaders_tmp = hashmap_create (HEADER_BUCKETS);
            if (hashofheaders_tmp == NULL) {
                update_stats (STAT_BADCONN);
                indicate_http_error (connptr, 503, "Internal error",
                                     "detail",
                                     "An internal server error occurred while processing "
                                     "your request. Please contact the administrator.",
                                     NULL);
                goto fail;
            }
            memcpy(&conn_tmp, connptr, sizeof(conn_tmp));
            request_tmp = process_request (&conn_tmp, hashofheaders_tmp);
            if (!request_tmp) {
                if (conn_tmp.show_stats) {
                    update_stats (STAT_BADCONN);
                }
                hashmap_delete(hashofheaders_tmp);
                goto fail;
            }
            free_request_struct (request_tmp);
            hashmap_delete(hashofheaders_tmp);
        }
#endif
        /*
         * The "hashofheaders" store the client's headers.
         */
        hashofheaders = hashmap_create (HEADER_BUCKETS);
        if (hashofheaders == NULL) {
                update_stats (STAT_BADCONN);
                indicate_http_error (connptr, 503, "Internal error",
                                     "detail",
                                     "An internal server error occurred while processing "
                                     "your request. Please contact the administrator.",
                                     NULL);
                goto fail;
        }

        /*
         * Get all the headers from the client in a big hash.
         */
        if (get_all_headers (connptr->client_fd, hashofheaders) < 0) {
                log_message (LOG_WARNING,
                             "Could not retrieve all the headers from the client");
                indicate_http_error (connptr, 400, "Bad Request",
                                     "detail",
                                     "Could not retrieve all the headers from "
                                     "the client.", NULL);
                update_stats (STAT_BADCONN);
                goto fail;
        }

        /*
         * Add any user-specified headers (AddHeader directive) to the
         * outgoing HTTP request.
         */
        for (i = 0; i < vector_length (config.add_headers); i++) {
                http_header_t *header = (http_header_t *)
                        vector_getentry (config.add_headers, i, NULL);

                hashmap_insert (hashofheaders,
                                header->name,
                                header->value, strlen (header->value) + 1);
        }

        request = process_request (connptr, hashofheaders);
        if (!request) {
                if (!connptr->show_stats) {
                        update_stats (STAT_BADCONN);
                }
                goto fail;
        }

#ifdef __FAT_AP__
        if (0 != get_cp_profile_info(connptr)){
            goto fail;
        }
        /*
         * Filter restricted domains/urls
         */
        if (request_in_bl (request->host, request->path)) {
                update_stats (STAT_DENIED);
                log_message (LOG_NOTICE,
                                 "Proxying refused on filtered domain \"%s\", url \"%s\"",
                                    request->host, request->path);
                indicate_http_error (connptr, 403, "Filtered",
                                     "detail",
                                     "The request you made has been filtered",
                                     "url", request->path, NULL);
                goto fail;
        }

        connptr->mangle_ecp_headers = 1;
        if (connptr->cp_type == CAPTIVE_PORTAL_EXTERNAL && connptr->current_up != NULL
         && strcasecmp(request->host, connptr->current_up->host) == 0) {
            if (connptr->current_up->usehttps) {
                goto skip_white_list;
            }
            /* Request is to ECP server, process as if in white list
            but modify destination port to the configured ECP server port */
            request->in_white_list = 1;
            if (request->port != connptr->current_up->port) {
                debug_log("tinyproxy", "%s: %d: request for http://%s:%d%s proxied to http://%s:%d%s",
                    __func__, __LINE__, request->host, request->port, request->path, request->host, connptr->current_up->port, request->path);
                request->port = connptr->current_up->port;
            }
#ifndef __OEM_siemens__
        } else if (connptr->cp_type == CAPTIVE_PORTAL_FACEBOOK){
            char *user_agent;
            if(hashmap_entry_by_key (hashofheaders, "User-Agent", (void **) &user_agent) > 0){
                if (strstr(user_agent, "CaptiveNetworkSupport") != NULL)
                    request->in_white_list = 1;
            }
#endif
        } else {
	        /*
	        * Check to see if request is in walled garden
	        */
	        request->in_white_list = request_in_wl (request->host, request->path);
            if (request->in_white_list) {
                /* Only when the URL is in the real walled garden, do we not mangle the headers from server */
                connptr->mangle_ecp_headers = 0;
            }
        }
	    if (request->in_white_list) {
            connptr->build_auto_wl = BUILD_AUTO_WL_ALL;
        } else {
	        request->in_white_list = request_in_auto_wl (request->host, request->path);
            if (request->in_white_list) {
                connptr->mangle_ecp_headers = 0;
            }
            connptr->build_auto_wl = BUILD_AUTO_WL_DOC_ONLY;
	    }
skip_white_list:
#endif


        /*
        connptr->upstream_proxy = UPSTREAM_HOST (request->host);
		fprintf(stderr, "* %d %s %s\n", request->in_white_list, 
                request->host, 
                connptr->upstream_proxy ? connptr->upstream_proxy->host:"n/a");
        */
        if (connptr->upstream_proxy != NULL) {
                if (connect_to_upstream (connptr, request) < 0) {
                        goto fail;
                }
#ifndef __FAT_AP__
        } else if (config.auth_url && config.auth_host && !request->in_white_list) {
#else
        } else if (!request->in_white_list && strcmp(request->method, "CONNECT")) {
            char *user_agent;
            int ret;
            if(hashmap_entry_by_key (hashofheaders, "User-Agent", (void **) &user_agent) > 0){
                strlcpy(connptr->user_agent, user_agent, sizeof(connptr->user_agent));
            }
#endif
            ret = connect_to_ecp (connptr, request);
            if (ret == -99) {
                goto done;
            }
            if (ret < 0) {
                goto fail;
            }
        } else {
                debug_log("tinyproxy", "%s: %d: request for http://%s:%d%s is proxied to original URL",
                    __func__, __LINE__, request->host, request->port, request->path);
#ifdef __FAT_AP__
                const char *internal_server = "127.0.0.1";
                int to_portal = 0;
                if(connptr->connect_method){
                    if(!strcmp(request->host, redirect_domain)){
                        request->port = 443;
                    }
                    else{
                        if (connptr->cp_type == CAPTIVE_PORTAL_EXTERNAL && connptr->current_up != NULL){
                            if (strcmp(connptr->current_up->host, request->host)){
                                request->port = 4343;
                            }
                            else{
                                to_portal = 1;
                                goto proxy;
                            } 
                        }
                        else if(connptr->cp_type == CAPTIVE_PORTAL_INTERNAL){
                            request->port = 4343;
                        }
                        else
                            goto fail;
                    }
                    connptr->server_fd = opensock (internal_server, request->port,
                                                   connptr->server_ip_addr);
                }
                else{
proxy:
                    connptr->server_fd = opensock (request->host, request->port,
                                               connptr->server_ip_addr);
                }
#else
                connptr->server_fd = opensock (request->host, request->port,
                                               connptr->server_ip_addr);
#endif
                if (connptr->server_fd < 0) {
                        indicate_http_error (connptr, 500, "Unable to connect",
                                             "detail",
                                             PACKAGE_NAME " "
                                             "was unable to connect to the remote web server.",
                                             "error", strerror (errno), NULL);
                        goto fail;
                }

#ifdef __FAT_AP__
                if(connptr->connect_method && to_portal != 1){
                    struct sockaddr_in sock_server;
                    int    name_len = sizeof(sock_server);

                    memset(&sock_server, 0, name_len);
                    getsockname(connptr->server_fd, (struct sockaddr *)&sock_server, &name_len);
                    sport = ntohs(sock_server.sin_port);
                    debug_log("tinyproxy", "%s: %d: request for http://%s:%d%s is proxied to original URL, internal source port is %d",
                        __func__, __LINE__, request->host, request->port, request->path, ntohs(sock_server.sin_port));
                    ecp_proxy_add(mac_to_str(connptr->user.mac), connptr->client_ip_addr, connptr->client_port, sport, getpid());
                    goto ssl_reply;
                }
#endif
                log_message (LOG_CONN,
                             "Established connection to host \"%s\" using "
                             "file descriptor %d.", request->host,
                             connptr->server_fd);

                if (!connptr->connect_method)
                        establish_http_connection (connptr, request);
        }

        if (process_client_headers (connptr, hashofheaders) < 0) {
                update_stats (STAT_BADCONN);
                goto fail;
        }

        if (!(connptr->connect_method && (connptr->upstream_proxy == NULL))) {
#ifdef __FAT_AP__
                if (process_server_headers (connptr, request->in_white_list) < 0)
#else
                if (process_server_headers (connptr) < 0)
#endif
                {
                        update_stats (STAT_BADCONN);
                        goto fail;
                }
        } else {
ssl_reply:
                if (send_ssl_response (connptr) < 0) {
                        log_message (LOG_ERR,
                                     "handle_connection: Could not send SSL greeting "
                                     "to client.");
                        update_stats (STAT_BADCONN);
                        goto fail;
                }
        }

        relay_connection (connptr);

        log_message (LOG_INFO,
                     "Closed connection between local client (fd:%d) "
                     "and remote client (fd:%d)",
                     connptr->client_fd, connptr->server_fd);
#ifdef __FAT_AP__
        debug_log ("tinyproxy", "Closed connection between local client (fd:%d) "
                    "and remote client (fd:%d), user %s(%s) on '%s', user_authenticated=%d",
                    connptr->client_fd, connptr->server_fd, 
                    ip_to_str(connptr->user.v4_ip), mac_to_str(connptr->user.mac), connptr->user.essid,
                    connptr->user_authenticated);
#endif

        goto done;

fail:
        /*
         * First, get the body if there is one.
         * If we don't read all there is from the socket first,
         * it is still marked for reading and we won't be able
         * to send our data properly.
         */
        if (get_request_entity (connptr) < 0) {
                log_message (LOG_WARNING,
                             "Could not retrieve request entity");
                indicate_http_error (connptr, 400, "Bad Request",
                                     "detail",
                                     "Could not retrieve the request entity "
                                     "the client.", NULL);
                update_stats (STAT_BADCONN);
        }
        if (connptr->error_variables) {
                send_http_error_message (connptr);
        } else if (connptr->show_stats) {
                showstats (connptr);
        }

done:
        if(sport)
            ecp_proxy_remove(sport, getpid());
        free_request_struct (request);
        hashmap_delete (hashofheaders);
        destroy_conn (connptr);
        return;
}
