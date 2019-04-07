/* tinyproxy - A fast light-weight HTTP proxy
 * Copyright (C) 2003 Steven Young <sdyoung@miranda.org>
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

/* This file contains source code for the handling and display of
 * HTML error pages with variable substitution.
 */

#include "main.h"

#include "common.h"
#include "buffer.h"
#include "conns.h"
#include "heap.h"
#include "html-error.h"
#include "network.h"
#include "utils.h"
#include "conf.h"

/*
 * Add an error number -> filename mapping to the errorpages list.
 */
#define ERRORNUM_BUFSIZE 8      /* this is more than required */
#define ERRPAGES_BUCKETCOUNT 16

#ifdef __FAT_AP__
extern char wispr_location_id[128];
extern char wispr_location_name[128];
#endif

int add_new_errorpage (char *filepath, unsigned int errornum)
{
        char errornbuf[ERRORNUM_BUFSIZE];

        config.errorpages = hashmap_create (ERRPAGES_BUCKETCOUNT);
        if (!config.errorpages)
                return (-1);

        snprintf (errornbuf, ERRORNUM_BUFSIZE, "%u", errornum);

        if (hashmap_insert (config.errorpages, errornbuf,
                            filepath, strlen (filepath) + 1) < 0)
                return (-1);

        return (0);
}

/*
 * Get the file appropriate for a given error.
 */
static char *get_html_file (unsigned int errornum)
{
        hashmap_iter result_iter;
        char errornbuf[ERRORNUM_BUFSIZE];
        char *key;
        static char *val;

        assert (errornum >= 100 && errornum < 1000);

        if (!config.errorpages)
                return (config.errorpage_undef);

        snprintf (errornbuf, ERRORNUM_BUFSIZE, "%u", errornum);

        result_iter = hashmap_find (config.errorpages, errornbuf);

        if (hashmap_is_end (config.errorpages, result_iter))
                return (config.errorpage_undef);

        if (hashmap_return_entry (config.errorpages, result_iter,
                                  &key, (void **) &val) < 0)
                return (config.errorpage_undef);

        return (val);
}

/*
 * Look up the value for a variable.
 */
static char *lookup_variable (struct conn_s *connptr, const char *varname)
{
        hashmap_iter result_iter;
        char *key;
        static char *data;

        result_iter = hashmap_find (connptr->error_variables, varname);

        if (hashmap_is_end (connptr->error_variables, result_iter))
                return (NULL);

        if (hashmap_return_entry (connptr->error_variables, result_iter,
                                  &key, (void **) &data) < 0)
                return (NULL);

        return (data);
}

/*
 * Send an already-opened file to the client with variable substitution.
 */
int
send_html_file (FILE *infile, struct conn_s *connptr)
{
        char *inbuf;
        char *varstart = NULL;
        char *p;
        const char *varval;
        int in_variable = 0;
        int r = 0;

        inbuf = (char *) safemalloc (4096);

        while (fgets (inbuf, 4096, infile) != NULL) {
                for (p = inbuf; *p; p++) {
                        switch (*p) {
                        case '}':
                                if (in_variable) {
                                        *p = '\0';
                                        varval = (const char *)
                                                lookup_variable (connptr,
                                                                 varstart);
                                        if (!varval)
                                                varval = "(unknown)";
                                        r = write_message (connptr->client_fd,
                                                           "%s", varval);
                                        in_variable = 0;
                                } else {
                                        r = write_message (connptr->client_fd,
                                                           "%c", *p);
                                }

                                break;

                        case '{':
                                /* a {{ will print a single {.  If we are NOT
                                 * already in a { variable, then proceed with
                                 * setup.  If we ARE already in a { variable,
                                 * this code will fallthrough to the code that
                                 * just dumps a character to the client fd.
                                 */
                                if (!in_variable) {
                                        varstart = p + 1;
                                        in_variable++;
                                } else
                                        in_variable = 0;

                        default:
                                if (!in_variable) {
                                        r = write_message (connptr->client_fd,
                                                           "%c", *p);
                                }
                        }

                        if (r)
                                break;
                }

                if (r)
                        break;

                in_variable = 0;
        }

        safefree (inbuf);

        return r;
}

int send_http_headers (struct conn_s *connptr, int code, const char *message)
{
        const char *headers =
            "HTTP/1.0 %d %s\r\n"
            "Server: %s/%s\r\n"
            "Content-Type: text/html\r\n" "Connection: close\r\n" "\r\n";
    
        return (write_message (connptr->client_fd, headers,
                               code, message, PACKAGE, VERSION));
}

#ifdef __FAT_AP__
extern char redirect_domain[128];
#ifndef __OEM_siemens__
int send_facebook_http_redirect (struct conn_s *connptr, const char *url, char *cookie, char *redirect_domain)
{
    static const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";
    char timebuf[256] = {0};
    char cookie_timebuf[256] = {0};
    char cookie_buf[1024] = {0};
    time_t now;
    int  len = 0;

    const char *headers =
        "HTTP/1.1 302 Captive Portal\r\n"
        "Server:\r\n"
        "Date: %s\r\n"
        "Cache-Control: no-cache,no-store,must-revalidate,post-check=0,pre-check=0\r\n"
        "Location: %s\r\n"
        "Connection: close\r\n"
        "%s\r\n"
        "\r\n"
        "<html>\r\n"
        "    <head>\r\n"
        "        <title>Redirecting to Facebook</title>\r\n"
        "    </head>\r\n"
        "    <body>If you are not redirected with 5 seconds, please <a href=\"%s\">click here</a>.</body>\r\n"
        "</html>\r\n";
           
    now = time(NULL); 
    strftime( timebuf, sizeof(timebuf), rfc1123_fmt, gmtime( &now ));

    if (cookie != NULL){
        now = now + 12 * 3600;
        strftime( cookie_timebuf, sizeof(cookie_timebuf), rfc1123_fmt, gmtime( &now ) );

        if (strlen(cookie) > sizeof(cookie_buf))
            debug_log("mini_httpd", "%s: %d: cookie buffer is short, please make it longer", __func__, __LINE__);
            
        snprintf(cookie_buf, sizeof(cookie_buf), "Set-Cookie: %s; Expires=%s; Domain=%s; Path=/fbwifi", cookie, cookie_timebuf, redirect_domain);
    }
    return (write_message (connptr->client_fd, headers,
        timebuf, url, cookie == NULL ? "" : cookie_buf, url));
}
#endif

int send_filtered_http_redirect (struct conn_s *connptr, const char *url)
{
    static const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";
    char timebuf[256];
    time_t now;

    const char *headers =
        "HTTP/1.1 200 Ok\r\n"
        "Server:\r\n"
        "Date: %s\r\n"
        "Cache-Control: no-cache,no-store,must-revalidate,post-check=0,pre-check=0\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html>\r\n"
        "    <head>\r\n"
        "    <meta http-equiv=\"Refresh\" content=\"0; url=%s\">\r\n"
        "    </head>\r\n"
        "</html>\r\n";
           
    now = time(NULL); 
    strftime( timebuf, sizeof(timebuf), rfc1123_fmt, gmtime( &now ) );
    return (write_message (connptr->client_fd, headers,
        timebuf, url));
}

int send_http_redirect (struct conn_s *connptr, const char *url)
{
    static const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";
    char timebuf[256];
    time_t now;

    const char *headers =
        "HTTP/1.1 302 Captive Portal\r\n"
        "Server:\r\n"
        "Date: %s\r\n"
        "Cache-Control: no-cache,no-store,must-revalidate,post-check=0,pre-check=0\r\n"
        "Location: %s\r\n"
        "Connection: close\r\n"
        "\r\n";
           
    now = time(NULL); 
    strftime( timebuf, sizeof(timebuf), rfc1123_fmt, gmtime( &now ) );
    return (write_message (connptr->client_fd, headers,
        timebuf, url));
}

int send_http_redirect_timeout (struct conn_s *connptr, const char *url)
{
    char msg[4096];
    create_http_redirect_header (SWARM_ECP_FAILTHROUGH_MSG, url, msg, sizeof(msg));
    return (write_message (connptr->client_fd, "%s", msg));
}

int send_wispr_http_redirect (struct conn_s *connptr, char *ip, char *mac, char *url)
{
    static const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";
    char *fqdn = redirect_domain;
    char timebuf[256] = {0};
    time_t now;
    char head_buf[4096] = {0};
    char html_buf[4096] = {0};
    int html_len = 0;
    int head_len = 0;

    const char *tail =
        "\r\n"
        "0\r\n"
        "\r\n";

    now = time(NULL);
    strftime( timebuf, sizeof(timebuf), rfc1123_fmt, gmtime( &now ) );

    html_len = snprintf(html_buf, sizeof(html_buf), "<html> <!--");
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "<WISPAccessGatewayParam xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://us-rws-gw.ipass.com/pages/WISPAccessGatewayParam.xsd\">");
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "<Redirect>");
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "<MessageType>100</MessageType>");
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "<ResponseCode>0</ResponseCode>");
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "<AccessProcedure>1.0</AccessProcedure>");
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "<AccessLocation>%s</AccessLocation>", wispr_location_id);
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "<LocationName>%s</LocationName>", wispr_location_name);
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "<LoginURL>https://%s/cgi-bin/login?cmd=login&amp;mac=%s&amp;ip=%s&amp;opcode=wispr</LoginURL>", fqdn, mac, ip);
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "<AbortLoginURL>https://%s/cgi-bin/login?cmd=abort&amp;mac=%s&amp;ip=%s&amp;opcode=wispr</AbortLoginURL>", fqdn, mac, ip);
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "</Redirect></WISPAccessGatewayParam>");
    html_len += snprintf(html_buf + html_len, sizeof(html_buf), "--> </html>");
    head_len = snprintf(head_buf, sizeof(head_buf), "HTTP/1.1 302 Temporarily Moved\r\n");
    head_len += snprintf(head_buf + head_len, sizeof(head_buf), "Date: %s\r\n", timebuf);
    head_len += snprintf(head_buf + head_len, sizeof(head_buf), "Server: \r\n");
    head_len += snprintf(head_buf + head_len, sizeof(head_buf), "Location: %s\r\n", url);
    head_len += snprintf(head_buf + head_len, sizeof(head_buf), "Connection: close\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\n%x\r\n", html_len);
    
    return (write_message (connptr->client_fd, "%s%s%s", head_buf, html_buf, tail));
}


#endif

/*
 * Display an error to the client.
 */
int send_http_error_message (struct conn_s *connptr)
{
        char *error_file;
        FILE *infile;
        int ret;
        const char *fallback_error =
            "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" "
            "\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n"
            "<html>\n"
            "<head><title>%d %s</title></head>\n"
            "<body>\n"
            "<h1>%s</h1>\n"
            "<p>%s</p>\n"
            "<hr />\n"
            "<p><em>Generated by %s version %s.</em></p>\n" "</body>\n"
            "</html>\n";

        send_http_headers (connptr, connptr->error_number,
                           connptr->error_string);

        error_file = get_html_file (connptr->error_number);
        if (!(infile = fopen (error_file, "r"))) {
                char *detail = lookup_variable (connptr, "detail");
                return (write_message (connptr->client_fd, fallback_error,
                                       connptr->error_number,
                                       connptr->error_string,
                                       connptr->error_string,
                                       detail, PACKAGE, VERSION));
        }

        ret = send_html_file (infile, connptr);
        fclose (infile);
        return (ret);
}

/*
 * Add a key -> value mapping for HTML file substitution.
 */

#define ERRVAR_BUCKETCOUNT 16

int
add_error_variable (struct conn_s *connptr, const char *key, const char *val)
{
        if (!connptr->error_variables)
                if (!
                    (connptr->error_variables =
                     hashmap_create (ERRVAR_BUCKETCOUNT)))
                        return (-1);

        return hashmap_insert (connptr->error_variables, key, val,
                               strlen (val) + 1);
}

#define ADD_VAR_RET(x, y)				   \
	do {                                               \
                if (y == NULL)                             \
                        break;                             \
		if (add_error_variable(connptr, x, y) < 0) \
			return -1;			   \
	} while (0)

/*
 * Set some standard variables used by all HTML pages
 */
int add_standard_vars (struct conn_s *connptr)
{
        char errnobuf[16];
        char timebuf[30];
        time_t global_time;

        snprintf (errnobuf, sizeof errnobuf, "%d", connptr->error_number);
        ADD_VAR_RET ("errno", errnobuf);

        ADD_VAR_RET ("cause", connptr->error_string);
        ADD_VAR_RET ("request", connptr->request_line);
        ADD_VAR_RET ("clientip", connptr->client_ip_addr);
        ADD_VAR_RET ("clienthost", connptr->client_string_addr);

        /* The following value parts are all non-NULL and will
         * trigger warnings in ADD_VAR_RET(), so we use
         * add_error_variable() directly.
         */

        global_time = time (NULL);
        strftime (timebuf, sizeof (timebuf), "%a, %d %b %Y %H:%M:%S GMT",
                  gmtime (&global_time));
        add_error_variable (connptr, "date", timebuf);

        add_error_variable (connptr, "website",
                            "https://www.banu.com/tinyproxy/");
        add_error_variable (connptr, "version", VERSION);
        add_error_variable (connptr, "package", PACKAGE);

        return (0);
}

/*
 * Add the error information to the conn structure.
 */
int
indicate_http_error (struct conn_s *connptr, int number,
                     const char *message, ...)
{
        va_list ap;
        char *key, *val;

        va_start (ap, message);

        while ((key = va_arg (ap, char *))) {
                val = va_arg (ap, char *);

                if (add_error_variable (connptr, key, val) == -1) {
                        va_end (ap);
                        return (-1);
                }
        }

        connptr->error_number = number;
        connptr->error_string = safestrdup (message);

        va_end (ap);

        return (add_standard_vars (connptr));
}

#ifdef __FAT_AP__
inline char *
encode_str_dup (const char * str)
{
    char        estr[4096] = "\0";
    return safestrdup(encode_str(str, estr, sizeof(estr)));
}

inline char *
encode_url_dup (const char * str)
{
    char      * p;
    char        estr[4096];
    int         len = sizeof(estr);

    p = estr;
    p[0] = 0;
    while (*str && (len > 1)) {
        if (len > 2) {
            snprintf (p, len, "%02x", *str);
            p += 2;
            len -= 2;
            str++;
        }
        else
            break;
    }
    *p = 0;

    return safestrdup(estr);
}
#endif
