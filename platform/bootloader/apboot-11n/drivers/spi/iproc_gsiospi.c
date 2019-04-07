/*
 * Copyright 2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or 
 * its licensors, and may only be used, duplicated, modified or distributed 
 * pursuant to the terms and conditions of a separate, written license 
 * agreement executed between you and Broadcom (an "Authorized License").  
 * Except as set forth in an Authorized License, Broadcom grants no license 
 * (express or implied), right to use, or waiver of any kind with respect to 
 * the Software, and Broadcom expressly reserves all rights in and to the 
 * Software and all intellectual property rights therein.  IF YOU HAVE NO 
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, 
 * AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE 
 * SOFTWARE.  
 * 
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization, 
 * constitutes the valuable trade secrets of Broadcom, and you shall use 
 * all reasonable efforts to protect the confidentiality thereof, and to 
 * use this information only in connection with your use of Broadcom 
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED 
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, 
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR 
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS 
 * ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, 
 * FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, 
 * QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU 
 * ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR 
 * ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, 
 * INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY 
 * RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM 
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN 
 * EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, 
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY 
 * FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 */


// NOTE: There is no " READ FUNCTION " in GSIO spi for BCM952012SVK " since it is connected
//to a WM8955 codec, which is a write-only part

#include <asm/memory.h>
#include <common.h>
#ifdef CONFIG_IPROC
#include <asm/arch/iproc_regs.h>
#include "iproc_gsio.h"

static int spi_init = 0;

static void * baseAddr;

#define R_REG(reg)       *(volatile unsigned long*)(baseAddr + (reg&0xffff))
#define W_REG(reg, val)   *(volatile unsigned long*)(baseAddr + (reg&0xffff))=val

#define R_REG_S(reg)       *(volatile unsigned short*)(baseAddr + (reg&0xffff))
#define W_REG_S(reg, val)   *(volatile unsigned short*)(baseAddr + (reg&0xffff))=val


/* XXX debug/trace
 * ?? To enable ERR by default
 * warning: in the dongles, the printf could be very costly and slow,
 *   in BMAC dongle, print big msg can even lead to BMAC rpc timeout
 *
 */
#define SPI_ERR_VAL	0x0001
#define SPI_MSG_VAL	0x0002
#define SPI_DBG_VAL	0x0004

#define BCMDBG

static u32 spi_msg_level = SPI_ERR_VAL|SPI_MSG_VAL|SPI_DBG_VAL;

#if defined(BCMDBG) || defined(BCMDBG_ERR)
#define SPI_ERR(args)	do {if (spi_msg_level & SPI_ERR_VAL) printf args;} while (0)
#else
#define SPI_ERR(args)
#endif

#ifdef BCMDBG
#define SPI_MSG(args)	do {if (spi_msg_level & SPI_MSG_VAL) printf args;} while (0)
#define SPI_DBG(args)	do {if (spi_msg_level & SPI_DBG_VAL) printf args;} while (0)
#else
#define SPI_MSG(args)
#define SPI_DBG(args)
#endif

#define SPI_INIT_CHK  \
    if (!spi_init) {\
         SPI_MSG(("%s,SPI device not init yet!\n", __func__)); \
         return SPI_ERR_INTERNAL; \
    }

#define QT_GSIO_VERIFICATION 0

#if QT_GSIO_VERIFICATION
#define SPI_TRIES 10000
#else
#define SPI_TRIES 100000
#endif

#define CC_ID_MASK		0xf
#define CC_ID_SHIFT	    20
#define PKG_HIGH_SKU 0
#define PKG_MEDIUM_SKU 2
#define PKG_LOW_SKU 1

#define GSIO_OPCODE   0x4
#define GSIO_CORE_INSERT_CSB 0
#define GSIO_ENDIANESS       0
#define GSIO_START           1

static uint32_t apb_clk = 0;

/* Function : cca_spi_cs_release
 *  - Release CS signal : 
 */
int
cca_spi_cs_release(void)
{
    SPI_DBG(("cca_spi_cs_release\n"));

    SPI_INIT_CHK;

    W_REG(GSIO_CTRL, 0);

    return SPI_ERR_NONE;
}
/* Function : cca_spi_write 
 *  - Write operation.
 * Return :
 * Note : 
 *     DO NOT do de-assert CS here, there is still possiblity of shift-in data . 
 */
int
cca_spi_write(uint8_t *buf, int len)
{
    int i = 0;
	int buf_len = len;
	int process_len = 0;
	uint8_t *process_buf = buf;
    uint quot = 0, remain = 0;
	uint32_t data = 0;
	uint32_t ctrl = 0;
	uint32_t nod = 0;
//	unsigned short value;

    SPI_DBG(("\ncca_spi_write: start\n"));

    SPI_INIT_CHK;

    while (buf_len > 0) {
	    ctrl = ((GSIO_OPCODE << GSIO_CTRL_GC_SHIFT) & GSIO_CTRL_GC_MASK) |
	        ((GSIO_ENDIANESS << GSIO_CTRL_ENDIAN_SHIFT) & GSIO_CTRL_ENDIAN_MASK) |
    	    ((GSIO_CORE_INSERT_CSB<< GSIO_CTRL_GG_SHIFT) & GSIO_CTRL_GG_MASK) |
	        ((GSIO_START << GSIO_CTRL_SB_SHIFT) & GSIO_CTRL_SB_MASK);
        quot = (buf_len / 4);
        remain = (buf_len % 4);
		data = 0;
        if (quot != 0) { /* buf_len >= 4 bytes */
            process_len = 4;
			data |= (*process_buf);
			process_buf++;
			data |= (*process_buf << 8);
			process_buf++;
			data |= (*process_buf << 16);
			process_buf++;
			data |= (*process_buf << 24);
			process_buf++;

            nod = 3;
        	ctrl |= ((nod << GSIO_CTRL_NOD_SHIFT) & GSIO_CTRL_NOD_MASK);
        } else { /* buf_len < 4 bytes */
            process_len = remain;

			for (i = 0; i < remain; i++) {
    			data |= (*process_buf << (8*i));
    			process_buf++;
			}

            nod = remain-1;
        	ctrl |= ((nod << GSIO_CTRL_NOD_SHIFT) & GSIO_CTRL_NOD_MASK);

        }

        SPI_DBG(("cca_spi_write: write Reg 0x%x, data 0x%x\n",GSIO_DATA, data));
        SPI_DBG(("cca_spi_write: write Reg 0x%x,ctrl 0x%x\n",GSIO_CTRL, ctrl));

        W_REG(GSIO_DATA, data);
        W_REG(GSIO_CTRL, ctrl);

    	for (i = 0; i < SPI_TRIES; i++) {
            ctrl = R_REG(GSIO_CTRL);
    		if (!(ctrl & GSIO_CTRL_SB_MASK)) {
    		    break;
    		}
    	}
    	if (i >= SPI_TRIES) {
    		SPI_ERR(("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i));
    		return -1;
    	}

		buf_len -= process_len;
    }
    return SPI_ERR_NONE;
}

/* Function : cca_spi_read
 *  - Read operation.
 * Return :
 * Note :
 */
int
cca_spi_read(uint8_t *buf, int len)
{
    int i;
    uint32_t value;
    uint32_t ctrl = 0;
    uint32_t nod = 0;
    int buf_len = len;
    uint quot = 0, remain = 0;

    SPI_DBG(("\ncca_spi_read: start\n"));

    SPI_INIT_CHK;

    while (buf_len > 0) {
        /* NSTAR-547, SW WAR for set BigEndian bit here */
        ctrl = ((4 << GSIO_CTRL_GC_SHIFT) & GSIO_CTRL_GC_MASK) |
            ((0 << GSIO_CTRL_ENDIAN_SHIFT) & GSIO_CTRL_ENDIAN_MASK) |
            ((1 << GSIO_CTRL_GG_SHIFT) & GSIO_CTRL_GG_MASK) |
            ((1 << GSIO_CTRL_SB_SHIFT) & GSIO_CTRL_SB_MASK);

        quot = (buf_len / 4);
        remain = (buf_len % 4);
        if (quot != 0) { /* buf_len >= 4 bytes */
            nod = 3;
            ctrl |= ((nod << GSIO_CTRL_NOD_SHIFT) & GSIO_CTRL_NOD_MASK);
            SPI_DBG(("cca_spi_read: write %x, %x\n",GSIO_CTRL, ctrl));
            W_REG(GSIO_CTRL, ctrl);

            for (i = 0; i < SPI_TRIES; i++) {
                ctrl = R_REG(GSIO_CTRL);
                if (!(ctrl & GSIO_CTRL_SB_MASK)) {
                    break;
                }
            }
            if (i >= SPI_TRIES) {
                SPI_ERR(("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i));
                return -1;
            }

            value = R_REG(GSIO_DATA);
            SPI_DBG(("cca_spi_read: done: value=%x\n",value));

            for (i = 0; i < 4; i++) {
                *buf = (value >> (8 * i)) & 0xff;
                buf ++;
            }

            buf_len -= 4;
        } else { /* buf_len < 4 bytes */
            nod = remain - 1;
            ctrl |= ((nod << GSIO_CTRL_NOD_SHIFT) & GSIO_CTRL_NOD_MASK);
            SPI_DBG(("cca_spi_read: write %x, %x\n",GSIO_CTRL, ctrl));
            W_REG(GSIO_CTRL, ctrl);

            for (i = 0; i < SPI_TRIES; i++) {
                ctrl = R_REG(GSIO_CTRL);
                if (!(ctrl & GSIO_CTRL_SB_MASK)) {
                    break;
                }
            }
            if (i >= SPI_TRIES) {
                SPI_ERR(("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i));
                return -1;
            }

            value = R_REG(GSIO_DATA);
            SPI_DBG(("cca_spi_read: done: value=%x\n",value));

            for (i = 0; i < remain; i++) {
                *buf = (value >> (8 * i)) & 0xff;
                buf ++;
            }

            buf_len -= remain;
        }

    }

    return SPI_ERR_NONE;
}

/* Function : cca_spi_freq_set
 *  - Open Northstar CCA SPI device.
 * Return :
 * Note : 
 *     Set the SPI frequency (offset 0xf0 in chipcommonA).
 *     
 */
int
cca_spi_freq_set(int speed_hz)
{
    int rv = SPI_ERR_NONE;
	uint32_t divider = 0;
	uint32_t val;
	int speed_khz;
    
    SPI_INIT_CHK;

	if (speed_hz > 20971520) {
    	SPI_ERR(("\n%s: exceed maximum 20MHz\n", __FUNCTION__));
	    return SPI_ERR_PARAM;
	}

    speed_khz = (speed_hz / 1024);

	divider = ((apb_clk * 1024) / speed_khz);
	val = R_REG(CLKDIV2);
	val &= ~CLKDIV2_GD_MASK;
	val |= ((divider << CLKDIV2_GD_SHIFT) & CLKDIV2_GD_MASK);
	W_REG(CLKDIV2, val);

    SPI_DBG(("bcm5301x_cca_spi set SPI freq(%dHz) done\n", speed_hz));
    return rv;
}

/* Function : cca_spi_init
 *  - Init Northstar CCA SPI.
 * Return :
 * Note : 
 *     Set the default SPI frequency (offset 0xf0 in chipcommonA).
 *     
 */
int
cca_spi_init(void)
{
    int rv = SPI_ERR_NONE;
	uint32_t pkg;
    
	if ( (R_REG(SPI_PRESENT) & 0x2) != 0x2)
	{
		return -1;
	}
    spi_init = 1;

    /* Get register base address */
	//baseAddr = ioremap(IPROC_CCA_REG_BASE, 0x1000);
    baseAddr = (void *)IPROC_CCA_REG_BASE;

    /* Get cpu/system clock */
    pkg = R_REG(0x18000000);
    pkg = ((pkg >> CC_ID_SHIFT) & CC_ID_MASK);
    /* AXI bus clock = CPU / 2 */
    /* APB clock = AXI bus / 4 */
    if (pkg == PKG_HIGH_SKU) {
        /* CPU 1GHz, AXI 500MHz */
        apb_clk = 125;
    } else {
        /* CPU 800MHz, AXI 400MHz */
        apb_clk = 100;
    }

    SPI_DBG(("bcm5301x_cca_spi module build %s %s. APB Clock @ %dMHz\n", __DATE__, __TIME__, apb_clk));

    /* Set default frequency 8Mhz, min cycle time 200 ns*/
    cca_spi_freq_set(8000000);



    return rv;
}

int
cca_spi_exit(void)
{
    spi_init = 0;

    return 0;
}
unsigned char cca_spi_write_command( uint8_t *buf, int len )
{
    int i = 0,j=0;
    int buf_len = len;
    int process_len = 0;
    uint8_t *process_buf = buf;
    uint quot = 0, remain = 0;
    uint32_t data = 0;
    uint32_t ctrl = 0,ctrl_rd=0;
    uint32_t nod = 0;
//    unsigned short value;

    //SPI_DBG(("\ncca_spi_read_command: start\n"));

    SPI_INIT_CHK;

    while (buf_len > 0) {
        ctrl = ((GSIO_OPCODE << GSIO_CTRL_GC_SHIFT) & GSIO_CTRL_GC_MASK) |
            ((GSIO_ENDIANESS << GSIO_CTRL_ENDIAN_SHIFT) & GSIO_CTRL_ENDIAN_MASK) |
            ((GSIO_CORE_INSERT_CSB<< GSIO_CTRL_GG_SHIFT) & GSIO_CTRL_GG_MASK) |
            ((GSIO_START << GSIO_CTRL_SB_SHIFT) & GSIO_CTRL_SB_MASK);
        quot = (buf_len / 4);
        remain = (buf_len % 4);
        data = 0;
        if (quot != 0) { /* buf_len >= 4 bytes */
            process_len = 4;
            data |= (*process_buf);
            process_buf++;
            data |= (*process_buf << 8);
            process_buf++;
            data |= (*process_buf << 16);
            process_buf++;
            data |= (*process_buf << 24);
            process_buf++;

            nod = 3;
            ctrl |= ((nod << GSIO_CTRL_NOD_SHIFT) & GSIO_CTRL_NOD_MASK);
        }
        else
        { /* buf_len < 4 bytes */
            process_len = remain;

            //printf("\n==========\n");
            for (i = 0; i < remain; i++)
            {
                //data |= (*process_buf << (8*i));
                //data |= (*process_buf << (8*i));

               // printf("\n ==> proc buf = 0x%x", *process_buf);

                //printf("\nRemain = %d",remain);
                W_REG(GSIO_DATA, *(process_buf+(remain-1-i)));
                nod = 0;
                ctrl |= ((nod << GSIO_CTRL_NOD_SHIFT) & GSIO_CTRL_NOD_MASK);
                W_REG(GSIO_CTRL, ctrl);

               //printf("\n ctrl = 0x%x, data wrote 0x%x, i = %d", ctrl, *(process_buf+(remain-1-i)), i);
#if 1
                for (j = 0; j < SPI_TRIES; j++) {
                    ctrl_rd = R_REG(GSIO_CTRL);
                    if (!(ctrl_rd & GSIO_CTRL_SB_MASK)) {
                        break;
                    }
                }
                if (j >= SPI_TRIES) {
                    printf("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl_rd, j);
                    return -1;
                }
#endif

                //process_buf++;
            }

            //printf("\n==========\n");
            //nod = remain-1;
            //ctrl |= ((nod << GSIO_CTRL_NOD_SHIFT) & GSIO_CTRL_NOD_MASK);

        }

        //SPI_DBG(("cca_spi_write: write Reg 0x%x, data 0x%x\n",GSIO_DATA, data));
        //SPI_DBG(("cca_spi_write: write Reg 0x%x, ctrl 0x%x\n",GSIO_CTRL, ctrl));

        //W_REG(GSIO_DATA, data);
        //W_REG(GSIO_CTRL, ctrl);
#if 0
        for (i = 0; i < SPI_TRIES; i++) {
            ctrl = R_REG(GSIO_CTRL);
            if (!(ctrl & GSIO_CTRL_SB_MASK)) {
                break;
            }
        }
        if (i >= SPI_TRIES) {
            SPI_ERR(("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i));
            return -1;
        }

        value = R_REG(GSIO_DATA);
       // SPI_DBG(("\ncca_spi_read_command: done: value=%x\n",value));
#endif
        buf_len -= process_len;

    }
    return SPI_ERR_NONE;
}

unsigned char
cca_spi_read_command(uint8_t *buf, int len)
{
    int i = 0,j=0;
    int buf_len = len;
    int process_len = 0;
    uint8_t *process_buf = buf;
    uint quot = 0, remain = 0;
    uint32_t data = 0;
    uint32_t ctrl = 0, ctrl_rd=0;
    uint32_t nod = 0;
    unsigned short value = 0;

    //SPI_DBG(("\ncca_spi_read_command: start\n"));

    SPI_INIT_CHK;

    while (buf_len > 0) {
        ctrl = ((GSIO_OPCODE << GSIO_CTRL_GC_SHIFT) & GSIO_CTRL_GC_MASK) |
            ((GSIO_ENDIANESS << GSIO_CTRL_ENDIAN_SHIFT) & GSIO_CTRL_ENDIAN_MASK) |
            ((GSIO_CORE_INSERT_CSB<< GSIO_CTRL_GG_SHIFT) & GSIO_CTRL_GG_MASK) |
            ((GSIO_START << GSIO_CTRL_SB_SHIFT) & GSIO_CTRL_SB_MASK);
        quot = (buf_len / 4);
        remain = (buf_len % 4);
        data = 0;
        if (quot != 0) { /* buf_len >= 4 bytes */
            process_len = 4;
            data |= (*process_buf);
            process_buf++;
            data |= (*process_buf << 8);
            process_buf++;
            data |= (*process_buf << 16);
            process_buf++;
            data |= (*process_buf << 24);
            process_buf++;

            nod = 3;
            ctrl |= ((nod << GSIO_CTRL_NOD_SHIFT) & GSIO_CTRL_NOD_MASK);
        }
        else
        { /* buf_len < 4 bytes */
            process_len = remain;

            for (i = 0; i < remain; i++)
            {
                //data |= (*process_buf << (8*i));
                //data |= (*process_buf << (8*i));

                //printf("\n ==> proc buf = 0x%x", *process_buf);
                W_REG(GSIO_DATA, *(process_buf+(remain-1-i)));
                nod = 0;
                ctrl |= ((nod << GSIO_CTRL_NOD_SHIFT) & GSIO_CTRL_NOD_MASK);
                W_REG(GSIO_CTRL, ctrl);

                //printf("\nRemain = %d",remain);

               //printf("\nCTL = 0x%x, data wrote 0x%x, i = %d", ctrl, *(process_buf+(remain-1-i)), i);
#if 1
                for (j = 0; j < SPI_TRIES; j++) {
                    ctrl_rd = R_REG(GSIO_CTRL);
                    if (!(ctrl_rd & GSIO_CTRL_SB_MASK)) {
                        break;
                    }
                }
                if (j >= SPI_TRIES) {
                    printf("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl_rd, j);
                    return -1;
                }

                value = R_REG(GSIO_DATA);
                //SPI_DBG(("\ncca_spi_read_command: done: read value=%x\n",value));

                //process_buf++;
#endif

            }
            //nod = remain-1;
            //ctrl |= ((nod << GSIO_CTRL_NOD_SHIFT) & GSIO_CTRL_NOD_MASK);

        }

        //SPI_DBG(("cca_spi_write: write Reg 0x%x, data 0x%x\n",GSIO_DATA, data));
        //SPI_DBG(("cca_spi_write: write Reg 0x%x, ctrl 0x%x\n",GSIO_CTRL, ctrl));

        //W_REG(GSIO_DATA, data);
        //W_REG(GSIO_CTRL, ctrl);
#if 0
        for (i = 0; i < SPI_TRIES; i++) {
            ctrl = R_REG(GSIO_CTRL);
            if (!(ctrl & GSIO_CTRL_SB_MASK)) {
                break;
            }
        }
        if (i >= SPI_TRIES) {
            SPI_ERR(("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i));
            return -1;
        }

        value = R_REG(GSIO_DATA);
       // SPI_DBG(("\ncca_spi_read_command: done: value=%x\n",value));
#endif
        value = R_REG(GSIO_DATA);
        //SPI_DBG(("\ncca_spi_read_command: done: value=%x\n",value));
        value = R_REG(GSIO_DATA);
        //SPI_DBG(("\ncca_spi_read_command: done: value=%x\n",value));
        value = R_REG(GSIO_DATA);
        //SPI_DBG(("\ncca_spi_read_command: done: value=%x\n",value));

        buf_len -= process_len;
    }
    return (value & 0xFF);
}
#endif /* CONFIG_IPROC */
