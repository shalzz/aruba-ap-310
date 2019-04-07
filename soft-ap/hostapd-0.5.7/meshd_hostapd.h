#ifndef MESHD_HOSTAPD
#define MESHD_HOSTAPD

#define HOSTAPD_PID_FILE 		"/tmp/hostapd_pid"

#define HOSTAPD_MAX_DEBUG_LOG_FILES         2
#define HOSTAPD_DEBUG_LOG_FILE              "/tmp/hostapd-debug-log"
#define HOSTAPD_MAX_DEBUG_LOG_FILE_SIZE     (16 * 1024)

/* commands sent to hostapd sync socket */
#define MESHD_HOSTAPD_SYNC_CMD		"meshd_vap_up"

/* commands sent to hostapd per-iface socket */
#define HOSTAPD_CMD_CONFIG                  "CONFIG "
#define HOSTAPD_CMD_NEW_STA                 "NEW_STA "
#define HOSTAPD_CMD_NEW_RECOVERY_STA        "NEW_RECOVERY_STA "
#define HOSTAPD_CMD_DEL_STA                 "DEL_STA "

struct meshd_params {
	char iface[IFNAMSIZ];
	char ssid[32+1];
	char hexkey[64+1];
	char passphrase[64+1];
};

#endif
