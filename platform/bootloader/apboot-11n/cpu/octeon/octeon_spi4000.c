/*************************************************************************
Copyright (c) 2004-2005 Cavium Networks (support@cavium.com). All rights
reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Cavium Networks' name may not be used to endorse or promote products
derived from this software without specific prior written permission.

This Software, including technical data, may be subject to U.S. export
control laws, including the U.S. Export Administration Act and its
associated regulations, and may be subject to export or import
regulations in other countries. You warrant that You will comply
strictly in all respects with all such regulations and acknowledge that
you have the responsibility to obtain licenses to export, re-export or
import the Software.

TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
AND WITH ALL FAULTS AND CAVIUM MAKES NO PROMISES, REPRESENTATIONS OR
WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY)
WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE
RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.

*************************************************************************/

/**
 * @file
 *
 * Support library for the SPI4000 card
 *
 * File version info: $Id: octeon_spi4000.c,v 1.7 2006/12/01 19:05:54 rfranz Exp $ $Name: SDK_1_5_0_build_187 $
 */

#include "common.h"

#ifdef OCTEON_SPI4000_ENET

#include "octeon_boot.h"
#include "octeon_hal.h"


#define SPI4000_BASE                (0x8000660000000000ull)
#define SPI4000_READ_ADDRESS        (SPI4000_BASE | 0xf0)
#define SPI4000_WRITE_ADDRESS       (SPI4000_BASE | 0xf2)
#define SPI4000_READ_DATA           (0x830066f400000000ull)
#define SPI4000_WRITE_DATA          (SPI4000_BASE | 0xf8)
#define SPI4000_DO_READ             (SPI4000_BASE | 0xfc)
#define SPI4000_GET_READ_STATUS     (0x830066fd00000000ull)
#define SPI4000_DO_WRITE            (SPI4000_BASE | 0xfe)
#define SPI4000_GET_WRITE_STATUS    (0x830066ff00000000ull)
#define MS                          (400000ull) /*  Not exactly a millisecond, but close enough for our delays */


/**
 * Read a value from the twsi interface
 *
 * @return Value after the read completes
 */
static inline uint64_t cvmx_twsi_read(void)
{
    cvmx_mio_tws_sw_twsi_t result;
    do
    {
        result.u64 = cvmx_read_csr(CVMX_MIO_TWS_SW_TWSI);
    } while (result.s.v);

#if 0
    if (result.s.r == 0)
        printf("TWSI: Read failed\n");
#endif

    return result.u64;
}


/**
 * Write a command to the twsi interface
 *
 * @param value  Value to write
 */
static inline int cvmx_twsi_write(uint64_t value)
{
    cvmx_write_csr(CVMX_MIO_TWS_SW_TWSI, value);

    cvmx_mio_tws_sw_twsi_t result;
    result.u64 = cvmx_twsi_read();
    if (result.s.r == 0)
        return -1;
    else
        return 0;
}


/**
 * Write data to the specified SPI4000 address
 *
 * @param address Address to write to
 * @param data    Data to write
 */
static void cvmx_spi4000_write(int address, int data)
{
    // set write address
    int addressh = (address >> 8) & 0xff;
    int addressl = address & 0xff;

    // set write address
    cvmx_twsi_write(SPI4000_WRITE_ADDRESS);
    cvmx_twsi_write(SPI4000_BASE | addressh);
    cvmx_twsi_write(SPI4000_BASE | addressl);

    // set write data
    int data0 = (data>>24) & 0xff;
    int data1 = (data>>16) & 0xff;
    int data2 = (data>>8) & 0xff;
    int data3 = (data>>0) & 0xff;

    cvmx_twsi_write(SPI4000_WRITE_DATA);
    cvmx_twsi_write(SPI4000_BASE | data0);
    cvmx_twsi_write(SPI4000_BASE | data1);
    cvmx_twsi_write(SPI4000_BASE | data2);
    cvmx_twsi_write(SPI4000_BASE | data3);

    // do the write
    cvmx_twsi_write(SPI4000_DO_WRITE);

    do
    {
        // check the status
        cvmx_twsi_write(SPI4000_GET_WRITE_STATUS);
    } while ((cvmx_twsi_read() & 0xff) != 4);
}


/**
 * Read data from the SPI4000.
 *
 * @param address Address to read from
 * @return Value at the specified address
 */
static int cvmx_spi4000_read(int address)
{
    int addressh = (address >> 8) & 0xff;
    int addressl = address & 0xff;

    cvmx_twsi_write(SPI4000_READ_ADDRESS);
    cvmx_twsi_write(SPI4000_BASE | addressh);
    cvmx_twsi_write(SPI4000_BASE | addressl);

    // do the read
    cvmx_twsi_write(SPI4000_DO_READ);

    cvmx_twsi_write(0x8100660000000000ull);
    uint64_t result = cvmx_twsi_read();
    while (result & 0xff)
    {
        // check the status
        cvmx_twsi_write(SPI4000_GET_READ_STATUS);
        result = cvmx_twsi_read();
    }

    cvmx_twsi_write(0x830066f400000000ull);
    int data0 = cvmx_twsi_read() & 0xff;
    cvmx_twsi_write(0x8100660000000000ull);
    int data1 = cvmx_twsi_read() & 0xff;
    cvmx_twsi_write(0x8100660000000000ull);
    int data2 = cvmx_twsi_read() & 0xff;
    cvmx_twsi_write(0x8100660000000000ull);
    int data3 = cvmx_twsi_read() & 0xff;

    return (data0<<24) | (data1<<16) | (data2<<8) | data3;
}


/**
 * Configure the SPI4000 Macs
 */
static void cvmx_spi4000_mac_config(void)
{
    //       IXF1010 configuration
    //       ---------------------
    //
    //       Step 1: Apply soft reset to TxFIFO and MAC
    //               MAC soft reset register. address=0x505
    //               TxFIFO soft reset. address=0x620
    //
    cvmx_spi4000_write(0x0505, 0x3ff);  //               OUT  0x505, 0x3ff  //reset all the MACs
    cvmx_spi4000_write(0x0620, 0x3ff);  //               OUT  0x620, 0x3ff  //reset the TX FIFOs
    //
    //               Global address and Configuration Register. address=0x500
    //
    //
    //       Step 2: Apply soft reset to RxFIFO and SPI.
    //
    cvmx_spi4000_write(0x059e, 0x3ff);  //               OUT  0x59e, 0x3ff  //reset the RX FIFOs
    //
    //
    //       Step 3a: Take the MAC out of softreset
    //               MAC soft reset register. address=0x505
    //
    cvmx_spi4000_write(0x0505, 0x0);    //               OUT  0x505, 0x0    //reset all the MACs
    //
    //
    //       Step 3b: De-assert port enables.
    //               Global address and Configuration Register. address=0x500
    //
    cvmx_spi4000_write(0x0500, 0x0);    //               OUT  0x500, 0x0    //disable all ports
    //
    //
    //       Step 4: Assert Clock mode change En.
    //               Clock and interface mode Change En. address=Serdes base + 0x14
    //               Serdes (Serializer/de-serializer). address=0x780
    //
    //           Can't find this one
    //
    //
    //       Step 5: Set MAC interface mode GMII speed.
    //               MAC interface mode and RGMII speed register. address=port_index+0x10
    //
    //               //repeat the following for all ports.
    //               OUT port_index+0x10, 0x07     //RGMII 1000 Mbps operation.
    cvmx_spi4000_write(0x0010, 0x3);    //
    cvmx_spi4000_write(0x0090, 0x3);    //
    cvmx_spi4000_write(0x0110, 0x3);    //
    cvmx_spi4000_write(0x0190, 0x3);    //
    cvmx_spi4000_write(0x0210, 0x3);    //
    cvmx_spi4000_write(0x0290, 0x3);    //
    cvmx_spi4000_write(0x0310, 0x3);    //
    cvmx_spi4000_write(0x0390, 0x3);    //
    cvmx_spi4000_write(0x0410, 0x3);    //
    cvmx_spi4000_write(0x0490, 0x3);    //
    //
    //
    //       Step 6: Change Interface to Copper mode
    //               Interface mode register. address=0x501
    //
    //               Can't find this
    //
    //
    //       Step 7: MAC configuration
    //               Station address configuration.
    //               Source MAC address low register. Source MAC address bit 31-0. address=port_index+0x00
    //               Source MAC address high register. Source MAC address bit 47-32. address=port_index+0x01
    //               where Port index is 0x0 to 0x5.
    //               This address is inserted in the source address filed when transmitting pause frames,
    //               and is also used to compare against unicast pause frames at the receiving side.
    //
    //               //repeat the following for all ports.
    //               OUT port_index+0x00, source MAC address low.
    cvmx_spi4000_write(0x0000, 0x0000); //
    cvmx_spi4000_write(0x0080, 0x0000); //
    cvmx_spi4000_write(0x0100, 0x0000); //
    cvmx_spi4000_write(0x0180, 0x0000); //
    cvmx_spi4000_write(0x0200, 0x0000); //
    cvmx_spi4000_write(0x0280, 0x0000); //
    cvmx_spi4000_write(0x0300, 0x0000); //
    cvmx_spi4000_write(0x0380, 0x0000); //
    cvmx_spi4000_write(0x0400, 0x0000); //
    cvmx_spi4000_write(0x0480, 0x0000); //
    //               OUT port_index+0x01, source MAC address high.
    cvmx_spi4000_write(0x0001, 0x0000); //
    cvmx_spi4000_write(0x0081, 0x0000); //
    cvmx_spi4000_write(0x0101, 0x0000); //
    cvmx_spi4000_write(0x0181, 0x0000); //
    cvmx_spi4000_write(0x0201, 0x0000); //
    cvmx_spi4000_write(0x0281, 0x0000); //
    cvmx_spi4000_write(0x0301, 0x0000); //
    cvmx_spi4000_write(0x0381, 0x0000); //
    cvmx_spi4000_write(0x0401, 0x0000); //
    cvmx_spi4000_write(0x0481, 0x0000); //
    //
    //
    //       Step 8: Set desired duplex mode
    //              Desired duplex register. address=port_index+0x02
    //
    //              Reserved
    //
    //       Step 9: Other configuration.
    //           FC Enable Register.             address=port_index+0x12
    //       Discard Unknown Control Frame.  address=port_index+0x15
    //               Diverse config write register.  address=port_index+0x18
    //               RX Packet Filter register.      address=port_index+0x19
    //
    //      Step 9a:
    cvmx_spi4000_write(0x0012, 0x7);    //           TX FD FC Enabled / RX FD FC Enabled
    cvmx_spi4000_write(0x0092, 0x7);    //           TX FD FC Enabled / RX FD FC Enabled
    cvmx_spi4000_write(0x0112, 0x7);    //           TX FD FC Enabled / RX FD FC Enabled
    cvmx_spi4000_write(0x0192, 0x7);    //           TX FD FC Enabled / RX FD FC Enabled
    cvmx_spi4000_write(0x0212, 0x7);    //           TX FD FC Enabled / RX FD FC Enabled
    cvmx_spi4000_write(0x0292, 0x7);    //           TX FD FC Enabled / RX FD FC Enabled
    cvmx_spi4000_write(0x0312, 0x7);    //           TX FD FC Enabled / RX FD FC Enabled
    cvmx_spi4000_write(0x0392, 0x7);    //           TX FD FC Enabled / RX FD FC Enabled
    cvmx_spi4000_write(0x0412, 0x7);    //           TX FD FC Enabled / RX FD FC Enabled
    cvmx_spi4000_write(0x0492, 0x7);    //           TX FD FC Enabled / RX FD FC Enabled
    //
    //      Step 9b:
    cvmx_spi4000_write(0x0015, 0x1);    //               Discard unknown control frames
    cvmx_spi4000_write(0x0095, 0x1);    //               Discard unknown control frames
    cvmx_spi4000_write(0x0115, 0x1);    //               Discard unknown control frames
    cvmx_spi4000_write(0x0195, 0x1);    //               Discard unknown control frames
    cvmx_spi4000_write(0x0215, 0x1);    //               Discard unknown control frames
    cvmx_spi4000_write(0x0295, 0x1);    //               Discard unknown control frames
    cvmx_spi4000_write(0x0315, 0x1);    //               Discard unknown control frames
    cvmx_spi4000_write(0x0395, 0x1);    //               Discard unknown control frames
    cvmx_spi4000_write(0x0415, 0x1);    //               Discard unknown control frames
    cvmx_spi4000_write(0x0495, 0x1);    //               Discard unknown control frames
    //
    //      Step 9c:
    cvmx_spi4000_write(0x0018, 0x11cd); //       Enable auto-CRC / auto-padding
    cvmx_spi4000_write(0x0098, 0x11cd); //       Enable auto-CRC / auto-padding
    cvmx_spi4000_write(0x0118, 0x11cd); //       Enable auto-CRC / auto-padding
    cvmx_spi4000_write(0x0198, 0x11cd); //       Enable auto-CRC / auto-padding
    cvmx_spi4000_write(0x0218, 0x11cd); //       Enable auto-CRC / auto-padding
    cvmx_spi4000_write(0x0298, 0x11cd); //       Enable auto-CRC / auto-padding
    cvmx_spi4000_write(0x0318, 0x11cd); //       Enable auto-CRC / auto-padding
    cvmx_spi4000_write(0x0398, 0x11cd); //       Enable auto-CRC / auto-padding
    cvmx_spi4000_write(0x0418, 0x11cd); //       Enable auto-CRC / auto-padding
    cvmx_spi4000_write(0x0498, 0x11cd); //       Enable auto-CRC / auto-padding
    //
    //      Step 9d:
    cvmx_spi4000_write(0x0019, 0x00);   //       Drop bad CRC / Drop Pause / No DAF
    cvmx_spi4000_write(0x0099, 0x00);   //       Drop bad CRC / Drop Pause / No DAF
    cvmx_spi4000_write(0x0119, 0x00);   //       Drop bad CRC / Drop Pause / No DAF
    cvmx_spi4000_write(0x0199, 0x00);   //       Drop bad CRC / Drop Pause / No DAF
    cvmx_spi4000_write(0x0219, 0x00);   //       Drop bad CRC / Drop Pause / No DAF
    cvmx_spi4000_write(0x0299, 0x00);   //       Drop bad CRC / Drop Pause / No DAF
    cvmx_spi4000_write(0x0319, 0x00);   //       Drop bad CRC / Drop Pause / No DAF
    cvmx_spi4000_write(0x0399, 0x00);   //       Drop bad CRC / Drop Pause / No DAF
    cvmx_spi4000_write(0x0419, 0x00);   //       Drop bad CRC / Drop Pause / No DAF
    cvmx_spi4000_write(0x0499, 0x00);   //       Drop bad CRC / Drop Pause / No DAF
    //
    //      Step 9d:
    cvmx_spi4000_write(0x059f, 0x03ff); //       Drop frames
    //
    //
    //      Step 9e: Set the TX FIFO marks
    cvmx_spi4000_write(0x0600, 0x0900); //               TXFIFO High watermark
    cvmx_spi4000_write(0x0601, 0x0900); //               TXFIFO High watermark
    cvmx_spi4000_write(0x0602, 0x0900); //               TXFIFO High watermark
    cvmx_spi4000_write(0x0603, 0x0900); //               TXFIFO High watermark
    cvmx_spi4000_write(0x0604, 0x0900); //               TXFIFO High watermark
    cvmx_spi4000_write(0x0605, 0x0900); //               TXFIFO High watermark
    cvmx_spi4000_write(0x0606, 0x0900); //               TXFIFO High watermark
    cvmx_spi4000_write(0x0607, 0x0900); //               TXFIFO High watermark
    cvmx_spi4000_write(0x0608, 0x0900); //               TXFIFO High watermark
    cvmx_spi4000_write(0x0609, 0x0900); //               TXFIFO High watermark
    //
    cvmx_spi4000_write(0x060a, 0x0800); //               TXFIFO Low watermark
    cvmx_spi4000_write(0x060b, 0x0800); //               TXFIFO Low watermark
    cvmx_spi4000_write(0x060c, 0x0800); //               TXFIFO Low watermark
    cvmx_spi4000_write(0x060d, 0x0800); //               TXFIFO Low watermark
    cvmx_spi4000_write(0x060e, 0x0800); //               TXFIFO Low watermark
    cvmx_spi4000_write(0x060f, 0x0800); //               TXFIFO Low watermark
    cvmx_spi4000_write(0x0610, 0x0800); //               TXFIFO Low watermark
    cvmx_spi4000_write(0x0611, 0x0800); //               TXFIFO Low watermark
    cvmx_spi4000_write(0x0612, 0x0800); //               TXFIFO Low watermark
    cvmx_spi4000_write(0x0613, 0x0800); //               TXFIFO Low watermark
    //
    cvmx_spi4000_write(0x0614, 0x0380); //               TXFIFO threshold
    cvmx_spi4000_write(0x0615, 0x0380); //               TXFIFO threshold
    cvmx_spi4000_write(0x0616, 0x0380); //               TXFIFO threshold
    cvmx_spi4000_write(0x0617, 0x0380); //               TXFIFO threshold
    cvmx_spi4000_write(0x0618, 0x0380); //               TXFIFO threshold
    cvmx_spi4000_write(0x0619, 0x0380); //               TXFIFO threshold
    cvmx_spi4000_write(0x061a, 0x0380); //               TXFIFO threshold
    cvmx_spi4000_write(0x061b, 0x0380); //               TXFIFO threshold
    cvmx_spi4000_write(0x061c, 0x0380); //               TXFIFO threshold
    cvmx_spi4000_write(0x061d, 0x0380); //               TXFIFO threshold
    //
    //
    //       Step 10: SPI4 Rx Configuration
    //               This step is done during N2 config
    //
    //
    //       Step 11: De-assert clock mode change en.
    //                (Can't find this one)
    //
    //
    //       Step 12: De-assert RxFIFO and SPI Rx/Tx reset
    cvmx_spi4000_write(0x059e, 0x0);    //               OUT  0x59e, 0x0    //reset the RX FIFOs
    //
    //
    //       Step 13: De-assert TxFIFO and MAC reset
    cvmx_spi4000_write(0x0620, 0x0);    //               OUT  0x620, 0x0    //reset the TX FIFOs
    //
    //
    //       Step 14: Assert port enable
    //               Global address and Configuration Register. address=0x500
    //
    cvmx_spi4000_write(0x0500, 0x03ff); //               OUT  0x500, 0x03ff //enable all ports
    //
    //
    //       Step 15: Disable loopback
    //                (Can't find this one)
    //
}


/**
 * Configure the SPI4000 PHYs
 */
static void cvmx_spi4000_phy_config(void)
{
    //
    //   Step 16: PHY configuration
    // cvmx_spi4000_write(0x0683, 0x6);     //            Enable MDI and Autoscan PHY registers
    //
    cvmx_spi4000_write(0x0681, 0x00e2);     //            Enable RGMII DELAYS for TX_CLK and RX_CLK (see spec) (jim had 0ce2 4x master ds)
    cvmx_spi4000_write(0x0680, 0x110014);   //            DO the PHY WRITE, addr 20
    cvmx_spi4000_write(0x0680, 0x110114);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110214);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110314);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110414);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110514);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110614);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110714);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110814);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110914);   //            DO the PHY WRITE
    //
    cvmx_spi4000_write(0x0681, 0x1140);     //            Configure for 1Gbit, auto-negoation, full duplex, disable PHY loopback
    cvmx_spi4000_write(0x0680, 0x110000);   //            DO the PHY WRITE, addr 0
    cvmx_spi4000_write(0x0680, 0x110100);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110200);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110300);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110400);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110500);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110600);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110700);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110800);   //            DO the PHY WRITE
    cvmx_spi4000_write(0x0680, 0x110900);   //            DO the PHY WRITE
    //
    cvmx_spi4000_write(0x0681, 0x0de1);     //
    cvmx_spi4000_write(0x0680, 0x110004);   //
    cvmx_spi4000_write(0x0680, 0x110104);   //
    cvmx_spi4000_write(0x0680, 0x110204);   //
    cvmx_spi4000_write(0x0680, 0x110304);   //
    cvmx_spi4000_write(0x0680, 0x110404);   //
    cvmx_spi4000_write(0x0680, 0x110504);   //
    cvmx_spi4000_write(0x0680, 0x110604);   //
    cvmx_spi4000_write(0x0680, 0x110704);   //
    cvmx_spi4000_write(0x0680, 0x110804);   //
    cvmx_spi4000_write(0x0680, 0x110904);   //

    cvmx_spi4000_write(0x0681, 0x9140);     //            Enable PHY reset
    cvmx_spi4000_write(0x0680, 0x110000);   //            DO the PHY WRITE, mac 0 port 0, addr 0
    cvmx_spi4000_write(0x0680, 0x110100);   //            DO the PHY WRITE, mac 0 port 1
    cvmx_spi4000_write(0x0680, 0x110200);   //            DO the PHY WRITE, mac 0 port 2
    cvmx_spi4000_write(0x0680, 0x110300);   //            DO the PHY WRITE, mac 0 port 3
    cvmx_spi4000_write(0x0680, 0x110400);   //            DO the PHY WRITE, mac 0 port 3
    cvmx_spi4000_write(0x0680, 0x110500);   //            DO the PHY WRITE, mac 0 port 3
    cvmx_spi4000_write(0x0680, 0x110600);   //            DO the PHY WRITE, mac 0 port 3
    cvmx_spi4000_write(0x0680, 0x110700);   //            DO the PHY WRITE, mac 0 port 3
    cvmx_spi4000_write(0x0680, 0x110800);   //            DO the PHY WRITE, mac 0 port 3
    cvmx_spi4000_write(0x0680, 0x110900);   //            DO the PHY WRITE, mac 0 port 3

    // cvmx_spi4000_write(0x0681, 0x1340);      //            Enable PHY restart auto neg.
    // cvmx_spi4000_write(0x0680, 0x110000);    //            DO the PHY WRITE
    // cvmx_spi4000_write(0x0680, 0x110100);    //            DO the PHY WRITE
    // cvmx_spi4000_write(0x0680, 0x110200);    //            DO the PHY WRITE
    // cvmx_spi4000_write(0x0680, 0x110300);    //            DO the PHY WRITE
    // cvmx_spi4000_write(0x0680, 0x110400);    //            DO the PHY WRITE
    // cvmx_spi4000_write(0x0680, 0x110500);    //            DO the PHY WRITE
    // cvmx_spi4000_write(0x0680, 0x110600);    //            DO the PHY WRITE
    // cvmx_spi4000_write(0x0680, 0x110700);    //            DO the PHY WRITE
    // cvmx_spi4000_write(0x0680, 0x110800);    //            DO the PHY WRITE
    // cvmx_spi4000_write(0x0680, 0x110900);    //            DO the PHY WRITE
}

/* Detect presence of SPI4000 on a given interface */
int cvmx_spi4000_detect(int interface)
{
    // Check for the presence of a SPI4000. If it isn't there,
    // these writes will timeout.
    if (cvmx_twsi_write(SPI4000_WRITE_ADDRESS))
        return -1;
    if (cvmx_twsi_write(SPI4000_BASE))
        return -1;
    if (cvmx_twsi_write(SPI4000_BASE))
        return -1;
    return 0;
}

/**
 * Initialize the SPI4000 for use
 *
 * @param interface SPI interface the SPI4000 is connected to
 */
int cvmx_spi4000_initialize(int interface)
{
    uint64_t result;

    // Configure the PKO port mode regardless of the presence of
    // the SPI4000. This allows the ports to be used with something
    // other than a SPI4000.
    cvmx_pko_reg_gmx_port_mode_t pko_mode;
    pko_mode.u64 = cvmx_read_csr(CVMX_PKO_REG_GMX_PORT_MODE);
    if (interface == 0)
        pko_mode.s.mode0 = 0;
    else
        pko_mode.s.mode1 = 0;
    cvmx_write_csr(CVMX_PKO_REG_GMX_PORT_MODE, pko_mode.u64);

    // Configure GMX
    // -------------------------------------------------
    cvmx_write_csr(CVMX_GMXX_TX_PRTS(interface), 0xa);
    // PRTS           [ 4: 0] ( 5b) = 10

    if (cvmx_spi4000_detect(interface) < 0)
        return -1;

    cvmx_spi4000_mac_config();
    cvmx_spi4000_phy_config();

    // Bringing up SPI4000 board
    //      Spi4 Interface0
    // -------------------------------------------------

    // Reset the Spi4 deskew logic
    // -------------------------------------------------
    cvmx_write_csr(CVMX_SPXX_DBG_DESKEW_CTL(interface), 0x00200000);
    // SPX0_DBG_DESKEW_CTL
    // DLLDIS         [ 0: 0] ( 1b) = 0
    // DLLFRC         [ 1: 1] ( 1b) = 0
    // OFFDLY         [ 7: 2] ( 6b) = 0
    // BITSEL         [12: 8] ( 5b) = 0
    // OFFSET         [17:13] ( 5b) = 0
    // MUX            [18:18] ( 1b) = 0
    // INC            [19:19] ( 1b) = 0
    // DEC            [20:20] ( 1b) = 0
    // CLRDLY         [21:21] ( 1b) = 1
    cvmx_wait(100 * MS);

    // Setup the CLKDLY right in the middle
    // -------------------------------------------------
    cvmx_write_csr(CVMX_SPXX_CLK_CTL(interface), 0x00000830);
    // SRXDLCK        [ 0: 0] ( 1b) = 0
    // RCVTRN         [ 1: 1] ( 1b) = 0
    // DRPTRN         [ 2: 2] ( 1b) = 0
    // SNDTRN         [ 3: 3] ( 1b) = 0
    // STATRCV        [ 4: 4] ( 1b) = 1
    // STATDRV        [ 5: 5] ( 1b) = 1
    // RUNBIST        [ 6: 6] ( 1b) = 0
    // CLKDLY         [11: 7] ( 5b) = 10
    // SRXLCK         [12:12] ( 1b) = 0
    // STXLCK         [13:13] ( 1b) = 0
    // SEETRN         [14:14] ( 1b) = 0
    cvmx_wait(100 * MS);

    // Lock SRX0 DLL
    // -------------------------------------------------
    cvmx_write_csr(CVMX_SPXX_CLK_CTL(interface), 0x00000831);
    // SRXDLCK        [ 0: 0] ( 1b) = 1
    // RCVTRN         [ 1: 1] ( 1b) = 0
    // DRPTRN         [ 2: 2] ( 1b) = 0
    // SNDTRN         [ 3: 3] ( 1b) = 0
    // STATRCV        [ 4: 4] ( 1b) = 1
    // STATDRV        [ 5: 5] ( 1b) = 1
    // RUNBIST        [ 6: 6] ( 1b) = 0
    // CLKDLY         [11: 7] ( 5b) = 10
    // SRXLCK         [12:12] ( 1b) = 0
    // STXLCK         [13:13] ( 1b) = 0
    // SEETRN         [14:14] ( 1b) = 0

    // Waiting for Inf0 Spi4 RX DLL to lock
    // -------------------------------------------------
    cvmx_wait(100 * MS);

    // Enable dynamic alignment
    // -------------------------------------------------
    cvmx_spxx_trn4_ctl_t spxx_trn4_ctl;
    spxx_trn4_ctl.u64 = 0;
    spxx_trn4_ctl.s.mux_en = 1;
    spxx_trn4_ctl.s.macro_en = 1;
    spxx_trn4_ctl.s.maxdist = 16;
    spxx_trn4_ctl.s.jitter = 1;
    cvmx_write_csr(CVMX_SPXX_TRN4_CTL(interface), spxx_trn4_ctl.u64);
    // MUX_EN         [ 0: 0] ( 1b) = 1
    // MACRO_EN       [ 1: 1] ( 1b) = 1
    // MAXDIST        [ 6: 2] ( 5b) = 16
    // SET_BOOT       [ 7: 7] ( 1b) = 0
    // CLR_BOOT       [ 8: 8] ( 1b) = 1
    // JITTER         [11: 9] ( 3b) = 1
    // TRNTEST        [12:12] ( 1b) = 0
    cvmx_write_csr(CVMX_SPXX_DBG_DESKEW_CTL(interface), 0x0);
    // DLLDIS         [ 0: 0] ( 1b) = 0
    // DLLFRC         [ 1: 1] ( 1b) = 0
    // OFFDLY         [ 7: 2] ( 6b) = 0
    // BITSEL         [12: 8] ( 5b) = 0
    // OFFSET         [17:13] ( 5b) = 0
    // MUX            [18:18] ( 1b) = 0
    // INC            [19:19] ( 1b) = 0
    // DEC            [20:20] ( 1b) = 0
    // CLRDLY         [21:21] ( 1b) = 0

    // SRX0 Ports
    // -------------------------------------------------
    cvmx_write_csr(CVMX_SRXX_COM_CTL(interface), 0x00000090);
    // INF_EN         [ 0: 0] ( 1b) = 0
    // ST_EN          [ 3: 3] ( 1b) = 0
    // PRTS           [ 9: 4] ( 6b) = 9

    // SRX0 Calendar Table
    // -------------------------------------------------
    cvmx_write_csr(CVMX_SRXX_SPI4_CALX(0, interface), 0x00013210);
    // PRT0           [ 4: 0] ( 5b) = 0
    // PRT1           [ 9: 5] ( 5b) = 1
    // PRT2           [14:10] ( 5b) = 2
    // PRT3           [19:15] ( 5b) = 3
    // ODDPAR         [20:20] ( 1b) = 1
    cvmx_write_csr(CVMX_SRXX_SPI4_CALX(1, interface), 0x00017654);
    // PRT0           [ 4: 0] ( 5b) = 4
    // PRT1           [ 9: 5] ( 5b) = 5
    // PRT2           [14:10] ( 5b) = 6
    // PRT3           [19:15] ( 5b) = 7
    // ODDPAR         [20:20] ( 1b) = 1
    cvmx_write_csr(CVMX_SRXX_SPI4_CALX(2, interface), 0x00000098);
    // PRT0           [ 4: 0] ( 5b) = 8
    // PRT1           [ 9: 5] ( 5b) = 9
    // PRT2           [14:10] ( 5b) = 0
    // PRT3           [19:15] ( 5b) = 0
    // ODDPAR         [20:20] ( 1b) = 0
    cvmx_write_csr(CVMX_SRXX_SPI4_STAT(interface), 0x0000010a);
    // LEN            [ 7: 0] ( 8b) = a
    // M              [15: 8] ( 8b) = 1

    // STX0 Config
    // -------------------------------------------------
    cvmx_write_csr(CVMX_STXX_ARB_CTL(interface), 0x0);
    // IGNTPA         [ 3: 3] ( 1b) = 0
    // MINTRN         [ 5: 5] ( 1b) = 0
    cvmx_write_csr(CVMX_GMXX_TX_SPI_MAX(interface), 0x0408);
    // MAX2           [15: 8] ( 8b) = 4
    // MAX1           [ 7: 0] ( 8b) = 8
    cvmx_write_csr(CVMX_GMXX_TX_SPI_THRESH(interface), 0x4);
    // THRESH         [ 5: 0] ( 6b) = 4
    cvmx_write_csr(CVMX_GMXX_TX_SPI_CTL(interface), 0x0);
    // ENFORCE        [ 2: 2] ( 1b) = 0
    // TPA_CLR        [ 1: 1] ( 1b) = 0
    // CONT_PKT       [ 0: 0] ( 1b) = 0

    // STX0 Training Control
    // -------------------------------------------------
    cvmx_write_csr(CVMX_STXX_SPI4_DAT(interface), 0x00000000);
    // MAX_T          [15: 0] (16b) = 0
    // ALPHA          [31:16] (16b) = 0

    // STX0 Calendar Table
    // -------------------------------------------------
    cvmx_write_csr(CVMX_STXX_SPI4_CALX(0, interface), 0x00013210);
    // PRT0           [ 4: 0] ( 5b) = 0
    // PRT1           [ 9: 5] ( 5b) = 1
    // PRT2           [14:10] ( 5b) = 2
    // PRT3           [19:15] ( 5b) = 3
    // ODDPAR         [20:20] ( 1b) = 1
    cvmx_write_csr(CVMX_STXX_SPI4_CALX(1, interface), 0x00017654);
    // PRT0           [ 4: 0] ( 5b) = 4
    // PRT1           [ 9: 5] ( 5b) = 5
    // PRT2           [14:10] ( 5b) = 6
    // PRT3           [19:15] ( 5b) = 7
    // ODDPAR         [20:20] ( 1b) = 1
    cvmx_write_csr(CVMX_STXX_SPI4_CALX(2, interface), 0x00000098);
    // PRT0           [ 4: 0] ( 5b) = 8
    // PRT1           [ 9: 5] ( 5b) = 9
    // PRT2           [14:10] ( 5b) = 0
    // PRT3           [19:15] ( 5b) = 0
    // ODDPAR         [20:20] ( 1b) = 0
    cvmx_write_csr(CVMX_STXX_SPI4_STAT(interface), 0x0000010a);
    // LEN            [ 7: 0] ( 8b) = a
    // M              [15: 8] ( 8b) = 1

    // Setup the IXF1010 Spi4 Interface here
    // -------------------------------------------------
    //mac wr 8701 0         // Disabled training to N2
    //mac wr 8701 208000        // (REP_T=32/DATA_MAX_T=0x008000)
    // MAX_BURSTs are fine for now
    // Watermarks are fine for now
    // CAL_M is fine at 1
    // IXF will send and sample on posedge clk

    // SRX0 & STX0 Inf0 Links are configured - begin training
    // -------------------------------------------------
    cvmx_write_csr(CVMX_SPXX_CLK_CTL(interface), 0x0000083b);
    // SRXDLCK        [ 0: 0] ( 1b) = 1
    // RCVTRN         [ 1: 1] ( 1b) = 1
    // DRPTRN         [ 2: 2] ( 1b) = 0
    // SNDTRN         [ 3: 3] ( 1b) = 1
    // STATRCV        [ 4: 4] ( 1b) = 1
    // STATDRV        [ 5: 5] ( 1b) = 1
    // RUNBIST        [ 6: 6] ( 1b) = 0
    // CLKDLY         [11: 7] ( 5b) = 10
    // SRXLCK         [12:12] ( 1b) = 0
    // STXLCK         [13:13] ( 1b) = 0
    // SEETRN         [14:14] ( 1b) = 0

    // Wait for 1010 training lock
    // -------------------------------------------------
    //mac rd 8702[12]=1     // (RX Calendar Register[12] = TX Spi4 Sync)
    do
    {
        result = cvmx_spi4000_read(0x702);
    } while (((result>>12) & 1) != 1);

    // STX0 has achieved sync
    // IFX1010 should be sending calendar data
    // Enable the STX0 STAT receiver
    // -------------------------------------------------
    cvmx_stxx_com_ctl_t stxx_com_ctl;
    stxx_com_ctl.u64 = 0;
    stxx_com_ctl.s.inf_en = 0;
    stxx_com_ctl.s.st_en = 1;
    cvmx_write_csr(CVMX_STXX_COM_CTL(interface), stxx_com_ctl.u64);

    // Waiting to sync on STX0 STAT
    // -------------------------------------------------
    //n2 rd 1350[10]=1      // SPX0_CLK_STAT - SPX0_CLK_STAT[STXCAL] should be 1 (bit10)

    cvmx_spxx_clk_stat_t stat;
    do
    {
        stat.u64 = cvmx_read_csr(CVMX_SPXX_CLK_STAT(interface));
    } while (stat.s.stxcal == 0);

    // STX0 is enabled
    // -------------------------------------------------
    stxx_com_ctl.u64 = 0;
    stxx_com_ctl.s.inf_en = 1;
    stxx_com_ctl.s.st_en = 1;
    cvmx_write_csr(CVMX_STXX_COM_CTL(interface), stxx_com_ctl.u64);

    // SRX0 begin auto-deskew
    // SRX0 auto-deskew lock
    // -------------------------------------------------
    cvmx_wait(1000 * MS);

    // SRX0 clear the boot bit
    // -------------------------------------------------
    spxx_trn4_ctl.s.clr_boot = 1;
    cvmx_write_csr(CVMX_SPXX_TRN4_CTL(interface), spxx_trn4_ctl.u64);

    // Wait for final training sequence to complete
    // -------------------------------------------------
    cvmx_wait(1000 * MS);

    // Lets get a good training cycle
    // -------------------------------------------------
    //n2 rd 1350[8]=1       // SPX0_CLK_STAT - SPX0_CLK_STAT[SRXTRN] should be 1 (bit8)
    do
    {
        stat.u64 = cvmx_read_csr(CVMX_SPXX_CLK_STAT(interface));
    } while (stat.s.srxtrn == 0);

    // SRX0 interface should be good, send calendar data
    // IFX1010 should now be sending IDLE cycles
    // -------------------------------------------------
    cvmx_srxx_com_ctl_t srxx_com_ctl;
    srxx_com_ctl.u64 = 0;
    srxx_com_ctl.s.prts = 9;
    srxx_com_ctl.s.inf_en = 1;
    srxx_com_ctl.s.st_en = 1;
    cvmx_write_csr(CVMX_SRXX_COM_CTL(interface), srxx_com_ctl.u64);

    // IFX1010 should see the calendar table and sync
    // -------------------------------------------------
    //mac rd 8702[13]=1     // (RX Calendar Register[13] = RX Spi4 Sync)
    do
    {
        result = cvmx_spi4000_read(0x702);
    } while (((result>>12) & 1) == 0);

    // Inf0 is synched
    // -------------------------------------------------
    // SPX0 is up
    return 0;
}

#endif  /* OCTEON_SPI4000_ENET */
