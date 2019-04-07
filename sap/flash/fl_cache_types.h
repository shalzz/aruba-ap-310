#ifndef _FL_CACHE_TYPES_H_
#define _FL_CACHE_TYPES_H_
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
typedef enum {
    APFC_MESH_PROV,
    APFC_MESH_CERT,
    APFC_RAP,
    APFC_SCRATCH,               /* shared scratch sector for above sectors */
    APFC_CACHE,                 /* the original AP Flash Cache sector */
    APFC_FAT_CONFIG,		/* "fat" AP config */
    APFC_DRT_FILE,
    APFC_NUM_SECTORS
} apfc_sector_t;

#define	APFC_NONE	-1

#endif // _FL_CACHE_TYPES_H_
