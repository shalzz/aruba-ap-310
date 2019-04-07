/*
 * Copyright 2010-2011 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
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

#include <command.h>
#include <common.h>
#include <fsl_validate.h>

static int do_esbc_validate(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
	if (argc < 2)
		return cmd_usage(cmdtp);

	return fsl_secboot_validate(cmdtp, flag, argc, argv);
}

U_BOOT_CMD(
	esbc_validate,	3,	0,	do_esbc_validate,
	"Validates signature of a given image using RSA verification\
	algorithm as part of Freescale Secure Boot Process",
	"<hdr_addr> <hash_val>"
);

static int do_esbc_halt(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
	printf("Core is entering spin loop.\n");
	while (1);

	return 0;
}

U_BOOT_CMD(
	esbc_halt,	1,	0,	do_esbc_halt,
	"Put the core in spin loop if control reaches to uboot\
	from bootscript",
	""
);
