/*
 * Cryptographic API.
 *
 * SHA1 Secure Hash Algorithm.
 *
 * Derived from cryptoapi implementation, adapted for in-place
 * scatterlist interface.
 *
 * Copyright (c) Alan Smithee.
 * Copyright (c) Andrew McDonald <andrew@mcdonald.org.uk>
 * Copyright (c) Jean-Francois Dive <jef@linuxbe.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 */
#include <crypto/internal/hash.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/cryptohash.h>
#include <linux/types.h>
#include <crypto/sha.h>
#include <asm/byteorder.h>

#ifdef CONFIG_ARUBA_AP
#include <linux/workqueue.h>
extern void aruba_save_kernel_kat_reboot_reason(const char *reason);
extern void kernel_restart(char *cmd);
static void fips_selftest_sha1(struct work_struct *);
static DECLARE_WORK(fips_sha1_kat_work, fips_selftest_sha1);
#endif

static int sha1_init(struct shash_desc *desc)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);

	*sctx = (struct sha1_state){
		.state = { SHA1_H0, SHA1_H1, SHA1_H2, SHA1_H3, SHA1_H4 },
	};

	return 0;
}

int crypto_sha1_update(struct shash_desc *desc, const u8 *data,
			unsigned int len)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);
	unsigned int partial, done;
	const u8 *src;

	partial = sctx->count % SHA1_BLOCK_SIZE;
	sctx->count += len;
	done = 0;
	src = data;

	if ((partial + len) >= SHA1_BLOCK_SIZE) {
		u32 temp[SHA_WORKSPACE_WORDS];

		if (partial) {
			done = -partial;
			memcpy(sctx->buffer + partial, data,
			       done + SHA1_BLOCK_SIZE);
			src = sctx->buffer;
		}

		do {
			sha_transform(sctx->state, src, temp);
			done += SHA1_BLOCK_SIZE;
			src = data + done;
		} while (done + SHA1_BLOCK_SIZE <= len);

		memset(temp, 0, sizeof(temp));
		partial = 0;
	}
	memcpy(sctx->buffer + partial, src, len - done);

	return 0;
}
EXPORT_SYMBOL(crypto_sha1_update);


/* Add padding and return the message digest. */
static int sha1_final(struct shash_desc *desc, u8 *out)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);
	__be32 *dst = (__be32 *)out;
	u32 i, index, padlen;
	__be64 bits;
	static const u8 padding[64] = { 0x80, };

	bits = cpu_to_be64(sctx->count << 3);

	/* Pad out to 56 mod 64 */
	index = sctx->count & 0x3f;
	padlen = (index < 56) ? (56 - index) : ((64+56) - index);
	crypto_sha1_update(desc, padding, padlen);

	/* Append length */
	crypto_sha1_update(desc, (const u8 *)&bits, sizeof(bits));

	/* Store state in digest */
	for (i = 0; i < 5; i++)
		dst[i] = cpu_to_be32(sctx->state[i]);

	/* Wipe context */
	memset(sctx, 0, sizeof *sctx);

	return 0;
}

static int sha1_export(struct shash_desc *desc, void *out)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);

	memcpy(out, sctx, sizeof(*sctx));
	return 0;
}

static int sha1_import(struct shash_desc *desc, const void *in)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);

	memcpy(sctx, in, sizeof(*sctx));
	return 0;
}

#ifdef CONFIG_ARUBA_AP
/*Idealy this is needed only for FIPS ArubaOS AP build
 * but it doesnt harm to execute self test on all Regular
 * ArubaOS AP build*/

extern char* fips_fail_test;

static const u8 plaintext[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" ;

static const u8 digest[20] = {
	0x84, 0x98, 0x3e, 0x44, 0x1c, 0x3b, 0xd2, 0x6e, 0xba, 0xae,
	0x4a, 0xa1, 0xf9, 0x51, 0x29, 0xe5, 0xe5, 0x46, 0x70, 0xf1
};

static void fips_selftest_sha1(struct work_struct *w)
{
	struct shash_desc *desc;
	unsigned char buf[20];
    int fail_test = 0;

	desc = kmalloc((sizeof(*desc) + sizeof(struct sha1_state)) * 2, GFP_KERNEL); // align
	if (!desc) {
		aruba_save_kernel_kat_reboot_reason("failed to allocate memory for SHA-1 context\n");
		kernel_restart(NULL);
	}
	desc->flags = 0;
	desc->tfm = crypto_alloc_shash("sha1", 0, 0);

	if (!desc->tfm) {
		aruba_save_kernel_kat_reboot_reason("failed to allocate memory for SHA-1 transform\n");
		kernel_restart(NULL);
	}

	printk("\nStarting Kernel SHA1 KAT ...");
	sha1_init(desc);
	crypto_sha1_update(desc, plaintext, strlen(plaintext));
	sha1_final(desc, buf);

    if(fips_fail_test && !memcmp(fips_fail_test, "sha1", 4))
        fail_test = 1;
	if(memcmp(buf, digest, sizeof(digest)) || fail_test)
	{
		printk("Failed Kernel SHA1 KAT \n");
		aruba_save_kernel_kat_reboot_reason("SHA1 Failed");
		kernel_restart(NULL);
	}
	crypto_free_shash(desc->tfm);
	kfree(desc);
	printk("Completed Kernel SHA1 KAT \n");
}

void handle_sha1_selftest(void)
{
	schedule_work(&fips_sha1_kat_work);
}
EXPORT_SYMBOL(handle_sha1_selftest);
#endif /* #ifdef CONFIG_ARUBA_AP */

static struct shash_alg alg = {
	.digestsize	=	SHA1_DIGEST_SIZE,
	.init		=	sha1_init,
	.update		=	crypto_sha1_update,
	.final		=	sha1_final,
	.export		=	sha1_export,
	.import		=	sha1_import,
	.descsize	=	sizeof(struct sha1_state),
	.statesize	=	sizeof(struct sha1_state),
	.base		=	{
		.cra_name	=	"sha1",
		.cra_driver_name=	"sha1-generic",
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA1_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
};

static int __init sha1_generic_mod_init(void)
{
	return crypto_register_shash(&alg);
}

static void __exit sha1_generic_mod_fini(void)
{
	crypto_unregister_shash(&alg);
}

module_init(sha1_generic_mod_init);
module_exit(sha1_generic_mod_fini);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SHA1 Secure Hash Algorithm");

MODULE_ALIAS("sha1");
