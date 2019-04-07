#ifndef STDDEFS_H
#define STDDEFS_H

#ifdef WIN32
/* need to disable C4768 for stl */
#pragma warning (disable : 4786)
#endif /* WIN32 */

#include <asm/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RESET          1
#define GET_HEAD       2
#define GET_TAIL       3
#define GET_NEXT       4
#define GET_PREV       5
#define SKIP_100       6
#define SKIP_NUM       7

#ifndef boolean__def
#ifndef WIN32
typedef char          boolean;
#else /* WIN32 */
typedef unsigned char boolean;
#endif /* WIN32 */
#define boolean__def
#endif
typedef void *        Opaque;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#ifndef WIN32
#  ifndef ERROR
#    define ERROR (-1)
#  endif

#  ifndef OK
#    define OK    (0)
#  endif
#else /* WIN32 */
#  ifndef OK
#    define OK    NOERROR
#  endif
#endif /* WIN32 */

#  ifdef WIN32

#    include <stdio.h>
#    include <getopt.h>


#    define snprintf      _snprintf
#    define vsnprintf     _vsnprintf
#    define strcasecmp    _stricmp
#    define strncasecmp   _strnicmp
char *strsep(char **, const char *);

typedef size_t socklen_t;


#    define bzero(_b_,_sz_)     memset((_b_), 0, (_sz_))


#    define LOCATION_CODE_DEPRECATED
#    define __attribute__(_x_)

/*
 * missing search.h
 */
#    define __const     const
#    ifndef __COMPAR_FN_T
#      define __COMPAR_FN_T
typedef int (*__compar_fn_t) (__const void *, __const void *);
#    endif /* __COMPAR_FN_T */

typedef enum
{
  preorder,
  postorder,
  endorder,
  leaf
}
VISIT;

#ifndef __ACTION_FN_T
# define __ACTION_FN_T
typedef void (*__action_fn_t) (__const void *__nodep, VISIT __value,
			       int __level);
#endif


/*
 * gettimeofday()
 */

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

int     gettimeofday(struct timeval *tv, struct timezone *tz);


/*
 * open, close, read, write, lseek
 */
#  include <io.h>

#  define O_SYNC        0


/*
 * MSG_DONTWAIT flag is not supported in windows.  Need to use ioctlsocket().
 */
#  define MSG_DONTWAIT  0

#  define Set_Socket_NBIO(_s_)                  \
   do {                                         \
       u_long m = 1;                            \
       DWORD e = WSAGetLastError();             \
       ioctlsocket((_s_), FIONBIO, &m);         \
       WSASetLastError(e);                      \
   } while (0)
#  define Set_Socket_BIO(_s_)                                           \
   do {                                         \
       u_long m = 0;                            \
       DWORD e = WSAGetLastError();             \
       ioctlsocket((_s_), FIONBIO, &m);         \
       WSASetLastError(e);                      \
   } while (0)


/*
 * missing errno, probably need to use ETIMEDOUT instead.
 */
#  define ETIME         62
#  define EMSGSIZE      90
#  define ETIMEDOUT     WSAETIMEDOUT


/*
 * strerror() in win32 doesn't take care of socket errors
 */

#  ifndef NDEBUG
#    define strerror(_en_)        tp_strerror(_en_)
#  endif /* NDEBUG */

/* meant to replace strerror() */
#    ifndef TP_STRERROR
#      define TP_STRERROR       1
extern char *tp_strerror(int en);
#    endif /* TP_STRERROR */


/*
 * sleep
 */
#  define sleep(_sec_)  Sleep((_sec_) * 1000)


/*
 * no localtime_r, gmtime_r
 */
#  define localtime_r(_a_,_b_)  memcpy((_b_), localtime(_a_),sizeof(struct tm))
#  define gmtime_r(_a_,_b_)     memcpy((_b_), gmtime(_a_), sizeof(struct tm))


/* for tm_gmtoff */
int     get_gmtoff(void);


/*
 * timeradd, timersub, etc. from sys/time.h
 */

# define timeradd(a, b, result)                                               \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                          \
    if ((result)->tv_usec >= 1000000)                                         \
      {                                                                       \
        ++(result)->tv_sec;                                                   \
        (result)->tv_usec -= 1000000;                                         \
      }                                                                       \
  } while (0)
# define timersub(a, b, result)                                               \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                          \
    if ((result)->tv_usec < 0) {                                              \
      --(result)->tv_sec;                                                     \
      (result)->tv_usec += 1000000;                                           \
    }                                                                         \
  } while (0)



/*
 * __FUNCTION__ macro is not supported in MSVC++ 6.0.
 */
#    ifndef __FUNCTION__
#      define __FUNCTION__      __FILE__
#      define __func__          __FUNCTION__
#    endif /* __FUNCTION__ */


/* no __BEGIN_DECLS / __END_DECLS */
#    ifndef __cplusplus
#      define __BEGIN_DECLS
#      define __END_DECLS
#    else  /* __cplusplus */
#      define __BEGIN_DECLS     extern "C" {
#      define __END_DECLS       }
#    endif /* __cplusplus */


/* inline is only for C++ */
#    ifndef __cplusplus
#      define inline
#      define __inline__
#    endif /* __cplusplus */


/*
 * access modes
 */
#    define F_OK        0


/*
 * random(), srandom()
 */
#    define random()            rand()
#    define srandom(_v_)        srand((_v_))


/* inet_aton(char *cp, struct in_addr *in) */
#    define inet_aton(_a_,_in_)                                              \
    ((_in_)->s_addr = inet_addr(_a_), ((_in_)->s_addr == INADDR_NONE && strcmp((_a_), "255.255.255.255") != 0) ? 0 : 1)


/* PATH_MAX */
#    ifndef PATH_MAX
#      define PATH_MAX  MAX_PATH
#    endif /* PATH_MAX */


/* MAXHOSTNAMELEN */
#    ifndef MAXHOSTNAMELEN
#      define MAXHOSTNAMELEN	64
#    endif /* MAXHOSTNAMELEN */


/* registry related functions */
char    *get_installed_dir(void);
char	*get_mmsbase_dir(void);
char    *get_var_dir(void);
char    *get_conf_dir(void);
char    *get_tmp_dir(void);
char    *get_java_home(void);


/* core file name generation */
int     get_dump_file_name(char *dump_name, int dump_name_len,
                           char *module_name, int pid);


/* check heap integrity, returns boolean */
int     heapcheck(int do_dump);


/* IIS related functions */
int     get_mms_webserver(char *name, int name_len);
int     set_mms_webserver(char *name);
int     is_webserver_iis(void);
int     is_mms_installed_on_iis(void);


/* initialize directory names, core dump and winsock */
int     init_if_windows(void);


#  else /* !WIN32 */

#    define O_BINARY    0

#    define Set_Socket_NBIO(_s_)
#    define Set_Socket_BIO(_s_)

#    define get_installed_dir()         "/opt"
#    define get_mmsbase_dir()           "/opt/" __OEM__
#    define get_var_dir()               "/var"
#    define get_conf_dir()              "/opt/" __OEM__ "/conf"
#    define get_tmp_dir()               "/tmp"
#    define get_java_home()             "/usr"

#    define init_if_windows()


#  endif /* !WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* STDDEFS_H */
