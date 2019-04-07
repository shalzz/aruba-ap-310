#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <syslog.h>
#include "wispr.h"

int g_wispr_client_type;

static int is_generic_wispr_client(char *e)
{
    const char *other_agents[] = {
        WI_2_USER_AGENT,
        UQ_WIFI_USER_AGENT           
    };
    int count = sizeof(other_agents)/sizeof(other_agents[0]);
    int i=0;

    /*  
     * AU_WIFI_USER_AGENT is specific to the customer KDDI
     * KDDI requires a full string match for this UA string
     */
    if (!strcmp((const char *)e, AU_WIFI_USER_AGENT)) {
       g_wispr_client_type = WISPR_CLIENT_GENERIC;
       return 1;

    } else {
        for (i=0; i < count; i++) {
            if (strstr((const char *)e, other_agents[i])) {
                g_wispr_client_type = WISPR_CLIENT_GENERIC;
                return 1;
            }
        }
    }
    return 0;
}
static int is_wispr_2_client(char *e) {
    if (strstr(e, WISPR_2_0_PREFIX_USER_AGENT)) {
        g_wispr_client_type = WISPR_CLIENT_WISPR_2_0;
        return 1;
    }
    return 0;
}
static int is_ipass_client(char *e)
{
    if (strstr(e, IPASS_USER_AGENT)) {
        g_wispr_client_type = WISPR_CLIENT_IPASS;
        return 1;
    }
    return 0;
}
static int is_kontor_client(char *e)
{
    if (strstr(e, KONTOR_USER_AGENT)) {
        g_wispr_client_type = WISPR_CLIENT_KONTOR;
        return 1;
    }
    return 0;
}
static int is_boingo_client(char *e)
{
    if (strstr(e, BOINGO_USER_AGENT)) {
        g_wispr_client_type = WISPR_CLIENT_BOINGO;
        return 1;
    }
    return 0;
}
static int is_weroam_client(char *e)
{
    if (strstr(e, WEROAM_USER_AGENT)) {
        g_wispr_client_type = WISPR_CLIENT_WEROAM;
        return 1;
    }
    return 0;
}
static int is_trustive_client(char *e)
{
    if (strstr(e, TRUSTIVE_USER_AGENT)) {
        g_wispr_client_type = WISPR_CLIENT_TRUSTIVE;
        return 1;
    }
    return 0;
}
static int is_boingo_partner_client(char *e)
{
    if (strstr(e, INFONET_USER_AGENT) ||
        strstr(e, FIBERLINK_USER_AGENT) ||
        strstr(e, MCI_USER_AGENT) ||
        strstr(e, SKYPE_USER_AGENT) ||
        strstr(e, SKYPE_WIFI_USER_AGENT) ||/*temporally put it here*/
        strstr(e, BTI_USER_AGENT) ||
        strstr(e, VELOFONE_USER_AGENT) ||
        strstr(e, ALLTEL_USER_AGENT)
        ) {
        g_wispr_client_type = WISPR_CLIENT_BOINGO_PARTNER;
        return 1;
    }
    return 0;
}
static int is_weroam_partner_client(char *e)
{
    if (strstr(e, ATNT_USER_AGENT) ||
        strstr(e, ATT_USER_AGENT) ||
        strstr(e, NETCLIENT_USER_AGENT) 
        ) {
        g_wispr_client_type = WISPR_CLIENT_WEROAM_PARTNER;
        return 1;
    }
    return 0;
}

int is_wispr_client(char *a)
{
    syslog(2, "check wispr");
    if (is_ipass_client(a) ||
        is_kontor_client(a) ||
        is_boingo_client(a) ||
        is_trustive_client(a) ||
        is_weroam_client(a) ||
        is_boingo_partner_client(a) ||
        is_weroam_partner_client(a) ||
        is_generic_wispr_client(a) ||
        is_wispr_2_client(a)) {
        return 1;
    }
    return 0;
}

