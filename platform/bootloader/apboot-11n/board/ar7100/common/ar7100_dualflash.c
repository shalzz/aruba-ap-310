#include <common.h>
#include <watchdog.h>
#include <jffs2/jffs2.h>
#include <asm/addrspace.h>
#include <asm/types.h>
#include "ar7100_soc.h"
#include "ar7100_flash.h"
#include "ar7100_dualflash.h"

/* 
* The second flash chipset info. It's better to use individual 
* flash info variable. 
*/
flash_info_t flash_info_second;

extern int read_id(void);

/* For read/write/erase second flash chipset */
static void ar7100_spi_write_enable2(void);
static void ar7100_spi_poll2(void);
static void ar7100_spi_read_page2(uint32_t addr, uint8_t *data, int len);
static void ar7100_spi_sector_erase2(uint32_t addr);
static void ar7100_spi_write_page2(uint32_t addr, uint8_t *data, int len);

void read_buff2(uint32_t from, int len, uchar *buf);

/*
* Description:
*   Write buff to the second flash.
*
* Parameters:
*   source: The source image file address in memory
*   len: Image file length
*
* Return:
*   void 
*/
void
write_buff2(uchar *source, ulong len)
{
    int total = 0, len_this_lp, bytes_this_page;
    ulong dst , addr = SECOND_FLASH_IMAGE_START;
    uchar *src;
    flash_info_t *info = &flash_info_second;
    int pscnt = 0, sector_size;

    sector_size = info->size/info->sector_count;

    while(total < len) {
        src              = source + total;
        dst              = addr   + total;
        bytes_this_page  = AR7100_SPI_PAGE_SIZE - (addr % AR7100_SPI_PAGE_SIZE);
        len_this_lp      = ((len - total) > bytes_this_page) ? bytes_this_page
                                                             : (len - total);
        ar7100_spi_write_page2(dst, src, len_this_lp);

        WATCHDOG_RESET();
        total += len_this_lp;
        pscnt += len_this_lp;
        if (pscnt > sector_size) {
            printf(".");
            pscnt = 0;
        }
    }

    ar7100_spi_done2();
    printf(" Done\n");

    /* DONT DELETE! Set chip select to the first flash */
    read_id();

    return;
}

/*
* Description:
*   Erase the second flash, it alway erase from 0x100000 to 0xd00000.
*
* Parameters:
*   void
*
* Return:
*   void
*/
void
flash_erase2(void)
{
    int i, sector_size = 0;
    int s_first, s_last;
    flash_info_t *info = &flash_info_second;

    sector_size = info->size / info->sector_count;

    /* Every time erase the fixed size range: 0x100000--0xd00000 12MB*/
    s_first = SECOND_FLASH_IMAGE_START / sector_size;
    s_last = SECOND_FLASH_IMAGE_END / sector_size;
    printf("sector %d-%d ", s_first, s_last);

    for (i = s_first; i <= s_last; i++) {
        printf(".");
        ar7100_spi_sector_erase2(i * sector_size);
        WATCHDOG_RESET();
    }

    ar7100_spi_sector_erase2(0);

    printf("Done\n");

    return;
}

/*
* Description:
*   Write image file to the second flash.
*
* Parameters:
*   addr: source image addrress
*   len: image length
*
* Return:
*   Always 0
*/
void
do_write_backup_image(unsigned int addr, unsigned int len)
{
    /* Erase the second flash */
    printf("Erase the second flash @ 0x%x -- 0x%x, ",
        SECOND_FLASH_IMAGE_START, SECOND_FLASH_IMAGE_END);
    flash_erase2();
    /* Write image to the second flash */
    printf("Write to the second flash @ 0x100000 ");
    write_buff2((uchar*)addr, len);

    return;
}

static void
ar7100_spi_write_enable2(void)
{
    ar7100_reg_wr_nf(AR7100_SPI_FS2, 1);
    ar7100_reg_wr_nf(AR7100_SPI_WRITE2, (2<<16) | AR7100_SPI_CLK_HIGH);
    ar7100_spi_bit_banger2(AR7100_SPI_CMD_WREN);
    ar7100_spi_go2();
}

static void
ar7100_spi_poll2(void)
{
    int rd;

    do {
        ar7100_reg_wr_nf(AR7100_SPI_WRITE2, (2<<16)|AR7100_SPI_CLK_HIGH);
        ar7100_spi_bit_banger2(AR7100_SPI_CMD_RD_STATUS);
        ar7100_spi_delay_8_2();
        rd = (ar7100_reg_rd(AR7100_SPI_RD_STATUS2) & 1);
    }while(rd);
}

static void
ar7100_spi_read_page2(uint32_t addr, uint8_t *data, int len)
{
        int i, j = 0;

        ar7100_reg_wr_nf(AR7100_SPI_FS2, 1);
        ar7100_reg_wr_nf(AR7100_SPI_WRITE2, (2<<16)|AR7100_SPI_CLK_HIGH);

        ar7100_spi_bit_banger2(AR7100_SPI_CMD_FAST_READ);
        ar7100_spi_send_addr2(addr);
        ar7100_spi_bit_banger2(0);

        for(i = 0; i < len; i++) {
                ar7100_spi_bit_banger_rd2();
                *(data++) = (uint8_t)(ar7100_reg_rd(AR7100_SPI_RD_STATUS) & 0xff);
                while (++j % 0xff == 0) {
                    WATCHDOG_RESET();
                }
        }
        ar7100_spi_go1();
}

static void
ar7100_spi_sector_erase2(uint32_t addr)
{
    ar7100_spi_write_enable2();
    ar7100_spi_bit_banger2(AR7100_SPI_CMD_SECTOR_ERASE);
    ar7100_spi_send_addr2(addr);
    ar7100_spi_go1();
    display(0x7d);
    ar7100_spi_poll2();
}

static void
ar7100_spi_write_page2(uint32_t addr, uint8_t *data, int len)
{
    int i;
    uint8_t ch;

    display(0x77);
    ar7100_spi_write_enable2();
    ar7100_spi_bit_banger2(AR7100_SPI_CMD_PAGE_PROG);
    ar7100_spi_send_addr2(addr);

    for(i = 0; i < len; i++) {
        ch = *(data + i);
        ar7100_spi_bit_banger2(ch);
    }

    ar7100_spi_go2();
    display(0x66);
    ar7100_spi_poll2();
    display(0x6d);
}

/* 
* Description:
*   Read function for the second flash
*
* Parameters:
*   from: source addr
*   len: read length
*   buf: destination addr
*
* Return:
*   void
*/
void
read_buff2(uint32_t from, int len, uchar *buf)
{
    uint32_t addr = from;

    if (!len)
        return;
    if (from + len > flash_info_second.size)
        return;

    ar7100_spi_read_page2(addr, buf, len);

    /* DONT DELETE! Set chip select to the first flash */
    read_id();

    return;
}


