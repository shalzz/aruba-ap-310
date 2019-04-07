#ifndef ARUBA_TRACE_COMP_H
#define ARUBA_TRACE_COMP_H

typedef enum _trace_comp_id {
    TRACE_MDNS,
    TRACE_CLI0,
    TRACE_SNMP,
    TRACE_L2TP,
    TRACE_STM,
    TRACE_DPIMGR,
    TRACE_LHM,
#ifndef __FAT_AP__
    TRACE_AUTH,
    TRACE_TNM,
    TRACE_UV,
#endif
    TRACE_TOTAL_COMP_NUM,
} trace_comp_id_t;

/* Library or common infra like syslog sub-components, allocation
   happens from MSB, so that applications can have their sub-component from
   LSB */
#define TRACE_SYSLOG                  27     // All syslog
#define DT_TRACE_F_HELLO_OP           28     // generic operation
#define DT_TRACE_F_GEN_OP             29     // generic operation
#define DT_TRACE_F_TBL_OP             30     // table operations
#define DT_TRACE_F_SOCK_OP            31     // data send/recv


/* MDNS sub-component (modules */
typedef enum _trace_mdns_subcomp_id {
   /* 0  */     MDNS_TRACE_FLAG_RECORDS,
   /* 1  */     MDNS_TRACE_FLAG_SLAVE  ,
   /* 2  */     MDNS_TRACE_FLAG_MASTER ,
   /* 3  */     MDNS_TRACE_FLAG_CPPM   ,
   /* 4  */     MDNS_TRACE_FLAG_PAPI   ,
   /* 5  */     MDNS_TRACE_FLAG_HOST   ,
   /* 6  */     MDNS_TRACE_FLAG_RFC3576,
   /* 7  */     MDNS_TRACE_FLAG_UPD    ,
   /* 8  */     MDNS_TRACE_FLAG_DB_SYNC,
   /* 9  */     MDNS_TRACE_FLAG_QUERY  ,
   /* 10 */     MDNS_TRACE_FLAG_GENL   ,
   /* 11 */     MDNS_TRACE_NOT_USED_11   ,
   /* 12 */     MDNS_TRACE_NOT_USED_12  ,
   /* 13 */     MDNS_TRACE_NOT_USED_13   ,
   /* 14 */     MDNS_TRACE_NOT_USED_14   ,
   /* 15 */     MDNS_TRACE_NOT_USED_15   ,
   /* 16 */     MDNS_TRACE_NOT_USED_16   ,
   /* 17 */     MDNS_TRACE_NOT_USED_17   ,
   /* 18 */     MDNS_TRACE_NOT_USED_18   ,
   /* 19 */     MDNS_TRACE_NOT_USED_19   ,
   /* 20 */     MDNS_TRACE_NOT_USED_20   ,
   /* 21 */     MDNS_TRACE_NOT_USED_21   ,
   /* 22 */     MDNS_TRACE_NOT_USED_22   ,
   /* 23 */     MDNS_TRACE_NOT_USED_23   ,
   /* 24 */     MDNS_TRACE_NOT_USED_24   ,
   /* 25 */     MDNS_TRACE_NOT_USED_25   ,
   /* 26 */     MDNS_TRACE_NOT_USED_26   ,
   /* 27 */     MDNS_TRACE_SYSLOG        , /* Used for syslog for all components */
   /* 28 */     MDNS_DT_TRACE_F_HELLO_OP   ,  /* MDNS will use DT library */
   /* 29 */     MDNS_DT_TRACE_F_GEN_OP   ,  /* MDNS will use DT library */
   /* 30 */     MDNS_DT_TRACE_F_TBL_OP   ,  /* MDNS will use DT library */
   /* 31 */     MDNS_DT_TRACE_F_SOCK_OP   ,  /* MDNS will use DT library */
} trace_mdns_subcomp_id_t;


/* CLI Subcomponent */
typedef enum _trace_cli_subcomp_id {
   /* 0  */     CLI_TRACE_FLAG_SNMP,
   /* 1  */     CLI_TRACE_FLAG_CONF  ,
   /* 2  */     CLI_TRACE_FLAG_MNGM ,
   /* 3  */     CLI_TRACE_FLAG_TIMERANGE   ,
   /* 4  */     CLI_TRACE_NOT_USED_4   ,
   /* 5  */     CLI_TRACE_NOT_USED_5   ,
   /* 6  */     CLI_TRACE_NOT_USED_6,
   /* 7  */     CLI_TRACE_NOT_USED_7    ,
   /* 8  */     CLI_TRACE_NOT_USED_8,
   /* 9  */     CLI_TRACE_NOT_USED_9  ,
   /* 10 */     CLI_TRACE_NOT_USED_10   ,
   /* 11 */     CLI_TRACE_NOT_USED_11   ,
   /* 12 */     CLI_TRACE_NOT_USED_12  ,
   /* 13 */     CLI_TRACE_NOT_USED_13   ,
   /* 14 */     CLI_TRACE_NOT_USED_14   ,
   /* 15 */     CLI_TRACE_NOT_USED_15   ,
   /* 16 */     CLI_TRACE_NOT_USED_16   ,
   /* 17 */     CLI_TRACE_NOT_USED_17   ,
   /* 18 */     CLI_TRACE_NOT_USED_18   ,
   /* 19 */     CLI_TRACE_NOT_USED_19   ,
   /* 20 */     CLI_TRACE_NOT_USED_20   ,
   /* 21 */     CLI_TRACE_NOT_USED_21   ,
   /* 22 */     CLI_TRACE_NOT_USED_22   ,
   /* 23 */     CLI_TRACE_NOT_USED_23   ,
   /* 24 */     CLI_TRACE_NOT_USED_24   ,
   /* 25 */     CLI_TRACE_NOT_USED_25   ,
   /* 26 */     CLI_TRACE_NOT_USED_26   ,
   /* 27 */     CLI_TRACE_SYSLOG   , /* Used for syslog for all components */
   /* 28 */     CLI_TRACE_NOT_USED_28   ,
   /* 29 */     CLI_TRACE_NOT_USED_29   ,
   /* 30 */     CLI_TRACE_NOT_USED_30   ,
   /* 31 */     CLI_TRACE_NOT_USED_31   ,
} trace_cli_subcomp_id_t;
 
/* SNMP Subcomponent */
typedef enum _trace_snmp_subcomp_id {
   /* 0  */     SNMP_TRACE_FLAG_TRAP,
   /* 1  */     SNMP_TRACE_FLAG_IAP_MIB  ,
   /* 2  */     SNMP_TRACE_NOT_USED_2 ,
   /* 3  */     SNMP_TRACE_NOT_USED_3   ,
   /* 4  */     SNMP_TRACE_NOT_USED_4   ,
   /* 5  */     SNMP_TRACE_NOT_USED_5   ,
   /* 6  */     SNMP_TRACE_NOT_USED_6,
   /* 7  */     SNMP_TRACE_NOT_USED_7    ,
   /* 8  */     SNMP_TRACE_NOT_USED_8,
   /* 9  */     SNMP_TRACE_NOT_USED_9  ,
   /* 10 */     SNMP_TRACE_NOT_USED_10   ,
   /* 11 */     SNMP_TRACE_NOT_USED_11   ,
   /* 12 */     SNMP_TRACE_NOT_USED_12  ,
   /* 13 */     SNMP_TRACE_NOT_USED_13   ,
   /* 14 */     SNMP_TRACE_NOT_USED_14   ,
   /* 15 */     SNMP_TRACE_NOT_USED_15   ,
   /* 16 */     SNMP_TRACE_NOT_USED_16   ,
   /* 17 */     SNMP_TRACE_NOT_USED_17   ,
   /* 18 */     SNMP_TRACE_NOT_USED_18   ,
   /* 19 */     SNMP_TRACE_NOT_USED_19   ,
   /* 20 */     SNMP_TRACE_NOT_USED_20   ,
   /* 21 */     SNMP_TRACE_NOT_USED_21   ,
   /* 22 */     SNMP_TRACE_NOT_USED_22   ,
   /* 23 */     SNMP_TRACE_NOT_USED_23   ,
   /* 24 */     SNMP_TRACE_NOT_USED_24   ,
   /* 25 */     SNMP_TRACE_NOT_USED_25   ,
   /* 26 */     SNMP_TRACE_NOT_USED_26   ,
   /* 27 */     SNMP_TRACE_SYSLOG   , /* Used for syslog for all components */
   /* 28 */     SNMP_TRACE_NOT_USED_28   ,
   /* 29 */     SNMP_TRACE_NOT_USED_29   ,
   /* 30 */     SNMP_TRACE_NOT_USED_30   ,
   /* 31 */     SNMP_TRACE_NOT_USED_31   ,
} trace_snmp_subcomp_id_t;

/* L2TP Subcomponent */
typedef enum _trace_l2tp_subcomp_id {
   /* 0  */    L2TP_TRACE_PROTOCOL, 
   /* 1  */    L2TP_TRACE_FSM,
   /* 2  */    L2TP_TRACE_API,
   /* 3  */    L2TP_TRACE_AVPDATA,
   /* 4  */    L2TP_TRACE_FUNC,
   /* 5  */    L2TP_TRACE_XPRT,
   /* 6  */    L2TP_TRACE_DATA,
   /* 7  */    L2TP_TRACE_SYSTEM,
   /* 8  */    L2TP_TRACE_CFG,
   /* 9  */    L2TP_TRACE_ERROR,
   /* 10 */     L2TP_TRACE_NOT_USED_10   ,
   /* 11 */     L2TP_TRACE_NOT_USED_11   ,
   /* 12 */     L2TP_TRACE_NOT_USED_12  ,
   /* 13 */     L2TP_TRACE_NOT_USED_13   ,
   /* 14 */     L2TP_TRACE_NOT_USED_14   ,
   /* 15 */     L2TP_TRACE_NOT_USED_15   ,
   /* 16 */     L2TP_TRACE_NOT_USED_16   ,
   /* 17 */     L2TP_TRACE_NOT_USED_17   ,
   /* 18 */     L2TP_TRACE_NOT_USED_18   ,
   /* 19 */     L2TP_TRACE_NOT_USED_19   ,
   /* 20 */     L2TP_TRACE_NOT_USED_20   ,
   /* 21 */     L2TP_TRACE_NOT_USED_21   ,
   /* 22 */     L2TP_TRACE_NOT_USED_22   ,
   /* 23 */     L2TP_TRACE_NOT_USED_23   ,
   /* 24 */     L2TP_TRACE_NOT_USED_24   ,
   /* 25 */     L2TP_TRACE_NOT_USED_25   ,
   /* 26 */     L2TP_TRACE_NOT_USED_26   ,
   /* 27 */     L2TP_TRACE_NOT_USED_27   , /* Used for syslog for all components */
   /* 28 */     L2TP_TRACE_NOT_USED_28   ,
   /* 29 */     L2TP_TRACE_NOT_USED_29   ,
   /* 30 */     L2TP_TRACE_NOT_USED_30   ,
   /* 31 */     L2TP_TRACE_NOT_USED_31   ,
} trace_l2tp_subcomp_id_t;

/* STM Subcomponent */
typedef enum _trace_stm_subcomp_id {
   /* 0  */     STM_TRACE_AUTH, 
   /* 1  */     STM_TRACE_CFG  ,
   /* 2  */     STM_TRACE_STA ,
   /* 3  */     STM_TRACE_BSS   ,
   /* 4  */     STM_TRACE_CLUSTER   ,
   /* 5  */     STM_TRACE_SAPM   ,
   /* 6  */     STM_TRACE_GSM,
   /* 7  */     STM_TRACE_RADIO    ,
   /* 8  */     STM_TRACE_SYS,
   /* 9  */     STM_TRACE_ENET  ,
   /* 10 */     STM_TRACE_SOS   ,
   /* 11 */     STM_TRACE_AMON   ,
   /* 12 */     STM_TRACE_MAX  ,
   /* 13 */     STM_TRACE_NOT_USED_13  ,
   /* 14 */     STM_TRACE_NOT_USED_14   ,
   /* 15 */     STM_TRACE_NOT_USED_15   ,
   /* 16 */     STM_TRACE_NOT_USED_16   ,
   /* 17 */     STM_TRACE_NOT_USED_17   ,
   /* 18 */     STM_TRACE_NOT_USED_18   ,
   /* 19 */     STM_TRACE_NOT_USED_19   ,
   /* 20 */     STM_TRACE_NOT_USED_20   ,
   /* 21 */     STM_TRACE_NOT_USED_21   ,
   /* 22 */     STM_TRACE_NOT_USED_22   ,
   /* 23 */     STM_TRACE_NOT_USED_23   ,
   /* 24 */     STM_TRACE_NOT_USED_24   ,
   /* 25 */     STM_TRACE_NOT_USED_25   ,
   /* 26 */     STM_TRACE_NOT_USED_26   ,
   /* 27 */     STM_TRACE_SYSLOG   ,      /* Used for syslog for all components */
   /* 28 */     STM_DT_TRACE_F_HELLO_OP   ,  /* STM uses DT library */
   /* 29 */     STM_DT_TRACE_F_GEN_OP   ,  /* STM uses DT library */ 
   /* 30 */     STM_DT_TRACE_F_TBL_OP   ,  /* STM uses DT library */
   /* 31 */     STM_DT_TRACE_F_SOCK_OP   ,  /* STM uses DT library */
} trace_stm_subcomp_id_t;

/* DPIMGR Subcomponent */
typedef enum _trace_dpimgr_subcomp_id {
   /* 0  */     DPIMGR_TRACE_GENERAL            , 
   /* 1  */     DPIMGR_TRACE_CLI                , 
   /* 2  */     DPIMGR_TRACE_DATA_QOSMOS        , 
   /* 3  */     DPIMGR_TRACE_CONTROL_QOSMOS     , 
   /* 4  */     DPIMGR_TRACE_CONTROL            , 
   /* 5  */     DPIMGR_TRACE_FW_VISIBILITY      , 
   /* 6  */     DPIMGR_TRACE_DATA_BCA           , 
   /* 7  */     DPIMGR_TRACE_CONTROL_BCA        , 
   /* 8  */     DPIMGR_TRACE_URL_VISIBILITY     , 
   /* 27 */     DPIMGR_TRACE_SYSLOG     = 27    , /* Used for syslog for all components */
   /* 31 */     DPIMGR_TRACE_MAX        = 31    ,
} trace_dpimgr_subcomp_id;

/* LHM Subcomponent */
typedef enum _trace_lhm_subcomp_id {
   /* 0  */     LHM_TRACE_GENERAL            , 
   /* 1  */     LHM_TRACE_POLICY             , 
   /* 2  */     LHM_TRACE_PROTOBUF           , 
   /* 3  */     LHM_TRACE_PAPI               , 
   /* 4  */     LHM_TRACE_JOB                , 
   /* 27 */     LHM_TRACE_SYSLOG     = 27    , /* Used for syslog for all components */
   /* 31 */     LHM_TRACE_MAX        = 31    ,
} trace_lhm_subcomp_id;

/* AUTH Subcomponent */
typedef enum _trace_auth_subcomp_id {
    /* 0 */     AUTH_TRACE_STM,
    /* 1 */     AUTH_TRACE_CLUSTER,
    /* 2 */     AUTH_TRACE_ESSID,
    /* 3 */     AUTH_TRACE_BSSID,
    /* 4 */     AUTH_TRACE_STA,
    /* 5 */     AUTH_TRACE_MACUSER,
    /* 6 */     AUTH_TRACE_SOS,
    /* 7 */     AUTH_TRACE_MAX,
    /* 8 */     AUTH_TRACE_NOT_USED_8    ,
    /* 9 */     AUTH_TRACE_NOT_USED_9    ,
   /* 10 */     AUTH_TRACE_NOT_USED_10   ,
   /* 11 */     AUTH_TRACE_NOT_USED_11   ,
   /* 12 */     AUTH_TRACE_NOT_USED_12   ,
   /* 13 */     AUTH_TRACE_NOT_USED_13   ,
   /* 14 */     AUTH_TRACE_NOT_USED_14   ,
   /* 15 */     AUTH_TRACE_NOT_USED_15   ,
   /* 16 */     AUTH_TRACE_NOT_USED_16   ,
   /* 17 */     AUTH_TRACE_NOT_USED_17   ,
   /* 18 */     AUTH_TRACE_NOT_USED_18   ,
   /* 19 */     AUTH_TRACE_NOT_USED_19   ,
   /* 20 */     AUTH_TRACE_NOT_USED_20   ,
   /* 21 */     AUTH_TRACE_NOT_USED_21   ,
   /* 22 */     AUTH_TRACE_NOT_USED_22   ,
   /* 23 */     AUTH_TRACE_NOT_USED_23   ,
   /* 24 */     AUTH_TRACE_NOT_USED_24   ,
   /* 25 */     AUTH_TRACE_NOT_USED_25   ,
   /* 26 */     AUTH_TRACE_NOT_USED_26   ,
   /* 27 */     AUTH_TRACE_SYSLOG   ,      /* Used for syslog for all components */
   /* 28 */     AUTH_TRACE_NOT_USED_28   ,
   /* 29 */     AUTH_TRACE_NOT_USED_29   ,
   /* 30 */     AUTH_TRACE_NOT_USED_30   ,
   /* 31 */     AUTH_TRACE_NOT_USED_31   ,
} trace_auth_subcomp_id_t;


/* TNM Subcomponent */
typedef enum _trace_tnm_subcomp_id {
    /* 0 */     TNM_TRACE_CLUSTER,
    /* 1 */     TNM_TRACE_PROCESS,
    /* 2 */     TNM_TRACE_PAPI,
    /* 3 */     TNM_TRACE_SOS,
    /* 4 */     TNM_TRACE_TUN_HBT,
    /* 5 */     TNM_TRACE_TUN_USER,
    /* 6 */     TNM_TRACE_FROM_SWCH,
    /* 7 */     TNM_TRACE_TO_SWCH,
    /* 8 */     TNM_TRACE_NODE_LIST,
    /* 9 */     TNM_TRACE_BUCKETMAP,
   /* 10 */     TNM_TRACE_IMC,
   /* 11 */     TNM_TRACE_TMR,
   /* 12 */     TNM_TRACE_GSM,
   /* 13 */     TNM_TRACE_CLI,
   /* 14 */     TNM_TRACE_MCAST,
   /* 15 */     TNM_TRACE_MAX,
   /* 16 */     TNM_TRACE_NOT_USED_16   ,
   /* 17 */     TNM_TRACE_NOT_USED_17   ,
   /* 18 */     TNM_TRACE_NOT_USED_18   ,
   /* 19 */     TNM_TRACE_NOT_USED_19   ,
   /* 20 */     TNM_TRACE_NOT_USED_20   ,
   /* 21 */     TNM_TRACE_NOT_USED_21   ,
   /* 22 */     TNM_TRACE_NOT_USED_22   ,
   /* 23 */     TNM_TRACE_NOT_USED_23   ,
   /* 24 */     TNM_TRACE_NOT_USED_24   ,
   /* 25 */     TNM_TRACE_NOT_USED_25   ,
   /* 26 */     TNM_TRACE_NOT_USED_26   ,
   /* 27 */     TNM_TRACE_SYSLOG   ,      /* Used for syslog for all components */
   /* 28 */     TNM_TRACE_NOT_USED_28   ,
   /* 29 */     TNM_TRACE_NOT_USED_29   ,
   /* 30 */     TNM_TRACE_NOT_USED_30   ,
   /* 31 */     TNM_TRACE_NOT_USED_31   ,
} trace_tnm_subcomp_id_t;

/* Wired Visibility (UV)  Subcomponent */
typedef enum _trace_wv_subcomp_id {
    /* 0 */     UV_TRACE_PROCESS,
    /* 1 */     UV_TRACE_PAPI,
    /* 2 */     UV_TRACE_GSM,
    /* 3 */     UV_TRACE_NOT_USED_3,
    /* 4 */     UV_TRACE_NOT_USED_4,
    /* 5 */     UV_TRACE_NOT_USED_5,
    /* 6 */     UV_TRACE_NOT_USED_6,
    /* 7 */     UV_TRACE_NOT_USED_7,
    /* 8 */     UV_TRACE_NOT_USED_8,
    /* 9 */     UV_TRACE_NOT_USED_9,
   /* 10 */     UV_TRACE_NOT_USED_10,
   /* 11 */     UV_TRACE_NOT_USED_11,
   /* 12 */     UV_TRACE_NOT_USED_12,
   /* 13 */     UV_TRACE_NOT_USED_13,
   /* 14 */     UV_TRACE_NOT_USED_14,
   /* 15 */     UV_TRACE_NOT_USED_15,
   /* 16 */     UV_TRACE_NOT_USED_16,
   /* 17 */     UV_TRACE_NOT_USED_17,
   /* 18 */     UV_TRACE_NOT_USED_18,
   /* 19 */     UV_TRACE_NOT_USED_19,
   /* 20 */     UV_TRACE_NOT_USED_20,
   /* 21 */     UV_TRACE_NOT_USED_21,
   /* 22 */     UV_TRACE_NOT_USED_22,
   /* 23 */     UV_TRACE_NOT_USED_23,
   /* 24 */     UV_TRACE_NOT_USED_24,
   /* 25 */     UV_TRACE_NOT_USED_25,
   /* 26 */     UV_TRACE_NOT_USED_26,
   /* 27 */     UV_TRACE_SYSLOG   ,      /* Used for syslog for all components */
   /* 28 */     UV_TRACE_NOT_USED_28,
   /* 29 */     UV_TRACE_NOT_USED_29,
   /* 30 */     UV_TRACE_NOT_USED_30,
   /* 31 */     UV_TRACE_NOT_USED_31,
} trace_wv_subcomp_id_t;

#endif /* ARUBA_TRACE_COMP_H */
