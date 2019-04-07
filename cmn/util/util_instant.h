#ifndef UTIL_INSTANT_H
#define UTIL_INSTANT_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <asm/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>

/* copied from frame.h that I can't include here */
#define FRAME_IP_SET(S, V) { \
    (S)[0] = htons(ntohl(V) >> 16); \
    (S)[1] = htons(ntohl(V) & 0xffff); \
}
#define FRAME_IP_GET(A) (htonl(ntohs((A)[0]) << 16 | ntohs((A)[1]))) 
#define FRAME_IPCOPY(S, D) { \
    (S)[0] = (D)[0]; \
    (S)[1] = (D)[1]; \
}
/* copy done */
#include <util/sos_types.h>
#include <util/acl.h>
#include <wifi/swarm_ioctl_acl.h>

#ifndef _debug_log
#define _debug_log(tag, arg...) \
{\
    {\
    char file_name[64]; \
    FILE *my_log_file; \
    snprintf(file_name, sizeof(file_name)-1, "/tmp/debug.log"); \
    my_log_file = fopen(file_name, "at"); \
    if (my_log_file) { \
        time_t t_now = time(NULL); \
        char _buf[256] = ""; \
        int l; \
        ctime_r(&t_now, _buf); \
        l = strlen(_buf); \
        if (l && isspace(_buf[l-1])) { \
            _buf[l-1] = 0; \
        } \
        fprintf(my_log_file, "%s: %s[%u]: ", _buf, tag, getpid()); \
        fprintf(my_log_file, arg); \
        fprintf(my_log_file, "\n"); \
        fclose(my_log_file); \
    } \
 } \
}
#endif

#ifndef error_log
#define error_log(tag, arg...) syslog(LOG_ERR, arg)
#endif

#ifndef debug_log
#define debug_log(tag, arg...) syslog(LOG_DEBUG, arg)
#endif

#ifndef UTIL_H
#ifndef mac_to_str
static inline char *
mac_to_str(__u8 *mac)
{
    static char mac_str[19];
    snprintf(mac_str, sizeof(mac_str)-1, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return mac_str;
}
#endif

#ifndef ip_to_str
static inline char *
ip_to_str(__u32 ip)
{
    struct in_addr inaddr;
    inaddr.s_addr = ip;
    return inet_ntoa(inaddr);
}
#endif

#ifndef str_to_ip
static inline __u32
str_to_ip (char * str)
{
    return ((__u32) inet_addr (str));
}
#endif
#endif

int tp_acl_cp_user_get (char * ip, __u16 port, swarm_info_user_t *user_out);
int tp_acl_cp_user_session_get (char * ip, __u16 port, swarm_info_user_t *user_out);
int tp_acl_cp_user_get_chk_dnat (char * ip, __u16 port, swarm_info_user_t *user_out, char *ap_ip);
int tp_acl_https_user_session_get (char * ip, __u16 port, swarm_info_user_t *user_out);
int tp_acl_cp_user_get_for_post_auth (char * ip, char *dst_ip, __u16 port, swarm_info_user_t *user_out);
extern int bind_socket (int sockfd, const char *addr, int family);
extern int connect_w_timeout(const char *tag, int sock, struct sockaddr *addr, int addrlen, int timeout);
extern int opensock_w_timeout (const char *tag, const char *host, int port, const char *bind_to, const char *bind_address);

#define SWARM_ECP_SHARED_MEM_FILE "/tmp/ecp_shared"
#define SWARM_ECP_SHARED_MEM_SIZE (64 * 1024)
#define SWARM_ECP_PROXY_SHARED_MEM_FILE "/tmp/ecp_proxy_shared"
#define SWARM_ECP_PROXY_NUMBER  20 /*Max is 20 in /etc/tinyproxy.conf*/
#define SWARM_ECP_PROXY_SHARED_MEM_SIZE (64 * SWARM_ECP_PROXY_NUMBER) 
#define SWARM_ECP_JS_REDIRECT     "<script> \n\
    document.location.href = \"%s\" \n\
    </script>\n"
#define SWARM_ECP_FAILTHROUGH_MSG "Sorry, Due to internet connectivity issues, this service is temporarily unavailable!"

#define SWARM_SID_SHARED_MEM_FILE "/tmp/sid_shared"
#define SWARM_SID_SHARED_MEM_SIZE (12 * 1024)
#define SWARM_SID_SHARED_NUMBER 128
#define SWARM_RADIUS_URL_MEM_FILE "/tmp/radius_url_shared"
#define SWARM_RADIUS_URL_NUMBER 256
#define SWARM_RADIUS_URL_MEM_SIZE (262 * SWARM_RADIUS_URL_NUMBER) 
#define SWARM_OUT_OF_SERVICE_MEM_FILE "/tmp/cli_oos_shared"
#define SWARM_OUT_OF_SERVICE_NUMBER 4
#define SWARM_OUT_OF_SERVICE_MEM_SIZE   16 
#define SWARM_AUTH_SURV_SHARED_MEM_FILE "/tmp/auth_surv_shared"
#define SWARM_SSID_INFO_SHARED_MEM_FILE "/tmp/ssid_info_shared"
#define SWARM_SSID_INFO_NUMBER CLI_MAX_SSID
#define SWARM_SSID_INFO_SHARED_MEM_SIZE (256 * SWARM_SSID_INFO_NUMBER)
#define SIZE_ARUBA_NAMED_VLAN  128
#define SIZE_ARUBA_ROLE        128
#define SIZE_MS_TUNNEL_PRIVATE_GROUP_ID  256
#define SIZE_PW_USER_NAME                256

typedef struct {
    char  username[256];
    __u8  password[16];
    char  mac[6];
    __u16 aruba_vlan;
    char  aruba_named_vlan[SIZE_ARUBA_NAMED_VLAN + 1];
    __u8  aruba_no_dhcp_fingerprint;
    char  aruba_role[SIZE_ARUBA_ROLE + 1];
    __u8  ms_tunnel_type;
    __u8  ms_tunnel_medium_type;
    char  ms_tunnel_private_group_id[SIZE_MS_TUNNEL_PRIVATE_GROUP_ID];
    char  pw_user_name[SIZE_PW_USER_NAME];
    __u32 pw_session_timeout;
    __u32 timestamp;
} cli_auth_surv_t;

typedef struct {
    __u16 aruba_vlan;
    char  aruba_named_vlan[SIZE_ARUBA_NAMED_VLAN + 1];
    __u8  aruba_no_dhcp_fingerprint;
    char  aruba_role[SIZE_ARUBA_ROLE + 1];
    __u8  ms_tunnel_type;
    __u8  ms_tunnel_medium_type;
    char  ms_tunnel_private_group_id[SIZE_MS_TUNNEL_PRIVATE_GROUP_ID];
    char  pw_user_name[SIZE_PW_USER_NAME];
    __u32 pw_session_timeout;
} cli_auth_surv_user_role_t;

#define SWARM_AUTH_SURV_NUMBER 2048
#define SWARM_AUTH_SURV_SHARED_MEM_SIZE (sizeof(cli_auth_surv_t) * SWARM_AUTH_SURV_NUMBER)  /*sieof (cli_auth_surv_t) * 2048kb, struct size 1064  */

typedef struct {
    cli_auth_surv_t user[SWARM_AUTH_SURV_NUMBER];
}cli_shared_auth_surv_t;

typedef struct {
    char name[SWARM_ECP_NAME_LEN];
    char server[SWARM_ECP_SERVER_LEN];
    int  port;
    char auth_url[SWARM_ECP_URL_LEN];
    char oos_url[SWARM_ECP_URL_LEN];
    char redirect_url[SWARM_ECP_REDIRECT_URL_LEN];
    char query_in_auth_url;
    char use_https;
    char server_failthrough;
    char is_down;
	char cp_offload;
    char prevent_frame_overlay;
    char switch_ip;
    time_t last_checked;
} ecp_server_cfg_t;

typedef struct {
    char ap_name[256];
    char vc_name[256];
    ecp_server_cfg_t ecp_server[16];
    char ap_mac[32];
} ecp_cfg_t;

typedef struct {
    char cmac[32];
    char cip[16];
    int  cport;
    int  sport;
    int  pid;
} ecp_proxy_t;

typedef struct {
    ecp_proxy_t client_info[SWARM_ECP_PROXY_NUMBER];
}ecp_proxy_info_t;

typedef struct {
    __u32 ip;
    char  user[32];
    char  sid[32];
    int   sid_type;
} cli_shared_sid_t;

typedef struct {
    cli_shared_sid_t session_id[SWARM_SID_SHARED_NUMBER];
}cli_sid_info_t; 

typedef struct {
    char mac[6];
    char redirect_url[256];
} cli_radius_redirect_url_t;

typedef struct {
    cli_radius_redirect_url_t url[SWARM_RADIUS_URL_NUMBER];
}cli_radius_url_info_t; 

#define CLI_MAX_NAS_ID_SIZE 64
typedef struct {
    char  ssid[CLI_MAX_SSID_LEN + 1];
    char  nas_id[CLI_MAX_NAS_ID_SIZE + 1];
} cli_ssid_info_t;

typedef struct {
    cli_ssid_info_t ssid_list[SWARM_SSID_INFO_NUMBER];
}cli_shared_ssid_info_t;

#define   OOS_VPN   0
#define   OOS_UPLINK   1
#define   OOS_INTERNET_REACHABLE  2
#define   OOS_PRI_UPLINK     3

typedef struct {
    int status[SWARM_OUT_OF_SERVICE_NUMBER];
}cli_out_of_service_t; 


/* Auto white list */
#define AUTO_WG_SHARED_MEM_SIZE     (32 * 1024)

#define SIZE_WG_NAME                64
#define AUTO_WG_MAX_NAMES           128

struct auto_filter_list {
    char pat[SIZE_WG_NAME];
    /* regex_t cpat; */
    time_t atime;
    char nl;
};

typedef struct {
    struct auto_filter_list  filter[AUTO_WG_MAX_NAMES];
    int count;
} auto_wg_t;

extern char *shared_mem_init(const char *filename, int create, unsigned int size_needed);
extern int create_http_redirect_header (const char *title, const char *url, char *msg, int maxlen);
extern int authenticate_client (const char *tag, swarm_info_user_t *user, const char *client_ip_addr, int client_port, const char *fb_token);
extern const char * iap_get_domainname(void);

extern int create_http_filtered_redirect_header (const char *title, const char *url, char *msg, int maxlen);
#ifndef __OEM_siemens__
extern int create_facebook_http_redirect_header (const char *url, char *msg, int maxlen);
extern int calc_fb_hash(char *id, char *key, char *cookie, char *out, int flag, char *redirect_domain);
extern int calc_fb_cookie_hash(char *mac, char *url, char *out);
extern void encode_fb_url(char *orig_url, int len, char *en_url);
extern void decode_orig_url(char *url, int len, char *de_url);
extern int do_base64(const unsigned char *input, int in_len, unsigned char *out);
#endif
extern void convert_path(const char *spath, char *dpath, int size);
extern int read_cp_config(const char *name, const char *type, const char *item, char *value, int size);

#ifndef __OEM_siemens__
#define FB_CONFIG_URL "https://www.facebook.com/wifiauth/config?gw_id="
#define FB_REDIRECT_URL "https://www.facebook.com/wifiauth/login/"
#define FB_PORTAL_URL "https://www.facebook.com/wifiauth/portal/"
#define FB_EN_INTERNAL_AUTH_URL_HEAD "https%3A%2F%2F"
#define FB_EN_INTERNAL_AUTH_URL_TAIL "%2Ffbwifi%2Fauth%2F"
#define FB_INTERNAL_AUTH_PATH "fbwifi/auth"
#define FB_EN_INTERNAL_CONTINUE_URL_HEAD "https%3A%2F%2F"
#define FB_EN_INTERNAL_CONTINUE_URL_TAIL "%2Ffbwifi%2Fcontinue%2F"
#define FB_INTERNAL_CONTINUE_URL "https://securelogin.arubanetworks.com/fbwifi/continue/"
#define FB_INTERNAL_CONTINUE_PATH "fbwifi/continue"
#define FB_FORWARD_PATH "fbwifi/forward"
#define FB_GET_ACCESS_TOKEN "https://graph.facebook.com/"
#define FB_ACCESS_TOKEN_FILE "/tmp/facebook_access_token"
#define FB_AUTH_OK_MSG "[Facebook]: You have been authenticated through your facebook account!"
#endif
#define ECP_SERVER_STATUS_EXPIRE_TIME 300

static inline time_t
get_uptime (void)
{
    unsigned long upsecs = 0;
    FILE *fp = 0;

    fp = fopen("/proc/uptime", "r");
    if(fp) {
        fscanf(fp, "%lu", &upsecs);
        fclose(fp);
    }
    return upsecs;
}

static inline int
ecp_server_is_down(const char *tag, ecp_server_cfg_t *ecp_server_cfg, const char *bind_to, const char *bind_address)
{
    time_t time_now = get_uptime();
    time_t time_expire = ecp_server_cfg->last_checked + ECP_SERVER_STATUS_EXPIRE_TIME;

    if (ecp_server_cfg->last_checked == 0 || time_expire < time_now) {
        int fd = opensock_w_timeout(tag, ecp_server_cfg->server, ecp_server_cfg->port, bind_to, bind_address);
        if (fd > 0) {
            close(fd);
            ecp_server_cfg->is_down = 0;
        } else {
            ecp_server_cfg->is_down = 1;
        }
        ecp_server_cfg->last_checked = time_now;
    }

    return ecp_server_cfg->is_down;
}

static inline ecp_cfg_t *
ecp_cfg_init(int create)
{
    return (ecp_cfg_t *)shared_mem_init(SWARM_ECP_SHARED_MEM_FILE, create, SWARM_ECP_SHARED_MEM_SIZE);
}
static inline ecp_proxy_info_t *
ecp_proxy_init(int create)
{
    return (ecp_proxy_info_t *)shared_mem_init(SWARM_ECP_PROXY_SHARED_MEM_FILE, create, SWARM_ECP_PROXY_SHARED_MEM_SIZE);
}
static inline cli_radius_url_info_t *
cli_radius_url_init(int create)
{
    return (cli_radius_url_info_t *)shared_mem_init(SWARM_RADIUS_URL_MEM_FILE, create, SWARM_RADIUS_URL_MEM_SIZE);
}

static inline cli_out_of_service_t *
cli_out_of_service_init(int create)
{
    return (cli_out_of_service_t *)shared_mem_init(SWARM_OUT_OF_SERVICE_MEM_FILE, create, SWARM_OUT_OF_SERVICE_MEM_SIZE);
}
static inline auto_wg_t *
get_auto_wg_from_ecp(ecp_cfg_t *ecp_cfg)
{
    return (auto_wg_t *)(((void *)ecp_cfg) + SWARM_ECP_SHARED_MEM_SIZE - AUTO_WG_SHARED_MEM_SIZE);
}

static inline cli_sid_info_t *
cli_shared_session_init(int create)
{
    return (cli_sid_info_t *)shared_mem_init(SWARM_SID_SHARED_MEM_FILE, create, SWARM_SID_SHARED_MEM_SIZE);
}

static inline cli_shared_auth_surv_t *
cli_shared_auth_surv_init(int create)
{
    return (cli_shared_auth_surv_t *)shared_mem_init(SWARM_AUTH_SURV_SHARED_MEM_FILE, create, SWARM_AUTH_SURV_SHARED_MEM_SIZE);
}

static inline cli_shared_ssid_info_t *
cli_shared_ssid_info_init(int create)
{
    return (cli_shared_ssid_info_t *)shared_mem_init(SWARM_SSID_INFO_SHARED_MEM_FILE, create, SWARM_SSID_INFO_SHARED_MEM_SIZE);
}

static inline int 
url_encode_char (char ch)
{
    static char encode_char[] = {';', '=', '$', ',', '<', '>', '^', '`', '\\', '[', ']', '{', '}', '|',
                                 '\"', ':', '/', '#', '?', '&', '@', '%', '+', '~', '(', ')', 0};
    int i;

    i = 0;
    while (encode_char[i] != 0) {
        if (encode_char[i] == ch)
            return (1);
        i++;
    }
    return (0);
}
static inline char *
encode_str (const char * str, char * estr, int len)
{
    char      * p;

    p = estr;
    p[0] = 0;
    while (*str && (len > 1)) {
        if (*str <= 32 || *str > 122 || url_encode_char (*str)) {
            if (len > 3) {
                snprintf (p, len, "%%%02X", *str & 0xff);
                p += 3;
                len -= 3;
                str++;
            }
            else
                break;
        }
        else {
            *p++ = *str++;
            len --;
        }
    }
    *p = 0;

    return estr;
}
#endif
