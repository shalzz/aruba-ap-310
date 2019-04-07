/*
 * (C) Copyright 2004,2005
 * Cavium Networks
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

#include <common.h>
#include <command.h>
#include <asm/au1x00.h>
#include <asm/mipsregs.h>
#include "octeon_boot.h"
#include <pci.h>
#include <lib_octeon_shared.h>
#include <lib_octeon.h>


#if defined(CONFIG_PCI)
extern void init_octeon_pci (void);

void pci_init_board (void)
{
	init_octeon_pci();
}
#endif



#if CONFIG_USE_EMULATOR
#define FLASH_SLOWDOWN_FACTOR   3
#else
#define FLASH_SLOWDOWN_FACTOR   1
#endif

/* Boot bus init for flash and peripheral access */
#define FLASH_RoundUP(_Dividend, _Divisor) (((_Dividend)+(_Divisor))/(_Divisor))
flash_info_t flash_info[CFG_MAX_FLASH_BANKS];	/* info for FLASH chips */
int octeon_boot_bus_init_board(void)
{
    cvmx_mio_boot_reg_cfgx_t reg_cfg;
    cvmx_mio_boot_reg_timx_t __attribute__((unused)) reg_tim;
    uint64_t ECLK_PERIOD;
    uint64_t val;

    /* Read CPU clock multiplier */
    uint64_t data = octeon_read64((uint64_t)0x80011F00000001E8ull);
    data = data >> 18;
    data &= 0x1f;
    data *= 50; /* cpu_ref = 50 */
    ECLK_PERIOD = 1000000 / data; /* eclk period (psecs) */

    /* Boot Flash */
    reg_tim.u64 = 0;
    reg_tim.s.oe = FLASH_RoundUP((FLASH_RoundUP(75000ULL, ECLK_PERIOD/FLASH_SLOWDOWN_FACTOR) - 1), 4);
    reg_tim.s.we = 0x3F;
    reg_tim.s.rd_hld = FLASH_RoundUP((FLASH_RoundUP(25000ULL, ECLK_PERIOD/FLASH_SLOWDOWN_FACTOR) - 1), 4);
    reg_tim.s.wr_hld = 0x3F;
    reg_tim.s.wait = 0x3F;
    reg_tim.s.page = FLASH_RoundUP((FLASH_RoundUP(35000ULL, ECLK_PERIOD/FLASH_SLOWDOWN_FACTOR) - 1), 4);
    octeon_write64(CVMX_MIO_BOOT_REG_TIMX(0), reg_tim.u64);

    /* Flash */
    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
    reg_cfg.s.size = ((64<<20) >> 16) - 1; /* 64MB */
    reg_cfg.s.base = ((NS0216_FLASH1_BASE_ADDR >> 16) & 0x1FFF);
    reg_tim.u64 = 0;
    reg_tim.s.oe = FLASH_RoundUP((FLASH_RoundUP(120000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.we = FLASH_RoundUP((FLASH_RoundUP(100000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.rd_hld = FLASH_RoundUP((FLASH_RoundUP(25000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.wr_hld = FLASH_RoundUP((FLASH_RoundUP(25000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.pause = FLASH_RoundUP((FLASH_RoundUP(40000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.wait = 0x3F;
    reg_tim.s.page = FLASH_RoundUP((FLASH_RoundUP(35000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.pagem = 1;
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(1), reg_cfg.u64);
    octeon_write64(CVMX_MIO_BOOT_REG_TIMX(1), reg_tim.u64);

    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
    reg_cfg.s.size = ((64<<20) >> 16) - 1; /* 64MB */
    reg_cfg.s.base = ((NS0216_FLASH2_BASE_ADDR >> 16) & 0x1FFF);
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(2), reg_cfg.u64);
    octeon_write64(CVMX_MIO_BOOT_REG_TIMX(2), reg_tim.u64);

    /* SPI-MAC (VSC7323) */
    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
    reg_cfg.s.size = ((128<<10) >> 16) - 1; /* 128KB */
    reg_cfg.s.base = ((NS0216_SPIMAC1_BASE_ADDR >> 16) & 0x1FFF);
    reg_tim.u64 = 0;
    reg_tim.s.oe = FLASH_RoundUP((FLASH_RoundUP(240000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.we = FLASH_RoundUP((FLASH_RoundUP(125000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.rd_hld = FLASH_RoundUP((FLASH_RoundUP(80000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.wr_hld = FLASH_RoundUP((FLASH_RoundUP(80000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.pause = FLASH_RoundUP((FLASH_RoundUP(90000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.wait = 0x3F;
    reg_tim.s.page = FLASH_RoundUP((FLASH_RoundUP(140000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.pagem = 1;
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(3), reg_cfg.u64);
    octeon_write64(CVMX_MIO_BOOT_REG_TIMX(3), reg_tim.u64);

    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
    reg_cfg.s.size = ((128<<10) >> 16) - 1; /* 128KB */
    reg_cfg.s.base = ((NS0216_SPIMAC2_BASE_ADDR >> 16) & 0x1FFF);
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(4), reg_cfg.u64);
    octeon_write64(CVMX_MIO_BOOT_REG_TIMX(4), reg_tim.u64);

    /* DEBUG-MAC (CS8900A) */
    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
    reg_cfg.s.size = ((64<<10) >> 16) - 1; /* 64KB */
    reg_cfg.s.base = ((NS0216_DEBMAC1_BASE_ADDR >> 16) & 0x1FFF);
    reg_tim.u64 = 0;
    reg_tim.s.oe = FLASH_RoundUP((FLASH_RoundUP(320000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.we = FLASH_RoundUP((FLASH_RoundUP(320000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.rd_hld = FLASH_RoundUP((FLASH_RoundUP(80000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.wr_hld = FLASH_RoundUP((FLASH_RoundUP(80000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.pause = FLASH_RoundUP((FLASH_RoundUP(90000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.wait = 0x3F;
    reg_tim.s.page = FLASH_RoundUP((FLASH_RoundUP(120000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.pagem = 1;
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(5), reg_cfg.u64);
    octeon_write64(CVMX_MIO_BOOT_REG_TIMX(5), reg_tim.u64);

    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
    reg_cfg.s.size = ((64<<10) >> 16) - 1; /* 64KB */
    reg_cfg.s.base = ((NS0216_DEBMAC1_BASE_ADDR >> 16) & 0x1FFF);
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(6), reg_cfg.u64);
    octeon_write64(CVMX_MIO_BOOT_REG_TIMX(6), reg_tim.u64);

    /* RTC */
    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
    reg_cfg.s.size = ((64<<10) >> 16) - 1; /* 64KB */
    reg_cfg.s.base = ((NS0216_RTC_BASE_ADDR >> 16) & 0x1FFF);
    reg_tim.u64 = 0;
    reg_tim.s.oe = FLASH_RoundUP((FLASH_RoundUP(75000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.we = 0x3F;
    reg_tim.s.rd_hld = FLASH_RoundUP((FLASH_RoundUP(25000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.wr_hld = 0x3F;
    reg_tim.s.wait = 0x3F;
    reg_tim.s.page = FLASH_RoundUP((FLASH_RoundUP(35000ULL, ECLK_PERIOD) - 1), 4);
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(7), reg_cfg.u64);
    octeon_write64(CVMX_MIO_BOOT_REG_TIMX(7), reg_tim.u64);
    return(0);
}

int checkboard (void)
{
    return 0;
}



int early_board_init(void)
{
    int cpu_ref = 33;

    DECLARE_GLOBAL_DATA_PTR;

    memset((void *)&(gd->mac_desc), 0x0, sizeof(octeon_eeprom_mac_addr_t));
    memset((void *)&(gd->clock_desc), 0x0, sizeof(octeon_eeprom_clock_desc_t));
    memset((void *)&(gd->board_desc), 0x0, sizeof(octeon_eeprom_board_desc_t));



    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(12), 1);  // power off
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(10), 1);  // run LED
    cvmx_write_csr(CVMX_GPIO_TX_CLR, 1 << 12 | 1 << 10);



    /* NOTE: this is early in the init process, so the serial port is not yet configured */

    gd->board_desc.board_type = CVMX_BOARD_TYPE_CUST_NS0216;
    gd->board_desc.rev_major = 1;
    gd->board_desc.rev_minor = 0;
    cpu_ref = 50;
    gd->ddr_clock_mhz = 266;

    gd->mac_desc.count = 32;
    char *s, *e;
    s = getenv ("ethaddr");
    if (s)
    {
        int i;
        for (i = 0; i < 6; ++i)
        {
            gd->mac_desc.mac_addr_base[i] = s ? simple_strtoul (s, &e, 16) : 0;
            if (s)
                s = (*e) ? e + 1 : e;
        }
    } else {
        gd->mac_desc.mac_addr_base[0] = 0x00;
        gd->mac_desc.mac_addr_base[1] = 0xD0;
        gd->mac_desc.mac_addr_base[2] = 0xA6;
        gd->mac_desc.mac_addr_base[3] = 0xFF;
        gd->mac_desc.mac_addr_base[4] = 0xFF;
        gd->mac_desc.mac_addr_base[5] = 0x00;
    }


    /* Read CPU clock multiplier */
    uint64_t data = octeon_read64((uint64_t)0x80011F00000001E8ull);
    data = data >> 18;
    data &= 0x1f;


    gd->cpu_clock_mhz = data * cpu_ref;

    /* adjust for 33.33 Mhz clock */
    if (cpu_ref == 33)
        gd->cpu_clock_mhz += (data)/4 + data/8;

    if (gd->cpu_clock_mhz < 100 || gd->cpu_clock_mhz > 600)
    {
        gd->cpu_clock_mhz = DEFAULT_ECLK_FREQ_MHZ;
    }
    return 0;

}
void octeon_led_str_write(const char *str)
{
    octeon_led_str_write_std(str);
}
