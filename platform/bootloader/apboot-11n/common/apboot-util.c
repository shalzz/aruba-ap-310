// vim:set ts=4 sw=4:
#include <config.h>
#ifdef CONFIG_APBOOT
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Aruba AP boot functions
 */

#include <common.h>
#include <command.h>
#include <lstring.h>
#include <flash.h>
#include <image.h>
#ifdef CONFIG_CMD_NAND
#include <nand.h>
#endif
#include <aruba_image.h>
#include <aruba_apboot.h>
#include <watchdog.h>
#include <asm-mips/global_data.h>
#include "net.h"
#include "md5.h"
#include "md5.h"
#ifdef CONFIG_BZIP2
#include "bzlib.h"
#endif

#if defined(CONFIG_OCTEON)
extern int do_bootoctlinux(cmd_tbl_t *, int, int, char **);
#elif defined(ARUBA_ARM) || defined(powerpc) || defined(CONFIG_IPROC) ||defined(CONFIG_MACH_IPQ806x)
extern int do_bootm (cmd_tbl_t *, int, int, char *[]);
#else
extern void do_bootm_linux(cmd_tbl_t *, int, int, char **, ulong, ulong *, int);
#endif

#ifndef CONFIG_STAGE1
int
boot_aruba_image(unsigned addr)
{
    char abuf[32];
	char *av[5];
	int r;

#ifdef powerpc
	snprintf(abuf,sizeof(abuf), "0x%x#%s", addr, ARUBA_BOARD_TYPE);
#else
	snprintf(abuf,sizeof(abuf), "0x%x", addr);
#endif
#ifdef CONFIG_OCTEON
	av[0] = "bootoctlinux";
	av[1] = abuf;
	av[2] = 0;

	r =  do_bootoctlinux(0, 0, 2, av);
#else
	av[0] = "bootm";
	av[1] = abuf;
	av[2] = 0;

#if defined(ARUBA_ARM) || defined(powerpc) || defined(CONFIG_IPROC) || defined(CONFIG_MACH_IPQ806x)
	// do_bootm() won't return, unlike do_bootoctlinux()
	(void) do_bootm(0, 0, 2, av);
#else
	// do_bootm_linux() won't return, unlike do_bootoctlinux()
	(void) do_bootm_linux(0, 0, 2, av, addr, 0, 0);
#endif
	/* NOTREACHED */
	r = 1;	// make gcc happy
#endif
	return r;
}
#endif /* CONFIG_STAGE1 */

// code 2 is special for network booting
int
aruba_basic_image_verify(
	void *load_addr, int machine, int kind, unsigned *lenp, int copy_hdr)
{
	aruba_image_t *ai;
	int i;
	unsigned *faddr;
	unsigned cksum = 0, nwords;
	unsigned store_len;

	if (lenp) {
		*lenp = 0;
	}

	WATCHDOG_RESET();

	ai = (aruba_image_t *)load_addr;
	if (ai->formatVer != htonl(ARUBA_IMAGE_STRUCT_VER)) {
		printf("Invalid image format version: 0x%x\n", ntohl(ai->formatVer));
		return 2;
	}
	if (ai->machineType != machine) {
#ifdef __SECONDARY_OS_IMAGE_TYPE
		if (ai->machineType == __SECONDARY_OS_IMAGE_TYPE) {
			goto good;
		}
#ifdef __COMPATIBLE_OS_IMAGE_TYPE
		if (ai->machineType == __COMPATIBLE_OS_IMAGE_TYPE) {
			goto good;
		}
#endif
#endif
		printf("Invalid machine type in image: 0x%x\n", ai->machineType);
		return 2;
	}

#ifdef __SECONDARY_OS_IMAGE_TYPE
good:
#endif

	if (ai->type != kind) {
		printf("Invalid image type: 0x%x\n", ai->type);
		return 2;
	}

	store_len = ntohl(ai->image_size);

	if (ai->chksum) {
		if (ai->nextHeaderID == htonl(ARUBA_IMAGE_NEXT_HEADER_SIGN)) {
			printf("\nImage is signed; ");
			nwords = ntohl(ai->image_size) + ARUBA_HEADER_SIZE;
			//
			// we have to store the whole image, with the header and the signature
			// trailer
			//
			store_len = ntohl(ai->size) + ARUBA_HEADER_SIZE;
		} else {
			printf("\nImage is not signed; ");
			nwords = ntohl(ai->size) + ARUBA_HEADER_SIZE;
			// store the image and the header
			store_len = nwords;
		}
		if (!copy_hdr) {
			//
			// for images that must be stored w/o the
			// header, also remove the signature to
			// save space
			//
			if (ai->nextHeaderID == htonl(ARUBA_IMAGE_NEXT_HEADER_SIGN)) {
				// use the image size length, not the total length
				store_len = ntohl(ai->image_size);
			} else {
				//
				// use the old size field in case this image was built with an
				// old addVersion utility
				//
				store_len = ntohl(ai->size);
			}
		}

		if (ai->comp == ARUBA_IMAGE_COMPRESSION_BZIP2) {
			//
			// The compressed file might not be a multiple of 4,
			// but addVersion will pad it, which is good.
			// For checksum purposes, assume this padding exists,
			// even though the length may not account for it.  We
			// can't just bump the length, as this will hose the
			// unzip code
			//
			while (nwords & 3) {
				nwords++;
			}
			//
			// also bump length to be copied in this case, since we assume
			// that there will be a header on it
			//
			while (store_len & 3) {
				store_len++;
			}
		}
		nwords /= sizeof(unsigned);

		faddr = (unsigned *)ai;
		printf("verifying checksum... ");
		for (i = 0; i < nwords; i++) {
			WATCHDOG_RESET();
			cksum += ntohl(*faddr++);
		}
		if (cksum) {
			printf("failed! 0x%x\n", cksum);
			return 2;
		}
		printf("passed\n");
	}

#if defined(CONFIG_MD5SUM)
	for (i = 0; i < 16; i++) {
		if (ai->md5_chksum[i] != 0) {
			break;
		}
	}
	if (i < 16) {
		aruba_image_t hdrcopy = *ai;
		MD5_CTX md5ctxt;
		unsigned char md5sum[16];

		printf("Verifying MD5 sum... ");
		hdrcopy.chksum = 0;
		memset(hdrcopy.md5_chksum, 0, 16);
		MD5Init(&md5ctxt);
		MD5Update(&md5ctxt, (unsigned char *) &hdrcopy, sizeof(aruba_image_t));
		// XXX; does this need the same FIPS ifdef?
		MD5Update(&md5ctxt, load_addr + sizeof(aruba_image_t), ntohl(ai->size));
		MD5Final(md5sum, &md5ctxt);
		if (memcmp(md5sum, ai->md5_chksum, 16) != 0) {
			printf("failed!\n");
			return 2;
		}
		printf("passed\n");
	}
#endif

	if (lenp) {
		*lenp = store_len;
	}

	return 0;
}

char *
aruba_osinfo_flags(unsigned flags)
{
	static char fbuf[64];

	fbuf[0] = '\0';

	if (flags & ARUBA_IMAGE_FLAGS_C_TEST_BUILD) {
		strlcat(fbuf, "C-build ", sizeof(fbuf));
	}
	if (flags & ARUBA_IMAGE_FLAGS_SWATCH) {
		strlcat(fbuf, "Instant ", sizeof(fbuf));
	}
	if (flags & ARUBA_IMAGE_FLAGS_DONT_CLEAR_ON_PURGE) {
		strlcat(fbuf, "preserve ", sizeof(fbuf));
	}
	if (flags & ARUBA_IMAGE_FLAGS_FACTORY_IMAGE) {
		strlcat(fbuf, "factory ", sizeof(fbuf));
	}
	flags &= ~(ARUBA_IMAGE_FLAGS_C_TEST_BUILD|ARUBA_IMAGE_FLAGS_SWATCH|ARUBA_IMAGE_FLAGS_DONT_CLEAR_ON_PURGE|ARUBA_IMAGE_FLAGS_FACTORY_IMAGE);
	if (flags) {
		char buf[16];
		snprintf(buf, sizeof(buf), "%04x", flags);
		strlcat(fbuf, buf, sizeof(fbuf));
	}
	return fbuf;
}

#if defined(CONFIG_CMD_UBI)
/*
 * maddr: memory address
 * faddr: flash offset address
 * size: the length
 * rw: 1: read, 0 write
 */
void *
aruba_ubi_image(void *maddr, void *faddr, unsigned size, unsigned rw)
{
    char dbuf[32], sbuf[32];
    char *av[6];
    int r;

    /* For Octomore, we have to load the image from UBI part to memory */
    snprintf(dbuf, sizeof(dbuf), "0x%x", (unsigned)maddr);

    av[0] = "ubi";

    /* set ubi parts first */
    av[1] = "part";

    if ((unsigned)faddr == AP_PRODUCTION_IMAGE) {
        av[2] = AP_PRODUCTION_IMAGE_NAME;
    } else {
        av[2] = AP_PROVISIONING_IMAGE_NAME;
    }
    av[3] = 0;

    r = do_ubi(0, 1, 3, av);

    if (rw) {
        av[1] = "read";
    } else {
        av[1] = "write";
    }
    av[2] = dbuf;

    if ((unsigned)faddr == AP_PRODUCTION_IMAGE) {
        av[3] = AP_PRODUCTION_IMAGE_NAME;
    } else {
        av[3] = AP_PROVISIONING_IMAGE_NAME;
    }
    snprintf(sbuf, sizeof(sbuf), "0x%x", size);
    av[4] = sbuf;
    av[5] = 0;
    
    r = do_ubi(0, 1, 5, av);
    if (!r) {
        faddr = maddr;
    }
    
    return faddr;
}
#endif

#ifdef AP_SLOW_FLASH_STAGING_AREA
void *
aruba_copy_to_staging_area(void *src, void *dest, unsigned size, unsigned quiet)
{
	aruba_image_t *ai;
	unsigned store_len;

#if defined(CONFIG_CMD_UBI)
    void *old_src = src;

    src = aruba_ubi_image(dest, src, ARUBA_HEADER_SIZE, 1);
#endif
	ai = (aruba_image_t *)src;

	if (ai->formatVer != htonl(ARUBA_IMAGE_STRUCT_VER)) {
		return src;
	}
	if (ai->machineType != __OS_IMAGE_TYPE) {
#ifdef __SECONDARY_OS_IMAGE_TYPE
		if (ai->machineType == __SECONDARY_OS_IMAGE_TYPE) {
			goto good;
		}
#ifdef __COMPATIBLE_OS_IMAGE_TYPE
		if (ai->machineType == __COMPATIBLE_OS_IMAGE_TYPE) {
			goto good;
		}
#endif
#endif
		return src;
	}

#ifdef __SECONDARY_OS_IMAGE_TYPE
good:
#endif

	if (ai->type != ARUBA_IMAGE_TYPE_ELF) {
		return src;
	}

	store_len = ntohl(ai->size) + ARUBA_HEADER_SIZE;

	if (store_len > size) {
		return src;
	}

	if (!quiet) {
		printf("Copying image from 0x%p\n", src);
	}

    WATCHDOG_RESET();
#if defined(USE_FLASH_READ_SUPPORT) || defined(ARUBA_ARM) || defined(CONFIG_IPROC)
#if defined(CONFIG_CMD_UBI)
    aruba_ubi_image(dest, old_src, store_len, 1);
#else
	aruba_flash_read(src, dest, store_len);
#endif
#else
	{
		size_t l = store_len;
		void *to = (void *)dest;
		void *from = (void *)src;

		while (l > 0) {
			size_t tail = (l > CHUNKSZ) ? CHUNKSZ : l;
			WATCHDOG_RESET();
			memmove (to, from, tail);
			to += tail;
			from += tail;
			l -= tail;
		}
	}
#endif

	return dest;

}
#endif	/* AP_SLOW_FLASH_STAGING_AREA */

static void
aruba_invalidate_dcache(void)
{
#ifdef CONFIG_IPROC
	extern void l2cc_invalidate_and_wait(void);

	invalidate_dcache_all();
	l2cc_invalidate_and_wait();
#endif
}

#ifdef CONFIG_CMD_NAND
extern int nand_mfg_device;

static int
aruba_flash_verify(ulong src, ulong dst, unsigned len, int flag)
{
    unsigned char *buf;
    int i, ret = 0;
    unsigned char *p;

    p = (unsigned char *)src;

    buf = (unsigned char *)CONFIG_VERIFY_LOAD_ADDR;
    if (!flag) {/* read form RAW flash */
        ret = nand_read_skip_bad(&nand_info[nand_mfg_device], dst, &len, buf);
    } else { /* read from UBI */
#ifdef CONFIG_CMD_UBI
        aruba_ubi_image((void *)buf, (void *)dst, len, 1);
#endif
    }
    for (i = 0; i < len; i++, p++, buf++) {
        if (*buf != *p) {
            printf("byte at 0x%08lx (0x%02x) "
                    "!= byte (0x%02x)\n",
                    p, *p, *buf);
            return 1;
        }
    }

    return ret;
}
#endif
int
aruba_copy_and_verify(
	cmd_tbl_t *cmdtp, int flag, ulong src, ulong dst, ulong end, unsigned len,
	int copy_hdr, int protected
)
{
	int r = 0;
	char sbuf[32], ebuf[32];
#ifndef CONFIG_CMD_NAND
    char rbuf[32], cbuf[32];
#endif
	char *av[6];
	int need_restore = 0;
#ifdef WORD_COPY_COMPARE
	int remainder = 0;
	char sbuf_rem[32], rbuf_rem[32], cbuf_rem[32];
#endif

	snprintf(sbuf, sizeof(sbuf), "0x%x", dst);
	snprintf(ebuf, sizeof(ebuf), "0x%x", end);

	if (protected) {
		printf("Unprotecting flash... ");
		av[0] = "protect";
		av[1] = "off";
		av[2] = sbuf;
		av[3] = ebuf;
		av[4] = 0;

		r = do_protect(cmdtp, flag, 4, av);
		if (r != 0) {
			goto out;
		}

		need_restore = 1;
	}

#ifdef CONFIG_CMD_UBI
    if (dst == AP_PRODUCTION_IMAGE || dst == AP_PROVISIONING_IMAGE) {
        goto write_flash;
    }
#endif
	av[0] = "erase";
	av[1] = sbuf;
	av[2] = ebuf;
	av[3] = 0;
	printf("Erasing flash sector @ %s...", sbuf);
	r = do_flerase(cmdtp, flag, 3, av);
#ifdef WORD_COPY_COMPARE
	av[0] = "cp.l";
#else
	av[0] = "cp.b";
#endif

	if (!copy_hdr) {
		src += ARUBA_HEADER_SIZE;
	}

#ifdef CONFIG_CMD_NAND
write_flash:    
    if (dst == CFG_MANUF_BASE || dst == APBOOT_IMAGE) {
        printf("Writing to flash...");
        /* Here assume apboot and mfg inf are in same flash */
        r = nand_write_skip_bad(&nand_info[nand_mfg_device], dst, &len, (u_char *)src, 0);

        if (r) {
            printf("failed\n");
            return r;
        } else {
            printf("OK\n");
        }
        aruba_invalidate_dcache();
        printf("Verifying flash... \n");
        r = aruba_flash_verify(src, dst, len, 0);
    }else if(dst == AP_PRODUCTION_IMAGE || dst == AP_PROVISIONING_IMAGE) {
#ifdef CONFIG_CMD_UBI
        aruba_ubi_image((void *)src, (void *)dst, len, 0);
        aruba_invalidate_dcache();
        printf("Verifying flash... \n");
        r = aruba_flash_verify(src, dst, len, 1);
#endif
    } else { /* program APBOOT */
        
        printk("Copy to UBI sectore: not supported yet\n");
    }
#else
	av[1] = rbuf;
	av[2] = sbuf;
	av[3] = cbuf;
	av[4] = 0;

	sprintf(rbuf, "0x%x", src);
#ifdef WORD_COPY_COMPARE

	remainder = len % 4;
    len = len/4;
#endif
	sprintf(cbuf, "0x%x", len);

	r = do_mem_cp(cmdtp, flag, 4, av);

#ifdef WORD_COPY_COMPARE
    if (remainder > 0) {
        sprintf(rbuf_rem, "0x%x", src + len*4);
        snprintf(sbuf_rem, sizeof(sbuf), "0x%x", dst + len*4);
        sprintf(cbuf_rem, "0x%x", remainder);
        /* copy the remaining bytes */
        av[0] = "cp.b";
        av[1] = rbuf_rem;
        av[2] = sbuf_rem;
        av[3] = cbuf_rem;
        av[4] = 0;
        r = do_mem_cp(cmdtp, flag, 4, av);
    }
#endif

#ifdef WORD_COPY_COMPARE
	av[0] = "cmp.l";
#else
	av[0] = "cmp.b";
#endif
	av[1] = rbuf;
	av[2] = sbuf;
	av[3] = cbuf;
	av[4] = 0;

	aruba_invalidate_dcache();

	printf("Verifying flash... ");
	r |= do_mem_cmp(cmdtp, flag, 4, av);
#ifdef WORD_COPY_COMPARE
    if (remainder > 0) {
        /* compare the remaining bytes */
        av[0] = "cmp.b";
        av[1] = rbuf_rem;
        av[2] = sbuf_rem;
        av[3] = cbuf_rem;
        av[4] = 0;

        r |= do_mem_cmp(cmdtp, flag, 4, av);
    }
#endif

#endif
	// restore protection
	if (need_restore) {
		printf("Protecting flash... ");
		av[0] = "protect";
		av[1] = "on";
		av[2] = sbuf;
		av[3] = ebuf;
		av[4] = 0;
		(void)do_protect(cmdtp, flag, 4, av);
	}
out:
	return r;
}

#if defined(CONFIG_USB_STORAGE) && defined(CONFIG_CMD_FAT) && defined(powerpc)
int
load_image_from_usb(void *loadaddr, char *path)
{
	extern char usb_started;
	extern int do_usb(void *, int, int, char **);
	extern int do_fat_fsload(void *, int, int, char **);
	char *av[8];
	int r;
	char lbuf[16];
	char sbuf[16];

#if 0
	av[0] = "usb";
	av[1] = "stop";
	av[2] = 0;
	do_usb(0, 0, 2, av);
#endif

	if (!usb_started) {
		av[0] = "usb";
		av[1] = "start";
		av[2] = 0;
		r = do_usb(0, 0, 2, av);
		if (r) {
			return 1;
		}
	}

	snprintf(lbuf, sizeof(lbuf), "0x%x", loadaddr);
	snprintf(sbuf, sizeof(sbuf), "0x%x", AP_PRODUCTION_IMAGE_SIZE);
	av[0] = "fatload";
	av[1] = "usb";
	av[2] = "0";
	av[3] = lbuf;
	av[4] = path;
	av[5] = sbuf;
	av[6] = 0;

	r = do_fat_fsload(0, 0, 6, av);
	if (r) {
		return 1;
	}
	return 0;
}
#endif	/* USB */

#endif // CONFIG_APBOOT
