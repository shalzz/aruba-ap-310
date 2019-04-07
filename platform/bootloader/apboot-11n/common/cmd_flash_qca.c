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
 * FLASH support
 */
#include <common.h>
#if defined(CONFIG_MACH_IPQ806x)
#include <command.h>

#ifdef CONFIG_HAS_DATAFLASH
#include <dataflash.h>
#endif

#ifndef CONFIG_APBOOT
#if defined(CONFIG_CMD_MTDPARTS)
#include <jffs2/jffs2.h>

/* partition handling routines */
int mtdparts_init(void);
int mtd_id_parse(const char *id, const char **ret_id, u8 *dev_type, u8 *dev_num);
int find_dev_and_part(const char *id, struct mtd_device **dev,
		u8 *part_num, struct part_info **part);
#endif
#endif

#ifndef CONFIG_SYS_NO_FLASH
#include <flash.h>
#include <mtd/cfi_flash.h>
extern flash_info_t flash_info[];	/* info for FLASH chips */

/*
 * This function computes the start and end addresses for both
 * erase and protect commands. The range of the addresses on which
 * either of the commands is to operate can be given in two forms:
 * 1. <cmd> start end - operate on <'start',  'end')
 * 2. <cmd> start +length - operate on <'start', start + length)
 * If the second form is used and the end address doesn't fall on the
 * sector boundary, than it will be adjusted to the next sector boundary.
 * If it isn't in the flash, the function will fail (return -1).
 * Input:
 *    arg1, arg2: address specification (i.e. both command arguments)
 * Output:
 *    addr_first, addr_last: computed address range
 * Return:
 *    1: success
 *   -1: failure (bad format, bad address).
*/
static int
addr_spec(char *arg1, char *arg2, ulong *addr_first, ulong *addr_last)
{
	char *ep;
	char len_used; /* indicates if the "start +length" form used */

	*addr_first = simple_strtoul(arg1, &ep, 16);
	if (ep == arg1 || *ep != '\0')
		return -1;

	len_used = 0;
	if (arg2 && *arg2 == '+'){
		len_used = 1;
		++arg2;
	}

	*addr_last = simple_strtoul(arg2, &ep, 16);
	if (ep == arg2 || *ep != '\0')
		return -1;

	if (len_used){
		/*
		 * *addr_last has the length, compute correct *addr_last
		 * XXX watch out for the integer overflow! Right now it is
		 * checked for in both the callers.
		 */
		*addr_last = *addr_first + *addr_last - 1;

		/*
		 * It may happen that *addr_last doesn't fall on the sector
		 * boundary. We want to round such an address to the next
		 * sector boundary, so that the commands don't fail later on.
		 */

	//	if (flash_sect_roundb(addr_last) > 0)
	//		return -1;
	} /* "start +length" from used */

	return 1;
}

int do_flinfo ( cmd_tbl_t *cmdtp, int flag, int argc, char * argv[])
{
#ifndef CONFIG_SYS_NO_FLASH
	ulong bank;
#endif

#ifdef CONFIG_HAS_DATAFLASH
	dataflash_print_info();
#endif

#ifndef CONFIG_SYS_NO_FLASH
	if (argc == 1) {	/* print info for all FLASH banks */
		for (bank=0; bank <CONFIG_SYS_MAX_FLASH_BANKS; ++bank) {
			printf ("\nBank # %ld: ", bank+1);

			flash_print_info (&flash_info[bank]);
		}
		return 0;
	}

	bank = simple_strtoul(argv[1], NULL, 16);
	if ((bank < 1) || (bank > CONFIG_SYS_MAX_FLASH_BANKS)) {
		printf ("Only FLASH Banks # 1 ... # %d supported\n",
			CONFIG_SYS_MAX_FLASH_BANKS);
		return 1;
	}
	printf ("\nBank # %ld: ", bank);
	flash_print_info (&flash_info[bank-1]);
#endif /* CONFIG_SYS_NO_FLASH */
	return 0;
}

extern int do_nand(cmd_tbl_t *, int, int, char **);

int do_flerase (cmd_tbl_t *cmdtp, int flag, int argc, char * argv[])
{
#ifndef CONFIG_SYS_NO_FLASH
	ulong  addr_first, addr_last;
#ifndef CONFIG_APBOOT
#if defined(CONFIG_CMD_MTDPARTS)
	struct mtd_device *dev;
	struct part_info *part;
	u8 dev_type, dev_num, pnum;
#endif
#endif
	int rcode = 0;
    char *av[4];
    char obuf[32], sbuf[32];
	if (argc < 2)
		return cmd_usage(cmdtp);

	if (strcmp(argv[1], "all") == 0) {
#ifdef CONFIG_APBOOT
		{
			//char *av[2];
			extern int do_factory_reset(cmd_tbl_t *, int, int, char **);
			printf("Performing factory reset\n");
			av[0] = "factory_reset";
			av[1] = 0;
			return do_factory_reset(0, 0, 1, av);
		}
#else
        {
            printf("Erase Flash ...\n");
            av[0] = "nand";
            av[1] = "erase.chip";
            rcode = do_nand(0, 0, 2, av);
        }
#endif
		return rcode;
	}
#ifndef CONFIG_APBOOT
#if defined(CONFIG_CMD_MTDPARTS)
	/* erase <part-id> - erase partition */
	if ((argc == 2) && (mtd_id_parse(argv[1], NULL, &dev_type, &dev_num) == 0)) {
		mtdparts_init();
		if (find_dev_and_part(argv[1], &dev, &pnum, &part) == 0) {
			if (dev->id->type == MTD_DEV_TYPE_NOR) {
				bank = dev->id->num;
				info = &flash_info[bank];
				addr_first = part->offset + info->start[0];
				addr_last = addr_first + part->size - 1;

				printf ("Erase Flash Partition %s, "
						"bank %ld, 0x%08lx - 0x%08lx ",
						argv[1], bank, addr_first,
						addr_last);

				rcode = flash_sect_erase(addr_first, addr_last);
				return rcode;
			}

			printf("cannot erase, not a NOR device\n");
			return 1;
		}
	}
#endif
#endif

	if (argc != 3)
		return cmd_usage(cmdtp);

	if (strcmp(argv[1], "bank") == 0) {
#ifdef CONFIG_APBOOT
		printf("Command is not supported in APBoot\n");
		return 1;
#endif
	}

	if (addr_spec(argv[1], argv[2], &addr_first, &addr_last) < 0){
		printf ("Bad address format\n");
		return 1;
	}

	if (addr_first >= addr_last)
		return cmd_usage(cmdtp);

    av[0] = "nand";
    av[1] = "erase.spread";
    snprintf(obuf, sizeof(obuf), "0x%x", addr_first);
    snprintf(sbuf, sizeof(sbuf), "0x%x", (addr_last - addr_first + 1));
    av[2] = obuf;
    av[3] = sbuf;
    
    rcode = do_nand(0, 0, 4, av);
	return rcode;
#else
	return 0;
#endif /* CONFIG_SYS_NO_FLASH */
}

int do_protect (cmd_tbl_t *cmdtp, int flag, int argc, char * argv[])
{
    printf("\n");
    return 0;
}
#endif
/**************************************************/
#if defined(CONFIG_CMD_MTDPARTS) && !defined(CONFIG_APBOOT)
# define TMP_ERASE	"erase <part-id>\n    - erase partition\n"
# define TMP_PROT_ON	"protect on <part-id>\n    - protect partition\n"
# define TMP_PROT_OFF	"protect off <part-id>\n    - make partition writable\n"
#else
# define TMP_ERASE	/* empty */
# define TMP_PROT_ON	/* empty */
# define TMP_PROT_OFF	/* empty */
#endif

U_BOOT_CMD(
	flinfo,    2,    1,    do_flinfo,
	"print FLASH memory information",
	"\n    - print information for all FLASH memory banks\n"
	"flinfo N\n    - print information for FLASH memory bank # N"
);

U_BOOT_CMD(
	erase,   3,   1,  do_flerase,
	"erase FLASH memory",
	"start end\n"
	"    - erase FLASH from addr 'start' to addr 'end'\n"
	"erase start +len\n"
	"    - erase FLASH from addr 'start' to the end of sect "
	"w/addr 'start'+'len'-1\n"
	"erase N:SF[-SL]\n    - erase sectors SF-SL in FLASH bank # N\n"
	"erase bank N\n    - erase FLASH bank # N\n"
	TMP_ERASE
	"erase all\n    - erase all FLASH banks"
);

U_BOOT_CMD(
	protect,  4,  1,   do_protect,
	"enable or disable FLASH write protection",
	"on  start end\n"
	"    - protect FLASH from addr 'start' to addr 'end'\n"
	"protect on start +len\n"
	"    - protect FLASH from addr 'start' to end of sect "
	"w/addr 'start'+'len'-1\n"
	"protect on  N:SF[-SL]\n"
	"    - protect sectors SF-SL in FLASH bank # N\n"
	"protect on  bank N\n    - protect FLASH bank # N\n"
	TMP_PROT_ON
	"protect on  all\n    - protect all FLASH banks\n"
	"protect off start end\n"
	"    - make FLASH from addr 'start' to addr 'end' writable\n"
	"protect off start +len\n"
	"    - make FLASH from addr 'start' to end of sect "
	"w/addr 'start'+'len'-1 wrtable\n"
	"protect off N:SF[-SL]\n"
	"    - make sectors SF-SL writable in FLASH bank # N\n"
	"protect off bank N\n    - make FLASH bank # N writable\n"
	TMP_PROT_OFF
	"protect off all\n    - make all FLASH banks writable"
);

#undef	TMP_ERASE
#undef	TMP_PROT_ON
#undef	TMP_PROT_OFF
#endif
