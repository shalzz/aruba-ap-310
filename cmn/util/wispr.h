#ifndef _WISPR_H_

#define IPASS_DISCOVER_LOCATION  "discoverlocation"

/* WISPr 2.0 clients have WISPR! prefix */
#define WISPR_2_0_PREFIX_USER_AGENT     "WISPR!"

#define HARDCODE_LOC_NAME "HOTSPOT-NAME, AP-NAME"
#define HARDCODE_LOC_ID   "isocc=us,cc=1,ac=408,network=HOTSPOT-ID"

/* GIS Message Type */
#define GIS_MSG_TYPE_REDIR          100
#define GIS_MSG_TYPE_PROXY          110
#define GIS_MSG_TYPE_AUTH           120
#define GIS_MSG_TYPE_LOGOFF         130
#define GIS_MSG_TYPE_AUTH_POLL      140
#define GIS_MSG_TYPE_ABORT_LOGIN    150

/* GIS Response Code */
#define GIS_RSP_CODE_NO_ERR         0 
#define GIS_RSP_CODE_LOGIN_OK       50
#define GIS_RSP_CODE_LOGIN_FAIL     100 
#define GIS_RSP_CODE_SRV_ERR        102 
#define GIS_RSP_CODE_NO_AUTH_SRV    105 
#define GIS_RSP_CODE_LOGOFF_OK      150 
#define GIS_RSP_CODE_LOGIN_ABORT    151 
#define GIS_RSP_CODE_PROXY_DETECT   200 
#define GIS_RSP_CODE_AUTH_PENDING   201 
#define GIS_RSP_CODE_INT_ERR        255 

#define GIS_DELAY                   5 /* seconds */

#define NO_WISPR_CLIENT             0 
#define WISPR_CLIENT_IPASS          1 
#define WISPR_CLIENT_KONTOR         2 
#define WISPR_CLIENT_BOINGO         3 
#define WISPR_CLIENT_WEROAM         4 
#define WISPR_CLIENT_TRUSTIVE       5 
#define WISPR_CLIENT_BOINGO_PARTNER 6 
#define WISPR_CLIENT_WEROAM_PARTNER 6 
/* This is defined for generic clients which dont
   need any special processing
*/
#define WISPR_CLIENT_GENERIC        8 
#define WISPR_CLIENT_WISPR_2_0      9 

/* If any new client is added to this file please add the same in
 * cmn/ncfg/authcfg_helper.c 
 */
#define WISPR_USER_AGENT_STRLEN     32
#define WISPR_USER_AGENT_MAX_NUMBER 32

#define IPASS_USER_AGENT  "iPassConnect"
#define KONTOR_USER_AGENT  "OSC"
#define BOINGO_USER_AGENT  "boingo"
#define WEROAM_USER_AGENT  "weroam"
#define TRUSTIVE_USER_AGENT  "Trustive"
/* Boingo partner client */
#define INFONET_USER_AGENT  "infonet"
#define FIBERLINK_USER_AGENT  "fiberlink"
#define MCI_USER_AGENT  "mci"
#define SKYPE_USER_AGENT  "skype"
#define SKYPE_WIFI_USER_AGENT  "Skype WISPr"
#define BTI_USER_AGENT  "bti"
#define VELOFONE_USER_AGENT  "velofone"
#define ALLTEL_USER_AGENT  "alltel"
/* Weroam partner client */
#define ATNT_USER_AGENT  "AT&T Global Network"
#define ATT_USER_AGENT  "ATT Global Network"
#define NETCLIENT_USER_AGENT  "NetClient"
    
/* Generic WISPr clients */
#define AU_WIFI_USER_AGENT     "au Wi-Fi Connect"
#define WI_2_USER_AGENT        "Wi2Connect"
#define UQ_WIFI_USER_AGENT     "UQWifiConnect"

extern int is_wispr_client(char *a);

extern int g_wispr_client_type;
#endif /* _WISPR_H_ */
