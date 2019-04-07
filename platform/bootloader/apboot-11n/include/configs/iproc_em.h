#ifndef __IPROC_EM_H
#define __IPROC_EM_H

#include "iproc_board.h" 

#define BOARD_LATE_INIT

#define CONFIG_PHYS_SDRAM_1_SIZE			0x01000000 /* 16 MB */

/* Backup uboot uses same environment area as primary uboot */
#define CONFIG_ENV_OFFSET	0x180000	/* 30000-b0000 - use last 10000 for env */

#endif /* __IPROC_EM_H */



	
