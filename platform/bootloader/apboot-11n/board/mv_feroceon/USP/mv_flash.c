#include <common.h>
#include <flash.h>
#include <watchdog.h>

#ifdef CONFIG_SPI_FLASH
#include <spi.h>
#include <spi_flash.h>

struct spi_flash *flash;

unsigned long spi_flash_init(void)
{
	flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS, CONFIG_ENV_SPI_MAX_HZ, SPI_MODE_3);
	if (!flash) {
		printf("Failed to initialize SPI flash at %u:%u\n", CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS);
		return 1;
	}
	return 0;
}

#endif

#ifndef CONFIG_SYS_NO_FLASH

flash_info_t    flash_info[CONFIG_SYS_MAX_FLASH_BANKS];

unsigned long flash_init(void)
{
    int i;

    for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; i++) {
	flash_info[i].flash_id = FLASH_UNKNOWN;
	flash_info[i].sector_count = 0;
	flash_info[i].size = 0;
    }

    /* we need to initialize spi flash before getting manufacture info */
    spi_flash_init(); 
    return 1;
}

int flash_erase(flash_info_t *info, int s_first, int s_last)
{
    unsigned len;
    int i;

    len = flash_info[0].size / flash_info[0].sector_count;
    for (i = s_first; i <= s_last; i++) {
        printf(".");
        WATCHDOG_RESET();
        (void)spi_flash_erase(flash, flash_info[0].start[i], len);
    }
    printf("done\n");
    return 0;
}

void flash_print_info(flash_info_t *info)
{
	int i;

	printf ("  Size: %ld MB in %d Sectors\n",
		info->size >> 20, info->sector_count);
#if 0
	printf (" Erase timeout %ld ms, write timeout %ld ms, buffer write timeout %ld ms, buffer size %d\n",
		info->erase_blk_tout,
		info->write_tout,
		info->buffer_write_tout,
		info->buffer_size);
#endif

	puts ("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; ++i) {
#ifdef CFG_FLASH_EMPTY_INFO
		int k;
		int size;
		int erased;
		volatile unsigned long *flash;

		/*
		 * Check if whole sector is erased
		 */
		if (i != (info->sector_count - 1))
			size = info->start[i + 1] - info->start[i];
		else
			size = info->start[0] + info->size - info->start[i];
		erased = 1;
		flash = (volatile unsigned long *) info->start[i];
		size = size >> 2;	/* divide by 4 for longword access */
		for (k = 0; k < size; k++) {
			if (*flash++ != 0xffffffff) {
				erased = 0;
				break;
			}
		}

		if ((i % 5) == 0)
			printf ("\n");
		/* print empty and read-only info */
		printf (" %08lX%s%s",
			info->start[i],
			erased ? " E" : "  ",
			info->protect[i] ? "RO " : "   ");
#else
		if ((i % 5) == 0)
			printf ("\n   ");
		printf (" %08lX%s",
			info->start[i], info->protect[i] ? " (RO)" : "     ");
#endif
	}
	putc ('\n');
	return;
}

int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
    addr -= flash_info[0].start[0];
//printf("%x %u\n", addr, cnt);
    WATCHDOG_RESET();
    return spi_flash_write(flash, addr, cnt, src);
}

int aruba_flash_read(ulong addr, uchar *dest, ulong cnt)
{
    addr -= flash_info[0].start[0];
//printf("%x %u\n", addr, cnt);
    WATCHDOG_RESET();
    return spi_flash_read(flash, addr, cnt, dest);
}
#endif
