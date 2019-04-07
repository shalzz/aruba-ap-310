/***********************************************************************
Copyright 2003-2006 Raza Microelectronics, Inc.(RMI). All rights
reserved.
Use of this software shall be governed in all respects by the terms and
conditions of the RMI software license agreement ("SLA") that was
accepted by the user as a condition to opening the attached files.
Without limiting the foregoing, use of this software in source and
binary code forms, with or without modification, and subject to all
other SLA terms and conditions, is permitted.
Any transfer or redistribution of the source code, with or without
modification, IS PROHIBITED,unless specifically allowed by the SLA.
Any transfer or redistribution of the binary code, with or without
modification, is permitted, provided that the following condition is
met:
Redistributions in binary form must reproduce the above copyright
notice, the SLA, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution:
THIS SOFTWARE IS PROVIDED BY Raza Microelectronics, Inc. `AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL RMI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*****************************#RMI_3#***********************************/
#ifndef __PCI_H__
#define __PCI_H__


#ifdef __MIPSEB__
#define PCI_SWAP32(x) __swab32(x)
#else
#define PCI_SWAP32(x) (x)
#endif

#define MAX_PCI_DEVS 32
#define MAX_PCI_FNS 8
#define PCI_NUM_SPACE 12
#define PCI_ANY_ID 0xffff

#define PCIE_IO_BASE        0x10000000
#define PCIE_IO_SIZE        0x01000000  // 16MB
#define PCIE_MEM_BASE       0x14000000
#define PCIE_MEM_SIZE       0x04000000  // 64MB
#define PCIE_CFG_BASE       0x18000000
#define PCIE_CFG_SIZE       0x02000000  // 32MB

//struct addr_space {
//	char name[40];
//	unsigned long base;
//	int size;
//	unsigned long flags;
//};

/* 
   In order to scan a PCI bus, we need the following information:
   1. Configuration Space address.
   2. Functions for PCI Configuaration Space read/write.
   3. PCI memory space base and size
   4. PCI I/O space base and size.
   5. Bus number
*/
struct pci_config_ops
{
	int (*_pci_cfg_r8) (unsigned long config_base,int reg, uint8_t *val);
	int (*_pci_cfg_r16)(unsigned long config_base,int reg, uint16_t *val);
	int (*_pci_cfg_r32)(unsigned long config_base,int reg, uint32_t *val);

	int (*_pci_cfg_w8) (unsigned long config_base,int reg, uint8_t val);
	int (*_pci_cfg_w16)(unsigned long config_base,int reg, uint16_t val);
	int (*_pci_cfg_w32)(unsigned long config_base,int reg, uint32_t val);
};

struct pci_device
{
    char name[90];
	char controller_name[10];
	uint32_t bus;
    uint32_t dev;
    uint32_t fn;
    
    uint16_t vendor;
    uint16_t device;
    uint32_t class;
    uint8_t  hdr_type;
    uint8_t  irq;

	unsigned long config_base;
	struct pci_config_ops *pci_ops;

	struct addr_space bars[PCI_NUM_SPACE];

	int (*driver)(struct pci_device *this_dev);

	struct pci_device *parent;

	struct pci_device *children;

	struct pci_device *next;
	struct pci_device *prev;
};
struct pci_device_id {
	unsigned int vendor;
	unsigned int device;
};


#define pci_reg_addr(pci_dev,reg) \
	( ( (((pci_dev)->bus) <<16) | ( ( (pci_dev)->dev) << 11)  | ( ( (pci_dev)->fn) << 8)) + (reg) )

#define pci_cfg_r8(dev,reg,val) \
	(((dev)->pci_ops)->_pci_cfg_r8((dev)->config_base, pci_reg_addr((dev),(reg)), (val)))
#define pci_cfg_r16(dev,reg,val) \
	(((dev)->pci_ops)->_pci_cfg_r16((dev)->config_base, pci_reg_addr((dev),(reg)), (val)))
#define pci_cfg_r32(dev,reg,val) \
	(((dev)->pci_ops)->_pci_cfg_r32((dev)->config_base, pci_reg_addr((dev),(reg)), (val)))

#define pci_cfg_w8(dev,reg,val) \
	(((dev)->pci_ops)->_pci_cfg_w8((dev)->config_base, pci_reg_addr((dev),(reg)), (val)))
#define pci_cfg_w16(dev,reg,val) \
	(((dev)->pci_ops)->_pci_cfg_w16((dev)->config_base, pci_reg_addr((dev),(reg)), (val)))
#define pci_cfg_w32(dev,reg,val) \
	(((dev)->pci_ops)->_pci_cfg_w32((dev)->config_base, pci_reg_addr((dev),(reg)), (val)))


extern int pci_find_cap(struct pci_device *this_dev, int cap);
extern int pci_dev_enable(struct pci_device *this_dev);
extern int enumerate_pci_bus(struct pci_device *parent,
			     struct addr_space *available_mem,
			     struct addr_space *available_io);
extern struct pci_device *pci_find_dev(unsigned short int vendor, 
					  unsigned short int device,
					  struct pci_device *start);
#endif
