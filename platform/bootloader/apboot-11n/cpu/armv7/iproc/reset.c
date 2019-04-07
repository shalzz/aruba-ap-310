/*
 * See file CREDITS for list of people who contributed to this
 * project.
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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/iproc_regs.h>
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
#define RESET_BASE_ADDR CRU_RESET_OFFSET 
#else
#define RESET_BASE_ADDR DMU_CRU_RESET 
#endif
/*
 * Reset the cpu by setting software reset request bit
 */
void reset_cpu(ulong ignored)
{
#ifdef GPIO_HW_RESET
	extern void gpio_set_as_output(unsigned);
	extern void gpio_out(unsigned, unsigned);
	gpio_set_as_output(GPIO_HW_RESET);
	udelay (50000);				/* wait 50 ms */
	gpio_out(GPIO_HW_RESET, 0);
	udelay (10000);				/* wait 10 ms */
#endif
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
	*(unsigned int *)(RESET_BASE_ADDR) = 0x1;
#else
	*(unsigned int *)(RESET_BASE_ADDR) = 0; /* Reset all */
#endif
	while (1) {
		// Never reached
	}
}
