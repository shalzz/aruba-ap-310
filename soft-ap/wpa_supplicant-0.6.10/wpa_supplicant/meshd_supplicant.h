
#define SUPPLICANT_GLOBAL_SOCKNAME	"/tmp/supp_gbl"
#define SUPPLICANT_IFACE_SOCKDIR	"/tmp/supp_if"
#define SUPPLICANT_MESH_CONF_FILE	"/aruba/bin/mesh_psk_supplicant.conf"

#ifdef __FAT_AP__
#define SUPPLICANT_UPLINK_CTRL_SOCKDIR  "/tmp/sta_supplicant_ctrl"
#define SUPPLICANT_UPLINK_STAT_SOCKDIR  "/tmp/sta_supplicant_status"
#define SUPPLICANT_UPLINK_CONF_FILE     "/aruba/bin/wpa_sta_supplicant.conf"   /* wifi uplink interface arubaX01 config file */
#define WIFI_UPLINK_MAGIC               0x06240624

enum wifi_uplink_wpa_cmd_id
{
    WIFI_UPLINK_SUPPLICANT_UP  = 1,
    WIFI_UPLINK_SUPPLICANT_SUCCESS,
    WIFI_UPLINK_SUPPLICANT_FAIL,
};
enum wifi_uplink_wpa_reason_id
{
    WIFI_UPLINK_REASON_OK          = 1,
    WIFI_UPLINK_REASON_EAPOL_HANDSHAKE,
    WIFI_UPLINK_REASON_DEAUTH,
    WIFI_UPLINK_REASON_UNKNOWN,
};
typedef struct _wifi_uplink_wpa_status {
    unsigned int  magic;
    unsigned int  cmd_id;
    unsigned int  data;
} __attribute__ ((packed)) wifi_uplink_wpa_status;

/* add message type for ap1x */
#define SUPPLICANT_AP1X_CTRL_SOCKDIR          "/tmp/ap1x_supplicant_ctrl"
#define SUPPLICANT_AP1X_STAT_SOCKDIR          "/tmp/ap1x_supplicant_status"
#define SUPPLICANT_AP1X_MAGIC                 0xdeadbeef
typedef struct _ap1x_wpa_status {
    unsigned int magic;
    unsigned int cmd_id;
} __attribute__ ((packed)) ap1x_wpa_status;
#define WIFI_UPLINK_IF_ADD_CMD          "INTERFACE_ADD %s\t" SUPPLICANT_UPLINK_CONF_FILE "\t%s\t\"%s\"\t%s\t\"%s\"\t%s"
#define WIFI_UPLINK_IF_REMOVE_CMD       "INTERFACE_REMOVE %s"


#define AP1X_AUTH_FILE_OK "/aruba/ap1x/auth_succeed"
#define AP1X_AUTH_FILE_FAIL "/aruba/ap1x/auth_fail"
#define AP1X_AUTH_FILE_TIMEOUT "/aruba/ap1x/auth_timeout"
#define AP1X_AUTH_FILE_SERVER_VALIDATE_FAIL "/aruba/ap1x/server_validate_fail"
#define AP1X_AUTH_SUCCESS_SEND_ERR "/aruba/ap1x/success_send_err"
#define AP1X_AUTH_MAGIC 0xdeadbeef
enum ap1x_wpa_cmd_id
{
    AP1X_AUTH_MSG_TIMEOUT  = 1,
    AP1X_AUTH_MSG_OK,
    AP1X_AUTH_MSG_FAIL,
    AP1X_AUTH_MSG_SERVER_VALIDATE_FAIL
};
/* for ap1x */
#endif


/* add <aruba001>\t<conf_file>\t<madwifi>\t<essid>\t<hexkey>\t<passphrase> */
#define INTERFACE_ADD_CMD		"INTERFACE_ADD %s\t" SUPPLICANT_MESH_CONF_FILE "\t%s\t\"%s\"\t%s\t%s"
#define INTERFACE_REMOVE_CMD		"INTERFACE_REMOVE %s"
/* <ess_len>TAB<essid>TAB<bssid>TAB<MHz>TAB<ie_len>TAB<ie> */
#define ASSOCIATE_INFORM_CMD		"ASSOCIATED %d\t%s\t%s\t%d\t%d\t%s"
#define DISASSOCIATE_INFORM_CMD		"DISASSOCIATED"
#define WPA_SUPPLICANT_OK_REPLY		"OK\n"

#define SUPPLICANT_MAX_DEBUG_LOG_FILES         2
#define SUPPLICANT_DEBUG_LOG_FILE              "/tmp/wpa_supplicant-debug-log"
#define SUPPLICANT_MAX_DEBUG_LOG_FILE_SIZE     (16 * 1024)



