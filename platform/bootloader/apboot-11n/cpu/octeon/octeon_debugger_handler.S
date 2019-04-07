/*
 * (C) Copyright 2004,2005
 * Cavium Networks
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/**
 * @file octeon_debugger_handler.S
 *
 * Minimal debugger exception handler that calls debugger stub
 *
 * $Id: octeon_debugger_handler.S,v 1.18 2006/12/13 20:28:00 cchavva Exp $ $Name: SDK_1_5_0_build_187 $
 *
 */


#include <config.h>
#ifndef CONFIG_ARUBA_OCTEON
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>

#include "octeon_mem_map.h"

#define COP0_INDEX	$0,0	/* TLB read/write index */
#define COP0_RANDOM	$1,0	/* TLB random index */
#define COP0_ENTRYLO0	$2,0	/* TLB entryLo0 */
#define COP0_ENTRYLO1	$3,0	/* TLB entryLo1 */
#define COP0_CONTEXT	$4,0	/* Context */
#define COP0_PAGEMASK	$5,0	/* TLB pagemask */
#define COP0_PAGEGRAIN	$5,1	/* TLB config for max page sizes */
#define COP0_WIRED	$6,0	/* TLB number of wired entries */
#define COP0_HWRENA	$7,0	/* rdhw instruction enable per register */
#define COP0_BADVADDR	$8,0	/* Bad virtual address */
#define COP0_COUNT	$9,0	/* Mips count register */
#define COP0_CVMCOUNT	$9,6	/* Cavium count register */
#define COP0_CVMCTL	$9,7	/* Cavium control */
#define COP0_ENTRYHI	$10,0	/* TLB entryHi */
#define COP0_COMPARE	$11,0	/* Mips compare register */
#define COP0_CVMMEMCTL	$11,7	/* Cavium memory control */
#define COP0_STATUS	$12,0	/* Mips status register */
#define COP0_INTCTL	$12,1	/* Useless (Vectored interrupts) */
#define COP0_SRSCTL	$12,2	/* Useless (Shadow registers) */
#define COP0_CAUSE	$13,0	/* Mips cause register */
#define COP0_EPC	$14,0	/* Exception program counter */
#define COP0_PRID	$15,0	/* Processor ID */
#define COP0_EBASE	$15,1	/* Exception base */
#define COP0_CONFIG	$16,0	/* Misc config options */
#define COP0_CONFIG1	$16,1	/* Misc config options */
#define COP0_CONFIG2	$16,2	/* Misc config options */
#define COP0_CONFIG3	$16,3	/* Misc config options */
#define COP0_WATCHLO0	$18,0	/* Address watch registers */
#define COP0_WATCHLO1	$18,1	/* Address watch registers */
#define COP0_WATCHHI0	$19,0	/* Address watch registers */
#define COP0_WATCHHI1	$19,1	/* Address watch registers */
#define COP0_XCONTEXT	$20,0	/* OS context */
#define COP0_MULTICOREDEBUG $22,0 /* Cavium debug */
#define COP0_DEBUG	$23,0	/* Debug status */
#define COP0_DEPC	$24,0	/* Debug PC */
#define COP0_PERFCONTROL0 $25,0	/* Performance counter control */
#define COP0_PERFCONTROL1 $25,2	/* Performance counter control */
#define COP0_PERFVALUE0	$25,1	/* Performance counter */
#define COP0_PERFVALUE1	$25,3	/* Performance counter */
#define COP0_CACHEERRI	$27,0	/* I cache error status */
#define COP0_CACHEERRD	$27,1	/* D cache error status */
#define COP0_TAGLOI	$28,0	/* I cache tagLo */
#define COP0_TAGLOD	$28,2	/* D cache tagLo */
#define COP0_DATALOI	$28,1	/* I cache dataLo */
#define COP0_DATALOD	$28,3	/* D cahce dataLo */
#define COP0_TAGHI	$29,2	/* ? */
#define COP0_DATAHII	$29,1	/* ? */
#define COP0_DATAHID	$29,3	/* ? */
#define COP0_ERROREPC	$30,0	/* Error PC */
#define COP0_DESAVE	$31,0	/* Debug scratch area */

#define HW_INSTRUCTION_BREAKPOINT_STATUS            (0xFFFFFFFFFF301000)
#define HW_INSTRUCTION_BREAKPOINT_ADDRESS(num)      (0xFFFFFFFFFF301100 + 0x100 * (num))
#define HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(num) (0xFFFFFFFFFF301108 + 0x100 * (num))
#define HW_INSTRUCTION_BREAKPOINT_ASID(num)         (0xFFFFFFFFFF301110 + 0x100 * (num))
#define HW_INSTRUCTION_BREAKPOINT_CONTROL(num)      (0xFFFFFFFFFF301118 + 0x100 * (num))

#define HW_DATA_BREAKPOINT_STATUS                   (0xFFFFFFFFFF302000)
#define HW_DATA_BREAKPOINT_ADDRESS(num)             (0xFFFFFFFFFF302100 + 0x100 * (num))
#define HW_DATA_BREAKPOINT_ADDRESS_MASK(num)        (0xFFFFFFFFFF302108 + 0x100 * (num))
#define HW_DATA_BREAKPOINT_ASID(num)                (0xFFFFFFFFFF302110 + 0x100 * (num))
#define HW_DATA_BREAKPOINT_CONTROL(num)             (0xFFFFFFFFFF302118 + 0x100 * (num))

#define SAVE_REGISTER(reg)	\
	sd reg, 0(k0);		\
	addi k0, 8

#define RESTORE_REGISTER(reg)	\
	ld reg, -8(k0);		\
	addi k0, -8

#define SAVE_COP0(reg)		\
	dmfc0 k1,reg;		\
	sd k1, 0(k0);		\
	addi k0, 8

#define RESTORE_COP0(reg)	\
	ld k1, -8(k0);		\
	addi k0, -8;		\
	dmtc0 k1,reg	

#define SAVE_ADDRESS(addr)	\
	dli k1, addr;		\
	ld k1, 0(k1);		\
	sd k1, 0(k0);		\
	addi k0, 8

#define RESTORE_ADDRESS(addr)	\
	dli t0, addr;		\
	ld k1, -8(k0);		\
	sd k1, 0(t0);		\
	addi k0, -8

#define REG_SAVE_BASE_DIV_4  (BOOTLOADER_DEBUG_REG_SAVE_BASE >> 2)

.set noreorder
.set noat

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//
// This handler is for debugging over UART1
//
// This handler is installed and implements a GDB remote protocol over
// UART1. The address of this handler can be found at
// BOOTLOADER_DEBUG_HANDLER_LIST[0].
//
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
    .globl cvmx_debugger_handler2
    .text
    .ent cvmx_debugger_handler2
cvmx_debugger_handler2:
	//
	// Store the registers into the per processor global space
        // Only have k0 register to work with to calculate address
        // based on core number to store to
	//


        mfc0    k0, $15, 1  // read exception base reg.
        andi    k0, 0xff    // mask off core ID
        sll     k0, 12      // multiply by 4096 (512 dwords) DEBUG_NUMREGS


        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores

        rotr    k0, k0, 31   // set bit 31 for kseg0 access
        addi    k0, 1
        rotr    k0, k0, 1


	sd	zero, 0(k0)
	sd	$1, 8(k0)
	sd	$2, 16(k0)
	sd 	$3, 24(k0)
	sd 	$4, 32(k0)
	sd	$5, 40(k0)
	sd	$6, 48(k0)
	sd	$7, 56(k0)
	sd	$8, 64(k0)
	sd	$9, 72(k0)
	sd	$10, 80(k0)
	sd	$11, 88(k0)
	sd	$12, 96(k0)
	sd	$13, 104(k0)
	sd	$14, 112(k0)
	sd	$15, 120(k0)
	sd	$16, 128(k0)
	sd	$17, 136(k0)
	sd	$18, 144(k0)
	sd	$19, 152(k0)
	sd	$20, 160(k0)
	sd	$21, 168(k0)
	sd	$22, 176(k0)
	sd	$23, 184(k0)
	sd	$24, 192(k0)	
	sd	$25, 200(k0)
	// Skip k0, its value is in COP0_DESAVE
	sd	$27, 216(k0)
	sd	$28, 224(k0)
	sd	$29, 232(k0)
	sd	$30, 240(k0)
	sd	$31, 248(k0)
	dmfc0	k1, COP0_DESAVE	// This is really the k0 register
	sd	k1, 208(k0)
	dmfc0	k1, COP0_STATUS	
	sd	k1, 256(k0)
	mflo	k1
	sd	k1, 264(k0)
	mfhi	k1
	sd	k1, 272(k0)
	dmfc0	k1, COP0_BADVADDR
	sd	k1, 280(k0)
	dmfc0	k1, COP0_CAUSE
	sd	k1, 288(k0)
	dmfc0	k1, COP0_DEPC
	lw      t0, 0(k1)
        li      t1, 0x7000007f
        bne     t0, t1, LDonotUpdateDepc
        nop
        addi    k1,k1,4

LDonotUpdateDepc:

	sd	k1, 296(k0)	// store pc value from DEPC_REG
	dmfc0	k1, COP0_DEBUG	
	sd	k1, 304(k0)
	dmfc0	k1, COP0_MULTICOREDEBUG	
	sd	k1, 312(k0)
	dmfc0   k1, COP0_PERFVALUE0  // store performance counter counter 0
	sd      k1, 320(k0)
	dmfc0   k1, COP0_PERFVALUE1  // store performance counter counter 1
	sd      k1, 328(k0)
	dmfc0   k1, COP0_PERFCONTROL0  // store performance counter counter 0
	sd      k1, 336(k0)
	dmfc0   k1, COP0_PERFCONTROL1  // store performance counter counter 1
	sd      k1, 344(k0)

        // restore gp for bootloader from DRAM
        li      t1, 0x80000000
        lw      gp, BOOTLOADER_DEBUG_GP_REG(t1)

	// restore k0 (global data pointer) to setup the correct context to 
	// use bootloader functions. */
	lw	k0, BOOTLOADER_DEBUG_KO_REG_SAVE(t1)

	//
	// Call the C debug exception handler with
	// its own stack
	//
        mfc0    t1, $15, 1  // load exception base
        andi    t1, 0xff
        li      t2, DEBUG_STACK_SIZE
        mul     t1, t1, t2
	li	sp, (0x1 << 31) | BOOTLOADER_DEBUG_STACK_BASE
        add     sp, sp, DEBUG_STACK_SIZE
        add     sp, sp, t1

	dla     t9, cvmx_debugger_exception
	jal 	t9
	nop

        //
        // FIXME
        // Special check for STBPP instruction
        // These we change to NOP
        //

	
	//
	// Restore registers and return to the program
	//
        mfc0    k0, $15, 1  // read exception base reg.
        andi    k0, 0xff    // mask off core ID
        sll     k0, 12      // multiply by 4096 (512 dwords) DEBUG_NUMREGS


        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores

        rotr    k0, k0, 31   // set bit 31 for kseg0 access
        addi    k0, 1
        rotr    k0, k0, 1

	ld	zero, 0(k0)
	ld	$1, 8(k0)
	ld	$2, 16(k0)
	ld 	$3, 24(k0)
	ld 	$4, 32(k0)
	ld	$5, 40(k0)
	ld	$6, 48(k0)
	ld	$7, 56(k0)
	ld	$8, 64(k0)
	ld	$9, 72(k0)
	ld	$10, 80(k0)
	ld	$11, 88(k0)
	ld	$12, 96(k0)
	ld	$13, 104(k0)
	ld	$14, 112(k0)
	ld	$15, 120(k0)
	ld	$16, 128(k0)
	ld	$17, 136(k0)
	ld	$18, 144(k0)
	ld	$19, 152(k0)
	ld	$20, 160(k0)
	ld	$21, 168(k0)
	ld	$22, 176(k0)
	ld	$23, 184(k0)
	ld	$24, 192(k0)	
	ld	$25, 200(k0)
	// k0 is register 26, we need to put off loading it till last
	// k1 is register 27, we need to put off loading it till second to last
	ld	$28, 224(k0)
	ld	$29, 232(k0)
	ld	$30, 240(k0)
	ld	$31, 248(k0)
        ld	k1,  256(k0)
	dmtc0	k1,  COP0_STATUS
	ld	k1,  264(k0)
	mtlo	k1
	ld	k1,  272(k0)
	mthi	k1
	ld	k1,  280(k0)
	dmtc0	k1,  COP0_BADVADDR
	ld	k1,  288(k0)
	dmtc0	k1,  COP0_CAUSE
	ld	k1,  296(k0)
	dmtc0	k1,  COP0_DEPC
	ld	k1,  304(k0)
	dmtc0	k1,  COP0_DEBUG	
	ld	k1,  312(k0)
	ori	k1, k1, 0x1	// clear MCD0 bit. It will be set again when we hit a break
	dmtc0	k1,  COP0_MULTICOREDEBUG
	ld	k1,  320(k0)	
	dmtc0   k1,  COP0_PERFVALUE0
	ld	k1,  328(k0)	
	dmtc0   k1,  COP0_PERFVALUE1
	ld	k1,  336(k0)	
	dmtc0   k1,  COP0_PERFCONTROL0
	ld	k1,  344(k0)	
	dmtc0   k1,  COP0_PERFCONTROL1
	ld	k1,  216(k0)	// Load k1 that was used above
        ld	k0,  208(k0)	// This needs to be last
	synci 	0($0)
	deret
	nop

    .end cvmx_debugger_handler2



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//
// This handler is for debugging over PCI
//
// This handler should be used by debuggers using the PCI bus to
// control Octeon. It saves all processor state to a global area and
// spins waiting for the control word to change. All state is then read
// back in and execution continues. Any settings changed while the core
// was spinning are loaded into the core before it returns. This allows
// the PCI debugger to change all registers, COP0 registers, TLB entries,
// and hardware breakpoints.
//
// This handler is not installed by default. The PCI debugger must
// overwrite the default handler by writing BOOTLOADER_DEBUG_HANDLER_LIST[1]
// to BOOTLOADER_DEBUG_TRAMPOLINE.
//
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
    .globl cvmx_debugger_handler_pci
    .text
    .ent cvmx_debugger_handler_pci
cvmx_debugger_handler_pci:
	//
	// k0 has already been stored into the debug scratch COP0 register.
	// The current k0 value is corrupted since it was used to jump here.
	//
	// Store the registers into the per processor global space
        // Only have k0 register to work with to calculate address
        // based on core number to store to
	//
        mfc0    k0, $15, 1  // read exception base reg.
        andi    k0, 0xff    // mask off core ID
        sll     k0, 12      // multiply by 4096 (512 dwords) DEBUG_NUMREGS

        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores

        rotr    k0, k0, 31   // set bit 31 for kseg0 access
        addi    k0, 1
        rotr    k0, k0, 1

	//
	// Save all the CPU registers
	//
	SAVE_REGISTER($0)
	SAVE_REGISTER($1)
	SAVE_REGISTER($2)
	SAVE_REGISTER($3)
	SAVE_REGISTER($4)
	SAVE_REGISTER($5)
	SAVE_REGISTER($6)
	SAVE_REGISTER($7)
	SAVE_REGISTER($8)
	SAVE_REGISTER($9)
	SAVE_REGISTER($10)
	SAVE_REGISTER($11)
	SAVE_REGISTER($12)
	SAVE_REGISTER($13)
	SAVE_REGISTER($14)
	SAVE_REGISTER($15)
	SAVE_REGISTER($16)
	SAVE_REGISTER($17)
	SAVE_REGISTER($18)
	SAVE_REGISTER($19)
	SAVE_REGISTER($20)
	SAVE_REGISTER($21)
	SAVE_REGISTER($22)
	SAVE_REGISTER($23)
	SAVE_REGISTER($24)
	SAVE_REGISTER($25)
	dmfc0	$25, COP0_DESAVE	// This is really the k0 register
	SAVE_REGISTER($25)
	SAVE_REGISTER($27)
	SAVE_REGISTER($28)
	SAVE_REGISTER($29)
	SAVE_REGISTER($30)
	SAVE_REGISTER($31)
	
	//
	// Save all of the COP0 registers
	//
	SAVE_COP0(COP0_STATUS)
	mflo	k1
	sd	k1, 0(k0)
	addi	k0, 8
	mfhi	k1
	sd	k1, 0(k0)
	addi	k0, 8
	SAVE_COP0(COP0_BADVADDR)
	SAVE_COP0(COP0_CAUSE)
	SAVE_COP0(COP0_DEPC)
	SAVE_COP0(COP0_INDEX)
	SAVE_COP0(COP0_RANDOM)
	SAVE_COP0(COP0_ENTRYLO0)
	SAVE_COP0(COP0_ENTRYLO1)
	SAVE_COP0(COP0_CONTEXT)
	SAVE_COP0(COP0_PAGEMASK)
	SAVE_COP0(COP0_PAGEGRAIN)
	SAVE_COP0(COP0_WIRED)
	SAVE_COP0(COP0_HWRENA)
	SAVE_COP0(COP0_COUNT)
	SAVE_COP0(COP0_CVMCOUNT)
	SAVE_COP0(COP0_CVMCTL)
	SAVE_COP0(COP0_ENTRYHI)
	SAVE_COP0(COP0_COMPARE)
	SAVE_COP0(COP0_CVMMEMCTL)
	SAVE_COP0(COP0_INTCTL)
	SAVE_COP0(COP0_SRSCTL)
	SAVE_COP0(COP0_EPC)
	SAVE_COP0(COP0_PRID)
	SAVE_COP0(COP0_EBASE)
	SAVE_COP0(COP0_CONFIG)
	SAVE_COP0(COP0_CONFIG1)
	SAVE_COP0(COP0_CONFIG2)
	SAVE_COP0(COP0_CONFIG3)
	SAVE_COP0(COP0_WATCHLO0)
	SAVE_COP0(COP0_WATCHLO1)
	SAVE_COP0(COP0_WATCHHI0)
	SAVE_COP0(COP0_WATCHHI1)
	SAVE_COP0(COP0_XCONTEXT)
	SAVE_COP0(COP0_MULTICOREDEBUG)
	SAVE_COP0(COP0_DEBUG)
	SAVE_COP0(COP0_PERFCONTROL0)
	SAVE_COP0(COP0_PERFCONTROL1)
	SAVE_COP0(COP0_PERFVALUE0)
	SAVE_COP0(COP0_PERFVALUE1)
	SAVE_COP0(COP0_CACHEERRI)
	SAVE_COP0(COP0_CACHEERRD)
	SAVE_COP0(COP0_TAGLOI)
	SAVE_COP0(COP0_TAGLOD)
	SAVE_COP0(COP0_DATALOI)
	SAVE_COP0(COP0_DATALOD)
	SAVE_COP0(COP0_TAGHI)
	SAVE_COP0(COP0_DATAHII)
	SAVE_COP0(COP0_DATAHID)
	SAVE_COP0(COP0_ERROREPC)
	SAVE_COP0(COP0_DESAVE)

	//
	// Save the instruction breakpoint information
	//
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_STATUS)
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS(0))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS(1))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS(2))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS(3))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(0))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(1))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(2))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(3))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ASID(0))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ASID(1))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ASID(2))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ASID(3))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_CONTROL(0))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_CONTROL(1))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_CONTROL(2))
	SAVE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_CONTROL(3))

	//
	// Save the data watchpoint information
	//
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_STATUS)
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS(0))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS(1))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS(2))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS(3))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS_MASK(0))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS_MASK(1))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS_MASK(2))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS_MASK(3))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ASID(0))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ASID(1))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ASID(2))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_ASID(3))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_CONTROL(0))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_CONTROL(1))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_CONTROL(2))
	SAVE_ADDRESS(HW_DATA_BREAKPOINT_CONTROL(3))
	
	//
	// Read the COP0_CONFIG1 register to find the number of TLB entries.
	// Save all the TLB entries (32 entries except cn58xx, has 64 entries).
	//
	mfc0	t1, COP0_CONFIG1 	
	srl	t1, t1, 25
	andi	t1, t1, 0x3f
	li	t0, -1
LTLBSave:	
	addu	t0, 1
	mtc0	t0, COP0_INDEX
	tlbr
	SAVE_COP0(COP0_ENTRYHI)
	SAVE_COP0(COP0_PAGEMASK)
	SAVE_COP0(COP0_ENTRYLO0)
	SAVE_COP0(COP0_ENTRYLO1)
	SAVE_REGISTER($0)
	bne	t0, t1, LTLBSave
	nop

	//
	// Signal the debugger we've stopped. First syncw is needed
	// to make sure all of the processor state has made it to
	// memory. The second gets the waiting flag out.
	//
	syncw
	li	t0, 1		
	sd	t0, 0(k0)
	syncw

	//
	// Spin here until the Debugger tells us to start running again
	//	
LWaitForDebugger:
	ld	t0, 0(k0)
	bne	t0, $0, LWaitForDebugger
	nop

	//
	// Restore the TLB entries. The debugger could have changed
	// them (Not likely).
	//	
	// First adjust the offset properly for Octeon models supporting
	// only 32 tlb entries.
	//
	mfc0	t0, COP0_CONFIG1
	srl	t0, t0, 25
	andi	t0, t0, 0x3f

LTLBRestore:	
	mtc0	t0, COP0_INDEX
	addi	k0, -8			// Skip this, it was added for alignment
	RESTORE_COP0(COP0_ENTRYLO1)
	RESTORE_COP0(COP0_ENTRYLO0)
	RESTORE_COP0(COP0_PAGEMASK)
	RESTORE_COP0(COP0_ENTRYHI)
	tlbwi
	bne	t0, $0, LTLBRestore
	addu 	t0, -1

	//
	// Restore and data breakpoints added / removed
	//
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_CONTROL(3))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_CONTROL(2))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_CONTROL(1))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_CONTROL(0))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ASID(3))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ASID(2))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ASID(1))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ASID(0))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS_MASK(3))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS_MASK(2))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS_MASK(1))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS_MASK(0))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS(3))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS(2))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS(1))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_ADDRESS(0))
	RESTORE_ADDRESS(HW_DATA_BREAKPOINT_STATUS)
	
	//
	// Restore and instruction breakpoints added / removed
	//
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_CONTROL(3))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_CONTROL(2))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_CONTROL(1))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_CONTROL(0))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ASID(3))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ASID(2))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ASID(1))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ASID(0))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(3))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(2))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(1))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS_MASK(0))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS(3))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS(2))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS(1))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_ADDRESS(0))
	RESTORE_ADDRESS(HW_INSTRUCTION_BREAKPOINT_STATUS)

	//
	// Restore all the COP0 registers
	//
	RESTORE_COP0(COP0_DESAVE)
	RESTORE_COP0(COP0_ERROREPC)
	RESTORE_COP0(COP0_DATAHID)
	RESTORE_COP0(COP0_DATAHII)
	RESTORE_COP0(COP0_TAGHI)
	RESTORE_COP0(COP0_DATALOD)
	RESTORE_COP0(COP0_DATALOI)
	RESTORE_COP0(COP0_TAGLOD)
	RESTORE_COP0(COP0_TAGLOI)
	RESTORE_COP0(COP0_CACHEERRD)
	RESTORE_COP0(COP0_CACHEERRI)
	RESTORE_COP0(COP0_PERFVALUE1)
	RESTORE_COP0(COP0_PERFVALUE0)
	RESTORE_COP0(COP0_PERFCONTROL1)
	RESTORE_COP0(COP0_PERFCONTROL0)
	RESTORE_COP0(COP0_DEBUG)
	RESTORE_COP0(COP0_MULTICOREDEBUG)
	RESTORE_COP0(COP0_XCONTEXT)
	RESTORE_COP0(COP0_WATCHHI1)
	RESTORE_COP0(COP0_WATCHHI0)
	RESTORE_COP0(COP0_WATCHLO1)
	RESTORE_COP0(COP0_WATCHLO0)
	RESTORE_COP0(COP0_CONFIG3)
	RESTORE_COP0(COP0_CONFIG2)
	RESTORE_COP0(COP0_CONFIG1)
	RESTORE_COP0(COP0_CONFIG)
	RESTORE_COP0(COP0_EBASE)
	RESTORE_COP0(COP0_PRID)
	RESTORE_COP0(COP0_EPC)
	RESTORE_COP0(COP0_SRSCTL)
	RESTORE_COP0(COP0_INTCTL)
	RESTORE_COP0(COP0_CVMMEMCTL)
	RESTORE_COP0(COP0_COMPARE)
	RESTORE_COP0(COP0_ENTRYHI)
	RESTORE_COP0(COP0_CVMCTL)
	RESTORE_COP0(COP0_CVMCOUNT)
	RESTORE_COP0(COP0_COUNT)
	RESTORE_COP0(COP0_HWRENA)
	RESTORE_COP0(COP0_WIRED)
	RESTORE_COP0(COP0_PAGEGRAIN)
	RESTORE_COP0(COP0_PAGEMASK)
	RESTORE_COP0(COP0_CONTEXT)
	RESTORE_COP0(COP0_ENTRYLO1)
	RESTORE_COP0(COP0_ENTRYLO0)
	RESTORE_COP0(COP0_RANDOM)
	RESTORE_COP0(COP0_INDEX)
	RESTORE_COP0(COP0_DEPC)
	RESTORE_COP0(COP0_CAUSE)
	RESTORE_COP0(COP0_BADVADDR)
	ld	k1, -8(k0)
	addi	k0, -8
	mthi	k1
	ld	k1, -8(k0)
	addi	k0, -8
	mtlo	k1
	RESTORE_COP0(COP0_STATUS)
	
	//
	// Restore all the regsiters
	//
	RESTORE_REGISTER($31)
	RESTORE_REGISTER($30)
	RESTORE_REGISTER($29)
	RESTORE_REGISTER($28)
	RESTORE_REGISTER($27)
	daddi	k0, -8		// Skip k0, do it last
	RESTORE_REGISTER($25)
	RESTORE_REGISTER($24)
	RESTORE_REGISTER($23)
	RESTORE_REGISTER($22)
	RESTORE_REGISTER($21)
	RESTORE_REGISTER($20)
	RESTORE_REGISTER($19)
	RESTORE_REGISTER($18)
	RESTORE_REGISTER($17)
	RESTORE_REGISTER($16)
	RESTORE_REGISTER($15)
	RESTORE_REGISTER($14)
	RESTORE_REGISTER($13)
	RESTORE_REGISTER($12)
	RESTORE_REGISTER($11)
	RESTORE_REGISTER($10)
	RESTORE_REGISTER($9)
	RESTORE_REGISTER($8)
	RESTORE_REGISTER($7)
	RESTORE_REGISTER($6)
	RESTORE_REGISTER($5)
	RESTORE_REGISTER($4)
	RESTORE_REGISTER($3)
	RESTORE_REGISTER($2)
	RESTORE_REGISTER($1)
	RESTORE_REGISTER($0)
        ld	k0,  208(k0)	// This needs to be last
	
	//
	// Flush the icache and return
	//
	synci 	0($0)
	deret
	nop

    .end cvmx_debugger_handler_pci



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//
// This handler is for profiling over PCI
//
// When a debug exception occurs, The current PC value and performance
// counters are stored in the same spot as the cvmx_debugger_handler_pci
// handler would store them. Execution then immediately starts up again.
// This way a PCI host can get the current PC values without affecting
// the Octeon performance too much.
//
// This handler is not installed by default. The PCI debugger must
// overwrite the default handler by writing BOOTLOADER_DEBUG_HANDLER_LIST[2]
// to BOOTLOADER_DEBUG_TRAMPOLINE.
//
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
    .globl cvmx_debugger_handler_pci_profiler
    .text
    .ent cvmx_debugger_handler_pci_profiler
cvmx_debugger_handler_pci_profiler:
	//
	// k0 has already been stored into the debug scratch COP0 register.
	// The current k0 value is corrupted since it was used to jump here.
	//
	// Since we're only profiling, only store the PC value and the
	// performance counters and immediately return.
	//
        mfc0    k0, $15, 1  // read exception base reg.
        andi    k0, 0xff    // mask off core ID
        sll     k0, 12      // multiply by 4096 (512 dwords) DEBUG_NUMREGS

        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores
        addiu   k0,  REG_SAVE_BASE_DIV_4    // add base offset - after exeption vectors for all cores

        rotr    k0, k0, 31   	// set bit 31 for kseg0 access
        addi    k0, 1
        rotr    k0, k0, 1
	
	ld	$0, 0(k0)		// Perform a load to make sure we don't get a RAW stall
	sd	t0, 0(k0)		// Temporarily store t0 for a working spot
	dmfc0	t0, COP0_DEPC		// Load the PC
	sd	t1, 8(k0)		// Temporarily store t1 for a working spot
	dmfc0	t1, COP0_PERFVALUE0	// Load performance counter 0
	sd	t2, 16(k0)		// Temporarily store t2 for a working spot
	dmfc0	t2, COP0_PERFVALUE1	// Load performance counter 1
	sd	t0, 296(k0)		// Store the PC into the correct location in the registers
	ld	t0, 0(k0)		// Restore t0
	sd	t1, 568(k0)		// Store performance counter 0
	ld	t1, 8(k0)		// Restore t1
	sd	t2, 576(k0)		// Store performance counter 0
	ld	t2, 16(k0)		// Restore t2
	syncw				// Make sure all writes go out
	dmfc0	k0, COP0_DESAVE		// Restore k0
	deret				// Return to normal execution
	nop

    .end cvmx_debugger_handler_pci_profiler
#endif
