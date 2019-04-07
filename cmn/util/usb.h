#ifndef _USB_H_
#define _USB_H_

#define USB_DRIVER_NONE         0
#define USB_DRIVER_AIRPRIME     1
#define USB_DRIVER_OPTION       2
#define USB_DRIVER_ACM          3
#define USB_DRIVER_SIERRA_GSM   4
#define USB_DRIVER_SIERRA_EVDO  5
#define USB_DRIVER_HSO          6
#define USB_DRIVER_BCM_WIMAX    7         
#define USB_DRIVER_PANTECH_UML290   8
#define USB_DRIVER_CDC_ETHER     9
#define USB_DRIVER_PANTECH_LTE USB_DRIVER_PANTECH_UML290 /*KAR-DEBUG : To delete later*/
#define USB_DRIVER_SIERRA_LTE  10
#define USB_DRIVER_RNDIS_LTE  11
#define USB_DRIVER_HUAWEI_LTE 12
#define USB_DRIVER_RNDIS_PANTECH_LTE  13
#define USB_DRIVER_OPTION_NOVATEL_LTE  14
#define USB_DRIVER_RNDIS_ALCATEL_LTE  15
#define USB_DRIVER_ZTE_LTE    16

/*New Modem request handled by IAP will start the usb_type value from 50 .This to avoid conflict of usb_type while merging*/
#define USB_DRIVER_GOBI  50    
#define USB_DRIVER_NETGEAR 51  /*Netgear 340u 341u*/
#define USB_DRIVER_PANTECH_295 52 
#define USB_DRIVER_HUAWEI_E3276_150 53
#define USB_DRIVER_NETGEAR_320 54  /*Netgear 320u*/
#define USB_DRIVER_NETGEAR_313 55
#define USB_DRIVER_ALCATEL_LTE_L800 57
#define USB_DRIVER_NOVATEL_U620 60
#define USB_DRIVER_FRANKLIN_U772 61
#define USB_DRIVER_ZTE_MF832S   62
#define USB_DRIVER_ZTE_MF832U   63
#define USB_DRIVER_ZTE_MF831   64
#define USB_DRIVER_ZTE_MF79S   65
#define USB_DRIVER_ZTE_MF825C  66
#define USB_DRIVER_HUAWEI_K5150 67
#define USB_DRIVER_ZTE_MF861   68
#define USB_DRIVER_NOVATEL_U730L 69
#define USB_DRIVER_STORAGE      100

/* 4G USB DRIVERS */
#define FOURG_USB_DRIVER_NONE        0 
#define FOURG_USB_DRIVER_BCM_WIMAX   1
#define FOURG_USB_DRIVER_LTE         2
#define FOURG_USB_DRIVER_PANTECH_LTE 3
#define FOURG_USB_DRIVER_PANTECH_AUTO 4

#define USB_TYPE_UNKNOWN        111       // Ceullar 3G device
#define USB_TYPE_CELLULAR       101       // Ceullar 3G/4G device
#define USB_TYPE_MODEM          102       // POTS Modem
#define USB_TYPE_PRINTER        103       // USB Printer
#define USB_TYPE_STORAGE        104       // Network storage
#define USB_TYPE_MODESW         105       // Device that needs to be modeswitched

#define USB_SERIAL_LEN          64
#define USB_PRODUCT_LEN         64
#define USB_MAX_TTYS            8
#define USB_MAX_DEVICES         16

#define USB_DEV_MAX_RETRIES     20      // number of times to re-test the tty port

#define USB_NAME_LEN            64
#define TTY_LEN                 64      // Could be quite long with the device prefix

#define USB_STATE_NONE          0
#define USB_STATE_TEST          1       // testing device
#define USB_STATE_READY         2       // device passed modem AT test and ready to go
#define USB_STATE_DIAL          3       // dialing ISP
#define USB_STATE_CONNECTED     4       // ISP connected
#define USB_STATE_DISCONNECTED  5       // disconnected from ISP
#define USB_STATE_ERR_GEN       100     // Generic error
#define USB_STATE_ERR_TTY       101     // Unable to open TTY device
#define USB_STATE_ERR_TIMEOUT   102     // Unable to send modem AT commands
#define USB_STATE_ERR_ABORT     103     // Abort condition matched
#define USB_STATE_ERR_CONNECT   104     // Error when trying to connect to ISP

#define USB_OPCODE_GET          100
#define USB_OPCODE_RECLASSIFY   101

#define USB_BRIEF_FILE          "/tmp/usb_brief.log"
#define USB_VERBOSE_FILE        "/tmp/usb_verbose.log"
#define USB_MODESW_LOG          "/var/log/oslog/modeswitch.log"
#define USB_BRIEF_CMD           "awk '/T:|P:|S:|I:/{sub(/T:/,\"\\nT:\");print}' /proc/bus/usb/devices"

#if defined (CONFIG_MILAGRO)
#define USB_VERBOSE_CMD         "usb-devices"
#else
#define USB_VERBOSE_CMD         "cat /proc/bus/usb/devices"
#endif

#define MODESW_PROFILE_MAX      64
#define MODESW_PARAM_LEN        128

#define MODESW_NONE             0
#define MODESW_EJECT            1
#define MODESW_REZERO           2
#define MODESW_USBMODESW        3

/* There is no active-standby mode now
 * in Uplink manager, only active-active.
 * Cellular is treated the same as wired.
 */
#define PRIOR_WIRED             200
#define PRIOR_CELL              200

#define COST_CELL               2
#define COST_WIRED              1

#define CELL_PROFILE_MAX        128
#define AT_CMD_MAX              256

/* USB Power mode: added by jili */
#define USB_POWER_MODE_AUTO 0
#define USB_POWER_MODE_ENABLE 1
#define USB_POWER_MODE_DISABLE 2

typedef enum {
    CELLULAR_NW_PREFERENCE_NONE = 0,
    CELLULAR_NW_PREFERENCE_AUTO,
    CELLULAR_NW_PREFERENCE_3G,
    CELLULAR_NW_PREFERENCE_4G,
    CELLULAR_NW_PREFERENCE_ADVANCED,
    CELLULAR_NW_PREFERENCE_AUTO_PREEMPTIVE,
    CELLULAR_NW_PREFERENCE_ADVANCED_PREEMPTIVE
} cel_nw_preference_t;

#define CELLULAR_3G_MAX_SPEED   42  // Mbps
#define CELLULAR_4G_MAX_SPEED   100 // Mbps
typedef enum {
    CELLULAR_3G = 0,
    CELLULAR_4G = 1
} usb_dev_mode_t;

// USB device declaration
typedef struct {
    char name[USB_NAME_LEN];            // profile name
    __u32 vendor;                       // USB vendor ID
    __u32 prodID;                       // USB product ID
    char product[USB_PRODUCT_LEN];      // USB product name
    char serial[USB_SERIAL_LEN];        // USB serial number
    char tty[TTY_LEN];                  // TTY port
                                        // profile
    int type;                           // Device type
    int address;                        // USB device address 
    int state;                          // USB device state
    int code;                           // error/status codes
    usb_dev_mode_t      dev_mode;       // Cellular device mode
    cel_nw_preference_t preference;     // mode for 4G.
#if defined(CONFIG_PORFIDIO) || defined(CONFIG_MILAGRO)
    __u32 bus;                            // bus number as we have multiple bus in case of grappa/amrula
#endif
} usb_dev_t;

#if defined(CONFIG_PORFIDIO) || defined(CONFIG_MILAGRO)
typedef struct {
    int address;
    int bus;
} usb_dev_desc_t;
#endif

typedef struct {
    char name[USB_NAME_LEN];            // profile name
    __u32 vendor;                       // USB vendor ID
    __u32 prodID;                       // USB product ID
    int method;                         // How to modeswitch 
    char param[MODESW_PARAM_LEN];       // Optional parameters 
} usb_modesw_t;                        

typedef struct {
    usb_dev_t usb;                      // USB device properties
    char dialer[USB_NAME_LEN];          // dialer group
    char user[USB_NAME_LEN];            // username authentication
    char pass[USB_NAME_LEN];            // password authenticatoin
    int driver;                         // linux driver to use
    int method;                         // How to modeswitch 
    char param[MODESW_PARAM_LEN];       // Optional parameters 
    int cost;                           // uplink cost
    int prior;                          // uplink priority
} cell_profile_t;

typedef struct {
    char name[USB_NAME_LEN];        // Dialer Group name
    char init_str[AT_CMD_MAX];  // Modem init string
    char dial_str[AT_CMD_MAX];  // Modem dial string
} dialer_group_t;

extern cell_profile_t def_cell_profiles[];

#endif /* _USB_H_ */
