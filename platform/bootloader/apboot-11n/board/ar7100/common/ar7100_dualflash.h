#ifndef _DUALFLASH_H
#define _DUALFLASH_H

#define AR7100_SPI_CMD_RD_FLASH_ID  0x9f

/* Macro for read/write/erase the 2nd flash chipset */
#define AR7100_SPI_FS2                  0x1f000000
#define AR7100_SPI_CLOCK2               0x1f000004
#define AR7100_SPI_WRITE2               0x1f000008
#define AR7100_SPI_READ2                0x1f000000
#define AR7100_SPI_RD_STATUS2           0x1f00000c

#define AR7100_SPI_CS_DIS2              0x70000
#define AR7100_SPI_CE_LOW2              0x10000
#define AR7100_SPI_CE_HIGH2             0x10100
#define AR7100_SPI_CLK_HIGH             (1<<8)

#define SECOND_FLASH_IMAGE_START        0x100000
#define SECOND_FLASH_IMAGE_END          0xd00000

#define ar7100_spi_bit_banger2(_byte)  do {     \
        int _i;                                 \
        for(_i = 0; _i < 8; _i++) {             \
                ar7100_reg_wr_nf(AR7100_SPI_WRITE2,     \
                        AR7100_SPI_CE_LOW2 | ar7100_be_msb(_byte, _i));         \
                ar7100_reg_wr_nf(AR7100_SPI_WRITE2,     \
                        AR7100_SPI_CE_HIGH2 | ar7100_be_msb(_byte, _i));        \
        }                                       \
}while(0); 

#define ar7100_spi_bit_banger_rd2()  do {       \
        int _i;                                 \
        for (_i = 0; _i < 8; _i++) {            \
                ar7100_reg_wr_nf(AR7100_SPI_WRITE2,     \
                        AR7100_SPI_CE_LOW2);            \
                ar7100_reg_wr_nf(AR7100_SPI_WRITE2,     \
                        AR7100_SPI_CE_HIGH2);           \
        }                                               \
}while(0);

#define ar7100_spi_go1()  do {                  \
        ar7100_reg_wr_nf(AR7100_SPI_WRITE2, AR7100_SPI_CE_HIGH2);       \
}while(0);

#define ar7100_spi_go2()  do {                  \
        ar7100_reg_wr_nf(AR7100_SPI_WRITE2, AR7100_SPI_CE_LOW2);        \
        ar7100_reg_wr_nf(AR7100_SPI_WRITE2, AR7100_SPI_CS_DIS2);        \
}while(0);

#define ar7100_spi_delay_8_2()          ar7100_spi_bit_banger2(0)
#define ar7100_spi_done2()              ar7100_reg_wr(AR7100_SPI_FS2, 0)

#define ar7100_spi_send_addr2(_addr) do {       \
        ar7100_spi_bit_banger2(((_addr & 0xff0000) >> 16));     \
        ar7100_spi_bit_banger2(((_addr & 0x00ff00) >> 8));      \
        ar7100_spi_bit_banger2(_addr & 0x0000ff);               \
}while(0);

#endif /* _DUALFLASH_H */
