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
#ifdef CONFIG_IPROC
#include <spi_flash_iproc.h>
#else

#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include <spi.h>

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

/* Common commands */
#define CMD_READ_ID			0x9f

#define CMD_READ_ARRAY_SLOW		0x03
#define CMD_READ_ARRAY_FAST		0x0b

#define CMD_WRITE_STATUS		0x01
#define CMD_PAGE_PROGRAM		0x02
#define CMD_WRITE_DISABLE		0x04
#define CMD_READ_STATUS			0x05
#define CMD_WRITE_ENABLE		0x06
#define CMD_ERASE_4K			0x20
#define CMD_ERASE_32K			0x52
#define CMD_ERASE_64K			0xd8
#define CMD_ERASE_CHIP			0xc7

struct spi_flash_region {
	unsigned int	count;
	unsigned int	size;
};

struct spi_flash {
	struct spi_slave *spi;

	const char	*name;

	u32		size;
	/* Write (page) size */
	u32		page_size;
	/* Erase (sector) size */
	u32		sector_size;
	/* 3 or 4 byte address width */
	u32             addr_width;

	u8		read_opcode;

#ifdef CONFIG_APBOOT
	/* if the flash is non-uniform sector size */
	u8      is_uniform_sector;
	sect_grp_desc_t *sect_grp_desc;
#endif
	u8		write_opcode;

	int		(*read)(struct spi_flash *flash, u32 offset,
				size_t len, void *buf);
	int		(*write)(struct spi_flash *flash, u32 offset,
				size_t len, const void *buf);
	int		(*erase)(struct spi_flash *flash, u32 offset,
				size_t len);
#ifdef CONFIG_SPI_FLASH_PROTECTION
	int		(*protect)(struct spi_flash *flash, int enable);
#endif
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
#ifdef CONFIG_SPI_FLASH_PROTECTION
static inline int spi_flash_protect(struct spi_flash *flash, int enable)
{
	return flash->protect(flash, enable);
}
#endif

#ifdef CONFIG_APBOOT
extern void aruba_fill_in_flash_info(struct spi_flash *spi, unsigned num_sectors, unsigned id);
#endif
#endif /* _SPI_FLASH_H_ */
#endif /* IPROC */
