/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.

*******************************************************************************/


#include <common.h>
#include <command.h>
#include <config.h>
#include <image.h>
#include <asm/byteorder.h>
#include <ide.h>
#include <ata.h>
#include "xor/mvXor.h"


#if defined(CONFIG_CMD_EXT2)

#define DOS_PART_MAGIC_OFFSET	0x1fe
#define PARTITION_CRC_OFF	0x0
#ifdef CONFIG_ISO_PARTITION
/* Make the buffers bigger if ISO partition support is enabled -- CD-ROMS
   have 2048 byte blocks */
#define DEFAULT_SECTOR_SIZE   2048
#else
#define DEFAULT_SECTOR_SIZE	512
#endif

#define MAX_BOOT_PART	   2

extern block_dev_desc_t * ide_get_dev(int dev);

static 	char  tmp_string[100];
static 	char  tmp_string1[100];

int do_bootext2 (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

	int dev,part,active_part= -1 ;
	ulong addr;
	volatile disk_partition_t info[2];
    	volatile char *env,*env2;
	block_dev_desc_t *dev_desc;
	ulong boot_part[MAX_BOOT_PART];
	char *ep;

	if (argc != 5)
	{
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	dev = simple_strtoul(argv[1], &ep, 16);
	if (*ep) {
		if (*ep != ':')
		{
			printf ("Usage:\n%s\n", cmdtp->usage);
			return(1);
		}

		for (part=0 ; part < MAX_BOOT_PART-1 ; part++)
		{
			ep++;
			boot_part[part] = (ulong)simple_strtoul(ep, &ep, 16);
			if (*ep != ',') {
				printf ("Usage:\n%s\n", cmdtp->usage);
				return(1);
			}

		}
		boot_part[part] = (ulong)simple_strtoul(++ep, NULL, 16);

	}
	else
	{
		puts ("\n** Invalid boot device, use `dev:boot_part1,boot_part2' **\n");
		return(1);
	}

	addr = simple_strtoul(argv[2], NULL, 16);

	dev_desc = (dev >= CONFIG_SYS_IDE_MAXDEVICE) ?  NULL : ide_get_dev(dev);

	if (dev_desc == NULL)
	{
		printf("Non valid dev number %x\n",dev);
		return 1;
	}


	/* Initialize IDE */
	sprintf(tmp_string,"ide reset");
	run_command(tmp_string,0);


	/* Search for Active partition in partition #1 and partition #2*/
	for (part = 0; part < MAX_BOOT_PART ;part++ )
	{
		if (get_partition_info (dev_desc, 
								boot_part[part], 
								(disk_partition_t*)&info[part]))
		{
			continue;
		}

		if (info[part].boot_ind ) 
		{
			active_part = part;
			break;
		}
	}

	/* If no active partition then return */
	if (active_part == -1)
	{
		printf("No active partition on %lx and %lx\n",
				boot_part[0],boot_part[1]);
		return 1;
	}

	/* Load /boot/uImage from active_part to addr */
	sprintf(tmp_string,"ext2load ide %x:%lx %lx %s", dev, boot_part[active_part], addr, argv[3]);
	printf("%s\n",tmp_string);
	run_command(tmp_string,0);

	sprintf(tmp_string,"root=%s%ld ro",argv[4],boot_part[active_part]);
    	setenv("bootargs_root",tmp_string);

	env = getenv("bootargs");
	env2 = getenv("bootargs_root");
	/* Save bootargs for secondary boot option if boot from active partition will fail */
	sprintf(tmp_string1,"%s",env);
	sprintf(tmp_string,"%s %s",env,env2);
    	setenv("bootargs",tmp_string);

	sprintf(tmp_string,"bootm %lx", addr);

	printf("%s\n",tmp_string);
	run_command(tmp_string,0);

	/* If we get here then first boot fail */
	active_part = (active_part + 1)%MAX_BOOT_PART;

	sprintf(tmp_string,"ext2load ide %x:%lx %lx %s", dev, boot_part[active_part], addr, argv[3]);
	printf("%s\n",tmp_string);
	run_command(tmp_string,0);

	sprintf(tmp_string,"root=%s%ld ro",argv[4],boot_part[active_part]);
    	setenv("bootargs_root",tmp_string);
	env2 = getenv("bootargs_root");
	sprintf(tmp_string,"%s %s %s",tmp_string1,env2, "boot_failure");
    	setenv("bootargs",tmp_string);

	sprintf(tmp_string,"bootm %lx", addr);

	printf("Starting secondary boot...\n");
	printf("%s\n",tmp_string);
	run_command(tmp_string,0);
	printf("Secondary boot fail...\n");
	return 1;
}

U_BOOT_CMD(
 	bootext2,	5,	0,	do_bootext2,
 	"bootext2    dev:boot_part1,boot_part2 addr boot_image linux_dev_name \n",
 	"dev:boot_part1,boot_part2 addr boot_image linux_dev_name\n"
	"- boot boot_image from active ext2 partition\n"
);


#endif



