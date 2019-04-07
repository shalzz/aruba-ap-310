#define MV_ASMLANGUAGE
#include "nBootstrap.h"


.globl nbootStart
nbootStart:
	/* Enable I-Cache */
        mrc     p15, 0, r1, c1, c0, 0
        orr     r1, r1, #0x00001000     /* set bit 12 (I) I-Cache */
        /* MUST BE PLACED AT END OF CACHE LINE!!!!!!!!!!!!!!! */
        mcr     p15, 0, r1, c1, c0, 0

	/* Set up the stack						    */
	ldr   r0, =BOOTER_BASE		
	sub   sp, r0, #12		/* leave 3 words for abort-stack    */
        
        /* jump to new  code */
        
        ldr   lr, =nand_boot
        mov   pc, lr
