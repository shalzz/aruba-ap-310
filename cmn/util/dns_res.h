#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <string.h>
#define CLI_IP_MODE_V4_ONLY                 0
#define CLI_IP_MODE_V4_PREFER               1
#define CLI_IP_MODE_V6_PREFER               2
#define CLI_IP_MODE_V6_ONLY                 3
#ifndef DEBUG_EXTRA_DNS_SERVER
#define DEBUG_EXTRA_DNS_SERVER "/tmp/extra_dns_server"
#endif

static inline char *ip_mode_file(int v) {
    static char file[16];
    sprintf(file,"/tmp/ip_mode_%u",v);
    return file;
}

static inline struct hostent * aruba_gethostbyname(char *host_name)
{
    struct hostent *host = NULL;
#if (__UCLIBC_MAJOR__ == 0 && __UCLIBC_MINOR__ == 9 && __UCLIBC_SUBLEVEL__ != 33 && __UCLIBC_SUBLEVEL__ != 34)
    res_init();
#endif
    if (access(ip_mode_file(CLI_IP_MODE_V4_ONLY), F_OK) == 0) {
        host = gethostbyname2(host_name, AF_INET);
    } else if (access(ip_mode_file(CLI_IP_MODE_V4_PREFER), F_OK) == 0){
        host = gethostbyname2(host_name, AF_INET);
        if (!host) {
            host = gethostbyname2(host_name, AF_INET6);
        }
    } else if (access(ip_mode_file(CLI_IP_MODE_V6_PREFER), F_OK) == 0){
        host = gethostbyname2(host_name, AF_INET6);
        if (!host) {
            host = gethostbyname2(host_name, AF_INET);
        }
    } else if (access(ip_mode_file(CLI_IP_MODE_V6_ONLY), F_OK) == 0){
        host = gethostbyname2(host_name, AF_INET6);
    }
    return host;
}
#define EXTRA_DNS_SERVER "8.8.8.8"
#ifndef MAXNS
#define MAXNS 3
#endif

#define DNS_BEST_EFFORT_TIME 3
/*Aruba_gethostbyname_without_res_init will NOT call res_init. */
static inline struct hostent * __aruba_gethostbyname_without_res_init(char *host_name)
{
    struct hostent *host = NULL;
    if (access(ip_mode_file(CLI_IP_MODE_V4_ONLY), F_OK) == 0) {
        host = gethostbyname2(host_name, AF_INET);
    } else if (access(ip_mode_file(CLI_IP_MODE_V4_PREFER), F_OK) == 0){
        host = gethostbyname2(host_name, AF_INET);
        if (!host) {
            host = gethostbyname2(host_name, AF_INET6);
        }
    } else if (access(ip_mode_file(CLI_IP_MODE_V6_PREFER), F_OK) == 0){
        host = gethostbyname2(host_name, AF_INET6);
        if (!host) {
            host = gethostbyname2(host_name, AF_INET);
        }
    } else if (access(ip_mode_file(CLI_IP_MODE_V6_ONLY), F_OK) == 0){
        host = gethostbyname2(host_name, AF_INET6);
    }
    return host;
}

static inline struct hostent * aruba_gethostbyname_with_best_effort(char * host_name)
{
    
    struct hostent *host = NULL;
    struct in_addr server_in_addr;
    int i,j;
    char extra_dns_server[32] = {0};
    FILE *fp = NULL;

    host = aruba_gethostbyname(host_name);
    if (host) {
        return host;
    }
    fp = fopen(DEBUG_EXTRA_DNS_SERVER,"r");
    if (fp){
        fscanf(fp,"%s",extra_dns_server);
        fclose(fp);
    } else {
        strncpy(extra_dns_server,EXTRA_DNS_SERVER,strlen(EXTRA_DNS_SERVER));
    }
    /* fill the extra dns server into _res */
    if (inet_aton(extra_dns_server,&server_in_addr)) {
        _res.nscount = MAXNS;
        for (j =0;j<MAXNS;j++) {
            _res.nsaddr_list[j].sin_addr = server_in_addr;
        }
        for (i =0; i < DNS_BEST_EFFORT_TIME; i ++) {
            host = __aruba_gethostbyname_without_res_init(host_name);
            if(host != NULL ) {
                break;
            }
        }
    }
    /*restore back to original dns servers. */
    res_init();
    return host;
}

static inline struct hostent *gethostbyname_with_best_effort(const char * server_name)
{
    struct hostent *host = NULL;
	struct in_addr server_in_addr;
    int i = 0,j=0;
    char extra_dns_server[32] = {0};
    FILE *fp = NULL;

    if (server_name == NULL) {
        return NULL;
    }
    
    host = gethostbyname(server_name);
    if (host != NULL) {
        return host;
    }

    fp = fopen(DEBUG_EXTRA_DNS_SERVER,"r");
    if (fp){
        fscanf(fp,"%s",extra_dns_server);
        fclose(fp);
    } else {
        strncpy(extra_dns_server,EXTRA_DNS_SERVER,strlen(EXTRA_DNS_SERVER));
    }
    /*fill the extra dns server into _res*/
	if(inet_aton(extra_dns_server,&server_in_addr)){
        res_init();
		_res.nscount = MAXNS;
        for (j =0; j < MAXNS; j ++) {
		    _res.nsaddr_list[j].sin_addr = server_in_addr;
        }
        for (i =0; i < DNS_BEST_EFFORT_TIME; i ++ ) {
            host = gethostbyname(server_name);
            if (host != NULL) {
                break;
            }
        }

    }

    /*restor back to orignial dns server*/
    res_init();
    return host;
}

static inline struct hostent *gethostbyname_with_extra_server(const char * server_name,const char * extra_dnsserver)
{
    struct hostent *host = NULL;
	struct in_addr server_in_addr;
    int i = 0,j=0;
    if (server_name == NULL) {
        return NULL;
    }
    for (i =0; i < DNS_BEST_EFFORT_TIME; i ++ ) {
        host = gethostbyname(server_name);
        if (host != NULL) {
            return host;
        }
    }
    /*fill the extra dns server into _res*/

    if (extra_dnsserver == NULL) {
        return NULL;
    }

	if(inet_aton(extra_dnsserver,&server_in_addr)){
		_res.nscount = MAXNS;
        for (j =0; j < MAXNS; j ++) {
		    _res.nsaddr_list[j].sin_addr = server_in_addr;
        }
        for (i =0; i < DNS_BEST_EFFORT_TIME; i ++ ) {
            host = gethostbyname(server_name);
            if (host != NULL) {
                break;
            }
        }

    }

    /*restor back to orignial dns server*/
    res_init();
    return host;
}

