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
 * Memory Functions
 *
 * Copied from FADS ROM, Dan Malek (dmalek@jlc.net)
 */

#include <common.h>
#include <malloc.h>
#include <command.h>
#include <flash.h>
#include <watchdog.h>
#if (CONFIG_COMMANDS & CFG_CMD_MMC)
#include <mmc.h>
#endif
#ifdef CONFIG_HAS_DATAFLASH
#include <dataflash.h>
#endif

#ifdef CONFIG_OCTEON
#include "octeon_boot.h"
#endif
#if defined(CONFIG_AR7100) || defined(CONFIG_AR7240) || defined(CONFIG_ATHEROS)
#include <ar7100_soc.h>
#endif
// For the Aruba memory test suite
#include <linux/ctype.h>
#include <rngs.h>
#if defined(CONFIG_NEBBIOLO) || defined(CONFIG_GRENACHE)
#include "nebbiolo/mem_ctrl.h"
#include "nebbiolo/flash_ctrl.h"
#include "nebbiolo/on_chip.h"
#endif
#ifdef CONFIG_ARM
#include <asm/io.h>
#endif

#if (CONFIG_COMMANDS & (CFG_CMD_MEMORY	| \
			CFG_CMD_I2C	| \
			CFG_CMD_ITEST	| \
			CFG_CMD_PCI	| \
			CMD_CMD_PORTIO	) )
            
static	ulong	base_address = 0;

#endif

extern void iproc_set_bank(unsigned);

#if (CONFIG_COMMANDS & CFG_CMD_MEMORY)

int do_ramtest(cmd_tbl_t *cmptp, int flag, int argc, char *argv[]);
int do_neb_mtest(cmd_tbl_t *cmdtp, int flag, int argc, char* argv[]);
int mtest_1(uint32_t* start, uint32_t* end, int invert, int t_flags);
int mtest_2(uint32_t* start, uint32_t* end, int invert, int t_flags);
int mtest_3(uint32_t* start, uint32_t* end, int invert, int t_flags);
int mtest_4(uint32_t* start, uint32_t* end, int invert, int t_flags);
int mtest_quick(uint32_t* start, uint32_t* end, int t_flags);
static void mtest_usage(void);
static void doMod3Test1stPass( uint32_t* address, uint32_t* end);
static int doMod3Test2ndPass (uint32_t* start, uint32_t* end);
static int doMod3Test3rdPass (uint32_t* start, uint32_t* end);
static int doMod3TestLastVerify (uint32_t* start, uint32_t* end);

#ifdef	CMD_MEM_DEBUG
#define	PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif

static int mod_mem(cmd_tbl_t *, int, int, int, char *[]);

/* Display values from last command.
 * Memory modify remembered values are different from display memory.
 */
uint	dp_last_addr, dp_last_size;
uint	dp_last_length = 0x40;
uint	mm_last_addr, mm_last_size;

static	uint64_t	base_address64 = (1ull << 63);

/* Memory Display
 *
 * Syntax:
 *	md{.b, .w, .l} {addr} {len}
 */
#define DISP_LINE_LEN	16
int do_mem_md ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong	addr, length;
	ulong	i, nbytes, linebytes;
	u_char	*cp;
	int	size;
	int rc = 0;

	/* We use the last specified parameters, unless new ones are
	 * entered.
	 */
	addr = dp_last_addr;
	size = dp_last_size;
	length = dp_last_length;

	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if ((flag & CMD_FLAG_REPEAT) == 0) {
		/* New command specified.  Check for a size specification.
		 * Defaults to long if no or incorrect specification.
		 */
		if ((size = cmd_get_data_size(argv[0], 4)) < 0)
			return 1;

		/* Address is specified since argc > 1
		*/
		addr = simple_strtoul(argv[1], NULL, 16);
		addr += base_address;

		/* If another parameter, it is the length to display.
		 * Length is the number of objects, not number of bytes.
		 */
		if (argc > 2)
			length = simple_strtoul(argv[2], NULL, 16);
	}

#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
	if (addr >= CONFIG_FLASH_MAP_SPLIT_ADDR) {
	    iproc_set_bank(2);
	} else {
	    iproc_set_bank(1);
	}
#endif
	/* Print the lines.
	 *
	 * We buffer all read data, so we can make sure data is read only
	 * once, and all accesses are with the specified bus width.
	 */
	nbytes = length * size;
	do {
		char	linebuf[DISP_LINE_LEN];
		uint	*uip = (uint   *)linebuf;
		ushort	*usp = (ushort *)linebuf;
		u_char	*ucp = (u_char *)linebuf;
#ifdef CONFIG_HAS_DATAFLASH
		int rc;
#endif
		printf("%08lx:", addr);
		linebytes = (nbytes>DISP_LINE_LEN)?DISP_LINE_LEN:nbytes;

#ifdef CONFIG_HAS_DATAFLASH
		if ((rc = read_dataflash(addr, (linebytes/size)*size, linebuf)) == DATAFLASH_OK){
			/* if outside dataflash */
			/*if (rc != 1) {
				dataflash_perror (rc);
				return (1);
			}*/
			for (i=0; i<linebytes; i+= size) {
				if (size == 4) {
					printf(" %08x", *uip++);
				} else if (size == 2) {
					printf(" %04x", *usp++);
				} else {
					printf(" %02x", *ucp++);
				}
				addr += size;
			}

		} else {	/* addr does not correspond to DataFlash */
#endif
		for (i=0; i<linebytes; i+= size) {
			if (size == 4) {
				printf(" %08x", (*uip++ = *((uint *)addr)));
			} else if (size == 2) {
				printf(" %04x", (*usp++ = *((ushort *)addr)));
			} else {
				printf(" %02x", (*ucp++ = *((u_char *)addr)));
			}
			addr += size;
#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
			if (addr >= CONFIG_FLASH_MAP_SPLIT_ADDR) {
			    iproc_set_bank(2);
			}
#endif
		}
#ifdef CONFIG_HAS_DATAFLASH
		}
#endif
		puts ("    ");
		cp = (u_char *)linebuf;
		for (i=0; i<linebytes; i++) {
			if ((*cp < 0x20) || (*cp > 0x7e))
				putc ('.');
			else
				printf("%c", *cp);
			cp++;
		}
		putc ('\n');
		WATCHDOG_RESET();
		nbytes -= linebytes;
		if (ctrlc()) {
			rc = 1;
			break;
		}
	} while (nbytes > 0);

	dp_last_addr = addr;
	dp_last_length = length;
	dp_last_size = size;
#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
        iproc_set_bank(1);
#endif
	return (rc);
}

int do_mem_mm ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return mod_mem (cmdtp, 1, flag, argc, argv);
}
int do_mem_nm ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return mod_mem (cmdtp, 0, flag, argc, argv);
}


int do_read_cmp ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if ((argc != 3)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
    uint32_t addr;
    uint32_t val;
    uint32_t *ptr;
    addr = simple_strtoul(argv[1], NULL, 16);
    addr += base_address;

    val = simple_strtoul(argv[2], NULL, 16);
    printf("Looking for value other than 0x%x, addr: 0x%x\n", val, addr);
    ptr = (void *)addr;
    while (*ptr++ == val)
        ;
    ptr--;
    printf("Found 0x%x at addr %p\n", *ptr, ptr);

    return(0);
}

#ifdef CONFIG_OCTEON
int do_read64 ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if ((argc != 2)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
    uint64_t addr, val;
    addr = simple_strtoull(argv[1], NULL, 16);
    if (!(addr & base_address64)) {
	    addr += base_address64;
    }
    addr &= ~7;

    printf("attempting to read from addr: 0x%Lx\n", addr);
    val = octeon_read64(addr);
    printf("0x%Lx: 0x%Lx\n", addr, val);

    return(0);
}
int do_read64b ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if ((argc != 2)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
    uint64_t addr;
    uint8_t val;
    addr = simple_strtoull(argv[1], NULL, 16);
    if (!(addr & base_address64)) {
	    addr += base_address64;
    }
    addr &= ~7;

    printf("attempting to read from addr: 0x%Lx\n", addr);
    val = octeon_read64_byte(addr);
    printf("0x%Lx: 0x%x\n", addr, val);

    return(0);
}
int do_read64l ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if ((argc != 2)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
    uint64_t addr;
    uint32_t val;
    addr = simple_strtoull(argv[1], NULL, 16);
    if (!(addr & base_address64)) {
	    addr += base_address64;
    }
    addr &= ~7;

    printf("attempting to read from addr: 0x%Lx\n", addr);
    val = cvmx_read64_uint32(addr);
    printf("0x%Lx: 0x%x\n", addr, val);

    return(0);
}

int do_write64 ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if ((argc != 3)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
    uint64_t addr, val;
    addr = simple_strtoull(argv[1], NULL, 16);
    if (!(addr & base_address64)) {
	    addr += base_address64;
    }
    addr &= ~7;
    val = simple_strtoull(argv[2], NULL, 16);

    printf("writing 0x%Lx to addr: 0x%Lx\n", val, addr);

    octeon_write64(addr, val);
    return(0);
}

int do_write64b ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if ((argc != 3)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
    uint64_t addr;
    uint8_t val;
    addr = simple_strtoull(argv[1], NULL, 16);
    if (!(addr & base_address64)) {
	    addr += base_address64;
    }
    addr &= ~7;
    val = simple_strtoull(argv[2], NULL, 16);

    printf("writing 0x%x to addr: 0x%Lx\n", val, addr);

    octeon_write64_byte(addr, val);
    return(0);
}
int do_write64l ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if ((argc != 3)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
    uint64_t addr;
    uint32_t val;
    addr = simple_strtoull(argv[1], NULL, 16);
    if (!(addr & base_address64)) {
	    addr += base_address64;
    }
    addr &= ~7;
    val = simple_strtoull(argv[2], NULL, 16);

    printf("writing 0x%x to addr: 0x%Lx\n", val, addr);

    cvmx_write64_uint32(addr, val);
    return(0);
}

int do_read_pci ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if ((argc != 2)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
    uint32_t val;
    uint64_t addr;
    addr = simple_strtoull(argv[1], NULL, 16);
//    if (!(addr & base_address64)) {
//	    addr += base_address64;
//    }

    addr |= 0x90011b0000000000ull;
    printf("attempting to read from addr: 0x%Lx\n", addr);
    val = cvmx_read64_uint32(addr);
    printf("0x%x: 0x%x\n", addr, __le32_to_cpu(val));

    return(0);
}

int do_write_pci ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	if ((argc != 3)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
    uint32_t val;
    uint64_t addr;
    addr = simple_strtoull(argv[1], NULL, 16);
//    if (!(addr & base_address64)) {
//	    addr += base_address64;
//   }
    val = simple_strtoull(argv[2], NULL, 16);

    addr |= 0x90011b0000000000ull;
    printf("writing 0x%x to addr: 0x%Lx\n", val, addr);

    cvmx_write64_uint32(addr, __cpu_to_le32(val));
    return(0);
}

#endif

#if defined(CONFIG_AR7100) || defined(CONFIG_AR7240) || defined(CONFIG_ATHEROS)
int 
do_read_pci ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    if ((argc != 2)) {
	    printf ("Usage:\n%s\n", cmdtp->usage);
	    return 1;
    }
    uint32_t val;
    uint32_t addr;
    addr = simple_strtoul(argv[1], NULL, 16);

    addr |= KSEG1;
    printf("attempting to read from addr: 0x%x\n", addr);
    val = ar7100_reg_rd(addr);
    printf("0x%x: 0x%x\n", addr, /*__le32_to_cpu*/(val));

    return(0);
}

int 
do_write_pci ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    uint32_t val;
    uint32_t addr;

    if ((argc != 3)) {
	    printf ("Usage:\n%s\n", cmdtp->usage);
	    return 1;
    }
    addr = simple_strtoul(argv[1], NULL, 16);
    val = simple_strtoul(argv[2], NULL, 16);

    addr |= KSEG1;
    printf("writing 0x%x to addr: 0x%x\n", val, addr);

    ar7100_reg_wr(addr, /*__cpu_to_le32*/(val));
    return(0);
}
#endif

#if defined(CONFIG_ARM) || defined(CONFIG_PPC)
int 
do_read_pci ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    uint32_t val;
    uint32_t addr;

    if ((argc != 2)) {
	    printf ("Usage:\n%s\n", cmdtp->usage);
	    return 1;
    }
    addr = simple_strtoul(argv[1], NULL, 16);

    printf("attempting to read from addr: 0x%x\n", addr);
    val = readl(addr);
    printf("0x%x: 0x%x\n", addr, /*__le32_to_cpu*/(val));

    return(0);
}

int 
do_write_pci ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    if ((argc != 3)) {
	    printf ("Usage:\n%s\n", cmdtp->usage);
	    return 1;
    }
    uint32_t val;
    uint32_t addr;
    addr = simple_strtoul(argv[1], NULL, 16);
    val = simple_strtoul(argv[2], NULL, 16);

    printf("writing 0x%x to addr: 0x%x\n", val, addr);

    writel(val, addr);
    return(0);
}
#endif

int do_mem_mw ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong	addr, writeval, count;
	int	size;

	if ((argc < 3) || (argc > 4)) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	/* Check for size specification.
	*/
	if ((size = cmd_get_data_size(argv[0], 4)) < 1)
		return 1;

#ifdef CONFIG_OCTEON
    if (size == 8)
    {
        uint64_t addr, writeval, count;
        addr = simple_strtoull(argv[1], NULL, 16);
        writeval = simple_strtoull(argv[2], NULL, 16);
        if (argc == 4) {
            count = simple_strtoull(argv[3], NULL, 16);
        } else {
            count = 1;
        }

        while (count-- > 0) {
            octeon_write64(addr, writeval);
            addr += 8;
        }
    }
#endif

	/* Address is specified since argc > 1
	*/
	addr = simple_strtoul(argv[1], NULL, 16);
	addr += base_address;

	/* Get the value to write.
	*/
	writeval = simple_strtoul(argv[2], NULL, 16);


	/* Count ? */
	if (argc == 4) {
		count = simple_strtoul(argv[3], NULL, 16);
	} else {
		count = 1;
	}

	while (count-- > 0) {
		if (size == 4)
			*((ulong  *)addr) = (ulong )writeval;
		else if (size == 2)
			*((ushort *)addr) = (ushort)writeval;
		else
			*((u_char *)addr) = (u_char)writeval;
		addr += size;
	}
	return 0;
}

#ifdef CONFIG_MX_CYCLIC
int do_mem_mdc ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	ulong count;

	if (argc < 4) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	count = simple_strtoul(argv[3], NULL, 10);

	for (;;) {
		do_mem_md (NULL, 0, 3, argv);

		/* delay for <count> ms... */
		for (i=0; i<count; i++)
			udelay (1000);

		/* check for ctrl-c to abort... */
		if (ctrlc()) {
			puts("Abort\n");
			return 0;
		}
	}

	return 0;
}

int do_mem_mwc ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	ulong count;

	if (argc < 4) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	count = simple_strtoul(argv[3], NULL, 10);

	for (;;) {
		do_mem_mw (NULL, 0, 3, argv);

		/* delay for <count> ms... */
		for (i=0; i<count; i++)
			udelay (1000);

		/* check for ctrl-c to abort... */
		if (ctrlc()) {
			puts("Abort\n");
			return 0;
		}
	}

	return 0;
}
#endif /* CONFIG_MX_CYCLIC */
#endif	/* CFG_CMD_MEMORY */

int do_mem_cmp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong	addr1, addr2, count, ngood;
	int	size;
	int     rcode = 0;

	if (argc != 4) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	/* Check for size specification.
	*/
	if ((size = cmd_get_data_size(argv[0], 4)) < 0)
		return 1;

	addr1 = simple_strtoul(argv[1], NULL, 16);
	addr1 += base_address;

	addr2 = simple_strtoul(argv[2], NULL, 16);
	addr2 += base_address;

#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
	if (addr1 >= CONFIG_FLASH_MAP_SPLIT_ADDR
	    || addr2 >= CONFIG_FLASH_MAP_SPLIT_ADDR) {
	    iproc_set_bank(2);
	} else {
	    iproc_set_bank(1);
	}
#endif

	count = simple_strtoul(argv[3], NULL, 16);

#ifdef CONFIG_HAS_DATAFLASH
	if (addr_dataflash(addr1) | addr_dataflash(addr2)){
		puts ("Comparison with DataFlash space not supported.\n\r");
		return 0;
	}
#endif

	ngood = 0;

	while (count-- > 0) {
		if (size == 4) {
			ulong word1 = *(ulong *)addr1;
			ulong word2 = *(ulong *)addr2;
			if (word1 != word2) {
				printf("word at 0x%08lx (0x%08lx) "
					"!= word at 0x%08lx (0x%08lx)\n",
					addr1, word1, addr2, word2);
				rcode = 1;
				break;
			}
		}
		else if (size == 2) {
			ushort hword1 = *(ushort *)addr1;
			ushort hword2 = *(ushort *)addr2;
			if (hword1 != hword2) {
				printf("halfword at 0x%08lx (0x%04x) "
					"!= halfword at 0x%08lx (0x%04x)\n",
					addr1, hword1, addr2, hword2);
				rcode = 1;
				break;
			}
		}
		else {
			u_char byte1 = *(u_char *)addr1;
			u_char byte2 = *(u_char *)addr2;
			if (byte1 != byte2) {
				printf("byte at 0x%08lx (0x%02x) "
					"!= byte at 0x%08lx (0x%02x)\n",
					addr1, byte1, addr2, byte2);
				rcode = 1;
				break;
			}
		}
		ngood++;
		addr1 += size;
		addr2 += size;
#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
		if (addr1 >= CONFIG_FLASH_MAP_SPLIT_ADDR
		    || addr2 >= CONFIG_FLASH_MAP_SPLIT_ADDR) {
		    iproc_set_bank(2);
		}
#endif
		WATCHDOG_RESET();
	}

	printf("%ld %s%s were the same\n",
		ngood, size == 4 ? "word" : size == 2 ? "halfword" : "byte",
		ngood == 1 ? "" : "s");
#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
        iproc_set_bank(1);
#endif
	return rcode;
}

int do_mem_cp ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong	addr, dest, count;
	int	size;

	if (argc != 4) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	/* Check for size specification.
	*/
	if ((size = cmd_get_data_size(argv[0], 4)) < 0)
		return 1;

	addr = simple_strtoul(argv[1], NULL, 16);
	addr += base_address;

	dest = simple_strtoul(argv[2], NULL, 16);
	dest += base_address;

	count = simple_strtoul(argv[3], NULL, 16);

	if (count == 0) {
		puts ("Zero length ???\n");
		return 1;
	}

#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
	if (addr >= CONFIG_FLASH_MAP_SPLIT_ADDR) {
	    iproc_set_bank(2);
	} else {
	    iproc_set_bank(1);
	}
#endif

#ifndef CFG_NO_FLASH
	/* check if we are copying to Flash */
	if ( (addr2info(dest) != NULL)
#ifdef CONFIG_HAS_DATAFLASH
	   && (!addr_dataflash(addr))
#endif
	   ) {
		int rc;

		printf ("Copying to flash... \n");
#if defined(CONFIG_MARVELL)
		/* If source addr is flash copy data to memory first */
 		if (addr2info(addr) != NULL)
 		{       char* tmp_buff;
 			int i;
 			if (NULL == (tmp_buff = malloc(count*size)))
 			{
 				puts (" Copy fail, NULL pointer buffer\n");
 				return (1);
 			}
 			for( i = 0 ; i < (count*size); i++)
 				*(tmp_buff + i) = *((char *)addr + i);
  
 			rc = flash_write (tmp_buff, dest, count*size);
 			free(tmp_buff);
 		}
 		else
#endif /* defined(CONFIG_MARVELL) */
		rc = flash_write ((char *)addr, dest, count*size);
		if (rc != 0) {
			flash_perror (rc);
#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
            iproc_set_bank(1);
#endif
			return (1);
		}
		puts ("done\n");
#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
        iproc_set_bank(1);
#endif
		return 0;
	}
#endif

#if (CONFIG_COMMANDS & CFG_CMD_MMC)
	if (mmc2info(dest)) {
		int rc;

		puts ("Copy to MMC... ");
		switch (rc = mmc_write ((uchar *)addr, dest, count*size)) {
		case 0:
			putc ('\n');
			return 1;
		case -1:
			puts ("failed\n");
			return 1;
		default:
			printf ("%s[%d] FIXME: rc=%d\n",__FILE__,__LINE__,rc);
			return 1;
		}
		puts ("done\n");
		return 0;
	}

	if (mmc2info(addr)) {
		int rc;

		puts ("Copy from MMC... ");
		switch (rc = mmc_read (addr, (uchar *)dest, count*size)) {
		case 0:
			putc ('\n');
			return 1;
		case -1:
			puts ("failed\n");
			return 1;
		default:
			printf ("%s[%d] FIXME: rc=%d\n",__FILE__,__LINE__,rc);
			return 1;
		}
		puts ("done\n");
		return 0;
	}
#endif

#ifdef CONFIG_HAS_DATAFLASH
	/* Check if we are copying from RAM or Flash to DataFlash */
	if (addr_dataflash(dest) && !addr_dataflash(addr)){
		int rc;

		puts ("Copy to DataFlash... ");

		rc = write_dataflash (dest, addr, count*size);

		if (rc != 1) {
			dataflash_perror (rc);
			return (1);
		}
		puts ("done\n");
		return 0;
	}

	/* Check if we are copying from DataFlash to RAM */
	if (addr_dataflash(addr) && !addr_dataflash(dest) && (addr2info(dest)==NULL) ){
		int rc;
		rc = read_dataflash(addr, count * size, (char *) dest);
		if (rc != 1) {
			dataflash_perror (rc);
			return (1);
		}
		return 0;
	}

	if (addr_dataflash(addr) && addr_dataflash(dest)){
		puts ("Unsupported combination of source/destination.\n\r");
		return 1;
	}
#endif

	while (count-- > 0) {
		if (size == 4)
			*((ulong  *)dest) = *((ulong  *)addr);
		else if (size == 2)
			*((ushort *)dest) = *((ushort *)addr);
		else
			*((u_char *)dest) = *((u_char *)addr);
		addr += size;
		dest += size;
		WATCHDOG_RESET();
	}
#if defined(CONFIG_IPROC) && defined(CONFIG_FLASH_MAP_SPLIT_ADDR)
    iproc_set_bank(1);
#endif
	return 0;
}

#if (CONFIG_COMMANDS & CFG_CMD_MEMORY)
int do_mem_base (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if (argc > 1) {
		/* Set new base address.
		*/
		base_address64 = simple_strtoull(argv[1], NULL, 16);
        base_address = (uint32_t)base_address64;
	}
	/* Print the current base address.
	*/
	printf("Base Address64: 0x%16qx, ", base_address64);
	printf("Base Address: 0x%08lx\n", base_address);
	return 0;
}

int do_mem_loop (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong	addr, length, i, junk;
	int	size;
	volatile uint	*longp;
	volatile ushort *shortp;
	volatile u_char	*cp;

	if (argc < 3) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	/* Check for a size spefication.
	 * Defaults to long if no or incorrect specification.
	 */
	if ((size = cmd_get_data_size(argv[0], 4)) < 0)
		return 1;

	/* Address is always specified.
	*/
	addr = simple_strtoul(argv[1], NULL, 16);

	/* Length is the number of objects, not number of bytes.
	*/
	length = simple_strtoul(argv[2], NULL, 16);

	/* We want to optimize the loops to run as fast as possible.
	 * If we have only one object, just run infinite loops.
	 */
	if (length == 1) {
		if (size == 4) {
			longp = (uint *)addr;
			for (;;)
				i = *longp;
		}
		if (size == 2) {
			shortp = (ushort *)addr;
			for (;;)
				i = *shortp;
		}
		cp = (u_char *)addr;
		for (;;)
			i = *cp;
	}

	if (size == 4) {
		for (;;) {
			longp = (uint *)addr;
			i = length;
			while (i-- > 0)
				junk = *longp++;
		}
	}
	if (size == 2) {
		for (;;) {
			shortp = (ushort *)addr;
			i = length;
			while (i-- > 0)
				junk = *shortp++;
		}
	}
	for (;;) {
		cp = (u_char *)addr;
		i = length;
		while (i-- > 0)
			junk = *cp++;
	}
}

#ifdef CONFIG_LOOPW
int do_mem_loopw (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong	addr, length, i, data;
	int	size;
	volatile uint	*longp;
	volatile ushort *shortp;
	volatile u_char	*cp;

	if (argc < 4) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	/* Check for a size spefication.
	 * Defaults to long if no or incorrect specification.
	 */
	if ((size = cmd_get_data_size(argv[0], 4)) < 0)
		return 1;

	/* Address is always specified.
	*/
	addr = simple_strtoul(argv[1], NULL, 16);

	/* Length is the number of objects, not number of bytes.
	*/
	length = simple_strtoul(argv[2], NULL, 16);

	/* data to write */
	data = simple_strtoul(argv[3], NULL, 16);

	/* We want to optimize the loops to run as fast as possible.
	 * If we have only one object, just run infinite loops.
	 */
	if (length == 1) {
		if (size == 4) {
			longp = (uint *)addr;
			for (;;)
				*longp = data;
					}
		if (size == 2) {
			shortp = (ushort *)addr;
			for (;;)
				*shortp = data;
		}
		cp = (u_char *)addr;
		for (;;)
			*cp = data;
	}

	if (size == 4) {
		for (;;) {
			longp = (uint *)addr;
			i = length;
			while (i-- > 0)
				*longp++ = data;
		}
	}
	if (size == 2) {
		for (;;) {
			shortp = (ushort *)addr;
			i = length;
			while (i-- > 0)
				*shortp++ = data;
		}
	}
	for (;;) {
		cp = (u_char *)addr;
		i = length;
		while (i-- > 0)
			*cp++ = data;
	}
}
#endif /* CONFIG_LOOPW */

/*
 * Perform a memory test. A more complete alternative test can be
 * configured using CFG_ALT_MEMTEST. The complete test loops until
 * interrupted by ctrl-c or by a failure of one of the sub-tests.
 */
int do_mem_mtest (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	vu_long	*addr, *start, *end;
	ulong	val;
	ulong	readback;

#if defined(CFG_ALT_MEMTEST)
	vu_long	addr_mask;
	vu_long	offset;
	vu_long	test_offset;
	vu_long	pattern;
	vu_long	temp;
	vu_long	anti_pattern;
	vu_long	num_words;
#if defined(CFG_MEMTEST_SCRATCH)
	vu_long *dummy = (vu_long*)CFG_MEMTEST_SCRATCH;
#else
	vu_long *dummy = NULL;
#endif
	int	j;
	int iterations = 1;

	static const ulong bitpattern[] = {
		0x00000001,	/* single bit */
		0x00000003,	/* two adjacent bits */
		0x00000007,	/* three adjacent bits */
		0x0000000F,	/* four adjacent bits */
		0x00000005,	/* two non-adjacent bits */
		0x00000015,	/* three non-adjacent bits */
		0x00000055,	/* four non-adjacent bits */
		0xaaaaaaaa,	/* alternating 1/0 */
	};
#else
	ulong	incr;
	ulong	pattern;
	int     rcode = 0;
#endif

	if (argc > 1) {
		start = (ulong *)simple_strtoul(argv[1], NULL, 16);
	} else {
		start = (ulong *)CFG_MEMTEST_START;
	}

	if (argc > 2) {
		end = (ulong *)simple_strtoul(argv[2], NULL, 16);
	} else {
		end = (ulong *)(CFG_MEMTEST_END);
	}

	if (argc > 3) {
		pattern = (ulong)simple_strtoul(argv[3], NULL, 16);
	} else {
		pattern = 0;
	}

#if defined(CFG_ALT_MEMTEST)
	printf ("Testing %08x ... %08x:\n", (uint)start, (uint)end);
	PRINTF("%s:%d: start 0x%p end 0x%p\n",
		__FUNCTION__, __LINE__, start, end);

	for (;;) {
		if (ctrlc()) {
			putc ('\n');
			return 1;
		}

		printf("Iteration: %6d\r", iterations);
		PRINTF("Iteration: %6d\n", iterations);
		iterations++;

		/*
		 * Data line test: write a pattern to the first
		 * location, write the 1's complement to a 'parking'
		 * address (changes the state of the data bus so a
		 * floating bus doen't give a false OK), and then
		 * read the value back. Note that we read it back
		 * into a variable because the next time we read it,
		 * it might be right (been there, tough to explain to
		 * the quality guys why it prints a failure when the
		 * "is" and "should be" are obviously the same in the
		 * error message).
		 *
		 * Rather than exhaustively testing, we test some
		 * patterns by shifting '1' bits through a field of
		 * '0's and '0' bits through a field of '1's (i.e.
		 * pattern and ~pattern).
		 */
		addr = start;
		for (j = 0; j < sizeof(bitpattern)/sizeof(bitpattern[0]); j++) {
		    val = bitpattern[j];
		    for(; val != 0; val <<= 1) {
			*addr  = val;
			*dummy  = ~val; /* clear the test data off of the bus */
			readback = *addr;
			if(readback != val) {
			     printf ("FAILURE (data line): "
				"expected %08lx, actual %08lx\n",
					  val, readback);
			}
			*addr  = ~val;
			*dummy  = val;
			readback = *addr;
			if(readback != ~val) {
			    printf ("FAILURE (data line): "
				"Is %08lx, should be %08lx\n",
					readback, ~val);
			}
		    }
		}

		/*
		 * Based on code whose Original Author and Copyright
		 * information follows: Copyright (c) 1998 by Michael
		 * Barr. This software is placed into the public
		 * domain and may be used for any purpose. However,
		 * this notice must not be changed or removed and no
		 * warranty is either expressed or implied by its
		 * publication or distribution.
		 */

		/*
		 * Address line test
		 *
		 * Description: Test the address bus wiring in a
		 *              memory region by performing a walking
		 *              1's test on the relevant bits of the
		 *              address and checking for aliasing.
		 *              This test will find single-bit
		 *              address failures such as stuck -high,
		 *              stuck-low, and shorted pins. The base
		 *              address and size of the region are
		 *              selected by the caller.
		 *
		 * Notes:	For best results, the selected base
		 *              address should have enough LSB 0's to
		 *              guarantee single address bit changes.
		 *              For example, to test a 64-Kbyte
		 *              region, select a base address on a
		 *              64-Kbyte boundary. Also, select the
		 *              region size as a power-of-two if at
		 *              all possible.
		 *
		 * Returns:     0 if the test succeeds, 1 if the test fails.
		 *
		 * ## NOTE ##	Be sure to specify start and end
		 *              addresses such that addr_mask has
		 *              lots of bits set. For example an
		 *              address range of 01000000 02000000 is
		 *              bad while a range of 01000000
		 *              01ffffff is perfect.
		 */
		addr_mask = ((ulong)end - (ulong)start)/sizeof(vu_long);
		pattern = (vu_long) 0xaaaaaaaa;
		anti_pattern = (vu_long) 0x55555555;

		PRINTF("%s:%d: addr mask = 0x%.8lx\n",
			__FUNCTION__, __LINE__,
			addr_mask);
		/*
		 * Write the default pattern at each of the
		 * power-of-two offsets.
		 */
		for (offset = 1; (offset & addr_mask) != 0; offset <<= 1) {
			start[offset] = pattern;
		}

		/*
		 * Check for address bits stuck high.
		 */
		test_offset = 0;
		start[test_offset] = anti_pattern;

		for (offset = 1; (offset & addr_mask) != 0; offset <<= 1) {
		    temp = start[offset];
		    if (temp != pattern) {
			printf ("\nFAILURE: Address bit stuck high @ 0x%.8lx:"
				" expected 0x%.8lx, actual 0x%.8lx\n",
				(ulong)&start[offset], pattern, temp);
			return 1;
		    }
		}
		start[test_offset] = pattern;

		/*
		 * Check for addr bits stuck low or shorted.
		 */
		for (test_offset = 1; (test_offset & addr_mask) != 0; test_offset <<= 1) {
		    start[test_offset] = anti_pattern;

		    for (offset = 1; (offset & addr_mask) != 0; offset <<= 1) {
			temp = start[offset];
			if ((temp != pattern) && (offset != test_offset)) {
			    printf ("\nFAILURE: Address bit stuck low or shorted @"
				" 0x%.8lx: expected 0x%.8lx, actual 0x%.8lx\n",
				(ulong)&start[offset], pattern, temp);
			    return 1;
			}
		    }
		    start[test_offset] = pattern;
		}

		/*
		 * Description: Test the integrity of a physical
		 *		memory device by performing an
		 *		increment/decrement test over the
		 *		entire region. In the process every
		 *		storage bit in the device is tested
		 *		as a zero and a one. The base address
		 *		and the size of the region are
		 *		selected by the caller.
		 *
		 * Returns:     0 if the test succeeds, 1 if the test fails.
		 */
		num_words = ((ulong)end - (ulong)start)/sizeof(vu_long) + 1;

		/*
		 * Fill memory with a known pattern.
		 */
		for (pattern = 1, offset = 0; offset < num_words; pattern++, offset++) {
			start[offset] = pattern;
		}

		/*
		 * Check each location and invert it for the second pass.
		 */
		for (pattern = 1, offset = 0; offset < num_words; pattern++, offset++) {
		    temp = start[offset];
		    if (temp != pattern) {
			printf ("\nFAILURE (read/write) @ 0x%.8lx:"
				" expected 0x%.8lx, actual 0x%.8lx)\n",
				(ulong)&start[offset], pattern, temp);
			return 1;
		    }

		    anti_pattern = ~pattern;
		    start[offset] = anti_pattern;
		}

		/*
		 * Check each location for the inverted pattern and zero it.
		 */
		for (pattern = 1, offset = 0; offset < num_words; pattern++, offset++) {
		    anti_pattern = ~pattern;
		    temp = start[offset];
		    if (temp != anti_pattern) {
			printf ("\nFAILURE (read/write): @ 0x%.8lx:"
				" expected 0x%.8lx, actual 0x%.8lx)\n",
				(ulong)&start[offset], anti_pattern, temp);
			return 1;
		    }
		    start[offset] = 0;
		}
	}

#else /* The original, quickie test */
	incr = 1;
	for (;;) {
		if (ctrlc()) {
			putc ('\n');
			return 1;
		}

		printf ("\rPattern %08lX  Writing..."
			"%12s"
			"\b\b\b\b\b\b\b\b\b\b",
			pattern, "");

		for (addr=start,val=pattern; addr<end; addr++) {
			*addr = val;
			val  += incr;
		}

		puts ("Reading...");

		for (addr=start,val=pattern; addr<end; addr++) {
			readback = *addr;
			if (readback != val) {
				printf ("\nMem error @ 0x%08X: "
					"found %08lX, expected %08lX\n",
					(uint)addr, readback, val);
				rcode = 1;
			}
			val += incr;
		}

		/*
		 * Flip the pattern each time to make lots of zeros and
		 * then, the next time, lots of ones.  We decrement
		 * the "negative" patterns and increment the "positive"
		 * patterns to preserve this feature.
		 */
		if(pattern & 0x80000000) {
			pattern = -pattern;	/* complement & increment */
		}
		else {
			pattern = ~pattern;
		}
		incr = -incr;
		WATCHDOG_RESET();
	}
	return rcode;
#endif
}


/* Modify memory.
 *
 * Syntax:
 *	mm{.b, .w, .l} {addr}
 *	nm{.b, .w, .l} {addr}
 */
static int
mod_mem(cmd_tbl_t *cmdtp, int incrflag, int flag, int argc, char *argv[])
{
	ulong	addr, i;
	int	nbytes, size;
	extern char console_buffer[];

	if (argc != 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

#ifdef CONFIG_BOOT_RETRY_TIME
	reset_cmd_timeout();	/* got a good command to get here */
#endif
	/* We use the last specified parameters, unless new ones are
	 * entered.
	 */
	addr = mm_last_addr;
	size = mm_last_size;

	if ((flag & CMD_FLAG_REPEAT) == 0) {
		/* New command specified.  Check for a size specification.
		 * Defaults to long if no or incorrect specification.
		 */
		if ((size = cmd_get_data_size(argv[0], 4)) < 0)
			return 1;

		/* Address is specified since argc > 1
		*/
		addr = simple_strtoul(argv[1], NULL, 16);
		addr += base_address;
	}

#ifdef CONFIG_HAS_DATAFLASH
	if (addr_dataflash(addr)){
		puts ("Can't modify DataFlash in place. Use cp instead.\n\r");
		return 0;
	}
#endif

	/* Print the address, followed by value.  Then accept input for
	 * the next value.  A non-converted value exits.
	 */
	do {
		printf("%08lx:", addr);
		if (size == 4)
			printf(" %08x", *((uint   *)addr));
		else if (size == 2)
			printf(" %04x", *((ushort *)addr));
		else
			printf(" %02x", *((u_char *)addr));

		nbytes = readline (" ? ");
		if (nbytes == 0 || (nbytes == 1 && console_buffer[0] == '-')) {
			/* <CR> pressed as only input, don't modify current
			 * location and move to next. "-" pressed will go back.
			 */
			if (incrflag)
				addr += nbytes ? -size : size;
			nbytes = 1;
#ifdef CONFIG_BOOT_RETRY_TIME
			reset_cmd_timeout(); /* good enough to not time out */
#endif
		}
#ifdef CONFIG_BOOT_RETRY_TIME
		else if (nbytes == -2) {
			break;	/* timed out, exit the command	*/
		}
#endif
		else {
			char *endp;
			i = simple_strtoul(console_buffer, &endp, 16);
			nbytes = endp - console_buffer;
			if (nbytes) {
#ifdef CONFIG_BOOT_RETRY_TIME
				/* good enough to not time out
				 */
				reset_cmd_timeout();
#endif
				if (size == 4)
					*((uint   *)addr) = i;
				else if (size == 2)
					*((ushort *)addr) = i;
				else
					*((u_char *)addr) = i;
				if (incrflag)
					addr += size;
			}
		}
	} while (nbytes);

	mm_last_addr = addr;
	mm_last_size = size;
	return 0;
}

#ifndef CONFIG_CRC32_VERIFY

int do_mem_crc (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong addr, length;
	ulong crc;
	ulong *ptr;

	if (argc < 3) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	addr = simple_strtoul (argv[1], NULL, 16);
	addr += base_address;

	length = simple_strtoul (argv[2], NULL, 16);

	crc = crc32 (0, (const uchar *) addr, length);

	printf ("CRC32 for %08lx ... %08lx ==> %08lx\n",
			addr, addr + length - 1, crc);

	if (argc > 3) {
		ptr = (ulong *) simple_strtoul (argv[3], NULL, 16);
		*ptr = crc;
	}

	return 0;
}

#else	/* CONFIG_CRC32_VERIFY */

int do_mem_crc (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong addr, length;
	ulong crc;
	ulong *ptr;
	ulong vcrc;
	int verify;
	int ac;
	char **av;

	if (argc < 3) {
  usage:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	av = argv + 1;
	ac = argc - 1;
	if (strcmp(*av, "-v") == 0) {
		verify = 1;
		av++;
		ac--;
		if (ac < 3)
			goto usage;
	} else
		verify = 0;

	addr = simple_strtoul(*av++, NULL, 16);
	addr += base_address;
	length = simple_strtoul(*av++, NULL, 16);

	crc = crc32(0, (const uchar *) addr, length);

	if (!verify) {
		printf ("CRC32 for %08lx ... %08lx ==> %08lx\n",
				addr, addr + length - 1, crc);
		if (ac > 2) {
			ptr = (ulong *) simple_strtoul (*av++, NULL, 16);
			*ptr = crc;
		}
	} else {
		vcrc = simple_strtoul(*av++, NULL, 16);
		if (vcrc != crc) {
			printf ("CRC32 for %08lx ... %08lx ==> %08lx != %08lx ** ERROR **\n",
					addr, addr + length - 1, crc, vcrc);
			return 1;
		}
	}

	return 0;

}
#endif	/* CONFIG_CRC32_VERIFY */

#ifdef CONFIG_OCTEON
#include "octeon_boot.h"
#include "cvmx-bootmem-shared.h"

int do_bootmem_named_alloc (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    char *name;
    uint64_t addr = 0;
    uint64_t max_addr = 0;
    uint64_t size;
    char buf[20];

    setenv("named_block_addr", NULL);
    setenv("named_block_size", NULL);

    if (argc < 3)
    {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
    }
    name = argv[1];
    size = simple_strtoull(argv[2], NULL, 16);
    if (argc == 4)
    {
        addr = simple_strtoull(argv[3], NULL, 16);
        max_addr = addr + size;
    }

#ifdef DEBUG
    printf("Name: %s, size: 0x%Lx, addr: 0x%Lx\n", name, size, addr);
#endif

    if (!glob_phy_desc_ptr)
    {
        printf("Error, bootmem alloc not initialized!\n");
        return(1);
    }
    addr = octeon_phy_mem_named_block_alloc(glob_phy_desc_ptr, size, addr, max_addr, 0, name);
    if (!addr)
    {
        printf("Named allocation failed!\n");
        return(1);
    }
    printf("Allocated 0x%Lx bytes at address: 0x%Lx, name: %s\n", size, addr, name);

    sprintf(buf, "0x%LX", addr);
    setenv("named_block_addr", buf);
    sprintf(buf, "0x%LX", size);
    setenv("named_block_size", buf);


#ifdef DEBUG
    printf("\n\n===========================================\n");
    printf("Dump of named blocks:\n");
    octeon_phy_mem_named_block_print(glob_phy_desc_ptr);
    octeon_phy_mem_list_print(glob_phy_desc_ptr);
#endif

    return(0);
}

int do_bootmem_named_free (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    if (argc < 2)
    {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
    }
    if (!octeon_phy_mem_named_block_free(glob_phy_desc_ptr, argv[1]))
    {
        printf("Error freeing block: %s\n", argv[1]);
        return(1);
    }
    return(0);
}

int do_bootmem_named_print (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    octeon_phy_mem_named_block_print(glob_phy_desc_ptr);
    return(0);
}
#endif

/**************************************************/
#if (CONFIG_COMMANDS & CFG_CMD_MEMORY)
U_BOOT_CMD(
	md,     3,     1,      do_mem_md,
	"md      - memory display\n",
	"[.b, .w, .l] address [# of objects]\n    - memory display\n"
);


U_BOOT_CMD(
	mm,     2,      1,       do_mem_mm,
	"mm      - memory modify (auto-incrementing)\n",
	"[.b, .w, .l] address\n" "    - memory modify, auto increment address\n"
);


U_BOOT_CMD(
	nm,     2,	    1,     	do_mem_nm,
	"nm      - memory modify (constant address)\n",
	"[.b, .w, .l] address\n    - memory modify, read and keep address\n"
);

U_BOOT_CMD(
	mw,    4,    1,     do_mem_mw,
	"mw      - memory write (fill)\n",
	"[.b, .w, .l] address value [count]\n    - write memory\n"
);

U_BOOT_CMD(
	cp,    4,    1,    do_mem_cp,
	"cp      - memory copy\n",
	"[.b, .w, .l] source target count\n    - copy memory\n"
);

U_BOOT_CMD(
	cmp,    4,     1,     do_mem_cmp,
	"cmp     - memory compare\n",
	"[.b, .w, .l] addr1 addr2 count\n    - compare memory\n"
);


#ifndef CONFIG_CRC32_VERIFY

U_BOOT_CMD(
	crc32,    4,    1,     do_mem_crc,
	"crc32   - checksum calculation\n",
	"address count [addr]\n    - compute CRC32 checksum [save at addr]\n"
);

#else	/* CONFIG_CRC32_VERIFY */

U_BOOT_CMD(
	crc32,    5,    1,     do_mem_crc,
	"crc32   - checksum calculation\n",
	"address count [addr]\n    - compute CRC32 checksum [save at addr]\n"
	"-v address count crc\n    - verify crc of memory area\n"
);

#endif	/* CONFIG_CRC32_VERIFY */

U_BOOT_CMD(
	base,    2,    1,     do_mem_base,
	"base    - print or set address offset\n",
	"\n    - print address offset for memory commands\n"
	"base off\n    - set address offset for memory commands to 'off'\n"
);
#ifdef CONFIG_OCTEON
U_BOOT_CMD(
	readpci,    2,    1,     do_read_pci,
	"readpci    - read 32 bit word from 64 bit address with swapping\n",
	"\n    - read 32 bit word from 64 bit address with swapping\n"
	"readpci addr\n    - read 32 bit word from 64 bit address with swapping\n"
);
U_BOOT_CMD(
	writepci,    4,    1,     do_write_pci,
	"writepci    - write 32 bit word to 64 bit address with swapping\n",
	"\n    - write 32 bit word to 64 bit address with swapping\n"
	"writepci addr val\n    - write 32 bit word to 64 bit address with swapping\n"
);
U_BOOT_CMD(
	read64,    2,    1,     do_read64,
	"read64    - read 64 bit word from 64 bit address\n",
	"\n    - read 64 bit word from 64 bit address\n"
	"read64 addr\n    - read 64 bit word from 64 bit address\n"
);
U_BOOT_CMD(
	write64,    4,    1,     do_write64,
	"write64    - write 64 bit word to 64 bit address\n",
	"\n    - write 64 bit word to 64 bit address\n"
	"write64 addr val\n    - write 64 bit word to 64 bit address\n"
);
U_BOOT_CMD(
	namedalloc,    4,    1,     do_bootmem_named_alloc,
	"namedalloc    - Allocate a named bootmem block\n",
	"\n    - Allocate a named bootmem block\n"
	"namedalloc name size [address]\n"
    "    - Allocate a named bootmem block with a given name and size at an\n"
    "      optional fixed address.  Sets environment variables named_block_addr,\n"
    "      named_block_size to address and size of block allocated.\n"
);

U_BOOT_CMD(
	namedfree,    2,    1,     do_bootmem_named_free,
	"namedfree    - Free a named bootmem block\n",
	"\n    - Free a named bootmem block\n"
	"namedfree name\n"
    "    - Free a named bootmem block.\n"
);
U_BOOT_CMD(
	namedprint,    1,    1,     do_bootmem_named_print,
	"namedprint    - Print list of named bootmem blocks\n",
	"\n    - Print list of named bootmem blocks\n"
	"namedprint\n"
    "    - Print list of named bootmem blocks.\n"
);


U_BOOT_CMD(
	read64b,    2,    1,     do_read64b,
	"read64b    - read 8 bit word from 64 bit address\n",
	"\n    - read 8 bit word from 64 bit address \n"
	"read64b addr\n    - read 8 bit word from 64 bit address\n"
);
U_BOOT_CMD(
	read64l,    2,    1,     do_read64l,
	"read64l    - read 32 bit word from 64 bit address\n",
	"\n    - read 32 bit word from 64 bit address \n"
	"read64l addr\n    - read 32 bit word from 64 bit address\n"
);
U_BOOT_CMD(
	write64b,    4,    1,     do_write64b,
	"write64b    - write 8 bit word to 64 bit address\n",
	"\n    - write 8 bit word to 64 bit address\n"
	"write64b addr val\n    - write 8 bit word to 64 bit address\n"
);
U_BOOT_CMD(
	write64l,    4,    1,     do_write64l,
	"write64l    - write 32 bit word to 64 bit address\n",
	"\n    - write 32 bit word to 64 bit address\n"
	"write64l addr val\n    - write 32 bit word to 64 bit address\n"
);
#endif

#if defined(CONFIG_AR7100) || defined(CONFIG_AR7240) || defined(CONFIG_ARM) || defined(CONFIG_PPC) || defined(CONFIG_ATHEROS)
U_BOOT_CMD(
	readpci,    2,    1,     do_read_pci,
	"readpci    - read 32 bit word from 32 bit address with swapping\n",
	"\n    - read 32 bit word from 32 bit address with swapping\n"
	"readpci addr\n    - read 32 bit word from 32 bit address with swapping\n"
);
U_BOOT_CMD(
	writepci,    4,    1,     do_write_pci,
	"writepci    - write 32 bit word to 32 bit address with swapping\n",
	"\n    - write 32 bit word to 32 bit address with swapping\n"
	"writepci addr val\n    - write 32 bit word to 32 bit address with swapping\n"
);
#endif

U_BOOT_CMD(
	read_cmp,    4,    1,     do_read_cmp,
	"read_cmp    - read and compare memory to val\n",
	"\n    - read and compare memory to val\n"
	"read_cmp addr val\n    - read and compare memory to val\n"
);

U_BOOT_CMD(
	loop,    3,    1,    do_mem_loop,
	"loop    - infinite loop on address range\n",
	"[.b, .w, .l] address number_of_objects\n"
	"    - loop on a set of addresses\n"
);

#ifdef CONFIG_LOOPW
U_BOOT_CMD(
	loopw,    4,    1,    do_mem_loopw,
	"loopw   - infinite write loop on address range\n",
	"[.b, .w, .l] address number_of_objects data_to_write\n"
	"    - loop on a set of addresses\n"
);
#endif /* CONFIG_LOOPW */

U_BOOT_CMD(
	mtest,    4,    1,     do_mem_mtest,
	"mtest   - simple RAM test\n",
	"[start [end [pattern]]]\n"
	"    - simple RAM read/write test\n"
);

#ifdef CONFIG_MX_CYCLIC
U_BOOT_CMD(
	mdc,     4,     1,      do_mem_mdc,
	"mdc     - memory display cyclic\n",
	"[.b, .w, .l] address count delay(ms)\n    - memory display cyclic\n"
);

U_BOOT_CMD(
	mwc,     4,     1,      do_mem_mwc,
	"mwc     - memory write cyclic\n",
	"[.b, .w, .l] address value delay(ms)\n    - memory write cyclic\n"
);
#endif /* CONFIG_MX_CYCLIC */

#endif

static int check_ecc(int flag)
{
#if defined(CONFIG_NEBBIOLO) || defined(CONFIG_GRENACHE)
    uint32_t    *pRegs0 = (uint32_t*)(DEFAULT_PHOENIX_IO_BASE+PHOENIX_IO_DDR2_CHN0_OFFSET);
    uint32_t    *pRegs2 = (uint32_t*)(DEFAULT_PHOENIX_IO_BASE+PHOENIX_IO_DDR2_CHN2_OFFSET);
    uint32_t    val0, val2 = 0;
#endif
    int     rc = 0;
    
#if defined(CONFIG_NEBBIOLO) || defined(CONFIG_GRENACHE)
    val0 = pRegs0[DDR2_CONFIG_REG_RMW_ECC_LOG_HI];
#if !defined(CONFIG_GRENACHE)
    val2 = pRegs2[DDR2_CONFIG_REG_RMW_ECC_LOG_HI];
#endif
    if(flag == 0) {
        if(val0) {
            if(val0 & 0x01) {
                printf("Ch0: Uncorrectable error detected\n");
                rc++;
            }
            if(val0 & 0x02) {
                printf("Ch0: Correctable error detected\n");
                rc++;
            }
            if(val0 & 0x04) {
                printf("Ch0: ECC Log overflow occurred\n");
                rc++;
            }
        } 
//        else {
//            printf("Ch0: No ECC errors\n");
//        }
        if(val2) {
            if(val2 & 0x01) {
                printf("Ch2: Uncorrectable error detected\n");
                rc++;
            }
            if(val2 & 0x02) {
                printf("Ch2: Correctable error detected\n");
                rc++;
            }
            if(val2 & 0x04) {
                printf("Ch2: ECC Log overflow occurred\n");
                rc++;
            }
        } 
//        else {
//            printf("Ch2: No ECC errors\n");
//        }
    } else {
        if(pRegs0[DDR2_CONFIG_REG_GLB_PARAMS]&0x20)
            printf("ECC Enabled for Channel 0\n");
        else
            printf("ECC Disabled for Channel 0\n");
        if(pRegs0[DDR2_CONFIG_REG_GLB_PARAMS]&0x20)
            printf("ECC Enabled for Channel 2\n");
        else
            printf("ECC Disabled for Channel 2\n");
    }
    if(val0){
        pRegs0[DDR2_CONFIG_REG_RMW_ECC_LOG_HI] = 0;
        pRegs0[DDR2_CONFIG_REG_RMW_ECC_LOG] = 0;    // Clear the error(s)
    }
    if(val2){
        pRegs2[DDR2_CONFIG_REG_RMW_ECC_LOG_HI] = 0;
        pRegs2[DDR2_CONFIG_REG_RMW_ECC_LOG] = 0;    // Clear the error(s)
    }
#endif

    return rc;
}

#define MT_FLAG_FOREVER     0x00000001
#define MT_FLAG_WONLY       0x00000002
#define MT_FLAG_STOPFAIL    0x00000004
#if defined(CONFIG_APBOOT) && defined(CONFIG_HW_WATCHDOG)
extern void hw_watchdog_off(void);
extern void hw_watchdog_init(void);
#endif

static int ram_test(int test, unsigned long *base, unsigned long step,
    unsigned long length, unsigned long pattern)
{
    unsigned long *ptr = base;
    unsigned long i, j = 0;
    unsigned char symbol[4] = {'|', '/', '-', '\\'};

#if defined(CONFIG_APBOOT) && defined(CONFIG_HW_WATCHDOG)
    hw_watchdog_reset();
#endif    
    printf("Test:%d, Pattern = 0x%08x\n", test + 1, pattern);

    printf("Write:  ");
    for (i = 0; i < length; i += step, ptr += step) {
        *ptr = pattern;
        if (i % 0x200000 == 0) {
            printf("\b");
            printf("%c", symbol[((j++) % 4)]);
        }
    }
    printf("\bFinished");
    printf("\nRead:  ");

    ptr = base;
  
    for (i = 0; i < length; i += step, ptr += step) {
          if (*ptr != pattern) {
            printf("(Mismatch at %08lx: Expected %08lx got %08lx)\n",
                (unsigned long) ptr, pattern, *ptr);
            return 1;
        }
        if (i % 0x200000 == 0) {
            printf("\b");
            printf("%c", symbol[((j++) % 4)]);
        }
    }
    printf("\bPassed\n\n");

    return 0;
}

int do_ramtest(cmd_tbl_t *cmptp, int flag, int argc, char *argv[])
{
    uint32_t patterns[4] = {0x55aa55aa, 0xaa55aa55, 0x0000ffff, 0xffff0000};
    uint32_t mem_start, mem_end;
    int i = 0;

    mem_start = 0x80000000; 
    mem_end = 0x87e00000;

    printf("Test memory %dM, range: 0x%x -- 0x%x:\n\n", 
        (mem_end - mem_start) / (1024 * 1024), mem_start, mem_end);

    for (i = 0; i < 4; i++) {
        if (ram_test(i, (unsigned long*)mem_start, 1, (mem_end - mem_start)/sizeof(uint32_t), patterns[i])) {
            printf("Test RAM failed.\n");
            return 1;
        } 
    }
    
    printf("Test RAM successed.\n");

    return 0;
}

static int  max_fail = 20;
int do_neb_mtest(cmd_tbl_t *cmdtp, int flag, int argc, char* argv[])
{
    uint32_t*   pStart = (uint32_t*) 0x0;
    uint32_t*   pEnd = (uint32_t*) 0x0ffffffc;  // test 256MB (default)
    uint32_t    i;
	//const int field = 2 * sizeof(unsigned long);
	int iter = 1;
	uint32_t error1, error2, error3, error4, error5, error6, error7;
    int en1, en2, en3, en4, t_flags;
    int index;
    int c;
        
    t_flags = en1 = en2 = en3 = en4 = 0;
    optind = 1;
    optreset = 1;
    
#if defined(CONFIG_NEBBIOLO) || defined(CONFIG_GRENACHE)
    cfg_ext_wd_vec(WD_VEC_DISABLE);
    set_ext_wd();
#endif
#if defined(CONFIG_APBOOT) && defined(CONFIG_HW_WATCHDOG)
    hw_watchdog_off();
#endif

//    printf("nargv: %s %s %s\n",argv[0],argv[1],argv[2]);
//    printf("argv=%x\n",argv);
//    ptest(argv);
//    return 0;
    while ((c = getopt (argc, argv, "1234fhHsw")) != -1)
    {
        switch (c)
        {
        case 'h':
        case 'H':
            mtest_usage();
            return 0;
            
        case '1':
            en1 = 1;
            break;
            
        case '2':
            en2 = 1;
            break;
            
        case '3':
            en3 = 1;
            //en3 = optarg;
            break;
            
        case '4':
            en4 = 1;
            break;
 
        case 'f':
            t_flags |= MT_FLAG_FOREVER;
            break;
 
        case 's':
            t_flags |= MT_FLAG_STOPFAIL;
            break;
 
        case 'w':
            t_flags |= MT_FLAG_WONLY;
            break;
 
        case '?':
            if (isprint (optopt))
              fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
              fprintf (stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            return 1;

        default:
            break;
        }
//        printf(".");
    }
    
    //for (index = optind; index < argc; index++)
    //    printf ("Non-option argument %s\n", argv[index]);
    
    index = argc - optind + 1;
    //printf("argc= %d optind= %d \r\n", argc, optind);
    switch(index){
    
        case 1:
            printf("Using default values start 0x%x end 0x%x iter %d \r\n", 
					(uint32_t)pStart, (uint32_t)pEnd, iter);
            break;
            
        case 2:
            mtest_usage();
            return 0;
            
        case 3:
            //printf("Using default start/end values\r\n");
            pStart = (uint32_t*) simple_strtoul(argv[argc -2], NULL, 0);
            pEnd = (uint32_t*) simple_strtoul(argv[argc -1], NULL, 0);
            break;
            
        case 4:
            pStart = (uint32_t*) simple_strtoul(argv[argc -3], NULL, 0);
            pEnd = (uint32_t*) simple_strtoul(argv[argc - 2], NULL, 0);
			iter = simple_strtoul(argv[argc -1], NULL, 0);
			break;

        default:
            printf("Usage: mtest <start_addr> <end_addr>\r\n");
            return 0;
    };
    
	printf("Running following tests %d times\n", iter);
    check_ecc(1);   // Clear errors and report ECC status
    if(en1 != 0)
    {
        printf("mtest_1 (alternating a's and 5's): %x - %x \r\n",
                (uint32_t) pStart, (uint32_t) pEnd);
        printf("mtest_1 (inverted) (alternating a's and 5's): %x - %x \r\n",
                (uint32_t) pStart, (uint32_t) pEnd);
    }
    if(en2 != 0)
    {
        printf("mtest_2 (address test): %x - %x \r\n",
                (uint32_t) pStart, (uint32_t) pEnd);
        printf("mtest_2 (inverted) (address test): %x - %x \r\n",
                (uint32_t) pStart, (uint32_t) pEnd);
    }
    if(en3 != 0)
    {
        printf("mtest_3 (random test): %x - %x \r\n",
                (uint32_t) pStart, (uint32_t) pEnd);
        printf("mtest_3 (random test): %x - %x \r\n",
                (uint32_t) pStart, (uint32_t) pEnd);
    }
    
    if(en4 != 0)
    {
        printf("mtest_4 (mod3 test): %x - %x \r\n",
                (uint32_t) pStart, (uint32_t) pEnd);
    }
    
	error1 = error2 = error3 = error4 = error5 = error6 = error7 = 0;

	for (i=0; i < iter; i++) {
		
		//printf(".");
        printf("iter: %d\n",i+1);
		//fflush(stdout);
        if(en1 != 0)
        {
		    error1 += mtest_1(pStart, pEnd, 0, t_flags);
            error1 += check_ecc(0);
		    error2 += mtest_1(pStart, pEnd, 1, t_flags);
            error2 += check_ecc(0);
		    if (ctrlc()) {
			    printf ("\nAbort\n");
                en1 = en2 = en3 = en4 = iter = 0;
			    break;
		    }
        }
        if(en2 != 0)
        {
		    error3 += mtest_2(pStart, pEnd, 0, t_flags);
            error3 += check_ecc(0);
		    error4 += mtest_2(pStart, pEnd, 1, t_flags);
            error4 += check_ecc(0);
		    if (ctrlc()) {
			    printf ("\nAbort\n");
                en1 = en2 = en3 = en4 = iter = 0;
			    break;
		    }
        }
        if(en3 != 0)
        {
            error5 += mtest_3(pStart, pEnd, 0, t_flags);
            error5 += check_ecc(0);
            error6 += mtest_3(pStart, pEnd, 1, t_flags);
            error6 += check_ecc(0);
		    if (ctrlc()) {
			    printf ("\nAbort\n");
                en1 = en2 = en3 = en4 = iter = 0;
			    break;
		    }
        }
        if(en4 != 0)
        {
            error7 += mtest_4(pStart, pEnd, 0, t_flags);
            error7 += check_ecc(0);
		    if (ctrlc()) {
			    printf ("\nAbort\n");
                en1 = en2 = en3 = en4 = iter = 0;
			    break;
		    }
        }
	}
        
	printf("\n");

	if (error1) {
		printf("mtest_1 failed %d times\n", error1);
	}

	if (error2) {
		printf("mtest_1 (inverted) failed %d times\n", error2);
	}

	if (error3) {
		printf("mtest_2 failed %d times\n", error3);
	}

	if (error4) {
		printf("mtest_2 (inverted) failed %d times\n", error4);
	}

	if (error5) {
		printf("mtest_3 failed %d times\n", error5);
	}

	if (error6) {
		printf("mtest_3 (inverted) failed %d times\n", error6);
	}
    
	if (error7) {
		printf("mtest_4 failed %d times\n", error7);
	}
#if defined(CONFIG_NEBBIOLO) || defined(CONFIG_GRENACHE)
    cfg_ext_wd_vec(WD_VEC_BOOT_CUR);
    set_ext_wd();
#endif    
#if defined(CONFIG_APBOOT) && defined(CONFIG_HW_WATCHDOG)
    hw_watchdog_init();
#endif
    return 0;   
}

// test every location with a's and 5's
int mtest_1(uint32_t* start, uint32_t* end, int invert, int t_flags)
{
    int result = 0;
    int errors = 0;
    uint32_t*   pMem;
    uint32_t    pattern, val;

    do {    
        if(invert != 0)
            pattern = 0xaa55aa55;
        else
            pattern = 0x55aa55aa;
    
        pMem = start;
        while(pMem < end)
        {
            *pMem++ = pattern;
            *pMem++ = pattern;  // write 64 bits before inverting pattern
            pattern = ~pattern;
        }
    
        if((t_flags & MT_FLAG_WONLY) == 0) { // write only flag not set
            pMem = start;
            if(invert != 0)
                pattern = 0xaa55aa55;
            else
                pattern = 0x55aa55aa;
        
            while(pMem < end)
            {
                val = *pMem++;
                if(val != pattern)
                {
                    printf("\nmtest_1 error: at:%x expect:%x , read:%x\r\n", (uint32_t) pMem-4, pattern, val);
                    result = 1;
                    errors ++;
                    if(errors >= max_fail) break;
                }
                val = *pMem++;
                if(val != pattern)
                {
                    printf("\nmtest_1 error: at:%x expect:%x , read:%x\r\n", (uint32_t) pMem-4, pattern, val);
                    result = 1;
                    errors ++;
                    if(errors >= max_fail) break;
                }
                pattern = ~pattern;
            }
        }
		if ( tstc() || ((t_flags & MT_FLAG_STOPFAIL)&& (errors != 0)) ) {
			break;
        }
    }while(t_flags & MT_FLAG_FOREVER);
    return result;
}

//  test every location with it's address/address complement
int mtest_2(uint32_t* start, uint32_t* end, int invert, int t_flags)
{
    int result = 0;
    int errors = 0;
    uint32_t*   pMem;
    uint32_t    pattern, val;
    
    do {   
        pMem = start;
        while(pMem != end)
        {
            if(invert != 0)
                pattern = ~((uint32_t) pMem);
            else
                pattern = (uint32_t) pMem;
            
            *pMem++ = pattern;
        }
 
        if((t_flags & MT_FLAG_WONLY) == 0) { // write only flag not set
            pMem = start;
            while(pMem != end)
            {
                if(invert != 0)
                    pattern = ~((uint32_t) pMem);
                else
                    pattern = (uint32_t) pMem;
            
                val = *pMem++;
                if(val != pattern)
                {
                    printf("\nmtest_2 error: at:%x expect:%x , read:%x\r\n", (uint32_t) pMem-4, pattern, val);
                    result = 1;
                    errors ++;
                    if(errors >= max_fail) break;
                }
            }
        }
		if ( tstc() || ((t_flags & MT_FLAG_STOPFAIL)&& (errors != 0)) ) {
			break;
        }
    }while(t_flags & MT_FLAG_FOREVER);
    return result;
}

// test every location using a block of random data
int mtest_3(uint32_t* start, uint32_t* end, int invert, int t_flags)
{
    int result = 0;
    int errors = 0;
    uint32_t    tdata[32], val;
    uint32_t*   pMem;
    int i;
 
    do {   
        for(i = 0; i < 32; i++) {
            tdata[i] = rand();
            //printf("%x\n",tdata[i]);
        }   
        pMem = start;
        while(pMem != end)
        {
    
            i = ((uint32_t) pMem) & 0x1f;
            /*
            if(invert != 0)
                pattern = ~tdata[i];
            else
                pattern = tdata[i];
            */            
            *pMem++ = tdata[i];
        }
    
        if((t_flags & MT_FLAG_WONLY) == 0) { // write only flag not set
            pMem = start;
            while(pMem != end)
            {
                i = ((uint32_t) pMem) & 0x1f;            
                val = *pMem++;
                if(val != tdata[i])
                {
                    printf("\nmtest_3 error: at:%x expect:%x , read:%x\r\n", (uint32_t) pMem-4, tdata[i], val);
                    result = 1;
                    errors ++;
                    if(errors >= max_fail) break;
                }
            }
        }
		if ( tstc() || ((t_flags & MT_FLAG_STOPFAIL)&& (errors != 0)) ) {
			break;
        }
    }while(t_flags & MT_FLAG_FOREVER);
    
    return result;
}

// test every location using a MOD3 algorithm
int mtest_4(uint32_t* start, uint32_t* end, int invert, int t_flags)
{
    int result = 0;

    if(t_flags)
        printf("INFO: mtest4 doesn't currently support these test flags: %x\n",t_flags);
            
    doMod3Test1stPass(start, end);
    result = doMod3Test2ndPass(start, end);
    if(result == 0) {
        result = doMod3Test3rdPass(start, end);
        if(result == 0) {
            result = doMod3TestLastVerify(start, end);
        }
    }
    
    return result;
}

// walk a 1 and 0 on the data bus, walk a 1 on on the addr bus
int mtest_quick(uint32_t* start, uint32_t* end, int t_flags)
{
    int result = 0;
    int errors = 0;
    int iter = 0;
    int invert, i;
    uint32_t*   pMem;
    uint32_t    pattern, val;

    // data bus tests in a single mem location
    // The memory parts are interleaved such that addrs ending in 0x0 or 0x8 are one
    // group of devices and 0x4 or 0xC belong to the other group. Therefore, to test
    // the data bus to all parts, we have to test at start and start + 0x4.
    pMem = start;
    for( i = 0; i <= 1; i++ )
    {
        for( invert = 0; invert <= 1; invert++ )
        {
            do {    
                for( iter = 0; iter < 32; iter++ )
                {
                    if( invert != 0 )
                        pattern = ~(0x1 << iter);  // walking 0
                    else
                        pattern = 0x1 << iter;  // walking 1

                    *pMem = pattern;
            
                    if((t_flags & MT_FLAG_WONLY) == 0) { // write only flag not set
                        val = *pMem;
                        if(val != pattern)
                        {
                            printf("\nmtest_quick error: at:%x expect:%x , read:%x\r\n", (uint32_t) pMem-4, pattern, val);
                            result = 1;
                            errors++;
                            if(errors >= max_fail) break;
                        }
                    }
                }
                if (tstc()) {
                    break;
                }
            }while(t_flags & MT_FLAG_FOREVER);
        }

        pMem++;  // see the above note
    }

    // walk a bit along the addr bus
    do {   
        pMem = start;
        for( iter = 0; iter < 32; iter++ )
        {
            pattern = 0x1 << iter;

            if( ((uint32_t) pMem + pattern <= (uint32_t) end) && 
                ((uint32_t) pMem + pattern >= (uint32_t) start) )
            {
                if( pattern <= 0xFF )
                {
                    *(uint8_t*) ((uint32_t) pMem + pattern) = (uint8_t) pattern;
                }
                else if( pattern <= 0xFFFF )
                {
                    *(uint16_t*) ((uint32_t) pMem + pattern) = (uint16_t) pattern;
                }
                else
                {
                    *(uint32_t*) ((uint32_t) pMem + pattern) = pattern;
                }

                //DEBUG
                //printf( "DBG: wrote pattern %#x to %#x\n", 
                //        pattern, (uint32_t) pMem + pattern );
            }
        }
 
        if((t_flags & MT_FLAG_WONLY) == 0) { // write only flag not set
            pMem = start;
            for( iter = 0; iter < 32; iter++ )
            {
                pattern = 0x1 << iter;
                
                if( ((uint32_t) pMem + pattern <= (uint32_t) end) && 
                    ((uint32_t) pMem + pattern >= (uint32_t) start) )
                {
                    pMem = (uint32_t*) ((uint32_t) start + pattern);
                    
                    if( pattern <= 0xFF )
                    {
                        val = (uint32_t) (*(uint8_t*) pMem);
                    }
                    else if( pattern <= 0xFFFF )
                    {
                        val = (uint32_t) (*(uint16_t*) pMem);
                    }
                    else
                    {
                        val = *pMem;
                    }

                    //DEBUG
                    //printf( "DBG: read %#x from %#x, expected %#x\n",
                    //        val, (uint32_t) pMem, pattern);

                    if(val != pattern)
                    {
                        printf("\nmtest_quick error: at:%x expect:%x , read:%x\r\n", (uint32_t) pMem-4, pattern, val);
                        result = 1;
                        errors ++;
                        if(errors >= max_fail) break;
                    }
                }
            }
        }
		if (tstc()) {
			break;
        }
    }while(t_flags & MT_FLAG_FOREVER);

    // walking a 0 on the addr bus would be painful without masking addrs

    return result;
}

static void mtest_usage(void)
{
    printf("Usage: mtest [-1234fw] <start_addr> <end_addr> <iter>\n");
    printf("-1     test with alternating 0xaaaaaaaa/0x55555555 pattern\n");
    printf("-2     test with address/inverted address pattern\n");
    printf("-3     test with random data pattern\n");
    printf("-4     test with modulo 3 XOR algorithm\n");
    printf("-f     loop forever...\n");
    printf("-w     write-only operation (for scope loop)\n");
}

void doMod3Test1stPass( uint32_t* start, uint32_t* end)
{
    uint32_t loop;
    uint32_t maxCnt          = (end - start);
    uint8_t  index           = 0;
    uint32_t dataArray[3] = {0x6db6db6d, 0xb6db6db6, 0xdb6db6db};

    for ( loop  = 0; loop  < maxCnt; loop ++, start++, index++){
        if ( 3 == index ){
            index = 0;
        }     
        *start = dataArray[index];
    }

    return;
}

int doMod3Test2ndPass (uint32_t* start, uint32_t* end)
{
    int      rtnStatus = 0;
    int errors = 0;
    uint32_t loop;
    uint32_t readData;
    uint32_t maxCnt          = (end - start);
    uint8_t  index           = 0;
    uint32_t dataArray[3] = {0x6db6db6d, 0xb6db6db6, 0xdb6db6db};

    for (loop  = 0; (loop  < maxCnt) && (rtnStatus < 10); loop ++, start++) {
    
        readData = *start;
        
        if (readData != dataArray[index]) {
            printf ( "ERROR: Data miscompare during 1st verify cycle!\r\n");
            printf ( "     : address    = 0x%x\r\n", (uint32_t) start);
            printf ( "     : write data = 0x%x\r\n", dataArray [index]);
            printf ( "     : read  data = 0x%x\r\n", readData);
            rtnStatus++;  // return error status
            errors ++;
            if(errors >= max_fail) break;
        }

        index++;        
        if (3 == index) index = 0;
        *start = dataArray[index] ^ readData;
    }
    return (rtnStatus);
}

int doMod3Test3rdPass (uint32_t* start, uint32_t* end)
{
    int      rtnStatus = 0;
    int      errors = 0;
    uint32_t loop;
    uint32_t readData;
    uint32_t maxCnt          = (end - start);
    uint8_t  index           = 2;
    uint32_t dataArray[3] = {0x6db6db6d, 0xb6db6db6, 0xdb6db6db};

    for(loop  = 0; (loop  < maxCnt) && (rtnStatus < 10); loop ++, start++) {
        readData = *start;
        
        if(readData != dataArray[index])
        {
            printf ( "ERROR! Data miscompare during 2nd verify cycle!\r\n");
            printf ( "     : address    = 0x%x\r\n", (uint32_t) start);
            printf ( "     : write data = 0x%x\r\n", dataArray[index]);
            printf ( "     : read  data = 0x%x\r\n", readData);
            rtnStatus++;  // return error status
            errors ++;
            if(errors >= max_fail) break;
        }

        index++;
        if ( 3 == index ) index = 0;   // valid values are 0, 1, 2    
        *start = dataArray[index] ^ readData;

    }

    return ( rtnStatus );
}

int doMod3TestLastVerify (uint32_t* start, uint32_t* end)
{
    int      rtnStatus = 0;
    int      errors = 0;
    uint32_t loop;
    uint32_t readData;
    uint32_t maxCnt          = (end - start);
    uint8_t  index           = 1;
    uint32_t dataArray[3] = {0x6db6db6d, 0xb6db6db6, 0xdb6db6db};

    for(loop  = 0; (loop  < maxCnt) && (rtnStatus < 10); loop ++, start++) {
        readData = *start;
        
        if(readData != dataArray[index])
        {
            printf ( "ERROR! Data miscompare during 2nd verify cycle!\r\n");
            printf ( "     : address    = 0x%x\r\n", (uint32_t) start);
            printf ( "     : write data = 0x%x\r\n", dataArray[index]);
            printf ( "     : read  data = 0x%x\r\n", readData);
            rtnStatus++;  // return error status
            errors ++;
            if(errors >= max_fail) break;
        }
        index++;
        if ( 3 == index ) index = 0;
    }

    return ( rtnStatus );
}

U_BOOT_CMD(
	mt,      8,      1,      do_neb_mtest,
	"mt    - Run the memory test suite.\n",
    "Usage: mt [-1 -2 -3 -4] <start_addr> <end_addr> <iter>\n"
    "-1     test with alternating 0xaaaaaaaa/0x55555555 pattern\n"
    "-2     test with address/inverted address pattern\n"
    "-3     test with random data pattern\n"
    "-4     test with modulo 3 XOR algorithm\n"
    "-f     loop forever...\n"
    "-w     write-only operation (for scope loop)\n"
    "<iter> run selected tests for this number of passes\n"
);

#if defined(CONFIG_AR7100)
/* RAM diagnose for entire memory */
U_BOOT_CMD(
    ramtest,    1,    1,    do_ramtest,
    "ramtest    - RAM diagnose for entire memory\n",
    "Usage: ramtest\n"
);
#endif /* CONFIG_AR7100 */

#endif	/* CFG_CMD_MEMORY */

#if defined(CONFIG_ATH_NAND_FL) || defined(ATH_DUAL_FLASH)
int do_nfcprd (int rd, cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong	addr, dest, count;
	int	i;
	unsigned char *p;

	if (argc != 4) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	addr = simple_strtoul(argv[1], NULL, 16);

	dest = simple_strtoul(argv[2], NULL, 16);

	count = simple_strtoul(argv[3], NULL, 16);

	if (count == 0) {
		puts ("Zero length ???\n");
		return 1;
	}

#ifdef ATH_DUAL_FLASH
#	define	nand_write	ath_nand_write_buff
#	define	nand_read	ath_nand_read_buff
#else
#	define	nand_write	write_buff
#	define	nand_read	read_buff
#endif
	switch (rd) {
		case 0: return nand_write(NULL, addr, dest, count);
		case 1: return nand_read(NULL, addr, dest, count);
		case 2: return nand_write((void *)1, addr, dest, count);
		case 3: return nand_read((void *)1, addr, dest, count);
	}
}

int do_nfcp ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return do_nfcprd(0, cmdtp, flag, argc, argv);
}

int do_nfrd ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return do_nfcprd(1, cmdtp, flag, argc, argv);
}

int do_nfcpx( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return do_nfcprd(2, cmdtp, flag, argc, argv);
}

int do_nfrdx( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	return do_nfcprd(3, cmdtp, flag, argc, argv);
}


U_BOOT_CMD(
	nfcp,    4,    1,    do_nfcp,
	"nfcp	- copy to nand flash\n",
	"ram_addr flash_addr count\n	- copy memory to nand flash\n"
);

U_BOOT_CMD(
	nfrd,    4,    1,    do_nfrd,
	"nfrd	- copy from nand flash\n",
	"ram_addr flash_addr count\n	- read to memory from nand flash\n"
);

U_BOOT_CMD(
	nfcpx,    4,    1,    do_nfcpx,
	"nfcpx	- copy to nand flash (include spare bytes)\n",
	"ram_addr flash_addr count\n	- copy memory to nand flash (include spare bytes)\n"
);

U_BOOT_CMD(
	nfrdx,    4,    1,    do_nfrdx,
	"nfrdx	- copy from nand flash (include spare bytes)\n",
	"ram_addr flash_addr count\n	- read to memory from nand flash (include spare bytes)\n"
);

#endif // CONFIG_ATH_NAND_FL || ATH_DUAL_FLASH
