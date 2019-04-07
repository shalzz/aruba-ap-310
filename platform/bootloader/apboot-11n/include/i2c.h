#ifndef _I2C_H
#define _I2C_H

#define I2C_CONFIG     0
#define I2C_CLKDIV     1
#define I2C_DEVADDR    2
#define I2C_ADDR       3
#define I2C_DATAOUT    4
#define I2C_DATAIN     5
#define I2C_STATUS     6
#define I2C_STARTXFR   7
#define I2C_BYTECNT    8
#define I2C_HDSTATIM   9

#define I2C_READ_XFR   1
#define I2C_WRITE_XFR  0
#define I2C_TND_XFR    2

#define I2C_SCLK_100k_DIV 0x14A
#define I2C_CLOCK_DIVISOR I2C_SCLK_100k_DIV
#define I2C_HDSTA_DELAY 0x107

#define I2C_RXTX_LEN	128	/* maximum tx/rx buffer length */

void i2c_init(int speed, int slaveaddr);

#if defined(CONFIG_MARVELL)
int i2c_probe (uchar chip);
#endif

#endif
