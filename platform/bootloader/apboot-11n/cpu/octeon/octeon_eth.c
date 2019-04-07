#define DEBUG 1
// vim:set ts=4 sw=4 expandtab:
/*************************************************************************
Copyright (c) 2005 Cavium Networks (support@cavium.com). All rights
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
 *
 * $Id: octeon_eth.c,v 1.78 2007/01/02 23:55:42 rfranz Exp $
 * 
 */
 
 
#include <common.h>
#include <command.h>
#include <exports.h>
#include <linux/ctype.h>
#include <lib_octeon.h>
#include <octeon_eeprom_types.h>
#include <net.h>
#include <miiphy.h>
#include "octeon_boot.h"
#include "octeon_eeprom_types.h"
#include "lib_octeon_shared.h"
#include "cvmx-config.h"


#ifdef DEBUG
#define dprintf if (enet_verbose()) printf
#else
#define dprintf(...)
#endif

static inline int enet_verbose(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    return (!!(gd->flags & GD_FLG_ENET_VERBOSE));
}

/* Global flag indicating common hw has been set up */
int octeon_global_hw_inited = 0;

/* pointer to device to use for loopback.  Only needed for IPD reset workaround */
struct eth_device *loopback_dev;

int octeon_eth_send(struct eth_device *dev, volatile void *packet, int length);
int octeon_ebt3000_rgmii_init(struct eth_device *dev, bd_t * bis, int);
int octeon_ebt3000_rgmii_init_wrap(struct eth_device *dev, bd_t * bis);


#ifdef OCTEON_INTERNAL_ENET
/************************************************************************/
typedef enum
{
    PORT_RGMII = 1,
    PORT_GMII,
    PORT_MII,
    PORT_SPI4000
} port_types;
/* Ethernet device private data structure for octeon ethernet */
typedef struct
{
    uint32_t port;
    uint32_t interface;
    uint32_t queue;
    uint32_t port_type;
    uint32_t initted_flag;
    uint32_t link_speed:2;   /* current link status, use to reconfigure on status changes */
    uint32_t link_duplex:1;
    uint32_t link_status:1;
    uint32_t loopback:1;
} octeon_eth_info_t;

#define USE_HW_TCPUDP_CHECKSUM  0

/* Make sure that we have enough buffers to keep prefetching blocks happy.
** Absolute minimum is probably about 200. */
#define NUM_PACKET_BUFFERS  100

#define PKO_SHUTDOWN_TIMEOUT_VAL     10

int  octeon_miiphy_read(char *p, unsigned char addr, unsigned char reg, unsigned short * value);
int octeon_miiphy_write(char *p, unsigned char  addr, unsigned char  reg, unsigned short value);

/****************************************************************/
/**
 * Free the packet buffers contained in a work queue entry.
 * The work queue entry is not freed.
 *
 * @param work   Work queue entry with packet to free
 */
void cvmx_helper_free_packet_data(cvmx_wqe_t *work)
{
    int64_t         remaining_bytes;
    cvmx_buf_ptr_t  buffer_ptr;
    uint64_t        start_of_buffer;

    buffer_ptr = work->packet_ptr;
    remaining_bytes = work->len;

    while (remaining_bytes > 0)
    {
        /* Remember the back pointer is in cache lines, not 64bit words */
        start_of_buffer = ((buffer_ptr.s.addr >> 7) - buffer_ptr.s.back) << 7;
        remaining_bytes -= buffer_ptr.s.size;
        buffer_ptr = *(cvmx_buf_ptr_t*)cvmx_phys_to_ptr(buffer_ptr.s.addr - 8);
        cvmx_fpa_free(cvmx_phys_to_ptr(start_of_buffer), CVMX_FPA_PACKET_POOL, 0);
    }
}

/****************************************************************/
void cvmx_pko_enable(void)
{
    cvmx_write_csr(CVMX_PKO_REG_FLAGS, 3);
}

static uint8_t cvmx_pko_queue_map[CVMX_PKO_MAX_OUTPUT_QUEUES];  /* Used to track queue usage for shutdown */
/**
 * Configure a output port and the associated queues for use.
 *
 * @param port       Port to configure.
 * @param base_queue First queue number to associate with this port.
 * @param num_queues Number of queues t oassociate with this port
 * @param priority   Array of priority levels for each queue. Values are
 *                   allowed to be 0-8. A value of 8 get 8 times the traffic
 *                   of a value of 1.  A value of 0 indicates that no rounds
 *                   will be participated in. These priorities can be changed
 *                   on the fly while the pko is enabled. A priority of 9
 *                   indicates that static priority should be used.  If static
 *                   priority is used all queues with static priority must be
 *                   contiguous starting at the base_queue, and lower numbered
 *                   queues have higher priority than higher numbered queues.
 *                   There must be num_queues elements in the array.
 */
cvmx_pko_status_t uboot_cvmx_pko_config_port(uint32_t port, uint32_t base_queue, uint32_t num_queues, const uint32_t priority[])
{
    cvmx_pko_status_t   result_code;
    uint64_t            queue;
    cvmx_pko_queue_cfg_t config;

    if ((port >= CVMX_PKO_NUM_OUTPUT_PORTS) && (port != CVMX_PKO_MEM_QUEUE_PTRS_ILLEGAL_PID))
    {
        printf("ERROR: cvmx_pko_config_port: Invalid port %llu\n", (unsigned long long)port);
        return CVMX_PKO_INVALID_PORT;
    }

    if (base_queue + num_queues > CVMX_PKO_MAX_OUTPUT_QUEUES)
    {
        printf("ERROR: cvmx_pko_config_port: Invalid queue range\n");
        return CVMX_PKO_INVALID_QUEUE;
    }

    dprintf("cvmx_pko_config_port(): port: %d, bq: %d, nq: %d\n", port, base_queue, num_queues);
    /* At this point, static_priority_base and static_priority_end are either both -1,
    ** or are valid start/end queue numbers */

    result_code = CVMX_PKO_SUCCESS;

    for (queue = 0; queue < num_queues; queue++)
    {
        uint64_t  *buf_ptr = NULL;
        config.u64          = 0;
        config.s.tail       = queue == (num_queues - 1);
        config.s.index      = queue;
        config.s.port       = port;
        config.s.queue      = base_queue + queue;

        /* Convert the priority into an enable bit field. Try to space the bits
            out evenly so the packet don't get grouped up */
        switch (priority[queue])
        {
            case 1: config.s.qos_mask = 0x01; break;
            case 2: config.s.qos_mask = 0x11; break;
            case 3: config.s.qos_mask = 0x49; break;
            case 4: config.s.qos_mask = 0x55; break;
            case 5: config.s.qos_mask = 0x57; break;
            case 6: config.s.qos_mask = 0x77; break;
            case 7: config.s.qos_mask = 0x7f; break;
            case 8: config.s.qos_mask = 0xff; break;
            default:
                printf("ERROR: cvmx_pko_config_port: Invalid priority %llu\n", (unsigned long long)priority[queue]);
                config.s.qos_mask = 0xff;
                result_code = CVMX_PKO_INVALID_PRIORITY;
                break;
        }

        if (port != CVMX_PKO_MEM_QUEUE_PTRS_ILLEGAL_PID)
        {
            buf_ptr = (uint64_t*)cvmx_fpa_alloc(CVMX_FPA_OUTPUT_BUFFER_POOL);
            if (!buf_ptr)
            {
                printf("ERROR: cvmx_pko_config_port: Unable to allocate output buffer.\n");
                return(CVMX_PKO_NO_MEMORY);
            }

            /* Set initial command buffer address and index in FAU register for queue */
            cvmx_fau_atomic_write64((cvmx_fau_reg_64_t)(CVMX_FAU_REG_OQ_ADDR_INDEX + 8 * (base_queue + queue)), CAST64(buf_ptr) << CVMX_PKO_INDEX_BITS);
            cvmx_pko_queue_map[base_queue + queue] = 1;

        }
        config.s.buf_ptr = cvmx_ptr_to_phys(buf_ptr);
        CVMX_SYNCW;
        cvmx_write_csr(CVMX_PKO_MEM_QUEUE_PTRS, config.u64);
    }

    return result_code;
}



static void cvm_oct_fill_hw_memory(uint64_t pool, uint64_t size, uint64_t elements)
{

    uint64_t memory;
    static int alloc_count = 0;

    char tmp_name[64];
    sprintf(tmp_name, "%s_fpa_alloc_%d", OCTEON_BOOTLOADER_NAMED_BLOCK_TMP_PREFIX, alloc_count++);
    memory = octeon_phy_mem_named_block_alloc(glob_phy_desc_ptr, size * elements, 0, 0x40000000, 128, tmp_name);
    if (!memory)
    {
        printf("Unable to allocate %lu bytes for FPA pool %ld\n", elements*size, pool);
        return;
    }
    while (elements--)
    {
        cvmx_fpa_free((void *)(uint32_t)(memory + elements*size), pool, 0);
    }
}

#ifdef CONFIG_MOSCATO
/**
 * @INTERNAL
 * Reset the packet output.
 */
static void __cvmx_pko_reset(void)
{
    cvmx_pko_reg_flags_t pko_reg_flags;
    pko_reg_flags.u64 = cvmx_read_csr(CVMX_PKO_REG_FLAGS);
    pko_reg_flags.s.reset = 1;
    cvmx_write_csr(CVMX_PKO_REG_FLAGS, pko_reg_flags.u64);
}
#endif

/**
 * Configure common hardware for all interfaces
 */
static void cvm_oct_configure_common_hw(void)
{

    /* Setup the FPA */
    cvmx_fpa_enable();

    cvm_oct_fill_hw_memory(CVMX_FPA_WQE_POOL, CVMX_FPA_WQE_POOL_SIZE, NUM_PACKET_BUFFERS);
#if CVMX_FPA_OUTPUT_BUFFER_POOL != CVMX_FPA_PACKET_POOL
    cvm_oct_fill_hw_memory(CVMX_FPA_OUTPUT_BUFFER_POOL, CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE, 128);
#endif

    cvm_oct_fill_hw_memory(CVMX_FPA_PACKET_POOL, CVMX_FPA_PACKET_POOL_SIZE, NUM_PACKET_BUFFERS);

    cvmx_ipd_config(CVMX_FPA_PACKET_POOL_SIZE/8,
                    CVMX_HELPER_FIRST_MBUFF_SKIP/8,
                    CVMX_HELPER_NOT_FIRST_MBUFF_SKIP/8,
                    CVMX_HELPER_FIRST_MBUFF_SKIP / 128,
                    CVMX_HELPER_NOT_FIRST_MBUFF_SKIP / 128,
                    CVMX_FPA_WQE_POOL,
                    CVMX_IPD_OPC_MODE_STF,
                    CVMX_HELPER_ENABLE_BACK_PRESSURE);

#ifdef CONFIG_MOSCATO
    // SCA; this PKO thing is killing me
    __cvmx_pko_reset();
#endif
    /* Set the PKO to think command buffers are an odd length. This makes
        it so we never have to divide a comamnd across two buffers */
    cvmx_pko_pool_cfg_t config;
    config.u64 = 0;
    config.s.pool = CVMX_FPA_OUTPUT_BUFFER_POOL;
#if USE_OPTIMIZED_PKO_BUFFER_SIZE
    config.s.size = CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE / 8 - 1;
#else
    config.s.size = CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE / 8;
#endif
    cvmx_write_csr(CVMX_PKO_REG_CMD_BUF, config.u64);

}











/**
 * Enqueue a packet into the PKO output queue
 *
 * @param dev    Linux device to enqueue for
 * @param pko_command
 *               PKO command encoding of the packet
 * @param packet The packet data
 * @return Zero on success
 */
static inline int cvm_oct_low_level_send(struct eth_device *dev, cvmx_pko_command_word0_t pko_command,
                                         cvmx_buf_ptr_t packet)
{

    octeon_eth_info_t* priv = (octeon_eth_info_t*)dev->priv;
    /* Get the queue command ptr location from the FAU */
    CVMX_SYNCIOBDMA;
    uint64_t tx_command_state = cvmx_scratch_read64(CVMX_SCR_SCRATCH);
    uint64_t *tx_command_ptr = cvmx_phys_to_ptr(tx_command_state>>CVMX_PKO_INDEX_BITS);
    uint64_t tx_command_index = tx_command_state & CVMX_PKO_INDEX_MASK;
    tx_command_ptr += tx_command_index;

    dprintf("cvm_oct_low_level_send: command: 0x%Lx, buf_ptr: 0x%Lx, oq_buf: %p, oq_ind: 0x%Lx\n",
           pko_command, packet, tx_command_ptr, tx_command_index);

    /* Check if we are at the end of the buffer and need to chain the next one */
#if USE_OPTIMIZED_PKO_BUFFER_SIZE
    if ((tx_command_index < CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE/8 - 4))
#else
    if ((tx_command_index < CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE/8 - 3))
#endif
    {
        /* No buffer needed. Output the command and go */
        tx_command_ptr[0] = pko_command.u64;
        tx_command_ptr[1] = packet.u64;
    }
    else
    {
        /* We need a new output buffer. At this point we have either 2 or 3
            words left. The first word will always be the pko command */
        tx_command_ptr[0] = pko_command.u64;

        /* Get a buffer for the new command buffer. We don't use scratch in
            case some user app is doing something. It's possible the user app
            is using the scrach when the interrupt happens. Reading scratch and
            fetching the next buffer isn't atomic */
        uint64_t *newbuf = cvmx_fpa_alloc(CVMX_FPA_OUTPUT_BUFFER_POOL);
        if ((!newbuf))
            return -1;

#if USE_OPTIMIZED_PKO_BUFFER_SIZE == 0
        /* If we only have two words left we have to split the command over the
            command buffers */
        if (CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE/8 - tx_command_index == 2)
        {
            tx_command_ptr[1] = cvmx_ptr_to_phys(newbuf);
            *newbuf = packet.u64;
            cvmx_fau_atomic_write64(CVMX_FAU_REG_OQ_ADDR_INDEX + priv->queue*8, (tx_command_ptr[1]<<CVMX_PKO_INDEX_BITS) + 1);
        }
        else
#endif
        {
            tx_command_ptr[1] = packet.u64;
            tx_command_ptr[2] = cvmx_ptr_to_phys(newbuf);
            cvmx_fau_atomic_write64(CVMX_FAU_REG_OQ_ADDR_INDEX + priv->queue*8, tx_command_ptr[2]<<CVMX_PKO_INDEX_BITS);
        }
    }

    cvmx_pko_doorbell(priv->port, priv->queue, 2);

#if 0
    {
        cvmx_pko_mem_debug9_t pko_dbg9;
        cvmx_pko_reg_read_idx_t pko_idx;

        pko_idx.u64 = 0;
        pko_idx.s.index = priv->queue;
        cvmx_write_csr(CVMX_PKO_REG_READ_IDX, pko_idx.u64);
        pko_dbg9.u64 = cvmx_read_csr(CVMX_PKO_MEM_DEBUG9);
        cvmx_write_csr(CVMX_PKO_REG_READ_IDX, pko_idx.u64);
        uint32_t count = (uint32_t)cvmx_read_csr(CVMX_PKO_MEM_COUNT0);
        cvmx_write_csr(CVMX_PKO_REG_READ_IDX, pko_idx.u64);
        uint64_t count1 = cvmx_read_csr(CVMX_PKO_MEM_COUNT1);

        printf("Doorbell count for queue %d is %d, cnt0:%d, cnt1: 0x%Lx\n", priv->queue, pko_dbg9.s.doorbell, count, count1);
    }
#endif
    return 0;
}



/**
 * Packet transmit
 *
 * @param skb    Packet to send
 * @param dev    Device info structure
 * @return Always returns zero
 */
static int cvm_oct_xmit(struct eth_device *dev, void *packet, int len)
{
    octeon_eth_info_t* priv = (octeon_eth_info_t*)dev->priv;
    cvmx_pko_command_word0_t    pko_command;
    cvmx_buf_ptr_t              hw_buffer;


    dprintf("cvm_oct_xmit addr: %p, len: %d\n", packet, len);


    /* Build the PKO buffer pointer */
    hw_buffer.u64 = 0;
    hw_buffer.s.addr = cvmx_ptr_to_phys(packet);
    hw_buffer.s.pool = CVMX_FPA_PACKET_POOL;
    hw_buffer.s.size = CVMX_FPA_PACKET_POOL_SIZE;

    /* Build the PKO command */
    pko_command.u64 = 0;
    pko_command.s.subone0 = 1;
    pko_command.s.dontfree = 0;
    pko_command.s.segs = 1;
    pko_command.s.total_bytes = len;



    /* Get the position for writing the PKO command. This will be used when we
        call cvm_oct_low_level_send. If we drop, this must be decremented */
    cvmx_fau_async_fetch_and_add64(CVMX_SCR_SCRATCH, CVMX_FAU_REG_OQ_ADDR_INDEX + priv->queue*8, 2);

    /* Send the packet to the output queue */
    if ((cvm_oct_low_level_send(dev, pko_command, hw_buffer)))
    {
        printf("Failed to send the packet\n");
    }
	return 0;
}

/**
 * cvmx_ipd_sub_port_fcs
 *
 * IPD_SUB_PORT_FCS = IPD Subtract Ports FCS Register
 * 
 * When set '1' the port corresponding to the but set will subtract 4 bytes from the end of
 * the packet.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_ipd_sub_port_fcs_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t port_bit                : 32;      /**< When set '1', the port corresponding to the bit
                                                         position set, will subtract the FCS for packets
                                                         on that port. */
#else
        uint64_t port_bit                : 32;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_ipd_sub_port_fcs_cn3020
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_3_63           : 61;
        uint64_t port_bit                : 3;       /**< When set '1', the port cooresponding to the bit
                                                         position set, will subtract the FCS for packets
                                                         on that port. */
#else
        uint64_t port_bit                : 3;
        uint64_t reserved_3_63           : 61;
#endif
    } cn3020;
    struct cvmx_ipd_sub_port_fcs_cn3020  cn30xx;
    struct cvmx_ipd_sub_port_fcs_cn3020  cn31xx;
    struct cvmx_ipd_sub_port_fcs_s       cn36xx;
    struct cvmx_ipd_sub_port_fcs_s       cn38xx;
    struct cvmx_ipd_sub_port_fcs_s       cn38xxp2;
    struct cvmx_ipd_sub_port_fcs_cn3020  cn50xx;
    struct cvmx_ipd_sub_port_fcs_s       cn56xx;
    struct cvmx_ipd_sub_port_fcs_s       cn58xx;
} cvmx_ipd_sub_port_fcs_t;

#define  CVMX_IPD_SUB_PORT_FCS                                CVMX_ADD_IO_SEG(0x00014F0000000170ull)

static void
cvm_oct_check_for_preamble_workaround(struct eth_device *dev, octeon_eth_info_t *priv)
{
    int interface = priv->interface;
    int index = priv->port - 16 * interface;
    cvmx_gmxx_rxx_int_reg_t gmxx_rxx_int_reg;
    gmxx_rxx_int_reg.u64 = cvmx_read_csr(CVMX_GMXX_RXX_INT_REG(index, interface));

    if (priv->link_speed == 2) { // 1000
        // don't use workaround in 1000baseT mode
        return;
    }
    if (!octeon_is_model(OCTEON_CN30XX)) {
        // don't use workaround on 5010
        return;
    }

    /* Read the GMXX_RXX_INT_REG[PCTERR] bit and see if we are getting
        preamble errors */
    if (gmxx_rxx_int_reg.s.pcterr)
    {
        /* We are getting preamble errors at 10Mbps. Most likely the
            PHY is giving us packets with mis aligned preambles. In
            order to get these packets we need to disable preamble
            checking and do it in software */
        cvmx_gmxx_rxx_frm_ctl_t gmxx_rxx_frm_ctl;
        cvmx_ipd_sub_port_fcs_t ipd_sub_port_fcs;

        /* Disable preamble checking */
        gmxx_rxx_frm_ctl.u64 = cvmx_read_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface));
        gmxx_rxx_frm_ctl.s.pre_chk = 0;
        cvmx_write_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface), gmxx_rxx_frm_ctl.u64);
        /* Disable FCS stripping */
        ipd_sub_port_fcs.u64 = cvmx_read_csr(CVMX_IPD_SUB_PORT_FCS);
        ipd_sub_port_fcs.s.port_bit &= 0xffffffffull ^ (1ull<<priv->port);
        cvmx_write_csr(CVMX_IPD_SUB_PORT_FCS, ipd_sub_port_fcs.u64);
        /* Clear any error bits */
        cvmx_write_csr(CVMX_GMXX_RXX_INT_REG(index, interface), gmxx_rxx_int_reg.u64);
        printf("%s: Using software preamble removal\n", dev->name);
    }
}

static void
cvm_oct_restore_default_preamble_behavior(octeon_eth_info_t *priv)
{
    cvmx_gmxx_rxx_frm_ctl_t gmxx_rxx_frm_ctl;
    cvmx_ipd_sub_port_fcs_t ipd_sub_port_fcs;
    cvmx_gmxx_rxx_int_reg_t gmxx_rxx_int_reg;
    int interface = priv->interface;
    int index = priv->port - 16 * interface;

    /* Enable preamble checking */
    gmxx_rxx_frm_ctl.u64 = cvmx_read_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface));
    gmxx_rxx_frm_ctl.s.pre_chk = 1;
    cvmx_write_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface), gmxx_rxx_frm_ctl.u64);
    /* Enable FCS stripping */
    ipd_sub_port_fcs.u64 = cvmx_read_csr(CVMX_IPD_SUB_PORT_FCS);
    ipd_sub_port_fcs.s.port_bit |= 1ull<<priv->port;
    cvmx_write_csr(CVMX_IPD_SUB_PORT_FCS, ipd_sub_port_fcs.u64);
    /* Clear any error bits */
    gmxx_rxx_int_reg.u64 = cvmx_read_csr(CVMX_GMXX_RXX_INT_REG(index, interface));
    cvmx_write_csr(CVMX_GMXX_RXX_INT_REG(index, interface), gmxx_rxx_int_reg.u64);
}

/**
 * Configure the RGMII port for the negotiated speed
 *
 * @param dev    Linux device for the RGMII port
 */
static void cvm_oct_configure_rgmii_speed(struct eth_device *dev, int pmsg)
{
    const char *link_duplex;
    const char *link_speed;
    const char *link_status;
    octeon_eth_info_t* priv = (octeon_eth_info_t*)dev->priv;
    int interface = priv->interface;
    int index = priv->port - 16 * interface;

    if (priv->loopback)
        return;

    cvmx_gmxx_rxx_rx_inbnd_t link;
    link.u64 = cvmx_read_csr(CVMX_GMXX_RXX_RX_INBND(index, interface));

dprintf("interface %u, port %u, index %u\n", priv->interface, priv->port, index);
//dprintf("link status reg %Lx\n", CVMX_GMXX_RXX_RX_INBND(index, interface));
dprintf("priv->port_type = %x, link.s.speed = %x, link.s.duplex = %x, link.s.status = %x\n",priv->port_type, link.s.speed, link.s.duplex, link.s.status);

#if CONFIG_OCTEON_TRANTOR
    /* Inband status does not seem to work */
    printf("Forcing link speed to 1000Mbps\n");
    link.s.speed = 2;
    link.s.duplex = 1;
#endif

#ifndef CONFIG_ARUBA_OCTEON
    if (priv->port_type == PORT_GMII)
    {
        /* GMII only supports full duplex gigabit */
        link.s.speed = 2;
        link.s.duplex = 1;
        link.s.status = 1;
    }
#endif
#if CONFIG_OCTEON_CN3005_EVB_HS5
    /* Port 1 is connected to switch, so force to gigabit full duplex */
    if (index == 1)
    {
        link.s.speed = 2;
        link.s.duplex = 1;
        link.s.status = 1;
    }
#endif
#if CONFIG_MOSCATO
    /* Port 1 is connected to switch, so force to 100 Mb/s full duplex */
    if (index == 1)
    {
        link.s.speed = 1;   // 100
        link.s.duplex = 1;
        link.s.status = 1;
    }
#endif
#if CONFIG_OCTEON_BBGW_REF
    /* Ports 0 and 1 are connected to switch, so force to gigabit full duplex */
    if (index == 0 || index == 1)
    {
        link.s.speed = 2;
        link.s.duplex = 1;
        link.s.status = 1;
    }
#endif

dprintf("#2 priv->port_type = %x, link.s.speed = %x, link.s.duplex = %x, link.s.status = %x\n",priv->port_type, link.s.speed, link.s.duplex, link.s.status);

    /* Check to see link config has changed, and reconfigure if required */
    if (priv->initted_flag)
    {
        if(link.s.speed != priv->link_speed ||
            link.s.status != priv->link_status ||
            link.s.duplex != priv->link_duplex)

        {
            if (pmsg) {
                printf("%s: ", dev->name);
            }
            cvm_oct_restore_default_preamble_behavior(priv);
        }
        else
        {
            cvm_oct_check_for_preamble_workaround(dev, priv);
            return;
        }
    }
    priv->link_speed =   link.s.speed; 
    priv->link_status =  link.s.status;
    priv->link_duplex =  link.s.duplex;

    cvmx_gmxx_prtx_cfg_t gmx_cfg;
    gmx_cfg.u64 = cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));
dprintf("gmx cfg reg %Lx\n", CVMX_GMXX_PRTX_CFG(index, interface));
dprintf("gmx cfg val = %Lx\n", gmx_cfg.u64);

	cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);
	// re-read register per 13.4.2 of HRM v1.0
	(void)cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));
	//
	// 13.4.2; max_packet_time = max packet size in bits / speed
	// we use worst case of 1600 bytes / 10 Mb/s to avoid thinking about it;
	// this gives 1280 usec, which we round up to 2000
	//
	udelay(2000);

#ifndef CONFIG_ARUBA_OCTEON
    gmx_cfg.s.duplex = 1; // link.s.duplex; /* Half duplex is broken right now */
#else
    gmx_cfg.s.duplex = link.s.duplex; // SCA; need to test this
#endif
//    gmx_cfg.s.en = 1;
    if (link.s.duplex)
        link_duplex = "Full";
    else
        link_duplex = "Half";

    if (link.s.speed == 0)
    {
        link_speed = " 10Mbs";
        gmx_cfg.s.slottime = 0;
        gmx_cfg.s.speed = 0;
        cvmx_write_csr(CVMX_GMXX_TXX_CLK(index, interface), 50);
        cvmx_write_csr(CVMX_GMXX_TXX_SLOT(index, interface), 0x40);
        cvmx_write_csr(CVMX_GMXX_TXX_BURST(index, interface), 0);
    }
    else if (link.s.speed == 1)
    {
        link_speed = "100Mbs";
        gmx_cfg.s.slottime = 0;
        gmx_cfg.s.speed = 0;
        cvmx_write_csr(CVMX_GMXX_TXX_CLK(index, interface), 5);
        cvmx_write_csr(CVMX_GMXX_TXX_SLOT(index, interface), 0x40);
        cvmx_write_csr(CVMX_GMXX_TXX_BURST(index, interface), 0);
    }
    else if (link.s.speed == 2)
    {
        link_speed = "  1Gbs";
        gmx_cfg.s.slottime = 1;
        gmx_cfg.s.speed = 1;
        cvmx_write_csr(CVMX_GMXX_TXX_CLK(index, interface), 1);
        cvmx_write_csr(CVMX_GMXX_TXX_SLOT(index, interface), 0x200);
        cvmx_write_csr(CVMX_GMXX_TXX_BURST(index, interface), 0x2000);
    }
    else
    {
        link_speed = " Rsrvd";
        gmx_cfg.s.slottime = 1;
        gmx_cfg.s.speed = 1;
        cvmx_write_csr(CVMX_GMXX_TXX_CLK(index, interface), 1);
        cvmx_write_csr(CVMX_GMXX_TXX_SLOT(index, interface), 0x200);
        cvmx_write_csr(CVMX_GMXX_TXX_BURST(index, interface), 0x2000);
    }

dprintf("final clk cfg val = %Lx %Lx\n", CVMX_GMXX_TXX_CLK(index, interface), 
cvmx_read_csr(CVMX_GMXX_TXX_CLK(index, interface)));
dprintf("final slot cfg val = %Lx %Lx\n", CVMX_GMXX_TXX_SLOT(index, interface), 
cvmx_read_csr(CVMX_GMXX_TXX_SLOT(index, interface)));
dprintf("final burst cfg val = %Lx %Lx\n", CVMX_GMXX_TXX_BURST(index, interface), 
cvmx_read_csr(CVMX_GMXX_TXX_BURST(index, interface)));

    if (link.s.status)
        link_status = "Up  ";
    else
        link_status = "Down";

	gmx_cfg.s.en = 0;
    cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);
	(void)cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));
dprintf("updated gmx cfg val = %Lx\n", gmx_cfg.u64);
	gmx_cfg.s.en = 1;
	cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);
dprintf("final gmx cfg val = %Lx\n", gmx_cfg.u64);

    if (pmsg) {
        printf("%s %s %s duplex, (port %2d)\n",
               link_status, link_speed, link_duplex, 16*interface + index);
    }
}


/* Not really general purpose - just used for a workaround in the network
** hardware shutdown routine.
*/
static void cvm_oct_configure_rgmii_loopback(struct eth_device *dev)
{
    octeon_eth_info_t* priv = (octeon_eth_info_t*)dev->priv;
    int interface = priv->interface;
    int index = priv->port - 16 * interface;

    /* force speed to gigabit full duplex */
    priv->link_speed =   2; 
    priv->link_status =  1;
    priv->link_duplex =  1;
    priv->loopback = 1;

    cvmx_gmxx_prtx_cfg_t gmx_cfg;
    gmx_cfg.u64 = cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));

    gmx_cfg.s.duplex = 1; // link.s.duplex; /* Half duplex is broken right now */
    gmx_cfg.s.en = 1;
    gmx_cfg.s.slottime = 1;
    gmx_cfg.s.speed = 1;
    cvmx_write_csr(CVMX_GMXX_TXX_CLK(index, interface), 1);
    cvmx_write_csr(CVMX_GMXX_TXX_SLOT(index, interface), 0x200);
    cvmx_write_csr(CVMX_GMXX_TXX_BURST(index, interface), 0x2000);

    cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);

    /* Enable loop, rx and tx (rx/tx need to be enabled for loopback mode */
    /* Port must be set to gigabit full duplex for loopback mode to work.
    ** Make sure we only enable the port we want.  */
    cvmx_write_csr(CVMX_ASXX_PRT_LOOP(interface), 0x1 << index);
    cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(interface), 0x1<< index);
    cvmx_write_csr(CVMX_ASXX_TX_PRT_EN(interface), 0x1<< index);
    cvmx_read64(CVMX_MIO_BOOT_BIST_STAT);  /* Make sure writes are complete */

}

/**
 * Perform per port configuration.
 *
 * @param dev    Device to configure
 */
static void cvm_oct_configure_port(struct eth_device *dev)
{
    octeon_eth_info_t* priv = (octeon_eth_info_t*)dev->priv;

    /* Packet output configures Queue and Ports */
    const uint32_t priorities[8] = {8,8,8,8,8,8,8,8};
    dprintf("cvm_oct_configure_port(); port: %d, q: %d\n", priv->port, priv->queue);
    uboot_cvmx_pko_config_port(priv->port, priv->queue,
                         1, priorities);

    /* Setup Port input tagging */
    cvmx_pip_port_cfg_t     port_config;
    cvmx_pip_port_tag_cfg_t tag_config;

    port_config.u64 = 0;
    port_config.s.mode = CVMX_PIP_PORT_CFG_MODE_SKIPL2; /* Process the headers and place the IP header in the work queue */
    port_config.s.qos = priv->port & 0x7;
    port_config.s.crc_en = 1;

    tag_config.u64 = 0;
    tag_config.s.ip6_src_flag  = CVMX_HELPER_INPUT_TAG_IPV6_SRC_IP;
    tag_config.s.ip6_dst_flag  = CVMX_HELPER_INPUT_TAG_IPV6_DST_IP;
    tag_config.s.ip6_sprt_flag = CVMX_HELPER_INPUT_TAG_IPV6_SRC_PORT;
    tag_config.s.ip6_dprt_flag = CVMX_HELPER_INPUT_TAG_IPV6_DST_PORT;
    tag_config.s.ip6_nxth_flag = CVMX_HELPER_INPUT_TAG_IPV6_NEXT_HEADER;
    tag_config.s.ip4_src_flag  = CVMX_HELPER_INPUT_TAG_IPV4_SRC_IP;
    tag_config.s.ip4_dst_flag  = CVMX_HELPER_INPUT_TAG_IPV4_DST_IP;
    tag_config.s.ip4_sprt_flag = CVMX_HELPER_INPUT_TAG_IPV4_SRC_PORT;
    tag_config.s.ip4_dprt_flag = CVMX_HELPER_INPUT_TAG_IPV4_DST_PORT;
    tag_config.s.ip4_pctl_flag = CVMX_HELPER_INPUT_TAG_IPV4_PROTOCOL;
    tag_config.s.inc_prt_flag  = CVMX_HELPER_INPUT_TAG_INPUT_PORT;
    tag_config.s.tcp6_tag_type = CVMX_HELPER_INPUT_TAG_TYPE;
    tag_config.s.tcp4_tag_type = CVMX_HELPER_INPUT_TAG_TYPE;
    tag_config.s.ip6_tag_type = CVMX_HELPER_INPUT_TAG_TYPE;
    tag_config.s.ip4_tag_type = CVMX_HELPER_INPUT_TAG_TYPE;
    tag_config.s.non_tag_type = CVMX_HELPER_INPUT_TAG_TYPE;
    tag_config.s.grp = 0;

    cvmx_pip_config_port(priv->port, port_config, tag_config);
}

static uint32_t octeon_get_pko_doorbell_count(uint32_t queue)
{
    cvmx_pko_mem_debug9_t pko_dbg9;
    cvmx_pko_mem_debug8_t pko_dbg8;
    cvmx_pko_reg_read_idx_t pko_idx;

    pko_idx.u64 = 0;
    pko_idx.s.index = queue;
    if (octeon_is_model(OCTEON_CN58XX) || octeon_is_model(OCTEON_CN50XX))
    {

        cvmx_write_csr(CVMX_PKO_REG_READ_IDX, pko_idx.u64);
        pko_dbg8.u64 = cvmx_read_csr(CVMX_PKO_MEM_DEBUG8);
        return(pko_dbg8.cn58xx.doorbell);
    }
    else
    {
        cvmx_write_csr(CVMX_PKO_REG_READ_IDX, pko_idx.u64);
        pko_dbg9.u64 = cvmx_read_csr(CVMX_PKO_MEM_DEBUG9);
        return(pko_dbg9.cn38xx.doorbell);
    }

}



int octeon_network_hw_shutdown_internal(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    int port, queue;
    int retval = 0;
    cvmx_gmxx_inf_mode_t mode;
    int i;
    int num_interfaces;
    int ports_per_interface;


    if (octeon_is_model(OCTEON_CN38XX))
    {
        num_interfaces = 2;      
        ports_per_interface = 10;  /* Max supported in bootloader, spi-4000 */
    }
    else
    {
        num_interfaces = 1;      
        ports_per_interface = 3;
    }

    octeon_eth_info_t *oct_eth_info;

    /* Don't need to do anything if networking not used */
    if (!octeon_global_hw_inited)
        return(retval);

    if (octeon_get_proc_id() == OCTEON_CN38XX_PASS1)
    {
        printf("\n");
        printf("\n");
        printf("ERROR: TFTP network reset not supported on PASS 1\n");
        printf("\n");
        return -1;
    }

    /* For pass 2 we need to make sure the device that we selected to loop back packets
    ** on has been initialized */
    if (octeon_get_proc_id() == OCTEON_CN38XX_PASS2)
    {
        if (!loopback_dev)
        {
            printf("Neither interface is RGMII, unable to reset network HW\n");
            return -1;
        }
        oct_eth_info = (octeon_eth_info_t *)loopback_dev->priv;

        dprintf("using port %d to loopback packets for ipd workaround\n", oct_eth_info->port);

        if (!oct_eth_info->initted_flag)
        {
            octeon_ebt3000_rgmii_init(loopback_dev, gd->bd, 1);
        }
    }


    /* Disable any further packet reception */
    for (i = 0; i < num_interfaces; i++)
    {
        mode.u64 = cvmx_read_csr(CVMX_GMXX_INF_MODE(i));
        if (mode.s.en
            && !((octeon_is_model(OCTEON_CN38XX) || octeon_is_model(OCTEON_CN58XX)) && mode.s.type == 1))
        {
            cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(i), 0x0);   /* Disable the RGMII RX ports */
            cvmx_write_csr(CVMX_ASXX_TX_PRT_EN(i), 0x0);   /* Disable the RGMII TX ports */
        }
    }
    /* TODO: If SPI interface is used, the SPI device should be configured to stop receiving packets*/


    /* Make sure PKO is empty.  Read doorbell count
    ** for configured output queues.  We at most have configured
    ** ports 0-9, 16-25 */
    int timeout;
    uint32_t count;
    for (i = 0; i < num_interfaces; i++)
    {

        for (port = i*16; port < i*16 + ports_per_interface; port++)
        {
            queue = cvmx_pko_get_base_queue(port);
            timeout = PKO_SHUTDOWN_TIMEOUT_VAL;
            /* Check doorbell count */
            while (0 != octeon_get_pko_doorbell_count(queue) && timeout-- > 0)
                udelay(10000);
            if ((count = octeon_get_pko_doorbell_count(queue)))
            {
                printf("Error draining PKO: queue %d doorbell count: %d is not zero\n", queue, count);
                retval = -1;
            }

        }
    }

    /* Drain POW, free buffers to FPA in case they are needed for loopback workaround. */
    cvmx_wqe_t *work;
    while ((work = cvmx_pow_work_request_sync(1)))
    {
        cvmx_helper_free_packet_data(work);
        cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
        dprintf("Freeing packet in network shutdown!\n");
    }

    /* In Octeon CN38XX, the IPD reset did not reset a fifo ptr,
    ** so this is set to zero by using an RGMII loopback.  If neither interface
    ** is RGMII, then this workaround won't work.
    ** All other chips don't need this step - the reset is sufficient.
    */
    if (octeon_get_proc_id() == OCTEON_CN38XX_PASS2)
    {
        oct_eth_info = (octeon_eth_info_t *)loopback_dev->priv;

        /* Set loopback_dev port into loopback mode*/
        cvm_oct_configure_rgmii_loopback(loopback_dev);

        /* Now read IPD_PTR_COUNT register */

        cvmx_ipd_ptr_count_t ipd_cnt;
        ipd_cnt.u64 = cvmx_read64(CVMX_IPD_PTR_COUNT);
        int to_add = (ipd_cnt.s.wqev_cnt + ipd_cnt.s.wqe_pcnt) & 0x7;
        int ptr = (-to_add) & 0x7;

        dprintf("Ptr is %d, to add count is: %d\n", ptr, to_add);

        /* Send enough packets to wrap internal ipd pointer to 0 */
        while (to_add--)
        {
            char buffer[100];
            /* Send a packet through loopback to be processed by IPD */
            octeon_eth_send(loopback_dev, buffer, 100);
            udelay(50000);
        }

        udelay(200000);

        /* Check pointer to make sure that our loopback packets moved it to 0 */
        ipd_cnt.u64 = cvmx_read64(CVMX_IPD_PTR_COUNT);
        to_add = (ipd_cnt.s.wqev_cnt + ipd_cnt.s.wqe_pcnt) & 0x7;
        ptr = (-to_add) & 0x7;
        dprintf("(After) Ptr is %d, to add count is: %d\n", ptr, to_add);
        if (to_add)
        {
            printf("ERROR: unable to reset packet input!\n");
            return -1;
        }

        /* Disable loopback and leave port disabled. */
        cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(oct_eth_info->interface), 0);
        cvmx_write_csr(CVMX_ASXX_TX_PRT_EN(oct_eth_info->interface), 0);
        cvmx_write_csr(CVMX_ASXX_PRT_LOOP(oct_eth_info->interface), 0);
        oct_eth_info->loopback = 0;
        cvmx_read64(CVMX_MIO_BOOT_BIST_STAT);  /* Make sure writes are complete */
        /* End workaround */
    }



    /* Drain POW  */
    while ((work = cvmx_pow_work_request_sync(1)))
    {
        cvmx_helper_free_packet_data(work);
        cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
        dprintf("Freeing packet in network shutdown!\n");
    }



    /* Drain FPA - this may not be necessary */
    for (i = 0; i < 8; i++)
    {
        count = 0;
        while (cvmx_fpa_alloc(i))
            count++;
        dprintf("Freed %d buffers from pool %d\n", count, i);
    }


    /* Since we are ingoring flow control, we assume that if PKO is empty
    ** the RGMII/SPI buffers are empty by now.  Since all we did was DHCP/TFTP,
    ** we should not have any outstanding packets at this point.
    ** In other situations, the GMX/SPX blocks would need to be checked to make sure they are
    ** empty before resetting.
    */
    /* Delay to let any data in GMX/SPX drain */
    udelay(50000);

    /* Reset the FPA, IPD, PKO.  These bits are new
    ** in pass 2 silicon. */
    cvmx_ipd_ctl_status_t ipd_ctl_status;
    ipd_ctl_status.u64 = cvmx_read_csr(CVMX_IPD_CTL_STATUS);
    ipd_ctl_status.s.reset = 1;
    cvmx_write_csr(CVMX_IPD_CTL_STATUS, ipd_ctl_status.u64);

    if (octeon_get_proc_id() != OCTEON_CN38XX_PASS2 && octeon_get_proc_id() != OCTEON_CN38XX_PASS1)
    {
        cvmx_pip_sft_rst_t pip_reset;
        pip_reset.u64 = cvmx_read_csr(CVMX_PIP_SFT_RST);
        pip_reset.s.rst = 1;
        cvmx_write_csr(CVMX_PIP_SFT_RST, pip_reset.u64);
    }

    cvmx_pko_reg_flags_t pko_reg_flags;
    pko_reg_flags.u64 = cvmx_read_csr(CVMX_PKO_REG_FLAGS);
    pko_reg_flags.s.reset = 1;
    cvmx_write_csr(CVMX_PKO_REG_FLAGS, pko_reg_flags.u64);

    cvmx_fpa_ctl_status_t fpa_ctl_status;
    fpa_ctl_status.u64 = cvmx_read_csr(CVMX_FPA_CTL_STATUS);
    fpa_ctl_status.s.reset = 1;
    cvmx_write_csr(CVMX_FPA_CTL_STATUS, fpa_ctl_status.u64);

    return(retval);
}

int octeon_network_hw_shutdown(void)
{
    int retval = octeon_network_hw_shutdown_internal();
    if (retval < 0)
    {
        /* Make this a fatal error since the error message is easily missed and ignoring
        ** it can lead to very strange networking behavior in the application.
        */
        printf("FATAL ERROR: Network shutdown failed.  Please reset the board.\n");
        while (1)
            ;
    }
    return(retval);
}

/*******************  Octeon networking functions ********************/
int octeon_ebt3000_rgmii_init(struct eth_device *dev, bd_t * bis, int pmsg)         /* Initialize the device	*/
{
    DECLARE_GLOBAL_DATA_PTR;
    octeon_eth_info_t* priv = (octeon_eth_info_t*)dev->priv;
    int interface = priv->interface;
    dprintf("octeon_ebt3000_rgmii_init(), dev_ptr: %p, port: %d, queue: %d\n", dev, priv->port, priv->queue);

    if (priv->initted_flag)
        return 1;

    if (!octeon_global_hw_inited)
    {
        cvm_oct_configure_common_hw();
    }
    cvm_oct_configure_port(dev);

    /* Ignore backpressure on RGMII ports */
    cvmx_write_csr(CVMX_GMXX_TX_OVR_BP(interface), 0xf << 8 | 0xf);

    uint64_t enable_mask = 1ull << priv->port;
    /* Enable the RGMII ports */
    if (octeon_is_model(OCTEON_CN30XX) || octeon_is_model(OCTEON_CN50XX))
    {
        cvmx_write_csr(CVMX_ASXX_TX_PRT_EN(interface), enable_mask);
        cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(interface), enable_mask);
#ifdef CONFIG_PALOMINO
        cvmx_write_csr(CVMX_GMXX_TX_PRTS(interface), 2);
#else
        cvmx_write_csr(CVMX_GMXX_TX_PRTS(interface), 3);
#endif
    }
    else if (octeon_is_model(OCTEON_CN31XX))
    {
        cvmx_write_csr(CVMX_ASXX_TX_PRT_EN(interface), enable_mask);
        cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(interface), enable_mask);
        cvmx_write_csr(CVMX_GMXX_TX_PRTS(interface), 3);
    }
    else if (octeon_is_model(OCTEON_CN38XX))
    {
        cvmx_write_csr(CVMX_ASXX_TX_PRT_EN(interface), enable_mask);
        cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(interface), enable_mask);
        cvmx_write_csr(CVMX_GMXX_TX_PRTS(interface), 4);
    }
    else
    {
        printf("ERROR: unsupported Octeon Model!!!!! (0x%x)\n", octeon_get_proc_id());
        return 0;
    }



    int index = priv->port - priv->interface*16;
    if (pmsg) {
        printf("%s:\t", dev->name);
    }
dprintf("init\n");
    cvm_oct_configure_rgmii_speed(dev, pmsg);
    cvmx_write_csr(CVMX_GMXX_TXX_THRESH(index, interface), 32);

    if (octeon_get_proc_id() ==  OCTEON_CN38XX_PASS1)
    {
        /* Set hi water mark as per errata GMX-4 */
        if (gd->cpu_clock_mhz*1000000 >= 325000000 && gd->cpu_clock_mhz*1000000 < 375000000)
            cvmx_write_csr(CVMX_ASXX_TX_HI_WATERX(index, interface), 12);
        else if (gd->cpu_clock_mhz*1000000 >= 375000000 && gd->cpu_clock_mhz*1000000 < 437000000)
            cvmx_write_csr(CVMX_ASXX_TX_HI_WATERX(index, interface), 11);
        else if (gd->cpu_clock_mhz*1000000 >= 437000000 && gd->cpu_clock_mhz*1000000 < 550000000)
            cvmx_write_csr(CVMX_ASXX_TX_HI_WATERX(index, interface), 10);
        else if (gd->cpu_clock_mhz*1000000 >= 550000000 && gd->cpu_clock_mhz*1000000 < 687000000)
            cvmx_write_csr(CVMX_ASXX_TX_HI_WATERX(index, interface), 9);
        else
        {
            printf("Unable to set high water marks. Unsupported frequency %u\n", gd->cpu_clock_mhz*1000000);
            return 0;
        }
    }

#if CONFIG_OCTEON_TRANTOR
    /* Normal timings don't work.  These may not be optimal, but they seem to work
    ** fine, at least for low traffic rates. */
    cvmx_write_csr(CVMX_ASXX_TX_CLK_SETX(index, interface), 0);
    cvmx_write_csr(CVMX_ASXX_RX_CLK_SETX(index, interface), 16);
#else

    // from 1.7.0 SDK
    if (OCTEON_IS_MODEL(OCTEON_CN50XX))
    {
        cvmx_write_csr(CVMX_ASXX_TX_CLK_SETX(index, interface), 16);
        cvmx_write_csr(CVMX_ASXX_RX_CLK_SETX(index, interface), 16);
    }
    else
    {
        cvmx_write_csr(CVMX_ASXX_TX_CLK_SETX(index, interface), 24);
        cvmx_write_csr(CVMX_ASXX_RX_CLK_SETX(index, interface), 24);
    }
#endif

    /* Set relaxed Preamble checking for pass 2*/
    if((octeon_get_proc_id() != OCTEON_CN38XX_PASS1))
    {
        cvmx_gmxx_rxx_frm_ctl_t frm_ctl;
        frm_ctl.u64 = cvmx_read_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface));
        frm_ctl.s.pre_free = 1;  /* New field, so must be compile time */
#if 0
        if (octeon_is_model(OCTEON_CN30XX)) {
            //
            // SCA; disable preamble checking on the 3010, God help us
            //
            frm_ctl.s.pre_chk = 0;
            frm_ctl.s.pre_strp = 1;
printf("disabled preamble checking\n");
        }
#endif
        cvmx_write_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface), frm_ctl.u64);
    }

#ifndef CONFIG_ARUBA_OCTEON
#if CONFIG_OCTEON_CN3005_EVB_HS5
    if (index == 1)
    {
        cvmx_write_csr(CVMX_ASXX_TX_CLK_SETX(index, interface), 0);
        cvmx_write_csr(CVMX_ASXX_RX_CLK_SETX(index, interface), 0);
    }
#endif
#endif

    if (!octeon_global_hw_inited)
    {
        cvmx_pko_enable();
        cvmx_ipd_enable();
        octeon_global_hw_inited = 1;
    }
    priv->initted_flag = 1;
    return(1);
}

int
octeon_ebt3000_rgmii_init_wrap(struct eth_device *dev, bd_t * bis)
{
    return octeon_ebt3000_rgmii_init(dev, bis, 1);
}

#ifdef OCTEON_SPI4000_ENET
int octeon_spi4000_init(struct eth_device *dev, bd_t * bis)         /* Initialize the device	*/
{
    octeon_eth_info_t* priv = (octeon_eth_info_t*)dev->priv;
    static char spi4000_inited[2] = {0};
    dprintf("octeon_spi4000_init(), dev_ptr: %p, port: %d, queue: %d\n", dev, priv->port, priv->queue);

    if (priv->initted_flag)
        return 1;

    if (!spi4000_inited[priv->interface])
    {
        spi4000_inited[priv->interface] = 1;
        if (cvmx_spi4000_initialize(priv->interface) < 0)
        {
            printf("ERROR initializing spi4000 on Octeon Interface %d\n", priv->interface);
            return 0;
        }
    }

    if (!octeon_global_hw_inited)
    {
        cvm_oct_configure_common_hw();
    }
    cvm_oct_configure_port(dev);


    if (!octeon_global_hw_inited)
    {
        cvmx_pko_enable();
        cvmx_ipd_enable();
        octeon_global_hw_inited = 1;
    }
    priv->initted_flag = 1;
    return(1);

}
#endif




void octeon_eth_rgmii_enable(struct eth_device *dev)
{

    cvmx_gmxx_inf_mode_t mode;
    octeon_eth_info_t *oct_eth_info;

    oct_eth_info = (octeon_eth_info_t *)dev->priv;

    
    mode.u64 = cvmx_read_csr(CVMX_GMXX_INF_MODE(oct_eth_info->interface));
    if (mode.s.en
        && !((octeon_is_model(OCTEON_CN38XX) || octeon_is_model(OCTEON_CN58XX)) && mode.s.type == 1))
    {
        uint64_t tmp;
        tmp = cvmx_read_csr(CVMX_ASXX_RX_PRT_EN(oct_eth_info->interface));
        tmp |= (1ull << (oct_eth_info->port & 0x3));
        cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(oct_eth_info->interface), tmp);   /* Disable the RGMII RX ports */
        tmp = cvmx_read_csr(CVMX_ASXX_TX_PRT_EN(oct_eth_info->interface));
        tmp |= (1ull << (oct_eth_info->port & 0x3));
        cvmx_write_csr(CVMX_ASXX_TX_PRT_EN(oct_eth_info->interface), tmp);   /* Disable the RGMII TX ports */
    }


}



int octeon_eth_send(struct eth_device *dev, volatile void *packet, int length)	   /* Send a packet	*/
{
    dprintf("ethernet TX! ptr: %p, len: %d\n", packet, length);
    /* We need to copy this to a FPA buffer, then give that to TX */

    /* This is needed since we disable rx/tx in halt */
    octeon_eth_rgmii_enable(dev);

    /* Poll for link status change */
dprintf("snd\n");
    cvm_oct_configure_rgmii_speed(dev, 1);
    void *fpa_buf = cvmx_fpa_alloc(CVMX_FPA_PACKET_POOL);
    if (!fpa_buf)
    {
        printf("ERROR allocating buffer for packet!\n");
    }
    memcpy(fpa_buf, (void *)packet, length);
    cvm_oct_xmit(dev, fpa_buf, length);
    return(0);
}

int octeon_etest_recv(struct eth_device *dev, uchar *pkt)         /* Check for received packets	*/
{
    cvmx_wqe_t *work = cvmx_pow_work_request_sync(1);
    if (!work)
    {
        /* Poll for link status change */
dprintf("rcv\n");
        cvm_oct_configure_rgmii_speed(dev, 1);
        return(0);
    }
    if (work->word2.s.rcv_error)
    {
        /* Work has error, so drop */
        printf("Error packet received, dropping\n");
        cvmx_helper_free_packet_data(work);
        cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
        return(0);
    }


    void *packet_data = cvmx_phys_to_ptr(work->packet_ptr.u64 & 0xffffffffffull);
    int length = work->len;
    memcpy(pkt, (char*) packet_data, length);

    dprintf("############# got work: %p, len: %d, packet_ptr: %p\n",
            work, length, packet_data);
#ifdef DEBUG
    /* Dump out packet contents */
    if (enet_verbose()) {
	int i, j;
	unsigned char *up = packet_data;

	for (i = 0; (i + 16) < length; i += 16)
	{
	    printf("%04x ", i);
	    for (j = 0; j < 16; ++j)
	    {
		printf("%02x ", up[i+j]);
	    }
	    printf("    ");
	    for (j = 0; j < 16; ++j)
	    {
		printf("%c", ((up[i+j] >= ' ') && (up[i+j] <= '~')) ? up[i+j] : '.');
	    }
	    printf("\n");
	}
	printf("%04x ", i);
	for (j = 0; i+j < length; ++j)
	{
	    printf("%02x ", up[i+j]);
	}
	for (; j < 16; ++j)
	{
	    printf("   ");
	}
	printf("    ");
	for (j = 0; i+j < length; ++j)
	{
	    printf("%c", ((up[i+j] >= ' ') && (up[i+j] <= '~')) ? up[i+j] : '.');
	}
	printf("\n");
    }
#endif

    //NetReceive (packet_data, length);
    cvmx_helper_free_packet_data(work);
    cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
    /* Free WQE and packet data */
    return(length);
}

int octeon_eth_recv(struct eth_device *dev)         /* Check for received packets	*/
{
    cvmx_wqe_t *work = cvmx_pow_work_request_sync(1);
    if (!work)
    {
        cvmx_gmxx_rxx_int_reg_t err;
        octeon_eth_info_t* priv = (octeon_eth_info_t*)dev->priv;
        int interface = priv->interface;
        int index = priv->port - 16 * interface;
        err.u64 = cvmx_read_csr(CVMX_GMXX_RXX_INT_REG(index, interface));
        /* Poll for link status change */
dprintf("rcv: GMXX INT REG: 0x%Lx\n", err.u64);
        cvm_oct_configure_rgmii_speed(dev, 1);
        return(0);
    }
    if (work->word2.s.rcv_error)
    {
        /* Work has error, so drop */
        if ((work->word2.snoip.err_code == 5) || (work->word2.snoip.err_code == 7))
        {
            /* We received a packet with either an alignment error or a
                FCS error. This may be signalling that we are running
                10Mbps with GMXX_RXX_FRM_CTL[PRE_CHK] off. If this is the
                case we need to parse the packet to determine if we can
                remove a non spec preamble and generate a correct packet */
            octeon_eth_info_t* priv = (octeon_eth_info_t*)dev->priv;
            int interface = priv->interface;
            int index = priv->port - 16 * interface;

            cvmx_gmxx_rxx_frm_ctl_t gmxx_rxx_frm_ctl;
            gmxx_rxx_frm_ctl.u64 = cvmx_read_csr(CVMX_GMXX_RXX_FRM_CTL(index, interface));
            if (gmxx_rxx_frm_ctl.s.pre_chk == 0)
            {
                uint8_t *ptr = cvmx_phys_to_ptr(work->packet_ptr.u64 & 0xffffffffffull);
                int i = 0;
                while (i<work->len-1)
                {
                    if (*ptr != 0x55)
                        break;
                    ptr++;
                    i++;
                }
                if (*ptr == 0xd5)
                {
                    //DEBUGPRINT("Port %d received 0xd5 preamble\n", work->ipprt);
                    work->packet_ptr.s.addr += i+1;
                    work->len -= i+5;
                }
                else if ((*ptr & 0xf) == 0xd)
                {
                    //DEBUGPRINT("Port %d received 0x?d preamble\n", work->ipprt);
                    work->packet_ptr.s.addr += i;
                    work->len -= i+4;
                    for (i=0; i<work->len; i++)
                    {
                        *ptr = ((*ptr&0xf0)>>4) | ((*(ptr+1)&0xf)<<4);
                        ptr++;
                    }
                }
                else
                {
                    printf("Unknown preamble, packet dropped\n");
                    //cvmx_helper_dump_packet(work);
                    cvmx_helper_free_packet_data(work);
                    cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
                    return 0;
                }
            }
        } else {
            printf("Error packet received, dropping: 0x%x\n", work->word2.s.err_code);
            cvmx_helper_free_packet_data(work);
            cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
            return(0);
        }
    }


    void *packet_data = cvmx_phys_to_ptr(work->packet_ptr.u64 & 0xffffffffffull);
    int length = work->len;

    dprintf("############# got work: %p, len: %d, packet_ptr: %p\n",
            work, length, packet_data);
#ifdef DEBUG
    /* Dump out packet contents */
    if (enet_verbose()) {
	int i, j;
	unsigned char *up = packet_data;

	for (i = 0; (i + 16) < length; i += 16)
	{
	    printf("%04x ", i);
	    for (j = 0; j < 16; ++j)
	    {
		printf("%02x ", up[i+j]);
	    }
	    printf("    ");
	    for (j = 0; j < 16; ++j)
	    {
		printf("%c", ((up[i+j] >= ' ') && (up[i+j] <= '~')) ? up[i+j] : '.');
	    }
	    printf("\n");
	}
	printf("%04x ", i);
	for (j = 0; i+j < length; ++j)
	{
	    printf("%02x ", up[i+j]);
	}
	for (; j < 16; ++j)
	{
	    printf("   ");
	}
	printf("    ");
	for (j = 0; i+j < length; ++j)
	{
	    printf("%c", ((up[i+j] >= ' ') && (up[i+j] <= '~')) ? up[i+j] : '.');
	}
	printf("\n");
    }
#endif

    NetReceive (packet_data, length);
    cvmx_helper_free_packet_data(work);
    cvmx_fpa_free(work, CVMX_FPA_WQE_POOL, 0);
    /* Free WQE and packet data */
    return(length);
}
void octeon_eth_halt(struct eth_device *dev)			/* stop SCC			*/
{
    /* Disable reception on this port at the GMX block */

    cvmx_gmxx_inf_mode_t mode;
    octeon_eth_info_t *oct_eth_info;

    oct_eth_info = (octeon_eth_info_t *)dev->priv;

    
    mode.u64 = cvmx_read_csr(CVMX_GMXX_INF_MODE(oct_eth_info->interface));
    if (mode.s.en
        && !((octeon_is_model(OCTEON_CN38XX) || octeon_is_model(OCTEON_CN58XX)) && mode.s.type == 1))
    {
        uint64_t tmp;
        tmp = cvmx_read_csr(CVMX_ASXX_RX_PRT_EN(oct_eth_info->interface));
        tmp &= ~(1ull << (oct_eth_info->port & 0x3));
        cvmx_write_csr(CVMX_ASXX_RX_PRT_EN(oct_eth_info->interface), tmp);   /* Disable the RGMII RX ports */
        tmp = cvmx_read_csr(CVMX_ASXX_TX_PRT_EN(oct_eth_info->interface));
        tmp &= ~(1ull << (oct_eth_info->port & 0x3));
        cvmx_write_csr(CVMX_ASXX_TX_PRT_EN(oct_eth_info->interface), tmp);   /* Disable the RGMII TX ports */
    }

}

/* Variable shared between RGMII and SPI setup routines. */
static int card_number;
static int available_mac_count = -1;
static uint8_t mac_addr[6];
#ifdef CONFIG_PALOMINO
int pal_port_map[2] = { 1, 0 };
#endif

/*********************************************************
Only handle built-in RGMII ports here
*********************************************/
int octeon_eth_initialize (bd_t * bis)
{
    DECLARE_GLOBAL_DATA_PTR;
    struct eth_device *dev;
    octeon_eth_info_t *oct_eth_info;
    int port;
    int interface;
#ifndef CONFIG_APBOOT
    uint32_t *mac_inc_ptr = (uint32_t *)(&mac_addr[2]);
#endif
    int num_ports;
    int num_ints;
#if defined(CONFIG_ARUBA_OCTEON) || defined(CONFIG_OCTEON_BBGW_REF)
	char *tmp, *end;
	unsigned char env_enetaddr[6];
#endif

    if ((getenv("enet_verbose") != NULL))
    {
        DECLARE_GLOBAL_DATA_PTR;
        gd->flags |= GD_FLG_ENET_VERBOSE;
    }

#if !defined(CONFIG_ARUBA_OCTEON) && !defined(CONFIG_OCTEON_BBGW_REF)
    if (available_mac_count == -1)
    {
        available_mac_count = gd->mac_desc.count;
        memcpy(mac_addr, (uint8_t *)(gd->mac_desc.mac_addr_base), 6);
    }
#else
    int i;
	tmp = getenv ("ethaddr");
	for (i=0; i<6; i++) {
		env_enetaddr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
		if (tmp)
			tmp = (*end) ? end + 1 : end;
	}
	available_mac_count = gd->mac_desc.count;
	memcpy((uint8_t *)(gd->mac_desc.mac_addr_base), env_enetaddr, 6);
	memcpy(mac_addr, (uint8_t *)(gd->mac_desc.mac_addr_base), 6);
#endif
    if (getenv("disable_networking"))
    {
        printf("Networking disabled with 'disable_networking' environment variable, skipping RGMII interface\n");
        return 0;
    }
    if (available_mac_count <= 0)
    {
        printf("No available MAC addresses for RGMII interface, skipping\n");
        return 0;
    }

    if (octeon_get_proc_id() == OCTEON_CN38XX_PASS1)
    {
        printf("\n");
        printf("\n");
        printf("WARNING:\n");
        printf("WARNING:OCTEON pass 1 does not support network hardware reset.\n");
        printf("WARNING:Bootloader networking use will cause unstable operation of any other\n");
        printf("WARNING:program or OS using the network hardware.\n");
        printf("WARNING:\n");
        printf("\n");
        printf("\n");
    }

    /* Do board specific init in early_board_init or checkboard if possible. */

    /* NOTE: on 31XX based boards, the GMXX_INF_MODE register must be set appropriately
    ** before this code is run (in checkboard for instance).  The hardware is configured based
    ** on the settings of GMXX_INF_MODE. */

    if (octeon_is_model(OCTEON_CN38XX))
        num_ints = 2;
    else
        num_ints = 1;


    /* Check to see what interface and ports we should use */
    for (interface = 0; interface < num_ints; interface++)
    {
        cvmx_gmxx_inf_mode_t mode;
        mode.u64 = cvmx_read_csr(CVMX_GMXX_INF_MODE(interface));
        int interface_ethernet = 0;

        if (octeon_is_model(OCTEON_CN31XX))
        {
            interface_ethernet = 1;
            if (mode.s.type)
                num_ports = 2; /* ports 1/2 combined into 1 GMII */
            else
                num_ports = 3; /* ports 1/2 each are RGMII */
        }
        else if (octeon_is_model(OCTEON_CN30XX) || octeon_is_model(OCTEON_CN50XX))
        {
            interface_ethernet = 1;
            if (mode.s.type)
                num_ports = 2; /* ports 1/2 combined into 1 GMII */
            else
#if defined(CONFIG_PALOMINO) || defined(CONFIG_MOSCATO)
				// palomino/moscato have only two ports
                num_ports = 2; /* ports 1/2 each are RGMII */
#else
                num_ports = 3; /* ports 1/2 each are RGMII */
#endif
        }
        else if (octeon_is_model(OCTEON_CN38XX))
        {
            interface_ethernet = !mode.s.type;
            num_ports = 4;
        }
        else
        {
            printf("ERROR: unsupported octeon model: 0x%x\n", octeon_get_proc_id());
            num_ports = 0;
        }

        if (mode.s.en && interface_ethernet)
        {
            dprintf("Interface %d is RGMII\n", interface);
            /* RGMII or SGMII */
            for (port = 16 * interface; (port < num_ports + 16 * interface) && available_mac_count-- > 0; port++)
            {
                dev = (struct eth_device *) malloc(sizeof(*dev));

                /* Save pointer for port 16 or 0 to use in IPD reset workaround */
                if (port == 16 || port == 0)
                    loopback_dev = dev;

                oct_eth_info = (octeon_eth_info_t *) malloc(sizeof(octeon_eth_info_t));

#ifdef CONFIG_PALOMINO
                oct_eth_info->port = pal_port_map[port];
#else
                oct_eth_info->port = port;
#endif
                oct_eth_info->queue = cvmx_pko_get_base_queue(oct_eth_info->port);
                oct_eth_info->interface = interface;
                oct_eth_info->initted_flag = 0;

                oct_eth_info->port_type = PORT_RGMII;

                //
                // on Palomino, both ports are RGMII
                // on other boards, e.g. Moscato, figure
                // out the mode fromm looking at how the GMX mode
                // register was set
                //
#ifndef CONFIG_PALOMINO
                if (!octeon_is_model(OCTEON_CN38XX) && port == 1 && num_ports == 2)
                    oct_eth_info->port_type = PORT_GMII;

                if ((octeon_is_model(OCTEON_CN30XX) || octeon_is_model(OCTEON_CN50XX)) && port == 0 && mode.s.p0mii)
                    oct_eth_info->port_type = PORT_MII;
#endif

                dprintf("Setting up port: %d, queue: %d, int: %d [%u]\n", oct_eth_info->port, oct_eth_info->queue, oct_eth_info->interface, card_number);
#if defined(CONFIG_ARUBA_OCTEON) || defined(CONFIG_OCTEON_BBGW_REF)
                sprintf (dev->name, "en%d", card_number);
#else
                sprintf (dev->name, "octeth%d", card_number);
#endif
                card_number++;

                dev->priv = (void *)oct_eth_info;
                dev->iobase = 0;
                dev->init = octeon_ebt3000_rgmii_init_wrap;
                dev->halt = octeon_eth_halt;
                dev->send = octeon_eth_send;
                dev->recv = octeon_eth_recv;
                memcpy(dev->enetaddr, mac_addr, 6);
#ifndef CONFIG_APBOOT
                (*mac_inc_ptr)++;  /* increment MAC address */
#endif

                eth_register (dev);
#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)
                miiphy_register(dev->name,
                                octeon_miiphy_read, octeon_miiphy_write);
#endif

#ifdef CONFIG_MOSCATO_notneeded
                if (!strcmp(dev->name, "en1")) {
                    octeon_ebt3000_rgmii_init(dev, bis, 0);
                }
#endif

            }
            if (octeon_is_model(OCTEON_CN38XX))
            {
                /* PKO registers */
                cvmx_pko_reg_gmx_port_mode_t pko_mode;
                pko_mode.u64 = cvmx_read_csr(CVMX_PKO_REG_GMX_PORT_MODE);
                if (interface == 0)
                    pko_mode.s.mode0 = 2;   /* 16 >> 2 == 4 ports */
                else
                    pko_mode.s.mode1 = 2;   /* 16 >> 2 == 4 ports */
                cvmx_write_csr(CVMX_PKO_REG_GMX_PORT_MODE, pko_mode.u64);
            }
        }
    }


    return card_number;
}

#ifdef OCTEON_SPI4000_ENET
int octeon_spi4000_initialize(bd_t * bis)
{
    DECLARE_GLOBAL_DATA_PTR;
    struct eth_device *dev;
    octeon_eth_info_t *oct_eth_info;
    int port;
    int interface = 0;
    uint32_t *mac_inc_ptr = (uint32_t *)(&mac_addr[2]);

    if (available_mac_count == -1)
    {
        available_mac_count = gd->mac_desc.count;
        memcpy(mac_addr, (void *)(gd->mac_desc.mac_addr_base), 6);
    }
    if (getenv("disable_networking"))
    {
        printf("Networking disabled with 'disable_networking' environment variable, skipping SPI interface\n");
        return 0;
    }
    if (cvmx_spi4000_detect(interface) < 0)
        return 0;
    if (available_mac_count <= 0)
    {
        printf("No available MAC addresses for SPI interface, skipping\n");
        return 0;
    }


    /* TODO - need to make sure that the RGMII MACs ignore backpressure
    ** and always transmit packets.  No packets can be buffered
    ** when the HW is reset.
    */

    for (port = 0; port < 10 && available_mac_count-- > 0; port++)
    {
        dev = (struct eth_device *) malloc(sizeof(*dev));
        oct_eth_info = (octeon_eth_info_t *) malloc(sizeof(octeon_eth_info_t));

        oct_eth_info->port = port;
        oct_eth_info->queue = cvmx_pko_get_base_queue(oct_eth_info->port);
        oct_eth_info->interface = interface;
        oct_eth_info->port_type = PORT_SPI4000;
        oct_eth_info->initted_flag = 0;

        dprintf("Setting up port: %d, queue: %d, int: %d\n", oct_eth_info->port, oct_eth_info->queue, oct_eth_info->interface);
        sprintf (dev->name, "octspi%d", card_number);
        card_number++;

        dev->priv = (void *)oct_eth_info;
        dev->iobase = 0;
        dev->init = octeon_spi4000_init;
        dev->halt = octeon_eth_halt;
        dev->send = octeon_eth_send;
        dev->recv = octeon_eth_recv;
        memcpy(dev->enetaddr, mac_addr, 6);
        (*mac_inc_ptr)++;  /* increment MAC address */

        eth_register (dev);
    }
    return card_number;
}
#endif  /* OCTEON_SPI4000_ENET */


#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)

/**
 * Perform an MII read. Called by the generic MII routines
 *
 * @param dev      Device to perform read for
 * @param phy_id   The MII phy id
 * @param location Register location to read
 * @return Result from the read or zero on failure
 */

int  octeon_miiphy_read(char *p, unsigned char addr, unsigned char reg, unsigned short * value)
{
    cvmx_smi_cmd_t          smi_cmd;
    cvmx_smi_rd_dat_t       smi_rd;
    uint64_t                start, end;         /* For timeout */

    smi_cmd.u64 = 0;
    smi_cmd.s.phy_op = 1;
    smi_cmd.s.phy_adr = addr;
    smi_cmd.s.reg_adr = reg;
    cvmx_write_csr(CVMX_SMI_CMD, smi_cmd.u64);

    start = octeon_get_cycles();
    end = start + 60000000;     /* 100 mS (assuming 600 MHz) should be enough */

    do
    {
        smi_rd.u64 = cvmx_read_csr(CVMX_SMI_RD_DAT);
    } while (smi_rd.s.pending && (octeon_get_cycles() < end));

    if (smi_rd.s.pending || !smi_rd.s.val)
	return -1;

    *value = smi_rd.s.dat;
    return 0;
}

/* Wrapper to make non-error checking code simpler */
int miiphy_read_wrapper(unsigned char  addr, unsigned char  reg)
{
    unsigned short value;

    if (miiphy_read("en0", addr, reg, &value) < 0)
    {
        printf("ERROR: miiphy_read_wrapper(0x%x, 0x%x) timed out.\n", addr, reg);
        return (-1);
    }

    return(value);
}




int octeon_miiphy_write(char *p, unsigned char  addr,
		 unsigned char  reg,
		 unsigned short value)
{
    cvmx_smi_cmd_t          smi_cmd;
    cvmx_smi_wr_dat_t       smi_wr;
    uint64_t                start, end;

    smi_wr.u64 = 0;
    smi_wr.s.dat = value;
    cvmx_write_csr(CVMX_SMI_WR_DAT, smi_wr.u64);

    smi_cmd.u64 = 0;
    smi_cmd.s.phy_op = 0;
    smi_cmd.s.phy_adr = addr;
    smi_cmd.s.reg_adr = reg;
    cvmx_write_csr(CVMX_SMI_CMD, smi_cmd.u64);

    start = octeon_get_cycles();
    end = start + 60000000;     /* 100 mS (assuming 600 MHz) should be enough */

    do
    {
        smi_wr.u64 = cvmx_read_csr(CVMX_SMI_WR_DAT);
    } while (smi_wr.s.pending && (octeon_get_cycles() < end));

    if (smi_wr.s.pending)
	return -1;

    return 0;
}

#endif /* MII */
#endif   /* OCTEON_INTERNAL_ENET */
