/*
 * Interface to SPI flash
 *
 * Copyright (C) 2008 Atmel Corporation
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
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
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include <spi.h>
#include <linux/types.h>
#include <linux/compiler.h>

#ifdef CONFIG_APBOOT
#define MAX_SECT_GRP	10

enum sector_size {
	SECT_SIZE_4K = 4 * 1024,
	SECT_SIZE_8K = 8 * 1024,
	SECT_SIZE_16K = 16 * 1024,
	SECT_SIZE_32K = 32 * 1024,
	SECT_SIZE_64K = 64 * 1024,
	SECT_SIZE_128K = 128 * 1024,
	SECT_SIZE_256K = 256 * 1024,
	SECT_SIZE_512K = 512 * 1024,
	SECT_SIZE_INVAL = 0xFFFFFFFF,
};

struct sect_grp_desc {
	uint32_t start_sector;
	uint32_t nr_sector;
	uint32_t start_addr;
	enum sector_size sect_size;
};
typedef struct sect_grp_desc sect_grp_desc_t;
#endif

struct spi_flash {
	struct spi_slave *spi;

	const char	*name;

	/* Total flash size */
	u32		size;
	/* Write (page) size */
	u32		page_size;

#ifdef CONFIG_APBOOT
	/* if the flash is non-uniform sector size */
	u8      is_uniform_sector;
	sect_grp_desc_t *sect_grp_desc;
#endif
	/* Erase (sector) size */
	u32		sector_size;

	int		(*read)(struct spi_flash *flash, u32 offset,
				size_t len, void *buf);
	int		(*write)(struct spi_flash *flash, u32 offset,
				size_t len, const void *buf);
	int		(*erase)(struct spi_flash *flash, u32 offset,
				size_t len);
};

struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int spi_mode);
void spi_flash_free(struct spi_flash *flash);

static inline int spi_flash_read(struct spi_flash *flash, u32 offset,
		size_t len, void *buf)
{
	return flash->read(flash, offset, len, buf);
}

static inline int spi_flash_write(struct spi_flash *flash, u32 offset,
		size_t len, const void *buf)
{
	return flash->write(flash, offset, len, buf);
}

static inline int spi_flash_erase(struct spi_flash *flash, u32 offset,
		size_t len)
{
	return flash->erase(flash, offset, len);
}

void spi_boot(void) __noreturn;

#ifdef CONFIG_APBOOT
extern void aruba_fill_in_flash_info(struct spi_flash *spi, unsigned num_sectors, unsigned id);
#endif

#endif /* _SPI_FLASH_H_ */
