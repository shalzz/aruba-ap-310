#ifndef CLI_PROV_LOGS_H
#define CLI_PROV_LOGS_H

#define VALID_SSID_DURING_BOOT_FILE "/tmp/valid_ssid_during_boot"

typedef enum
{
    PROV_STATE_INIT = 0,
    PROV_STATE_DHCP_OPTION,
    PROV_STATE_AMP_AUTODISCOVERY,
    PROV_STATE_ACTIVATE,
    PROV_STATE_AMP,
    PROV_STATE_ATHENA,
    PROV_STATE_MGMT_SERVER,
    PROV_STATE_UAP_ADP
} provision_state_t;

typedef enum
{
    CONFIG_AUDIT_NONE = 0,
    CONFIG_AUDIT_CMD_RECEIVED,
    CONFIG_AUDIT_CFG_SENT,
    CONFIG_AUDIT_NEW_CFG_RECEIVED,
    CONFIG_AUDIT_NEW_CFG_APPLIED,
    CONFIG_AUDIT_NEW_UPG_RECEIVED,
} config_audit_state_t;

typedef enum
{
    PROV_LOG_PENDING = 0,
    PROV_LOG_SUCCESS,
    PROV_LOG_WARNING,
    PROV_LOG_ERROR,
    PROV_LOG_FAIL_THROUGH,
    PROV_LOG_DEBUG
} provision_log_type_t;

typedef struct
{
    char name[64];
    provision_state_t state;
    provision_log_type_t type;
    char fmt[256];
} provision_log_entry_t;

void append_provision_log_entry(provision_log_entry_t *entry, const char *msg, const char *args, int no_repeat);
void format_log_args (char * buf, int maxlen, char *format, ...);

#define append_provision_log(entry, args...) _append_provision_log(entry, FALSE, args)
#define append_provision_log_no_repeat(entry, args...) _append_provision_log(entry, TRUE, args)

#if __FAT_AP__
#define _append_provision_log(entry, no_repeat, args...) {\
    char msg[512]; \
    char arg_str[512]; \
    format_log_args(arg_str, sizeof(arg_str), (entry)->fmt, args); \
    snprintf(msg, sizeof(msg), (entry)->fmt, args); \
    append_provision_log_entry(entry, msg, arg_str, no_repeat); \
}
#else
#define _append_provision_log(entry, no_repeat, args...) 
#endif

#define DEF_PROV_LOG_ENTRY(name, state, type, fmt) \
provision_log_entry_t name = { \
    #name, \
    state, \
    type, \
    fmt\
};

extern provision_log_entry_t 
    prov_uap_adp_success,
    prov_uap_adp_warning,
    prov_uap_adp_error;
#ifdef __OEM_hp__
#define CENTRAL_NAME "HP Central"
#else
#define CENTRAL_NAME "Aruba Central"
#endif
#define CENTRAL_AUTH_TYPE "Auth Type                  "
#endif
