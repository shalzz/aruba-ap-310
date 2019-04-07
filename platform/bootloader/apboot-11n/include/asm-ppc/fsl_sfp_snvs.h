/*
 * Copyright 2010-2011 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _FSL_SFP_SNVS_
#define _FSL_SFP_SNVS_

#include <common.h>

/* Number of SRKH registers */
#define NUM_SRKH_REGS	8

/*
 * SNVS read. This specifies the possible reads
 * from the SNVS
 */
enum {
	SNVS_SSM_ST,
	SNVS_SW_FSV,
	SNVS_SW_SV,
};

void branch_to_self(void);
int change_sec_mon_state(uint32_t initial_state, uint32_t final_state);
void generate_reset_req(void);

#endif
