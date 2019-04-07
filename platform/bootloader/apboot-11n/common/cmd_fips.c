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
 * Fips CLI
 *
 */

#ifdef FIPS_TEST
#include <common.h>
#include <malloc.h>
#include <command.h>
#include <flash.h>
#include "xyssl/rsa.h"
#include <watchdog.h>

extern int
sha_test (char *fips_buf, int bufSize, unsigned int *MDlen, 
	unsigned int *msgLen);
extern int
rsa_sigver_test (char *fips_buf, int bufSize);
int do_mem_buff_sha_fips (char *argv[]);
int do_mem_buff_rsa_fips (char *argv[]);

int do_mem_buff_fips ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    if (argc < 4) {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }

    if(strcmp(argv[1], "rsa") == 0){
	do_mem_buff_rsa_fips (argv);
    } else if (strcmp(argv[1], "sha1") == 0) {
	do_mem_buff_sha_fips (argv);
    }
    return 0;
}

int do_mem_buff_sha_fips (char *argv[])
{
    ulong	addr = 0, length = 0;
    int rc = 0;
    unsigned int mdlen = 0;
    unsigned int msgLen = 0;

    addr = simple_strtoul(argv[2], NULL, 16);
    length = simple_strtoul(argv[3], NULL, 16);
    sha_test ((char *)addr, length, &mdlen, &msgLen); 
    return (rc);
}
int do_mem_buff_rsa_fips (char *argv[])
{
    ulong	addr = 0, length = 0;
    int rc = 0;
    
    addr = simple_strtoul(argv[2], NULL, 16);
    length = simple_strtoul(argv[3], NULL, 16);

    rsa_sigver_test((char *)addr, length);
    return (rc);
}

U_BOOT_CMD(
	fipstest,     4,     1,      do_mem_buff_fips,
	"fipstest rsa|sha1 0xddr size     - Do SHA1 Fips test",
	"address bytes-in-hex"
	);
#endif
