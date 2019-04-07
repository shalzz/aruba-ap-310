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

#include <asm/fsl_sfp_snvs.h>
#include <configs/corenet_ds.h>
#include <common.h>

int change_sec_mon_state(u32 initial_state, u32 final_state)
{
	ccsr_snvs_regs_t *snvs_regs = (void *)(CONFIG_SYS_SNVS_ADDR);
	u32 sts = in_be32(&snvs_regs->hp_stat);
	int timeout = 10;

	if ((sts & HPSR_SSM_ST_MASK) != initial_state)
		return -1;

	if (initial_state == HPSR_SSM_ST_TRUST) {
		switch (final_state) {
		case HPSR_SSM_ST_NON_SECURE:
			printf("SNVS state transitioning to Soft Fail.\n");
			setbits_be32(&snvs_regs->hp_com,
				(in_be32(&snvs_regs->hp_com) | HPCOMR_SW_SV));

			/*
			 * poll till SNVS is in
			 * Soft Fail state
			 */
			while (((sts & HPSR_SSM_ST_MASK) !=
				HPSR_SSM_ST_SOFT_FAIL)) {
				while (timeout) {
					sts = in_be32(&snvs_regs->hp_stat);

					if ((sts & HPSR_SSM_ST_MASK) ==
						HPSR_SSM_ST_SOFT_FAIL)
						break;

					udelay(10);
					timeout--;
				}
			}

			if (timeout == 0) {
				printf("SNVS state transition timeout.\n");
				branch_to_self();
			}

			timeout = 10;

			printf("SNVS state transitioning to Non Secure.\n");
			setbits_be32(&snvs_regs->hp_com,
				(in_be32(&snvs_regs->hp_com) | HPCOMR_SSM_ST));

			/*
			 * poll till SNVS is in
			 * Non Secure state
			 */
			while (((sts & HPSR_SSM_ST_MASK) !=
				HPSR_SSM_ST_NON_SECURE)) {
				while (timeout) {
					sts = in_be32(&snvs_regs->hp_stat);

					if ((sts & HPSR_SSM_ST_MASK) ==
						HPSR_SSM_ST_NON_SECURE)
						break;

					udelay(10);
					timeout--;
				}
			}

			if (timeout == 0) {
				printf("SNVS state transition timeout.\n");
				branch_to_self();
			}
			break;
		case HPSR_SSM_ST_SOFT_FAIL:
			printf("SNVS state transitioning to Soft Fail.\n");
			setbits_be32(&snvs_regs->hp_com,
				(in_be32(&snvs_regs->hp_com) | HPCOMR_SW_FSV));

			/*
			 * polling loop till SNVS is in
			 * Soft Fail state
			 */
			while (((sts & HPSR_SSM_ST_MASK) !=
				HPSR_SSM_ST_SOFT_FAIL)) {
				while (timeout) {
					sts = in_be32(&snvs_regs->hp_stat);

					if ((sts & HPSR_SSM_ST_MASK) ==
						HPSR_SSM_ST_SOFT_FAIL)
						break;

					udelay(10);
					timeout--;
				}
			}

			if (timeout == 0) {
				printf("SNVS state transition timeout.\n");
				branch_to_self();
			}
			break;
		default:
			return -1;
		}
	} else if (initial_state == HPSR_SSM_ST_NON_SECURE) {
		switch (final_state) {
		case HPSR_SSM_ST_SOFT_FAIL:
			printf("SNVS state transitioning to Soft Fail.\n");
			setbits_be32(&snvs_regs->hp_com,
				(in_be32(&snvs_regs->hp_com) | HPCOMR_SW_FSV));

			/*
			 * polling loop till SNVS is in
			 * Soft Fail state
			 */
			while (((sts & HPSR_SSM_ST_MASK) !=
				HPSR_SSM_ST_SOFT_FAIL)) {
				while (timeout) {
					sts = in_be32(&snvs_regs->hp_stat);

					if ((sts & HPSR_SSM_ST_MASK) ==
						HPSR_SSM_ST_SOFT_FAIL)
						break;

					udelay(10);
					timeout--;
				}
			}

			if (timeout == 0) {
				printf("SNVS state transition timeout.\n");
				branch_to_self();
			}
			break;
		default:
			return -1;
		}
	}

	return 0;
}

void generate_reset_req(void)
{
	ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	printf("Generating reset request");
	out_be32(&gur->rstcr, 0x2);	/* HRESET_REQ */
	branch_to_self();
}
