#ifndef __AP60_FLASH_COMMON_H__
#define __AP60_FLASH_COMMON_H__
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
/*****************************************************
	Flash MAP for the MERLOT/MUSCAT:

 		+-----------------------+ <- 0xBFC00000
		|                       |		
		|                       |   256KB reserved for IDTBoot (4x64KB sectors)
		|                       |
		|                       |
		+-----------------------+ <- 0xBFC40000
		|                       |  1x64KB sector: Mesh Provision
		+-----------------------+
		|                       |  1x64KB sector: Mesh Cert
		+-----------------------+
		|                       |  1x64KB sector: RAPv3 config
		+-----------------------+
		|                       |  1x64KB sector: shared redundnt/scratch sectr
		|                       |                 for above 3 sectors      
		+-----------------------+ <- 0xBFC80000
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |		
		|                       |   (55=63-8)x64KB sectors for image storage
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |
		|                       |	
		|                       |	
		+-----------------------+ <- 0xBFFF0000
		|         .             |
		|         .             |  Start of 8x8KB sectors
		+-----------------------+ <- 0xBFFF0000
		|                       |  5nd from top 8KB sector: Manufacturing PROM
		+-----------------------+
		|                       |  4th from top 8KB sector: IDT Env bootparams
		+-----------------------+
		|                       |  3rd from top 8KB sector: Atheros/Vx bootstring / flash cache
		+-----------------------+
		|                       |  2nd from top 8KB sector: ART (Manufacturing)
		+-----------------------+
		|                       |  1st from top 8KB sector: RF calibration data
		+-----------------------+ <- 0xBFFFFFFF
*******************************************************************************

	Flash map for Palomino/Moscato

Palomino has 16MB of flash, divided into 128KB sectors.

Palomino flash starts at 0xbfc00000 when the system powers up, but after
startup, it is re-mapped to 0xbec00000 so that the entire flash can be
mapped on the Octeon boot bus.  In Linux we refer to it at 0x1ec00000 (the
physical address).

	+-----------------------+ <- 0xBEC00000 (after bootup)
	|                       |   Stage 1 boot (2 sectors)
	|                       |		
	+-----------------------+ <- 0xBEC40000
	|                       |   Stage 2 boot bank 1 (2 sectors)
	|                       |
	+-----------------------+ <- 0xBEC80000
	|                       |   Stage 2 boot bank 2 (2 sectors)
	|                       |
	+-----------------------+ <- 0xBEA00000
	|                       |   Unused (2 sectors)
	|                       |
	+-----------------------+ <- 0xBED00000
	|                       |
	|                       |
	|                       |
	|                       |
	|                       |   64x128KB sectors for production image storage
	|                       |		
	|                       |
	|                       |		
	+-----------------------+ <- 0xBF500000
	|                       |
	|                       |
	|                       |   32x128KB sectors for provisioning image storage
	|                       |
	|                       |
	+-----------------------+ <- 0xBF900000
	|                       |   Stage 2 Boot upgrade staging area
	|                       |
	+-----------------------+ <- 0xBF940000
	|                       |   13x128KB spare sectors
	|                       |
	+-----------------------+
	|                       |  9th from top; panic dump information @ 0xbfae0000
	+-----------------------+
	|                       |  8th from top; Fat AP Config @ 0xbfb00000
	+-----------------------+
	|                       |  7th from top; AP mesh provisioning @ 0xbfb20000
	+-----------------------+
	|                       |  6th from top; AP TPM data @ 0xbfb40000
	+-----------------------+
	|                       |  5th from top; remote AP information @ 0xbfb60000
	+-----------------------+
	|                       |  4th from top; AP scratch sector @ 0xbfb80000
	+-----------------------+
	|                       |  3rd from top; AP flash cache @ 0xbfba0000
	+-----------------------+
	|                       |  2nd from top 128KB sector: Mfg / Radio data
	|			|  (mfg data @ offset 0, radio data @ offset 
	|			|   0x400 and 0x2400); radio data is present only
	|                       |  on Palomino.
	+-----------------------+
	|                       |  1st from top 128KB sector: Environment data
	+-----------------------+ <- 0xBFBFFFFF
		
*******************************************************************************

	Flash map for Chuck

Accton device has 8MB of flash, divided into 64KB sectors.

Flash starts at 0xa8000000 (0xbfc00000 during boot)
		
    +-----------------------+ <- 0xA8000000 (after bootup)
    |                       |
    |                       |   256KB reserved for idtboot
    |                       |
    +-----------------------+ <- 0xA8040000  (256KB spare)
    |                       |
    +-----------------------+ <- 0xA8080000  (0xbfc80000)
    |                       |
    |                       |
    |                       |
    |                       |
    |                       |  (112=128-16)*64KB sectors for image (~7M)
    |.......................| <- 0xA83F0000  (provisioning image)
    |                       |
    |                       |
    |                       |
    |                       |
    |                       |
    +-----------------------+
    |                       |  spare
    +-----------------------+
    |                       |  7th from top; AP mesh provisioning @ 0xa8790000
    +-----------------------+
    |                       |  6th from top; AP mesh certificate @ 0xa87a0000
    +-----------------------+
    |                       |  5th from top; remote AP information @ 0xa87b0000
    +-----------------------+
    |                       |  4th from top; AP scratch sector @ 0xa87c0000
    +-----------------------+
    |                       |  3rd from top; AP flash cache @ 0xa87d0000
    +-----------------------+
    |                       |  2nd from top (0xa87e0000): Environment data
    +-----------------------+
    |                       |  1st from top (0xa87f0000): Mfg / Radio data
    |                       |  (mfg data @ offset 0, radio data @ offset 0xf8)
    |                       |  (serial number @ offset 0x88, MAC @ offset 0x66)
    +-----------------------+ <- 0xA87FFFFF

*******************************************************************************

	Flash map for Mijiu

PepWave/Wistron device has 8MB of flash, divided into 64KB sectors.

(similar to Merlot/Muscat, using 8MB flash instead of 4MB)

Flash starts at 0xa8000000 (0xbfc00000 during boot)

    +-----------------------+ <- 0xa8000000 (after bootup)
    |                       |
    |                       |   256KB reserved for idtboot
    |                       |
    +-----------------------+ <- 0xa8040000  (256KB spare)
    |                       |
    +-----------------------+ <- 0xa8080000  (0xbfc80000)
    |                       |
    |                       |
    |                       |
    |                       |
    |                       |  (110=128-18)*64KB sectors for image (~7M)
    |.......................| <- 0xa83f0000  (provisioning image)
    |                       |
    |                       |
    |                       |
    |                       |
    |                       |
    +-----------------------+
    |         spare         | 10th from top; spare @ 0xa8760000
    +-----------------------+
    |                       |  9th from top; AP mesh provisioning @ 0xa8770000
    +-----------------------+
    |                       |  8th from top; AP mesh certificate @ 0xa8780000
    +-----------------------+
    |                       |  7th from top; remote AP information @ 0xa8790000
    +-----------------------+
    |                       |  6th from top; AP scratch sector @ 0xa87a0000
    +-----------------------+
    |                       |  5th from top; Mfg PROM (optional) @ 0xa87b0000
    +-----------------------+
    |                       |  4th from top; IDT Env bootparams @ 0xa87c0000
    +-----------------------+
    |                       |  3rd from top; Ath/VxBoot/fl-cache @ 0xa87d0000
    +-----------------------+
    |                       |  2nd from top; ART (Manufacturing) @ 0xa87e0000
    |                       |  (SN @ offset 0xe000, MAC @ offset 0x60)
    +-----------------------+
    |                       |  1st from top; RF calibration data @ 0xa87f0000
    +-----------------------+ <- 0xa87fffff


*******************************************************************************

	Flash map for Talisker/Jura-R/MSR2K/BlueBlood(16M)/Scapa(16M)

Talisker has 16MB of serial flash, divided into 64KB sectors.
Talisker flash starts at 0xbfc00000 when the system boots up, but after
startup, it is re-mapped to 0xbf000000 so that the entire flash can be
mapped into memory.  In Linux we refer to it at 0x1f000000 (the
physical address).

	+-----------------------+ <- 0xBF000000 (after bootup)
	|                       |		
	|                       |   1MB reserved for U-boot (16 64KB sectors)
	|                       |
	+-----------------------+ <- 0xBF100000  (primary image = 112 64KB sectors)
	|                       |
	|                       |
	|                       |
	|                       |   (224=256-16-7-9)x64KB sectors for image storage (2 x 7M images)
	|.......................| <- 0xBF800000  (secondary image = 112 64KB sectors)
	|                       |
	|                       |
	|                       |
	|                       |
	+-----------------------+ <- 0xBFF00000
	|                       |  7 x 64KB spare sectors @ 0xbff00000
	+-----------------------+ <- 0xBFF70000
	|                       |  9th from top; AP panic dump @ 0xbff70000
	+-----------------------+
	|                       |  8th from top; Fat AP Config @ 0xbff80000
	+-----------------------+
	|                       |  7th from top; AP mesh provisioning @ 0xbff90000
	+-----------------------+
	|                       |  6th from top; TPM data @ 0xbffa0000
	+-----------------------+
	|                       |  5th from top; remote AP information @ 0xbffb0000
	+-----------------------+
	|                       |  4th from top; AP scratch sector @ 0xbffc0000
	+-----------------------+
	|                       |  3rd from top; AP flash cache @ 0xbffd0000
	+-----------------------+
	|                       |  2nd from top 64KB sector: Mfg data @ 0xbffe0000
	+-----------------------+
	|                       |  1st from top 64KB sector: Environment data @
	|                       |  0xbfff0000
	+-----------------------+ <- 0xBFFFFFFF
		
*******************************************************************************

	Flash map for Scapa/BlueBlood(8M)

Scapa has 8MB of serial flash, divided into 64KB sectors.
Scapa flash starts at 0xbfc00000 when the system boots up, but after
startup, it is re-mapped to 0xbf000000 so that the entire flash can be
mapped into memory.  In Linux we refer to it at 0x1f000000 (the
physical address).

	+-----------------------+ <- 0xBF000000 (after bootup)
	|                       |		
	|                       |   1MB reserved for U-boot (16 64KB sectors)
	|                       |
	+-----------------------+ <- 0xBF100000
	|                       |
	|                       |
	|                       |
	|                       |
	|                       |		
	|                       |   96 sectors for image storage (6MB)
	|                       |
	|                       |
	|                       |
	|                       |
	|                       |
	|                       |
	+-----------------------+
	|                       |  7 spare sectors at 0xbf700000
	+-----------------------+
	|                       |  9th from top; AP panic dump @ 0xbf770000
	+-----------------------+
	|                       |  8th from top; Fat AP Config @ 0xbf780000
	+-----------------------+
	|                       |  7th from top; AP mesh provisioning @ 0xbf790000
	+-----------------------+
	|                       |  6th from top; TPM data @ 0xbf7a0000
	+-----------------------+
	|                       |  5th from top; remote AP information @ 0xbf7b0000
	+-----------------------+
	|                       |  4th from top; AP scratch sector @ 0xbf7c0000
	+-----------------------+
	|                       |  3rd from top; AP flash cache @ 0xbf7d0000
	+-----------------------+
	|                       |  2nd from top 64KB sector: Mfg data @ 0xbf7e0000
	+-----------------------+
	|                       |  1st from top 64KB sector: Environment data @
	|                       |  0xbf7f0000
	+-----------------------+ <- 0xBF7FFFFF
		
*******************************************************************************

	Flash map for Arran

Arran has 16MB of serial flash, divided into 64KB sectors.
The addresses below are in U-boot numbering for ease of debugging.

	+-----------------------+ <- 0xF6000000
	|                       |		
	|                       |   1MB reserved for U-boot (16 64KB sectors)
	|                       |
	+-----------------------+ <- 0xF6100000  (primary image = 112 64KB sectors)
	|                       |
	|                       |
	|                       |
	|                       |   (224=256-16-7-9)x64KB sectors for image storage (2 x 7M images)
	|.......................| <- 0xF6800000  (secondary image = 112 64KB sectors)
	|                       |
	|                       |
	|                       |
	|                       |
	+-----------------------+ <- 0xF6F00000
	|                       |  7 x 64KB spare sectors @ 0xF6F00000
	+-----------------------+
	|                       |  9th from top; AP panic dump @ 0xf6f70000
	+-----------------------+
	|                       |  8th from top; Fat AP Config @ 0xf6f80000
	+-----------------------+
	|                       |  7th from top; AP mesh provisioning @ 0xf6f90000
	+-----------------------+
	|                       |  6th from top; TPM data @ 0xf6fa0000
	+-----------------------+
	|                       |  5th from top; remote AP information @ 0xf6fb0000
	+-----------------------+
	|                       |  4th from top; AP scratch sector @ 0xf6fc0000
	+-----------------------+
	|                       |  3rd from top; AP flash cache @ 0xf6fd0000
	+-----------------------+
	|                       |  2nd from top 64KB sector: Mfg data @ 0xf6fe0000
	+-----------------------+
	|                       |  1st from top 64KB sector: Environment data @
	|                       |  0xf6ff0000
	+-----------------------+ <- 0xF6FFFFFF

*******************************************************************************

	Flash map for Ardmore

Ardmore has 32MB of parallel NOR flash, divided into 128KB sectors.
The addresses below are in U-boot numbering for ease of debugging.
Unlike other platforms, the Freescale bootloader is at the end of the
flash since the CPU starts execution at 0xfffffffc.  This means that everything
else is off from other designs; we put the images at the beginning of the
flash, and the other sectors are four down from where they normally would
be.  We also only reserve 512K for APBoot on this design, rather than the
usual 1MB.

	+-----------------------+ <- 0xEE000000  (primary image = 112 128KB sectors)
	|                       |		
	|                       |   14MB reserved for OS 0
	|                       |
	+-----------------------+ <- 0xEEE00000  (secondary image = 112 128KB sectors)
	|                       |
	|                       |   (224=256-4-9-19)x128KB sectors for image storage (2 x 14M images)
	|                       |   14MB reserved for OS 1
	|                       |
	+-----------------------+ <- 0xEFC00000
	|                       |  19 x 128KB spare sectors @ 0xEFC00000
	+-----------------------+
	|                       |  13th from top; AP panic dump @ 0xEFE60000
	+-----------------------+
	|                       |  12th from top; Fat AP Config @ 0x0xEFE80000
	+-----------------------+
	|                       |  11th from top; AP mesh provisioning @ 0xEFEA0000
	+-----------------------+
	|                       |  10th from top; TPM data @ 0xEFEC0000
	+-----------------------+
	|                       |  9th from top; remote AP information @ 0xEFEE0000
	+-----------------------+
	|                       |  8th from top; AP scratch sector @ 0xEFF00000
	+-----------------------+
	|                       |  7th from top; AP cache sector @ 0xEFF20000
	+-----------------------+
	|                       |  6th from top 128KB sector: Mfg data @ 0xEFF40000
	+-----------------------+
	|                       |  5th from top 128KB sector: environment data @ 0xEFF60000
	|                       |
	+-----------------------+  4th from top: Boot loader @ 0xEFF80000 (4 128KB sectors)
	|                       |
	|                       |
	+-----------------------+ <- 0xEFFFFFFF

*******************************************************************************

	Flash map for Springbank

Springbank has 32MB of serial NOR flash, divided into 64KB sectors.
The addresses below are in U-boot numbering for ease of debugging.

	+-----------------------+ <- 0x1E000000  (APBoot = 16 64KB sectors)
	|                       |		
	|                       |   1MB reserved for APBoot
	|                       |
	+-----------------------+ <- 0x1E100000  (primary image = 224 64KB sectors)
	|                       |
	|                       |   (448=512-16-9-39)x128KB sectors for image storage (2 x 14M images)
	|                       |   14MB reserved for OS 0
	|                       |
	+-----------------------+ <- 0x1EF00000
	|                       |
	|                       |   14MB reserved for OS 1
	|                       |
	+-----------------------+ <- 0x1FD00000
	|                       |  39 x 64KB spare sectors @ 0x1FD00000
	+-----------------------+
	|                       |  9th from top; AP panic dump @ 0x1FF70000
	+-----------------------+
	|                       |  8th from top; Fat AP Config @ 0x0x1FF80000
	+-----------------------+
	|                       |  7th from top; AP mesh provisioning @ 0x1FF90000
	+-----------------------+
	|                       |  6th from top; TPM data @ 0x1FFA0000
	+-----------------------+
	|                       |  5th from top; remote AP information @ 0x1FFB0000
	+-----------------------+
	|                       |  4th from top; AP scratch sector @ 0x1FFC0000
	+-----------------------+
	|                       |  3rd from top; AP cache sector @ 0x1FFD0000
	+-----------------------+
	|                       |  2nd from top 128KB sector: Mfg data @ 0x1FFE0000
	+-----------------------+
	|                       |  top 64KB sector: environment data @ 0x1FFF0000
	+-----------------------+ <- 0x1FFFFFFF

*****************************************************/

#define AP2WG_FLASH_SECTOR1_SIZE (64*1024) /* 64KB */
#define AP2WG_FLASH_SECTOR2_SIZE (64*1024) /* 64KB */
#define AP2WG_FLASH_START (0xA8000000)
#define AP2WG_TOT_FLASH_SIZE (0x800000)
#define AP2WG_FLASH_END ((AP2WG_FLASH_START)+(AP2WG_TOT_FLASH_SIZE))
#define AP2WG_FLASH_IDTBOOT_END (AP2WG_FLASH_START+0x80000) /*includes spare*/
#define AP2WG_ENV_PARAMS_SECT_START (AP2WG_FLASH_END-2*AP2WG_FLASH_SECTOR2_SIZE)
#define AP2WG_ENV_PARAMS_SECT_SIZE (AP2WG_FLASH_SECTOR2_SIZE)
#define AP2WG_ENV_PARAMS_CFG_SIZE (8*1024)
#define AP2WG_MANUFACT_SECT_START (AP2WG_FLASH_END - AP2WG_FLASH_SECTOR2_SIZE)
#define AP2WG_MANUFACT_SECT_SIZE (AP2WG_FLASH_SECTOR2_SIZE)
#define AP2WG_MANUFACT_SECT_END (AP2WG_MANUFACT_SECT_START + \
        AP2WG_MANUFACT_SECT_SIZE)
#define AP2WG_PROVISIONING_IMAGE (AP2WG_FLASH_IDTBOOT_END + 0x370000 - \
        AP2WG_FLASH_START)
#define AP2WG_OVERWRITE_IDTBOOT(x) (((x<AP2WG_FLASH_IDTBOOT_END)&&\
        (x>=AP2WG_FLASH_START))?1:0)
#define AP2WG_OVERWRITE_MFG_DATA(x) (((x<AP2WG_MANUFACT_SECT_END)&&\
        (x>=AP2WG_MANUFACT_SECT_START))?1:0)
#define AP2WG_SNUM_OFFSET	0x7F0088 /* 9 bytes of serial num */
#define AP2WG_SNUM_SIZE		9
#define AP2WG_SAP_IMG_START       0xa8080000
#define AP2WG_FLASH_IMG_OFFSET    0x80000
#define AP2WG_FLASH_SIZE          0x800000
#define AP2WG_FLASH_IMG_SIZE      ((AP2WG_FLASH_SIZE)-(AP2WG_FLASH_IMG_OFFSET))

#define AP60P_FLASH_SECTOR1_SIZE (64*1024) /* 64KB */
#define AP60P_FLASH_SECTOR2_SIZE (64*1024) /* 64KB */
#define AP60P_FLASH_START (0xA8000000)
#define AP60P_TOT_FLASH_SIZE (0x800000)
#define AP60P_FLASH_END ((AP60P_FLASH_START)+(AP60P_TOT_FLASH_SIZE))
#define AP60P_FLASH_IDTBOOT_END (AP60P_FLASH_START+0x80000) /*includes spare*/
#define AP60P_ENV_PARAMS_SECT_START (AP60P_FLASH_END-4*AP60P_FLASH_SECTOR2_SIZE)
#define AP60P_ENV_PARAMS_SECT_SIZE (AP60P_FLASH_SECTOR2_SIZE)
#define AP60P_ENV_PARAMS_CFG_SIZE (8*1024)
#define AP60P_MANUFACT_SECT_START (AP60P_FLASH_END - 2*AP60P_FLASH_SECTOR2_SIZE)
#define AP60P_MANUFACT_SECT_SIZE (AP60P_FLASH_SECTOR2_SIZE)
#define AP60P_MANUFACT_SECT_END (AP60P_MANUFACT_SECT_START + \
        AP60P_MANUFACT_SECT_SIZE)
#define AP60P_ART_SECT_START (AP60P_FLASH_END - AP60P_FLASH_SECTOR2_SIZE)
#define AP60P_ART_SECT_SIZE (AP60P_FLASH_SECTOR2_SIZE)
#define AP60P_ART_SECT_END (AP60P_ART_SECT_START + AP60P_ART_SECT_SIZE)
#define AP60P_PROVISIONING_IMAGE (AP60P_FLASH_IDTBOOT_END + 0x370000 - AP60P_FLASH_START)
#define AP60P_OVERWRITE_IDTBOOT(x) (((x<AP60P_FLASH_IDTBOOT_END)&&\
        (x>=AP60P_FLASH_START))?1:0)
#define AP60P_OVERWRITE_MFG_DATA(x) (((x<AP60P_MANUFACT_SECT_END)&&\
        (x>=AP60P_MANUFACT_SECT_START))?1:0)
#define AP60P_OVERWRITE_ART_DATA(x) (((x<AP60P_ART_SECT_END)&&\
        (x>=AP60P_ART_SECT_START))?1:0)
#define AP60P_SNUM_OFFSET	0x7EE000 /* 9 bytes of serial num */
#define AP60P_SNUM_SIZE		9
#define AP60P_SAP_IMG_START       0xa8080000
#define AP60P_FLASH_IMG_OFFSET    0x80000
#define AP60P_FLASH_SIZE          0x800000
#define AP60P_FLASH_IMG_SIZE      ((AP60P_FLASH_SIZE)-(AP60P_FLASH_IMG_OFFSET))


#define AP70_FLASH_LOCK_PIN (1<<3)
#define AP70_FLASH_LOCK_ADR 0xB8010008 /* in virtual space */

#define ATHEROS_FLASH_LOCK_PIN 0x04000000
#define ATHEROS_FLASH_LOCK_ADR 0xB8400000

#define AP60_FLASH_FIRST_OFFSET 0xAAA /* Flash cmd offsets */
#define AP60_FLASH_SECND_OFFSET 0x555

#define TOT_NUM_OF_SECTORS	64
#define NUM_OF_8K_SECTORS	8	/* first 8 sectors are KB */
#define NUM_OF_64K_SECTORS	63  /* next 63 sectors are 64KB */

#define AP60_FLASH_SECTOR1_SIZE (64*1024) /* 64KB */
#define AP60_FLASH_SECTOR2_SIZE (8*1024)  /* 8KB */

#define SIZE_64K_SECTORS	(63*(AP60_FLASH_SECTOR1_SIZE))
#define SIZE_8K_SECTORS		(8*(AP60_FLASH_SECTOR2_SIZE))
#define AP60_TOT_FLASH_SIZE	((SIZE_8K_SECTORS)+(SIZE_64K_SECTORS))

#define AP60_FLASH_START	(0xBFC00000)
#define AP60_FLASH_END		((AP60_FLASH_START)+(AP60_TOT_FLASH_SIZE)) /* bfffffff */
#define AP60_FLASH_IDTBOOT_END (0xBFC40000) /* 4x64KB frm start */

#define LAST_SECT_START		(AP60_FLASH_END - 8*AP60_FLASH_SECTOR2_SIZE)
#define AP60_MANUFACT_SECT_START	((AP60_FLASH_END)-(5*(AP60_FLASH_SECTOR2_SIZE)))
#define AP60_MANUFACT_SECT_SIZE	(AP60_FLASH_SECTOR2_SIZE)

#define AP60_ENV_PARAMS_SECT_START	((AP60_FLASH_END)-(4*(AP60_FLASH_SECTOR2_SIZE)))
#define AP60_ENV_PARAMS_SECT_SIZE	(AP60_FLASH_SECTOR2_SIZE)

#define ENV_ATHEROS_STR_SECT_START ((AP60_FLASH_END)-(3*(AP60_FLASH_SECTOR2_SIZE)))
#define ENV_ATHEROS_STR_SECT_SIZE  (AP60_FLASH_SECTOR2_SIZE)

#define ART_SECT_START	((AP60_FLASH_END)-(2*(AP60_FLASH_SECTOR2_SIZE)))
#define ART_SECT_SIZE	(AP60_FLASH_SECTOR2_SIZE)

#define RF_CALIB_SECT_START	((AP60_FLASH_END)-(AP60_FLASH_SECTOR2_SIZE))
#define RF_CALIB_SECT_SIZE	(AP60_FLASH_SECTOR2_SIZE)

#define OVERWRITE_IDTBOOT(x) (((x<AP60_FLASH_IDTBOOT_END)&&(x>=AP60_FLASH_START))?1:0)
#define OVERWRITE_LAST_4(x) ((x>ENV_PARAMS_SECT_START)?1:0)

#define AP80_FLASH_START	(0xBE000000)
#define AP80_TOT_FLASH_SIZE	(0x800000)
#define AP80_FLASH_END		((AP80_FLASH_START)+(AP80_TOT_FLASH_SIZE)) /* be7fffff */
#define AP80_FLASH_IDTBOOT_END	(AP80_FLASH_START+0x40000)
#define AP80_ENV_PARAMS_SECT_START (AP80_FLASH_START+63*0x10000+0x8000)

#define AP80_OVERWRITE_IDTBOOT(x) (((x<AP80_FLASH_IDTBOOT_END)&&(x>=AP80_FLASH_START))?1:0)

#define AP80_SNUM_OFFSET	0x7FC0E0 /* 10 bytes of serial num */
#define AP80_SNUM_SIZE		10
#define AP80_REV_OFFSET		0x7FC0EE /* 15 bytes of rev num */
#define AP80_REV_SIZE		10
#define AP60_SAP_IMG_START	 0xbfc80000 /* upper 512KB for idtboot */
#define AP60_FLASH_IMG_OFFSET	 0x80000
#define AP60_FLASH_SIZE		 0x400000
#define AP60_FLASH_IMG_SIZE	 ((AP60_FLASH_SIZE)-(AP60_FLASH_IMG_OFFSET))
#define AP_80_IMAGE2_OFFSET	0x400000

#define AP40_SNUM_OFFSET	0x3E0088 /* 9 bytes of serial num */
#define AP40_SNUM_SIZE		9
#define AP40_SAP_IMG_START       AP41_SAP_IMG_START
#define AP40_FLASH_IMG_OFFSET    AP41_FLASH_IMG_OFFSET
#define AP40_FLASH_IMG_SIZE      AP41_FLASH_IMG_SIZE

#define AP41_SNUM_OFFSET	0x3E007c /* 9 bytes of serial num */
#define AP41_SNUM_SIZE		9
#define AP41_FLASH_IMG_OFFSET	 0x40000
#define AP41_FLASH_SIZE		 0x400000
#define AP41_SAP_IMG_START       0xbfc40000 /* upper 256KB for idtboot */
#define AP41_FLASH_IMG_SIZE	 ((AP41_FLASH_SIZE)-(AP41_FLASH_IMG_OFFSET))

#define APDUOWJ_SNUM_OFFSET	0x7FC063 /* 9 bytes of serial num */
#define APDUOWJ_SNUM_SIZE	3

#define AP12x_FLASH_SECTOR1_SIZE (128*1024) /* 128KB */
#define AP12x_FLASH_SECTOR2_SIZE (128*1024)  /* 128KB */
#define AP12x_FLASH_START	(0xBEC00000)
#define AP12x_TOT_FLASH_SIZE	(0x1000000)
#define AP12x_FLASH_END		((AP12x_FLASH_START)+(AP12x_TOT_FLASH_SIZE)) /* 0xbfbfffff*/
#define AP12x_FLASH_IDTBOOT_END	(AP12x_FLASH_START+0x100000)
#define AP12x_ENV_PARAMS_SECT_START (AP12x_FLASH_END - AP12x_FLASH_SECTOR2_SIZE)
#define AP12x_ENV_PARAMS_SECT_SIZE (AP12x_FLASH_SECTOR2_SIZE)

#define AP12x_ENV_ATHEROS_STR_SECT_START ((AP12x_FLASH_END)-(3*(AP12x_FLASH_SECTOR2_SIZE)))
#define AP12x_ENV_ATHEROS_STR_SECT_SIZE  (AP12x_FLASH_SECTOR2_SIZE)

#define AP12x_LAST_SECT_START	(AP12x_FLASH_END - 2*AP12x_FLASH_SECTOR2_SIZE)
#define AP12x_MANUFACT_SECT_START	((AP12x_FLASH_END)-(2*(AP12x_FLASH_SECTOR2_SIZE)))
#define AP12x_MANUFACT_SECT_SIZE	(AP12x_FLASH_SECTOR2_SIZE)
#define AP12x_OVERWRITE_IDTBOOT(x) (((x<AP12x_FLASH_IDTBOOT_END)&&(x>=AP12x_FLASH_START))?1:0)
#define RAP5_PROVISIONING_IMAGE (AP12x_FLASH_IDTBOOT_END + 0x800000 - AP12x_FLASH_START)
#define	AP12x_BOOT_STAGING_AREA_SIZE (AP12x_FLASH_SECTOR1_SIZE * 2)
#define	AP12x_BOOT_STAGING_AREA (0xBF900000 - AP12x_FLASH_START)
#define AP12x_PANIC_SECT_START	(AP12x_FLASH_END - 9*AP12x_FLASH_SECTOR2_SIZE)
#define AP12x_SAP_IMG_START	 0xbed00000
#define AP12x_FLASH_IMG_OFFSET	 0x100000

#define BBGW_FLASH_SECTOR1_SIZE (128*1024) /* 128KB */
#define BBGW_FLASH_SECTOR2_SIZE (128*1024)  /* 128KB */
#define BBGW_FLASH_START	(0xBDC00000)
#define BBGW_TOT_FLASH_SIZE	(0x2000000)
#define BBGW_FLASH_END		((BBGW_FLASH_START)+(BBGW_TOT_FLASH_SIZE)) /* 0xbfbfffff*/
#define BBGW_FLASH_IDTBOOT_END	(BBGW_FLASH_START+0x100000)
#define BBGW_ENV_PARAMS_SECT_START (BBGW_FLASH_END - BBGW_FLASH_SECTOR2_SIZE)
#define BBGW_ENV_PARAMS_SECT_SIZE (BBGW_FLASH_SECTOR2_SIZE)

#define BBGW_ENV_ATHEROS_STR_SECT_START ((BBGW_FLASH_END)-(3*(BBGW_FLASH_SECTOR2_SIZE)))
#define BBGW_ENV_ATHEROS_STR_SECT_SIZE  (BBGW_FLASH_SECTOR2_SIZE)

#define BBGW_LAST_SECT_START	(BBGW_FLASH_END - 2*BBGW_FLASH_SECTOR2_SIZE)
#define BBGW_MANUFACT_SECT_START	((BBGW_FLASH_END)-(2*(BBGW_FLASH_SECTOR2_SIZE)))
#define BBGW_MANUFACT_SECT_SIZE	(BBGW_FLASH_SECTOR2_SIZE)
#define BBGW_OVERWRITE_IDTBOOT(x) (((x<BBGW_FLASH_IDTBOOT_END)&&(x>=BBGW_FLASH_START))?1:0)
//#define RAP5_PROVISIONING_IMAGE (BBGW_FLASH_IDTBOOT_END + 0x800000 - BBGW_FLASH_START)
#define	BBGW_BOOT_STAGING_AREA_SIZE (BBGW_FLASH_SECTOR1_SIZE * 2)
#define	BBGW_BOOT_STAGING_AREA (0xBF900000 - BBGW_FLASH_START)
#define BBGW_PANIC_SECT_START	(BBGW_FLASH_END - 9*BBGW_FLASH_SECTOR2_SIZE)
#define BBGW_SAP_IMG_START	 0xbdd00000
#define BBGW_FLASH_IMG_OFFSET	 0x100000

#define AP10x_FLASH_SECTOR1_SIZE (64*1024) /* 64KB */
#define AP10x_FLASH_SECTOR2_SIZE (64*1024)  /* 64KB */
#define AP10x_FLASH_START	(0xBF000000)
#define AP10x_TOT_FLASH_SIZE	(0x1000000)
#define AP10x_FLASH_END		((AP10x_FLASH_START)+(AP10x_TOT_FLASH_SIZE)) /* 0xbfffffff*/
#define AP10x_FLASH_IDTBOOT_END	(AP10x_FLASH_START+0x100000)
#define AP10x_ENV_PARAMS_SECT_START (AP10x_FLASH_END - AP10x_FLASH_SECTOR2_SIZE)
#define AP10x_ENV_PARAMS_SECT_SIZE (AP10x_FLASH_SECTOR2_SIZE)

#define AP10x_ENV_ATHEROS_STR_SECT_START ((AP10x_FLASH_END)-(3*(AP10x_FLASH_SECTOR2_SIZE)))
#define AP10x_ENV_ATHEROS_STR_SECT_SIZE  (AP10x_FLASH_SECTOR2_SIZE)

#define AP10x_LAST_SECT_START	(AP10x_FLASH_END - 2*AP10x_FLASH_SECTOR2_SIZE)
#define AP10x_MANUFACT_SECT_START	((AP10x_FLASH_END)-(2*(AP10x_FLASH_SECTOR2_SIZE)))
#define AP10x_MANUFACT_SECT_SIZE	(AP10x_FLASH_SECTOR2_SIZE)
#define AP10x_OVERWRITE_IDTBOOT(x) (((x<AP10x_FLASH_IDTBOOT_END)&&(x>=AP10x_FLASH_START))?1:0)
#define AP10x_PANIC_SECT_START	((AP10x_FLASH_END)-(9*(AP10x_FLASH_SECTOR2_SIZE)))
#define AP10x_PANIC_SECT_SIZE	(AP10x_FLASH_SECTOR2_SIZE)
#define AP10x_SAP_IMG_START	 0xbf100000
#define AP10x_FLASH_IMG_OFFSET	 0x100000

/* country-code = mfg offset + 512 bytes */
#define AP10x_CCODE_OFFSET	0xFE0100  /* CCODE-<ccode>-<sha1> */
#define AP10x_CCODE_SIZE	64        /* 6 (CCODE-) + 7 (<CC>-) + 40 (SHA1) + 11 spare bytes */

// Scapa / AP-92
#define AP9x_FLASH_SECTOR1_SIZE (64*1024) /* 64KB */
#define AP9x_FLASH_SECTOR2_SIZE (64*1024)  /* 64KB */
#define AP9x_FLASH_START	(0xBF000000)
#define AP9x_TOT_FLASH_SIZE	(0x800000)
#define AP9x_FLASH_END		((AP9x_FLASH_START)+(AP9x_TOT_FLASH_SIZE)) /* 0xbf7fffff */
#define AP9x_FLASH_IDTBOOT_END	(AP9x_FLASH_START+0x100000)
#define AP9x_ENV_PARAMS_SECT_START (AP9x_FLASH_END - AP9x_FLASH_SECTOR2_SIZE)
#define AP9x_ENV_PARAMS_SECT_SIZE (AP9x_FLASH_SECTOR2_SIZE)
#define AP9x_ENV_ATHEROS_STR_SECT_START ((AP9x_FLASH_END)-(3*(AP9x_FLASH_SECTOR2_SIZE)))
#define AP9x_ENV_ATHEROS_STR_SECT_SIZE  (AP9x_FLASH_SECTOR2_SIZE)
#define AP9x_LAST_SECT_START	(AP9x_FLASH_END - 2*AP9x_FLASH_SECTOR2_SIZE)
#define AP9x_MANUFACT_SECT_START	((AP9x_FLASH_END)-(2*(AP9x_FLASH_SECTOR2_SIZE)))
#define AP9x_MANUFACT_SECT_SIZE	(AP9x_FLASH_SECTOR2_SIZE)
#define AP9x_OVERWRITE_IDTBOOT(x) (((x<AP9x_FLASH_IDTBOOT_END)&&(x>=AP9x_FLASH_START))?1:0)
#define AP9x_PANIC_SECT_START	((AP9x_FLASH_END)-(9*(AP9x_FLASH_SECTOR2_SIZE)))
#define AP9x_PANIC_SECT_SIZE	(AP9x_FLASH_SECTOR2_SIZE)
#define AP9x_SAP_IMG_START	 0xbf100000
#define AP9x_FLASH_IMG_OFFSET	 0x100000

// Arran / AP-13x
#define AP13x_FLASH_SECTOR1_SIZE (64*1024) /* 64KB */
#define AP13x_FLASH_SECTOR2_SIZE (64*1024)  /* 64KB */
#define AP13x_FLASH_START	(0xf6000000)
#define AP13x_TOT_FLASH_SIZE	(0x1000000)
#define AP13x_FLASH_END		((AP13x_FLASH_START)+(AP13x_TOT_FLASH_SIZE)) /* 0xbf7fffff */
#define AP13x_FLASH_IDTBOOT_END	(AP13x_FLASH_START+0x100000)
#define AP13x_ENV_PARAMS_SECT_START (AP13x_FLASH_END - AP13x_FLASH_SECTOR2_SIZE)
#define AP13x_ENV_PARAMS_SECT_SIZE (AP13x_FLASH_SECTOR2_SIZE)
#define AP13x_ENV_ATHEROS_STR_SECT_START ((AP13x_FLASH_END)-(3*(AP13x_FLASH_SECTOR2_SIZE)))
#define AP13x_ENV_ATHEROS_STR_SECT_SIZE  (AP13x_FLASH_SECTOR2_SIZE)
#define AP13x_LAST_SECT_START	(AP13x_FLASH_END - 2*AP13x_FLASH_SECTOR2_SIZE)
#define AP13x_MANUFACT_SECT_START	((AP13x_FLASH_END)-(2*(AP13x_FLASH_SECTOR2_SIZE)))
#define AP13x_MANUFACT_SECT_SIZE	(AP13x_FLASH_SECTOR2_SIZE)
#define AP13x_OVERWRITE_IDTBOOT(x) (((x<AP13x_FLASH_IDTBOOT_END)&&(x>=AP13x_FLASH_START))?1:0)
#define AP13x_PANIC_SECT_START	((AP13x_FLASH_END)-(9*(AP13x_FLASH_SECTOR2_SIZE)))
#define AP13x_PANIC_SECT_SIZE	(AP13x_FLASH_SECTOR2_SIZE)
#define AP13x_SAP_IMG_START	 0xf6100000
#define AP13x_FLASH_IMG_OFFSET	 0x100000

// Tamdhu
#define TAMDHU_FLASH_SECTOR1_SIZE (64*1024) /* 64KB */
#define TAMDHU_FLASH_SECTOR2_SIZE (64*1024)  /* 64KB */
#define TAMDHU_FLASH_START	(0xf6000000)
#define TAMDHU_TOT_FLASH_SIZE	(0x2000000)
#define TAMDHU_FLASH_END		((TAMDHU_FLASH_START)+(TAMDHU_TOT_FLASH_SIZE)) /* 0xbf7fffff */
#define TAMDHU_FLASH_IDTBOOT_END	(TAMDHU_FLASH_START+0x100000)
#define TAMDHU_ENV_PARAMS_SECT_START (TAMDHU_FLASH_END - TAMDHU_FLASH_SECTOR2_SIZE)
#define TAMDHU_ENV_PARAMS_SECT_SIZE (TAMDHU_FLASH_SECTOR2_SIZE)
#define TAMDHU_ENV_ATHEROS_STR_SECT_START ((TAMDHU_FLASH_END)-(3*(TAMDHU_FLASH_SECTOR2_SIZE)))
#define TAMDHU_ENV_ATHEROS_STR_SECT_SIZE  (TAMDHU_FLASH_SECTOR2_SIZE)
#define TAMDHU_LAST_SECT_START	(TAMDHU_FLASH_END - 2*TAMDHU_FLASH_SECTOR2_SIZE)
#define TAMDHU_MANUFACT_SECT_START	((TAMDHU_FLASH_END)-(2*(TAMDHU_FLASH_SECTOR2_SIZE)))
#define TAMDHU_MANUFACT_SECT_SIZE	(TAMDHU_FLASH_SECTOR2_SIZE)
#define TAMDHU_OVERWRITE_IDTBOOT(x) (((x<TAMDHU_FLASH_IDTBOOT_END)&&(x>=TAMDHU_FLASH_START))?1:0)
#define TAMDHU_PANIC_SECT_START	((TAMDHU_FLASH_END)-(9*(TAMDHU_FLASH_SECTOR2_SIZE)))
#define TAMDHU_PANIC_SECT_SIZE	(TAMDHU_FLASH_SECTOR2_SIZE)
#define TAMDHU_SAP_IMG_START	 0xf6100000
#define TAMDHU_FLASH_IMG_OFFSET	 0x100000

/* Ardmore */
#define ARDMORE_FLASH_SECTOR1_SIZE (128*1024) /* 128KB */
#define ARDMORE_FLASH_SECTOR2_SIZE (128*1024)  /* 128KB */
#define ARDMORE_FLASH_START	(0xee000000)
#define ARDMORE_TOT_FLASH_SIZE	(0x2000000)
#define ARDMORE_FLASH_END		((ARDMORE_FLASH_START)+(ARDMORE_TOT_FLASH_SIZE))
#define ARDMORE_FLASH_IDTBOOT_START	(ARDMORE_FLASH_END - (4 * ARDMORE_FLASH_SECTOR2_SIZE))
#define ARDMORE_ENV_PARAMS_SECT_START (ARDMORE_FLASH_END - (5*ARDMORE_FLASH_SECTOR2_SIZE))
#define ARDMORE_ENV_PARAMS_SECT_SIZE (ARDMORE_FLASH_SECTOR2_SIZE)
#define ARDMORE_MANUFACT_SECT_START	((ARDMORE_FLASH_END)-(6*(ARDMORE_FLASH_SECTOR2_SIZE)))
#define ARDMORE_MANUFACT_SECT_SIZE	(ARDMORE_FLASH_SECTOR2_SIZE)
#define ARDMORE_OVERWRITE_IDTBOOT(x) (((x<ARDMORE_FLASH_END)&&(x>=ARDMORE_FLASH_IDTBOOT_START))?1:0)
#define ARDMORE_PANIC_SECT_START	((ARDMORE_FLASH_END)-(13*(ARDMORE_FLASH_SECTOR2_SIZE)))
#define ARDMORE_PANIC_SECT_SIZE	(ARDMORE_FLASH_SECTOR2_SIZE)
#define ARDMORE_SAP_IMG_START	 0xee000000

/* Tomatin 16MB flash */
#define TOMATIN_FLASH_SECTOR1_SIZE (128*1024) /* 128KB */
#define TOMATIN_FLASH_SECTOR2_SIZE (128*1024)  /* 128KB */
#define TOMATIN_FLASH_START	(0xef000000)
#define TOMATIN_TOT_FLASH_SIZE	(0x1000000)
#define TOMATIN_FLASH_END		((TOMATIN_FLASH_START)+(TOMATIN_TOT_FLASH_SIZE))
#define TOMATIN_FLASH_IDTBOOT_START	(TOMATIN_FLASH_END - (4 * TOMATIN_FLASH_SECTOR2_SIZE))
#define TOMATIN_ENV_PARAMS_SECT_START (TOMATIN_FLASH_END - (5*TOMATIN_FLASH_SECTOR2_SIZE))
#define TOMATIN_ENV_PARAMS_SECT_SIZE (TOMATIN_FLASH_SECTOR2_SIZE)
#define TOMATIN_MANUFACT_SECT_START	((TOMATIN_FLASH_END)-(6*(TOMATIN_FLASH_SECTOR2_SIZE)))
#define TOMATIN_MANUFACT_SECT_SIZE	(TOMATIN_FLASH_SECTOR2_SIZE)
#define TOMATIN_OVERWRITE_IDTBOOT(x) (((x<TOMATIN_FLASH_END)&&(x>=TOMATIN_FLASH_IDTBOOT_START))?1:0)
#define TOMATIN_PANIC_SECT_START	((TOMATIN_FLASH_END)-(13*(TOMATIN_FLASH_SECTOR2_SIZE)))
#define TOMATIN_PANIC_SECT_SIZE	(TOMATIN_FLASH_SECTOR2_SIZE)
#define TOMATIN_SAP_IMG_START	 0xef000000

/* Springbank */
#define SPRINGBANK_FLASH_SECTOR1_SIZE (64*1024) /* 64KB */
#define SPRINGBANK_FLASH_SECTOR2_SIZE (64*1024)  /* 64KB */
#define SPRINGBANK_FLASH_START	(0x1e000000)
#define SPRINGBANK_TOT_FLASH_SIZE	(0x2000000)
#define SPRINGBANK_FLASH_END		((SPRINGBANK_FLASH_START)+(SPRINGBANK_TOT_FLASH_SIZE))
#define SPRINGBANK_FLASH_IDTBOOT_START	SPRINGBANK_FLASH_START
#define SPRINGBANK_ENV_PARAMS_SECT_START (SPRINGBANK_FLASH_END - SPRINGBANK_FLASH_SECTOR2_SIZE)
#define SPRINGBANK_ENV_PARAMS_SECT_SIZE (SPRINGBANK_FLASH_SECTOR2_SIZE)
#define SPRINGBANK_MANUFACT_SECT_START	((SPRINGBANK_FLASH_END)-(2*(SPRINGBANK_FLASH_SECTOR2_SIZE)))
#define SPRINGBANK_MANUFACT_SECT_SIZE	(SPRINGBANK_FLASH_SECTOR2_SIZE)
#define SPRINGBANK_OVERWRITE_IDTBOOT(x) (((x<SPRINGBANK_FLASH_END)&&(x>=SPRINGBANK_FLASH_IDTBOOT_START))?1:0)
#define SPRINGBANK_PANIC_SECT_START	((SPRINGBANK_FLASH_END)-(9*(SPRINGBANK_FLASH_SECTOR2_SIZE)))
#define SPRINGBANK_PANIC_SECT_SIZE	(SPRINGBANK_FLASH_SECTOR2_SIZE)
#define SPRINGBANK_SAP_IMG_START	 0x1e100000
#define DRT_SECT_SIZE     (256*1024) 
/* Springbank 16MB flash */
#define SPRINGBANK_16_FLASH_SECTOR1_SIZE (64*1024) /* 64KB */
#define SPRINGBANK_16_FLASH_SECTOR2_SIZE (64*1024)  /* 64KB */
#define SPRINGBANK_16_FLASH_START	(0x1e000000)
#define SPRINGBANK_16_TOT_FLASH_SIZE	(0x1000000)
#define SPRINGBANK_16_FLASH_END		((SPRINGBANK_16_FLASH_START)+(SPRINGBANK_16_TOT_FLASH_SIZE))
#define SPRINGBANK_16_FLASH_IDTBOOT_START	SPRINGBANK_16_FLASH_START
#define SPRINGBANK_16_ENV_PARAMS_SECT_START (SPRINGBANK_16_FLASH_END - SPRINGBANK_16_FLASH_SECTOR2_SIZE)
#define SPRINGBANK_16_ENV_PARAMS_SECT_SIZE (SPRINGBANK_16_FLASH_SECTOR2_SIZE)
#define SPRINGBANK_16_MANUFACT_SECT_START	((SPRINGBANK_16_FLASH_END)-(2*(SPRINGBANK_16_FLASH_SECTOR2_SIZE)))
#define SPRINGBANK_16_MANUFACT_SECT_SIZE	(SPRINGBANK_16_FLASH_SECTOR2_SIZE)
#define SPRINGBANK_16_OVERWRITE_IDTBOOT(x) (((x<SPRINGBANK_16_FLASH_END)&&(x>=SPRINGBANK_16_FLASH_IDTBOOT_START))?1:0)
#define SPRINGBANK_16_PANIC_SECT_START	((SPRINGBANK_16_FLASH_END)-(9*(SPRINGBANK_16_FLASH_SECTOR2_SIZE)))
#define SPRINGBANK_16_PANIC_SECT_SIZE	(SPRINGBANK_16_FLASH_SECTOR2_SIZE)
#define SPRINGBANK_16_SAP_IMG_START	 0x1e100000

/* P1020RDB (not P1020WLAN, that is the same is Ardmore) */
#define P1020RDB_FLASH_SECTOR1_SIZE (128*1024) /* 128KB */
#define P1020RDB_FLASH_SECTOR2_SIZE (128*1024)  /* 128KB */
#define P1020RDB_FLASH_START	(0xef000000)
#define P1020RDB_TOT_FLASH_SIZE	(0x1000000)
#define P1020RDB_FLASH_END		((P1020RDB_FLASH_START)+(P1020RDB_TOT_FLASH_SIZE))
#define P1020RDB_FLASH_IDTBOOT_START	(P1020RDB_FLASH_END - (4 * P1020RDB_FLASH_SECTOR2_SIZE))
#define P1020RDB_ENV_PARAMS_SECT_START (P1020RDB_FLASH_END - (5*P1020RDB_FLASH_SECTOR2_SIZE))
#define P1020RDB_ENV_PARAMS_SECT_SIZE (P1020RDB_FLASH_SECTOR2_SIZE)
#define P1020RDB_MANUFACT_SECT_START	((P1020RDB_FLASH_END)-(6*(P1020RDB_FLASH_SECTOR2_SIZE)))
#define P1020RDB_MANUFACT_SECT_SIZE	(P1020RDB_FLASH_SECTOR2_SIZE)
#define P1020RDB_OVERWRITE_IDTBOOT(x) (((x<P1020RDB_FLASH_END)&&(x>=P1020RDB_FLASH_IDTBOOT_START))?1:0)
#define P1020RDB_PANIC_SECT_START	((P1020RDB_FLASH_END)-(13*(P1020RDB_FLASH_SECTOR2_SIZE)))
#define P1020RDB_PANIC_SECT_SIZE	(P1020RDB_FLASH_SECTOR2_SIZE)
#define P1020RDB_SAP_IMG_START	 0xef000000


/* X4 */
#define X4_FLASH_SECTOR_SIZE        (64*1024) /* 64KB */
#define X4_FLASH_START	            0 
#define X4_TOT_FLASH_SIZE	        (4 * 1024 * 1024)
#define X4_FLASH_END		        (X4_FLASH_START+X4_TOT_FLASH_SIZE)
#define X4_FLASH_IDTBOOT_START	    (X4_FLASH_END - (4 * X4_FLASH_SECTOR_SIZE))
#define X4_ENV_PARAMS_SECT_START    0x100000 // (X4_FLASH_END - (7 * X4_FLASH_SECTOR_SIZE))
#define X4_ENV_PARAMS_SECT_SIZE     X4_FLASH_SECTOR_SIZE 
#define X4_FMAN_UCODE_START         0x110000    
#define X4_FMAN_UCODE_SIZE          X4_FLASH_SECTOR_SIZE
#define X4_MANUFACT_SECT_START	    0x130000
#define X4_MANUFACT_SECT_SIZE	    X4_FLASH_SECTOR_SIZE
#define X4_OVERWRITE_IDTBOOT(x)     (((x<X4_FLASH_END)&&(x>=X4_FLASH_IDTBOOT_START))?1:0)
#define X4_PANIC_SECT_START	        0x160000
#define X4_PANIC_SECT_SIZE	        X4_FLASH_SECTOR_SIZE
#define X4_CACHE_SECT_START	        0x170000 
#define X4_CACHE_SECT_SIZE	        X4_FLASH_SECTOR_SIZE 
#define X4_CERT_SECT_START	        0x180000 
#define X4_CERT_SECT_SIZE	        X4_FLASH_SECTOR_SIZE 
#define X4_RADIO_CAL_SECT_START     0x190000
#define X4_RADIO_CAL_SECT_SIZE      (4 * X4_FLASH_SECTOR_SIZE)      
#define X4_MESH_PROV_START	        0x1D0000 
#define X4_RAP_SECT_START	        0x1E0000 
#define X4_SCRATCH_SECT_START	    0x1F0000 
#define X4_SAP_IMG_START	        0xef000000

/* Masterson */
#define MASTERSON_FLASH_SECTOR_SIZE     0x1F000 /* 124kiB for UBI LEB */
#define MASTERSON_MANUFACT_SECT_SIZE    MASTERSON_FLASH_SECTOR_SIZE

/* Lagavulin */
#define LAGAVULIN_FLASH_SECTOR_SIZE 0x1F000
#define LAGAVULIN_MANUFACT_SECT_SIZE  LAGAVULIN_FLASH_SECTOR_SIZE 

//XXXVINAYXXX
#define AP45x_FLASH_SECTOR1_SIZE	NORFLASH_SECT_SIZE
#define AP45x_FLASH_SECTOR2_SIZE	NORFLASH_SECT_SIZE
//#define AP45x_FLASH_SECTOR1_SIZE (64*1024) /* 128KB */
//#define AP45x_FLASH_SECTOR2_SIZE (64*1024)  /* 128KB */
#define AP45x_FLASH_START	(0xBC000000)
#define AP45x_TOT_FLASH_SIZE	(0x1000000)
#define AP45x_FLASH_END		((AP45x_FLASH_START)+(AP45x_TOT_FLASH_SIZE)) /* 0xbfbfffff*/
#define AP45x_FLASH_IDTBOOT_END	(AP45x_FLASH_START+0x100000)
//#define AP45x_ENV_PARAMS_SECT_START (0xBC1F0000) // XXXRGKXXX
#define AP45x_ENV_PARAMS_SECT_START	(AP45x_FLASH_START+NORFLASH_BOOT_ENV_OFFSET)
#define AP45x_ENV_PARAMS_SECT_SIZE (AP45x_FLASH_SECTOR2_SIZE) // XXXSCAXXX

#define AP45x_ENV_ATHEROS_STR_SECT_START ((AP45x_FLASH_END)-(3*(AP45x_FLASH_SECTOR2_SIZE)))
#define AP45x_ENV_ATHEROS_STR_SECT_SIZE  (AP45x_FLASH_SECTOR2_SIZE)

#define AP45x_LAST_SECT_START	(AP45x_FLASH_END - 2*AP45x_FLASH_SECTOR2_SIZE)
#define AP45x_MANUFACT_SECT_START	(AP45x_FLASH_START+NORFLASH_RADIO_DATA_OFFSET)
#define AP45x_MANUFACT_SECT_SIZE	(AP45x_FLASH_SECTOR2_SIZE)
#define AP45x_OVERWRITE_IDTBOOT(x) (((x<AP45x_FLASH_IDTBOOT_END)&&(x>=AP45x_FLASH_START))?1:0)

#ifdef CONFIG_MERLOT
#define MANUFACT_SECT_SIZE AP60_MANUFACT_SECT_SIZE
#define MAX_ENV_PARAMS_SECT_SIZE AP2WG_ENV_PARAMS_SECT_SIZE
#endif

#ifdef CONFIG_PALOMINO
#define MANUFACT_SECT_SIZE AP12x_MANUFACT_SECT_SIZE
#endif

#if defined(CONFIG_TALISKER) || defined(CONFIG_DALMORE) || defined(CONFIG_BOWMORE)
#define MANUFACT_SECT_SIZE AP10x_MANUFACT_SECT_SIZE
#endif

#ifdef CONFIG_ARRAN
#define MANUFACT_SECT_SIZE AP13x_MANUFACT_SECT_SIZE
#endif

#ifdef CONFIG_ARDMORE
#define MANUFACT_SECT_SIZE ARDMORE_MANUFACT_SECT_SIZE
#endif

#ifdef CONFIG_SPRINGBANK
#define MANUFACT_SECT_SIZE SPRINGBANK_MANUFACT_SECT_SIZE
#endif

#ifdef CONFIG_OCTOMORE
#define AP_FLASH_SIZE AP10x_TOT_FLASH_SIZE
#define AP_FLASH_SECTOR1_SIZE AP10x_FLASH_SECTOR1_SIZE
#define AP_FLASH_SECTOR2_SIZE AP10x_FLASH_SECTOR2_SIZE
#define ENV_PARAMS_SECT_SIZE AP10x_ENV_PARAMS_SECT_SIZE
#define MANUFACT_SECT_SIZE AP10x_MANUFACT_SECT_SIZE
#endif

#ifdef CONFIG_MASTERSON
#define MANUFACT_SECT_SIZE MASTERSON_MANUFACT_SECT_SIZE
#endif

#ifdef CONFIG_X4
#define MANUFACT_SECT_SIZE X4_FLASH_SECTOR_SIZE
#endif

#ifdef CONFIG_GLENMORANGIE
#define MANUFACT_SECT_SIZE AP10x_MANUFACT_SECT_SIZE
#endif

#ifdef CONFIG_KILCHOMAN
#define MANUFACT_SECT_SIZE AP10x_MANUFACT_SECT_SIZE
#endif

#ifdef CONFIG_LAGAVULIN
#define MANUFACT_SECT_SIZE LAGAVULIN_MANUFACT_SECT_SIZE  
#endif

#endif /*__AP60_FLASH_COMMON_H__*/
