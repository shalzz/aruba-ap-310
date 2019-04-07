/*****************************************************************************
* Copyright 2011 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL"). 
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*****************************************************************************/


#ifndef __IPROC_H
#define __IPROC_H 1
#include <linux/types.h>
#include <asm/arch/iproc_regs.h>

/* APIs */
/* Function to get current CPU id */
unsigned int get_cpuid(void);

/* Function to enable interrupt at distributor level */
void gic_enable_interrupt(uint8_t intid);
/* Function to disable interrupt at distributor level */
void gic_disable_interrupt(uint8_t intid);
/* Function to set Software Generated Interrupt */
void gic_set_sgi(uint8_t intid, uint8_t filter, uint8_t targets);
/* Function to configure the interrupt properties in GIC distributor */
void gic_config_interrupt(uint8_t intid, uint8_t secure_mode, uint8_t trig_mode, uint8_t priority, uint8_t targets);
/* Function to install interrupt handler */
void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data);
void systick_isr(void *data);
int l2cc_enable();
void scu_enable();

#endif /*__IPROC_H */