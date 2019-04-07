#ifndef _HASH_H
#define _HASH_H

#include <stdio.h>
#include <asm/types.h>
#ifdef HAVE_IPV6_HASH 
#include "util/ipv6.h"
#endif
#include "util/ipv6_cmn.h"

#if defined (MVC_CLIENT_PLATFORM)

#define RESET          1
#define GET_HEAD       2
#define GET_TAIL       3
#define GET_NEXT       4
#define GET_PREV       5
#define SKIP_100       6
#define SKIP_NUM       7
typedef unsigned char boolean;

typedef void *        Opaque;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#else
#include "stddefs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
#define HASH_SMALL		23
#define HASH_MEDIUM		59
#define HASH_LARGE	    101

#define GET_VALUE      0
#define GET_KEY        1
#define GET_STRING     2
#define GET_MAC        3
#define GET_IPAF       4

typedef __u32         (*hash_fn_t)(void *key);
typedef boolean       (*key_equal_fn_t)(void *key1, void *key2);

typedef struct _sHashElem {
    void              * key;
    char              * str;
    __u8                macaddr[6];
    void              * value;
    struct _sHashElem * next;
#ifdef HAVE_IPV6_HASH 
    prefixIpv6_t      *ipv6Addr;
#endif
    ip_addr_t         *IpAfAddr;
} sHashElem;

typedef struct _sHashState {
    int         currIndex;
    sHashElem * currElem;
} sHashState;

typedef struct _sHash {
#ifdef DEBUG_HASH
    char        *owner;
#ifdef X86_TARGET_PLATFORM
    __u64     	 owner_line;
#else
    __u32     	 owner_line;
#endif
    int         *collisions;
#endif
    int          size;
    sHashElem ** table;
    sHashState	 current;
    int          numOfNodes;
    int         max_coll;
    hash_fn_t       hash_fn;
    key_equal_fn_t  key_equal_fn; 
    boolean        added_to_list; 
#ifdef EMS_PLATFORM 
    void       * rwLock;
#endif
#ifdef HAVE_IPV6_HASH 
    void       * hashSem;
    void       *(*semCreate)(int options, int count);
    int        (*semTake)(void *sem, int count);
    int        (*semGive)(void *sem);
    int        (*semDelete)(void *sem);
    //void       (*hashFunc)(void **arg);
#endif
} sHash;
#ifdef HAVE_IPV6_HASH 
typedef struct _sHashFunPtr{
    void       *(*semCreate)(int options, int count);
    int        (*semTake)(void *sem, int count);
    int        (*semGive)(void *sem);
    int        (*semDelete)(void *sem);
    //void       (*hashFunc)(void **arg);
} sHashFunPtr;
#endif

typedef struct {
} hash_stats_entry_t;

typedef struct {
    unsigned int    size;
    unsigned int    num_nodes;
    unsigned int    max_collisions;
    char            owner[64];
   	int owner_line;
} hash_stats_t;

extern boolean HashIsEmpty (sHash * hashTable);
extern void *  HashGet (sHash * hashTable, int msg, int type);
extern void *  HashGetR (sHash * hashTable, int msg, int type, sHashState *);
extern void *  HashGetFromBucket (sHash * hashTable, int msg, 
                                  int type, int bucket);
extern void *  HashGetRFromBucket (sHash * hashTable, int msg, 
                                   int type, int bucket, sHashState *);
extern boolean HashRemove (sHash * hashTable, void * key);
extern void * HashRemoveGet (sHash * hashTable, void * key);
extern boolean HashInsertKeyAndValue (sHash * hashTable, void * key, 
                                      void * value);
extern boolean HashUpdateKeyAndValue (sHash * hashTable, void * key, 
                                      void * value);
extern boolean HashInsert (sHash * hashTable, void * value);
extern void *  HashFind (sHash * hashTable, void * key);
extern void    HashDestroy (sHash * hashTable);
extern void    HashReset (sHash * hashTable);
extern sHash * _HashCreate (int inSize, char *caller, char *caller_line);
extern sHash * HashCreateExt (int inSize, hash_fn_t hash_fn,
                           key_equal_fn_t key_equal_fn,
                           char *caller, char *caller_line);
#ifdef EMS_PLATFORM
extern sHash * HashCreateLocked (int inSize);
#endif
extern __u32   HashEntries (sHash * hashTable);


extern void *  HashStringFind (sHash * hashTable, const char * str);
extern boolean HashIsStringPresent (sHash * hashTable, const char * str);
extern boolean HashStringInsert (sHash * hashTable, const char * str);
extern boolean HashStringInsertKeyAndValue (sHash * hashTable, 
                                            const char * str, void * value);
extern boolean HashStringUpdateKeyAndValue (sHash * hashTable, 
                                            const char * str, void * value);
extern boolean HashStringRemove (sHash * hashTable, const char * str);

extern void *  HashStringCaseFind (sHash * hashTable, const char * str);
extern boolean HashIsStringCasePresent (sHash * hashTable, const char * str);
extern boolean HashStringCaseInsert (sHash * hashTable, const char * str);
extern boolean HashStringCaseInsertKeyAndValue (sHash * hashTable, 
                                            const char * str, void * value);
extern boolean HashStringCaseUpdateKeyAndValue (sHash * hashTable, 
                                            const char * str, void * value);
extern boolean HashStringCaseRemove (sHash * hashTable, const char * str);

extern void *  HashMacFind (sHash * hashTable, const __u8 * mac);
extern boolean HashIsMacPresent (sHash * hashTable, const __u8 * mac);
extern boolean HashIsMacPresentAndSetHashState (sHash * hashTable, const __u8 * mac);
extern boolean HashMacInsertKeyAndValue (sHash * hashTable, const __u8 * mac, 
                                         void * value);
extern boolean HashMacUpdateKeyAndValue (sHash * hashTable, const __u8 * mac, 
                                         void * value);
extern boolean HashMacInsert (sHash * hashTable, const __u8 * mac);
extern boolean HashMacRemove (sHash * hashTable, const __u8 * mac);
extern boolean HashIsMacPresentandValue (sHash * hashTable, const __u8 * mac, void ** value);
extern unsigned int HashMacId (const __u8 * mac, unsigned int size);
#ifdef HAVE_IPV6_HASH 
void
HashDestroyIpv6 (sHash * hashTable);
extern sHash *_HashCreateIpv6(int inSize, sHashFunPtr *ptr, char *caller, 
        char *caller_line);
extern void *  HashIpv6AddrFind (sHash * hashTable, const prefixIpv6_t
                                 *ipv6Addr);
extern boolean HashIsIpv6AddrPresent(sHash * hashTable, const prefixIpv6_t
                                     *ipv6Addr);
extern boolean HashIpv6AddrInsertKeyAndValue (sHash * hashTable, const
                                              prefixIpv6_t    *ipv6Addr, 
                                         void * value);
extern boolean HashIpv6AddrUpdateKeyAndValue (sHash * hashTable, const
                                              prefixIpv6_t    *ipv6Addr, 
                                         void * value);
extern boolean HashIpv6AddrInsert (sHash * hashTable, const prefixIpv6_t
                                   *ipv6Addr);
extern boolean HashIpv6AddrRemove (sHash * hashTable, const prefixIpv6_t
                                   *ipv6Addr);
void *
HashIpv6GetR (sHash * hashTable, int msg, int type, sHashState *state);
void *
HashIpv6Get (sHash * hashTable, int msg, int type);
#endif

void
HashDestroyIpAf (sHash * hashTable);
extern void *  HashIpAfAddrFind (sHash * hashTable, const ip_addr_t
                                 *ipAfAddr);
extern boolean HashIsIpAfAddrPresent(sHash * hashTable, const ip_addr_t
                                     *ipAfAddr);
extern boolean HashIpAfAddrInsertKeyAndValue (sHash * hashTable, const
                                              ip_addr_t    *ipAfAddr, 
                                         void * value);
extern boolean HashIpAfAddrUpdateKeyAndValue (sHash * hashTable, const
                                              ip_addr_t    *ipAfAddr, 
                                         void * value);
extern boolean HashIpAfAddrInsert (sHash * hashTable, const ip_addr_t
                                   *ipAfAddr);
extern boolean HashIpAfAddrRemove (sHash * hashTable, const ip_addr_t
                                   *ipAfAddr);

void *
HashIpAfGetR (sHash * hashTable, int msg, int type, sHashState *state);
void *
HashIpAfGet (sHash * hashTable, int msg, int type);

#define HashCreate(size)                            \
        _HashCreate(size, (char *)__FUNCTION__, (char *)__LINE__)    \

#define HashCreateIpv6(inSize, ptr)                 \
    _HashCreateIpv6(inSize, ptr, (char *)__FUNCTION__, (char *)__LINE__)    \

#ifdef __cplusplus
};
#endif

#endif /* _HASH_H */
