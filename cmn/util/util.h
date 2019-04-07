/* ------------------------------------------------------------------------- 
 * Name        : util.h
 * Description : 
 * ------------------------------------------------------------------------- */

#ifndef UTIL_H
#define UTIL_H
// public section
#include <stdio.h>
#if defined(i386) || defined(__x86_64__) || (_MIPS_SIM == _ABIN32)
#ifndef X86_TARGET_PLATFORM
#include <linux/types.h>
#endif
#endif
#include <sys/types.h>
#include <asm/types.h>
#include "stddefs.h"
#include "mac_util.h"

//
// ARUBA supported AP types moved to sysdef/ap.xml
//
typedef int aruba_ap_type;
typedef int aruba_reg_index;
#include <oem/aptype.h>
#include <oem/aphw.h>

/* look at util.c for now info on aruba_popen() and aruba_pclose() */
#define popen  aruba_popen
#define pclose aruba_pclose

#define LOC_TO_STR_MAX_LEN 14
#define MAC_STR_ALEN       20  // "00:00:00:00:00:00", 18 + 2 to for alignment.

#define GSM_UUID_TOTAL_NUM_BYTES (12)
#define UUID_STR_ALEN      (GSM_UUID_TOTAL_NUM_BYTES * 2 + 2) 
#define UUID_NSTRINGS 64

#define ALCATEL_FMT	  0x1  // for sys_get_model 
#define NO_CC_SUFFIX	  0x2  // for sys_get_model 

#define CKSUM_LEN         128
#define KB                1024
#define MB                (KB*KB)
#define SAFE_FLASH_GAP    2  // for under reporting available flash space
#define SAFE_MEM_GAP      5  // for under reporting available tmpfs space

#define ANC_UPGRADE_MB_MIN			40 // in MB , for flash space check during ancillary upgrade
#define UPGRADE_MB_NEEDED 85  // in MB space for the whole AOS image
#define SCP_THROTTLE_MEM_LIMIT      130 // in MB, minimum free memory to trigger SCP throttling

#define SIZE_TUNNEL_GRP_NAME      20
#define SIZE_NEXTHOPLIST_NAME     128
#define SIZE_CRYPTO_MAP_NAME      128

#define UPLINK_TAG_LEN            32

#define USE_FLASH          1
#define USE_TMP            2
#define USE_FLASH1         3

#define BOOTFLASH_PART_PRIMARY      0
#define BOOTFLASH_PART_SECONDARY    1
#define MAX_FQDN_LEN				256
#define MAX_FQDN_LABEL_LEN			64

#if defined(CONFIG_NEBBIOLO)
#define ARUBA_BOOTLOADER_IMAGE_ADDR	 0x1c0000
#define BOOTROM_MTD_FILENAME    "/dev/bootrom-ro"
#elif defined(CONFIG_GRENACHE)
#define ARUBA_BOOTLOADER_IMAGE_ADDR	 0x170000
#define BOOTROM_MTD_FILENAME    "/dev/mtd0"
#elif defined(CONFIG_PORFIDIO)
/* For Porfidio, cpboot is the stage 2 bootloader. Hence the image header is
 * at the beginning of the MTD partition for cpboot
 */
#define ARUBA_BOOTLOADER_IMAGE_ADDR	 0
#define BOOTROM_MTD_FILENAME    "/dev/cpboot1"
#elif defined(CONFIG_MILAGRO)
#define ARUBA_BOOTLOADER_IMAGE_ADDR      0
#define BOOTROM_MTD_FILENAME    "/dev/cpboot"
#endif

#ifdef __FAT_AP__
#define DEFAULT_GW_ADDR "/tmp/gwaddr"
#endif

#ifdef __FAT_AP__
#define DEFAULT_GW_ADDR "/tmp/gwaddr"
#define _debug_log(tag, arg...) syslog(LOG_DEBUG, arg)
#if 0
 { \
    FILE *my_log_file = fopen("/tmp/debug.log", "at"); \
    if (my_log_file) { \
        fprintf(my_log_file, arg); \
        fprintf(my_log_file, "\n"); \
        fclose(my_log_file); \
    } \
}
#endif


#define debug_log(tag, arg...) _debug_log(tag, arg)

#define error_log(tag, arg...) _debug_log(tag, arg)

#endif


#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/* Minimum size of flash before we start warning users */
#ifndef X86_TARGET_PLATFORM
#define MIN_FLASH_WARN 5
#else
/* 2GB in x86 controller platforms */
#define MIN_FLASH_WARN 2000 
#endif

#define IP_ADDR_IS_CLASSD(ip)  ((ip & 0xF0000000) == 0xE0000000)
#define IP_ADDR_IS_CLASSE(ip)  ((ip & 0xF0000000) == 0xF0000000)
#define IP_PREFIX_TO_NETMASK(prefix) (~((0x1 << (32 - prefix)) - 1))

//
// Old version numbers were all a.b.c.d
// With GCS, it is now possible to have
// version numbers that follow no 
// particular pattern; this is complicated
// since we use major.minor for config format
// and other things
//
// The solution is to store two versions, the base
// version, which is always a.b.c.d, but is not
// shown to the user.  The user-visible version is
// an opaque string, which for AirOS will match
// the a.b.c.d version, but for GCS may not.
//
#define _MAX_VER_LEN 32
typedef struct _swver {
	__u8 major;
	__u8 minor;
	__u8 maint;
	__u8 patch;
	char vstr[_MAX_VER_LEN];
} swver;

#define IPV4_ADR_PRINT(a) (__u8)(a>>24),(__u8)(a>>16),(__u8)(a>>8),(__u8)a
#define IPV4_ADR_PRINT_FMT  "%d.%d.%d.%d"

#define  UUID_STRUCT_MEMCPY(dst_addr, src_addr) memcpy(dst_addr, src_addr, sizeof(gsm_uuid_t))
#define  UUID_MEMBER_MEMCPY(dst_addr, src_addr) memcpy(dst_addr, src_addr, sizeof(gsm_uuid_t))
#define  UUID_STRUCT_MEMSET(addr) memset(addr, 0, sizeof(gsm_uuid_t))
#define  UUID_MEMBER_COMPARE_IS_DIFFERENT(srcaddr, dstaddr)  \
                         memcmp(srcaddr, dstaddr, sizeof(gsm_uuid_t))

typedef enum {
	CMD_EXEC_START = 1,
	CMD_EXEC_FAIL,
	CMD_EXEC_SUC
}cmd_exec_status_t;

#define PERF_TEST_RUNNING "/tmp/perf_test_running"
#define PERF_TEST_SUCCESS "/tmp/perf_test_success"
#define PERF_TEST_FAILURE "/tmp/perf_test_failure"

typedef enum {
    RSDB_2G,
    RSDB_5G,
    RSDB_SPLIT_2G_PLUS_5G,
    RSDB_NUM_MODES
} rsdb_mode_t;

typedef enum {
    DUAL_5G_DISABLED,
    DUAL_5G_ENABLED,
    DUAL_5G_AUTO,
    DUAL_5G_NUM_MODES
} dual5g_mode_t;

typedef enum {
    DUMP_TFTP,
    DUMP_SCP,
    DUMP_USB,
    /*
    *   Additional modes can be added to consolidate dump collection settings 
    *   into this infrastructure
    */
    DUMP_NUM_MODES
} dump_transfer_mode_t;

extern int          getSwVersion(swver *sv);
extern void         getBuild(swver *sv);

extern __u8         valid_display_string(char * str, int len);
extern __u8         validate_file_name(char * file_name, int len);
extern char       * mac_to_str (const __u8 * mac);
extern char       * mac_to_str_r (const __u8 * mac, char* buffer, boolean upper );
extern char       * mac_to_str_no_delemit (const __u8 * mac, char* buffer, boolean upper );
extern char       * mac_to_str_w_options (const __u8 * mac, char* buffer,
                                boolean upperCase, char delimiter );
extern __u8       * str_to_mac (char * mac_str);

extern char       * uuid_to_str(const unsigned char *uuid);
extern __u8       * str_to_uuid (char * str);

extern __u32        str_to_ip  (char * ip);
extern char *       ip_to_str  (__u32 ip);
extern __u8 *       ip_to_str2 (__u32 ip);
#ifdef __FAT_AP__
extern __u32        nh_addl(__u32 x, __u32 opr);
extern __u32        nn_addl(__u32 x, __u32 opr);
extern __u16        nh_adds(__u16 x, __u16 opr);
extern __u16        nn_adds(__u16 x, __u16 opr);
extern __u32        nh_subl(__u32 x, __u32 opr);
extern __u32        nn_subl(__u32 x, __u32 opr);
extern __u16        nh_subs(__u16 x, __u16 opr);
extern __u16        nn_subs(__u16 x, __u16 opr);
extern __u32        nh_andl(__u32 x, __u32 opr);
extern __u32        nn_andl(__u32 x, __u32 opr);
extern __u16        nh_ands(__u16 x, __u16 opr);
extern __u16        nn_ands(__u16 x, __u16 opr);
#endif

extern __u8 *       str_to_ip6      (char * ip);
extern char *       ip6_to_str      (__u8 *ip);
extern char *       ip6_to_str_long (__u8 *ip);

extern char       * loc_to_str (__u32 loc) ;
extern __u32        str_to_loc (__s8 * str) ;

extern char *       proto_to_str (__u16 proto);
extern __u16        str_to_proto (char * str);
extern __u8       * str_to_oui  (char * str);

extern boolean      execute_command (char * cmd);
extern boolean      execute_command_no_io (char * cmd);
extern void         getRtcTime(char *buf);
extern int	        getSysUptime(void);
extern double       getSysUptime_d(void);

extern boolean      get_boolean_value (char * value);
extern char *       get_boolean_str   (boolean value);

extern __u8 *       encr_to_str (__u32 encr);
extern char *       rate_offset_to_str (__u8 rate_offset, boolean isa);

extern char *       dbm_pwr_to_str(int pwr);
extern int          str_to_pwr_dbm(char *str);

extern __u8         xctoc (char c);
extern int          execute_command_status (char * cmd);
extern int          execute_command_status_no_io (char * cmd, __u8 * so, __u8 * se);
extern int          execute_command_status_no_io_nowait (char * cmd, char *so, char *se, char *status, char *del);
extern int          execute_command_status_nowait (char * cmd, char *so, char *se, char *status, char *del);
extern int          hextoint(char *ptr, int len);
extern __u32        get_build_time (void);
extern boolean execute_command_proc (__u8 chCmd, __u8 *file);
extern boolean cp_file_to_from(const char *to, const char *from);

#ifdef UC_CROSS
extern __u32        get_build_num (void);
#endif

extern unsigned int get_img_part_size(void);
extern int          sizeFlash(void);
extern int          freeFlash(void);
extern int          freeFlash1(void);
extern int          freeDisk(int under_report);
extern int          get_temp_dir(int size, char **tmpd);
extern int          get_free_mem(unsigned long long*, unsigned long long*);
extern int          get_free_mem_cached(unsigned long long*, unsigned long long*, unsigned long long*);
extern void         saveMachineInfo(char *filename);

extern char *       sec_to_str (__u32 sec);

int writeScciReg(int reg, int c);
int readScciReg(int reg, int *val);
int ReadNvram(unsigned long offset, unsigned long size, char *buffer);
int WriteNvram(unsigned long offset, unsigned long size, char *buffer);
int ReadCPLD(unsigned long reg, unsigned char *data);
int WriteCPLD(unsigned long reg, unsigned char *data);
int ReadCPLD16(unsigned long reg, unsigned short *data);
int WriteCPLD16(unsigned long reg, unsigned short *data);
void set_ap_flash_dev_name(char *);
char *get_ap_flash_dev_name(void);
#if defined (AP_PLATFORM)
int ReadApFlash(unsigned long offset, unsigned long size, unsigned char *buffer, char *devname);
int WriteApFlash(unsigned long offset, unsigned long size, unsigned char *buffer, char *devname);
#else
int ReadApFlash(unsigned long offset, unsigned long size, unsigned char *buffer);
int WriteApFlash(unsigned long offset, unsigned long size, unsigned char *buffer);
#endif
int ReadSamMachType(unsigned char* buffer);
int getEnv(char* var, char *val);
int forceGetEnv(char* var, char *val);
char *getApString(aruba_ap_type apType);
char *getRegApString(aruba_reg_index reg_index);    
int isOrtronicsAp(aruba_ap_type apType);
aruba_ap_type getApTypeFromString(char* ap_str) ;
aruba_reg_index getRegApIndexFromString(char* ap_str) ;
aruba_reg_index getRegApIndexFromType(aruba_ap_type ap_type);
int ids_stype_to_wifi_stype (int stype);
int wifi_stype_to_ids_stype (int stype);
void tpm_hw_init(void);
int WriteCertKey(unsigned long offset, unsigned long size, char *buffer);
int ReadCertKey(unsigned long offset, unsigned long size, char *buffer);


int get_mac_addr(__u8 *result, int in_len);
int get_sys_serial_num(__u8 *result, int in_len);
int get_sys_cpu_model(__u8 *, int);
char *get_x86_vm_type(void);
int get_total_cpu( int *);
int get_total_socket(int *);
int get_sys_service_tag(__u8 *result, int in_len);
int get_system_flash( unsigned long long *flash_size);
boolean is_x86_virtual_platform(void);

#if defined(CONFIG_NEBBIOLO)
int get_tpm_mac_addr_nebbiolo(__u8 *result, int in_len);
int get_mgmt_mac_addr(__u8 *result, int in_len);
#endif
#if defined(SWITCH_PLATFORM)
#include <asm/types.h>
#include <util/aruba_prom.h>
int get_mac_range(__u32 *range);
int ReadEeprom(unsigned long offset, unsigned long size, char *buffer);
int WriteEeprom(unsigned long offset, unsigned long size, char *buffer);
#if defined(CONFIG_PORFIDIO) || defined(CONFIG_MILAGRO)
int ReadIOEeprom(unsigned long offset, unsigned long size, char *buffer);
int WriteIOEeprom(unsigned long offset, unsigned long size, char *buffer);
int write_xloader_bootrom(unsigned long offset, unsigned long size, char *buffer);
int write_cpboot_bootrom(unsigned long offset, unsigned long size, char *buffer);
int read_xloader_bootrom(unsigned long offset, unsigned long size, char *buffer);
int read_cpboot_bootrom(unsigned long offset, unsigned long size, char *buffer);
int read_porfidio_bootrom(char *file, unsigned long offset, unsigned long size, char *buffer);
int write_porfidio_bootrom(char *file, unsigned long offset, unsigned long size, char *buffer);
#endif
int set_bootflash_part(int mxc_fd, int secondary);
int read_bootrom(unsigned long offset, unsigned long size, char *buffer);
int write_bootrom(unsigned long offset, unsigned long size, char *buffer);
int read_bootrom_env(unsigned long offset, unsigned long size, char *buffer);
int write_bootrom_env(unsigned long offset, unsigned long size, char *buffer);
int is_nebbiolo_card();
int chk_chksum(aruba_prom_t *manuf);
#endif /* CONFIG_NEBBIOLO */

#if defined(CONFIG_CABERNET) || defined(CONFIG_SYRAH) || defined(CONFIG_MALBEC)
#include <linux/rtc.h>
unsigned long DS1374_DateToBinary(struct rtc_time *rtc_tm);
int getTimeFromDS1374(struct rtc_time*);
int setTimeInDS1374(struct rtc_time*);
#endif

#if defined(CONFIG_PORFIDIO) || defined(CONFIG_MILAGRO)
extern int get_plat_type();
#endif

#if defined(APSIM_PLATFORM)
int get_apsim_serial_num(__u8 *result, int in_len);
#endif /* defined(APSIM_PLATFORM) */

extern int bssid_to_wifi_dev(aphw_t * ap_info, apradio_t *radios, __u8 *bssid);
extern int bssid_to_wifi_vap(apradio_t *radios, __u8 *bssid);

#if defined (AP_PLATFORM) || defined(CONFIG_GRENACHE)
extern aruba_ap_type getApType(void);
extern int getApFlashVariant(void);
extern int get_ap_booted_partition(void);
extern int get_assy_num(__u8 *result, int in_len);
enum booted_partition_values { BOOT_UNKNOWN = -2, BOOT_NETWORK, BOOT_PARTITION_0, BOOT_PARTITION_1 };
extern int get_mesh_role(void);
int is_mesh_ap(void);
int get_minor_rev_num(__u8 *result, int in_len);
int get_ap_serial_num(__u8 *result, int in_len);
int get_ap_service_tag(__u8 *result, int in_len);
int get_duowj_serial_num(char *arg, int bufsz) ;
int get_ap_max_eirp(int *result, int in_len);
#endif
extern int get_mesh_vap_id(aphw_t * ap_info, int vap_type);
#ifdef SWITCH_PLATFORM
char *sys_get_model(int);

char *sys_get_sc_type(int);
char *sys_get_sc_type_orig(int);
#endif

#if defined(CONFIG_GRENACHE)
extern int get_gren_mac_addr(__u8 *, int);
#endif

char *ap_user_visible_model(char *);
int sys_update_model(char *);
boolean sys_model_is_switch(char *);
extern int ap_is_ext_antenna_capable(aruba_ap_type apType);
extern int ap_has_rssi_meter(aruba_ap_type apType);
extern int ap_is_outdoor_ap(aruba_ap_type apType, int);

#ifndef WIN32
extern FILE *aruba_popen (const char *, const char *);
extern int aruba_pclose(FILE *);
extern FILE *aruba_popen_thr (const char *, const char *, pid_t *);
extern int aruba_pclose_thr(FILE *, pid_t);
#else /* WIN32 */
#  define aruba_popen   _popen
#  define aruba_pclose  _pclose
#  define aruba_popen_thr(_a_,_b_,_c_)  _popen((_a_), (_b_))
#  define aruba_pclose_thr(_a_,_b_)     _pclose((_a_))
#endif /* WIN32 */

/* returns argc.
 * cmd itself is modified to unescape quoting characters: ', ", \.
 * so cmd should be modifiable.
 * pointers in argv array point somewhere in cmd, so that they don't need
 * to be freeed.
 */
int cmdline_to_argv(int argc, char *argv[], char *cmd);
/* returns (would be) string length
 * All the arguments are quoted, so that shell special characters, such as
 * <, >, &, &&, |, ||, etc, will be treated as regular characters.
 */
int argv_to_cmdline(char *cmd, int cmdlen, int argc, char *argv[]);
/* returns (would be) string length
 * All the arguments get quoted, so that shell special characters, such as
 * <, >, &, &&, |, ||, etc, will be treated as regular characters.
 * All the arguments should be char *.  Last argument should be NULL.
 */
int args_to_cmdline(char *cmd, int cmdlen, ...);

/* check if the file name contains only alphanumeric, '-', '_' and '.'. */
int is_valid_restricted_filename(char *filename);

int upload_file(char *cfile, char *fname, char *url);

#ifdef UTIL_IMP
// private section

#endif


#ifdef UTIL_INIT
// initialization section

static char month_abbr[12][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};
#endif

#if defined(CONFIG_PORFIDIO) || defined(CONFIG_MILAGRO)
#define NORFLASH_SECT_SIZE          0x20000
#define NORFLASH_BOOTROM_OFFSET     0
#define NORFLASH_NVRAM_OFFSET       0
#define NORFLASH_BOOT_ENV_OFFSET    0x160000
#ifdef CONFIG_MILAGRO
#define BOOTROM_FILENAME            "/dev/cpboot" 
#else
#define BOOTROM_FILENAME            "/dev/cpboot1" 
#endif
#elif defined(X86_TARGET_PLATFORM)
#define NORFLASH_SECT_SIZE          0x20000
#define NORFLASH_BOOTROM_OFFSET     0
#define NORFLASH_NVRAM_OFFSET       0
#define NORFLASH_BOOT_ENV_OFFSET    0x160000
#define BOOTROM_FILENAME            "/dev/cpboot1" 
#elif defined(CONFIG_NEBBIOLO) 
#define NORFLASH_SECT_SIZE          0x20000
#define NORFLASH_BOOTROM_OFFSET     0
#define NORFLASH_NVRAM_OFFSET       0
#define NORFLASH_MESH_PROV_OFFSET   0x190000
#define NORFLASH_MESH_CERT_OFFSET   0x1a0000
#define NORFLASH_RAP_DATA_OFFSET    0x1b0000
#define NORFLASH_AP_SCRATCH_OFFSET  0x1c0000
#define NORFLASH_AP_CACHE_OFFSET    0x1d0000
#define NORFLASH_RADIO_DATA_OFFSET  0x1e0000
#define NORFLASH_BOOT_ENV_OFFSET    0x1f0000
#define BOOTROM_FILENAME            "/dev/bootrom"
#elif defined(CONFIG_GRENACHE) || defined(AP_PLATFORM)
// this stinks, but we need these defines to compile ap60_flash_common.h for
// other APs
#include <include/grenache_flash.h>
#endif

#define VERSIONS_COMPAT 1
#define VERSIONS_NOT_COMPAT 0

int isImgCompatWithCfg(unsigned int imgMajVer,   unsigned int imgMinVer, 
                       unsigned int imgMaintVer, unsigned int imgPatchVer,
                       unsigned int cfgMajVer,   unsigned int cfgMinVer,
                       unsigned int cfgMaintVer, unsigned int cfgPatchVer);

int isImgCompatWithCPU(unsigned int imgMajVer,   unsigned int imgMinVer, 
                       unsigned int imgMaintVer, unsigned int imgPatchVer);
                       
int chkUsbPath(char *srcfile, char *path_out);
int chkUsbSlotPartPath(char *srcfile, int slot, int part, char *path_out);

#define EMAIL_ADDR_LOCAL_LEN 64
#define EMAIL_ADDR_DOMAIN_LEN 255
#define EMAIL_ADDR_LOCAL_SPECIAL_CHARS "!#$%&'*+-/=?^_`.{|}~"

int email_addr_validate(const char *address);
int aruba_open_device(int o_mode);
char *str_to_lower(char *str);
extern __u8 valid_mac (__u8 * retbuf, __u8 *pValue);

void pap_hexify (char *buffer, char *str, int len);

#define UINT32_DIFF(c, a, b) {\
  if ((a) >= (b)) \
    (c) = (a) - (b); \
  else \
    (c) = UINT_MAX - (b) + (a); \
}

/*
 * Domain name validation
 */
typedef enum {
     DOMAIN_NAME_VALID = 0,
     DOMAIN_NAME_INVALID_CHARS,
     DOMAIN_NAME_INVALID_POSITION
}domain_name_err_code_t;

extern domain_name_err_code_t domain_name_validate(const char *name);
extern int validate_lag_num(char *);
extern boolean is_numeric (char *s, __u32 *);
extern boolean is_unicast_ipaddr(__u32 ip);

#define AP_MASTER_DISCOVERY_TYPE_FILE "/tmp/discover_type"
extern int write_str_to_file(char* fname, char* str);

#define PROCESS_NAME_SYSLOGD          "syslogd"
extern int process_normal(char* pname);

extern int dpi_is_enabled(void);
extern void removeSpaces(char *string);
extern int removeSubString(char *subString, char *orgString);
extern int get_disaster_recovery_status(void);
extern int get_central_managed_status(void);

#define DPI_ENABLED     "dpi_enabled"
#define DPI_DISABLED     "dpi_disabled"
#define BR_UPGRADE_BOOTARGS  "br_upgrade -i bootargs -s '"

#if defined(CONFIG_MILAGRO)
#define DPI_DTS             "/mswitch/bin/milagro_7280_feat_wdpi.dtb.signed"
#define DTBUPDATE_BIN       "/mswitch/bin/dtbupdate"
#define DPI_DTBUPDATE_CMD   DTBUPDATE_BIN " -u " DPI_DTS
#define DPI_DTBCLEAR_CMD    DTBUPDATE_BIN " -c "
#endif

extern pid_t gettid(void);

enum ap_version_format_values {IAP_VER_FORMAT, FULL_VER_FORMAT};
extern char * ap_version_format(char *, char *, unsigned l, int format);
#ifdef __FAT_AP__
typedef struct flash_ophist {
    __u32 size;
    __u32 wpos;
    struct {
        __u32 stamp;
        __u32 offset;
#define FLASH_OPWR 0
#define FLASH_OPRD 1
        __u32 op;
        __u32 bytes;
        __u32 cost;
        __s8 comm[32];
        __s8 pcomm[32];
    } items[0];
} __attribute__((packed)) flash_ophist;

flash_ophist *flash_ophist_init();

#define FLASH_OPHIST_SHM    "/tmp/flashophist"

enum eol_ver_check_values { NON_EOL_AP, EOL_AP_VER_CHECK_SUCCEED, EOL_AP_VER_CHECK_FAIL, EOL_AP_VER_LEN_ERROR};
extern void clear_quotes( char *str);
extern char * get_os_version(void);
struct ap_ver{
    int mamaj; 
    int mamin;
    int marev;
    int mapatch;
    int miapmaj;
    int miapmin;
    int miaprev;
    int miappatch;
};
extern int match_ap_version(char* ver, struct ap_ver *match_ver);
extern int check_eol_ap_version(char* ver, int ap_type);
extern int compare_r_version_with_c_version(char *r_version, int *r_ver_len, char *c_version, int *c_ver_len);
#endif

extern int version_within_range(ap_image_version_range_t *vr, ap_image_version_t *iv);
void cfg_dpi_enabled(int current_enabled, int next_enabled);

typedef enum {
    MASTER_MODEL_UNKNOWN,
    MASTER_MODEL_INVALID,
    MASTER_MODEL_MM,
    MASTER_MODEL_LEGACY_MM,
} legacy_mm_t;
extern int is_legacy_mm(void);
extern int is_model_legacy_mm(char *model);
extern int is_master_with_tpm(void);
extern int is_platform_with_tpm(void);

#if defined(X86_TARGET_PLATFORM)
typedef enum {
    X86_VM_PLAT,
    X86_HMM_PLAT,
} x86_hw_plat_t;    
#define HMM_FILE "/tmp/.ishwMM"
#define DPI_ENABLED_FILE "/flash/.dpi"
#define DRIVE_PFX "/tmp/dpfx"

# define TOKEN "kernel"
# define APPEND_TOKEN "console=ttyS0"
#define TOKEN_SERIAL "serial"
#define TOKEN_TIMEOUT "timeout"
#define X86_GRUB_SERIAL_LINE1 "serial --speed=9600 --unit=0 --word=8 --parity=no --stop=1\r\n"
#define X86_GRUB_SERIAL_LINE2 "terminal --timeout=10 console serial\r\n"
#define X86_GRUB_CONF_FILE1      "/mnt/grub/boot/grub/grub.conf"
#define X86_GRUB_CONF_FILE2      "/mnt/grub/grub/grub.conf"
#define X86_GRUB_CONF_TMP      "/tmp/tmp_grub.conf"
#define X86_GRUB_CONF_TMP_ID      "/tmp/tmp_grub_%d.conf"
#define X86_GRUB_PART_TRGT      "/mnt/grub"
#define X86_GRUB_PART_TYPE_EXT3 "ext3"
#define X86_GRUB_PART_TYPE_EXT4 "ext4"
#define X86_GRUB_EXE1_NAME "e2fs_stage1_5"
#define X86_GRUB_EXE2_NAME "stage1"
#define X86_GRUB_EXE3_NAME "stage2"
#define X86_GRUB_EXE_PATH "/mnt/grub/boot/grub/"
#define X86_GRUB_EXE1_PATH "/mnt/grub/boot/grub/e2fs_stage1_5"
#define X86_GRUB_EXE2_PATH "/mnt/grub/boot/grub/stage1"
#define X86_GRUB_EXE3_PATH "/mnt/grub/boot/grub/stage2"
#define X86_SIGNED_FILE "/mnt/grub/boot/grub/signed_grub"
#define X86_GRUB_VERIFY_EXE "/mswitch/bin/grub_verify"

int findFileinDir(char *dir, char *fileName, int fileNameLength, char *fullfileName);
int filecopyfromto(char *srcfilename, char *dstfilename);
int removeFilesinDir(char *dir);
int readUpdateGrubConf(char *file, int partNum);
int updateGrubConfigx86(char *partitionName, int partNum);
int getGrubPartitionName(char *name, int nameLen);
int getLicensePartitionName(char *name, int nameLen);
int getFailsafePartitionName(char *name, int nameLen);
#endif

#define LOCAL_CONIFGURE_STATUS_FILE   "/tmp/.local_configure_enable" 
#define DISASTER_RECOVERY_ROLE_FILE   "/tmp/.disaster_recovery_on"
#define LOCAL_CONFIGURE_WARN_STRING   "This controller is managed by a " \
                                      "Mobility Master.\n" \
                                      "Configuration changes can only be " \
                                      "performed on the Mobility Master."
extern void reboot_me(__u8 rb_code);
#define TECH_SUPPORT_LOG_FILE "/var/log/oslog/tech-support.log"

#define CENTRAL_CFG_FILE "/flash/central.cfg"
#define ATH_CENTRAL_MANAGED_FILE "/flash/central_managed"
#define ATH_CENTRAL_MANAGED_STR "\r\nOperation not permitted. This controller "\
                                "is being managed by Aruba Central.\n"

#define ZTP_RESTART_PROCESSES "/tmp/ztp_restart_processes"
#define AWS_DETECTION_FILE "/tmp/.awscloud"

#if defined(CONFIG_MASTERSON) || defined(CONFIG_KILCHOMAN) || defined(CONFIG_LAGAVULIN)
/*
    GB2017 maybe_packed
    to avoid alignment faults (arm64),
    alignement handling via exceptions (arm)
    or access extra bus cycles (powerpc, arm64),
    keep local structures aligned by leaving the macro empty.
    If space is really an issue, one can reorder fields.
*/
# define maybe_packed
#else

# define maybe_packed __attribute__ ((packed))
#endif

#define L3_MASTER_SWITCHED_OVER_FILE "/flash/.l3_master_switched_over"

extern boolean is_central_managed(void);
extern boolean is_aws_cloud(void);
extern int getrole(void);
#ifndef __FAT_AP__
extern int is_master(void);
#endif
extern boolean is_mm_cloud_mode(void);
int get_setup_role(void);

extern int get_global_cfgid (void) ;
int is_flash1_present(void);
int is_flash_crash_present(void);
int checkSosCrash(void);
int get_temp_dir2(int, char **);

// added for process snapshot
#ifdef EMS_PLATFORM
#define DEFAULT_ARUBALOG        "/var/log/"
#else
#define DEFAULT_ARUBALOG        "/var/log/oslog/"
#define DEFAULT_ARUBA_TRACELOG        "/var/log/trace/"
#endif

void get_process_logs(char *srcdir, char *destdir);
void get_process_conf(char *destdir);

extern int get_l3_master_switched_over_status(void);
extern int set_l3_master_switched_over_status(void);
extern int is_valid_fqdn(const char *);
extern int is_standalone(void);

static inline
__u32
concatenate_from_gsm_encr_alg(__u16 lsb, __u16 msb)
{
    __u32 retVal = ( (__u32)(msb << 16) | (__u32)lsb);
    return retVal;
}

static inline
void
split_into_gsm_encr_alg(__u32 value, __u16* lsb, __u16* msb)
{
    if (NULL != lsb && NULL != msb) {
        *lsb = (__u16) (value & 0xFFFF);
        *msb = (__u16) ((value >> 16) & 0xFFFF);
    }
}

extern void mark_device_upgraded(void);
extern int is_device_upgraded(void);
#endif

//For Postgres DB schema and data upgrade
#ifndef POSTGRES_DB_UPGRADE
#define POSTGRES_DB_UPGRADE
int upgrade_postgres_db(char *);
int upgrade_postgres_db_data(char *);

#define PROVISION_TYPE_FILE "/flash/.provision_type_file"
#define PROVISION_TYPE_SIZE 8
extern int set_provision_type(int);
extern int get_provision_type(void);
#endif

