#ifndef ARUBA_TRACE_H
#define ARUBA_TRACE_H

#include <time.h>
#include <stdio.h>
#include <asm/types.h>
#include <stdarg.h>
#include <syslog.h>
#include "aruba_trace_comp.h"
#ifdef __FAT_AP__
#include "../util/ipv6_cmn.h"
#else
#include <util/ipv6_cmn.h>
#endif

#define  TRACE_MAX_SUB_COMP      32

#define ARUBA_TRACE_MAXPATHLEN           256
#define ETH_ALEN                         6

#ifdef AP_PLATFORM
#ifdef __FAT_AP__
#define ARUBA_MAX_TRACE_ENTRIES          128/* This msg buffer can be read from core dump */ 
#else /*__FAT_AP__ */
#define ARUBA_MAX_TRACE_ENTRIES          1
#endif /*__FAT_AP__ */
#else /* AP_PLATFORM */
#define ARUBA_TRACE_THREAD_SAFE          1  /* We need thread safe LIB for these platforms */
#endif /* AP_PLATFORM */

#define ARUBA_MAX_TRACE_CHAR_PER_LINE    512 

#ifdef ARUBA_TRACE_THREAD_SAFE
    #define __ARUBA_TRACE_INT_LOCK(x)  __trace_lock(x)
    #define __ARUBA_TRACE_INT_UNLOCK(x)  __trace_unlock(x)
#else
    #define __ARUBA_TRACE_INT_LOCK() 
    #define __ARUBA_TRACE_INT_UNLOCK()
#endif /* ARUBA_TRACE_THREAD_SAFE */

#define	T_EMER      LOG_EMERG
#define	T_ALERT     LOG_ALERT
#define	T_CRIT      LOG_CRIT	
#define	T_ERR       LOG_ERR		
#define	T_WARN      LOG_WARNING	
#define	T_NOTICE    LOG_NOTICE
#define	T_INFO      LOG_INFO
#define	T_DBG       LOG_DEBUG
#define ARUBA_TRACE_TOTAL_LEVELS  8

typedef struct _trace_time {
    int         tt_up_to_date;      /* Set when strings are up to date */
    time_t      tt_str_time;     /* Set to time corresponding to strings */
    char        tt_str[16];
    char        tt_ctime[26];
} trace_time_t;


typedef struct _trace_file_ctx {
    char            tf_file[ARUBA_TRACE_MAXPATHLEN];      /* File, where trace will go */
    FILE           *tf_stream;        /* File/stream FD,TODO: change to FD if req */
    off_t           tf_f_limit_size;
    off_t           tf_f_size;
    unsigned int    tf_limit_files;
} trace_file_ctx_t;

typedef struct aruba_trace_buff_s {
    char            tdata[ARUBA_MAX_TRACE_CHAR_PER_LINE+1];
}aruba_trace_buff_t;


/* component name id, like mdns, stm, cli, snmp, l2tp... */
typedef struct _trace_sub_comp {
   char     *tsc_name;
   char     *tsc_log_name;   /* 3-6 char only */
} trace_sub_comp_t;

/* Sub component or module */
typedef struct _trace_comp {
    char                *tc_name;
    trace_sub_comp_t     tc_sub_comp[TRACE_MAX_SUB_COMP];
} trace_comp_t;

typedef struct _trace_level_str {
    int     tl_level;
    char    *tl_name;
}trace_level_str_t ;


extern trace_comp_t  g_trace_comp[];
extern trace_level_str_t g_trace_lvl_str[];
#ifndef ARUBA_TRACE_THREAD_SAFE
extern aruba_trace_buff_t aruba_trace_buffer[ARUBA_MAX_TRACE_ENTRIES];
#endif /* ARUBA_TRACE_THREAD_SAFE */
extern int g_trace_enable_in_fg;
extern __u16 trace_context;
extern __u32 aruba_trace_filter_module_flag;

#define ARUBA_TRACE_LOGS(count) { \
    __u32 first_row = 0; \
    __u16 local_context = 0; \
    __u32 final_context = 0; \
    aruba_trace_buff_t * trace_entry; \
\
    if (count > ARUBA_MAX_TRACE_ENTRIES){ \
        count = ARUBA_MAX_TRACE_ENTRIES; \
    } \
\
    first_row = ARUBA_MAX_TRACE_ENTRIES - count; \
    local_context = (trace_context + first_row) % ARUBA_MAX_TRACE_ENTRIES; \
    final_context = (trace_context % ARUBA_MAX_TRACE_ENTRIES); \
    while (final_context != local_context) {\
        trace_entry = &aruba_trace_buffer[local_context ++];\
        local_context = local_context % ARUBA_MAX_TRACE_ENTRIES; \
        snprintf(amapi_col[0],AMAPI_MAX_COLUMN_WIDTH,"%s",trace_entry->tdata); \
        add_data(amapi_buf,"",amapi_col[0],FALSE); \
    } \
}

#define ARUBA_MACCMP(S, D) \
    ((((__u16 *)(S))[0] ^ ((__u16 *)(D))[0]) | \
     (((__u16 *)(S))[1] ^ ((__u16 *)(D))[1]) | \
     (((__u16 *)(S))[2] ^ ((__u16 *)(D))[2]))

#define TRACE_IS_SUB_COMP_ENABLE(x) \
    (aruba_trace_filter_module_flag & (1 << x))

extern int g_comp_id; 
static inline const char * trace_get_subcomp_name(__u32 x)
{
    return ((x < TRACE_MAX_SUB_COMP) ? g_trace_comp[g_comp_id].tc_sub_comp[x].tsc_name:"INVALID-sub-comp");
}

extern void aruba_trace_msg (__u32 msg_id,__u32 log_level,const char * fn_name,__u32 line_no,const char * fmt, ...);
extern void aruba_tracing (__u32 flag,__u32 log_level,__u8 * mac,__u32 ip,const char * fn_name,__u32 line_no,const char * fmt, ...);
extern void ipv6_aruba_tracing(__u32 sub_comp_id,__u32 log_level,__u8 * mac, ip_addr_t ip,const char * fn_name,__u32 line_no,const char * fmt, ...);
extern void aruba_tracing_lite(__u32 sub_comp_id, __u32 log_level, const char * fmt, ...);
extern void trace_module_update_as_it_is(__u32 sub_comp_id);
extern void vtrace_buffer_fn (__u32 flag,__u32 log_level,__u8 * mac,__u32 ip,const char * fn_name,__u32 line_no,const char * fmt,va_list arg);
extern void ipv6_vtrace_buffer_fn(__u32 sub_comp_id,__u32 log_level,__u8 * mac, ip_addr_t ip,const char * fn_name,__u32 line_no,const char * fmt,va_list arg);

extern __u32 aruba_tracing_filter(__u32 flag,__u8 * mac,__u32 ip,__u32 log_level);
extern void trace_module_update(__u32 flag,__u8 set);
extern void trace_mac_update (__u8 * mac);
extern void trace_ip_update (__u32 ip);
extern void trace_log_level_update(__u32 log_level);
extern void trace_write_2_file(char *msg, int msg_len);
extern __u8 * trace_get_mac_filter(void);
extern __u32 trace_get_module_flag(void);
extern __u8 trace_get_log_level(void);
extern int trace_get_ip_filter(ip_addr_t *ip);

extern char *
task_timer_get_wall_str(void);
extern void
aruba_trace_init(__u32 comp_id, char *trace_file_name, off_t each_file_sz_limit, unsigned int max_trace_files);
extern trace_file_ctx_t  g_aruba_trace_file;
extern int g_trace_enable_in_flag; 
extern char* trace_get_lvl_str(int lvl);

#ifdef __FAT_AP__
extern int varuba_tracing(__u32 sub_comp_id,__u8 * mac,__u32 ip,const char * fn_name,__u32 line_no,const char * fmt,va_list arg);
#else
extern void varuba_tracing(__u32 sub_comp_id,__u8 * mac,__u32 ip,const char * fn_name,__u32 line_no,const char * fmt,va_list arg);
#endif

extern int trace_handle_cfg_req(__u8 *msg, __u32 len);
extern __u32 trace_encode_cfg_msg(__u8 *ptr, __u8 *mac, __u32 ip, __u32 sub_comp, __u8 level);
#if defined(AP_PLATFORM) && !defined(__FAT_AP__)
extern void aruba_trace_set_limit_size(off_t each_file_sz_limit);
#endif
#endif // ARUBA_TRACE_H
