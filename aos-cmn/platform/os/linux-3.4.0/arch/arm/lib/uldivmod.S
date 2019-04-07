#include <linux/linkage.h>
#include <asm/assembler.h>

#ifdef __ARMEB__
#define xxh r0
#define xxl r1
#define yyh r2
#define yyl r3
#else
#define xxh r1
#define xxl r0
#define yyh r3
#define yyl r2
#endif	

#undef push
	.globl __aeabi_uldivmod
ENTRY(__aeabi_uldivmod)
	sub sp, sp, #8
#if defined(__thumb2__)
	mov ip, sp
	push {ip, lr}
#else
	push {sp, lr}
#endif
	bl __gnu_uldivmod_helper
	ldr lr, [sp, #4]
	add sp, sp, #8
	pop {r2, r3}
	bx lr
	
