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

#ifndef _FSL_SECBOOT_ERR_H
#define _FSL_SECBOOT_ERR_H

#define ERROR_ESBC_CLIENT_CPUID_NO_MATCH			0x1
#define ERROR_ESBC_CLIENT_HDR_LOC				0x2
#define ERROR_ESBC_CLIENT_HEADER_BARKER				0x4
#define ERROR_ESBC_CLIENT_HEADER_KEY_LEN			0x8
#define ERROR_ESBC_CLIENT_HEADER_SIG_LEN			0x10
#define ERROR_ESBC_CLIENT_HEADER_KEY_LEN_NOT_TWICE_SIG_LEN	0x20
#define ERROR_ESBC_CLIENT_HEADER_KEY_MOD_1			0x40
#define ERROR_ESBC_CLIENT_HEADER_KEY_MOD_2			0x80
#define ERROR_ESBC_CLIENT_HEADER_SIG_KEY_MOD			0x100
#define ERROR_ESBC_CLIENT_HEADER_SG_ESBC_EP			0x200
#define ERROR_ESBC_CLIENT_HASH_COMPARE_KEY			0x400
#define ERROR_ESBC_CLIENT_HASH_COMPARE_EM			0x800
#define ERROR_ESBC_CLIENT_SSM_TRUSTSTS				0x1000
#define ERROR_ESBC_CLIENT_BAD_ADDRESS				0x2000
#define ERROR_ESBC_CLIENT_MISC					0x4000
#define ERROR_ESBC_CLIENT_HEADER_SG_ENTIRES_BAD			0x8000
#define ERROR_ESBC_CLIENT_HEADER_SG				0x10000
#define ERROR_ESBC_WRONG_CMD					0x20000
#define ERROR_ESBC_MISSING_BOOTM				0x40000
#define ERROR_ESBC_CLIENT_MAX					0x0

struct fsl_secboot_errcode {
	int errcode;
	const char *name;
};

static const struct fsl_secboot_errcode fsl_secboot_errcodes[] = {
	{ ERROR_ESBC_CLIENT_CPUID_NO_MATCH,
		"Current core is not boot core i.e core0" },
	{ ERROR_ESBC_CLIENT_HDR_LOC,
		"Header address not in allowed memory range" },
	{ ERROR_ESBC_CLIENT_HEADER_BARKER,
		"Wrong barker code in header" },
	{ ERROR_ESBC_CLIENT_HEADER_KEY_LEN,
		"Wrong public key length in header" },
	{ ERROR_ESBC_CLIENT_HEADER_SIG_LEN,
		"Wrong signature length in header" },
	{ ERROR_ESBC_CLIENT_HEADER_KEY_LEN_NOT_TWICE_SIG_LEN,
		"Public key length not twice of signature length" },
	{ ERROR_ESBC_CLIENT_HEADER_KEY_MOD_1,
		"Public key Modulus most significant bit not set" },
	{ ERROR_ESBC_CLIENT_HEADER_KEY_MOD_2,
		"Public key Modulus in header not odd" },
	{ ERROR_ESBC_CLIENT_HEADER_SIG_KEY_MOD,
		"Signature not less than modulus" },
	{ ERROR_ESBC_CLIENT_HEADER_SG_ESBC_EP,
		"Entry point not in allowed space or one of the SG entries" },
	{ ERROR_ESBC_CLIENT_HASH_COMPARE_KEY,
		"Public key hash comparison failed" },
	{ ERROR_ESBC_CLIENT_HASH_COMPARE_EM,
		"RSA verification failed" },
	{ ERROR_ESBC_CLIENT_SSM_TRUSTSTS,
		"SNVS not in TRUSTED state" },
	{ ERROR_ESBC_CLIENT_BAD_ADDRESS,
		"Bad address error" },
	{ ERROR_ESBC_CLIENT_MISC,
		"Miscallaneous error" },
	{ ERROR_ESBC_CLIENT_HEADER_SG,
		"No SG support"  },
	{ ERROR_ESBC_WRONG_CMD,
		"Failure in command/Unknown command/Wrong arguments," \
		" Core in infinite loop"},
	{ ERROR_ESBC_MISSING_BOOTM,
		"Bootm command missing from bootscript"},
	{ ERROR_ESBC_CLIENT_MAX, "NULL" }
};

#endif
