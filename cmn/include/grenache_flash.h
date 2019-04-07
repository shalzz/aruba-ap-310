#ifndef __grenache_flash_h__
#define __grenache_flash_h__
/*
 * Copyright (C) 2002-2007 by Aruba Networks, Inc.
 * All Rights Reserved.
 * 
 * This software is an unpublished work and is protected by copyright and 
 * trade secret law.  Unauthorized copying, redistribution or other use of 
 * this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */
#define NORFLASH_SECT_SIZE          0x10000
#define NORFLASH_BOOTROM_OFFSET     0
#define NORFLASH_NVRAM_OFFSET       0x180000
#define NORFLASH_MESH_PROV_OFFSET   0x190000
#define NORFLASH_MESH_CERT_OFFSET   0x1a0000
#define NORFLASH_RAP_DATA_OFFSET    0x1b0000
#define NORFLASH_AP_SCRATCH_OFFSET  0x1c0000
#define NORFLASH_AP_CACHE_OFFSET    0x1d0000
#define NORFLASH_RADIO_DATA_OFFSET  0x1e0000
#define NORFLASH_BOOT_ENV_OFFSET    0x1f0000
#define BOOTROM_FILENAME            "/dev/mtd0"
#endif /* __grenache_flash_h__ */
