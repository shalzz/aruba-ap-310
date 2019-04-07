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

#ifndef __OCTEON_HAL_H__
#define __OCTEON_HAL_H__

/* Check for N32 ABI, defined for 32-bit Simple Exec applications
   and Linux N32 ABI.*/
#if (defined _ABIN32 && _MIPS_SIM == _ABIN32)
#define CVMX_ABI_N32
/* Check for N64 ABI, defined for 64-bit Linux toolchain. */
#elif (defined _ABI64 && _MIPS_SIM == _ABI64)
#define CVMX_ABI_N64
/* Check for O32 ABI, defined for Linux 032 ABI, not supported yet. */
#elif (defined _ABIO32 && _MIPS_SIM == _ABIO32)
#define CVMX_ABI_O32
/* Check for EABI ABI, defined for 64-bit Simple Exec applications. */
#else
#define CVMX_ABI_EABI
#endif

#ifndef __BYTE_ORDER
    #if defined(__BIG_ENDIAN) && !defined(__LITTLE_ENDIAN)
        #define __BYTE_ORDER __BIG_ENDIAN
    #elif !defined(__BIG_ENDIAN) && defined(__LITTLE_ENDIAN)
        #define __BYTE_ORDER __LITTLE_ENDIAN
    #elif !defined(__BIG_ENDIAN) && !defined(__LITTLE_ENDIAN)
        #define __BIG_ENDIAN 4321
        #define __BYTE_ORDER __BIG_ENDIAN
    #else
        #error Unable to determine Endian mode
    #endif
#endif


#include "octeon_csr.h"


/* PCI IO SPACE DID with proper swap (1) */
#define OCTEON_PCI_IOSPACE_BASE     0x80011a0400000000ull

#define CVMX_TMP_STR(x) CVMX_TMP_STR2(x)
#define CVMX_TMP_STR2(x) #x

#define CVMX_OCT_DID_PCI 3ULL
#define CVMX_OCT_DID_TAG_SWTAG      CVMX_FULL_DID(CVMX_OCT_DID_TAG,0ULL)
#define CVMX_OCT_DID_TAG 12ULL
#define CVMX_OCT_DID_PKT_SEND       CVMX_FULL_DID(CVMX_OCT_DID_PKT,2ULL)
#define CVMX_OCT_DID_PKT 10ULL

#define CVMX_SYNCW  OCTEON_SYNCW
#define CVMX_SYNCIOBDMA asm volatile ("synciobdma" : : :"memory")

#ifndef TRUE
#define FALSE   0
#define TRUE    (!(FALSE))
#endif
/* These macros for used by 32 bit applications */
#define CVMX_MIPS32_SPACE_KSEG0 1
#define CVMX_ADD_SEG32(segment, add)          (((int64_t)(segment << 31)) | (add))

/* Currently all IOs are performed using XKPHYS addressing. Linux uses the
    CvmMemCtl register to enable XKPHYS addressing to IO space from user mode.
    Future OSes may need to change the upper bits of IO addresses. The
    following define controls the upper two bits for all IO addresses generated
    by the simple executive library */
#define CVMX_IO_SEG CVMX_MIPS_SPACE_XKPHYS
/* These macros simplify the process of creating common IO addresses */
#define CVMX_ADD_SEG(segment, add)          ((((uint64_t)segment) << 62) | (add))
#define CVMX_ADD_IO_SEG(add)                CVMX_ADD_SEG(CVMX_IO_SEG, (add))
#define CVMX_ADDR_DIDSPACE(did)             (((CVMX_IO_SEG) << 22) | ((1ULL) << 8) | (did))
#define CVMX_ADDR_DID(did)                  (CVMX_ADDR_DIDSPACE(did) << 40)
#define CVMX_FULL_DID(did,subdid)           (((did) << 3) | (subdid))

void octeon_write64(uint64_t csr_addr, uint64_t val);
uint64_t octeon_read64(uint64_t csr_addr);

#define CVMX_POP(result, input) asm ("pop %[rd],%[rs]" : [rd] "=d" (result) : [rs] "d" (input))

#define CVMX_PREPARE_FOR_STORE(address, offset) asm volatile ("pref 30, " CVMX_TMP_STR(offset) "(%[rbase])" : : [rbase] "d" (address) )


/**************************************************************************/


/* The following #if controls the definition of the macro
    CVMX_BUILD_WRITE64. This macro is used to build a store operation to
    a full 64bit address. With a 64bit ABI, this can be done with a simple
    pointer access. 32bit ABIs require more complicated assembly */
#if defined(CVMX_ABI_N64) || defined(CVMX_ABI_EABI)

/* We have a full 64bit ABI. Writing to a 64bit address can be done with
    a simple volatile pointer */
#define CVMX_BUILD_WRITE64(TYPE, ST)                                    \
static inline void cvmx_write64_##TYPE(uint64_t addr, TYPE##_t val)     \
{                                                                       \
    *(volatile TYPE##_t *)(unsigned long)addr = val;                    \
}

#elif defined(CVMX_ABI_N32)

/* The N32 ABI passes all 64bit quantities in a single register, so it is
    possible to use the arguments directly. We have to use inline assembly
    for the actual store since a pointer would truncate the address */
#define CVMX_BUILD_WRITE64(TYPE, ST)                                    \
static inline void cvmx_write64_##TYPE(uint64_t addr, TYPE##_t val)     \
{                                                                       \
    asm volatile (ST " %[v], 0(%[c])" ::[v] "r" (val), [c] "r" (addr)); \
}

#elif defined(CVMX_ABI_O32)

/* Ok, now the ugly stuff starts. O32 splits 64bit quantities into two
    separate registers. Assembly must be used to put them back together
    before they're used. What should be a simple store becomes a
    convoluted mess of shifts and ors */
#define CVMX_BUILD_WRITE64(TYPE, ST)                                    \
static inline void cvmx_write64_##TYPE(uint64_t csr_addr, TYPE##_t val) \
{                                                                       \
    if (sizeof(TYPE##_t) == 8)                                          \
    {                                                                   \
        uint32_t csr_addrh = csr_addr>>32;                              \
        uint32_t csr_addrl = csr_addr;                                  \
        uint32_t valh = (uint64_t)val>>32;                              \
        uint32_t vall = val;                                            \
        uint32_t tmp1;                                                  \
        uint32_t tmp2;                                                  \
        uint32_t tmp3;                                                  \
                                                                        \
        asm volatile (                                                  \
            ".set push\n"                                             \
            ".set mips64\n"                                             \
            "dsll   %[tmp1], %[valh], 32\n"                             \
            "dsll   %[tmp2], %[csrh], 32\n"                             \
            "dsll   %[tmp3], %[vall], 32\n"                             \
            "dsrl   %[tmp3], %[tmp3], 32\n"                             \
            "or     %[tmp1], %[tmp1], %[tmp3]\n"                        \
            "dsll   %[tmp3], %[csrl], 32\n"                             \
            "dsrl   %[tmp3], %[tmp3], 32\n"                             \
            "or     %[tmp2], %[tmp2], %[tmp3]\n"                        \
            ST "    %[tmp1], 0(%[tmp2])\n"                              \
            ".set pop\n"                                             \
            : [tmp1] "=&r" (tmp1), [tmp2] "=&r" (tmp2), [tmp3] "=&r" (tmp3)\
            : [valh] "r" (valh), [vall] "r" (vall),                     \
              [csrh] "r" (csr_addrh), [csrl] "r" (csr_addrl)            \
        );                                                              \
    }                                                                   \
    else                                                                \
    {                                                                   \
        uint32_t csr_addrh = csr_addr>>32;                              \
        uint32_t csr_addrl = csr_addr;                                  \
        uint32_t tmp1;                                                  \
        uint32_t tmp2;                                                  \
                                                                        \
        asm volatile (                                                  \
            ".set push\n"                                             \
            ".set mips64\n"                                             \
            "dsll   %[tmp1], %[csrh], 32\n"                             \
            "dsll   %[tmp2], %[csrl], 32\n"                             \
            "dsrl   %[tmp2], %[tmp2], 32\n"                             \
            "or     %[tmp1], %[tmp1], %[tmp2]\n"                        \
            ST "    %[val], 0(%[tmp1])\n"                               \
            ".set pop\n"                                             \
            : [tmp1] "=&r" (tmp1), [tmp2] "=&r" (tmp2)                  \
            : [val] "r" (val), [csrh] "r" (csr_addrh),                  \
              [csrl] "r" (csr_addrl)                                    \
        );                                                              \
    }                                                                   \
}

#else

/* cvmx-abi.h didn't recognize the ABI. Force the compile to fail. */
#error: Unsupported ABI

#endif

/* The following #if controls the definition of the macro
    CVMX_BUILD_READ64. This macro is used to build a load operation from
    a full 64bit address. With a 64bit ABI, this can be done with a simple
    pointer access. 32bit ABIs require more complicated assembly */
#if defined(CVMX_ABI_N64) || defined(CVMX_ABI_EABI)

/* We have a full 64bit ABI. Writing to a 64bit address can be done with
    a simple volatile pointer */
#define CVMX_BUILD_READ64(TYPE, LT)                                     \
static inline TYPE##_t cvmx_read64_##TYPE(uint64_t addr)                \
{                                                                       \
    return *(volatile TYPE##_t *)(unsigned long)addr;                   \
}

#elif defined(CVMX_ABI_N32)

/* The N32 ABI passes all 64bit quantities in a single register, so it is
    possible to use the arguments directly. We have to use inline assembly
    for the actual store since a pointer would truncate the address */
#define CVMX_BUILD_READ64(TYPE, LT)                                     \
static inline TYPE##_t cvmx_read64_##TYPE(uint64_t addr)                \
{                                                                       \
    TYPE##_t val;                                                       \
    asm volatile (LT " %[v], 0(%[c])": [v] "=r" (val) : [c] "r" (addr));\
    return val;                                                         \
}

#elif defined(CVMX_ABI_O32)

/* Ok, now the ugly stuff starts. O32 splits 64bit quantities into two
    separate registers. Assembly must be used to put them back together
    before they're used. What should be a simple load becomes a
    convoluted mess of shifts and ors */
#define CVMX_BUILD_READ64(TYPE, LT)                                     \
static inline TYPE##_t cvmx_read64_##TYPE(uint64_t csr_addr)            \
{                                                                       \
    if (sizeof(TYPE##_t) == 8)                                          \
    {                                                                   \
        uint32_t csr_addrh = csr_addr>>32;                              \
        uint32_t csr_addrl = csr_addr;                                  \
        uint32_t valh;                                                  \
        uint32_t vall;                                                  \
                                                                        \
        asm volatile (                                                  \
            ".set push\n"                                             \
            ".set mips64\n"                                             \
            "dsll   %[valh], %[csrh], 32\n"                             \
            "dsll   %[vall], %[csrl], 32\n"                             \
            "dsrl   %[vall], %[vall], 32\n"                             \
            "or     %[valh], %[valh], %[vall]\n"                        \
            LT "    %[vall], 0(%[valh])\n"                              \
            "dsrl   %[valh], %[vall], 32\n"                             \
            "dsll   %[vall], %[vall], 32\n"                             \
            "dsrl   %[vall], %[vall], 32\n"                             \
            ".set pop\n"                                             \
            : [valh] "=&r" (valh), [vall] "=&r" (vall)                  \
            : [csrh] "r" (csr_addrh), [csrl] "r" (csr_addrl)            \
        );                                                              \
        return ((uint64_t)valh<<32) | vall;                             \
    }                                                                   \
    else                                                                \
    {                                                                   \
        uint32_t csr_addrh = csr_addr>>32;                              \
        uint32_t csr_addrl = csr_addr;                                  \
        TYPE##_t val;                                                   \
        uint32_t tmp;                                                   \
                                                                        \
        asm volatile (                                                  \
            ".set push\n"                                             \
            ".set mips64\n"                                             \
            "dsll   %[val], %[csrh], 32\n"                              \
            "dsll   %[tmp], %[csrl], 32\n"                              \
            "dsrl   %[tmp], %[tmp], 32\n"                               \
            "or     %[val], %[val], %[tmp]\n"                           \
            LT "    %[val], 0(%[val])\n"                                \
            ".set pop\n"                                             \
            : [val] "=&r" (val), [tmp] "=&r" (tmp)                      \
            : [csrh] "r" (csr_addrh), [csrl] "r" (csr_addrl)            \
        );                                                              \
        return val;                                                     \
    }                                                                   \
}
#else

/* cvmx-abi.h didn't recognize the ABI. Force the compile to fail. */
#error: Unsupported ABI

#endif

/* The following defines 8 functions for writing to a 64bit address. Each
    takes two arguments, the address and the value to write.
    cvmx_write64_int64      cvmx_write64_uint64
    cvmx_write64_int32      cvmx_write64_uint32
    cvmx_write64_int16      cvmx_write64_uint16
    cvmx_write64_int8       cvmx_write64_uint8 */
CVMX_BUILD_WRITE64(int64, "sd");
CVMX_BUILD_WRITE64(int32, "sw");
CVMX_BUILD_WRITE64(int16, "sh");
CVMX_BUILD_WRITE64(int8, "sb");
CVMX_BUILD_WRITE64(uint64, "sd");
CVMX_BUILD_WRITE64(uint32, "sw");
CVMX_BUILD_WRITE64(uint16, "sh");
CVMX_BUILD_WRITE64(uint8, "sb");
#define cvmx_write64 cvmx_write64_uint64

/* The following defines 8 functions for reading from a 64bit address. Each
    takes the address as the only argument
    cvmx_read64_int64       cvmx_read64_uint64
    cvmx_read64_int32       cvmx_read64_uint32
    cvmx_read64_int16       cvmx_read64_uint16
    cvmx_read64_int8        cvmx_read64_uint8 */
CVMX_BUILD_READ64(int64, "ld");
CVMX_BUILD_READ64(int32, "lw");
CVMX_BUILD_READ64(int16, "lh");
CVMX_BUILD_READ64(int8, "lb");
CVMX_BUILD_READ64(uint64, "ld");
CVMX_BUILD_READ64(uint32, "lwu");
CVMX_BUILD_READ64(uint16, "lhu");
CVMX_BUILD_READ64(uint8, "lbu");
#define cvmx_read64 cvmx_read64_uint64


/**************************************************************************/










/* Make aliases for cvmx read/write functions */
#define cvmx_write_io octeon_write64
#define cvmx_read_io octeon_read64

#define cvmx_read_csr cvmx_read64
#define cvmx_get_cycle  octeon_get_cycles

#define CVMX_HW_BASE        ((volatile uint64_t *) 0L)
/* these counts are all in words relative to zero */
#define CVMX_REG_OFFSET     (-4*1024ll)            // local scratchpad register base
#define CVMX_IOBDMA_OFFSET  (-3*1024ll)
#define CVMX_IOBDMA_SEP     16 /* 128 bytes */
#define CVMX_CSR_OFFSET     (-2*1024ll)
#define CVMX_CSR_SEP        1 /* 8 bytes */
// IOBDMA addresses
#define CVMX_IOBDMA_SENDSINGLE (CVMX_IOBDMA_OFFSET+(CVMX_ADD_WIN_DMA_SENDSINGLE*CVMX_IOBDMA_SEP))


/*****************************************************************************/
#ifndef __CVMX_H__
static inline void cvmx_write_csr(uint64_t csr_addr, uint64_t val)
{
#if 0
    simprintf("CSR WRITE: 0x%llx <- 0x%llx\n", csr_addr & ~(1ULL<<63), val);
#endif

    cvmx_write64(csr_addr, val);

    /* Perform an immediate read after every write to an RSL register to force
        the write to complete. It doesn't matter what RSL read we do, so we
        choose CVMX_MIO_BOOT_BIST_STAT because it is fast and harmless */
    if ((csr_addr >> 40) == (0x800118))
        cvmx_read64(CVMX_MIO_BOOT_BIST_STAT);
}
static inline void cvmx_send_single(uint64_t data)
{
    octeon_write64((uint64_t)(CVMX_IOBDMA_SENDSINGLE * (long long)8), data);
}
static inline void cvmx_wait(uint64_t cycles)
{
    uint64_t done = cvmx_get_cycle() + cycles;

    while (cvmx_get_cycle() < done)
    {
        /* Spin */
    }
}
#endif



/* Defines to represent the different versions of Octeon.  */
#include "octeon-model.h"
/* Provide alias for __octeon_is_model_runtime__ */
#define octeon_is_model(x)     __octeon_is_model_runtime__(x)


/*****************************************************************************/
#ifndef __CVMX_IPD_H__
static inline void cvmx_ipd_enable(void)
{
    cvmx_ipd_ctl_status_t ipd_reg;
    ipd_reg.u64 = cvmx_read_csr(CVMX_IPD_CTL_STATUS);
    ipd_reg.s.ipd_en = TRUE;
    cvmx_write_csr(CVMX_IPD_CTL_STATUS, ipd_reg.u64);
}
static inline void cvmx_ipd_config(uint64_t mbuff_size,
                                   uint64_t first_mbuff_skip,
                                   uint64_t not_first_mbuff_skip,
                                   uint64_t first_back,
                                   uint64_t second_back,
                                   uint64_t wqe_fpa_pool,
                                   cvmx_ipd_mode_t cache_mode,
                                   uint64_t back_pres_enable_flag
                                  )
{
    cvmx_ipd_mbuff_first_skip_t first_skip;
    cvmx_ipd_mbuff_not_first_skip_t not_first_skip;
    cvmx_ipd_mbuff_size_t size;
    cvmx_ipd_first_next_ptr_back_t first_back_struct;
    cvmx_ipd_second_next_ptr_back_t second_back_struct;
    cvmx_ipd_wqe_fpa_pool_t wqe_pool;
    cvmx_ipd_ctl_status_t ipd_ctl_reg;

    first_skip.u64 = 0;
    first_skip.s.skip_sz = first_mbuff_skip;
    cvmx_write_csr(CVMX_IPD_1ST_MBUFF_SKIP, first_skip.u64);

    not_first_skip.u64 = 0;
    not_first_skip.s.skip_sz = not_first_mbuff_skip;
    cvmx_write_csr(CVMX_IPD_NOT_1ST_MBUFF_SKIP, not_first_skip.u64);

    size.u64 = 0;
    size.s.mb_size = mbuff_size;
    cvmx_write_csr(CVMX_IPD_PACKET_MBUFF_SIZE, size.u64);

    first_back_struct.u64 = 0;
    first_back_struct.s.back = first_back;
    cvmx_write_csr(CVMX_IPD_1st_NEXT_PTR_BACK, first_back_struct.u64);

    second_back_struct.u64 = 0;
    second_back_struct.s.back = second_back;
    cvmx_write_csr(CVMX_IPD_2nd_NEXT_PTR_BACK,second_back_struct.u64);

    wqe_pool.u64 = 0;
    wqe_pool.s.wqe_pool = wqe_fpa_pool;
    cvmx_write_csr(CVMX_IPD_WQE_FPA_QUEUE, wqe_pool.u64);

    ipd_ctl_reg.u64 = 0;
    ipd_ctl_reg.s.opc_mode = cache_mode;
    ipd_ctl_reg.s.pbp_en = back_pres_enable_flag;
    cvmx_write_csr(CVMX_IPD_CTL_STATUS, ipd_ctl_reg.u64);

}
#endif

/*****************************************************************************/
#ifndef __CVMX_PKO_H__
static inline int cvmx_pko_get_base_queue(int port)
{
    if (port < 16)
        return port * CVMX_PKO_QUEUES_PER_PORT_INTERFACE0;
    else if (port<32)
        return 16 * CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 +
                (port-16) * CVMX_PKO_QUEUES_PER_PORT_INTERFACE1;
    else
        return 16 * CVMX_PKO_QUEUES_PER_PORT_INTERFACE0 +
                16 * CVMX_PKO_QUEUES_PER_PORT_INTERFACE1 +
                (port-32) * CVMX_PKO_QUEUES_PER_PORT_PCI;
}
static inline void cvmx_pko_doorbell(uint64_t port, uint64_t queue, uint64_t len)
{
   cvmx_pko_doorbell_address_t ptr;

   ptr.u64          = 0;
   ptr.s.mem_space  = CVMX_IO_SEG;
   ptr.s.did        = CVMX_OCT_DID_PKT_SEND;
   ptr.s.is_io      = 1;
   ptr.s.port       = port;
   ptr.s.queue      = queue;
   CVMX_SYNCW;  /* Need to make sure output queue data is in DRAM before doorbell write */
   cvmx_write_io(ptr.u64, len);
}
#endif


/*****************************************************************************/
#ifndef __CVMX_PIP_H__
static inline void cvmx_pip_config_port(uint64_t port_num,
                                        cvmx_pip_port_cfg_t port_cfg,
                                        cvmx_pip_port_tag_cfg_t port_tag_cfg)
{
    cvmx_write_csr(CVMX_PIP_PRT_CFGX(port_num), port_cfg.u64);
    cvmx_write_csr(CVMX_PIP_PRT_TAGX(port_num), port_tag_cfg.u64);
}
#endif




/*****************************************************************************/
#ifndef __CVMX_POW_H__
static inline cvmx_wqe_t * cvmx_pow_work_request_sync_nocheck(int wait)
{
    cvmx_pow_load_addr_t ptr;
    cvmx_pow_tag_load_resp_t result;

    ptr.u64 = 0;
    ptr.swork.mem_region = CVMX_IO_SEG;
    ptr.swork.is_io = 1;
    ptr.swork.did = CVMX_OCT_DID_TAG_SWTAG;
    ptr.swork.wait = wait;

    result.u64 = cvmx_read_csr(ptr.u64);

    if (result.s_work.no_work)
        return NULL;
    else
        return (cvmx_wqe_t*)cvmx_phys_to_ptr(result.s_work.addr);
}
static inline cvmx_wqe_t * cvmx_pow_work_request_sync(int wait)
{
#if 0
    /* Must not have a switch pending when requesting work */
    cvmx_pow_tag_sw_wait();
#endif
    return(cvmx_pow_work_request_sync_nocheck(wait));

}
#endif


/*****************************************************************************/
#ifndef __CVMX_FPA_H__
static inline void cvmx_fpa_free(void *ptr, uint64_t pool, uint64_t num_cache_lines)
{
#define CVMX_OCT_DID_FPA 5ULL
    cvmx_addr_t newptr;
    newptr.u64 = cvmx_ptr_to_phys(ptr);
    newptr.sfilldidspace.didspace = CVMX_ADDR_DIDSPACE(CVMX_FULL_DID(CVMX_OCT_DID_FPA,pool));
    /* value written is number of cache lines not written back */
    cvmx_write_io(newptr.u64, num_cache_lines);
}
static inline void *cvmx_fpa_alloc(uint64_t pool)
{
    uint64_t address = cvmx_read_csr(CVMX_ADDR_DID(CVMX_FULL_DID(CVMX_OCT_DID_FPA,pool)));
    if (address)
        return cvmx_phys_to_ptr(address);
    else
        return NULL;
}
static inline void cvmx_fpa_enable(void)
{
    int i;
    cvmx_fpa_fpf_marks_t marks;
    for (i=1; i<8; i++)
    {
        marks.u64 = cvmx_read_csr(CVMX_FPA_FPF1_MARKS + (i-1)*8ull);
        marks.s.fpf_wr = 0xe0;
        cvmx_write_csr(CVMX_FPA_FPF1_MARKS + (i-1)*8ull, marks.u64);
    }

    /* Enforce a 10 cycle delay between config and enable */
    cvmx_wait(10);

    cvmx_fpa_ctl_status_t status;
    status.u64 = 0; /* FIXME: CVMX_FPA_CTL_STATUS read is unmodelled */
    status.s.enb = 1;
    cvmx_write_csr(CVMX_FPA_CTL_STATUS, status.u64);
}
#endif



/*****************************************************************************/
#ifndef __CVMX_FAU_H__
#define CVMX_FAU_LOAD_IO_ADDRESS    cvmx_build_io_address(0x1e, 0)
#define CVMX_FAU_BITS_SCRADDR       63,56
#define CVMX_FAU_BITS_LEN           55,48
#define CVMX_FAU_BITS_INEVAL        35,14
#define CVMX_FAU_BITS_TAGWAIT       13,13
#define CVMX_FAU_BITS_NOADD         13,13
#define CVMX_FAU_BITS_SIZE          12,11
#define CVMX_FAU_BITS_REGISTER      10,0
static inline uint64_t cvmx_fau_iobdma_data(uint64_t scraddr, int64_t value, uint64_t tagwait,
                                          cvmx_fau_op_size_t size, uint64_t reg)
{
    return (CVMX_FAU_LOAD_IO_ADDRESS |
                      cvmx_build_bits(CVMX_FAU_BITS_SCRADDR, scraddr>>3) |
                      cvmx_build_bits(CVMX_FAU_BITS_LEN, 1) |
                      cvmx_build_bits(CVMX_FAU_BITS_INEVAL, value) |
                      cvmx_build_bits(CVMX_FAU_BITS_TAGWAIT, tagwait) |
                      cvmx_build_bits(CVMX_FAU_BITS_SIZE, size) |
                      cvmx_build_bits(CVMX_FAU_BITS_REGISTER, reg));
}

static inline uint64_t cvmx_fau_store_address(uint64_t noadd, uint64_t reg)
{
    return (CVMX_ADD_IO_SEG(CVMX_FAU_LOAD_IO_ADDRESS) |
            cvmx_build_bits(CVMX_FAU_BITS_NOADD, noadd) |
            cvmx_build_bits(CVMX_FAU_BITS_REGISTER, reg));
}
static inline void cvmx_fau_atomic_write64(cvmx_fau_reg_64_t reg, int64_t value)
{
    octeon_write64(cvmx_fau_store_address(1, reg), value);
}
static inline void cvmx_fau_async_fetch_and_add64(uint64_t scraddr, cvmx_fau_reg_64_t reg, int64_t value)
{
    cvmx_send_single(cvmx_fau_iobdma_data(scraddr, value, 0, CVMX_FAU_OP_SIZE_64, reg));
}
#endif




#endif /* __OCTEON_HAL_H__  */
