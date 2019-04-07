/*
 * Copyright 2010-2011 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _FSL_VALIDATE_H_
#define _FSL_VALIDATE_H_

#include <fsl_secboot_types.h>
#include <command.h>

/* Barker code size in bytes */
#define ESBC_BARKER_LEN	4	/* barker code length in ESBC uboot client */
				/* header */

/* No-error return values */
#define ESBC_VALID_HDR	0	/* header is valid */

/* Maximum number of SG entries allowed */
#define MAX_SG_ENTRIES	8

/*
 * ESBC uboot client header structure.
 * The struct contain the following fields
 * barker code
 * public key offset
 * pub key length
 * signature offset
 * length of the signature
 * ptr to SG table
 * no of entries in SG table
 * esbc ptr
 * size of esbc
 * esbc entry point
 * Scatter gather flag
 * UID flag
 * FSL UID
 * OEM UID
 * Here, pub key is modulus concatenated with exponent
 * of equal length
 */
struct fsl_secboot_img_hdr {
	u8 barker[ESBC_BARKER_LEN];	/* barker code */
	u32 pkey;		/* public key offset */
	u32 key_len;		/* pub key length in bytes */
	u32 psign;		/* signature offset */
	u32 sign_len;		/* length of the signature in bytes */
	union {
		struct fsl_secboot_sg_table *psgtable;	/* ptr to SG table */
		u8 *pimg;	/* ptr to ESBC client image */
	};
	union {
		u32 sg_entries;	/* no of entries in SG table */
		u32 img_size;	/* ESBC client image size in bytes */
	};
	ulong img_start;		/* ESBC client entry point */
	u32 sg_flag;		/* Scatter gather flag */
	u32 uid_flag;		/* Flag to indicate uid is present or not */
	u32 fsl_uid;		/* Freescale unique id */
	u32 oem_uid;		/* OEM unique id */
};

/*
 * SG table.
 * This struct contains the following fields
 * length of the segment
 * pointer to data segment
 */
struct fsl_secboot_sg_table {
	u32 len;		/* length of the segment in bytes */
	u8 *pdata;		/* ptr to the data segment */
};

/*
 * ESBC private structure.
 * Private structure used by ESBC to store following fields
 * ESBC client key
 * ESBC client key hash
 * ESBC client Signature
 * Encoded hash recovered from signature
 * Encoded hash of ESBC client header plus ESBC client image
 */
struct fsl_secboot_img_priv {
	uint32_t hdr_location;
	struct fsl_secboot_img_hdr hdr;

	u8 img_key[2 * KEY_SIZE_BYTES];	/* ESBC client key */
	u8 img_key_hash[32];	/* ESBC client key hash */

	u8 img_sign[KEY_SIZE_BYTES];		/* ESBC client signature */

	u8 img_encoded_hash[KEY_SIZE_BYTES];	/* EM wrt RSA PKCSv1.5  */
						/* Includes hash recovered after
						 * signature verification
						 */

	u8 img_encoded_hash_second[KEY_SIZE_BYTES];/* EM' wrt RSA PKCSv1.5 */
						/* Includes hash of
						 * ESBC client header plus
						 * ESBC client image
						 */

	struct fsl_secboot_sg_table sgtbl[MAX_SG_ENTRIES];	/* SG table */
	u32 ehdrloc;		/* ESBC client location */
};

int fsl_secboot_validate(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[]);

#endif
