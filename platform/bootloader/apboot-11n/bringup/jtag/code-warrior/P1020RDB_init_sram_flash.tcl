########################################################################################
# Initialization file for P1020RDB board - SRAM
# Clock Configuration:
#       CPU0: 800 MHz, CPU1:800 MHz, CCB: 400 MHz,
#       DDR: 800 MHz, SYSCLK: 66 MHz
########################################################################################

variable CCSRBAR 0xff700000

proc CCSR {reg_off} {
	global CCSRBAR
	
	return p:0x[format %x [expr {$CCSRBAR + $reg_off}]]
}

proc init_board {} {

	global CCSRBAR

	# bits 4-7 - ROM_LOC
	# 1000 Local bus FCM 8-bit NAND flash small page
	# 1010 Local bus FCM 8-bit NAND flash large page
	# 1101 Local bus GPCM 8-bit ROM
	# 1110 Local bus GPCM 16-bit ROM
	# 1111 Local bus GPCM 16-bit ROM (default)
	# using PORBMSR registers from Global Utilities
	variable ROM_LOC					0x[format %x [expr {[mem [CCSR 0xe0004] -np] & 0x0f000000}]]
	variable FCM_SMALL_PAGE_BOOT_LOC 	"0x08000000"
	variable FCM_LARGE_PAGE_BOOT_LOC 	"0x0A000000"
	variable GPCM_8BIT_BOOT_LOC	 		"0x0D000000"
	variable GPCM_16BIT_BOOT_LOC	 	"0x0E000000"

	variable BOOT_DEVICE		0x[format %x [expr {$ROM_LOC & 0x0c000000 }]]
	variable NAND_BOOT_DEVICE 	"0x08000000"
	variable NOR_BOOT_DEVICE 	"0x0c000000"

	##################################################################################
	# configure internal SRAM at 0x00000000

	# L2CTL
	# bit 0 = 0 		- L2E: L2 SRAM disabled
	# bit 2-3 = 01 		- L2SIZ: = 256K
	# bit 13-15 = 001 	- L2SRAM: Entire array is a single SRAM (265KB)
	mem [CCSR 0x20000] = 0x10010000


	# L2SRBAR0
	# bit 0-17 = BASE addr: 0x00000000
	mem [CCSR 0x20100] = 0x00000000

	# L2SRBAREA0
	# bit 28-31 = EXTENTED BASE addr: 0x00000000
	mem [CCSR 0x20104] = 0x00000000

	# L2CTL
	# bit 0 = 1 		- L2E: L2 SRAM enable
	mem [CCSR 0x20000] = 0x90010000
	
	
	# configure local access windows

	# LAWBAR0 - Local Bus NOR
	# bit 8 - 31 = 0xFF000000 - base addr
	mem [CCSR 0xc08] = 0x000FF000

	# LAWAR0
	# bit 0 = 1 - enable window
	# bit 7-11 = 00100 - Local Bus
	# bit 26 - 31 =  011000 16M - size
	mem [CCSR 0xc10] = 0x80400017


	# LAWBAR5 - CPLD
	# bit 8 - 31 = 0xEFA00000 - base addr
	mem [CCSR 0xca8] = 0x000efa00
	
	# LAWAR5
	# bit 0 = 1 - enable window
	# bit 7-11 = 00100 - Local Bus
	# bit 26-31 = 010000 128k - size
	mem [CCSR 0xcb0] = 0x80400010

	# LAWBAR6 - Local Bus NAND
	# bit 8 - 31 = 0xEFF00000 - base addr
	mem [CCSR 0xcc8] = 0x000eff00
	
	# LAWAR6
	# bit 0 = 1 - enable window
	# bit 7-11 = 00100 - IFC
	# bit 26-31 = 001011 16k - size
	mem [CCSR 0xcd0] = 0x8040000d


	#SPI init
	# SPMODE 
	mem [CCSR 0x7000] = 0x80000404 
	# SPIM - catch all events
	mem [CCSR 0x7008] = 0x0000FB00
	# SPMODE0
	mem [CCSR 0x7020] = 0x25170008

	##################################################################################
	# configure Local Bus memory controller 

	if {$BOOT_DEVICE == $NAND_BOOT_DEVICE} {
		# CS0 - NAND Flash 	
		# BR0 base address at 0xEFF00000, port size 8 bit, FCM, ECC checking and generation are enabled for FCM on full-page transfers
		mem [CCSR 0x5000] = 0xEFF00C21

		if {$ROM_LOC == $FCM_SMALL_PAGE_BOOT_LOC} {
			# boot from fcm small page boot location
			# OR0 64KB flash size, FCM (small page NAND Flash)
			mem [CCSR 0x5004] = 0xFFFF03AE
		} else {
			# boot from fcm large page boot location
			# OR0 64KB flash size, FCM (large page NAND Flash)
			mem [CCSR 0x5004] = 0xFFFF07AE
		}
		# CS1 - NOR Flash 
		# BR1 base address at 0xFF000000, port size 16 bit, GPCM, DECC disabled
		mem [CCSR 0x5008] = 0xFF001001 
		# OR1 16MB flash size
		mem [CCSR 0x500c] = 0xFF000FF7
	} elseif {$BOOT_DEVICE == $NOR_BOOT_DEVICE} {

		if {$ROM_LOC == $GPCM_8BIT_BOOT_LOC} {
			# boot from nor gpcm 8 bit boot location
			# CS0 - NOR Flash 
			# BR0 base address at 0xFF000000, port size 8 bit, GPCM, DECC disabled
			mem [CCSR 0x5000] = 0xFF000801 
		} else {
			# boot from nor gpcm 16 bit boot location (default)
			# CS0 - NOR Flash 
			# BR0 base address at 0xFF000000, port size 16 bit, GPCM, DECC disabled
			mem [CCSR 0x5000] = 0xFF001001 
		}
		# OR0 16MB flash size
		mem [CCSR 0x5004] = 0xFF000FF7
		
		# CS1 - NAND Flash 	
		# BR1 base address at 0xF8000000, port size 8 bit, FCM
		mem [CCSR 0x5008] = 0xEFF00C21
		# OR1 64KB flash size, FCM (small page NAND Flash)
		mem [CCSR 0x500c] = 0xFFF003AE
	} else {
		# boot from nor gpcm 16 bit boot location (default)
		# CS0 - NOR Flash 
		# BR0 base address at 0xFF000000, port size 16 bit, GPCM, DECC disabled
		mem [CCSR 0x5000] = 0xFF001001 

		# OR0 16MB flash size
		mem [CCSR 0x5004] = 0xFF000FF7
		
		# CS1 - NAND Flash 	
		# BR1 base address at 0xEFF00000, port size 8 bit, FCM
		mem [CCSR 0x5008] = 0xEFF00C21
		# OR1 64KB flash size, FCM (small page NAND Flash)
		mem [CCSR 0x500c] = 0xFFF003AE
	}
	
	# FMR - setting for NAND
	mem [CCSR 0x50e0] = 0x0000F020

	# CS3 - CPLD 
	# BR3 base address at 0xEFA00000, port size 8 bit, GCPM 
	mem [CCSR 0x5018] = 0xEFA00801
	# OR3 128KB flash size
	mem [CCSR 0x501c] = 0xFFFE09F7

	# LBCR
	mem [CCSR 0x50d0] = 0x4000000f
	# LCRR
	mem [CCSR 0x50d4] = 0x80000008
	

}

proc P1020RDB_init_core {} {

	global CCSRBAR
	
	variable CAM_GROUP "regPPCTLB1/"
	variable SPR_GROUP "e500 Special Purpose Registers/"
	variable GPR_GROUP "General Purpose Registers/"
	variable SSP_GROUP "Standard Special Purpose Registers/"
	
	#######################################################################
	# set interrupt vectors
	# IVPR 
	reg ${SPR_GROUP}IVPR = 0xFFFF0000 	
	# debug - (a valid instruction should exist to be fetched)
	reg ${SPR_GROUP}IVOR15 = 0x0000F000	
	# program
	reg ${SPR_GROUP}IVOR6 = 0x0000F700	

	#######################################################################
	# Set a breakpoint at the reset address 
	reg ${SPR_GROUP}IAC1 = 0xfffffffc
	reg ${SPR_GROUP}DBCR0 = 0x40800000
	reg ${SPR_GROUP}DBCR1 = 0x00000000
	reg	${SSP_GROUP}MSR = 0x02000200

	config runcontrolsync off
	go
	wait 50   
	config runcontrolsync on
	stop

	reg ${SPR_GROUP}DBCR0 = 0x41000000
	reg ${SPR_GROUP}IAC1 = 0x00000000

	##################################################################################
	# move CCSR at 0xE0000000

	# CCSRBAR
	# bit 8 - 23 - BASE_ADDR
	mem [CCSR 0x0] = 0x000e0000
	set CCSRBAR 0xe0000000

	#######################################################################
	# invalidate BR0
	mem [CCSR 0x5000] = 0x00001000	

	# ABIST off
	# L2ERRDIS[MBECCDIS]=1 L2ERRDIS[SBECCDIS]=1
	mem [CCSR 0x20e44] = 0x0000001D

	# activate debug interrupt and enable SPU
	reg MSR = 0x02000200
	
	
	##################################################################################
	#	
	#	Memory Map
	#
	#   0x00000000  0x0003FFFF 	SRAM	            256K 	
	#   0xE0000000  0xE01FFFFF  CCSR   				2M
	#   0xEFA00000  0xEFA1FFFF  CPLD         		128k
	#   0xEFF00000	0xEFF0FFFF  NAND			    64k
	#   0xFF000000	0xFFFFFFFF  LocalBus NOR FLASH	16M
	#
	##################################################################################
	
	##################################################################################
	# MMU initialization

	# define 1MB	TLB1 entry 4: 0xE0000000 - 0xE00FFFFF; CCSR Space, non cacheable, guarded
	reg ${CAM_GROUP}L2MMU_CAM4 = 0x500003CAFC080000E0000000E0000001
	
	# define 256KB  TLB1 entry 5: 0x00000000 - 0x0003FFFF; for internal chip SRAM
	reg ${CAM_GROUP}L2MMU_CAM5 = 0x400001C0FC0800000000000000000001
	
	# define 256KB  TLB entry 6: 0xEFC00000 - 0xEFCFFFFF; for CPLD, cache inhibited, guarded
	reg ${CAM_GROUP}L2MMU_CAM6 = 0x500001CAFC080000EFD00000EFD00001

	# define 1MB   TLB1 entry 7: 0xF8000000 - 0xF80FFFFF; for Local Bus, cache inhibited, guarded
	reg ${CAM_GROUP}L2MMU_CAM7 = 0x500003CAFC080000F8000000F8000001
	
	# define 16MB   TLB1 entry 8: 0xFC000000 - 0xFFFFFFFF; for Local Bus, cache inhibited, guarded
	reg ${CAM_GROUP}L2MMU_CAM8 = 0x70000FCAFC080000FF000000FF000001

	##################################################################################
	# disable Boot Page Translation Register
	mem v:0xe0000020 = 0x00000000 

	# Invalidate again BR0 to prevent flash data damage in case 
	# the boot sequencer re-enables CS0 access
	mem v:0xe0005000 = 0x00001000 
	
	init_board
    
	##################################################################################
	# Interrupt vectors initialization 
	# interrupt vectors in RAM at 0x00000000
	# IVPR (default reset value) 
	reg ${SPR_GROUP}IVPR = 0x00000000
	
	# interrupt vector offset registers 
	# IVOR0 - critical input
	reg ${SPR_GROUP}IVOR0 = 0x00000100	
	# IVOR1 - machine check
	reg ${SPR_GROUP}IVOR1 = 0x00000200	
	# IVOR2 - data storage
	reg ${SPR_GROUP}IVOR2 = 0x00000300	
	# IVOR3 - instruction storage
	reg ${SPR_GROUP}IVOR3 = 0x00000400	
	# IVOR4 - external input
	reg ${SPR_GROUP}IVOR4 = 0x00000500	
	# IVOR5 - alignment
	reg ${SPR_GROUP}IVOR5 = 0x00000600	
	# IVOR6 - program
	reg ${SPR_GROUP}IVOR6 = 0x00000700
	# IVOR8 - system call
	reg ${SPR_GROUP}IVOR8 = 0x00000c00	
	# IVOR10 - decrementer
	reg ${SPR_GROUP}IVOR10 = 0x00000900	
	# IVOR11 - fixed-interval timer interrupt
	reg ${SPR_GROUP}IVOR11 = 0x00000f00	
	# IVOR12 - watchdog timer interrupt
	reg ${SPR_GROUP}IVOR12 = 0x00000b00	
	# IVOR13 - data TLB errror
	reg ${SPR_GROUP}IVOR13 = 0x00001100	
	# IVOR14 - instruction TLB error
	reg ${SPR_GROUP}IVOR14 = 0x00001000	
	# IVOR15 - debug
	reg ${SPR_GROUP}IVOR15 = 0x00001500	
	# IVOR32 - SPE-APU unavailable
	reg ${SPR_GROUP}IVOR32 = 0x00001600	
	# IVOR33 - SPE-floating point data exception
	reg ${SPR_GROUP}IVOR33 = 0x00001700
	# IVOR34 - SPE-floating point round exception
	reg ${SPR_GROUP}IVOR34 = 0x00001800
	# IVOR35 - performance monitor
	reg ${SPR_GROUP}IVOR35 = 0x00001900	

    
   	##################################################################################
	# Debugger settings
	
	# infinite loop at program exception to prevent taking the exception again
	mem v:0x00000700 = 0x48000000
	mem v:0x00001500 = 0x48000000
	
	# enable floating point
	reg ${SSP_GROUP}MSR = 0x02001200
	
	# enable machine check
	reg ${SPR_GROUP}HID0 = 0x00004000
	
	
	#Trap debug event enable
	reg ${SPR_GROUP}DBCR0 = 0x41000000

	#  set the PC at the reset address (for debug-->connect)
	reg ${GPR_GROUP}PC = 0xFFFFFFFC
	# for debugging starting at program entry point when stack is not initialized
	reg ${GPR_GROUP}SP = 0x0000000F

}

proc envsetup {} {
	# Environment Setup
	radix x 
	config hexprefix 0x
	config MemIdentifier v
	config MemWidth 32 
	config MemAccess 32 
	config MemSwap off
}

#-------------------------------------------------------------------------------
# Main                                                                          
#-------------------------------------------------------------------------------

  envsetup
  
  P1020RDB_init_core
