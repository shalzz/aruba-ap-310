#include <common.h>
#include <watchdog.h>
#include <jffs2/jffs2.h>
#include <asm/addrspace.h>
#include <asm/types.h>
#include "ar7100_soc.h"
#include "ar7100_flash.h"

#if defined(AR7100_DUALFLASH_SUPPORT)
#include "ar7100_dualflash.h"
#endif

/*
 * globals
 */
flash_info_t flash_info[CFG_MAX_FLASH_BANKS];

#if defined (AR7100_DUALFLASH_SUPPORT)
/* Whether the board has the second flash or not, 0 for not exist; 1 for exist */
int __second_flash_exist = 0;
#endif

#ifdef display
#undef display
#endif
#define display(x)  ;

#define AR7100_SPI_CMD_WRITE_SR     0x01

/*
 * statics
 */
static void ar7100_spi_write_enable(void);
static void ar7100_spi_poll(void);
static void ar7100_spi_write_page(uint32_t addr, uint8_t *data, int len);
static void ar7100_spi_sector_erase(uint32_t addr);
static void ar7100_spi_flash_unblock(void);

static void
ar7100_spi_flash_unblock(void)
{
    ar7100_spi_write_enable();
    ar7100_spi_bit_banger(AR7100_SPI_CMD_WRITE_SR);
    ar7100_spi_bit_banger(0x0);
    ar7100_spi_go();
    ar7100_spi_poll();
}

int
read_id(void)
{
    u32 rd = 0x777777;

    ar7100_reg_wr_nf(AR7100_SPI_FS, 1);                  
    ar7100_reg_wr_nf(AR7100_SPI_WRITE, AR7100_SPI_CS_DIS);
    ar7100_spi_bit_banger(0x9f);
    ar7100_spi_delay_8();
    ar7100_spi_delay_8();
    ar7100_spi_delay_8();
    rd = ar7100_reg_rd(AR7100_SPI_RD_STATUS);
//    rd = ar7100_reg_rd(AR7100_SPI_READ); 
//    printf("id read %#x\n", rd);
    ar7100_spi_done(); 
    return rd & 0xffffff;
}

unsigned long 
flash_init (void)
{
//    int i;
    u32 rd = 0x666666;

    ar7100_reg_wr_nf(AR7100_SPI_CLOCK, 0x43);
    ar7100_spi_flash_unblock();
    rd = read_id();
/*
    rd = ar7100_reg_rd(AR7100_SPI_RD_STATUS);
    printf ("rd = %x\n", rd);
    if (rd & 0x80) {
    }
*/

#if defined(AR7100_DUALFLASH_SUPPORT)
    /* Initialize the second flash */
    ar7100_reg_rmw_set(AR7100_GPIO_FUNCTIONS, 1<<12);
    ar7100_reg_rmw_clear(AR7100_GPIO_INT_ENABLE, 1);
    ar7100_reg_rmw_set(AR7100_GPIO_OE, 1);

    /* Detect whether has the second flash or not */
    ar7100_reg_wr_nf(AR7100_SPI_FS2, 1);
    ar7100_reg_wr_nf(AR7100_SPI_WRITE2, AR7100_SPI_CS_DIS2);
    ar7100_spi_bit_banger2(0x9f);
    ar7100_spi_bit_banger2(0);
    ar7100_spi_bit_banger2(0);
    ar7100_spi_bit_banger2(0);
    rd = ar7100_reg_rd(AR7100_SPI_RD_STATUS);
    ar7100_spi_done2();

    /* Spansion flash ID is 0x01. */
    if (((rd >> 4) & 0xff) == 0x1) {
        __second_flash_exist = 1;
    } else {
        __second_flash_exist = 0;
    }
    rd = read_id();
#endif

    /*
     * hook into board specific code to fill flash_info
     */
    return (flash_get_geom((flash_info_t *)&flash_info));
}


void flash_print_info (flash_info_t *info)
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

int
flash_erase(flash_info_t *info, int s_first, int s_last)
{
    int i, sector_size = info->size/info->sector_count;

#if 0
    printf("\nFirst %#x last %#x sector size %#x\n",
           s_first, s_last, sector_size);
#endif
#ifdef GPIO_FLASH_WRITE
    ar7100_gpio_out_val(GPIO_FLASH_WRITE, FLASH_WRITE_ENABLE);
#endif
    for (i = s_first; i <= s_last; i++) {
        printf(".");
        ar7100_spi_sector_erase(i * sector_size);
	WATCHDOG_RESET();
    }
    ar7100_spi_done();
#ifdef GPIO_FLASH_WRITE
    ar7100_gpio_out_val(GPIO_FLASH_WRITE, FLASH_WRITE_DISABLE);
#endif
    printf("done\n");

    return 0;
}

/*
 * Write a buffer from memory to flash:
 * 0. Assumption: Caller has already erased the appropriate sectors.
 * 1. call page programming for every 256 bytes
 */
int 
write_buff(flash_info_t *info, uchar *source, ulong addr, ulong len)
{
    int total = 0, len_this_lp, bytes_this_page;
    ulong dst;
    uchar *src;
    int pscnt = 0, sector_size = info->size/info->sector_count;
    
//    printf ("write addr: %x\n", addr); 
    addr = addr - CFG_FLASH_BASE;

#ifdef GPIO_FLASH_WRITE
    ar7100_gpio_out_val(GPIO_FLASH_WRITE, FLASH_WRITE_ENABLE);
#endif

    while(total < len) {
        src              = source + total;
        dst              = addr   + total;
        bytes_this_page  = AR7100_SPI_PAGE_SIZE - (addr % AR7100_SPI_PAGE_SIZE);
        len_this_lp      = ((len - total) > bytes_this_page) ? bytes_this_page
                                                             : (len - total);
        ar7100_spi_write_page(dst, src, len_this_lp);
	WATCHDOG_RESET();
        total += len_this_lp;
	pscnt += len_this_lp;
	if (pscnt > sector_size) {
	    printf(".");
	    pscnt = 0;
	}
    }

    ar7100_spi_done();

#ifdef GPIO_FLASH_WRITE
    ar7100_gpio_out_val(GPIO_FLASH_WRITE, FLASH_WRITE_DISABLE);
#endif
    return 0;
}

static void
ar7100_spi_write_enable(void)  
{
    ar7100_reg_wr_nf(AR7100_SPI_FS, 1);                  
    ar7100_reg_wr_nf(AR7100_SPI_WRITE, AR7100_SPI_CS_DIS);     
    ar7100_spi_bit_banger(AR7100_SPI_CMD_WREN);             
    ar7100_spi_go();
}

static void
ar7100_spi_poll(void)   
{
    int rd;                                                 

    do {
        ar7100_reg_wr_nf(AR7100_SPI_WRITE, AR7100_SPI_CS_DIS);     
        ar7100_spi_bit_banger(AR7100_SPI_CMD_RD_STATUS);        
        ar7100_spi_delay_8();
        rd = (ar7100_reg_rd(AR7100_SPI_RD_STATUS) & 1);               
    }while(rd);
}

static void
ar7100_spi_write_page(uint32_t addr, uint8_t *data, int len)
{
    int i;
    uint8_t ch;

    display(0x77);
    ar7100_spi_write_enable();
    ar7100_spi_bit_banger(AR7100_SPI_CMD_PAGE_PROG);
    ar7100_spi_send_addr(addr);

    for(i = 0; i < len; i++) {
        ch = *(data + i);
        ar7100_spi_bit_banger(ch);
    }

    ar7100_spi_go();
    display(0x66);
    ar7100_spi_poll();
    display(0x6d);
}

static void
ar7100_spi_sector_erase(uint32_t addr)
{
    ar7100_spi_write_enable();
    ar7100_spi_bit_banger(AR7100_SPI_CMD_SECTOR_ERASE);
    ar7100_spi_send_addr(addr);
    ar7100_spi_go();
    display(0x7d);
    ar7100_spi_poll();
}

static int
read_sr(void)
{
    u32 rd = 0x777777;

    ar7100_reg_wr_nf(AR7100_SPI_FS, 1);                  
    ar7100_reg_wr_nf(AR7100_SPI_WRITE, AR7100_SPI_CS_DIS);
    ar7100_spi_bit_banger(AR7100_SPI_CMD_RD_STATUS);
    ar7100_spi_delay_8();
    rd = ar7100_reg_rd(AR7100_SPI_RD_STATUS);
//    rd = ar7100_reg_rd(AR7100_SPI_READ); 
//    printf("SR read %#x\n", rd);
    ar7100_spi_done(); 
    return rd & 0xff;
}

void
ar7100_spi_flash_enable_hpm(int onoff)
{
    int id;

    id = read_id();
    switch (id) {
    case 0xc22018:	// MXIC 25L12845E
        break;
    default:
printf("Unknown flash ID; not enabling HPM\n");
       return;
    }
    ar7100_spi_write_enable();
    ar7100_reg_wr_nf(AR7100_SPI_FS, 1);                  
    ar7100_reg_wr_nf(AR7100_SPI_WRITE, AR7100_SPI_CS_DIS);     
    ar7100_spi_bit_banger(AR7100_SPI_CMD_WRITE_SR);

    //
    // update SRWD bit; once this is set, the GPIO pin will control whether we
    // can unblock the flash
    //
    printf("writing %x\n", onoff ? 0x80: 0x0);
    ar7100_spi_bit_banger(onoff ? 0x80 : 0x0);
    ar7100_spi_go();
    ar7100_spi_poll();

    printf("SR is now %x\n", read_sr());
}

#ifdef GPIO_FLASH_WRITE
void
talisker_test_hpm(void)
{
	int sr;
	ar7100_spi_flash_enable_hpm(1);
	sr = read_sr();
	if (!(sr & 0x80)) {
	    printf("failed: SR %x\n", sr);
	    return;
	}
printf("GPIO pin %x\n",ar7100_reg_rd(AR7100_GPIO_OUT));
	ar7100_gpio_out_val(GPIO_FLASH_WRITE, FLASH_WRITE_DISABLE);
printf("GPIO pin now %x\n",ar7100_reg_rd(AR7100_GPIO_OUT));
	ar7100_spi_flash_enable_hpm(0);
	sr = read_sr();
	if (!(sr & 0x80)) {
	    printf("#2 failed: SR %x\n", sr);
	    return;
	}
	ar7100_gpio_out_val(GPIO_FLASH_WRITE, FLASH_WRITE_ENABLE);
printf("GPIO pin finally %x\n",ar7100_reg_rd(AR7100_GPIO_OUT));
	ar7100_spi_flash_enable_hpm(0);
	sr = read_sr();
	if (sr & 0x80) {
	    printf("#3 failed: SR %x\n", sr);
	    return;
	}
}
#endif
