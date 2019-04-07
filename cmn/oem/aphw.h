// vim:set ts=4 sw=4 expandtab:
#ifndef _APHW_H_
#define _APHW_H_
/*
 * Copyright (C) 2002-2009 by Aruba Networks, Inc.
 * All Rights Reserved.
 * 
 * This software is an unpublished work and is protected by copyright and 
 * trade secret law.  Unauthorized copying, redistribution or other use of 
 * this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */
#include "aptype.h"
#include "oemdefs.h"

#define MAX_DEV_LEN 32

typedef enum {
    APRADIO_BAND_G,
    APRADIO_BAND_A,
    APRADIO_BAND_AG,
} apradio_band_t;

typedef enum {
    RADIO_BAND_FULL,
    RADIO_BAND_LOWER,
    RADIO_BAND_MIDDLE,
    RADIO_BAND_UPPER,
    RADIO_NUM_BANDS
} apradio_band_range;

typedef struct channel_range_info_t {
    unsigned int channel_min;
    unsigned int channel_max;
} channel_range_info_t;
#define APHW_MAX_CHANNEL_RANGES 8

typedef struct scan_capability_t {
    apradio_band_t band;
    unsigned int num_channel_ranges;
    channel_range_info_t channel_ranges[APHW_MAX_CHANNEL_RANGES];
} scan_capability_t;
typedef struct radar_thresholds_t {
    unsigned int radar_thr0_20_lo;
    unsigned int radar_thr0_40_lo;
    unsigned int radar_thr0_80_lo;
    unsigned int radar_thr0_160_lo;
    unsigned int radar_thr0_20_hi;
    unsigned int radar_thr0_40_hi;
    unsigned int radar_thr0_80_hi;
    unsigned int radar_thr0_160_hi;
} radar_thresholds_t;

typedef struct bandpass_filter_t {
    unsigned int gpio_enabled;
    unsigned int gpio_out;
} bandpass_filter_t;

typedef enum {
    APRADIO_ANT_INT,
    APRADIO_ANT_EXT,
    APRADIO_ANT_INT_EXT,
} apradio_ant_t;

typedef enum {
    APRADIO_SMART_ANT_NOT_SUPPORT,
	APRADIO_SMART_ANT_SUPPORT,
} apradio_smart_ant_t;

typedef struct apradio_ht_cap_t {
    unsigned ht_capable:1,
             shortgi_20:1,
             shortgi_40:1;
    unsigned char supported_mcs[10]; // one bit per MCS (0-76)
    unsigned char stbc_tx_streams;
    unsigned char stbc_rx_streams;
    unsigned char has_ldpc;
    unsigned char has_txbf;
    unsigned char min_mpdu_spacing;
} apradio_ht_cap_t;

typedef struct {
    unsigned vht_capable:1,
             shortgi_80:1,
             shortgi_160:1,
             supported_channel_width:2,
             has_mumimo:1,
             has_vht160:1,
             has_vht160_contiguous_only:1;
    unsigned short supported_mcs_map;
    unsigned char spatial_streams;
    unsigned char spatial_streams_160;
} apradio_vht_cap_t;

typedef struct {
    unsigned int he_capable:1,
                 mu_ofdma:2,
                 mu_mimo:2,
                 has_bss_color:1,
                 has_obss_pd:1,
                 has_ofdma_random_access:1,
                 has_multi_tid_ampdu:1,
                 max_ru:7;
} apradio_he_cap_t;

#define AP_HT_MCS_SUPPORTED(smcs, m) \
    ((smcs)[(m) / 8] & (1 << ((m) % 8)))

typedef struct apradio_spectrum_cap_t {
    unsigned spectrum_capable:1,
             fft_capable_2:1, // capable of FFT in 2.4 GHZ only
             fft_capable_5:1, // capable of FFT in 5 GHZ only
             fft_capable:1,   // all freq bands of the radio type
             hybrid_capable:1,// capable of ap-mode with spectrum-monitoring
             fft_20mhz_all_bins:1, // no missing channel edge bins in 20MHz
             ng_classification:1,
             fft_80mhz:1;     // capable of scanning 80MHz
} apradio_spectrum_cap_t;

typedef struct apradio_t {
    apradio_band_t band;
    apradio_band_range band_range;
    unsigned int num_channel_ranges;
    channel_range_info_t *channel_ranges;
    scan_capability_t* scan_capability;
    radar_thresholds_t *radar_thresholds;
    bandpass_filter_t *bandpass_filter;
    apradio_ht_cap_t ht_cap;
    apradio_vht_cap_t vht_cap;
    apradio_he_cap_t he_cap;
    apradio_spectrum_cap_t spectrum_cap;
    apradio_ant_t ant;
	apradio_smart_ant_t smart_ant;
    int num_antennas;
    unsigned no_diversity:1,
             use_virtual_ssids:1;
    unsigned wired_antenna;
    int max_ssids;
    int max_tx_chains_under_af;
    int max_tx_pwr_under_af; 
    int base_bssid_offset;
    char base_bssid_from[MAX_DEV_LEN];
    char linux_24_base_bssid_from[MAX_DEV_LEN];
    unsigned misc_attr;
    unsigned poeaf_action;
    unsigned interface_down;
    unsigned bssid_reserved;
    int ant_pol;
    int min_cond_pwr; /*Minimum conducted power(10x) per chain*/
    int int_ant_gain;
    int ext_ant_loss;
} apradio_t;

typedef struct ap_wigig_t {
    char *ifname;
} ap_wigig_t;

typedef enum ap_enet_type_t {
// needs to match aruba-tc.my
    APHW_ENET_TYPE_FAST = 1,
    APHW_ENET_TYPE_GIG = 2,
    APHW_ENET_TYPE_XGIG = 3,    // someday ;->
    APHW_ENET_TYPE_2_GIG = 4,   
    APHW_ENET_TYPE_5_GIG = 5,   
} ap_enet_type_t;

typedef struct apenet_t {
    char *ifname;
    ap_enet_type_t enet_type;
    unsigned secure_access:1,
             shutdown:1,
             enslave:1,
             link_config:1,
             dot3az:1,
             dot3bz:1,
             dot3ad:1,
             dot3at:10,
             pse:2,
             lldp_overriden_power:10;
} apenet_t;

typedef enum {
    AP_ENVIRONMENT_INDOOR,
    AP_ENVIRONMENT_OUTDOOR,
} ap_environment_t; // superseded by sap_installation_mode_t
typedef enum {
    AP_INSTALLATION_TYPE_DEFAULT=0,
    AP_INSTALLATION_TYPE_INDOOR,
    AP_INSTALLATION_TYPE_OUTDOOR,
} ap_installation_mode_t;

typedef struct {
    int major;
    int minor;
    int rev;
} ap_image_version_t;

typedef struct {
    ap_image_version_t begin_version;
    ap_image_version_t end_version;
} ap_image_version_range_t;
#define APHW_MAX_VERSION_RANGES 2

typedef struct {
    int cc_index;
    apradio_band_t band;
    int num_chans;
    unsigned char *chans;
} ap_chanlist_t;

#define APHW_MAX_RADIOS     2
#define APHW_MAX_ENETS      8
#define APHW_MAX_USBS       1
#define APHW_MAX_WIGIGS     1

typedef struct apflash_t {
    char *af_name;
    unsigned af_start;
    unsigned af_offset;
    unsigned af_size;
    unsigned af_cachetype;
    unsigned af_sectorsize;
    char *af_devname;
} apflash_t;
#define APHW_MAX_FLASH_SEGMENTS 20

typedef struct apflash_variant_t {
    char     *afv_name;
    int       afv_size;
    char     *afv_minver;
    char     *afv_min_iap_aos_convert_ver;
    char     *afv_min_instant_ver;
    char     *afv_variants;
    char     *afv_instant_variants;
    apflash_t afv_flash_segments[APHW_MAX_FLASH_SEGMENTS];
    unsigned  afv_num_flash_segments;
} apflash_variant_t;
#define APHW_MAX_FLASH_VARIANTS 2

typedef struct ap_gpio_t {
    unsigned   ag_pin;
    char *ag_name;
} ap_gpio_t;
#define APHW_MAX_GPIO_PINS 20

typedef struct ap_i2c_bus_t {
    unsigned   ib_num;
    unsigned   ib_clock;
    unsigned   ib_data;
} ap_i2c_bus_t;
#define APHW_MAX_I2C_BUSSES 2

typedef struct ap_i2c_device_t {
    unsigned   id_bus;
    char      *id_name;
} ap_i2c_device_t;
#define APHW_MAX_I2C_DEVICES 5

#define APHW_MAX_PHY_LED_MODES 4
#define APHW_MAX_PHY_LED_REGS 8
#define APHW_MAX_SWITCH_LED_MODES 4
#define APHW_MAX_SWITCH_LED_REGS 4
#define APHW_MAX_CTL_LED_MODES 4
#define APHW_MAX_CTL_LED_REGS 4

typedef struct ap_control_led_reg_t {
    unsigned int cr_reg;
    unsigned int cr_val;
} ap_control_led_reg_t;

typedef struct ap_control_led_t {
    char      *cl_name;
    ap_control_led_reg_t cl_regs[APHW_MAX_CTL_LED_REGS];
    unsigned   cl_nregs;
} ap_control_led_t;

typedef struct ap_switch_led_reg_t {
    unsigned short sr_reg;
    unsigned int sr_val;
} ap_switch_led_reg_t;

typedef struct ap_switch_led_t {
    char      *sl_name;
    ap_switch_led_reg_t sl_regs[APHW_MAX_SWITCH_LED_REGS];
    unsigned   sl_nregs;
} ap_switch_led_t;

typedef struct ap_phy_led_reg_t {
    unsigned short pr_reg;
    unsigned short pr_val;
} ap_phy_led_reg_t;

typedef struct ap_phy_led_t {
    char      *pl_name;
    ap_phy_led_reg_t pl_regs[APHW_MAX_PHY_LED_REGS];
    unsigned   pl_nregs;
} ap_phy_led_t;

typedef struct ap_mmio_led_t {
    unsigned   ml_mmio;
    unsigned   ml_val;
} ap_mmio_led_t;

typedef struct ap_gpio_led_t {
    unsigned   gl_pin;
    unsigned   gl_active_low;
} ap_gpio_led_t;

typedef struct ap_led_t {
    char      *led_name;
    unsigned   led_type;
    union {
        ap_gpio_led_t led_gpio;
        ap_mmio_led_t led_mmio;
        ap_phy_led_t  led_phy[APHW_MAX_PHY_LED_MODES];
        ap_switch_led_t led_switch[APHW_MAX_SWITCH_LED_MODES];
        ap_control_led_t led_control[APHW_MAX_CTL_LED_MODES];
    } led_u;
    unsigned led_num_led_modes;
} ap_led_t;
#define APHW_MAX_LEDS 10

#define APHW_LED_GPIO 1
#define APHW_LED_MMIO 2
#define APHW_LED_PHY  3
#define APHW_LED_SW_S16 4
#define APHW_LED_SW_S26 5
#define APHW_LED_CTL_MM 6

typedef enum {
    APPOWER_ACTION_NONE,
    APPOWER_ACTION_SHUTDOWN_RADIOS = 0x01,
} ap_power_action_t;


typedef enum apradio_hw_opmode_t {
    HW_OP_DEFAULT = 0,
    HW_OP_RSDB_SPLIT = 1,
    HW_OP_RSDB_5G = 2,
    HW_OP_RSDB_2G = 3,
    HW_OP_DUAL_5G_ON = 4,
    HW_OP_DUAL_5G_OFF = 5,   
} apradio_hw_opmode_t;

typedef struct apradio_info_t {
    apradio_hw_opmode_t opmode;
    apradio_t radios[APHW_MAX_RADIOS];
} apradio_info_t;

typedef enum {
    ARUBA_SMP_TASK_WIFI0_IRQ = 0,
    ARUBA_SMP_TASK_WIFI1_IRQ,
    ARUBA_SMP_TASK_WIFI2_IRQ,

    ARUBA_SMP_TASK_WIFI0_TIMER,
    ARUBA_SMP_TASK_WIFI1_TIMER,
    ARUBA_SMP_TASK_WIFI2_TIMER,

    ARUBA_SMP_TASK_WIFI0_THREAD,
    ARUBA_SMP_TASK_WIFI1_THREAD,
    ARUBA_SMP_TASK_WIFI2_THREAD,

    ARUBA_SMP_TASK_WIFI0_WORK,
    ARUBA_SMP_TASK_WIFI1_WORK,
    ARUBA_SMP_TASK_WIFI2_WORK,

    ARUBA_SMP_TASK_NSS0_QUEUE0_IRQ,
    ARUBA_SMP_TASK_NSS0_QUEUE1_IRQ,
    ARUBA_SMP_TASK_NSS0_QUEUE2_IRQ,
    ARUBA_SMP_TASK_NSS0_QUEUE3_IRQ,
    ARUBA_SMP_TASK_NSS0_RXBUF_IRQ,
    ARUBA_SMP_TASK_NSS0_TXBUF_FREE_IRQ,

    ARUBA_SMP_TASK_NSS1_QUEUE0_IRQ,
    ARUBA_SMP_TASK_NSS1_QUEUE1_IRQ,
    ARUBA_SMP_TASK_NSS1_QUEUE2_IRQ,
    ARUBA_SMP_TASK_NSS1_QUEUE3_IRQ,
    ARUBA_SMP_TASK_NSS1_RXBUF_IRQ,
    ARUBA_SMP_TASK_NSS1_TXBUF_FREE_IRQ,

    ARUBA_SMP_TASK_FIREWALL_PRIMARY_WORK,
    ARUBA_SMP_TASK_FIREWALL_SECONDARY_WORK,
    ARUBA_SMP_TASK_FIREWALL_TIMER,

    ARUBA_SMP_TASK_ANUL_RADIO0_TIMER,
    ARUBA_SMP_TASK_ANUL_RADIO1_TIMER,
    ARUBA_SMP_TASK_ANUL_RADIO2_TIMER,

    ARUBA_SMP_TASK_ANUL_RADIO0_WORK,
    ARUBA_SMP_TASK_ANUL_RADIO1_WORK,
    ARUBA_SMP_TASK_ANUL_RADIO2_WORK,

    ARUBA_SMP_TASK_MAX = 256,
} aruba_smp_binding_task_t;
typedef enum {
    ARUBA_SMP_TASK_TYPE_INVALID = 0,
    ARUBA_SMP_TASK_TYPE_IRQ = 1,
    ARUBA_SMP_TASK_TYPE_TIMER = 2,
    ARUBA_SMP_TASK_TYPE_THREAD = 3,
    ARUBA_SMP_TASK_TYPE_WORK = 4,
} aruba_smp_task_type_t;
typedef struct aruba_smp_binding {
    const char *task_id;
    aruba_smp_task_type_t task_type;
    int allowed_cpu;
    int count;
    unsigned long task_param;
} aruba_smp_binding_t;
typedef struct aruba_smp_binding_info {
    int num_cpus;
    aruba_smp_binding_t task_bindings[ARUBA_SMP_TASK_MAX];
} aruba_smp_binding_info_t;

typedef struct aphw_t {
    int num_radios;
    unsigned standard_serial_number:1,
             separate_wireless_mac:1,
             require_signed_images:1,
             has_switch_chip:1,
             show_switching_enable:1,
             remote_only:1,
             factory_cert:1,
             accel_license:1,
             rfprotect:1,
             boot_upgradable:1,
             blink_leds:1,
             built_in:1,
             tlv_inventory:1,
             fips_kat_support:1,
             no_dns_discovery:1,
             dual_flash:1,
             dtim_support:1,
             rsdb_support:1,
             dual5g_support:1,
             dynamic_mode_support:1,
             brcm_ap:1,
             qca_ap:1,
             wpa3_support:1,
             am_copy_optimizations:1,
             acktimeout_proc_support:1,
             usb_fourg_support:1,
             anyspot_support:1,
             has_pwr_mon_chip:1,
             power_failover:1,
             always_vlan:1,
             multizone_support:1,
             csr_support:1,
             openflow_support:1,
             rfd_support:1,
             mumimo_amsdu_war:1,
             switch_bfrm_war:1,
             cluster_upg_support:1,
             image_preload_support:1,
             sys_core_support:1,
             ses_esl_support:1,
             wol_support:1;
    unsigned is_11ax_ap:1,
             precise_radar_freq_support:1; 
    int max_clients_per_radio;
    char *ble_support;
    char *ble_port;
    unsigned ble_port_speed;
    char *ble_firmware;
    char *ble_hw_type_str;
    char *fcc_id ;   
    char *fcc_model ;
    unsigned rssi_meter;
    unsigned first_switch_port;
    unsigned second_image_offset;
    unsigned firewall_max_queue;
    unsigned num_firewall_queues;
    unsigned ath_keymax;
    unsigned max_mtu;
    unsigned radar_thr0_20_lo;
    unsigned radar_thr0_40_lo;
    unsigned radar_thr0_80_lo;
    unsigned radar_thr0_160_lo;
    unsigned radar_thr0_20_hi;
    unsigned radar_thr0_40_hi;
    unsigned radar_thr0_80_hi;
    unsigned radar_thr0_160_hi;
    char *proc_file;
    unsigned provisioning_mode_led;
    int int_ant_2_gain;
    int int_ant_5_gain;
    int ext_ant_2_loss;
    int ext_ant_5_loss;
    int default_ext_ant_2_gain;
    int default_ext_ant_5_gain;
    int iap_default_ext_ant_2_gain;
    int iap_default_ext_ant_5_gain;
    int ap_max_clients;
    int num_usbs;
    apflash_variant_t flash_variants[APHW_MAX_FLASH_VARIANTS];
    unsigned num_flash_variants;
#ifndef CONFIG_MERLOT
    ap_gpio_t gpio_pins[APHW_MAX_GPIO_PINS];
    unsigned num_gpio_pins;
    ap_i2c_bus_t i2c_busses[APHW_MAX_I2C_BUSSES];
    unsigned num_i2c_busses;
    ap_i2c_device_t i2c_devices[APHW_MAX_I2C_DEVICES];
    unsigned num_i2c_devices;
    ap_led_t leds[APHW_MAX_LEDS];
    unsigned num_leds;
#endif
    int pci_rst_delay;
    aruba_smp_binding_info_t *smp_info;
    apradio_t radios[APHW_MAX_RADIOS];
    int num_wigigs;
    ap_wigig_t wigigs[APHW_MAX_WIGIGS];
    int num_chanlists;
    ap_chanlist_t *chanlists;
    apradio_info_t *radio_operations;
    int num_radio_operations;
    unsigned overall_max_ssids;
    unsigned char primary_uplink;
    int num_enets;
    apenet_t enets[APHW_MAX_ENETS];
    ap_environment_t environment;  // XXX obsolete?
    ap_installation_mode_t installation; 
    /* Use aphw_is_outdoor_ap() instead of "environment" because this 
       function ensures "installation" provisioning parameter is considered */
    ap_image_version_t min_iap_aos_convert_version;
    ap_image_version_t min_instant_version;
    char *release_variants;
    char *instant_variants;
    unsigned char boot_image;
    unsigned char os_image;
    unsigned char iap_image;
    unsigned char has_tpm;
    char *poe_power_proc;
    char *radio_bus_type;
    char *usb_host_module;
    int mesh_capable;
    unsigned gre_cut_thru:1,
             gre_uplink_vlan_cut_thru:1,
             gre_ctf_frag:1,
             gre_offload_uplink_vlan:1,
             gre_offload_ipv4:1,
             gre_offload_ipv6:1,
             ipsec_offload_ipv4:1,
             ipsec_offload_ipv6:1,
             gre_full_offload:1;
    int jumbo_len;
    int calibration_in_flash;
    int usb_power_override; /* knob to enable usb with insufficient(AF) power */
    char *poe_pse_proc;
    unsigned char adv_pwr_control;
    int num_supported_version_range;
    ap_image_version_range_t supported_version_range[APHW_MAX_VERSION_RANGES];
    int num_supported_fips_version_range;
    ap_image_version_range_t supported_fips_version_range[APHW_MAX_VERSION_RANGES];
    int num_supported_iap_version_range;
    ap_image_version_range_t supported_iap_version_range[APHW_MAX_VERSION_RANGES];
    int uap;
} aphw_t;


#ifdef __KERNEL__
extern int asap_is_outdoor_ap(int apType, int installation);
extern unsigned aruba_get_num_usbs(void);

extern aphw_t __ap_hw_info[];
extern int xml_ap_model, ap_flash_variant;
#define this_ap_hw_info __ap_hw_info[xml_ap_model]
#define ARUBA_SMP_HAS_BINDING_INFO() (this_ap_hw_info.smp_info != NULL)

extern unsigned aruba_avail_cpus;
#define ARUBA_MULTI_CORE (aruba_avail_cpus > 1)
#define ARUBA_DUAL_CORE (aruba_avail_cpus == 2)
#define ARUBA_QUAD_CORE (aruba_avail_cpus == 4)
#define ARUBA_CPU_CORE_INDEX_0    0
#define ARUBA_CPU_CORE_INDEX_1    1
#define ARUBA_CPU_CORE_INDEX_2    2
#define ARUBA_CPU_CORE_INDEX_3    3
#ifdef CONFIG_ARUBA_SMP
extern int aruba_smp_get_task_cpu(aruba_smp_binding_task_t task);
extern int aruba_smp_bind_task(aruba_smp_binding_task_t task, unsigned long param);
#else
static inline int aruba_smp_get_task_cpu(aruba_smp_binding_task_t task) { return 0; }
static inline int aruba_smp_bind_task(aruba_smp_binding_task_t task, unsigned long param) { return 0; }
#endif
#define ARUBA_AP_NUM_FIREWALL_QUEUE() this_ap_hw_info.num_firewall_queues
#define ARUBA_AP_HAS_MULTI_FIREWALL_QUEUE() (this_ap_hw_info.num_firewall_queues > 1)
#endif

#define ARUBA_PS_UNKNOWN        "Unknown"
#define ARUBA_PS_DC             "DC"
#define ARUBA_PS_NA             "Not applicable"
#define ARUBA_PS_AT_O_UE        "POE-AT: User Overriden: USB port enabled"
#define ARUBA_PS_AT_O_UD        "POE-AT: User Overriden: USB port disabled"
#define ARUBA_PS_AT             "POE-AT: No restrictions"
#define ARUBA_PS_AF_UD          "POE-AF: USB port disabled;"
#define ARUBA_PS_AF_ED_UD_1_3   "POE-AF: One Ethernet port disabled; USB port disabled; 1x3 chain for 2.4GHz; 3x3 chain for 5GHz"
#define ARUBA_PS_AF_ED_UE_1_3   "POE-AF: One Ethernet port disabled; USB port enabled; 1x3 chain for 2.4GHz; 3x3 chain for 5GHz"
#define ARUBA_PS_AF_ED_2_2      "POE-AF: One Ethernet port disabled; 2 TX chains per radio"

#define ARUBA_PS_DC_PSEE_UE     "DC: PSE Enabled, USB Enabled"
#define ARUBA_PS_DC_PSED_UE     "DC: PSE Disabled, USB Enabled"
#define ARUBA_PS_AT_PSE10_UE    "POE-AT: PSE Enabled(10W max), USB(BLE) Enabled"
#define ARUBA_PS_AT_PSE10_UD    "POE-AT: PSE Enabled(10W max), USB(BLE) Disabled"
#define ARUBA_PS_AT_PSED_UE     "POE-AT: PSE Disabled, USB(BLE) Enabled"
#define ARUBA_PS_AT_PSED_UD     "POE-AT: PSE Disabled, USB(BLE) Disabled"
#define ARUBA_PS_AF_PSED_UE     "POE-AF: PSE Disabled, USB(BLE) Enabled"
#define ARUBA_PS_AF_PSED_UD     "POE-AF: PSE Disabled, USB(BLE) Disabled"

#define ARUBA_PS_NO_RESTRICT    "POE: No restrictions"











 

#endif // _APHW_H_
