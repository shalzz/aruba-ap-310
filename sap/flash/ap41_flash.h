#ifndef __AP41_FLASH_COMMON_H__
#define __AP41_FLASH_COMMON_H__
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
/*****************************************************
		Flash MAP for the AP41:
	
 		+-----------------------+ <- 0xBFC00000
		|                       |		
		|                       |   256KB reserved for IDTBoot (8x8K + 3x64KB sectors)
		|                       |
		+-----------------------+ <- 0xBFC40000
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |		
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		+-----------------------+ <- 0xBFFB0000
		|                       |  5th from top 64KB sector: mesh cert
		+-----------------------+ <- 0xBFFC0000
		|                       |  4th from top 64KB sector: environment
		+-----------------------+
		|                       |  3rd from top 64KB sector: Atheros/Vx bootstring
		+-----------------------+
		|                       |  2nd from top 64KB sector: Manufacturing
		+-----------------------+
		|                       |  1st from top 64KB sector: RF calibration data
       	        +-----------------------+ <- 0xBFFFFFFF
		
 *****************************************************/

#define AP41_NUM_FLASH              1
#define AP41_FLASH_FIRST_OFFSET 0xAAA /* Flash cmd offsets */
#define AP41_FLASH_SECND_OFFSET 0x555

#define AP41_TOT_NUM_OF_SECTORS	64
#define AP41_NUM_OF_8K_SECTORS	8	/* first 8 sectors are KB */
#define AP41_NUM_OF_64K_SECTORS	63  /* next 63 sectors are 64KB */

#ifndef AP41_FLASH_SIZE
#define AP41_FLASH_SIZE         0x400000
#endif
#define AP41_FLASH_START	0xBFC00000
#define AP41_FLASH_END		((AP41_FLASH_START)+(AP41_FLASH_SIZE))
#define AP41_FLASH_IDTBOOT_END  ((AP41_FLASH_START)+0x40000) /* 4x64KB frm start */

#define AP41_FLASH_SECTOR1_SIZE (8*1024)  /* 8KB */
#define AP41_FLASH_SECTOR2_SIZE (64*1024) /* 64KB */

#define AP41_ENV_PARAMS_SECT_START	((AP41_FLASH_END)-(4*(AP41_FLASH_SECTOR2_SIZE)))
#define AP41_ENV_PARAMS_SECT_SIZE	(AP41_FLASH_SECTOR2_SIZE)
#define AP41_ENV_PARAMS_CFG_SIZE	(AP41_FLASH_SECTOR1_SIZE)

#define AP41_ENV_ATHEROS_STR_SECT_START ((AP41_FLASH_END)-(3*(AP41_FLASH_SECTOR2_SIZE)))
#define AP41_ENV_ATHEROS_STR_SECT_SIZE  (AP41_FLASH_SECTOR2_SIZE)

#define AP41_ART_SECT_START	        ((AP41_FLASH_END)-(2*(AP41_FLASH_SECTOR2_SIZE)))
#define AP41_ART_SECT_SIZE	        (AP41_FLASH_SECTOR2_SIZE)

#define AP41_RF_CALIB_SECT_START	((AP41_FLASH_END)-(1*(AP41_FLASH_SECTOR2_SIZE)))
#define AP41_RF_CALIB_SECT_SIZE	        (AP41_FLASH_SECTOR2_SIZE)

#define AP41_OVERWRITE_IDTBOOT(x) ((((x)<AP41_FLASH_IDTBOOT_END)&&((x)>=AP41_FLASH_START))?1:0)
#define AP41_OVERWRITE_LAST_4(x) (((x)>=AP41_ENV_PARAMS_SECT_START)?1:0)

#endif /*__AP41_FLASH_H__*/
