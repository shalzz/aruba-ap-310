#ifndef _MIPSREGS_H
#define _MIPSREGS_H

#include "asm/system.h"

/*
 * Coprocessor 0 register names
 */
#define CP0_INDEX $0
#define CP0_RANDOM $1
#define CP0_ENTRYLO0 $2
#define CP0_ENTRYLO1 $3
#define CP0_CONF $3
#define CP0_CONTEXT $4
#define CP0_PAGEMASK $5
#define CP0_WIRED $6
#define CP0_INFO $7
#define CP0_BADVADDR $8
#define CP0_COUNT $9
#define CP0_ENTRYHI $10
#define CP0_COMPARE $11
#define CP0_STATUS $12
#define CP0_CAUSE $13
#define CP0_EPC $14
#define CP0_PRID $15
#define CP0_CONFIG $16
#define CP0_LLADDR $17
#define CP0_WATCHLO $18
#define CP0_WATCHHI $19
#define CP0_XCONTEXT $20
#define CP0_FRAMEMASK $21
#define CP0_DIAGNOSTIC $22
#define CP0_DEBUG $23
#define CP0_DEPC $24
#define CP0_PERFORMANCE $25
#define CP0_ECC $26
#define CP0_CACHEERR $27
#define CP0_TAGLO $28
#define CP0_TAGHI $29
#define CP0_ERROREPC $30
#define CP0_DESAVE $31

/*
 * Status register bits available in all MIPS CPUs.
 */
#define ST0_KX                  0x00000080
#define ST0_IM                  0x0000ff00
#define  STATUSB_IP0            8
#define  STATUSF_IP0            (1   <<  8)
#define  STATUSB_IP1            9
#define  STATUSF_IP1            (1   <<  9)
#define  STATUSB_IP2            10
#define  STATUSF_IP2            (1   << 10)
#define  STATUSB_IP3            11
#define  STATUSF_IP3            (1   << 11)
#define  STATUSB_IP4            12
#define  STATUSF_IP4            (1   << 12)
#define  STATUSB_IP5            13
#define  STATUSF_IP5            (1   << 13)
#define  STATUSB_IP6            14
#define  STATUSF_IP6            (1   << 14)
#define  STATUSB_IP7            15
#define  STATUSF_IP7            (1   << 15)
#define  STATUSB_IP8            0
#define  STATUSF_IP8            (1   << 0)
#define  STATUSB_IP9            1
#define  STATUSF_IP9            (1   << 1)
#define  STATUSB_IP10           2
#define  STATUSF_IP10           (1   << 2)
#define  STATUSB_IP11           3
#define  STATUSF_IP11           (1   << 3)
#define  STATUSB_IP12           4
#define  STATUSF_IP12           (1   << 4)
#define  STATUSB_IP13           5
#define  STATUSF_IP13           (1   << 5)
#define  STATUSB_IP14           6
#define  STATUSF_IP14           (1   << 6)
#define  STATUSB_IP15           7
#define  STATUSF_IP15           (1   << 7)

#define ST0_IE			0x00000001
#define ST0_CH                  0x00040000
#define ST0_NMI                 0x00080000
#define ST0_SR                  0x00100000
#define ST0_TS                  0x00200000
#define ST0_BEV                 0x00400000
#define ST0_RE                  0x02000000
#define ST0_FR                  0x04000000
#define ST0_CU                  0xf0000000
#define ST0_CU0                 0x10000000
#define ST0_CU1                 0x20000000
#define ST0_CU2                 0x40000000
#define ST0_CU3                 0x80000000
#define ST0_XX                  0x80000000	/* MIPS IV naming */

/*
 * Bitfields and bit numbers in the coprocessor 0 cause register.
 *
 * Refer to your MIPS R4xx0 manual, chapter 5 for explanation.
 */
#define  CAUSEB_EXCCODE         2
#define  CAUSEF_EXCCODE         (31  <<  2)
#define  CAUSEB_IP              8
#define  CAUSEF_IP              (255 <<  8)
#define  CAUSEB_IP0             8
#define  CAUSEF_IP0             (1   <<  8)
#define  CAUSEB_IP1             9
#define  CAUSEF_IP1             (1   <<  9)
#define  CAUSEB_IP2             10
#define  CAUSEF_IP2             (1   << 10)
#define  CAUSEB_IP3             11
#define  CAUSEF_IP3             (1   << 11)
#define  CAUSEB_IP4             12
#define  CAUSEF_IP4             (1   << 12)
#define  CAUSEB_IP5             13
#define  CAUSEF_IP5             (1   << 13)
#define  CAUSEB_IP6             14
#define  CAUSEF_IP6             (1   << 14)
#define  CAUSEB_IP7             15
#define  CAUSEF_IP7             (1   << 15)
#define  CAUSEB_IV              23
#define  CAUSEF_IV              (1   << 23)
#define  CAUSEB_CE              28
#define  CAUSEF_CE              (3   << 28)
#define  CAUSEB_BD              31
#define  CAUSEF_BD              (1   << 31)

#define CONF_CM_CACHABLE_NO_WA		0
#define CONF_CM_CACHABLE_WA		1
#define CONF_CM_UNCACHED		2
#define CONF_CM_CACHABLE_NONCOHERENT	3
#define CONF_CM_CACHABLE_CE		4
#define CONF_CM_CACHABLE_COW		5
#define CONF_CM_CACHABLE_CUW		6
#define CONF_CM_CACHABLE_ACCELERATED	7
#define CONF_CM_CMASK			7
#define CONF_DB				(1 <<  4)
#define CONF_IB				(1 <<  5)
#define CONF_SC				(1 << 17)
#define CONF_AC                         (1 << 23)
#define CONF_HALT                       (1 << 25)

#ifndef __ASSEMBLY__

/*
 * Macros to access the system control coprocessor
 */
#define read_32bit_cp0_register(source)                         \
({ int __res;                                                   \
        __asm__ __volatile__(                                   \
	".set\tpush\n\t"					\
	".set\tnoreorder\n\t"					\
        "mfc0\t%0,"STR(source)"\n\t"                            \
	".set\tpop"						\
        : "=r" (__res));                                        \
        __res;})

#define write_32bit_cp0_register(register,value)                \
        __asm__ __volatile__(                                   \
        "mtc0\t%0,"STR(register)"\n\t"				\
	"nop"							\
        : : "r" (value));

#define read_c0_status()	read_32bit_cp0_register($12)
#define write_c0_status(val)	write_32bit_cp0_register($12, val)

/*
 * Manipulate the status register.
 * Mostly used to access the interrupt bits.
 */
#define __BUILD_SET_CP0(name,register)				\
static inline unsigned int					\
set_cp0_##name(unsigned int set)				\
{								\
	unsigned int res;					\
								\
	res = read_32bit_cp0_register(register);		\
	res |= set;						\
	write_32bit_cp0_register(register, res);		\
								\
	return res;						\
}								\
								\
static inline unsigned int					\
clear_cp0_##name(unsigned int clear)				\
{								\
	unsigned int res;					\
								\
	res = read_32bit_cp0_register(register);		\
	res &= ~clear;						\
	write_32bit_cp0_register(register, res);		\
								\
	return res;						\
}								\
								\
static inline unsigned int					\
change_cp0_##name(unsigned int change, unsigned int new)	\
{								\
	unsigned int res;					\
								\
	res = read_32bit_cp0_register(register);		\
	res &= ~change;						\
	res |= (new & change);					\
	write_32bit_cp0_register(register, res);		\
								\
	return res;						\
}

__BUILD_SET_CP0(status, CP0_STATUS)
    __BUILD_SET_CP0(cause, CP0_CAUSE)
    __BUILD_SET_CP0(config, CP0_CONFIG)

#ifdef CONFIG_OCTEON
/*
 * This should be changed when we get a compiler that support the MIPS32 ISA.
 */
#define read_mips32_cp0_config1()                               \
({ int __res;                                                   \
	__asm__ __volatile__(                                   \
	".set\tpush\n\t"                                   \
        ".set\tnoreorder\n\t"                                   \
	".set\tnoat\n\t"                                        \
	".word\t0x40018001\n\t"                                 \
	"move\t%0,$1\n\t"                                       \
	".set\tpop\n\t"                                          \
	:"=r" (__res));                                         \
	__res;})

#else
/*
 * This should be changed when we get a compiler that support the MIPS32 ISA.
 */
#define read_mips32_cp0_config1()                               \
({ int __res;                                                   \
        __asm__ __volatile__(                                   \
	".set\tnoreorder\n\t"                                   \
	".set\tnoat\n\t"                                        \
	"#.set\tmips64\n\t"					\
	"#mfc0\t$1, $16, 1\n\t"					\
	"#.set\tmips0\n\t"					\
     	".word\t0x40018001\n\t"                                 \
	"move\t%0,$1\n\t"                                       \
	".set\tat\n\t"                                          \
	".set\treorder"                                         \
	:"=r" (__res));                                         \
        __res;})
#endif
#if 0        
static inline void set_entrylo0(unsigned long long val)
{
	unsigned long flags;

	__save_and_cli(flags);
	__asm__ __volatile__(".set\tmips3\n\t"
			     "dsll\t%L0, %L0, 32\n\t"
			     "dsrl\t%L0, %L0, 32\n\t"
			     "dsll\t%M0, %M0, 32\n\t"
			     "or\t%L0, %L0, %M0\n\t"
			     "dmtc0\t%L0, $2\n\t" ".set\tmips0"::"r"(val));
	__restore_flags(flags);
}

static inline void set_entrylo1(unsigned long long val)
{
	unsigned long flags;

	__save_and_cli(flags);
	__asm__ __volatile__(".set\tmips3\n\t"
			     "dsll\t%L0, %L0, 32\n\t"
			     "dsrl\t%L0, %L0, 32\n\t"
			     "dsll\t%M0, %M0, 32\n\t"
			     "or\t%L0, %L0, %M0\n\t"
			     "dmtc0\t%L0, $3\n\t" ".set\tmips0"::"r"(val));
	__restore_flags(flags);
}
#endif
static inline void set_entryhi(unsigned long val)
{
	__asm__ __volatile__(".set push\n\t"
			     ".set reorder\n\t"
			     "mtc0 %z0, $10\n\t" ".set pop"::"Jr"(val));
}

static inline unsigned long get_entryhi(void)
{
	unsigned long val;

	__asm__ __volatile__(".set push\n\t"
			     ".set reorder\n\t"
			     "mfc0 %0, $10\n\t" ".set pop":"=r"(val));

	return val;
}

static inline void set_index(unsigned long val)
{
	__asm__ __volatile__(".set push\n\t"
			     ".set reorder\n\t"
			     "mtc0 %z0, $0\n\t" ".set pop"::"Jr"(val));
}

static inline unsigned long get_index(void)
{
	unsigned long val;

	__asm__ __volatile__(".set push\n\t"
			     ".set reorder\n\t"
			     "mfc0 %0, $0\n\t" ".set pop":"=r"(val));
	return val;
}

/* TLB operations. */
static inline void tlb_probe(void)
{
	__asm__ __volatile__(".set push\n\t"
			     ".set reorder\n\t" "tlbp\n\t" ".set pop");
}

static inline void tlb_read(void)
{
	__asm__ __volatile__(".set push\n\t"
			     ".set reorder\n\t" "tlbr\n\t" ".set pop");
}

#ifdef CONFIG_OCTEON
/*
 * For now use this only with interrupts disabled!
 */
#define read_64bit_cp0_register(source)                         \
({ int __res;                                                   \
	__asm__ __volatile__(                                   \
        ".set\tpush\n\t"                                       \
        ".set\tmips3\n\t"                                       \
	"dmfc0\t%0,"STR(source)"\n\t"                           \
	".set\tpop"                                           \
	: "=r" (__res));                                        \
	__res;})

#define write_64bit_cp0_register(register,value)                \
	__asm__ __volatile__(                                   \
	".set\tpush\n\t"                                       \
        ".set\tmips3\n\t"                                       \
	"dmtc0\t%0,"STR(register)"\n\t"                         \
	".set\tpop"                                           \
	: : "r" (value))
#endif

#ifdef CONFIG_OCTEON
#define tlb_write_indexed()                                     \
	__asm__ __volatile__(                                   \
		".set push\n\t"                            \
                ".set noreorder\n\t"                            \
		"tlbwi\n\t"                                     \
".set pop")
#else
static inline void tlb_write_indexed(void)
{
	__asm__ __volatile__(".set push\n\t"
			     ".set reorder\n\t" "tlbwi\n\t" ".set pop");
}
#endif

static inline void tlb_write_random(void)
{
	__asm__ __volatile__(".set push\n\t"
			     ".set reorder\n\t" "tlbwr\n\t" ".set pop");
}

#endif

#endif
