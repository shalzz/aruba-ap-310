/*
 * (C) Copyright 2006
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
 * @file octeon_pci.h
 *
 * $Id: octeon_pci.h,v 1.3 2006/10/20 21:21:08 rfranz Exp $
 * 
 */
 

void pci_dev_post_init(void);


int octeon_pci_io_readb (unsigned int reg);
void octeon_pci_io_writeb (int value, unsigned int reg);
int octeon_pci_io_readw (unsigned int reg);
void octeon_pci_io_writew (int value, unsigned int reg);
int octeon_pci_io_readl (unsigned int reg);
void octeon_pci_io_writel (int value, unsigned int reg);
int octeon_pci_mem1_readb (unsigned int reg);
void octeon_pci_mem1_writeb (int value, unsigned int reg);
int octeon_pci_mem1_readw (unsigned int reg);
void octeon_pci_mem1_writew (int value, unsigned int reg);
int octeon_pci_mem1_readl (unsigned int reg);
void octeon_pci_mem1_writel (int value, unsigned int reg);



/* NOTE: These assume that BAR 1 is set up with a base address
** of 0, which 1-1 maps the low 128 MBytes of PCI bus memory space
** to the low 128 MBytes of Octeon DRAM */
#define octeon_pci_phys_to_bus(a)	((a) & 0x3fffffff)
#define octeon_pci_bus_to_phys(a)	((a) | (1 << 31))
