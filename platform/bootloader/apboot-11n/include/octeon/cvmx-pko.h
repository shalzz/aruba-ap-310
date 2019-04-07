/***********************license start***************
 * Copyright (c) 2003-2008  Cavium Networks (support@cavium.com). All rights
 * reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of Cavium Networks nor the names of
 *       its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 *
 * This Software, including technical data, may be subject to U.S.  export
 * control laws, including the U.S.  Export Administration Act and its
 * associated regulations, and may be subject to export or import regulations
 * in other countries.  You warrant that You will comply strictly in all
 * respects with all such regulations and acknowledge that you have the
 * responsibility to obtain licenses to export, re-export or import the
 * Software.
 *
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND CAVIUM NETWORKS MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 * RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
 * REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
 * DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
 * PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET
 * POSSESSION OR CORRESPONDENCE TO DESCRIPTION.  THE ENTIRE RISK ARISING OUT
 * OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
 *
 *
 * For any questions regarding licensing please contact marketing@caviumnetworks.com
 *
 ***********************license end**************************************/






/**
 * @file
 *
 * Interface to the hardware Packet Output unit.
 *
 * Starting with SDK 1.7.0, the PKO output functions now support
 * two types of locking. CVMX_PKO_LOCK_ATOMIC_TAG continues to
 * function similarly to previous SDKs by using POW atomic tags
 * to preserve ordering and exclusivity. As a new option, you
 * can now pass CVMX_PKO_LOCK_CMD_QUEUE which uses a ll/sc
 * memory based locking instead. This locking has the advantage
 * of not affecting the tag state but doesn't preserve packet
 * ordering. CVMX_PKO_LOCK_CMD_QUEUE is appropriate in most
 * generic code while CVMX_PKO_LOCK_CMD_QUEUE should be used
 * with hand tuned fast path code.
 *
 * Some of other SDK differences visible to the command command
 * queuing:
 * - PKO indexes are no longer stored in the FAU. A large
 *   percentage of the FAU register block used to be tied up
 *   maintaining PKO queue pointers. These are now stored in a
 *   global named block.
 * - The PKO <b>use_locking</b> parameter can now have a global
 *   effect. Since all application use the same named block,
 *   queue locking correctly applies across all operating
 *   systems when using CVMX_PKO_LOCK_CMD_QUEUE.
 * - PKO 3 word commands are now supported. Use
 *   cvmx_pko_send_packet_finish3().
 *
 * <hr>$Revision: 33479 $<hr>
 */


#ifndef __CVMX_PKO_H__
#define __CVMX_PKO_H__

#ifndef CVMX_DONT_INCLUDE_CONFIG
#include "executive-config.h"
#ifdef CVMX_ENABLE_PKO_FUNCTIONS
#include "cvmx-config.h"
#endif
#endif

#include "cvmx-cvmmem.h"
#include "cvmx-fau.h"
#include "cvmx-fpa.h"
#include "cvmx-pow.h"
#include "cvmx-cmd-queue.h"

/* Adjust the command buffer size by 1 word so that in the case of using only
** two word PKO commands no command words stradle buffers.  The useful values
** for this are 0 and 1. */
#define CVMX_PKO_COMMAND_BUFFER_SIZE_ADJUST (1)

#ifdef	__cplusplus
extern "C" {
#endif

#define CVMX_PKO_MAX_OUTPUT_QUEUES_STATIC 256
#define CVMX_PKO_MAX_OUTPUT_QUEUES      ((OCTEON_IS_MODEL(OCTEON_CN31XX) || OCTEON_IS_MODEL(OCTEON_CN3010) || OCTEON_IS_MODEL(OCTEON_CN3005) || OCTEON_IS_MODEL(OCTEON_CN50XX)) ? 32 : (OCTEON_IS_MODEL(OCTEON_CN58XX) || OCTEON_IS_MODEL(OCTEON_CN56XX)) ? 256 : 128)
#define CVMX_PKO_NUM_OUTPUT_PORTS       40
#define CVMX_PKO_MEM_QUEUE_PTRS_ILLEGAL_PID 63 // use this for queues that are not used
#define CVMX_PKO_QUEUE_STATIC_PRIORITY  9
#define CVMX_PKO_ILLEGAL_QUEUE  0xFFFF
#if CVMX_PKO_USE_FAU_FOR_OUTPUT_QUEUES
#define CVMX_PKO_INDEX_BITS     12
#define CVMX_PKO_INDEX_MASK     ((1ull << CVMX_PKO_INDEX_BITS) - 1)
#else
#define CVMX_PKO_MAX_QUEUE_DEPTH 0
#endif

typedef enum
{
    CVMX_PKO_SUCCESS,
    CVMX_PKO_INVALID_PORT,
    CVMX_PKO_INVALID_QUEUE,
    CVMX_PKO_INVALID_PRIORITY,
    CVMX_PKO_NO_MEMORY
} cvmx_pko_status_t;

/**
 * This enumeration represents the differnet locking modes supported by PKO.
 */
typedef enum
{
    CVMX_PKO_LOCK_NONE = 0,         /**< PKO doesn't do any locking. It is the responsibility
                                        of the application to make sure that no other core is
                                        accessing the same queue at the smae time */
    CVMX_PKO_LOCK_ATOMIC_TAG = 1,   /**< PKO performs an atomic tagswitch to insure exclusive
                                        access to the output queue. This will maintain
                                        packet ordering on output */
#if CVMX_PKO_USE_FAU_FOR_OUTPUT_QUEUES == 0
    CVMX_PKO_LOCK_CMD_QUEUE = 2,    /**< PKO uses the common command queue locks to insure
                                        exclusive access to the output queue. This is a memory
                                        based ll/sc. This is the most portable locking
                                        mechanism */
#endif
} cvmx_pko_lock_t;

typedef struct
{
    uint32_t    packets;
    uint64_t    octets;
  uint64_t doorbell;
} cvmx_pko_port_status_t;

/**
 * This structure defines the address to use on a packet enqueue
 */
typedef union
{
    uint64_t                u64;
    struct
    {
        cvmx_mips_space_t   mem_space   : 2;    /**< Must CVMX_IO_SEG */
        uint64_t            reserved    :13;    /**< Must be zero */
        uint64_t            is_io       : 1;    /**< Must be one */
        uint64_t            did         : 8;    /**< The ID of the device on the non-coherent bus */
        uint64_t            reserved2   : 4;    /**< Must be zero */
        uint64_t            reserved3   :18;    /**< Must be zero */
        uint64_t            port        : 6;    /**< The hardware likes to have the output port in addition to the output queue */
        uint64_t            queue       : 9;    /**< The output queue to send the packet to (0-127 are legal) */
        uint64_t            reserved4   : 3;    /**< Must be zero */
   } s;
} cvmx_pko_doorbell_address_t;

/**
 * Structure of the first packet output command word.
 */
typedef union
{
    uint64_t                u64;
    struct
    {
        cvmx_fau_op_size_t  size1       : 2; /**< The size of the reg1 operation - could be 8, 16, 32, or 64 bits */
        cvmx_fau_op_size_t  size0       : 2; /**< The size of the reg0 operation - could be 8, 16, 32, or 64 bits */
        uint64_t            subone1     : 1; /**< If set, subtract 1, if clear, subtract packet size */
        uint64_t            reg1        :11; /**< The register, subtract will be done if reg1 is non-zero */
        uint64_t            subone0     : 1; /**< If set, subtract 1, if clear, subtract packet size */
        uint64_t            reg0        :11; /**< The register, subtract will be done if reg0 is non-zero */
        uint64_t            le          : 1; /**< When set, interpret segment pointer and segment bytes in little endian order */
        uint64_t            n2          : 1; /**< When set, packet data not allocated in L2 cache by PKO */
        uint64_t            wqp         : 1; /**< If set and rsp is set, word3 contains a pointer to a work queue entry */
        uint64_t            rsp         : 1; /**< If set, the hardware will send a response when done */
        uint64_t            gather      : 1; /**< If set, the supplied pkt_ptr is really a pointer to a list of pkt_ptr's */
        uint64_t            ipoffp1     : 7; /**< If ipoffp1 is non zero, (ipoffp1-1) is the number of bytes to IP header,
                                                and the hardware will calculate and insert the  UDP/TCP checksum */
        uint64_t            ignore_i    : 1; /**< If set, ignore the I bit (force to zero) from all pointer structures */
        uint64_t            dontfree    : 1; /**< If clear, the hardware will attempt to free the buffers containing the packet */
        uint64_t            segs        : 6; /**< The total number of segs in the packet, if gather set, also gather list length */
        uint64_t            total_bytes :16; /**< Including L2, but no trailing CRC */
    } s;
} cvmx_pko_command_word0_t;

/* CSR typedefs have been moved to cvmx-csr-*.h */

/**
 * Definition of internal state for Packet output processing
 */
typedef struct
{
    uint64_t *      start_ptr;          /**< ptr to start of buffer, offset kept in FAU reg */
} cvmx_pko_state_elem_t;


#ifdef CVMX_ENABLE_PKO_FUNCTIONS
/**
 * Call before any other calls to initialize the packet
 * output system.
 */
extern void cvmx_pko_initialize_global(void);
extern int cvmx_pko_initialize_local(void);

#endif


/**
 * Enables the packet output hardware. It must already be
 * configured.
 */
extern void cvmx_pko_enable(void);


/**
 * Disables the packet output. Does not affect any configuration.
 */
extern void cvmx_pko_disable(void);


/**
 * Shutdown and free resources required by packet output.
 */

#ifdef CVMX_ENABLE_PKO_FUNCTIONS
extern void cvmx_pko_shutdown(void);
#endif

/**
 * Configure a output port and the associated queues for use.
 *
 * @param port       Port to configure.
 * @param base_queue First queue number to associate with this port.
 * @param num_queues Number of queues t oassociate with this port
 * @param priority   Array of priority levels for each queue. Values are
 *                   allowed to be 1-8. A value of 8 get 8 times the traffic
 *                   of a value of 1. There must be num_queues elements in the
 *                   array.
 */
extern cvmx_pko_status_t cvmx_pko_config_port(uint64_t port, uint64_t base_queue, uint64_t num_queues, const uint64_t priority[]);


/**
 * Ring the packet output doorbell. This tells the packet
 * output hardware that "len" command words have been added
 * to its pending list.  This command includes the required
 * CVMX_SYNCWS before the doorbell ring.
 *
 * @param port   Port the packet is for
 * @param queue  Queue the packet is for
 * @param len    Length of the command in 64 bit words
 */
static inline void cvmx_pko_doorbell(uint64_t port, uint64_t queue, uint64_t len)
{
   cvmx_pko_doorbell_address_t ptr;

   ptr.u64          = 0;
   ptr.s.mem_space  = CVMX_IO_SEG;
   ptr.s.did        = CVMX_OCT_DID_PKT_SEND;
   ptr.s.is_io      = 1;
   ptr.s.port       = port;
   ptr.s.queue      = queue;
   CVMX_SYNCWS;  /* Need to make sure output queue data is in DRAM before doorbell write */
   cvmx_write_io(ptr.u64, len);
}


#if CVMX_PKO_USE_FAU_FOR_OUTPUT_QUEUES && defined(CVMX_ENABLE_PKO_FUNCTIONS)
/**
 * Set queue index to a specific value.
 * This abstracts the write operation normally performed on FAU registers.
 *
 * @param queue  Queue number associated with a port
 * @param index  Value set, the initial index into the queue.
 */
static inline void cvmx_pko_set_queue_index(int queue, uint64_t index)
{
#ifdef CVMX_PKO_LOCKLESS_OPERATION
    /* Remap array index to avoid cross-core memory activity */
    uint32_t mapped_idx  = (queue & 0xF) << 4 | (queue & 0xF0) >> 4;

    cvmx_pko_queue_index[mapped_idx] = index;

#else
    cvmx_fau_atomic_write64((cvmx_fau_reg_64_t)(CVMX_FAU_REG_OQ_ADDR_INDEX + 8 * queue), index);

#endif
}

/**
 * Increment queue index by a specific value.
 * This abstracts the fetch-and-add operation normally performed on FAU registers.
 *
 * @param queue  Queue number associated with a port
 * @param added  Signed value added to the queue index.
 * @return The queue index value before the increment or decrement.
 */
static inline uint64_t cvmx_pko_update_queue_index(int queue, int64_t added)
{
#ifdef CVMX_PKO_LOCKLESS_OPERATION
    /* Remap array index to avoid cross-core memory activity */
    uint32_t mapped_idx  = (queue & 0xF) << 4 | (queue & 0xF0) >> 4;
    uint64_t prev        = cvmx_pko_queue_index[mapped_idx];

    cvmx_pko_queue_index[mapped_idx] = prev + added;
    return prev;

#else
    return cvmx_fau_fetch_and_add64((cvmx_fau_reg_64_t)(CVMX_FAU_REG_OQ_ADDR_INDEX + 8 * queue), added);

#endif
}
#endif


/**
 * Prepare to send a packet.  This may initiate a tag switch to
 * get exclusive access to the output queue structure, and
 * performs other prep work for the packet send operation.
 *
 * cvmx_pko_send_packet_finish() MUST be called after this function is called,
 * and must be called with the same port/queue/use_locking arguments.
 *
 * The use_locking parameter allows the caller to use three
 * possible locking modes.
 * - CVMX_PKO_LOCK_NONE
 *      - PKO doesn't do any locking. It is the responsibility
 *          of the application to make sure that no other core
 *          is accessing the same queue at the smae time.
 * - CVMX_PKO_LOCK_ATOMIC_TAG
 *      - PKO performs an atomic tagswitch to insure exclusive
 *          access to the output queue. This will maintain
 *          packet ordering on output.
 * - CVMX_PKO_LOCK_CMD_QUEUE
 *      - PKO uses the common command queue locks to insure
 *          exclusive access to the output queue. This is a
 *          memory based ll/sc. This is the most portable
 *          locking mechanism.
 *
 * NOTE: If atomic locking is used, the POW entry CANNOT be
 * descheduled, as it does not contain a valid WQE pointer.
 *
 * @param port   Port to send it on
 * @param queue  Queue to use
 * @param use_locking
 *               CVMX_PKO_LOCK_NONE, CVMX_PKO_LOCK_ATOMIC_TAG, or CVMX_PKO_LOCK_CMD_QUEUE
 */
#ifdef CVMX_ENABLE_PKO_FUNCTIONS
static inline void cvmx_pko_send_packet_prepare(uint64_t port, uint64_t queue, cvmx_pko_lock_t use_locking)
{
#if CVMX_PKO_USE_FAU_FOR_OUTPUT_QUEUES && defined(CVMX_PKO_LOCKLESS_OPERATION)

    /* Prefetch queue index array */
    CVMX_PREFETCH0(cvmx_pko_queue_index + ((queue & 0xF) << 4));
#else
    if (use_locking == CVMX_PKO_LOCK_ATOMIC_TAG)
    {
        /* Must do a full switch here to handle all cases.  We use a fake WQE pointer, as the POW does
        ** not access this memory.  The WQE pointer and group are only used if this work is descheduled,
        ** which is not supported by the cvmx_pko_send_packet_prepare/cvmx_pko_send_packet_finish combination.
        ** Note that this is a special case in which these fake values can be used - this is not a general technique.
        */
        uint32_t tag = CVMX_TAG_SW_BITS_INTERNAL << CVMX_TAG_SW_SHIFT | CVMX_TAG_SUBGROUP_PKO  << CVMX_TAG_SUBGROUP_SHIFT | (CVMX_TAG_SUBGROUP_MASK & queue);
        cvmx_pow_tag_sw_full((cvmx_wqe_t *)cvmx_phys_to_ptr(0x80), tag, CVMX_POW_TAG_TYPE_ATOMIC, 0);
#if CVMX_PKO_USE_FAU_FOR_OUTPUT_QUEUES
        /* start a tagwait FAU operation for command buffer offset */
        cvmx_fau_async_tagwait_fetch_and_add64(CVMX_SCR_SCRATCH, (cvmx_fau_reg_64_t)(CVMX_FAU_REG_OQ_ADDR_INDEX + 8 * queue), 2);
    }
    else
    {
        cvmx_fau_async_fetch_and_add64(CVMX_SCR_SCRATCH, (cvmx_fau_reg_64_t)(CVMX_FAU_REG_OQ_ADDR_INDEX + 8 * queue), 2);
#endif
    }
#endif
}


/**
 * Complete packet output. cvmx_pko_send_packet_prepare() must be called exactly once before this,
 * and the same parameters must be passed to both cvmx_pko_send_packet_prepare() and
 * cvmx_pko_send_packet_finish().
 *
 * @param port   Port to send it on
 * @param queue  Queue to use
 * @param pko_command
 *               PKO HW command word
 * @param packet Packet to send
 * @param use_locking
 *               CVMX_PKO_LOCK_NONE, CVMX_PKO_LOCK_ATOMIC_TAG, or CVMX_PKO_LOCK_CMD_QUEUE
 *
 * @return returns CVMX_PKO_SUCCESS on success, or error code on failure of output
 */
static inline cvmx_pko_status_t cvmx_pko_send_packet_finish(uint64_t port, uint64_t queue,
                                        cvmx_pko_command_word0_t pko_command,
                                        cvmx_buf_ptr_t packet, cvmx_pko_lock_t use_locking)
{
#if CVMX_PKO_USE_FAU_FOR_OUTPUT_QUEUES
    uint64_t *cur_oq_ptr;
    uint64_t oq_index;
    uint64_t hw_newbuf;
    uint64_t *p_newbuf;

    CVMX_SYNCWS;  /* flush any pending writes before entering critical region, avoid stalling on SYNCW inside critical region */

#ifdef CVMX_PKO_LOCKLESS_OPERATION

    /* Read and increment queue index */
    oq_index = cvmx_pko_update_queue_index(queue, 2);

#else
    if (use_locking)
    {
        /* Spin while waiting for tag to complete so core can be interrupted. */
        cvmx_pow_tag_sw_wait();
    }
    CVMX_SYNCIOBDMA;  /* wait for index fetch&add and the tag switch response to be in scratch */

    /* Start of critical section.  The switch to atomic has completed.  The critical
    ** section ends when the main program either switches to another tag or
    ** gets the next piece of work.
    */

    /* FAU tagwait timeouts are disabled, and we wait for the tag completion before continuing, so
    ** we always get a valid FAU response.
    */
    oq_index = cvmx_scratch_read64(CVMX_SCR_SCRATCH);
#endif

    /* Buffer pointer is in upper 64 - CVMX_PKO_INDEX_BITS bits,
    ** index is in lower CVMX_PKO_INDEX_BITS bits.
    */
    cur_oq_ptr = (uint64_t*)cvmx_phys_to_ptr(oq_index >> CVMX_PKO_INDEX_BITS);
    oq_index &= CVMX_PKO_INDEX_MASK;
    cur_oq_ptr += oq_index;

    if (cvmx_likely(oq_index < ((CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE/8) - 3 - CVMX_PKO_COMMAND_BUFFER_SIZE_ADJUST)))
    {
        cur_oq_ptr[0] = pko_command.u64;
        cur_oq_ptr[1] = packet.u64;

        cvmx_pko_doorbell(port, queue, 2);
        return CVMX_PKO_SUCCESS;

    }

    /* We need to allocate new buffer. Allocate new buffer from FPA */
    hw_newbuf = cvmx_scratch_read64(CVMX_SCR_OQ_BUF_PRE_ALLOC);
#ifdef PKO_LOCKLESS_OPERATION
    if (cvmx_unlikely(!hw_newbuf))
    {
        /* Async buffer allocation was still pending, force sync and re-read */
        CVMX_SYNCIOBDMA;
        hw_newbuf = cvmx_scratch_read64(CVMX_SCR_OQ_BUF_PRE_ALLOC);
    }
#endif

    p_newbuf = (uint64_t*)cvmx_phys_to_ptr(hw_newbuf);

    /* Request new oq buffer - SYNCIOBDMA for critical section will ensure completion before use */
    cvmx_scratch_write64(CVMX_SCR_OQ_BUF_PRE_ALLOC, 0);
    cvmx_fpa_async_alloc(CVMX_SCR_OQ_BUF_PRE_ALLOC, CVMX_FPA_OUTPUT_BUFFER_POOL);

    if (cvmx_unlikely(!hw_newbuf))
    {
        /* We don't have room, so decrement our index, and return error. */
        cvmx_pko_update_queue_index(queue, -2);
        return CVMX_PKO_NO_MEMORY;
    }

    /* we either have 2 or 3 words left */
    cur_oq_ptr[0] = pko_command.u64;

    /* Since we set the command size word to the pool size minus one, the
        buffer will always end with 3 words. This is faster than the split
        case. The "if" statement is still here in case someone begins using
        the three word command format. At that time this code will work, just
        be less optimal */
    if (cvmx_unlikely(oq_index >= (CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE/8) - 3))
    {
        /* 2 words left, spills into new buf */
        p_newbuf[0] = packet.u64;
        cur_oq_ptr[1] = hw_newbuf;
        oq_index = 1;
    }
    else
    {
        /* 3 words left, fills current buffer exactly */
        cur_oq_ptr[1] = packet.u64;
        cur_oq_ptr[2] = hw_newbuf;
        oq_index = 0;
    }

    /* Set new buffer pointer and index */
    cvmx_pko_set_queue_index(queue, (hw_newbuf << CVMX_PKO_INDEX_BITS) | oq_index);
    /* Read it and wait for response to ensure that the index is updated within critical section */
    cvmx_pko_update_queue_index(queue, 0);


    /* Hardware will free the previous buffer when it is finished with it */
    cvmx_pko_doorbell(port, queue, 2);
    return CVMX_PKO_SUCCESS;
#else
    cvmx_cmd_queue_result_t result;
    if (use_locking == CVMX_PKO_LOCK_ATOMIC_TAG)
        cvmx_pow_tag_sw_wait();
    result = cvmx_cmd_queue_write2(CVMX_CMD_QUEUE_PKO(queue),
                                   (use_locking == CVMX_PKO_LOCK_CMD_QUEUE),
                                   pko_command.u64,
                                   packet.u64);
    if (cvmx_likely(result == CVMX_CMD_QUEUE_SUCCESS))
    {
        cvmx_pko_doorbell(port, queue, 2);
        return CVMX_PKO_SUCCESS;
    }
    else if ((result == CVMX_CMD_QUEUE_NO_MEMORY) || (result == CVMX_CMD_QUEUE_FULL))
    {
        return CVMX_PKO_NO_MEMORY;
    }
    else
    {
        return CVMX_PKO_INVALID_QUEUE;
    }
#endif
}


/**
 * Complete packet output. cvmx_pko_send_packet_prepare() must be called exactly once before this,
 * and the same parameters must be passed to both cvmx_pko_send_packet_prepare() and
 * cvmx_pko_send_packet_finish().
 *
 * @param port   Port to send it on
 * @param queue  Queue to use
 * @param pko_command
 *               PKO HW command word
 * @param packet Packet to send
 * @param addr   Plysical address of a work queue entry or physical address to zero on complete.
 * @param use_locking
 *               CVMX_PKO_LOCK_NONE, CVMX_PKO_LOCK_ATOMIC_TAG, or CVMX_PKO_LOCK_CMD_QUEUE
 *
 * @return returns CVMX_PKO_SUCCESS on success, or error code on failure of output
 */
static inline cvmx_pko_status_t cvmx_pko_send_packet_finish3(uint64_t port, uint64_t queue,
                                        cvmx_pko_command_word0_t pko_command,
                                        cvmx_buf_ptr_t packet, uint64_t addr, cvmx_pko_lock_t use_locking)
{
#if CVMX_PKO_USE_FAU_FOR_OUTPUT_QUEUES
    #warning Implement cvmx_pko_send_packet_finish3
    return CVMX_PKO_NO_MEMORY;
#else
    cvmx_cmd_queue_result_t result;
    if (use_locking == CVMX_PKO_LOCK_ATOMIC_TAG)
        cvmx_pow_tag_sw_wait();
    result = cvmx_cmd_queue_write3(CVMX_CMD_QUEUE_PKO(queue),
                                   (use_locking == CVMX_PKO_LOCK_CMD_QUEUE),
                                   pko_command.u64,
                                   packet.u64,
                                   addr);
    if (cvmx_likely(result == CVMX_CMD_QUEUE_SUCCESS))
    {
        cvmx_pko_doorbell(port, queue, 3);
        return CVMX_PKO_SUCCESS;
    }
    else if ((result == CVMX_CMD_QUEUE_NO_MEMORY) || (result == CVMX_CMD_QUEUE_FULL))
    {
        return CVMX_PKO_NO_MEMORY;
    }
    else
    {
        return CVMX_PKO_INVALID_QUEUE;
    }
#endif
}

/**
 * Return the pko output queue associated with a port and a specific core.
 * In normal mode (PKO lockless operation is disabled), the value returned
 * is the base queue.
 *
 * @param port   Port number
 * @param core   Core to get queue for
 *
 * @return Core-specific output queue
 */
static inline int cvmx_pko_get_base_queue_per_core(int port, int core)
{
#ifndef CVMX_HELPER_PKO_MAX_PORTS_INTERFACE0
    #define CVMX_HELPER_PKO_MAX_PORTS_INTERFACE0 16
#endif
#ifndef CVMX_HELPER_PKO_MAX_PORTS_INTERFACE1
    #define CVMX_HELPER_PKO_MAX_PORTS_INTERFACE1 16
#endif
    if (port < CVMX_PKO_MAX_PORTS_INTERFACE0)
        return port * CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 + core;
    else if (port >=16 && port < 16 + CVMX_PKO_MAX_PORTS_INTERFACE1)
        return CVMX_PKO_MAX_PORTS_INTERFACE0 * CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 +
	       (port-16) * CVMX_PKO_QUEUES_PER_PORT_INTERFACE1 + core;
    else if ((port >= 32) && (port < 36))
        return CVMX_PKO_MAX_PORTS_INTERFACE0 * CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 +
               CVMX_PKO_MAX_PORTS_INTERFACE1 * CVMX_PKO_QUEUES_PER_PORT_INTERFACE1 +
               (port-32) * CVMX_PKO_QUEUES_PER_PORT_PCI;
    else if ((port >= 36) && (port < 40))
        return CVMX_PKO_MAX_PORTS_INTERFACE0 * CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 +
               CVMX_PKO_MAX_PORTS_INTERFACE1 * CVMX_PKO_QUEUES_PER_PORT_INTERFACE1 +
               4 * CVMX_PKO_QUEUES_PER_PORT_PCI +
               (port-36) * CVMX_PKO_QUEUES_PER_PORT_LOOP;
    else
        /* Given the limit on the number of ports we can map to
         * CVMX_MAX_OUTPUT_QUEUES_STATIC queues (currently 256,
         * divided among all cores), the remaining unmapped ports
         * are assigned an illegal queue number */
        return CVMX_PKO_ILLEGAL_QUEUE;
}

/**
 * For a given port number, return the base pko output queue
 * for the port.
 *
 * @param port   Port number
 * @return Base output queue
 */
static inline int cvmx_pko_get_base_queue(int port)
{
#ifdef CVMX_PKO_LOCKLESS_OPERATION
    return cvmx_pko_get_base_queue_per_core(port, cvmx_get_core_num());
#else
    return cvmx_pko_get_base_queue_per_core(port, 0);
#endif
}

/**
 * For a given port number, return the number of pko output queues.
 *
 * @param port   Port number
 * @return Number of output queues
 */
static inline int cvmx_pko_get_num_queues(int port)
{
    if (port < 16)
        return CVMX_PKO_QUEUES_PER_PORT_INTERFACE0;
    else if (port<32)
        return CVMX_PKO_QUEUES_PER_PORT_INTERFACE1;
    else if (port<36)
        return CVMX_PKO_QUEUES_PER_PORT_PCI;
    else if (port<40)
        return CVMX_PKO_QUEUES_PER_PORT_LOOP;
    else
        return 0;
}

/**
 * Get the status counters for a port.
 *
 * @param port_num Port number to get statistics for.
 * @param clear    Set to 1 to clear the counters after they are read
 * @param status   Where to put the results.
 */
static inline void cvmx_pko_get_port_status(uint64_t port_num, uint64_t clear, cvmx_pko_port_status_t *status)
{
    cvmx_pko_reg_read_idx_t pko_reg_read_idx;
    cvmx_pko_mem_count0_t pko_mem_count0;
    cvmx_pko_mem_count1_t pko_mem_count1;

    pko_reg_read_idx.u64 = 0;
    pko_reg_read_idx.s.index = port_num;
    cvmx_write_csr(CVMX_PKO_REG_READ_IDX, pko_reg_read_idx.u64);

    pko_mem_count0.u64 = cvmx_read_csr(CVMX_PKO_MEM_COUNT0);
    status->packets = pko_mem_count0.s.count;
    if (clear)
    {
        pko_mem_count0.s.count = port_num;
        cvmx_write_csr(CVMX_PKO_MEM_COUNT0, pko_mem_count0.u64);
    }

    pko_mem_count1.u64 = cvmx_read_csr(CVMX_PKO_MEM_COUNT1);
    status->octets = pko_mem_count1.s.count;
    if (clear)
    {
        pko_mem_count1.s.count = port_num;
        cvmx_write_csr(CVMX_PKO_MEM_COUNT1, pko_mem_count1.u64);
    }

    if (OCTEON_IS_MODEL(OCTEON_CN3XXX))
    {
        cvmx_pko_mem_debug9_t debug9;
        pko_reg_read_idx.s.index = cvmx_pko_get_base_queue(port_num);
        cvmx_write_csr(CVMX_PKO_REG_READ_IDX, pko_reg_read_idx.u64);
        debug9.u64 = cvmx_read_csr(CVMX_PKO_MEM_DEBUG9);
        status->doorbell = debug9.cn38xx.doorbell;
    }
    else
    {
        cvmx_pko_mem_debug8_t debug8;
        pko_reg_read_idx.s.index = cvmx_pko_get_base_queue(port_num);
        cvmx_write_csr(CVMX_PKO_REG_READ_IDX, pko_reg_read_idx.u64);
        debug8.u64 = cvmx_read_csr(CVMX_PKO_MEM_DEBUG8);
        status->doorbell = debug8.cn58xx.doorbell;
    }
}


/**
 * Rate limit a PKO port to a max packets/sec. This function is only
 * supported on CN57XX, CN56XX, CN55XX, and CN54XX.
 *
 * @param port      Port to rate limit
 * @param packets_s Maximum packet/sec
 * @param burst     Maximum number of packets to burst in a row before rate
 *                  limiting cuts in.
 *
 * @return Zero on success, negative on failure
 */
extern int cvmx_pko_rate_limit_packets(int port, int packets_s, int burst);

/**
 * Rate limit a PKO port to a max bits/sec. This function is only
 * supported on CN57XX, CN56XX, CN55XX, and CN54XX.
 *
 * @param port   Port to rate limit
 * @param bits_s PKO rate limit in bits/sec
 * @param burst  Maximum number of bits to burst before rate
 *               limiting cuts in.
 *
 * @return Zero on success, negative on failure
 */
extern int cvmx_pko_rate_limit_bits(int port, uint64_t bits_s, int burst);

#endif /* CVMX_ENABLE_PKO_FUNCTIONS */

#ifdef	__cplusplus
}
#endif

#endif   /* __CVMX_PKO_H__ */
