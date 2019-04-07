/*
 * Portions Copyrighted by Freescale Semiconductor, Inc., 2010-2011
 */

#ifndef __FSL_SECBOOT_TYPES_H_
#define __FSL_SECBOOT_TYPES_H_

#include <linux/types.h>

#define false 0
#define true  1
#define NULL ((void *)0)
#define WORD_SIZE 4
#define WORD_BITS 32

typedef unsigned int bool;

/* Data types */
enum Signedness { UNSIGNED = 0, SIGNED, };
enum Sign { POSITIVE = 0, NEGATIVE = 1, };
enum ByteOrder { LITTLE_ENDIAN_ORDER = 0, BIG_ENDIAN_ORDER = 1 };

/* Minimum and maximum size of RSA signature length in bits */
#define KEY_SIZE       4096
#define KEY_SIZE_BYTES (KEY_SIZE/8)
#define KEY_SIZE_WORDS (KEY_SIZE_BYTES/(WORD_SIZE))

/*
 * Integer structure.
 * This struct contains the following fields
 * reg - holds the integer value
 * reg_size - size of the integer value
 * sign - sign of the integer value
 * This is used to hold the integer representation of
 * modulus, exponent and signature.
 */
typedef struct {
       u32 reg[4 * KEY_SIZE_WORDS];
       u32 reg_size;
       enum Sign sign;
} Integer;

/*
 * RSA structure.
 * This struct contains the following fields
 * exponent
 * modulus
 */
typedef struct {
       Integer N;              /* modulus */
       Integer E;              /* exponent */
} RSA;

typedef struct {
       Integer m_modulus;
       Integer m_result, m_result1;
} ModularArithmetic;

typedef struct {
       Integer m_u;
       ModularArithmetic m_ma;
       u32 m_workspace[5 * KEY_SIZE_WORDS];
} MontgomeryRepresentation;

typedef union {
       u64 m_whole;
       struct {
#ifdef IS_LITTLE_ENDIAN
               u32 low;
               u32 high;
#else
               u32 high;
               u32 low;
#endif
       } m_halfs;
} DWord;

typedef struct {
       int pos;
       Integer exp;
       Integer windowModulus;
       unsigned int windowSize, windowBegin;
       u32 expWindow;
       bool fastNegate, negateNext, firstTime, finished;
} WindowSlider;

#define probably(x)       __builtin_expect(!!(x), 1)

#define div(n, base) ({                              \
       u32 __base = (base);                       \
       u32 __rem;                                 \
       if (probably(((n) >> 32) == 0)) {                 \
               __rem = (u32)(n) % __base;         \
               (n) = (u32)(n) / __base;           \
       } else                                          \
               __rem = divide64(&(n), __base);       \
       __rem;                                          \
})

u32 divide64(u64 *n, u32 b);

#endif
