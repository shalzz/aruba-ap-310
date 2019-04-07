#ifndef _FL_CACHE_H_
#define _FL_CACHE_H_
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
/*
 * Definitions for cache area in AP flash. The cache is the 3rd sector
 * from the end.
 */

#ifndef __KERNEL__
#include <util/util.h>
#endif


#define APFC_DIGEST_SIZE  16
#define APFC_MAGIC_SIZE   8
#define APFC_MAGIC_STRING "TheCache"

typedef struct {
    char magic[APFC_MAGIC_SIZE];
    __u32 last_write_time;	/* can't use time_t due to 64-bit kernel */
    __u32 write_count;
    __u8 digest[APFC_DIGEST_SIZE];
} apfc_hdr_t;

#define APFC_ENTRY_TYPE_SIZE    12

typedef struct {
    char type[APFC_ENTRY_TYPE_SIZE];
    __u32 size;         // size does not include this header.
} apfc_entry_hdr_t;


#define APFC_MAX_ENTRIES        30
typedef struct {
    char *type;
    __u8 *data;
    __u32 size;
} apfc_entry_t;

#define APFC_ROUND_UP_SIZE 16
#define APFC_ROUND_UP(s)   \
    (((s) + APFC_ROUND_UP_SIZE - 1)  & ~(APFC_ROUND_UP_SIZE - 1))

#define APFC_TOTAL_ENTRY_SIZE(eh) \
    APFC_ROUND_UP(sizeof(apfc_entry_hdr_t) + (eh)->size)

#define APFC_NEXT_ENTRY(eh) \
    ((apfc_entry_hdr_t *) (((__u8 * ) (eh)) + APFC_TOTAL_ENTRY_SIZE(eh)))

#ifndef __KERNEL__
typedef enum {
    APFC_RESULT_OK,
    APFC_ALLOC_FAILED,
    APFC_IO_ERROR,
    APFC_NO_SPACE,
    APFC_UNKNOWN_AP_TYPE,
    APFC_UNINITIALIZED,
    APFC_CORRUPTED,
    APFC_NO_SECTOR,
    APFC_IO_ABORTED,
    APFC_NO_SCRATCH_SECTOR,
    APFC_NUM_RESULT_CODES
} apfc_result_t;

#include <sap/flash/fl_cache_types.h>

extern apfc_result_t apfc_init(apfc_sector_t sector);
extern apfc_result_t apfc_read(apfc_sector_t sector, int num_ent, char **types,
        __u8 **ptrs, __u32 *sizes);
extern apfc_result_t apfc_write(apfc_sector_t sector, int num_ent, char **types,
        __u8 **ptrs, __u32 *sizes);
// only for MFG use
extern apfc_result_t apfc_init_and_write(apfc_sector_t sector, int num_ent, char **types,
        __u8 **ptrs, __u32 *sizes);
extern apfc_result_t apfc_delete(apfc_sector_t sector, int num_ent, char **types);
extern apfc_result_t apfc_read_raw(apfc_sector_t sector, __u8 **ptr, __u32 *size);
extern void apfc_free_buf_all(void);
extern void apfc_free_buf(apfc_sector_t sector);
extern apfc_result_t apfc_get_last_write_time(apfc_sector_t sector, time_t *ptime);
extern apfc_result_t apfc_get_write_count(apfc_sector_t sector, __u32 *pcnt);
extern apfc_result_t apfc_check(apfc_sector_t sector);
extern apfc_result_t apfc_read_all(apfc_sector_t s, apfc_entry_t **entry_list, __u8 *num);
extern char *apfc_error_string(apfc_result_t);
extern char *apfc_sector_string(apfc_sector_t sec);
extern apfc_result_t apfc_get_sector_info(apfc_sector_t sector);
extern apfc_result_t apfc_unlock(apfc_sector_t s);
extern apfc_result_t apfc_lock(apfc_sector_t s);
/*
 * Files in APFC_DIR will be written to the flash cache upon reboot.
 */

#define APFC_DIR        "/tmp/apfc"
#endif // !__KERNEL__

#endif // _FL_CACHE_H_
