/*

 Copyright 2006 Cavium Networks

 Copyright (c) 2003 Vitesse Semiconductor Corporation. All Rights Reserved.
 Unpublished rights reserved under the copyright laws of the United States of 
 America, other countries and international treaties. The software is provided
 without fee. Permission to use, copy, store, modify, disclose, transmit or 
 distribute the software is granted, provided that this copyright notice must 
 appear in any copy, modification, disclosure, transmission or distribution of 
 the software. Vitesse Semiconductor Corporation retains all ownership, 
 copyright, trade secret and proprietary rights in the software. THIS SOFTWARE
 HAS BEEN PROVIDED "AS IS," WITHOUT EXPRESS OR IMPLIED WARRANTY INCLUDING, 
 WITHOUT LIMITATION, IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 PARTICULAR USE AND NON-INFRINGEMENT.
 
 $Id: vcs7385.c,v 1.4 2006/12/15 22:08:21 rfranz Exp $
 $Revision: 1.4 $

*/




#define B_SYSTEM   7  /* System Registers (Sub Block: 0) */
#define R_SYSTEM_GLORESET     0x14 /* Global Reset */
#define R_SYSTEM_CPUCTRL      0x30 /* CPU/interrupt Control */
#define R_SYSTEM_ICPU_CTRL    0x10 /* Internal CPU Control */
#define R_SYSTEM_ICPU_ADDR    0x11 /* Internal CPU On-Chip RAM Address */
#define R_SYSTEM_ICPU_DATA    0x12 /* Internal CPU On-Chip RAM Data */
#define R_SYSTEM_CPUMODE      0x00 /* CPU Transfer Mode */
#define R_SYSTEM_SIPAD        0x01 /* SI Padding */

#define ICPU_CTRL_SRESET        0x01
#define ICPU_CTRL_CLK_EN        0x02
#define ICPU_CTRL_EXT_ACC_EN    0x04
#define ICPU_CTRL_BOOT_EN       0x08
#define ICPU_CTRL_RST_HOLD      0x80

#define VTSS_T_RESET      125000  /* Waiting time (nanoseconds) after reset command */
#include "vcs7385.h"


#include "g8_g5_2_29_unmanaged_Cavium.h"
#define CODE_OFFSET (0)

#include <octeon_hal.h>
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif

int 
vcs7385_read (uint block, uint subblock, const uint reg, ulong * const value)
{
    uint32_t        timeout;
    cvmx_mpi_datx_t mpiData[8];
    cvmx_mpi_sts_t  mpiStatus;
    cvmx_mpi_tx_t   mpiTxCtl;

    cn3005_evb_hs5_vcs_chip_select(0); /* Assert VCS chip select */

    /* A read command asserts the MSB of the command word. The remainder of
     * the word is the target register number.
     */
    cvmx_write_csr (CVMX_MPI_DATX(0), (block<<5) | (0/*READ*/<<4) | (subblock<<0));
    cvmx_write_csr (CVMX_MPI_DATX(1), reg );

    mpiTxCtl.u64 = 0;
    mpiTxCtl.s.leavecs = 0;
    mpiTxCtl.s.txnum   = 2 /* bytes */;
    mpiTxCtl.s.totnum  = 6 /* 2 bytes out and 4 byte back */;
    cvmx_write_csr (CVMX_MPI_TX, mpiTxCtl.u64);

    timeout = 30000000; /* ~1 sec at 400 MHz--ought to be enough at any speed */
    
    do {
	mpiStatus.u64 = cvmx_read_csr (CVMX_MPI_STS);
    } while (mpiStatus.s.busy && (--timeout > 0));

    cn3005_evb_hs5_vcs_chip_select(1); /* Deassert VCS chip select */

    if (!timeout) {
        printf("%s: Timeout error\n", __FUNCTION__);
        return (-1);
    }

    if (mpiStatus.s.rxnum != 6) {
        printf("%s: Receive error\n", __FUNCTION__);
        return (-1);
    }

    /* The returned data appears in MPI_DAT[4..7], not in MPI_DAT[0]. */
    mpiData[2].u64 = cvmx_read_csr (CVMX_MPI_DATX(2));
    mpiData[3].u64 = cvmx_read_csr (CVMX_MPI_DATX(3));
    mpiData[4].u64 = cvmx_read_csr (CVMX_MPI_DATX(4));
    mpiData[5].u64 = cvmx_read_csr (CVMX_MPI_DATX(5));

    *value  = ((mpiData[2].s.data & 0xff) <<  0)
        |     ((mpiData[3].s.data & 0xff) <<  8)
        |     ((mpiData[4].s.data & 0xff) << 16)
        |     ((mpiData[5].s.data & 0xff) << 24);
    
    dprintf ("%s: block 0x%02x, subblock 0x%02x, reg 0x%02x, value 0x%08lx\n",
            __FUNCTION__, block, subblock, reg, *value);

    return (0);
}



void vcs7835_code_write_init(void)
{

    cn3005_evb_hs5_vcs_chip_select(0); /* Assert VCS chip select */

    /* Write a single command and byte value. */
    cvmx_write_csr (CVMX_MPI_DATX(0), (B_SYSTEM<<5) | (1/*WRITE*/<<4) | (0<<0));
    cvmx_write_csr (CVMX_MPI_DATX(1), R_SYSTEM_ICPU_DATA );
    cvmx_write_csr (CVMX_MPI_DATX(2), (0 >>  0) & 0xff);
    cvmx_write_csr (CVMX_MPI_DATX(3), (0 >>  8) & 0xff);
    cvmx_write_csr (CVMX_MPI_DATX(4), (0 >> 16) & 0xff);
    cvmx_write_csr (CVMX_MPI_DATX(5), (0 >> 24) & 0xff);

}

int vcs7835_code_write_byte(uint8_t byte)
{

    uint32_t       timeout;
    cvmx_mpi_sts_t mpiStatus;

    cvmx_write_csr (CVMX_MPI_DATX(2), (byte >>  0) & 0xff);

    cvmx_mpi_tx_t  mpiTxCtl;
    mpiTxCtl.u64 = 0;
    mpiTxCtl.s.leavecs = 0 /* Deassert CS after the transmit */;
    mpiTxCtl.s.txnum   = 6 /* bytes */;
    mpiTxCtl.s.totnum  = 6 /* 6 bytes out and 0 bytes back */;
    cvmx_write_csr (CVMX_MPI_TX, mpiTxCtl.u64);

    timeout = 3000000; /* ~1 sec at 400 MHz -- ought to be enough at any speed */

    do {
        mpiStatus.u64 = cvmx_read_csr (CVMX_MPI_STS);
    } while (mpiStatus.s.busy && (--timeout > 0));


    if (!timeout)
        return (-1);

    return (0);
}
int
vcs7385_write (uint block, uint subblock, const uint reg, ulong const value)
{
    uint32_t       timeout;
    cvmx_mpi_sts_t mpiStatus;
    cvmx_mpi_tx_t  mpiTxCtl;

    dprintf ("%s: block 0x%02x, subblock 0x%02x, reg 0x%02x, value 0x%08lx\n",
            __FUNCTION__, block, subblock, reg, value);

    cn3005_evb_hs5_vcs_chip_select(0); /* Assert VCS chip select */

    /* Write a single command and byte value. */
    cvmx_write_csr (CVMX_MPI_DATX(0), (block<<5) | (1/*WRITE*/<<4) | (subblock<<0));
    cvmx_write_csr (CVMX_MPI_DATX(1), reg );
    cvmx_write_csr (CVMX_MPI_DATX(2), (value >>  0) & 0xff);
    cvmx_write_csr (CVMX_MPI_DATX(3), (value >>  8) & 0xff);
    cvmx_write_csr (CVMX_MPI_DATX(4), (value >> 16) & 0xff);
    cvmx_write_csr (CVMX_MPI_DATX(5), (value >> 24) & 0xff);

    mpiTxCtl.u64 = 0;
    mpiTxCtl.s.leavecs = 0 /* Deassert CS after the transmit */;
    mpiTxCtl.s.txnum   = 6 /* bytes */;
    mpiTxCtl.s.totnum  = 6 /* 6 bytes out and 0 bytes back */;
    cvmx_write_csr (CVMX_MPI_TX, mpiTxCtl.u64);

    timeout = 3000000; /* ~1 sec at 400 MHz -- ought to be enough at any speed */

    do {
        mpiStatus.u64 = cvmx_read_csr (CVMX_MPI_STS);
    } while (mpiStatus.s.busy && (--timeout > 0));

    cn3005_evb_hs5_vcs_chip_select(1); /* Deassert VCS chip select */

    if (!timeout)
        return (-1);

    return (0);
}


/* VCS Chip Select is on GPIO 3 */
void cn3005_evb_hs5_vcs_chip_select__(int onoff)
{
    //    octeon_gpio_cfg_output(3);

    if (onoff)
        octeon_gpio_set(1<<3);    /* CS = 1 */
    else
        octeon_gpio_clr(1<<3);  /* CS = 0 */
}



#define VTSS_NSLEEP(nsec) { \
    uint64_t timeout_time = cvmx_get_cycle() + 1ull * nsec; \
    while (cvmx_get_cycle() < timeout_time); \
}




void load_8051_code(void)
{
    ulong data = 0x0;
    int i;

    //vcs7385_write(B_SYSTEM, 0, R_SYSTEM_GLORESET, (1<<4)|(1<<3)|(1<<2));
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_GLORESET, (1<<4)|(0<<3)|(1<<2));

    dprintf("Code size is: %d bytes\n", (int)sizeof(code_array));
    vcs7385_read(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, &data);
    dprintf("ICPU_CTL (orig): 0x%lx\n", data);

    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_CPUCTRL, 0);

    vcs7385_read(B_SYSTEM, 0, R_SYSTEM_CPUCTRL, &data);
    dprintf("SYSTEM_CPUCTRL: 0x%lx\n", data);

    /* Stop CPU, enable external access */

    /* Stop clock */
    data &= ~ICPU_CTRL_CLK_EN;
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, data);
    /* enable external access and reset hold */
    data |= ICPU_CTRL_EXT_ACC_EN | ICPU_CTRL_RST_HOLD;
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, data);
    /* Put into reset */
    data &= ~ICPU_CTRL_SRESET;
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, data);



    vcs7385_read(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, &data);
    dprintf("ICPU_CTL (before writes): 0x%lx\n", data);

    /* Now write pattern to internal ram */

#if 0
    /* Clear all internal memory */
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_ADDR, 0);
    for (i = 0;i < 8192; i++)
        vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_DATA, 0);
#endif

    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_ADDR, 0);
#if 1
    for (i = 0;i < sizeof(code_array) - CODE_OFFSET; i++)
        vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_DATA, code_array[i + CODE_OFFSET]);
#else
    /* This does not work after power cycle..... */
    /* Use optimized code loading routines to speed up loading */
    vcs7835_code_write_init();
    for (i = 0;i < sizeof(code_array) - CODE_OFFSET; i++)
        vcs7835_code_write_byte(code_array[i + CODE_OFFSET]);
    cn3005_evb_hs5_vcs_chip_select(1); /* Deassert VCS chip select */
#endif

#ifdef DEBUG
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_ADDR, 0);
    for (i = 0;i < sizeof(code_array) - CODE_OFFSET; i++)
    {
        vcs7385_read(B_SYSTEM, 0, R_SYSTEM_ICPU_DATA, &data);
        if (i < 32)
            dprintf("ram[0x%lx]:0x%lx\n", (unsigned long)i, (unsigned long)data);
        if (data != code_array[i + CODE_OFFSET])
        {
            dprintf("ERROR: code mismatch at byte %d\n", i);
        }
    }
#endif

    /* Clear scratch memory */
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_ADDR, 1 << 31);
    for (i = 0;i < 256; i++)
    {
        vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_DATA, 0);
    }

    /* Now try to reset 8051 to boot from RAM */

    vcs7385_read(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, &data);
    dprintf("Reseting 8051, icpu_ctrl: 0x%lx\n", data);


    data &= ~ICPU_CTRL_RST_HOLD;
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, data);
    vcs7385_read(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, &data);dprintf("line %d: ICPU_CTRL: 0x%lx\n", __LINE__, data);
    /* Clear EXT_ACC_EN */
    data &= ~ICPU_CTRL_EXT_ACC_EN;
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, data);
    vcs7385_read(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, &data);dprintf("line %d: ICPU_CTRL: 0x%lx\n", __LINE__, data);

    data &= ~ICPU_CTRL_SRESET;
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, data);
    vcs7385_read(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, &data);dprintf("line %d: ICPU_CTRL: 0x%lx\n", __LINE__, data);

    data |= ICPU_CTRL_BOOT_EN | ICPU_CTRL_CLK_EN;
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, data);
    vcs7385_read(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, &data);dprintf("line %d: ICPU_CTRL: 0x%lx\n", __LINE__, data);

    dprintf("Final ICPU_CTRL: 0x%lx\n", data);

#ifdef DEBUG
    /* Loop reading ICPU_CTRL to see if code changes divisor */
    while (data == 0x1f4b)
    {
        vcs7385_read(B_SYSTEM, 0, R_SYSTEM_ICPU_CTRL, &data);
        dprintf("loop ICPU_CTRL: 0x%lx\n", data);
    }
#endif



}


int vitesse_init(void)
{

    printf("Loading Vitesse switch code....");

    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_GLORESET, (1<<0));
    VTSS_NSLEEP(VTSS_T_RESET);
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_CPUMODE, 0x81818181);

    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_CPUCTRL, 0<<8 | (0 ? 0 : 1)<<5);
    vcs7385_write(B_SYSTEM, 0, R_SYSTEM_SIPAD, 0);


    load_8051_code();
    printf("done.\n");
    return 0;
}
