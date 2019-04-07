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
 * File defining different Octeon model IDs and macros to
 * compare them.
 *
 * <hr>$Revision: 33479 $<hr>
 */

#ifndef __OCTEON_MODEL_H__
#define __OCTEON_MODEL_H__

#ifdef	__cplusplus
extern "C" {
#endif


/* NOTE: These must match what is checked in common-config.mk */
/* Defines to represent the different versions of Octeon.  */


/* IMPORTANT: When the default pass is updated for an Octeon Model,
** the corresponding change must also be made in the oct-sim script. */


/* The defines below should be used with the OCTEON_IS_MODEL() macro to
** determine what model of chip the software is running on.  Models ending
** in 'XX' match multiple models (families), while specific models match only
** that model.  If a pass (revision) is specified, then only that revision
** will be matched.  Care should be taken when checking for both specific
** models and families that the specific models are checked for first.
** While these defines are similar to the processor ID, they are not intended
** to be used by anything other that the OCTEON_IS_MODEL framework, and
** the values are subject to change at anytime without notice.
**
** NOTE: only the OCTEON_IS_MODEL() macro/function and the OCTEON_CN* macros
** should be used outside of this file.  All other macros are for internal
** use only, and may change without notice.
*/


/* Flag bits in top byte */
#define OM_IGNORE_REVISION        0x01000000      /* Ignores revision in model checks */
#define OM_IGNORE_SUBMODEL        0x02000000      /* Ignores submodels  */
#define OM_MATCH_PREVIOUS_MODELS  0x04000000      /* Match all models previous than the one specified */

#define OCTEON_CN56XX_PASS1     0x000d0400
#define OCTEON_CN56XX_PASS1_1   0x000d0401
#define OCTEON_CN56XX_PASS2     0x000d0408
#define OCTEON_CN56XX           (OCTEON_CN56XX_PASS1 | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)

/* NOTE: Octeon CN57XX, CN55XX, and CN54XX models are not identifiable using the
    OCTEON_IS_MODEL() functions, but are treated as CN56XX */

#define OCTEON_CN58XX_PASS1     0x000d0300
#define OCTEON_CN58XX_PASS1_1   0x000d0301
#define OCTEON_CN58XX_PASS1_2   0x000d0303
#define OCTEON_CN58XX_PASS2     0x000d0308
#define OCTEON_CN58XX           (OCTEON_CN58XX_PASS1 | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)

#define OCTEON_CN50XX_PASS1     0x000d0600
#define OCTEON_CN50XX           (OCTEON_CN50XX_PASS1 | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)

/* NOTE: Octeon CN5000F model is not identifiable using the OCTEON_IS_MODEL()
    functions, but are treated as CN50XX */

#define OCTEON_CN52XX_PASS1     0x000d0700
#define OCTEON_CN52XX           (OCTEON_CN52XX_PASS1 | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)

#define OCTEON_CN38XX_PASS1     0x000d0000
#define OCTEON_CN38XX_PASS2     0x000d0001
#define OCTEON_CN38XX_PASS3     0x000d0003
#define OCTEON_CN38XX       	(OCTEON_CN38XX_PASS2 | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)

/* NOTE: OCTEON CN36XX models are not identifiable using the OCTEON_IS_MODEL() functions,
** but are treated as 38XX with a smaller L2 cache.  Setting OCTEON_MODEL to
** OCTEON_CN36XX will not affect how the program is built (it will be built for OCTEON_CN38XX)
** but does cause the simulator to properly simulate the smaller L2 cache. */


/* The OCTEON_CN31XX matches CN31XX models and the CN3020 */
#define OCTEON_CN31XX_PASS1    	0x000d0100
#define OCTEON_CN31XX_PASS1_1  	0x000d0102
#define OCTEON_CN31XX       	(OCTEON_CN31XX_PASS1 | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)

#define OCTEON_CN3005_PASS1    	0x000d0210
#define OCTEON_CN3005_PASS1_1  	0x000d0212
#define OCTEON_CN3005       	(OCTEON_CN3005_PASS1 | OM_IGNORE_REVISION)

#define OCTEON_CN3010_PASS1    	0x000d0200
#define OCTEON_CN3010_PASS1_1  	0x000d0202
#define OCTEON_CN3010       	(OCTEON_CN3010_PASS1 | OM_IGNORE_REVISION)

#define OCTEON_CN3020_PASS1    	0x000d0110
#define OCTEON_CN3020_PASS1_1  	0x000d0112
#define OCTEON_CN3020       	(OCTEON_CN3020_PASS1 | OM_IGNORE_REVISION)


/* This model is only used for internal checks, it
** is not valid model for the OCTEON_MODEL environment variable.
** This matches the CN3010 and CN3005 but NOT the CN3020*/
#define OCTEON_CN30XX       	(OCTEON_CN3010_PASS1   | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)
#define OCTEON_CN30XX_PASS1   	(OCTEON_CN3010_PASS1   | OM_IGNORE_SUBMODEL)
#define OCTEON_CN30XX_PASS1_1  	(OCTEON_CN3010_PASS1_1 | OM_IGNORE_SUBMODEL)

/* This matches the complete family of CN3xxx CPUs, and not subsequent models */
#define OCTEON_CN3XXX           (OCTEON_CN58XX_PASS1 | OM_MATCH_PREVIOUS_MODELS | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL )

/* The revision byte (low byte) has two different encodings.
** CN3XXX:
** 
**     bits
**     <7:5>: reserved (0)
**     <4>:   alternate package
**     <3:0>: revision
**     
** CN5XXX:
** 
**     bits
**     <7>:   reserved (0)
**     <6>:   alternate package
**     <5:3>: major revision
**     <2:0>: minor revision
** 
*/ 

/* Masks used for the various types of model/family/revision matching */
#define OCTEON_38XX_FAMILY_MASK      0x00ffff00
#define OCTEON_38XX_FAMILY_REV_MASK  0x00ffff0f
#define OCTEON_38XX_MODEL_MASK       0x00ffff10
#define OCTEON_38XX_MODEL_REV_MASK   (OCTEON_38XX_FAMILY_REV_MASK | OCTEON_38XX_MODEL_MASK)

/* CN5XXX and use different layout of bits in the revision ID field */
#define OCTEON_58XX_FAMILY_MASK      OCTEON_38XX_FAMILY_MASK
#define OCTEON_58XX_FAMILY_REV_MASK  0x00ffff3f
#define OCTEON_58XX_MODEL_MASK       0x00ffffc0
#define OCTEON_58XX_MODEL_REV_MASK   (OCTEON_58XX_FAMILY_REV_MASK | OCTEON_58XX_MODEL_MASK)


#define __OCTEON_MATCH_MASK__(x,y,z) (((x) & (z)) == ((y) & (z)))


/* NOTE: This for internal use only!!!!! */
#define __OCTEON_IS_MODEL_COMPILE__(arg_model, chip_model) \
    ((((arg_model & OCTEON_38XX_FAMILY_MASK) <= OCTEON_CN3010_PASS1)  && (\
     ((((arg_model) & (OM_MATCH_PREVIOUS_MODELS | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)) == OM_IGNORE_REVISION) && __OCTEON_MATCH_MASK__((chip_model), (arg_model), OCTEON_38XX_MODEL_MASK)) || \
     ((((arg_model) & (OM_MATCH_PREVIOUS_MODELS | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)) == OM_IGNORE_SUBMODEL) && __OCTEON_MATCH_MASK__((chip_model), (arg_model), OCTEON_38XX_FAMILY_REV_MASK)) || \
     ((((arg_model) & (OM_MATCH_PREVIOUS_MODELS | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)) == (OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)) && __OCTEON_MATCH_MASK__((chip_model), (arg_model), OCTEON_38XX_FAMILY_MASK)) || \
     ((((arg_model) & (OM_MATCH_PREVIOUS_MODELS | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)) == 0) && __OCTEON_MATCH_MASK__((chip_model), (arg_model), OCTEON_38XX_MODEL_REV_MASK)) || \
     ((((arg_model) & (OM_MATCH_PREVIOUS_MODELS)) == OM_MATCH_PREVIOUS_MODELS) && (((chip_model) & OCTEON_38XX_MODEL_MASK) < ((arg_model) & OCTEON_38XX_MODEL_MASK))) \
    )) || \
    (((arg_model & OCTEON_38XX_FAMILY_MASK) > OCTEON_CN3010_PASS1)  && (\
     ((((arg_model) & (OM_MATCH_PREVIOUS_MODELS | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)) == OM_IGNORE_REVISION) && __OCTEON_MATCH_MASK__((chip_model), (arg_model), OCTEON_58XX_MODEL_MASK)) || \
     ((((arg_model) & (OM_MATCH_PREVIOUS_MODELS | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)) == OM_IGNORE_SUBMODEL) && __OCTEON_MATCH_MASK__((chip_model), (arg_model), OCTEON_58XX_FAMILY_REV_MASK)) || \
     ((((arg_model) & (OM_MATCH_PREVIOUS_MODELS | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)) == (OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)) && __OCTEON_MATCH_MASK__((chip_model), (arg_model), OCTEON_58XX_FAMILY_MASK)) || \
     ((((arg_model) & (OM_MATCH_PREVIOUS_MODELS | OM_IGNORE_REVISION | OM_IGNORE_SUBMODEL)) == 0) && __OCTEON_MATCH_MASK__((chip_model), (arg_model), OCTEON_58XX_MODEL_REV_MASK)) || \
     ((((arg_model) & (OM_MATCH_PREVIOUS_MODELS)) == OM_MATCH_PREVIOUS_MODELS) && (((chip_model) & OCTEON_58XX_MODEL_MASK) < ((arg_model) & OCTEON_58XX_MODEL_MASK))) \
    )))


#if defined(USE_RUNTIME_MODEL_CHECKS) || defined(__U_BOOT__) || (defined(__linux__) && defined(__KERNEL__))
/* forward declarations */
static inline uint32_t cvmx_get_proc_id(void) __attribute__ ((pure));
static inline uint64_t cvmx_read_csr(uint64_t csr_addr);

/* NOTE: This for internal use only!!!!! */
static inline int __octeon_is_model_runtime__(uint32_t model)
{
    uint32_t cpuid = cvmx_get_proc_id();

    /* Check for special case of mismarked 3005 samples. We only need to check
        if the sub model isn't being ignored */
    if ((model & OM_IGNORE_SUBMODEL) == 0)
    {
        if (cpuid == OCTEON_CN3010_PASS1 && (cvmx_read_csr(0x80011800800007B8ull) & (1ull << 34)))
            cpuid |= 0x10;
    }
    return(__OCTEON_IS_MODEL_COMPILE__(model, cpuid));

}

/* The OCTEON_IS_MODEL macro should be used for all Octeon model checking done in a program.
** This should be kept runtime if at all possible.  Any compile time (#if OCTEON_IS_MODEL) usage
** must be condtionalized with OCTEON_IS_COMMON_BINARY() if runtime checking support is required.
**
*/
#define OCTEON_IS_MODEL(x) __octeon_is_model_runtime__(x)
#define OCTEON_IS_COMMON_BINARY() 1
#undef OCTEON_MODEL
#else
#define OCTEON_IS_MODEL(x) __OCTEON_IS_MODEL_COMPILE__(x, OCTEON_MODEL)
#define OCTEON_IS_COMMON_BINARY() 0
#endif

const char *octeon_model_get_string(uint32_t chip_id);
const char *octeon_model_get_string_buffer(uint32_t chip_id, char * buffer);

#include "octeon-feature.h"

#ifdef	__cplusplus
}
#endif

#endif    /* __OCTEON_MODEL_H__ */
