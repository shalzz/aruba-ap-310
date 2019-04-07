/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.

*******************************************************************************/

#define MV_ASMLANGUAGE
#include "mvOsAsm.h"
#include "mvSysHwConfig.h"

#include "mvCtrlEnvAsm.h"
#include "mvBoardEnvSpec.h"



#include "sys/mvCpuIfRegs.h"


jumpStart:

.section	".reset_vector_sect",#alloc, #execinstr
#if defined(MV_88F6082) || defined(MV_88F6183) || defined(DB_88F5181_OLD) || defined(DB_FPGA) || \
	defined(MV88F6281) || defined(MV88F6282) || defined(MV88F6192) || defined(MV88F6180)  || defined(MV_88F6183L) || \
    defined(MV88F6190) || defined(MV88F6280) || defined(MV88F6500)  || \
    defined(MV88F6510)  || defined(MV88F6530) || defined(MV88F6560) /* KW2 */

#if defined(__BE)
	/* disable I-Cache */
	.word 0x100f11ee /* mrc     15, 0, r0, cr1, cr0, {0} */
	.word 0x010ac0e3 /* bic     r0, r0, #4096   ; 0x1000 */
	.word 0x0700c0e3 /* bic     r0, r0, #7      ; 0x7 */
	.word 0x020080e3 /* orr     r0, r0, #2      ; 0x2 */
	.word 0x100f01ee /* mcr     15, 0, r0, cr1, cr0, {0} */
	/* disable L2 prefetch */
	.word 0x110f3fee /* mrc p15, 1, r0, c15, c1 */
	.word 0x010480e3 /* orr r0, r0, #(1<<24) */
	.word 0x110f2fee /* mcr p15, 1, r0, c15, c1 */
        /* convert CPU to big endian */
        .word 0x100f11ee /* mrc p15, 0, r0, c1, c0 */
        .word 0x800080e3 /* orr r0, r0, #0x80 */
        .word 0x100f01ee /* mcr p15, 0, r0, c1, c0 */
	nop;nop;nop;nop;
	nop;nop;nop;nop;
        
#endif
#endif        

        /* Check if we booted from DRAM. If we did someone already      */
        /* initialize the DRAM controller                               */ 
        
	adr	r4, jumpStart		/* r4 <- current position of code   */
	mov	r5, #~0xff
	and	r4, r4, r5

#if defined(MV78XX0)
/* Add for load code into I cache */
	/*
	 * flush v4 I/D caches
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	/* Load source code from 0xffff0000-0xffff1000 */
        mov   r8, r4		/* U-boot reset vector address on flash */
        mov   r2, #0x1000	/* U-boot size of code in reset vector */

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00000007	/* 2:0 (CAM) */
	orr	r0, r0, #0x00000002	/* set bit 2 (A) Align */
	orr	r0, r0, #0x00001000	/* Enabled I-cache */	
	mcr	p15, 0, r0, c1, c0, 0
    nop
    nop

	/* Load code into I Cache */
load_loop:
	mcr   p15, 0, r8, c7, c13, 1
        add   r8, r8, #32 	/* 8 dwords * 4 bytes */
        sub   r2, r2, #32 	/* 8 dwords * 4 bytes */
        cmp   r2, #0 /* check if we have read a full Page */
        bne   load_loop

	/* Load source code from 0xfff80000-0xfff84000 */
        mov   r8, #0
        mov   r0, #0xff		/* U-boot base address on flash */
    	orr   r8, r8, r0, LSL #24
        mov   r0, #0xf8		/* U-boot base address on flash */
    	orr   r8, r8, r0, LSL #16
        mov   	r2, #0x4000	/* U-boot size of code in reset vector */

	/* Load code into I Cache */
load_loop1:
	mcr   p15, 0, r8, c7, c13, 1
        add   r8, r8, #32 	/* 8 dwords * 4 bytes */
        sub   r2, r2, #32 	/* 8 dwords * 4 bytes */
        cmp   r2, #0 /* check if we have read a full Page */
        bne   load_loop1

	/* Lock I-cache */
	mrc	p15, 0, r0, c9, c0, 1
	orr	r0, r0, #0xf
	mcr	p15, 0, r0, c9, c0, 1
    nop
    nop
/* End of code load */
#endif /* MV78XX0 */

        ldr     r5, __start		/* r5 <- linker results for _start   */
        ldr     r2, _jumpStart		/* r2 <- linker results reset vector */
	sub	r8, r2, r5		/* r8 <- (reset vector address - start address) */
	sub	r8, r4, r8		/* r8 <- absolute address to jump to */
					/* r8 <- (current code address -      */
	ldr	sp, =0
	ldr	lr, =0 
	ldr	r5, =CONFIG_SYS_RESET_ADDRESS 	/* test if we run from flash or RAM */
	cmp     r4, r5                  /* don't reloc during debug         */
	beq     romBoot
        
        mov     r5, #1
        ldr     r4, =dramBoot
        str     r5, [r4]                /* We started executing from DRAM */
romBoot:

#if defined(MV78200)
	mov	r0, #0
	mrc	p15, 1, r0, c15, c1, 0
        /* Check if we are CPU0 or CPU1 */
	and	r0, r0, #0x4000
	cmp	r0, #0x4000
    beq device_cont

/* Setting the PEX header device ID for 0x78200 due to a problem in engineering sample devices */
/* There is no need to implement this workaround for production devices */
    MV_DV_REG_READ_ASM(r6, r1, PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))
	ldr   r1, =MV_78200_DEV_ID
    ldr   r2, =0xffff
    and   r6, r6, r2
    orr   r6, r6, r1, LSL #PXDAVI_DEV_ID_OFFS
    MV_DV_REG_WRITE_ASM(r6, r1, PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))
#endif

#if defined(MV_88F1181)

	/* set gpp out en */
        ldr r2, = 0xf33
        MV_DV_REG_WRITE_ASM(r1, r1, 0x10104)

	/* turn on debug led to 2 */
	ldr r2, = 0x8
        MV_DV_REG_WRITE_ASM(r1, r1, 0x10100)
         
         ldr pc, = 0xfff90000
                  
#else

#if defined(DB_FPGA)
	b device_cont
#endif /* DB_FPGA */

#if !defined(MV78XX0)
	/* Read device ID  */
	MV_DV_CTRL_MODEL_GET_ASM(r6, r1);
	ldr   r1, =0x5281
        cmp   r6, r1
	beq   device_5281
	
	/* TC90 acts as Orion 2 C0 */
	ldr   r1, =0x1281
        cmp   r6, r1
	beq   device_5281_C0

	/* 6183 & 6183L */
	ldr   r1, =0x6183
        cmp   r6, r1
	bne   device_cont
#endif

#if defined(MV_88F6183L)
/* Setting the PEX header device ID for 6183L */
    MV_DV_REG_READ_ASM(r6, r1, PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))
	ldr   r1, =MV_6183L_DEV_ID    
    ldr   r2, =0xffff
    and   r6, r6, r2
    orr   r6, r6, r1, LSL #PXDAVI_DEV_ID_OFFS
    MV_DV_REG_WRITE_ASM(r6, r1, PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))
#endif /* MV_88F6183L */


#if !defined(MV78XX0)
        /* Read device revision */
	MV_DV_CTRL_REV_GET_ASM(r6, r1);
	cmp   r6, #0x3  /* 6183 == B0 */
        bne   device_cont

        MV_DV_REG_READ_ASM (r6, r1, CPU_FTDLL_CONFIG_REG)
        ldr    r1, =0x10000
        orr     r6, r6, r1
        MV_DV_REG_WRITE_ASM (r6, r1, CPU_FTDLL_CONFIG_REG)
#endif

	b device_cont
	
device_5281:

        /* Read device revision */
	MV_DV_CTRL_REV_GET_ASM(r6, r1);
	cmp   r6, #0x0  /* Orion 2 == A0 */
        beq   device_5281_A0

	cmp   r6, #0x1  /* Orion 2 == B0 */
        beq   device_5281_B0

	cmp   r6, #0x2  /* Orion 2 == C0 */
        beq   device_5281_C0

	cmp   r6, #0x4  /* Orion 2 == d0 */
        beq   device_5281_D0

	cmp   r6, #0x5  /* Orion 2 == d1 */
        beq   device_5281_D1

	cmp   r6, #0x6  /* Orion 2 == d2 */
        beq   device_5281_D2

	b device_cont

device_5281_D0:
device_5281_D1:
device_5281_D2:
#if !defined(MV78XX0)
        MV_DV_REG_READ_ASM (r6, r1, CPU_FTDLL_CONFIG_REG)

        ldr    r1, =0xFFFF8080
        and    r6, r6, r1
        ldr    r1, =0x1902
        orr     r6, r6, r1

        MV_DV_REG_WRITE_ASM (r6, r1, CPU_FTDLL_CONFIG_REG)
#endif
	b device_cont
         
device_5281_C0:
#if !defined(MV78XX0)
        MV_DV_REG_READ_ASM (r6, r1, CPU_FTDLL_CONFIG_REG)

        ldr    r1, =0xFFFF8080
        and    r6, r6, r1
        ldr    r1, =0x8308
        orr     r6, r6, r1

        MV_DV_REG_WRITE_ASM (r6, r1, CPU_FTDLL_CONFIG_REG)
#endif
	b device_cont

device_5281_B0:
#if !defined(MV78XX0)
        MV_DV_REG_READ_ASM (r6, r1, CPU_FTDLL_CONFIG_REG)

        ldr    r1, =0xFFFF8080
        and    r6, r6, r1
        ldr    r1, =0x821b
        orr     r6, r6, r1

        MV_DV_REG_WRITE_ASM (r6, r1, CPU_FTDLL_CONFIG_REG)
#endif
	b device_cont
device_5281_A0:	
device_cont:
#if defined(MV88F6190) || defined(MV88F6192)
        MV_DV_REG_READ_ASM(r6, r1, CHIP_BOND_REG)
        ldr   r2, =0x3
        and   r6, r6, r2
        cmp   r6, #0x1
        bne   not619X

/* Setting the PEX header device ID for 6190 and 6192 */
    MV_DV_REG_READ_ASM(r6, r1, PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))
#if defined(MV88F6190)
	ldr   r1, =MV_6190_DEV_ID
#else
	ldr   r1, =MV_6192_DEV_ID
#endif /* MV_88F6190 */
    ldr   r2, =0xffff
    and   r6, r6, r2
    orr   r6, r6, r1, LSL #PXDAVI_DEV_ID_OFFS
    MV_DV_REG_WRITE_ASM(r6, r1, PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))
not619X:
#endif
#if defined(MV88F6280)
/* Setting the PEX header device ID for 6280 */
    MV_DV_REG_READ_ASM(r6, r1, PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))
	ldr   r1, =MV_6280_DEV_ID
    ldr   r2, =0xffff
    and   r6, r6, r2
    orr   r6, r6, r1, LSL #PXDAVI_DEV_ID_OFFS
    MV_DV_REG_WRITE_ASM(r6, r1, PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))
#endif

#if defined(MV88F6500) || defined(MV88F6510) || defined(MV88F6560) || defined(MV88F6530)
    /* Setting the PEX header device ID for 65XX */
    MV_DV_REG_READ_ASM(r6, r1, PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))
#if defined(MV88F6500)
	ldr   r1, =MV_6560_DEV_ID
#elif defined(MV88F6510)
	ldr   r1, =MV_6510_DEV_ID
#elif defined(MV88F6530)
	ldr   r1, =MV_6530_DEV_ID
#elif defined(MV88F6560)
	ldr   r1, =MV_6560_DEV_ID
#endif /* MV88F6560 */
    ldr   r2, =0xffff
    and   r6, r6, r2
    orr   r6, r6, r1, LSL #PXDAVI_DEV_ID_OFFS
    MV_DV_REG_WRITE_ASM(r6, r1, PEX_CFG_DIRECT_ACCESS(0,PEX_DEVICE_AND_VENDOR_ID))
#endif /* defined(MV88F6500) || defined(MV88F6510) || defined(MV88F6560) || defined(MV88F6530) */
        mov lr, r8
        mov pc, lr

#endif         


__start:
        .word  _start
        
.globl dramBoot
dramBoot:
        .word   0

_jumpStart:
        .word   .reset_vector_sect
        
.section	".dummy",#alloc, #execinstr
	.long 0xffffffff
	.long 0xffffffff
	.long 0xffffffff
	.long 0xffffffff


