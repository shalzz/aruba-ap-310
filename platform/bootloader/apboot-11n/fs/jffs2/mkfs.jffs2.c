/* vi: set sw=4 ts=4: */
/*
 * Build a JFFS2 image in a file, from a given directory tree.
 *
 * Copyright 2001, 2002 Red Hat, Inc.
 *           2001 David A. Schleef <ds@lineo.com>
 *           2002 Axis Communications AB
 *           2001, 2002 Erik Andersen <andersen@codepoet.org>
 *           2004 University of Szeged, Hungary
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Cross-endian support added by David Schleef <ds@schleef.org>.
 *
 * Major architectural rewrite by Erik Andersen <andersen@codepoet.org>
 * to allow support for making hard links (though hard links support is
 * not yet implemented), and for munging file permissions and ownership
 * on the fly using --faketime, --squash, --devtable.   And I plugged a
 * few memory leaks, adjusted the error handling and fixed some little
 * nits here and there.
 *
 * I also added a sample device table file.  See device_table.txt
 *  -Erik, September 2001
 *
 * Cleanmarkers support added by Axis Communications AB
 *
 * Rewritten again.  Cleanly separated host and target filsystem
 * activities (mainly so I can reuse all the host handling stuff as I
 * rewrite other mkfs utils).  Added a verbose option to list types
 * and attributes as files are added to the file system.  Major cleanup
 * and scrubbing of the code so it can be read, understood, and
 * modified by mere mortals.
 *
 *  -Erik, November 2002
 */

#include <common.h>
#include <asm/byteorder.h>
#include <exports.h>
#include <linux/stat.h>
#include <linux/dirent.h>
#include <rtc.h>
#include <jffs2/nodelist.h>
#include <zlib.h>
#include "crc32.h"
#include <command.h>

#ifndef CONFIG_APBOOT
#define mkfs_debug_msg(a...)	{ }
#define MKFS_JFFS2_OBUF 0x2000000   // a blob of DRAM in kuseg we'll use for an output "file"

struct filesystem_entry {
	char *name;					/* Name of this directory (think basename) */
	char *path;					/* Path of this directory (think dirname) */
	char *fullname;				/* Full name of this directory (i.e. path+name) */
	char *hostname;				/* Full path to this file on the host filesystem */
	struct stat sb;				/* Stores directory permissions and whatnot */
	char *link;					/* Target a symlink points to. */
	struct filesystem_entry *parent;	/* Parent directory */
	struct filesystem_entry *prev;	/* Only relevant to non-directories */
	struct filesystem_entry *next;	/* Only relevant to non-directories */
	struct filesystem_entry *files;	/* Only relevant to directories */
};

static int verbose = 1;
static int squash_uids = 0;
static int squash_perms = 0;
static int fake_times = 1;
static int out_fd = 0;

/* Here is where we do the actual creation of the file system */

#define JFFS2_MAX_FILE_SIZE 0xFFFFFFFF
#ifndef JFFS2_MAX_SYMLINK_LEN
#define JFFS2_MAX_SYMLINK_LEN 254
#endif

static uint32_t ino = 0;
static int out_ofs = 0;
static int erase_block_size = 0x20000;
//static int erase_block_size = 65536;
static int pad_fs_size = 0;
static int add_ebhs = 0;
static struct jffs2_raw_ebh ebh;
static int ebh_size = sizeof(ebh);
static unsigned char ffbuf[16] =
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff
};

/* We default to 4096, per x86.  When building a fs for
 * 64-bit arches and whatnot, use the --pagesize=SIZE option */
//int page_size = 2048;
int page_size = 4096;

static void full_write(int fd, const void *buf, int len)
{
    memcpy((char*)(MKFS_JFFS2_OBUF+out_ofs), buf, len);
	out_ofs += len;
}

static void padblock(void)
{
	while (out_ofs % erase_block_size) {
		full_write(out_fd, ffbuf, min(sizeof(ffbuf),
					erase_block_size - (out_ofs % erase_block_size)));
	}
}

static void pad(int req)
{
	while (req) {
		if (req > sizeof(ffbuf)) {
			full_write(out_fd, ffbuf, sizeof(ffbuf));
			req -= sizeof(ffbuf);
		} else {
			full_write(out_fd, ffbuf, req);
			req = 0;
		}
	}
}

static inline void padword(void)
{
	if (out_ofs % 4) {
		full_write(out_fd, ffbuf, 4 - (out_ofs % 4));
	}
}

static inline void pad_block_if_less_than(int req)
{
	if (add_ebhs) {
		if ((out_ofs % erase_block_size) == 0) {
			full_write(out_fd, &ebh, sizeof(ebh));
			pad(ebh_size - sizeof(ebh));
			padword();
		}
	}
	if ((out_ofs % erase_block_size) + req > erase_block_size) {
		padblock();
	}
	if (add_ebhs) {
		if ((out_ofs % erase_block_size) == 0) {
			full_write(out_fd, &ebh, sizeof(ebh));
			pad(ebh_size - sizeof(ebh));
			padword();
		}
	}
}

static void write_dirent(struct filesystem_entry *e)
{
	char *name = e->name;
	struct jffs2_raw_dirent rd;
	struct stat *statbuf = &(e->sb);
	static uint32_t version = 0;

	memset(&rd, 0, sizeof(rd));

	rd.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	rd.nodetype = cpu_to_je16(JFFS2_NODETYPE_DIRENT);
	rd.totlen = cpu_to_je32(sizeof(rd) + strlen(name));
	rd.hdr_crc = cpu_to_je32(fs_crc32(0, &rd,
				sizeof(struct jffs2_unknown_node) - 4));
	rd.pino = cpu_to_je32((e->parent) ? e->parent->sb.st_ino : 1);
	rd.version = cpu_to_je32(version++);
	rd.ino = cpu_to_je32(statbuf->st_ino);
	rd.mctime = cpu_to_je32(statbuf->st_mtime);
	rd.nsize = strlen(name);
	rd.type = IFTODT(statbuf->st_mode);
	rd.node_crc = cpu_to_je32(fs_crc32(0, &rd, sizeof(rd) - 8));
	rd.name_crc = cpu_to_je32(fs_crc32(0, name, strlen(name)));

	pad_block_if_less_than(sizeof(rd) + rd.nsize);
	full_write(out_fd, &rd, sizeof(rd));
	full_write(out_fd, name, rd.nsize);
	padword();
}

extern unsigned long NetBootFileXferSize;
static void write_regular_file(struct filesystem_entry *e)
{
	int /*fd,*/ len;
	uint32_t ver;
	unsigned int offset;
	unsigned char *buf, *cbuf, *wbuf;
	struct jffs2_raw_inode ri;
	struct stat *statbuf;
    int o_ptr = 0, rd_size;

	statbuf = &(e->sb);
	if (statbuf->st_size >= JFFS2_MAX_FILE_SIZE) {
		printf("Skipping file \"%s\" too large.", e->path);
		return;
	}
    
	statbuf->st_ino = ++ino;
	printf("writing file '%s'  ino=%lu  parent_ino=%lu\n",
			e->name, (unsigned long) statbuf->st_ino,
			(unsigned long) e->parent->sb.st_ino);
	write_dirent(e);

	buf = malloc(page_size);
	cbuf = NULL;

	ver = 0;
	offset = 0;

	memset(&ri, 0, sizeof(ri));
	ri.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri.nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);

	ri.ino = cpu_to_je32(statbuf->st_ino);
	ri.mode = statbuf->st_mode;
	ri.uid = cpu_to_je16(statbuf->st_uid);
	ri.gid = cpu_to_je16(statbuf->st_gid);
	ri.atime = cpu_to_je32(statbuf->st_atime);
	ri.ctime = cpu_to_je32(statbuf->st_ctime);
	ri.mtime = cpu_to_je32(statbuf->st_mtime);
	ri.isize = cpu_to_je32(statbuf->st_size);

//	while ((len = read(fd, buf, page_size))) {   // serious HACK - we assume that we are only adding one regular file
                                                // and that we are reading it here.
	do {
		unsigned char *tbuf = buf;

        if((NetBootFileXferSize - o_ptr) >= page_size) {
            rd_size = page_size;
            //printf(".");
        } else {
            rd_size = NetBootFileXferSize - o_ptr;  // FUCKME - use the values passed not the globals
            //printf("last=%d\n",rd_size);
        }
            
        if(rd_size) {
            memcpy(buf, (char*)(load_addr+o_ptr),rd_size);
            o_ptr += rd_size;
        }
        len = rd_size;

		while (len) {
			uint32_t dsize, space;
//            uint16_t compression;

			pad_block_if_less_than(sizeof(ri) + JFFS2_MIN_DATA_LEN);
			dsize = len;
			space =	erase_block_size - (out_ofs % erase_block_size) - sizeof(ri);
			if (space > dsize)
				space = dsize;

//			compression = jffs2_compress(tbuf, &cbuf, &dsize, &space);
//                        ri.compr = compression & 0xff;
//                        ri.usercompr = (compression >> 8) & 0xff;
			if (ri.compr) {
				wbuf = cbuf;
			} else {
				wbuf = tbuf;
				dsize = space;
			}

			ri.totlen = cpu_to_je32(sizeof(ri) + space);
			ri.hdr_crc = cpu_to_je32(fs_crc32(0,
						&ri, sizeof(struct jffs2_unknown_node) - 4));

			ri.version = cpu_to_je32(++ver);
			ri.offset = cpu_to_je32(offset);
			ri.csize = cpu_to_je32(space);
			ri.dsize = cpu_to_je32(dsize);
			ri.node_crc = cpu_to_je32(fs_crc32(0, &ri, sizeof(ri) - 8));
			ri.data_crc = cpu_to_je32(fs_crc32(0, wbuf, space));

			full_write(out_fd, &ri, sizeof(ri));
			full_write(out_fd, wbuf, space);
			padword();

            if (tbuf != cbuf) {
				free(cbuf);
				cbuf = NULL;
			}

			tbuf += dsize;
			len -= dsize;
			offset += dsize;
		}
	}while(rd_size);
    
	if (!je32_to_cpu(ri.version)) {
        printf("Hit empty file\n");
		/* Was empty file */
		pad_block_if_less_than(sizeof(ri));

		ri.version = cpu_to_je32(++ver);
		ri.totlen = cpu_to_je32(sizeof(ri));
		ri.hdr_crc = cpu_to_je32(fs_crc32(0,
					&ri, sizeof(struct jffs2_unknown_node) - 4));
		ri.csize = cpu_to_je32(0);
		ri.dsize = cpu_to_je32(0);
		ri.node_crc = cpu_to_je32(fs_crc32(0, &ri, sizeof(ri) - 8));

		full_write(out_fd, &ri, sizeof(ri));
		padword();
	}
	free(buf);
}

#if 0
static void write_symlink(struct filesystem_entry *e)
{
	int len;
	struct stat *statbuf;
	struct jffs2_raw_inode ri;

	statbuf = &(e->sb);
	statbuf->st_ino = ++ino;
	mkfs_debug_msg("writing symlink '%s'  ino=%lu  parent_ino=%lu",
			e->name, (unsigned long) statbuf->st_ino,
			(unsigned long) e->parent->sb.st_ino);
	write_dirent(e);

	len = strlen(e->link);
	if (len > JFFS2_MAX_SYMLINK_LEN) {
		printf("symlink too large. Truncated to %d chars.",
				JFFS2_MAX_SYMLINK_LEN);
		len = JFFS2_MAX_SYMLINK_LEN;
	}

	memset(&ri, 0, sizeof(ri));

	ri.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri.nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);
	ri.totlen = cpu_to_je32(sizeof(ri) + len);
	ri.hdr_crc = cpu_to_je32(fs_crc32(0,
				&ri, sizeof(struct jffs2_unknown_node) - 4));

	ri.ino = cpu_to_je32(statbuf->st_ino);
	ri.mode = (statbuf->st_mode);
//	ri.mode = cpu_to_jemode(statbuf->st_mode);
	ri.uid = cpu_to_je16(statbuf->st_uid);
	ri.gid = cpu_to_je16(statbuf->st_gid);
	ri.atime = cpu_to_je32(statbuf->st_atime);
	ri.ctime = cpu_to_je32(statbuf->st_ctime);
	ri.mtime = cpu_to_je32(statbuf->st_mtime);
	ri.isize = cpu_to_je32(statbuf->st_size);
	ri.version = cpu_to_je32(1);
	ri.csize = cpu_to_je32(len);
	ri.dsize = cpu_to_je32(len);
	ri.node_crc = cpu_to_je32(fs_crc32(0, &ri, sizeof(ri) - 8));
	ri.data_crc = cpu_to_je32(fs_crc32(0, e->link, len));

	pad_block_if_less_than(sizeof(ri) + len);
	full_write(out_fd, &ri, sizeof(ri));
	full_write(out_fd, e->link, len);
	padword();
}
#endif

static void write_pipe(struct filesystem_entry *e)
{
	struct stat *statbuf;
	struct jffs2_raw_inode ri;

	statbuf = &(e->sb);
	statbuf->st_ino = ++ino;
	if (S_ISDIR(statbuf->st_mode)) {
		mkfs_debug_msg("writing dir '%s'  ino=%lu  parent_ino=%lu",
				e->name, (unsigned long) statbuf->st_ino,
				(unsigned long) (e->parent) ? e->parent->sb.  st_ino : 1);
	}
	write_dirent(e);

	memset(&ri, 0, sizeof(ri));

	ri.magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri.nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);
	ri.totlen = cpu_to_je32(sizeof(ri));
	ri.hdr_crc = cpu_to_je32(fs_crc32(0,
				&ri, sizeof(struct jffs2_unknown_node) - 4));

	ri.ino = cpu_to_je32(statbuf->st_ino);
	ri.mode = (statbuf->st_mode);
	ri.uid = cpu_to_je16(statbuf->st_uid);
	ri.gid = cpu_to_je16(statbuf->st_gid);
	ri.atime = cpu_to_je32(statbuf->st_atime);
	ri.ctime = cpu_to_je32(statbuf->st_ctime);
	ri.mtime = cpu_to_je32(statbuf->st_mtime);
	ri.isize = cpu_to_je32(0);
	ri.version = cpu_to_je32(1);
	ri.csize = cpu_to_je32(0);
	ri.dsize = cpu_to_je32(0);
	ri.node_crc = cpu_to_je32(fs_crc32(0, &ri, sizeof(ri) - 8));
	ri.data_crc = cpu_to_je32(0);

	pad_block_if_less_than(sizeof(ri));
	full_write(out_fd, &ri, sizeof(ri));
	padword();
}

static void recursive_populate_directory(struct filesystem_entry *dir)
{
	struct filesystem_entry *e;

	if (verbose) {
		printf("%s\n", dir->fullname);
	}
	e = dir->files;
	while (e) {

		switch (e->sb.st_mode & S_IFMT) {
			case S_IFDIR:
				if (verbose) {
					printf("\td %04o %9lu %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, e->sb.st_size,
							(int) (e->sb.st_uid), (int) (e->sb.st_gid),
							e->name);
				}
				write_pipe(e);
				break;
#if 0 // not at Aruba                
			case S_IFSOCK:
				if (verbose) {
					printf("\ts %04o %9lu %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, e->sb.st_size,
							(int) e->sb.st_uid, (int) e->sb.st_gid, e->name);
				}
				write_pipe(e);
				break;
			case S_IFIFO:
				if (verbose) {
					printf("\tp %04o %9lu %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, e->sb.st_size,
							(int) e->sb.st_uid, (int) e->sb.st_gid, e->name);
				}
				write_pipe(e);
				break;
			case S_IFCHR:
				if (verbose) {
					printf("\tc %04o %4d,%4d %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, major(e->sb.st_rdev),
							minor(e->sb.st_rdev), (int) e->sb.st_uid,
							(int) e->sb.st_gid, e->name);
				}
				write_special_file(e);
				break;
			case S_IFBLK:
				if (verbose) {
					printf("\tb %04o %4d,%4d %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, major(e->sb.st_rdev),
							minor(e->sb.st_rdev), (int) e->sb.st_uid,
							(int) e->sb.st_gid, e->name);
				}
				write_special_file(e);
				break;
			case S_IFLNK:
				if (verbose) {
					printf("\tl %04o %9lu %5d:%-3d %s -> %s\n",
							e->sb.st_mode & ~S_IFMT, e->sb.st_size,
							(int) e->sb.st_uid, (int) e->sb.st_gid, e->name,
							e->link);
				}
				write_symlink(e);
				break;
#endif // 0 unused file types                
			case S_IFREG:
				if (verbose) {
					printf("\tf %04o %9lu %5d:%-3d %s\n",
							e->sb.st_mode & ~S_IFMT, e->sb.st_size,
							(int) e->sb.st_uid, (int) e->sb.st_gid, e->name);
				}
				write_regular_file(e);
				break;
			default:
				printf("Unknown mode %o for %s", e->sb.st_mode,
						e->fullname);
				break;
		}
		e = e->next;
	}

	e = dir->files;
	while (e) {
		if (S_ISDIR(e->sb.st_mode)) {
			if (e->files) {
				recursive_populate_directory(e);
			} else if (verbose) {
				printf("%s\n", e->fullname);
			}
		}
		e = e->next;
	}
}

static void create_target_filesystem(struct filesystem_entry *root)
{
	ebh.magic    = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ebh.nodetype = cpu_to_je16(JFFS2_NODETYPE_ERASEBLOCK_HEADER);
	ebh.totlen   = cpu_to_je32(sizeof(ebh));
	ebh.hdr_crc  = cpu_to_je32(fs_crc32(0, &ebh, sizeof(struct jffs2_unknown_node)-4));
	ebh.reserved = 0;
	ebh.compat_fset = JFFS2_EBH_COMPAT_FSET;
	ebh.incompat_fset = JFFS2_EBH_INCOMPAT_FSET;
	ebh.rocompat_fset = JFFS2_EBH_ROCOMPAT_FSET;
	ebh.erase_count = cpu_to_je32(0);
	ebh.node_crc = cpu_to_je32(fs_crc32(0, (unsigned char *)&ebh + sizeof(struct jffs2_unknown_node) + 4,
							sizeof(struct jffs2_raw_ebh) - sizeof(struct jffs2_unknown_node) - 4));

    out_ofs = 0;    // reset the static output file size each invocation
	if (ino == 0)
		ino = 1;

	root->sb.st_ino = 1;
	recursive_populate_directory(root);

	if (pad_fs_size == -1) {
		padblock();
	} else {
		if (pad_fs_size && add_ebhs){
			padblock();
			while (out_ofs < pad_fs_size) {
				full_write(out_fd, &ebh, sizeof(ebh));
				pad(ebh_size - sizeof(ebh));
				padblock();
			}
		} else {
			while (out_ofs < pad_fs_size) {
				full_write(out_fd, ffbuf, min(sizeof(ffbuf), pad_fs_size - out_ofs));
			}

		}
	}
}

int mkfs_jffs2_buf2buf(u8* src, u32 src_len, u8* dst, u32* dst_len)
{
//	struct stat sb;
	struct filesystem_entry root, img;

    squash_uids = 1;
    squash_perms = 1;
    fake_times = 1;
    verbose = 1;

    root.next = 0;
    root.prev = 0;
    root.parent = 0;
    root.files = &img;
    root.name = "";
    root.path = "/";
    root.fullname = "/";
    root.hostname = "/";
    root.sb.st_uid = 0;
    root.sb.st_gid = 0x64;
    root.sb.st_mode = 0x41ed;
    root.sb.st_size = 0;

    img.parent = &root;
    img.next = 0;
    img.prev = 0;
    img.files = 0;
    img.name = "uImage";
    img.path = "/";
    img.fullname = "/uImage";
    img.hostname = "/uImage";
    img.sb.st_uid = 0;
    img.sb.st_gid = 0x64;
    img.sb.st_mode = 0x81a4;
    img.sb.st_size = src_len;
    
	create_target_filesystem(&root);
    *dst_len = out_ofs;
    //printf("OFS=0x%x\n",out_ofs);

	return 0;
}   
#endif /* CONFIG_APBOOT */
