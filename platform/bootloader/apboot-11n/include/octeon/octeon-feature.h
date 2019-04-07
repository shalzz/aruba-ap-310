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
 * File defining checks for different Octeon features.
 *
 * <hr>$Revision: 30468 $<hr>
 */

#ifndef __OCTEON_FEATURE_H__
#define __OCTEON_FEATURE_H__

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum
{
    OCTEON_FEATURE_SAAD,        /* Octeon models in the CN5XXX family and higher support atomic add instructions to memory (saa/saad) */
    OCTEON_FEATURE_ZIP,         /* Does this Octeon support the ZIP offload engine? */
    OCTEON_FEATURE_CRYPTO,      /* Does this Octeon support crypto acceleration using COP2? */
    OCTEON_FEATURE_PCIE,        /* Does this Octeon support PCI express? */
    OCTEON_FEATURE_KEY_MEMORY,  /* Some Octeon models support internal memory for storing cryptographic keys */
    OCTEON_FEATURE_LED_CONTROLLER, /* Octeon has a LED controller for banks of external LEDs */
    OCTEON_FEATURE_TRA,         /* Octeon has a trace buffer */
} octeon_feature_t;

#ifdef __mips__
static inline int cvmx_fuse_read(int fuse);
#endif

/**
 * Determine if the current Octeon supports a specific feature. These
 * checks have been optimized to be fairly quick, but they should still
 * be kept out of fast path code.
 *
 * @param feature Feature to check for. This should always be a constant so the
 *                compiler can remove the switch statement through optimization.
 *
 * @return Non zero if the feature exists. Zero if the feature does not
 *         exist.
 */
static inline int octeon_has_feature(octeon_feature_t feature)
{
#ifdef __mips__
    switch (feature)
    {
        case OCTEON_FEATURE_SAAD:
            return !OCTEON_IS_MODEL(OCTEON_CN3XXX);

        case OCTEON_FEATURE_ZIP:
            if (OCTEON_IS_MODEL(OCTEON_CN30XX) || OCTEON_IS_MODEL(OCTEON_CN50XX) || OCTEON_IS_MODEL(OCTEON_CN52XX))
                return 0;
            else if (OCTEON_IS_MODEL(OCTEON_CN38XX_PASS1))
                return 1;
            else
                return (!cvmx_fuse_read(121));

        case OCTEON_FEATURE_CRYPTO:
            return (!cvmx_fuse_read(90));

        case OCTEON_FEATURE_PCIE:
            return (OCTEON_IS_MODEL(OCTEON_CN56XX) || OCTEON_IS_MODEL(OCTEON_CN52XX));

        case OCTEON_FEATURE_KEY_MEMORY:
        case OCTEON_FEATURE_LED_CONTROLLER:
            return (OCTEON_IS_MODEL(OCTEON_CN38XX) || OCTEON_IS_MODEL(OCTEON_CN58XX) || OCTEON_IS_MODEL(OCTEON_CN56XX));
        case OCTEON_FEATURE_TRA:
            return !(OCTEON_IS_MODEL(OCTEON_CN30XX) || OCTEON_IS_MODEL(OCTEON_CN50XX));
    }
#endif
    return 0;
}

#ifdef	__cplusplus
}
#endif

#endif    /* __OCTEON_FEATURE_H__ */
