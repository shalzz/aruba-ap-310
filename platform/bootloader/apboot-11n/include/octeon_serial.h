/*************************************************************************
Copyright (c) 2003-2004 Cavium Networks (support@cavium.com). All rights
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
 * $Id: octeon_serial.h,v 1.5 2005/10/21 16:26:00 kreese Exp $
 *
 */

#ifndef __OCTEON_SERIAL_H__
#define __OCTEON_SERIAL_H__

#define  OCTEON_MIO_UARTX_RBR(offset)                 (0x8001180000000800ull+(offset*1024))
#define  OCTEON_MIO_UARTX_THR(offset)                 (0x8001180000000840ull+(offset*1024))
#define  OCTEON_MIO_UARTX_DLL(offset)                 (0x8001180000000880ull+(offset*1024))
#define  OCTEON_MIO_UARTX_IER(offset)                 (0x8001180000000808ull+(offset*1024))
#define  OCTEON_MIO_UARTX_DLH(offset)                 (0x8001180000000888ull+(offset*1024))
#define  OCTEON_MIO_UARTX_IIR(offset)                 (0x8001180000000810ull+(offset*1024))
#define  OCTEON_MIO_UARTX_FCR(offset)                 (0x8001180000000850ull+(offset*1024))
#define  OCTEON_MIO_UARTX_LCR(offset)                 (0x8001180000000818ull+(offset*1024))
#define  OCTEON_MIO_UARTX_MCR(offset)                 (0x8001180000000820ull+(offset*1024))
#define  OCTEON_MIO_UARTX_LSR(offset)                 (0x8001180000000828ull+(offset*1024))
#define  OCTEON_MIO_UARTX_MSR(offset)                 (0x8001180000000830ull+(offset*1024))
#define  OCTEON_MIO_UARTX_SCR(offset)                 (0x8001180000000838ull+(offset*1024))
#define  OCTEON_MIO_UARTX_SRR(offset)                 (0x8001180000000A10ull+(offset*1024))

#define OCTEON_UART_NUM_PORTS     2
#define OCTEON_UART_TX_FIFO_SIZE  16
#define OCTEON_UART_RX_FIFO_SIZE  16

#endif /* __OCTEON_SERIAL_H__ */
