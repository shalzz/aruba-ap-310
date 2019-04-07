/*
 * Copyright (C) 2009 Freescale Semiconductor, Inc.
 *
 * Author: Mingkai Hu (Mingkai.hu@freescale.com)
 * Based on stmicro.c by Wolfgang Denk (wd@denx.de),
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com),
 * and  Jason McMullan (mcmullan@netapp.com)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#ifdef CONFIG_IPROC
#include <malloc.h>
#include <spi_flash_iproc.h>
#include <watchdog.h>

#include "spi_flash_internal.h"

#define CMD_SP_WREN             0x06	/* Write Enable */

#define CMD_4FAST_READ          0XC
#define CMD_SP_PP               0x12    /* 4B address page programming */
#define CMD_4READ               0x13    /* 4B address read */
#define CMD_4K_4ERASE           0x21    /* 4B address erase */
#define CMD_4ERASE              0xDC    /* 4B address erase */

struct spansion_spi_flash_params {
	u16 idcode1;
	u16 idcode2;
	u16 pages_per_sector;
	u16 nr_sectors;
	const char *name;
    u8  is_uniform_sector;
    sect_grp_desc_t sect_grp_desc[MAX_SECT_GRP];
};

static const struct spansion_spi_flash_params spansion_spi_flash_table[] = {
	{
		.idcode1 = 0x0213,
		.idcode2 = 0,
		.pages_per_sector = 256,
		.nr_sectors = 16,
		.name = "S25FL008A",
		.is_uniform_sector = 1,
	},
	{
		.idcode1 = 0x0214,
		.idcode2 = 0,
		.pages_per_sector = 256,
		.nr_sectors = 32,
		.name = "S25FL016A",
		.is_uniform_sector = 1,
	},
	{
		.idcode1 = 0x0215,
		.idcode2 = 0,
		.pages_per_sector = 256,
		.nr_sectors = 64,
		.name = "S25FL032A",
		.is_uniform_sector = 1,
	},
	{
		.idcode1 = 0x0216,
		.idcode2 = 0,
		.pages_per_sector = 256,
		.nr_sectors = 128,
		.name = "S25FL064A",
		.is_uniform_sector = 1,
	},
	{
		.idcode1 = 0x2018,
		.idcode2 = 0x0301,
		.pages_per_sector = 256,
		.nr_sectors = 256,
		.name = "S25FL128P_64K",
		.is_uniform_sector = 1,
	},
	{
		.idcode1 = 0x2018,
		.idcode2 = 0x0300,
		.pages_per_sector = 1024,
		.nr_sectors = 64,
		.name = "S25FL128P_256K",
		.is_uniform_sector = 1,
	},
	{
		.idcode1 = 0x0215,
		.idcode2 = 0x4d00,
		.pages_per_sector = 256,
		.nr_sectors = 64,
		.name = "S25FL032P",
		.is_uniform_sector = 1,
	},
	{
		.idcode1 = 0x2018,
		.idcode2 = 0x4d01,
		.pages_per_sector = 256,
		.nr_sectors = 256,
		.name = "S25FL128S_64K",
		.is_uniform_sector = 1,
	},
	{
		.idcode1 = 0x2018,
		.idcode2 = 0x4d01,
		.pages_per_sector = 256,
		.nr_sectors = 256,
		.name = "S25FL128S_64K",
		.is_uniform_sector = 1,
	},
	{
		.idcode1 = 0x0219,
		.idcode2 = 0x4d01,
		.pages_per_sector = 256,
		.nr_sectors = 542,
		.name = "S25FL256S_64K",
		.is_uniform_sector = 0,
		.sect_grp_desc[0].sect_size = SECT_SIZE_4K,
		.sect_grp_desc[0].start_sector = 0,
		.sect_grp_desc[0].nr_sector = 32,
		.sect_grp_desc[1].sect_size = SECT_SIZE_64K,
		.sect_grp_desc[1].start_sector = 32,
		.sect_grp_desc[1].nr_sector = 510,
		.sect_grp_desc[2].sect_size = SECT_SIZE_INVAL,
	},
	{
		.idcode1 = 0x2018,
		.idcode2 = 0x4d01,
		.pages_per_sector = 256,
		.nr_sectors = 256,
		.name = "S25FL129P_64K",
		.is_uniform_sector = 1,
	},
	{
		.idcode1 = 0x2019,
		.idcode2 = 0x4d01,
		.pages_per_sector = 256,
		.nr_sectors = 512,
		.name = "S25FL256S",
		.is_uniform_sector = 1,
	},
};

static void spansion_spi_flash_addr(u32 addr, u8 *cmd, u8 mode4b)
{
	/* cmd[0] is actual command */
    if (mode4b) {
        cmd[1] = addr >> 24;
    }
	cmd[mode4b + 1] = addr >> 16;
	cmd[mode4b + 2] = addr >> 8;
	cmd[mode4b + 3] = addr >> 0;
}

int spansion_spi_flash_cmd_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	u32 start, end, erase_size;
	int ret;
	u8 cmd[5];
#ifdef CONFIG_APBOOT
    u32 sect_grp_start_addr;
    u32 sect_grp_end_addr;
    u32 i;
    u8  mode4b = 0;
#endif

#ifdef CONFIG_APBOOT
    if (offset + len > 0x1000000) {
		mode4b = 1;
	}
#endif

	erase_size = flash->sector_size;

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		debug("SF: Unable to claim SPI bus\n");
		return ret;
	}

	if (erase_size == 4096)
		cmd[0] = CMD_ERASE_4K;
	else
		cmd[0] = CMD_ERASE_64K;
	start = offset;
	end = start + len;

	while (offset < end) {
#ifdef CONFIG_APBOOT
	    if (!flash->is_uniform_sector) { 
	       for (i = 0; i < MAX_SECT_GRP; i++) {
	            sect_grp_start_addr = flash->sect_grp_desc[i].start_addr;
	            if (flash->sect_grp_desc[i+1].sect_size != SECT_SIZE_INVAL) {
	               sect_grp_end_addr = flash->sect_grp_desc[i+1].start_addr - 1; 
	            } else {
	               sect_grp_end_addr =  flash->size - 1;  
	            }
	            if (offset >= sect_grp_start_addr  && 
	                offset <= sect_grp_end_addr) {
	                erase_size = flash->sect_grp_desc[i].sect_size;
	                break;
	            }
	        }
	    }

	    if (offset % erase_size || len % erase_size) {
		    printf("SF: Erase offset/length not multiple of erase size offset=0x%x len=0x%x erase_size=0x%x\n",
                    offset, len, erase_size);
		    printf("SF: Erase offset/length not multiple of erase size\n");
            ret = -1;
            goto out;
	    }

	    if (erase_size == 4096) {
	        cmd[0] = mode4b ? CMD_NEW_ERASE_4K : CMD_ERASE_4K;
        } else if (erase_size == 64 * 1024) {
	        cmd[0] = mode4b ? CMD_NEW_ERASE_64K : CMD_ERASE_64K;
        } else {
	        printf("%s[%d]: Unknow erase size\n", __func__, __LINE__);
	        ret = -1;
	        goto out;
	    }
#endif
		spansion_spi_flash_addr(offset, cmd, mode4b);
		offset += erase_size;

		debug("SF: erase %2x %2x %2x %2x (%x)\n", cmd[0], cmd[1],
		      cmd[2], cmd[3], offset);

		ret = spi_flash_cmd_write_enable(flash);
		if (ret)
			goto out;

		ret = spi_flash_cmd_write(flash->spi, cmd, mode4b?  5 : 4, NULL, 0);
        
		if (ret)
			goto out;

		ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PAGE_ERASE_TIMEOUT);
		if (ret)
			goto out;
	}

	debug("SF: Successfully erased %zu bytes @ %#x\n", len, start);

 out:
	spi_release_bus(flash->spi);
	return ret;
}

int spansion_spi_flash_cmd_write_multi(struct spi_flash *flash, u32 offset,
		size_t len, const void *buf)
{
	unsigned long page_addr, byte_addr, page_size;
	size_t chunk_len, actual;
	int ret;
	u8 cmd[5];
	unsigned sector_size = 65536; // XXX
	int cnt = 0;
    u8 mode4b = 0;

    if (offset + len > 0x1000000) {
        mode4b = 1;
    }

	page_size = flash->page_size;
	page_addr = offset / page_size;
	byte_addr = offset % page_size;

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		debug("SF: unable to claim SPI bus\n");
		return ret;
	}

	cmd[0] = mode4b ? CMD_NEW_PAGE_PROGRAM : CMD_PAGE_PROGRAM;
	for (actual = 0; actual < len; actual += chunk_len) {
		chunk_len = min(len - actual, page_size - byte_addr);

        if (mode4b) {
            cmd[mode4b] = page_addr >> 16;
        }
		cmd[mode4b + 1] = page_addr >> 8;
		cmd[mode4b + 2] = page_addr;
		cmd[mode4b + 3] = byte_addr;

		debug("PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x } chunk_len = %zu\n",
		      buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);

		ret = spi_flash_cmd_write_enable(flash);
		if (ret < 0) {
			debug("SF: enabling write failed\n");
			break;
		}

		ret = spi_flash_cmd_write(flash->spi, cmd, mode4b? 5 : 4,
					  buf + actual, chunk_len);
		if (ret < 0) {
			debug("SF: write failed\n");
			break;
		}

		ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
		if (ret)
			break;

		page_addr++;
		byte_addr = 0;

  		cnt += chunk_len;
  		if (cnt > sector_size) {
  			printf(".");
			WATCHDOG_RESET();
  			cnt = 0;
  		}
	}

	debug("SF: program %s %zu bytes @ %#x\n",
	      ret ? "failure" : "success", len, offset);

	spi_release_bus(flash->spi);
	return ret;
}

int spansion_spi_flash_cmd_read_fast(struct spi_flash *flash, u32 offset,
		size_t len, void *data)
{
	u8 cmd[6];
    u8 mode4b = 0;

    if (offset + len >= 0x1000000) {
        mode4b = 1;
    }
	cmd[0] = mode4b ? CMD_NEW_READ_FAST : CMD_READ_ARRAY_FAST;
	spansion_spi_flash_addr(offset, cmd, mode4b);
	cmd[mode4b + 4] = 0x00;

	return spi_flash_read_common(flash, cmd, mode4b ? 6 : 5, data, len);
}

struct spi_flash *spi_flash_probe_spansion(struct spi_slave *spi, u8 *idcode)
{
	struct spansion_spi_flash_params *params;
	struct spi_flash *flash;
	unsigned int i, sect_grp_desc_idx = 0;
	unsigned short jedec, ext_jedec;

	jedec = idcode[1] << 8 | idcode[2];
	ext_jedec = idcode[3] << 8 | idcode[4];

	for (i = 0; i < ARRAY_SIZE(spansion_spi_flash_table); i++) {
		params =(struct spansion_spi_flash_params *)&spansion_spi_flash_table[i];
		if (params->idcode1 == jedec) {
			if (params->idcode2 == ext_jedec)
				break;
		}
	}

	if (i == ARRAY_SIZE(spansion_spi_flash_table)) {
		debug("SF: Unsupported SPANSION ID %04x %04x\n", jedec, ext_jedec);
		return NULL;
	}

	flash = malloc(sizeof(*flash));
	if (!flash) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	flash->spi = spi;
	flash->name = params->name;

	flash->write = spansion_spi_flash_cmd_write_multi;
	flash->erase = spansion_spi_flash_cmd_erase;
	flash->read = spansion_spi_flash_cmd_read_fast;

	flash->page_size = 256;
	flash->sector_size = 256 * params->pages_per_sector;
    flash->size = 0;
    if (params->is_uniform_sector) {
	    flash->size = flash->sector_size * params->nr_sectors;
    } else {
        for (i = 0; i < CFG_MAX_FLASH_SECT; i ++) {
            if (i == params->sect_grp_desc[sect_grp_desc_idx + 1].start_sector) {
                params->sect_grp_desc[sect_grp_desc_idx + 1].start_addr = flash->size;
                sect_grp_desc_idx++;
            }
            if (params->sect_grp_desc[sect_grp_desc_idx].sect_size != SECT_SIZE_INVAL) {
                flash->size += params->sect_grp_desc[sect_grp_desc_idx].sect_size;
            } else {
                break;
            }
        }
    }

#ifdef CONFIG_APBOOT
    flash->is_uniform_sector = params->is_uniform_sector;
    flash->sect_grp_desc = (sect_grp_desc_t *)params->sect_grp_desc;
	aruba_fill_in_flash_info(flash, params->nr_sectors, params->idcode1);
#endif
	return flash;
}
#endif
