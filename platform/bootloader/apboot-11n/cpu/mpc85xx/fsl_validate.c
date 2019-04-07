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

#include <fsl_validate.h>
#include <fsl_secboot_err.h>
#include <asm/fsl_sfp_snvs.h>
#include <command.h>
#include <common.h>
#include <malloc.h>
#include <rsa.h>
#include <sha256.h>

#define SHA256_BITS	256
#define SHA256_BYTES	(256/8)
#define SHA256_NIBBLES	(256/4)
#define NUM_HEX_CHARS	(sizeof(ulong) * 2)

/* This array contains DER value for SHA-256 */
static const u8 hash_identifier[] = { 0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60,
		0x86, 0x48, 0x01, 0x65,	0x03, 0x04, 0x02, 0x01, 0x05, 0x00,
		0x04, 0x20
		};

static u8 hash_val[SHA256_BYTES];
static const u8 barker_code[ESBC_BARKER_LEN] = { 0x68, 0x39, 0x27, 0x81 };

void branch_to_self(void) __attribute__ ((noreturn));

/*
 * This function will put core in infinite loop.
 * This will be called when the ESBC can not proceed further due
 * to some errors.
 */
void branch_to_self(void)
{
	printf("Core is in infinite loop due to errors.\n");
	while (1);
}

/*
 * Handles the ESBC uboot client header verification failure.
 * This  function  handles all the errors which might occur in the
 * parsing and checking of ESBC uboot client header. It will also
 * set the error bits in the SNVS.
 */
static void fsl_secboot_header_verification_failure(void)
{
	ccsr_snvs_regs_t *snvs_regs = (void *)(CONFIG_SYS_SNVS_ADDR);
	ccsr_sfp_regs_t *sfp_regs = (void *)(CONFIG_SYS_SFP_ADDR);
	u32 sts = in_be32(&snvs_regs->hp_stat);

	/* 29th bit of OSPR is ITS */
	u32 its = in_be32(&sfp_regs->ospr) >> 2;

	/*
	 * Read the SNVS status register
	 * Read SSM_ST field
	 */
	sts = in_be32(&snvs_regs->hp_stat);
	if ((sts & HPSR_SSM_ST_MASK) == HPSR_SSM_ST_TRUST) {
		if (its == 1)
			change_sec_mon_state(HPSR_SSM_ST_TRUST,
				HPSR_SSM_ST_SOFT_FAIL);
		else
			change_sec_mon_state(HPSR_SSM_ST_TRUST,
				HPSR_SSM_ST_NON_SECURE);
	}

	generate_reset_req();
}

/*
 * Handles the ESBC uboot client image verification failure.
 * This  function  handles all the errors which might occur in the
 * public key hash comparison and signature verification of
 * ESBC uboot client image. It will also
 * set the error bits in the SNVS.
 */
static void fsl_secboot_image_verification_failure(void)
{
	ccsr_snvs_regs_t *snvs_regs = (void *)(CONFIG_SYS_SNVS_ADDR);
	ccsr_sfp_regs_t *sfp_regs = (void *)(CONFIG_SYS_SFP_ADDR);
	u32 sts = in_be32(&snvs_regs->hp_stat);

	/* 31st bit of OVPR is ITF */
	u32 itf = in_be32(&sfp_regs->ovpr);

	/*
	 * Read the SNVS status register
	 * Read SSM_ST field
	 */
	sts = in_be32(&snvs_regs->hp_stat);
	if ((sts & HPSR_SSM_ST_MASK) == HPSR_SSM_ST_TRUST) {
		if (itf == 1) {
			change_sec_mon_state(HPSR_SSM_ST_TRUST,
				HPSR_SSM_ST_SOFT_FAIL);

			generate_reset_req();
		} else {
			change_sec_mon_state(HPSR_SSM_ST_TRUST,
				HPSR_SSM_ST_NON_SECURE);
		}
	}
}

static void fsl_secboot_bootscript_parse_failure(void)
{
	fsl_secboot_header_verification_failure();
}

/*
 * Handles the errors in esbc boot.
 * This  function  handles all the errors which might occur in the
 * esbc boot phase. It will call the appropriate api to log the
 * errors and set the error bits in the SNVS.
 */
static void fsl_secboot_handle_error(int error)
{
	const struct fsl_secboot_errcode *e;

	for (e = fsl_secboot_errcodes; e->errcode != ERROR_ESBC_CLIENT_MAX;
		e++) {
		if (e->errcode == error)
			printf("ERROR :: %x :: %s\n", error, e->name);
	}

	switch (error) {
	case ERROR_ESBC_CLIENT_HEADER_BARKER:
	case ERROR_ESBC_CLIENT_HEADER_KEY_LEN:
	case ERROR_ESBC_CLIENT_HEADER_SIG_LEN:
	case ERROR_ESBC_CLIENT_HEADER_KEY_LEN_NOT_TWICE_SIG_LEN:
	case ERROR_ESBC_CLIENT_HEADER_KEY_MOD_1:
	case ERROR_ESBC_CLIENT_HEADER_KEY_MOD_2:
	case ERROR_ESBC_CLIENT_HEADER_SIG_KEY_MOD:
	case ERROR_ESBC_CLIENT_HEADER_SG_ESBC_EP:
	case ERROR_ESBC_CLIENT_HEADER_SG_ENTIRES_BAD:
		fsl_secboot_header_verification_failure();
	case ERROR_ESBC_CLIENT_HASH_COMPARE_KEY:
	case ERROR_ESBC_CLIENT_HASH_COMPARE_EM:
		fsl_secboot_image_verification_failure();
		break;
	case ERROR_ESBC_MISSING_BOOTM:
		fsl_secboot_bootscript_parse_failure();
		break;
	case ERROR_ESBC_WRONG_CMD:
	default:
		branch_to_self();
		break;
	}
}

/*
 * Calculate hash of key obtained via offset present in ESBC uboot
 * client hdr. This function calculates the hash of key which is obtained
 * through offset present in ESBC uboot client header.
 */
static void calc_img_key_hash(sha256_context *ctx,
				struct fsl_secboot_img_priv *img)
{
	int i;

	/* calc hash of the esbc key */
	sha256_starts(ctx);
	sha256_update(ctx, img->img_key, img->hdr.key_len);
	sha256_finish(ctx, hash_val);

	for (i = 0; i < SHA256_BYTES; i++)
		img->img_key_hash[i] = hash_val[i];
}

/*
 * Calculate hash of ESBC hdr and ESBC. This function calculates the
 * single hash of ESBC header and ESBC image. If SG flag is on, all
 * SG entries are also hashed alongwith the complete SG table.
 */
static void calc_esbchdr_esbc_hash(sha256_context *ctx,
	struct fsl_secboot_img_priv *img)
{
	int i = 0;

	/* calculate the hash of the CSF header */
	sha256_starts(ctx);
	sha256_update(ctx, (u8 *) &img->hdr,
		sizeof(struct fsl_secboot_img_hdr));
	sha256_update(ctx, img->img_key, img->hdr.key_len);

	if (img->hdr.sg_flag) {
		/* calculate hash of the SG table */
		sha256_update(ctx, (u8 *) &img->sgtbl,
			img->hdr.sg_entries *
			sizeof(struct fsl_secboot_sg_table));

		/* calculate the hash of each entry in the table */
		for (i = 0; i < img->hdr.sg_entries; i++)
			sha256_update(ctx, img->sgtbl[i].pdata,
				img->sgtbl[i].len);
	} else {
		/* contiguous ESBC */
		sha256_update(ctx, (u8 *) img->hdr.pimg,
			img->hdr.img_size);
	}

	sha256_finish(ctx, hash_val);
}

/*
 * Construct encoded hash EM' wrt PKCSv1.5. This function calculates the
 * pointers for padding, DER value and hash. And finally, constructs EM'
 * which includes hash of complete CSF header and ESBC image. If SG flag
 * is on, hash of SG table and entries is also included.
 */
static void construct_img_encoded_hash_second(struct fsl_secboot_img_priv *img)
{
	/*
	 * RSA PKCSv1.5 encoding format for encoded message is below
	 * EM = 0x0 || 0x1 || PS || 0x0 || DER || Hash
	 * PS is Padding String
	 * DER is DER value for SHA-256
	 * Hash is SHA-256 hash
	 * *********************************************************
	 * representative points to first byte of EM initially and is
	 * filled with 0x0
	 * representative is incremented by 1 and second byte is filled
	 * with 0x1
	 * padding points to third byte of EM
	 * digest points to full length of EM - 32 bytes
	 * hash_id (DER value) points to 19 bytes before pDigest
	 * separator is one byte which separates padding and DER
	 */

	size_t len;
	u8 *representative;
	u8 *padding, *digest;
	u8 *hash_id, *separator;
	int i;

	len = (img->hdr.key_len / 2) - 1;
	representative = img->img_encoded_hash_second;
	representative[0] = 0;
	representative[1] = 1;  /* block type 1 */

	padding = &representative[2];
	digest = &representative[1] + len - 32;
	hash_id = digest - sizeof(hash_identifier);
	separator = hash_id - 1;

	/* fill padding area pointed by padding with 0xff */
	memset(padding, 0xff, separator - padding);

	/* fill byte pointed by separator */
	*separator = 0;

	/* fill SHA-256 DER value  pointed by HashId */
	memcpy(hash_id, hash_identifier, sizeof(hash_identifier));

	/* fill hash pointed by Digest */
	for (i = 0; i < SHA256_BYTES; i++)
		digest[i] = hash_val[i];
}

/*
 * Reads and validates the ESBC client header.
 * This function reads key and signature from the ESBC client header.
 * If Scatter/Gather flag is on, lengths and offsets of images
 * present as SG entries are also read. This function also checks
 * whether the header is valid or not.
 */
static int read_validate_esbc_client_header(struct fsl_secboot_img_priv *img)
{
	char buf[20];
	struct fsl_secboot_img_hdr *hdr = &img->hdr;
	void *esbc = (u8 *) img->ehdrloc;
	u8 *k, *s;

	/* check barker code */
	if (memcmp(hdr->barker, barker_code, ESBC_BARKER_LEN))
		return ERROR_ESBC_CLIENT_HEADER_BARKER;

	sprintf(buf, "%p", hdr->pimg);
	setenv("img_addr", buf);

	/* key length should be twice of signature length */
	if (hdr->key_len == 2 * hdr->sign_len) {
		/* check key length */
		if (!((hdr->key_len == 2 * KEY_SIZE_BYTES / 4) ||
			(hdr->key_len == 2 * KEY_SIZE_BYTES / 2) ||
			(hdr->key_len == 2 * KEY_SIZE_BYTES)))
			return ERROR_ESBC_CLIENT_HEADER_KEY_LEN;

		/* check signature length */
		if (!((hdr->sign_len == KEY_SIZE_BYTES / 4) ||
			(hdr->sign_len == KEY_SIZE_BYTES / 2) ||
			(hdr->sign_len == KEY_SIZE_BYTES)))
			return ERROR_ESBC_CLIENT_HEADER_SIG_LEN;
	} else {
		return ERROR_ESBC_CLIENT_HEADER_KEY_LEN_NOT_TWICE_SIG_LEN;
	}

	memcpy(&img->img_key, esbc + hdr->pkey, hdr->key_len);
	memcpy(&img->img_sign, esbc + hdr->psign, hdr->sign_len);

	/* No SG support */
	if (hdr->sg_flag)
		return ERROR_ESBC_CLIENT_HEADER_SG;

	/* modulus most significant bit should be set */
	k = (u8 *) &img->img_key;

	if ((k[0] & 0x80) == 0)
		return ERROR_ESBC_CLIENT_HEADER_KEY_MOD_1;

	/* modulus value should be odd */
	if ((k[hdr->key_len / 2 - 1] & 0x1) == 0)
		return ERROR_ESBC_CLIENT_HEADER_KEY_MOD_2;

	/* Check signature value < modulus value */
	s = (u8 *) &img->img_sign;

	if (!(memcmp(s, k, hdr->sign_len) < 0))
		return ERROR_ESBC_CLIENT_HEADER_SIG_KEY_MOD;

	return ESBC_VALID_HDR;
}

static inline int str2long(const char *p, ulong *num)
{
	char *endptr;

	if (!p)
		return 0;
	else
		*num = simple_strtoul(p, &endptr, 16);

	return *p != '\0' && *endptr == '\0';
}

int fsl_secboot_validate(cmd_tbl_t *cmdtp, int flag, int argc,
		char * const argv[])
{
	int hash_srk = 1;
	ccsr_sfp_regs_t *sfp_regs = (void *)(CONFIG_SYS_SFP_ADDR);
	ulong hash[SHA256_BYTES/sizeof(ulong)];
	char hash_str[NUM_HEX_CHARS + 1];
	sha256_context ctx;
	ulong addr = simple_strtoul(argv[1], NULL, 16);
	struct fsl_secboot_img_priv *img;
	struct fsl_secboot_img_hdr *hdr;
	void *esbc;
	int ret, i;
	u32 srk_hash[8];

	if (argc == 3) {
		char *cp = argv[2];
		int i = 0;

		if (*cp == '0' && *(cp + 1) == 'x')
			cp += 2;

		/* The input string expected is in hex, where
		 * each 4 bits would be represented by a hex
		 * sha256 hash is 256 bits long, which would mean
		 * num of characters = 256 / 4
		 */
		if (strlen(cp) != SHA256_NIBBLES) {
			printf("%s is not a 256 bits hex string as expected\n",
				argv[2]);
			return -1;
		}

		for (i = 0; i < sizeof(hash)/sizeof(ulong); i++) {
			strncpy(hash_str, cp + (i * NUM_HEX_CHARS),
				NUM_HEX_CHARS);
			hash_str[NUM_HEX_CHARS] = '\0';
			if (!str2long(hash_str, &hash[i])) {
				printf("%s is not a 256 bits hex string "
					"as expected\n", argv[2]);
				return -1;
			}
		}

		hash_srk = 0;
	}

	img = malloc(sizeof(struct fsl_secboot_img_priv));

	if (!img)
		return -1;

	memset(img, 0, sizeof(struct fsl_secboot_img_priv));

	hdr = &img->hdr;
	img->ehdrloc = addr;
	esbc = (u8 *) img->ehdrloc;

	memcpy(hdr, esbc, sizeof(struct fsl_secboot_img_hdr));

	/* read and validate esbc header */
	ret = read_validate_esbc_client_header(img);

	if (ret != ESBC_VALID_HDR) {
		fsl_secboot_handle_error(ret);
		return 0;
	}

	/* SRKH present in SFP */
	for (i = 0; i < NUM_SRKH_REGS; i++)
		srk_hash[i] = in_be32(&sfp_regs->srk_hash[i]);

	/*
	 * Calculate hash of key obtained via offset present in
	 * ESBC uboot client hdr
	 */
	calc_img_key_hash(&ctx, img);

	/* Compare hash obtained above with SRK hash present in SFP */
	if (hash_srk)
		ret = memcmp(srk_hash, img->img_key_hash, SHA256_BYTES);
	else
		ret = memcmp(&hash, &img->img_key_hash, SHA256_BYTES);

	if (ret != 0) {
		fsl_secboot_handle_error(ERROR_ESBC_CLIENT_HASH_COMPARE_KEY);
		return 0;
	}

	/* Calculate hash of ESBC hdr and ESBC */
	calc_esbchdr_esbc_hash(&ctx, img);

	/* Construct encoded hash EM' wrt PKCSv1.5 */
	construct_img_encoded_hash_second(img);

	/* Signature verification */
	rsa_public_verif(img->img_sign, img->img_encoded_hash, img->img_key,
				img->hdr.key_len);

	/*
	 * compare the encoded messages EM' and EM wrt RSA PKCSv1.5
	 * memcmp returns zero on success
	 * memcmp returns non-zero on failure
	 */
	ret = memcmp(&img->img_encoded_hash_second, &img->img_encoded_hash,
		img->hdr.sign_len);

	if (ret) {
		fsl_secboot_handle_error(ERROR_ESBC_CLIENT_HASH_COMPARE_EM);
		return 0;
	}

	printf("esbc_validate command successful\n");

	return 0;
}
