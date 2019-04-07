
#ifndef __OCTEON_EBT3000_SHARED_H__
#define __OCTEON_EBT3000_SHARED_H__

#define OCTEON_EBT3000_EEPROM_ADDR  (0x52)

/* EBT3000 has one pair of DIMM sockets. DIMM 0 must be populated.
** Populating DIMM 1 enables the 128/144-bit interface.
*/
#define OCTEON_EBT3000_DRAM_SOCKET_CONFIGURATION \
	{0x50, 0x51}

#define OCTEON_EBT3000_DDR_BOARD_DELAY		5815

#endif
