/* vim:set ts=4 sw=4: */
#ifndef ARUBALOG
#define ARUBALOG

#ifdef __cplusplus
extern "C" {
#endif

#if defined(APSIM_LITE)
#define __va_list__
#endif /* defined(APSIM_LITE) */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <util/hash.h>
#include <stdarg.h>
#include "../syslogutils/generic_systemlogs.h"
#ifdef __FAT_AP__
#include "../syslogutils/fips_logs.h"
#endif
#include "aruba_trace.h"
#include "include/msg_catalog.h"

#if defined(__LOG_AUDIT__) && !defined(WIN32)
#define __ATTR_DEP__ __attribute__((deprecated))
#else
#define __ATTR_DEP__
#endif

#define LOG_ENABLED(lgl) \
	do {\
		if(lgl > lgClientInfo.lglvl && lgl > lgClientInfo.cliLglvl) \
			return; \
		if(lglvlNames[lgl].c_name == NULL)\
			return; \
	}while(0)

//Will refine this after cleanup.
#define LOG_DEBUG_ENABLED(dval) \
do {\
	if (!(lgClientInfo.dEnable[dval-1].enable))\
		return;\
}while(0)

boolean IS_LOG_DEBUG_ENABLED(int dval);

#define MAX_LOG_MESSAGE_LEN 1024
#define MAX_FAC_NAME_LEN 32
#define MAX_DEBUG_NAME_LEN 16

/*
 * 
 */ 
boolean is_aruba_log(int lgl);
boolean is_aruba_log_facility(int fac, int lgl);
boolean is_aruba_debug_log(int dval);
boolean is_user_debug_log(int lgl, char* mac, uint32_t ip, char* name);
boolean is_debugging_enabled_for_mac (char * mac);
boolean is_user_debugging_enabled (void);


extern sHash *macHT, *ipHT, *nameHT;
extern boolean _isdefaultLogLevel;

#ifndef WIN32 /* no variable argument macro in VC6.0 */
#ifdef EMS_PLATFORM
#define arubaLog(lgl, fmt, args...)\
do{\
    if(!((lgl > lgClientInfo.lglvl) && (lgl > lgClientInfo.cliLglvl))) \
        _arubaLog(lgl, fmt, ## args);                                  \
}while(0)		

#if !defined(APSIM_LITE)
#define varubaLog(lgl, fmt, args) \
do{\
    if(!((lgl > lgClientInfo.lglvl) && (lgl > lgClientInfo.cliLglvl))) \
        _varubaLog(lgl, fmt, args);                                  \
}while(0)		
#else /* !defined(APSIM_LITE) */
#define varubaLog(lgl, fmt, args) \
do{\
    if(!((lgl > lgClientInfo.lglvl) && (lgl > lgClientInfo.cliLglvl))) \
        _arubaLog(lgl, fmt, ## args);                                  \
}while(0)		
#endif /* !defined(APSIM_LITE) */


#define arubaLogFacility(fac, lgl, fmt, args...) \
do{\
    if(is_aruba_log_facility(fac, lgl))\
        _arubaLogFacility(fac, lgl, fmt, ## args);\
}while(0)		

#define arubaLogToFacility(fac, lgl, fmt, args...) \
do{\
    if(is_aruba_log_facility(fac, lgl))\
        _arubaLogToFacility(fac, lgl, fmt, ## args);\
}while(0)		


#define arubaDebugLog(dval, lgl, fmt, args...)\
do {\
    if(!((LOG_DEBUG > lgClientInfo.lglvl) && (lgl > lgClientInfo.cliLglvl))) \
        _arubaLog(LOG_DEBUG, fmt, ## args);                                  \
}while(0)

#define arubaUserLog(code, lgl, mac, ip, name, fmt, args...)     \
do {\
    if ((!((lgl > lgClientInfo.lglvl) && (lgl > lgClientInfo.cliLglvl))) || \
        ((mac && HashIsMacPresent(macHT, (void *) mac)) ||   \
         (ip && ((int) HashFind(ipHT, (void *) ip) != 0)) || \
        (name && HashIsStringPresent(nameHT, name)))) \
            _arubaUserLog(code, lgl, mac, ip, name, fmt, ## args);       \
}while(0)
#else //Switch and AP Platform.
#define arubaLog(lgl, fmt, args...)\
do{\
}while(0)		

#define varubaLog(lgl, fmt, args) \
do{\
}while(0)		

#define arubaLogFacility(fac, lgl, fmt, args...) \
do{\
}while(0)		

#define arubaLogToFacility(fac, lgl, fmt, args...) \
do{\
}while(0)		


#define arubaDebugLog(dval, lgl, fmt, args...)\
do {\
}while(0)

#define arubaUserLog(code, lgl, mac, ip, name, fmt, args...)     \
do {\
}while(0)
#endif

#else /* WIN32 */

#define arubaLog                _arubaLog
#define varubaLog               _varubaLog
#define arubaLogFacility        _arubaLogFacility
#define arubaLogToFacility      _arubaLogToFacility
extern void arubaDebugLog(int dval, int lglvl, char *fmt, ...);
#define arubaUserLog            _arubaUserLog

#endif /* WIN32 */

typedef struct subCatEntry_s{
	int sId;
	int sLvl;
}subCatEntry_t;

typedef struct facEntry_s{
	int fac;
	int lglvl;
	int isall;
	int allLglvl;
	sHash* subCat;
}facEntry_t;

typedef struct dKey_s{
	int lvl;
	int sId;
}dKey_t;

typedef struct dFacEntry_s{
	int fac;
	int hId;
	sKeyTypes_e sType;
	sHash* keys;
}dFacEntry_t;

extern int newFacilities[];
extern int numNewFacilities;

extern boolean isDbgLvlEnabled(int fac);


#define MAX_SEARCH_HASHS	100
typedef struct loggerClientInfo_s {
	int			fac;   // Facility Id
	int			deflglvl;	 // Default Log level.
	int			lglvl;	  // Log level this facility is configured to support.
	int 		cliLglvl;	//This Log level is used to determine whether to 
							// send data to CLI.
	int 		cliSock;
	struct sockaddr_in	cliServerAddr;
	char 		fName[MAX_FAC_NAME_LEN]; // Facility  Name, Sent out in the log 
	sHash*		facList;
	sHash*		debugList;
	sHash*		kHashId[MAX_SEARCH_HASHS];
#if defined(AP_PLATFORM)
	char ap_identity[128];
	int ap_ident_comp;
    time_t apip_last_modified;
    time_t apname_last_modified;
#endif
									// message.
}loggerClientInfo_t;

#define ARUBA_SYSTEM_LOG_BUF_SIZE 1024
extern char __aruba_system_log_buf__[ARUBA_SYSTEM_LOG_BUF_SIZE];


#ifndef WIN32 /* no variable argument macro in VC6.0 */


#ifdef AP_PLATFORM
#define arubaNewLogWithDestIP(__fac__, __lgl__, __id__, __log_dest__, __fmt__, args...) \
do{\
	facEntry_t* _fac;\
    if(_isdefaultLogLevel){\
        if(__lgl__ <= LOG_WARNING)\
            _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, __log_dest__, __fmt__, ## args); \
    }else{\
        if(lgClientInfo.facList){\
        _fac = (facEntry_t *)HashFind(lgClientInfo.facList, (void *)__fac__);\
        if(_fac && (__lgl__ <= _fac->lglvl))\
            _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, __log_dest__, __fmt__, ## args); \
        }\
    }\
}while(0)
#endif

#ifdef AP_PLATFORM
#define arubaNewLogWithTagWithDestIP(__fac__, __lgl__, __id__, __sTag__, __log_dest__, __fmt__, args...) \
do{\
	facEntry_t* _fac;\
	subCatEntry_t* _sfac;\
	if(__lgl__ < LOG_WARNING){\
            _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, __log_dest__, __fmt__, ## args); \
	}\
	else if(lgClientInfo.facList){\
		_fac = (facEntry_t *)HashFind(lgClientInfo.facList, (void *)__fac__);\
		if(_fac && _fac->isall){\
			if((__lgl__ <= _fac->allLglvl))\
                            _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, __log_dest__, __fmt__, ## args); \
		}else{\
			_sfac = (subCatEntry_t *)HashFind(_fac->subCat, (void *)__sTag__);\
			if((_sfac != NULL) && (__lgl__ <= _sfac->sLvl)){\
                            _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, __log_dest__,  __fmt__, ## args); \
			}\
		}\
	}\
}while(0)
#endif

#ifdef __FAT_AP__
#define arubaNewLog(__fac__, __lgl__, __id__, __fmt__, args...)\
    _arubaNewLogWithFltr(__fac__, __lgl__, __id__, __fmt__, ## args)

#ifdef __FAT_AP__
#define arubaNewLogWithTag(__fac__, __lgl__, __id__, __sTag__, __fmt__, args...)\
    _arubaNewLogWithFltr(__fac__, __lgl__, __id__, __fmt__, ## args)
#else
#define arubaNewLogWithTag(__fac__, __lgl__, __id__, __sTag__, __fmt__, args...)\
    _arubaNewLogTagWithFltr(__fac__, __lgl__, __id__, __sTag__, __fmt__, ## args)
#endif

#define arubaNewMacDebugLog(__log_dest__, __fac__, __lgl__, __id__, _hId_, _mac_, __fmt__, args...)\
    _arubaNewMacDebugWithFltr(__fac__, __lgl__, __id__, _hId_, (char*)_mac_, __fmt__, ## args)
#else  /* FAT_AP */

#define arubaNewLog(__fac__, __lgl__, __id__, __fmt__, args...)\
do{\
	facEntry_t* _fac;\
    if(_isdefaultLogLevel){\
        if(__lgl__ <= LOG_WARNING)\
            _arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args);\
    }else{\
        if(lgClientInfo.facList){\
        _fac = (facEntry_t *)HashFind(lgClientInfo.facList, (void *)__fac__);\
        if(_fac && (__lgl__ <= _fac->lglvl))\
            _arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args);\
        }\
    }\
}while(0)

#define arubaNewLogWithTag(__fac__, __lgl__, __id__, __sTag__, __fmt__, args...)\
do{\
	facEntry_t* _fac;\
	subCatEntry_t* _sfac;\
	if(__lgl__ < LOG_WARNING){\
		_arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args);\
	}\
	else if(lgClientInfo.facList){\
		_fac = (facEntry_t *)HashFind(lgClientInfo.facList, (void *)__fac__);\
		if(_fac && _fac->isall){\
			if((__lgl__ <= _fac->allLglvl))\
				_arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args);\
		}else{\
			_sfac = (subCatEntry_t *)HashFind(_fac->subCat, (void *)__sTag__);\
			if((_sfac != NULL) && (__lgl__ <= _sfac->sLvl)){\
				_arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args);\
			}\
		}\
	}\
}while(0)

#ifdef AP_PLATFORM
#define arubaNewMacDebugLog(__log_dest__, __fac__, __lgl__, __id__, _hId_, _mac_, __fmt__, args...) \
do{\
	dKey_t* dkey;                                                   \
        ip_addr_t local_ipa;                                                  \
	if((dkey = (dKey_t *)HashMacFind(lgClientInfo.kHashId[_hId_], (const __u8 *)_mac_)) \
           != NULL) {                                                   \
            if(__lgl__ <= dkey->lvl) {                                  \
                if (__log_dest__) {                                     \
                    local_ipa = *__log_dest__;                                    \
                    _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, local_ipa, __fmt__, ## args); \
                } else                                                  \
                    _arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args); \
            }                                                           \
	}                                                               \
}while(0)
#else
#define arubaNewMacDebugLog(__log_dest__, __fac__, __lgl__, __id__, _hId_, _mac_, __fmt__, args...) \
do{\
	dKey_t* dkey;                                                   \
	if((dkey = (dKey_t *)HashMacFind(lgClientInfo.kHashId[_hId_], (const __u8 *)_mac_)) \
           != NULL) {                                                   \
            if(__lgl__ <= dkey->lvl) {                                  \
                _arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args); \
            }                                                           \
	}                                                               \
}while(0)
#endif

#endif /* __FAT_AP__ */

#define arubaNewMacDebugLogWithTag(__log_dest__, __fac__, __lgl__, __id__, __sTag__, _hId_, _mac_, __fmt__, args...)\
do{\
        dKey_t* dkey;                                                   \
        ip_addr_t local_ipa;                                                  \
	if ((dkey = HashMacFind(lgClientInfo.kHashId[_hId_], (void *)_mac_)) != NULL) { \
		if(((dkey->sId == 0) || (dkey->sId == sTag)) &&         \
                   (__lgl__ <= dkey->lvl)) {                            \
                        if (__log_dest__) {                             \
                                local_ipa = *__log_dest__;                        \
                                _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, local_ipa, __fmt__, ## args); \
                        } else                                          \
                                _arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args); \
		}                                                       \
	}                                                               \
}while(0)


#define arubaNewStrDebugLog(__log_dest__, __fac__, __lgl__, __id__, _hId_, _str_, __fmt__, args...)\
do{\
	dKey_t* dkey;                                                   \
        ip_addr_t local_ipa;                                                  \
	if((dkey = HashStringFind(lgClientInfo.kHashId[_hId_], (void *)_str_)) != NULL) {\
		if(__lgl__ <= dkey->lvl) {                              \
                        if (__log_dest__) {                             \
                                local_ipa = *__log_dest__;                        \
                                _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, local_ipa, __fmt__, ## args); \
                        } else                                          \
                                _arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args); \
		}                                                       \
	}                                                               \
}while(0)

#define arubaNewStrDebugLogWithTag(__log_dest__, __fac__, __lgl__, __id__, __sTag__, _hId_, _str_, __fmt__, args...)\
do{\
	dKey_t* dkey;                                                   \
        ip_addr_t local_ipa;                                                  \
	if((dkey = HashStringFind(lgClientInfo.kHashId[_hId_], (void *)_str_)) != NULL) { \
		if(((dkey->sId == 0) || (dkey->sId == sTag)) &&         \
                   (__lgl__ <= dkey->lvl)) {                            \
                        if (__log_dest__) {                             \
                                local_ipa = *__log_dest__;                        \
                                _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, local_ipa, __fmt__, ## args); \
                        } else                                          \
                                _arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args); \
		}                                                       \
	}                                                               \
}while(0)


#define arubaNewIntDebugLog(__log_dest__, __fac__, __lgl__, __id__, _hId_, _Id_, __fmt__, args...)\
do{\
	dKey_t* dkey;                                                   \
        ip_addr_t local_ipa;                                                  \
	if((dkey = HashFind(lgClientInfo.kHashId[_hId_], (void *)_Id_)) != NULL) { \
		if(__lgl__ <= dkey->lvl) {                              \
                        if (__log_dest__) {                             \
                                local_ipa = *__log_dest__;                        \
                                _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, local_ipa, __fmt__, ## args); \
                        } else                                          \
                                _arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args); \
		}                                                       \
	}                                                               \
}while(0)

#define arubaNewIntDebugLogWithTag(__log_dest__, __fac__, __lgl__, __id__, __sTag__, _hId_, _Id_, __fmt__, args...)\
do{\
	dKey_t* dkey;                                                   \
        ip_addr_t local_ipa;                                                  \
	if((dkey = HashFind(lgClientInfo.kHashId[_hId_], (void *)_Id_)) != NULL) { \
		if(((dkey->sId == 0) || (dkey->sId == sTag)) &&         \
                   (__lgl__ <= dkey->lvl)) {                            \
                        if (__log_dest__) {                             \
                                local_ipa = *__log_dest__;                        \
                                _arubaNewLogWithDestIP(__fac__, __lgl__, __id__, local_ipa, __fmt__, ## args); \
                        } else                                          \
                                _arubaNewLog(__fac__, __lgl__, __id__, __fmt__, ## args); \
		}                                                       \
	}                                                               \
}while(0)

#define arubaGenSystemLog(__lgl__, __fmt__, args...)\
do{\
	facEntry_t* __fac;\
    int _logit = 0; \
    if(_isdefaultLogLevel){\
        if(__lgl__ <= LOG_WARNING)\
            _logit = 1 ; \
    }else{ \
	  if(lgClientInfo.facList){\
        __fac = (facEntry_t *)HashFind(lgClientInfo.facList,    \
                                       (void *)LOG_SYSTEM);     \
		if(__fac && (__lgl__ <= __fac->lglvl))                  \
            _logit = 1 ; \
      }\
    }\
    if(_logit) { \
        snprintf(__aruba_system_log_buf__,                  \
                 sizeof(__aruba_system_log_buf__),          \
                 __fmt__, ## args);                         \
        if(__lgl__ == LOG_DEBUG)\
            SYSTEM_MSG_INTERNAL_SYSTEM_GEN_DEBUG(           \
                __aruba_system_log_buf__);                  \
        else if(__lgl__ == LOG_INFO)\
            SYSTEM_MSG_INTERNAL_SYSTEM_GEN_INFO(            \
                __aruba_system_log_buf__);                  \
        else if(__lgl__ == LOG_WARNING)\
            SYSTEM_MSG_INTERNAL_SYSTEM_GEN_WARNING(         \
                __aruba_system_log_buf__);                  \
        else\
            SYSTEM_MSG_INTERNAL_SYSTEM_GEN_ERROR(           \
                __aruba_system_log_buf__);                  \
	}\
}while(0)

/* For internal aruba log and aruba trace framework logs only.
 * This function applies the configured log filter settings but
 * then calls syslog directly instead of sending the log through
 * the logging framework.  The logging framework sends the logs
 * through aruba trace, so if we send internal aruba trace logs
 * through the logging framework we risk an infinite loop. */
#define arubaGenInternalSystemSyslog(__lgl__, __fmt__, args...)         \
do{                                                                     \
    facEntry_t* __fac;                                                  \
    int _logit = 0;                                                     \
    if (_isdefaultLogLevel) {                                           \
        if (__lgl__ <= LOG_WARNING)                                     \
            _logit = 1;                                                 \
    } else {                                                            \
        if (lgClientInfo.facList) {                                     \
            __fac = (facEntry_t *)HashFind(lgClientInfo.facList,        \
                                       (void *)LOG_SYSTEM);             \
            if (__fac && (__lgl__ <= __fac->lglvl))                     \
                _logit = 1;                                             \
        }                                                               \
    }                                                                   \
    if (_logit) {                                                       \
        syslog(LOG_SYSTEM | __lgl__, __fmt__, ## args);                 \
    }                                                                   \
}while(0)

#define arubaGenWebServerSystemLog(__lgl__, __fmt__, args...)\
do{\
    facEntry_t* _fac;\
    subCatEntry_t* _sfac;\
    if(lgClientInfo.facList){\
        _fac = (facEntry_t *)HashFind(lgClientInfo.facList,             \
                                      (void *)LOG_SYSTEM);              \
        if(_fac == NULL) { \
            arubaGenSystemLog(LOG_ERR,                                  \
                              "LOG_SYSTEM facility lookup failed");     \
        } else if(_fac->isall){\
            if((__lgl__ <= _fac->allLglvl)){\
                snprintf(__aruba_system_log_buf__,                      \
                         sizeof(__aruba_system_log_buf__),              \
                         __fmt__, ## args);                             \
                if(__lgl__ == LOG_DEBUG){\
                    SYSTEM_MSG_WEBSERVER_INTERNAL_SYSTEM_GEN_DEBUG(     \
                        __aruba_system_log_buf__);                      \
                }else if(__lgl__ == LOG_INFO){\
                    SYSTEM_MSG_WEBSERVER_INTERNAL_SYSTEM_GEN_INFO(      \
                        __aruba_system_log_buf__);                      \
                } else if(__lgl__ == LOG_NOTICE) {\
                    SYSTEM_MSG_WEBSERVER_INTERNAL_SYSTEM_GEN_NOTICE(    \
                        __aruba_system_log_buf__);                      \
                } else if(__lgl__ == LOG_WARNING) {\
                    SYSTEM_MSG_WEBSERVER_INTERNAL_SYSTEM_GEN_WARNING(   \
                        __aruba_system_log_buf__);                      \
                }else {\
                    SYSTEM_MSG_WEBSERVER_INTERNAL_SYSTEM_GEN_ERROR(     \
                        __aruba_system_log_buf__);                      \
                }\
            }\
        }else{\
            _sfac = (subCatEntry_t *)HashFind(                          \
                _fac->subCat, (void *)SUBCAT_SYSTEM_WEBSERVER);          \
            if(_sfac != NULL && __lgl__ <= _sfac->sLvl){\
                snprintf(__aruba_system_log_buf__,                      \
                         sizeof(__aruba_system_log_buf__),              \
                         __fmt__, ## args);                             \
                if(__lgl__ == LOG_DEBUG) {\
                    SYSTEM_MSG_WEBSERVER_INTERNAL_SYSTEM_GEN_DEBUG(     \
                        __aruba_system_log_buf__);                      \
                } else if(__lgl__ == LOG_INFO) {\
                    SYSTEM_MSG_WEBSERVER_INTERNAL_SYSTEM_GEN_INFO(      \
                        __aruba_system_log_buf__);                      \
                } else if(__lgl__ == LOG_NOTICE) {\
                    SYSTEM_MSG_WEBSERVER_INTERNAL_SYSTEM_GEN_NOTICE(    \
                        __aruba_system_log_buf__);                      \
                } else if(__lgl__ == LOG_WARNING) {\
                    SYSTEM_MSG_WEBSERVER_INTERNAL_SYSTEM_GEN_WARNING(   \
                        __aruba_system_log_buf__);                      \
                } else {\
                    SYSTEM_MSG_WEBSERVER_INTERNAL_SYSTEM_GEN_ERROR(     \
                        __aruba_system_log_buf__);                      \
                }\
            }\
        }\
    } \
}while(0)

#define arubaGenRadvdNetworkLog(__lgl__, __fmt__, args...)\
do{\
    snprintf(__aruba_system_log_buf__,                      \
            sizeof(__aruba_system_log_buf__),              \
            __fmt__, ## args);                             \
    if(__lgl__ <= LOG_ERR) {\
        NETWORK_MSG_GENERIC_NETWORK_ERR(     \
            __aruba_system_log_buf__);                      \
    } else {\
        NETWORK_MSG_GENERIC_NETWORK_DEBUG(     \
            __aruba_system_log_buf__);                      \
    }\
}while(0)

#define arubaGenOspfNetworkLog(__lgl__, __fmt__, args...)\
do{\
    snprintf(__aruba_system_log_buf__,                      \
            sizeof(__aruba_system_log_buf__),              \
            __fmt__, ## args);                             \
    if(__lgl__ <= LOG_ERR) {\
        NETWORK_MSG_GENERIC_NETWORK_ERR(     \
            __aruba_system_log_buf__);                      \
    } else {\
        NETWORK_MSG_GENERIC_NETWORK_DEBUG(     \
            __aruba_system_log_buf__);                      \
    }\
}while(0)

#define arubaGenDhcpdNetworkLog(__lgl__, __fmt__, args...)\
do{\
    snprintf(__aruba_system_log_buf__,                     \
                sizeof(__aruba_system_log_buf__),              \
                __fmt__, ## args);                             \
        NETWORK_MSG_GENERIC_NETWORK_DEBUG(                     \
                __aruba_system_log_buf__);                     \
    }while(0)

#define arubaGenCaptivePortalSecurityLog(__lgl__, __fmt__, args...)\
do{\
    facEntry_t* _fac;\
    subCatEntry_t* _sfac;\
    if(lgClientInfo.facList){\
        _fac = (facEntry_t *)HashFind(lgClientInfo.facList,         \
                                      (void *)LOG_SYSTEM);          \
        if(_fac == NULL) { \
            arubaGenSystemLog(LOG_ERR,                              \
                              "LOG_SYSTEM facility lookup failed"); \
        } else if(_fac->isall){\
            if((__lgl__ <= _fac->allLglvl)){\
                snprintf(__aruba_system_log_buf__,                  \
                         sizeof(__aruba_system_log_buf__),          \
                         __fmt__, ## args);                         \
                if(__lgl__ == LOG_DEBUG){\
                    SYSTEM_MSG_CAPTIVE_PORTAL_GEN_DEBUG(            \
                        __aruba_system_log_buf__);                  \
                }else if(__lgl__ == LOG_INFO){\
                    SYSTEM_MSG_CAPTIVE_PORTAL_GEN_INFO(             \
                        __aruba_system_log_buf__);                  \
                } else if(__lgl__ == LOG_NOTICE) {\
                    SYSTEM_MSG_CAPTIVE_PORTAL_GEN_NOTICE(           \
                        __aruba_system_log_buf__);                  \
                } else if(__lgl__ == LOG_WARNING) {\
                    SYSTEM_MSG_CAPTIVE_PORTAL_GEN_WARNING(          \
                        __aruba_system_log_buf__);                  \
                }else {\
                    SYSTEM_MSG_CAPTIVE_PORTAL_GEN_ERROR(            \
                        __aruba_system_log_buf__);                  \
                }\
            }\
        }else{\
            _sfac = (subCatEntry_t *)HashFind(                      \
                _fac->subCat, (void *)SUBCAT_SYSTEM_WEBSERVER);     \
            if(_sfac != NULL && __lgl__ <= _sfac->sLvl){\
                snprintf(__aruba_system_log_buf__,                  \
                         sizeof(__aruba_system_log_buf__),          \
                         __fmt__, ## args);                         \
                if(__lgl__ == LOG_DEBUG) {\
                    SYSTEM_MSG_CAPTIVE_PORTAL_GEN_DEBUG(            \
                        __aruba_system_log_buf__);                  \
                } else if(__lgl__ == LOG_INFO) {\
                    SYSTEM_MSG_CAPTIVE_PORTAL_GEN_INFO(             \
                        __aruba_system_log_buf__);                  \
                } else if(__lgl__ == LOG_NOTICE) {\
                    SYSTEM_MSG_CAPTIVE_PORTAL_GEN_NOTICE(           \
                        __aruba_system_log_buf__);                  \
                } else if(__lgl__ == LOG_WARNING) {\
                    SYSTEM_MSG_CAPTIVE_PORTAL_GEN_WARNING(          \
                        __aruba_system_log_buf__);                  \
                } else {\
                    SYSTEM_MSG_CAPTIVE_PORTAL_GEN_ERROR(            \
                        __aruba_system_log_buf__);                  \
                }\
            }\
        }\
    } \
}while(0)

#else /* WIN32 */

#define arubaNewLog                     _arubaNewLog

extern void arubaNewLogWithTag(int fac, int lglvl, int id, char *sTag,
                               char *fmt, ...);
extern void arubaNewMacDebugLog(int fac, int lglvl, int id, int hid, char *mac,
                                char *fmt, ...);
extern void arugaNewMacDebugLogWithTag(int fac, int lglvl, int id, int sTag,
                                       int hid, unsigned int mac,
                                       char *fmt, ...);
extern void arubaNewStrDebugLog(int fac, int lglvl, int id, int hid, char *str,
                                char *fmt, ...);
extern void arubaNewStrDebugLogWithTag(int fac, int lglvl, int id, int sTag,
                                       int hid, char *str, char *fmt, ...);
extern void arubaNewIntDebugLog(int fac, int lglvl, int id, int hid, int Id,
                                char *fmt, ...);
extern void arubaNewIntDebugLogWithTag(int fac, int lglvl, int id, int sTag,
                                       int hid, int Id, char *fmt, ...);
extern void arubaGenSystemLog(int lglvl, char *fmt, ...);
extern void arubaGenInternalSystemSyslog(int lglvl, char *fmt, ...);


#define arubaNewLogWithTag              _arubaNewLogWithTag
#define arubaNewMacDebugLog             _arubaNewMacDebugLog
#define arubaNewMacDebugLogWithTag      _arubaNewMacDebugLogWithTag
#define arubaNewStrDebugLog             _arubaNewStrDebugLog
#define arubaNewStrDebugLogWithTag      _arubaNewStrDebugLogWithTag
#define arubaNewIntDebugLog             _arubaNewIntDebugLog
#define arubaNewIntDebugLogWithTag      _arubaNewIntDebugLogWithTag
#define arubaGenSystemLog               _arubaGenSystemLog
#define arubaGenInternalSystemSyslog    _arubaGenInternalSystemSyslog

#endif /* WIN32 */


extern loggerClientInfo_t	lgClientInfo; // Is part of all the Client 
										  // executables.
extern char arubaLogBuf[];  //Used to send the logging message to syslog

//All the Tasks should call this function for Intializing the logging.
extern void arubaLogInit(int fac, char* facName, int lglvl);
//This will clean the memory allocated at the time of log initialization.
extern void arubaLogClean(void);
//Function to log the messages.

extern void _arubaNewLogWithDestIP(int fac, int lglvl, int id, ip_addr_t log_dest, char* fmt, ...) __attribute__ ((format (printf, 5, 6))) __ATTR_DEP__;
extern void _arubaNewLog(int fac, int lglvl, int id, char* fmt, ...) __attribute__ ((format (printf, 4, 5))) __ATTR_DEP__;
extern void _arubaLog(int lglvl, char* fmt, ...) __attribute__ ((format (printf, 2, 3))) __ATTR_DEP__;
#if !defined(APSIM_LITE)
extern void _varubaLog(int lglvl, char* fmt, va_list) __attribute__ ((format (printf, 2, 0))) __ATTR_DEP__;
#endif /* !defined(APSIM_LITE) */
extern void _arubaLogFacility(int fac, int lglvl, char* fmt, ...) __attribute__ ((format (printf, 3, 4))) __ATTR_DEP__;
extern void _arubaLogToFacility(int fac, int lglvl, char* fmt, ...) __attribute__ ((format (printf, 3, 4))) __ATTR_DEP__;
extern void arubaLogAllTerminals(char* fmt, ...) __attribute__ ((format (printf, 1, 2))) __ATTR_DEP__;
extern void _arubaUserLog(int code, int lglvl, unsigned char *mac, 
	unsigned int ip, char *name, char* fmt, ...) __attribute__ ((format (printf, 6, 7))) __ATTR_DEP__;

#ifdef __FAT_AP__
extern void _arubaNewLogWithFltr(int fac, int lglvl, int id, char* fmt, ...);
extern void _arubaNewLogTagWithFltr(int fac, int lglvl, int id, int sTag, char* fmt, ...);
extern void _arubaNewMacDebugWithFltr(int fac, int lglvl, int id, int hid, char *mac, char* fmt, ...);
#endif /* __FAT_AP__ */
//Function to log Debug messages.

#define ARUBA_LOG_CONFIG_MESSAGE_START	MC_LOG_CONFIG_MSG_START

#define LOG_CONFIG_TRACE_LEVEL	ARUBA_LOG_CONFIG_MESSAGE_START + 1
#define LOG_CONFIG_CLI_LOG_LEVEL	ARUBA_LOG_CONFIG_MESSAGE_START + 2
#define LOG_CONFIG_LOG_LEVEL	ARUBA_LOG_CONFIG_MESSAGE_START + 3
#define LOG_CONFIG_CLI_TRACE_LEVEL	ARUBA_LOG_CONFIG_MESSAGE_START + 4
#define LOG_CONFIG_DEBUG_VAL	ARUBA_LOG_CONFIG_MESSAGE_START + 5
#ifdef __FAT_AP__
#define LOG_CONFIG_GLOBAL_LOG_LEVEL	ARUBA_LOG_CONFIG_MESSAGE_START + 6
#endif

// Note: 100 MESSAGE CODEs are reserved for this module. 

//Message header for messages coming from CLI to the tasks. The Message type
//is defined above.
typedef struct logConfigHdr_s{
	int msgType;
} logConfigHdr_t;

//Message to change the trace level.
typedef struct traceChangeConfig_s{
	int defVal;
	int lglvl;
}traceChangeConfig_t;

typedef struct logLvlInfo_s{
	int facId;
	int level;
	int numSub;
	subCatEntry_t sInfo[MAX_SUB_CAT];
}logLvlInfo_t;

//Message to change the log level.
typedef struct lglvlChangeConfig_s{
	int numLogs;
	logLvlInfo_t lvlInfo[1];
}lglvlChangeConfig_t;

typedef struct debugVals_s{
	int len;
}debugVals_t;

typedef struct debugChInfo_s{
	int facId;
	int numDvals;
	debugVals_t dVals[1];
}debugChInfo_t;

//Message to change the log level.
typedef struct debugChangeConfig_s{
	int numFacs;
	logLvlInfo_t lvlInfo[1];
}debugChangeConfig_t;


//Functions to modify the Log info in the client.
extern void arubaTraceChangeLglvl(int lglvl, int defVal);
extern void arubaTraceChangeCliLglvl(int lglvl, int defVal);
extern void arubaChangeLglvl(lglvlChangeConfig_t* lglvChan);

// All the tasks should call the function to determine if it is a 
// log config message.
extern int isLogConfigMessage(char* data);

//This function will process the log config message and take appropriate steps.
extern void procLogConfigMessage(char* data);
#ifdef EMS_PLATFORM
extern void __ems_update_logs(int);
#endif

// These functions are used by the Cli functions to send log config messages 
// to the Tasks.
extern char* makeCliTraceChangeConfigMessage(Opaque pid, int lglvl, int defVal, int* buflen);
extern char* makeTraceChangeConfigMessage(Opaque pid, int lglvl, int defVal, int* buflen);
void arubaDebugUser(int start, char *name, unsigned int ip,
		unsigned char *mac);

int arubaLogLevel(void);

#ifdef __cplusplus
};
#endif

#endif /* ARUBALOG */
