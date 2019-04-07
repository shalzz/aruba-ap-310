#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "asm/addrspace.h" // pick up defines of KSEGx
#ifndef PAGE_SIZE
#define PAGE_SIZE (1<<12)
#endif
//#define KSEG0     (0xffffffff80000000ULL)
//#define KSEG1     (0xffffffffA0000000ULL)
//#define KSEG2     (0xffffffffC0000000ULL)
//#define KSEG3     (0xffffffffE0000000ULL)

#define XKPHYS        0x8000000000000000
#define CCA_UNCACHED  2
#define CCA_CACHED    3

#ifndef __ASSEMBLY__
//#include "types.h"
//#include "linux/types.h"

/*
 * The following macros are especially useful for __asm__
 * inline assembler.
 */
#ifndef __STR
#define __STR(x) #x
#endif
#ifndef STR
#define STR(x) __STR(x)
#endif

__asm__(".macro\t__sti\n\t"
	".set\tpush\n\t"
	".set\treorder\n\t"
	".set\tnoat\n\t"
	"mfc0\t$1,$12\n\t"
	"ori\t$1,0x1f\n\t"
	"xori\t$1,0x1e\n\t" "mtc0\t$1,$12\n\t" ".set\tpop\n\t" ".endm");

static __inline__ void __sti(void)
{
	__asm__ __volatile__("__sti":	/* no outputs */
			     :	/* no inputs */
			     :"memory");
}

__asm__(".macro\t__cli\n\t"
	".set\tpush\n\t"
	".set\tnoat\n\t"
	"mfc0\t$1,$12\n\t"
	"ori\t$1,1\n\t"
	"xori\t$1,1\n\t"
	".set\tnoreorder\n\t"
	"mtc0\t$1,$12\n\t"
	"sll\t$0, $0, 1\t\t\t# nop\n\t"
	"sll\t$0, $0, 1\t\t\t# nop\n\t"
	"sll\t$0, $0, 1\t\t\t# nop\n\t" ".set\tpop\n\t" ".endm");

static __inline__ void __cli(void)
{
	__asm__ __volatile__("__cli":	/* no outputs */
			     :	/* no inputs */
			     :"memory");
}

#define sti() __sti()
#define cli() __cli()

#define __save_flags(x)							\
__asm__ __volatile__(							\
	".set\tpush\n\t"						\
	".set\treorder\n\t"						\
	"mfc0\t%0,$12\n\t"						\
	".set\tpop\n\t"							\
	: "=r" (x))

#define save_flags(x) __save_flags(x)

__asm__(".macro\t__save_and_cli result\n\t"
	".set\tpush\n\t"
	".set\treorder\n\t"
	".set\tnoat\n\t"
	"mfc0\t\\result, $12\n\t"
	"ori\t$1, \\result, 1\n\t"
	"xori\t$1, 1\n\t"
	".set\tnoreorder\n\t"
	"mtc0\t$1, $12\n\t"
	"sll\t$0, $0, 1\t\t\t# nop\n\t"
	"sll\t$0, $0, 1\t\t\t# nop\n\t"
	"sll\t$0, $0, 1\t\t\t# nop\n\t" ".set\tpop\n\t" ".endm");

#define __save_and_cli(x)						\
__asm__ __volatile__(							\
	"__save_and_cli\t%0"						\
	: "=r" (x)							\
	: /* no inputs */						\
	: "memory")

__asm__(".macro\t__restore_flags flags\n\t"
	".set\tnoreorder\n\t"
	".set\tnoat\n\t"
	"mfc0\t$1, $12\n\t"
	"andi\t\\flags, 1\n\t"
	"ori\t$1, 1\n\t"
	"xori\t$1, 1\n\t"
	"or\t\\flags, $1\n\t"
	"mtc0\t\\flags, $12\n\t"
	"sll\t$0, $0, 1\t\t\t# nop\n\t"
	"sll\t$0, $0, 1\t\t\t# nop\n\t"
	"sll\t$0, $0, 1\t\t\t# nop\n\t"
	".set\tat\n\t" ".set\treorder\n\t" ".endm");

#define save_and_cli(x) do { save_flags(x); cli(); } while(0)

#define __restore_flags(flags)						\
do {									\
	unsigned long __tmp1;						\
									\
	__asm__ volatile (						\
		"__restore_flags\t%0"					\
		: "=r" (__tmp1)						\
		: "0" (flags)						\
		: "memory");						\
} while(0)

#define restore_flags(x) __restore_flags(x)

static inline unsigned long get_gp(void);
static inline unsigned long get_gp(void)
{
	unsigned long gp = 0;
	__asm__ volatile (".set push           \n"
			  ".set noreorder      \n"
			  " move %0, $28       \n"
			  ".set pop            \n":"=r" (gp)
	    );
	return gp;
}

static inline unsigned long get_sp(void);
static inline unsigned long get_sp(void)
{
	unsigned long sp = 0;
	__asm__ volatile (".set push           \n"
			  ".set noreorder      \n"
			  " move %0, $29       \n"
			  ".set pop            \n":"=r" (sp)
	    );
	return sp;
}

static inline void cpu_halt(void);
static inline void cpu_halt(void)
{
	cli();
	for (;;) {
		__asm__ volatile ("wait\n");
	}
}

#ifndef AR7100
//static inline unsigned long virt_to_phys(volatile void *address);
static inline unsigned long virt_to_phys(volatile void *address)
{
	unsigned long addr = (unsigned long)address;
	/* For now assume all addresses are in KSEG0 or KSEG1 */
	if (addr >= (unsigned long)KSEG2)
		return 0;
	if (addr >= (unsigned long)KSEG1)
		return addr - (unsigned long)KSEG1;
	return addr - (unsigned long)KSEG0;
}

//static inline void *phys_to_virt(unsigned long address);
static inline void *phys_to_virt(unsigned long address)
{
	/* For now assume all addresses are in KSEG0 */
	return (void *)((address & 0x1fffffff) + (unsigned long)KSEG0);
}

static __inline__ uint32_t lw_40bit_phys(uint64_t phys, int cca)
{
	uint32_t value = 0;

	__asm__ __volatile__(".set push\n"
			     ".set noreorder\n"
			     ".set mips64\n"
			     "dli   $8, " STR(XKPHYS) "\n"
			     "or    $8, $8, %2\n"
			     "daddu $8, $8, %1\n"
			     "lw    %0, 0($8) \n" ".set pop\n":"=r"(value)
			     :"r"(phys & 0xfffffffffcULL),
			     "r"((uint64_t) cca << 59)
			     :"$8");

	return value;
}

static __inline__ void sw_40bit_phys(uint64_t phys, int cca, uint32_t value)
{
	__asm__ __volatile__(".set push\n"
			     ".set noreorder\n"
			     ".set mips64\n"
			     "dli   $8, " STR(XKPHYS) "\n"
			     "or    $8, $8, %2\n"
			     "daddu $8, $8, %1\n"
			     "sw    %0, 0($8) \n"
			     ".set pop\n"::"r"(value),
			     "r"(phys & 0xfffffffffcULL),
			     "r"((uint64_t) cca << 59)
			     :"$8");
}

static __inline__ void sh_40bit_phys(uint64_t phys, int cca, uint32_t value)
{
  __asm__ __volatile__(".set push\n"
                       ".set noreorder\n"
                       ".set mips64\n"
                       "dli   $8, "STR(XKPHYS)"\n"
                       "or    $8, $8, %2\n"
                       "daddu $8, $8, %1\n"
                       "sh    %0, 0($8) \n"
                       ".set pop\n"
                       :
                       : "r"(value), "r"(phys & 0xfffffffffeULL), "r"((uint64_t)cca << 59)
                       : "$8"
                       );
}

static __inline__ uint32_t lw_40bit_phys_uncached(uint64_t phys)
{
	return lw_40bit_phys(phys, CCA_UNCACHED);
}
static __inline__ uint32_t lw_40bit_phys_cached(uint64_t phys)
{
	return lw_40bit_phys(phys, CCA_CACHED);
}
static __inline__ void sw_40bit_phys_uncached(uint64_t phys, uint32_t value)
{
	sw_40bit_phys(phys, CCA_UNCACHED, value);
}
static __inline__ void sw_40bit_phys_cached(uint64_t phys, uint32_t value)
{
	sw_40bit_phys(phys, CCA_CACHED, value);
}
static __inline__ void sh_40bit_phys_uncached(uint64_t phys, uint16_t value)
{
      sh_40bit_phys(phys, CCA_UNCACHED, value);
}
static __inline__ void sh_40bit_phys_cached(uint64_t phys, uint16_t value)
{
      sh_40bit_phys(phys, CCA_CACHED, value);
}
#endif /* AR7100 */


#endif

#endif
