#ifndef _MESHD_WPA_IFACE_H_
#define _MESHD_WPA_IFACE_H_

#include <asm/types.h>
#define SSID_MAX_LEN 32
#define MESHD_SOCKET_NAME       "/tmp/meshd_unix_sock"
#define MESHD_HOSTAPD_SYNC_FILE     "/tmp/meshd_hostapd_sync"
#define MESHD_11AD_UNIX_NAME       "/tmp/meshd_11ad_unix_sock"
#define MESHD_WPA_MAGIC 0xfefdfcfb

/* cmds rcvd over meshd's sync socket */
enum meshd_wpa_cmd_id {
    MESHD_WPA_HOSTAPD_UP=0,
    MESHD_WPA_SUPPLICANT_UP,
    MESHD_WPA_HOSTAPD_STA_AUTH_SUCCESS,
    MESHD_WPA_HOSTAPD_STA_AUTH_FAIL,
    MESHD_WPA_SUPPLICANT_SUCCESS,
    MESHD_WPA_SUPPLICANT_FAIL,
    MESHD_WPA_STA_ADD,
    MESHD_WPA_STA_DEL,
    MESHD_WPA_BSS_ADD,
    MESHD_WPA_BSS_DEL,
    MESHD_WPA_SCAN_DONE,
    MESHD_WPA_CONN_SUCCESS,
    MESHD_WPA_CONN_FAIL,
    MESHD_WPA_UNUSED
};

typedef struct meshd_wpa_cmd {
    __u32 magic;
    __u8 cmd_id;
    __u8 data[0];
} __attribute__ ((packed)) meshd_wpa_cmd_t;

typedef struct meshd_bss_info {
	__u8 bssid[6];
	__u8 ssid[SSID_MAX_LEN +1];
	int ssid_len;
    __u8 bssid_aband[6]; // the bssid of another band
    __u32 lms;
    __u8 encryption;
    __u8 sta_num;
    __u8 chan;
	signed char rssi;
} __attribute__ ((packed)) meshd_bss_info_t;

#endif
