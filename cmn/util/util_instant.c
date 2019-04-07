#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include "util_instant.h"
#ifndef __OEM_siemens__
#include <crypto-util/cryptohash.h>
#include <crypto-util/md5.h>
#ifdef __NO_OPENSSL__
#include <crypto-util/cyassl_wrap/xyssl/base64_cyassl_wrap.h>
#endif

void md5_calc(output, input, inlen)
unsigned char *output;
unsigned char *input;                                /* input block */
unsigned int inlen;                     /* length of input block */
{
    MD5Context context;

    MD5Init(&context);
    MD5Update(&context, input, inlen);
    MD5Final(output, &context);
}
#endif

static int
tp_fw_ioctl (swarm_acl_t * acl)
{
    struct ifreq  ifr;
    int           rc, fd;

    fd = socket (PF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        fprintf (stderr, "could not open socket.\n");
        return (0);
    }

    memset  (&ifr, 0, sizeof(ifr));
    strcpy (ifr.ifr_name, "br0");

    ifr.ifr_data = acl;
    if ((rc = ioctl (fd, SIOCSWARMIOCTL, &ifr)) < 0) {
        fprintf (stderr,"Could not ioctl to num of entries\n");
        close (fd);
        return (0);
    }

    close (fd);
    return (1);
}

int
_tp_acl_cp_user_get (char * ip, __u16 port, swarm_info_user_t *user_out, __u32 cmd, char *ap_ip, char *dst_ip)
{
    swarm_acl_t acl;
    swarm_info_session_t *session = &(acl.session);
    swarm_info_user_t *user = &(acl.user);
    int           i1, i2, i3, i4;
    char          dip[32];

    memset (&acl, 0, sizeof (acl));
    acl.cmd = cmd;
    if (dst_ip) {
        sscanf (dst_ip, "%d.%d.%d.%d", &i1, &i2, &i3, &i4);
    } else {
        sscanf (ip, "%d.%d.%d.%d", &i1, &i2, &i3, &i4);
        i4--;
    }

    sprintf (dip, "%d.%d.%d.%d", i1, i2, i3, i4);

    FRAME_IP_SET (session->u.v4.sip, str_to_ip (ip));
    FRAME_IP_SET (session->u.v4.dip, str_to_ip (dip));
    session->sport = htons(port);
    session->valid = 1;

    if (!tp_fw_ioctl (&acl)) {
        syslog(LOG_ERR, "%s: %d: Did not retrieve any data from datapath for session '%s'-'%s'\n",
            __func__, __LINE__, ip, dip);
        return -1;
    }

    if (!(user->valid)) {
        if ((ap_ip == NULL) || ((ap_ip != NULL) && (!strcmp(ap_ip, dip))))
        syslog(LOG_ERR, "%s: %d: Could not retrieve valid data from datapath for session '%s'-'%s'\n",
            __func__, __LINE__, ip, dip);
        return -2;
    }

    memcpy(user_out, user, sizeof(*user_out));
    return 0;
}

inline int
tp_acl_https_user_session_get (char * ip, __u16 port, swarm_info_user_t *user_out)
{
    return _tp_acl_cp_user_get(ip, port, user_out, SWARM_IOCTL_HTTPS_USER_SESSION_GET, NULL, NULL);
}

inline int
tp_acl_cp_user_session_get (char * ip, __u16 port, swarm_info_user_t *user_out)
{
    return _tp_acl_cp_user_get(ip, port, user_out, SWARM_IOCTL_CP_USER_SESSION_GET, NULL, NULL);
}

inline int
tp_acl_cp_user_get (char * ip, __u16 port, swarm_info_user_t *user_out)
{
    return _tp_acl_cp_user_get(ip, port, user_out, SWARM_IOCTL_CP_USER_GET, NULL, NULL);
}

inline int
tp_acl_cp_user_get_for_post_auth (char * ip, char *dst_ip, __u16 port, swarm_info_user_t *user_out)
{
    return _tp_acl_cp_user_get(ip, port, user_out, SWARM_IOCTL_CP_USER_GET, NULL, dst_ip);
}

inline int
tp_acl_cp_user_get_chk_dnat (char * ip, __u16 port, swarm_info_user_t *user_out, char *ap_ip)
{
    /*ap_ip is the IP address of br0:1*/
    return _tp_acl_cp_user_get(ip, port, user_out, SWARM_IOCTL_CP_USER_GET_CHK_DNAT, ap_ip, NULL);
}

int
bind_socket (int sockfd, const char *addr, int family)
{
        struct addrinfo hints, *res, *ressave;

        assert (sockfd >= 0);
        assert (addr != NULL && strlen (addr) != 0);

        memset (&hints, 0, sizeof (struct addrinfo));
        hints.ai_family = family;
        hints.ai_socktype = SOCK_STREAM;

        /* The local port it not important */
        if (getaddrinfo (addr, NULL, &hints, &res) != 0)
                return -1;

        ressave = res;

        /* Loop through the addresses and try to bind to each */
        do {
                if (bind (sockfd, res->ai_addr, res->ai_addrlen) == 0)
                        break;  /* success */
        } while ((res = res->ai_next) != NULL);

        freeaddrinfo (ressave);
        if (res == NULL)        /* was not able to bind to any address */
                return -1;

        return sockfd;
}

char *
shared_mem_init(const char *filename, int create, unsigned int size_needed)
{
    int r, fd;
    int cfl = 0;
    void *mp = NULL;

    if (create) {
        cfl = O_CREAT|O_EXCL;
    }
    fd = open(filename, O_RDWR|cfl, 0600);

    if (fd == -1) {
        fd = open(filename, O_RDWR, 0600);
    }

    if (fd < 0) {
        return NULL;
    }

    r = fcntl(fd, F_GETFD, 0);
    if (r == -1) {
        close(fd);
        return NULL;
    }
    r = fcntl(fd, F_SETFD, r | FD_CLOEXEC);
    if (r == -1) {
        close(fd);
        return NULL;
    }

    r = ftruncate(fd, size_needed);
    if (r == -1) {
        close(fd);
        return NULL;
    }

    mp = mmap(0, size_needed, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (mp == MAP_FAILED) {
        close(fd);
        return NULL;
    }

    if (create) {
        memset(mp, 0, size_needed);
    }

    return (char *)mp;
}

#ifndef __OEM_siemens__
int create_facebook_http_redirect_header (const char *url, char *msg, int maxlen)
{
    static const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";
    char timebuf[256];
    time_t now;

    const char *headers =
        "HTTP/1.1 302\r\n"
        "Server:\r\n"
        "Date: %s\r\n"
        "Cache-Control: no-cache,no-store,must-revalidate,post-check=0,pre-check=0\r\n"
        "Location: %s\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html>\r\n"
        "    <head>\r\n"
        "        <title>Redirecting to Facebook</title>\r\n"
        "        <body>If you are not redirected with 5 seconds, please <a href=\"%s\">click here</a>.</body>\r\n"
        "    </head>\r\n"
        "    <body>\r\n"
        "    </body>\r\n"
        "</html>\r\n";

    now = time(NULL); 
    strftime( timebuf, sizeof(timebuf), rfc1123_fmt, gmtime( &now ) );
    return snprintf(msg, maxlen, headers, timebuf, url, url);
}
#endif

int
create_http_filtered_redirect_header (const char *title, const char *url, char *msg, int maxlen)
{
    static const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";
    char timebuf[256];
    time_t now;

    const char *headers =
        "HTTP/1.1 200 Ok\r\n"
        "Server:\r\n"
        "Date: %s\r\n"
        "Cache-Control: no-cache,no-store,must-revalidate,post-check=0,pre-check=0\r\n"
        "\r\n"
        "<html> \r\n"
        "    <head> \r\n"
        "        <meta http-equiv=\"Refresh\" content=\"0; url=%s\">\r\n"
        "    </head> \r\n"
        "    <body> \r\n"
        "        <h1>%s</h1> \r\n"
        "    </body> \r\n"
        "</html> \r\n";

    now = time(NULL); 
    strftime( timebuf, sizeof(timebuf), rfc1123_fmt, gmtime( &now ) );

    return snprintf(msg, maxlen, headers, timebuf, url, title);
}

int
create_http_redirect_header (const char *title, const char *url, char *msg, int maxlen)
{
    static const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";
    char timebuf[256];
    time_t now;
    static int seconds = 3;

    const char *headers =
        "HTTP/1.1 200 Ok\r\n"
        "Server:\r\n"
        "Date: %s\r\n"
        "Cache-Control: no-cache,no-store,must-revalidate,post-check=0,pre-check=0\r\n"
        "\r\n"
        "<html> \r\n"
        "    <head> </head> \r\n"
        "    <body> \r\n"
        "      <h1>%s</h1> \r\n"
        "      <hr/> \r\n"
        "      <h3>Redirecting in <span id=\"leftTxt\"></span> sec</h3> \r\n"
        "      <script type=\"text/javascript\"> \r\n"
        "        var secondsLeft = %d; \r\n"
        "        var timer,wait; \r\n"
        "        document.getElementById('leftTxt').innerHTML = secondsLeft; \r\n"
        "        wait = function() { \r\n"
        "           secondsLeft--; \r\n"
        "           document.getElementById('leftTxt').innerHTML = secondsLeft; \r\n"
        "           if (secondsLeft == 1) { \r\n"
        "               clearInterval(timer); \r\n"
        "               window.location.replace('%s'); \r\n"
        "           } \r\n"
        "        }; \r\n"
        "        timer = setInterval(wait, 1000); \r\n"
        "      </script> \r\n"
        "    </body> \r\n"
        "    <script> \r\n"
        "        function doRedirect() { \r\n"
        "             window.location.replace('%s'); \r\n"
        "        } \r\n"
        "    </script> \r\n"
        "</html> \r\n";

    now = time(NULL); 
    strftime( timebuf, sizeof(timebuf), rfc1123_fmt, gmtime( &now ) );

    return snprintf(msg, maxlen, headers, timebuf, title, seconds, url, url);
}

const char *
iap_get_domainname(void)
{
    static char domain[256] = "";
    FILE *fp;
    
    if (domain[0] == 0) {
        if ((fp = fopen("/tmp/oem_domainname", "r")) != NULL) {
            if (1 != fscanf(fp, "%s", domain)) {
                domain[0] = 0;
            }
            fclose(fp);
        }
        if (domain[0] == 0) {
            strcpy(domain, "arubanetworks.com");
        }
    }
    return domain;
}

int authenticate_client (const char *tag, swarm_info_user_t *user, const char *client_ip_addr, int client_port, const char *fb_token)
{
    char    str[128];
    pid_t   pid;

    sprintf (str, "%d", client_port);

    debug_log(tag, "%s: %s: %d: Authenticating user IP %s NATTed IP %s port %d mac %s on essid '%s'",
        __FILE__, __func__, __LINE__, ip_to_str(user->v4_ip), client_ip_addr, client_port, mac_to_str(user->mac), user->essid);

    setenv ("REMOTE_ADDR", client_ip_addr, 1);
    setenv ("REMOTE_PORT", str, 1);
    setenv ("REQUEST_METHOD", "GET", 1);
    setenv ("QUERY_STRING", "opcode=external_cp_auth", 1);
    setenv (ECP_AUTH_TOKEN_ENVP, "1", 1);
    if (fb_token != NULL)
        setenv (ECP_FACEBOOK_TOKEN, fb_token, 1);
    if ((pid = fork ()) > 0) {
    	/* parent */
        debug_log(tag, "%s: %s: %d: Authenticated user IP %s NATTed IP %s port %d mac %s on essid '%s'",
            __FILE__, __func__, __LINE__, ip_to_str(user->v4_ip), client_ip_addr, client_port, mac_to_str(user->mac), user->essid);
	    return (0);
    }
    if (pid < 0) {
        error_log(tag, "%s: %s: %d: Failed to authenticate user due to internal error",
            __FILE__, __func__, __LINE__);
        return -1;
    }
    sleep(2);
    execl ("/etc/httpd/swarm.cgi", "/etc/httpd/swarm.cgi", (char *) 0);
    exit (1);
}

int 
connect_w_timeout(const char *tag, int sock, struct sockaddr *addr, int addrlen, int timeout)
{   
    int flags = 0, error = 0, ret = 0;
    fd_set  rset, wset;
    socklen_t   len = sizeof(error);
    struct timeval  ts;

    if (!timeout) {
        return connect(sock, addr, addrlen);
    }
    
    ts.tv_sec = timeout;
    ts.tv_usec = 0;
    
    /* clear out descriptor sets for select */
    /* add socket to the descriptor sets */
    FD_ZERO(&rset);
    FD_SET(sock, &rset);
    wset = rset;    /* structure assignment ok */
    
    /* set socket nonblocking flag */
    if( (flags = fcntl(sock, F_GETFL, 0)) < 0) {
        error_log(tag, "%s: %d: could not set nonblocking, err '%s'", __func__, __LINE__, strerror(errno));
        return connect(sock, addr, addrlen);
    }
    
    if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
        error_log(tag, "%s: %d: could not set nonblocking, err '%s'", __func__, __LINE__, strerror(errno));
        return connect(sock, addr, addrlen);
    }
    
    /* initiate non-blocking connect */
    if( (ret = connect(sock, addr, addrlen)) < 0 ) {
        if (errno != EINPROGRESS) {
            error_log(tag, "%s: %d: connect failed with ret=%d error='%s'",
                __func__, __LINE__, ret, strerror(errno));
            return ret;
        }
    }

    if(ret == 0) {
        /* then connect succeeded right away */
        goto done;
    }
    
    /* we are waiting for connect to complete now */
    if( (ret = select(sock + 1, &rset, &wset, NULL, &ts)) < 0) {
        error_log(tag, "%s: %d: select failed with ret=%d error='%s'",
            __func__, __LINE__, ret, strerror(errno));
        return -1;
    }

    if(ret == 0){   /* we had a timeout */
        error_log(tag, "%s: %d: connection timedout", __func__, __LINE__);
        errno = ETIMEDOUT;
        return -1;
    }

    /* we had a positivite return so a descriptor is ready */
    if (FD_ISSET(sock, &rset) || FD_ISSET(sock, &wset)){
        if(getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            error_log(tag, "%s: %d: getsockopt failed with ret=%d error='%s'",
                __func__, __LINE__, ret, strerror(errno));
            return -1;
        }
    }else {
        error_log(tag, "%s: %d: socket not read and write ready",
            __func__, __LINE__);
        return -1;
    }

    if(error){  /* check if we had a socket error */
        errno = error;
        error_log(tag, "%s: %d: socket error: '%s'",
            __func__, __LINE__, strerror(errno));
        return -1;
    }

done:
    /* put socket back in blocking mode */
    if(fcntl(sock, F_SETFL, flags) < 0) {
        error_log(tag, "%s: %d: could not unset blocking: %s",
            __func__, __LINE__, strerror(errno));
        return -1;
    }

    return 0;
}

int
opensock_w_timeout (const char *tag, const char *host, int port, const char *bind_to, const char *bind_address)
{
        int sockfd, n;
        struct addrinfo hints, *res, *ressave;
        char portstr[6];

        assert (host != NULL);
        assert (port > 0);

        memset (&hints, 0, sizeof (struct addrinfo));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        snprintf (portstr, sizeof (portstr), "%d", port);

        n = getaddrinfo (host, portstr, &hints, &res);
        if (n != 0) {
                error_log(tag,
                             "opensock: Could not retrieve info for %s:%s, ret=%d, errno=%d", host, portstr, n, errno);
                return -1;
        }

        ressave = res;
        do {
                sockfd =
                    socket (res->ai_family, res->ai_socktype, res->ai_protocol);
                if (sockfd < 0)
                        continue;       /* ignore this one */

                /* Bind to the specified address */
                if (bind_to) {
                        if (bind_socket (sockfd, bind_to,
                                         res->ai_family) < 0) {
                                error_log(tag, "%s: %d: failed to binding socket %d", __func__, __LINE__, sockfd);
                                close (sockfd);
                                continue;       /* can't bind, so try again */
                        }
                } else if (bind_address) {
                        if (bind_socket (sockfd, bind_address,
                                         res->ai_family) < 0) {
                                error_log(tag, "%s: %d: failed to binding address", __func__, __LINE__);
                                close (sockfd);
                                continue;       /* can't bind, so try again */
                        }
                }
                if (
                    connect_w_timeout(tag, sockfd, res->ai_addr, res->ai_addrlen, 10)
                    == 0) {
                        break;  /* success */
                }

                close (sockfd);
        } while ((res = res->ai_next) != NULL);

        freeaddrinfo (ressave);
        if (res == NULL) {
                error_log(tag,
                             "opensock: Could not establish a connection to %s",
                             host);
                return -1;
        }

        return sockfd;
}
#ifndef __OEM_siemens__
void encode_fb_url(char *orig_url, int len, char *en_url)
{
    int i = 0;
    char *src, *des;
    int n = 0;;

    src = orig_url;
    des = en_url;

    for (i = 0; i < len; i ++){
        switch(src[i])
        {
            case ':':
            n = sprintf(des, "%s%%3A", des);
            break;

            case '/':
            n = sprintf(des, "%s%%2F", des);
            break;

            case '=':
            n = sprintf(des, "%s%%3D", des);
            break;

            case '?':
            n = sprintf(des, "%s%%3F", des);
            break;

            default:
            des[n] = src[i];
            n++;
            break;
        }
    }

    return;
}

void decode_orig_url(char *url, int len, char *de_url)
{
    int i = 0;
    char *src, *des;

    src = url;
    des = de_url;

    while(i < len){
        if (*src == '%'){
            if (!strncmp(src + 1, "3A", 2)){
               *des = ':';
            }
            if (!strncmp(src + 1, "2F", 2)){
               *des = '/';
            }
            if (!strncmp(src + 1, "3D", 2)){
               *des = '=';
            }
            if (!strncmp(src + 1, "3F", 2)){
               *des = '?';
            }
            if (!strncmp(src + 1, "3B", 2)){
               *des = ';';
            }
            if (!strncmp(src + 1, "24", 2)){
               *des = '$';
            }
            if (!strncmp(src + 1, "2C", 2)){
               *des = ',';
            }
            if (!strncmp(src + 1, "3C", 2)){
               *des = '<';
            }
            if (!strncmp(src + 1, "3E", 2)){
               *des = '>';
            }
            if (!strncmp(src + 1, "5E", 2)){
               *des = '^';
            }
            if (!strncmp(src + 1, "60", 2)){
               *des = '`';
            }
            if (!strncmp(src + 1, "5C", 2)){
               *des = '\\';
            }
            if (!strncmp(src + 1, "5B", 2)){
               *des = '[';
            }
            if (!strncmp(src + 1, "5D", 2)){
               *des = ']';
            }
            if (!strncmp(src + 1, "7B", 2)){
               *des = '{';
            }
            if (!strncmp(src + 1, "7D", 2)){
               *des = '}';
            }
            if (!strncmp(src + 1, "7C", 2)){
               *des = '|';
            }
            if (!strncmp(src + 1, "5C", 2)){
               *des = '\"';
            }
            if (!strncmp(src + 1, "23", 2)){
               *des = '#';
            }
            if (!strncmp(src + 1, "26", 2)){
               *des = '&';
            }
            if (!strncmp(src + 1, "40", 2)){
               *des = '@';
            }
            if (!strncmp(src + 1, "25", 2)){
               *des = '%';
            }
            if (!strncmp(src + 1, "28", 2)){
               *des = '(';
            }
            if (!strncmp(src + 1, "29", 2)){
               *des = ')';
            }
            if (!strncmp(src + 1, "2B", 2)){
               *des = '+';
            }
            if (!strncmp(src + 1, "7E", 2)){
               *des = '~';
            }
            src += 3;
            des += 1;
            i += 3;
        }
        else{
            *des = *src;
            src ++;
            des += 1;
            i ++;
        }
    }
    return;
}

int calc_fb_hash(char *id, char *key, char *cookie, char *out, int flag, char *redirect_domain)
{
    char data[512] = {0};
    int data_len = 0;
    unsigned char en_string[512] = {0};
    unsigned char md[512] = {0};
    char *to = out;
    int i = 0;
    int ret = 0;
    int en_len = sizeof(en_string);

    if(id == NULL || key == NULL || cookie == NULL || out == NULL){
        syslog(LOG_DEBUG, "%s, %d: input error", __func__, __LINE__);
        return 1;
    }

    syslog(LOG_DEBUG, "%s, %d: input id is %s, key is %s, cookie is %s", __func__, __LINE__, id, key, cookie);

    if(flag){
        data_len = snprintf(data, sizeof(data), "%s||https://%s/%s/?c=%s", id, redirect_domain,  FB_INTERNAL_CONTINUE_PATH, cookie);
    }
    else
        data_len = snprintf(data, sizeof(data), "%s||https://%s/%s/?c=%s", id, redirect_domain, FB_INTERNAL_AUTH_PATH, cookie);

    syslog(LOG_DEBUG, "%s, %d: for hmac_hash  key is %s, key len is %d, data is %s, data_len is %d", __func__, __LINE__, key, strlen(key), data, data_len);
    hmac_hash(EVP_sha256(), key, strlen(key), data, data_len, md);
    {
        /*hard code here*/
        if((ret = base64_encode(en_string, &en_len, md, 32/*SHA256_DIGEST_SIZE*/)) != 0){
            en_len = 0;
        }
       syslog(LOG_DEBUG, "%s, %d: encode date is %s", __func__, __LINE__, en_string);
       if(en_len > 0){
           for (i = 0; i < en_len - 1; i ++){
               if (en_string[i] == '='){
                   continue;
               }else if (en_string[i] == '+'){
                   *to = '-';
                    to ++;
               }else if(en_string[i] == '/'){
                   *to = '_';
                    to ++;
               }else{
                   *to = en_string[i];
                    to ++;
               }
           }
           return 0;
       }
       else
           return 1;
    }
}

int calc_fb_cookie_hash(char *mac, char *url, char *out)
{
    unsigned char data[512] = {0};
    int data_len = 0;
    unsigned char en_string[512] = {0};
    unsigned char md[512] = {0};
    char *to = out;
    int i = 0;
    int ret = 0;
    int en_len = sizeof(en_string);

    data_len = snprintf(data, sizeof(data), "%s||%s", mac, url);

    syslog(LOG_DEBUG, "%s, %d: the original data is %s", __func__, __LINE__, data);
    md5_calc(md, data, data_len);
    if(md[0] != 0){
        /*hard code here*/
        if((ret = base64_encode(en_string, &en_len, md, strlen(md)/*MD5_DIGEST_SIZE*/)) != 0){
            en_len = 0;
        }
       syslog(LOG_DEBUG, "%s, %d: encode date is %s", __func__, __LINE__, en_string);
       if(en_len > 0){
           for (i = 0; i < en_len - 1; i ++){
               if (en_string[i] == '='){
                   continue;
               }else if (en_string[i] == '+'){
                   *to = '-';
                    to ++;
               }else if(en_string[i] == '/'){
                   *to = '_';
                    to ++;
               }else{
                   *to = en_string[i];
                    to ++;
               }
           }
           return 0;
       }
       else
           return 1;
    }

    return 1;
}
#endif

void
convert_path(const char *spath, char *dpath, int size)
{
    __u8 *sp = (__u8 *)spath;
    char *dp = dpath;
    
    for (; *sp != 0 && (dp - dpath) < size - 1; sp++, dp += 2) {
        sprintf(dp, "%02x", *sp);
    }
    *dp = 0;
}

int
read_cp_config(const char *name, const char *type, const char *item, char *value, int size)
{
    char pathname[256], cmd[512];
    FILE *fp;

    if (!name || name[0] == 0) {
        return -1;
    }
    convert_path(name, pathname, sizeof(pathname));
    snprintf(cmd, sizeof(cmd), "/tmp/%s/%s/%s", type, pathname, item);
    if ((fp = fopen(cmd, "r")) == NULL) {
        return -1;
    }
    if (fgets(value, size, fp) == NULL) {
        fclose(fp);
        return -1;
    }
    value[strcspn(value, "\r\n")] = 0;
    fclose(fp);
    return 0;
}
