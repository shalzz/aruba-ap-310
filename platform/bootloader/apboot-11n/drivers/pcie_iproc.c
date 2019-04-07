
/*
 * PCI init
 */

#include <config.h>
#ifdef CONFIG_IPROC

#include <common.h>
#include <pci_fsl.h>
#include <asm/io.h>
#include <post.h>

#include "pcie_iproc.h"


#define IN_DDR_ADDR     0x80200000

#if 0
//#define USE_PORT_0    1
unsigned long  PORT_IN_USE = 1;
unsigned long  OUT_PCI_ADDR = 0x40000000;
unsigned long  PCI_SLOT_ADDR = 0x18013000;

u32 linkError;

#if 0
#ifdef USE_PORT_1
#define PORT_IN_USE     1
#define PCI_SLOT_ADDR   0x18013000
#define OUT_PCI_ADDR    0x40000000
#elif USE_PORT_0
#define PORT_IN_USE     0
#define PCI_SLOT_ADDR   0x18012000
#define OUT_PCI_ADDR    0x08000000
#else
#define PORT_IN_USE     2
#define PCI_SLOT_ADDR   0x18014000
#define OUT_PCI_ADDR    0x48000000
#endif
#endif
//#define PCI_SLOT_ADDR   0x18012000
//#define OUT_PCI_ADDR    0x08000000
#endif

/*
 * Per port control structure
 */
static struct soc_pcie_port {
	unsigned busno;
	void * reg_base;
	void * out_pci_addr;
} soc_pcie_ports[3] = {
	{
	.busno = 0,
	.reg_base = (void *)0x18012000,
	.out_pci_addr = (void *)0x08000000,
	},
	{
	.busno = 1,
	.reg_base = (void *)0x18013000,    
	.out_pci_addr = (void *)0x40000000,
	},
	{
	.busno = 2,
	.reg_base = (void *)0x18014000,
	.out_pci_addr = (void *)0x48000000,
	}
};

#ifdef DEBUG
static int conf_trace = 1;
#else
static int conf_trace = 0;
#endif

#ifdef DEBUG
static void pci_dump_standard_conf(struct soc_pcie_port * port);

static void pci_dump_extended_conf(struct soc_pcie_port * port);
#endif

void pci_bus0_read_config_word (pci_dev_t dev, int where, unsigned short *val);
void pci_bus0_read_config_byte (pci_dev_t dev, int where, unsigned char *val);

/*
 * Check link status, return 0 if link is up in RC mode,
 * otherwise return non-zero
 */
#if 1
static int __pci_bus0_find_next_cap_ttl(unsigned int devfn,
		u8 pos, int cap, int *ttl)
{
	u8 id;

	while ((*ttl)--) {
		pci_bus0_read_config_byte( devfn, pos, &pos);
		if (pos < 0x40)
			break;
		pos &= ~3;
		pci_bus0_read_config_byte(devfn, pos + PCI_CAP_LIST_ID,
				&id);
		if (id == 0xff)
			break;
		if (id == cap)
			return pos;
		pos += PCI_CAP_LIST_NEXT;
	}
	return 0;
}
#define PCI_FIND_CAP_TTL        48

 static int __pci_bus0_find_next_cap( unsigned int devfn,
                                u8 pos, int cap)
 {
         int ttl = PCI_FIND_CAP_TTL;

         return __pci_bus0_find_next_cap_ttl( devfn, pos, cap, &ttl);
 }


static int __pci_bus0_find_cap_start(unsigned int devfn, u8 hdr_type)
 {
         u16 status;

         pci_bus0_read_config_word( devfn, PCI_STATUS, &status);
         if (!(status & PCI_STATUS_CAP_LIST))
                 return 0;

         switch (hdr_type) {
         case PCI_HEADER_TYPE_NORMAL:
         case PCI_HEADER_TYPE_BRIDGE:
                 return PCI_CAPABILITY_LIST;
         case PCI_HEADER_TYPE_CARDBUS:
                 return PCI_CB_CAPABILITY_LIST;
         default:
                 return 0;
         }

         return 0;
}
static int pci_bus0_find_capability( unsigned int devfn, int cap)
{
         int pos;
         u8 hdr_type;

         pci_bus0_read_config_byte(devfn, PCI_HEADER_TYPE, &hdr_type);

         pos = __pci_bus0_find_cap_start( devfn, hdr_type & 0x7f);
         if (pos)
                 pos = __pci_bus0_find_next_cap( devfn, pos, cap);

         return pos;
 }
//Link status register definitions



//3:0
//RO
//Link Speed. The negotiated Link speed.
//   0001b = 2.5 Gb/s
//All other encodings are reserved.

//9:4
//RO
//Negotiated Link Width. The negotiated Link width.
//   000001b = x1
//   000010b = x2
//   000100b = x4
//   001000b = x8
//   001100b = x12
//   010000b = x16
//   100000b = x32
//   All other encodings are reserved.

//10
//RO
//Training Error. 1 = indicates that a Link training error occurred. Reserved on Endpoint devices and Switch upstream ports.
//Cleared by hardware upon successful training of the Link to the L0 Link state.

//11
//RO
//Link Training. When set to one, indicates that Link training is in progress (Physical Layer LTSSM is
//in the Configuration or Recovery state) or that the Retrain Link bit was set to one but Link training has not yet begun.
//Hardware clears this bit once Link training is complete.
// This bit is not applicable and reserved on Endpoint devices and the Upstream Ports of Switches.

//12
//HWInit
//Slot Clock Configuration. This bit indicates that the component uses the same physical reference clock
//that the platform provides on the connector. If the device uses an independent clock irrespective of the
//presence of a reference on the connector, this bit must be clear.

static int soc_pcie_check_link(struct soc_pcie_port * port)
{
    u32 devfn = 0;
	u16 pos, tmp16;
	u8 nlw, tmp8;
	int linkError;


	linkError = 0;

	/* See if the port is in EP mode, indicated by header type 00 */
    pci_bus0_read_config_byte(devfn, PCI_HEADER_TYPE, &tmp8);
	if( tmp8 != PCI_HEADER_TYPE_BRIDGE ) {
		printf("PCIe port %d in End-Point mode - ignored\n");
		linkError = 1;
		return -1;
	}
    else
        printf("PCIe%u: RC, ", port->busno);

	/* NS PAX only changes NLW field when card is present */
    pos = pci_bus0_find_capability( devfn, PCI_CAP_ID_EXP);

#ifdef DEBUG
    printf(/*L*/"\n pos is %d\n", pos);
#endif

    pci_bus0_read_config_word(devfn, pos + PCI_EXP_LNKSTA, &tmp16);

#ifdef DEBUG
	printf("==>PCIE: LINKSTA reg %#x val %#x\n",
		pos+PCI_EXP_LNKSTA, tmp16 );
#endif


	nlw = (tmp16 & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT ;
	if ( nlw == 0 )
	{
		linkError = 1;
	}
	//port->link = tmp16 & PCI_EXP_LNKSTA_DLLLA ;

	//if( nlw != 0 ) port->link = 1;
#if 0
	for( ; pos < 0x100; pos += 2 )
		{
        	pci_bus0_read_config_word(devfn, pos , &tmp16);
		if( tmp16 ) printf("reg[%#x]=%#x, ", pos , tmp16 );
		}
	//printf("PCIE link=%d\n", port->link );
#endif

	return( (nlw)? 0: -1);
}
#endif

/*
 * Initializte the PCIe controller
 */
static void  soc_pcie_hw_init(struct soc_pcie_port * port)
{
	/* Turn-on Root-Complex (RC) mode, from reset defailt of EP */

	/* The mode is set by straps, can be overwritten via DMU
	   register <cru_straps_control> bit 5, "1" means RC
	 */
	/* Send a downstream reset */
#if 0
	/* if port 0 */
	if( 0x18012000 == ( port->reg_base ) )
	{
		printf("\n\n\n==============INIT ");
		if ( post_get_board_diags_type() == BCM958623HR )
		{
			printf("\n\n\n port 1 ==============\n");
			/* Send a downstream reset on port 1 - sw workaround for the perst signal */
			/* on the 23 HR board                                                     */
			__raw_writel( 0x3, 0x18013000 + SOC_PCIE_CONTROL);
			udelay(250);
			__raw_writel( 0x1, 0x18013000 + SOC_PCIE_CONTROL);
			udelay(25000);

		}

	}
#endif
	__raw_writel( 0x3, port->reg_base + SOC_PCIE_CONTROL);
	udelay(250);
	__raw_writel( 0x1, port->reg_base + SOC_PCIE_CONTROL);
	udelay(25000);


	/* TBD: take care of PM, check we're on */
}


int iproc_pcie_rd_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 * value)
{
	int cfg_type = 0;

	if (PCI_BUS(dev) == 0 && PCI_DEV(dev) > 0) {
		return 0xffffffff;
	}
	if (PCI_BUS(dev) > 0) {
		if (PCI_FUNC(dev) > 1) {
			return -1;
		}
		cfg_type = 1;
	}

	if(conf_trace) printf("Pcie_rd_conf_dword: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset) | cfg_type,
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	*value = __raw_readl(soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF);

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %08x\n\n", dev, offset, *value);

	return PCIBIOS_SUCCESSFUL;
}

int iproc_pcie_rd_conf_word(struct pci_controller *hose, pci_dev_t dev, int offset, u16 * value)
{
	unsigned int tmp;
	int cfg_type = 0;

	if (PCI_BUS(dev) == 0 && PCI_DEV(dev) > 0) {
		return 0xffffffff;
	}
	if (PCI_BUS(dev) > 0) {
		if (PCI_FUNC(dev) > 1) {
			return -1;
		}
		cfg_type = 1;
	}

	if(conf_trace) printf("Pcie_rd_conf_word: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset) | cfg_type,
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	tmp = __raw_readl(soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF);
    *value = (tmp >> (8 * (offset & 3))) & 0xffff;

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %04x\n\n", dev, offset, *value);

	return PCIBIOS_SUCCESSFUL;
}

int iproc_pcie_rd_conf_byte(struct pci_controller *hose, pci_dev_t dev, int offset, u8 * value)
{
	unsigned int tmp;
	int cfg_type = 0;

	if (PCI_BUS(dev) == 0 && PCI_DEV(dev) > 0) {
		return 0xffffffff;
	}
	if (PCI_BUS(dev) > 0) {
		if (PCI_FUNC(dev) > 1) {
			return -1;
		}
		cfg_type = 1;
	}

	if(conf_trace) printf("Pcie_rd_conf_byte: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset) | cfg_type,
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	tmp = __raw_readl(soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF);
    *value = (tmp >> (8 * (offset & 3))) & 0xff;

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %02x\n\n", dev, offset, *value);

	return PCIBIOS_SUCCESSFUL;
}

int iproc_pcie_wr_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 value)
{
	int ret = PCIBIOS_SUCCESSFUL;

	if(conf_trace) printf("Pcie_wr_conf_dword: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset),
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	__raw_writel(value, soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF);
	return ret;
}

int iproc_pcie_wr_conf_word(struct pci_controller *hose, pci_dev_t dev, int offset, u16 value)
{
	int ret = PCIBIOS_SUCCESSFUL;

	if(conf_trace) printf("Pcie_wr_conf_word: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %04x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset),
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	__raw_writew(value, soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF + (offset & 3) );
	return ret;
}

int iproc_pcie_wr_conf_byte(struct pci_controller *hose, pci_dev_t dev, int offset, u8 value)
{
	int ret = PCIBIOS_SUCCESSFUL;

	if(conf_trace) printf("Pcie_wr_conf_byte: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %02x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(offset),
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	__raw_writeb(value, soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF + (offset & 3) );
	return ret;
}


int pci_read_config_dword (pci_dev_t dev, int where, unsigned int *val)
{
	struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));
	int cfg_type = 0;

	if (PCI_BUS(dev) == 0 && PCI_DEV(dev) > 0) {
		*val = ~0;
		return 0xffffffff;
	}
	if (PCI_BUS(dev) > 0) {
		if (PCI_FUNC(dev) > 1) {
			*val = ~0;
			return -1;
		}
#if 1
		/* XXX; suppress duplicate devices */
		if (PCI_DEV(dev) > 0) {
			*val = ~0;
			return -1;
		}
#endif
		cfg_type = 1;
	}

	if (!hose) {
		*val = ~0;
		return -1;
	}

	if(conf_trace) printf("pci_read_config_dword: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where) | cfg_type,
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	*val = __raw_readl(soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF);

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %08x\n\n", dev, where, *val);

	return PCIBIOS_SUCCESSFUL;
}

int pci_read_config_word (pci_dev_t dev, int where, unsigned short *val)
{
	unsigned int tmp;
	struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));
	int cfg_type = 0;

//printf("\nRD %d/%d/%d\n", PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev));
	if (PCI_BUS(dev) == 0 && PCI_DEV(dev) > 0) {
		*val = ~0;
		return 0xffffffff;
	}
	if (PCI_BUS(dev) > 0) {
		if (PCI_FUNC(dev) > 1) {
			*val = ~0;
			return -1;
		}
#if 1
		/* XXX; suppress duplicate devices */
		if (PCI_DEV(dev) > 0) {
			*val = ~0;
			return -1;
		}
#endif
		cfg_type = 1;
	}

	if (!hose) {
		*val = ~0;
		return -1;
	}

	if(conf_trace) printf("pci_read_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where) | cfg_type,
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	tmp = __raw_readl(soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF);
    *val = (tmp >> (8 * (where & 3))) & 0xffff;

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %04x\n\n", dev, where, *val);

	return PCIBIOS_SUCCESSFUL;
}

int pci_read_config_byte (pci_dev_t dev, int where, unsigned char *val)
{
	unsigned int tmp;
	struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));
	int cfg_type = 0;

	if (PCI_BUS(dev) == 0 && PCI_DEV(dev) > 0) {
		*val = ~0;
		return -1;
	}
	if (PCI_BUS(dev) > 0) {
		if (PCI_FUNC(dev) > 1) {
			*val = ~0;
			return -1;
		}
#if 1
		/* XXX; suppress duplicate devices */
		if (PCI_DEV(dev) > 0) {
			*val = ~0;
			return -1;
		}
#endif
		cfg_type = 1;
	}

	if (!hose) {
		*val = ~0;
		return -1;
	}
	if(conf_trace) printf("pci_read_config_byte: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where) | cfg_type,
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	tmp = __raw_readl(soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF);
    *val = (tmp >> (8 * (where & 3))) & 0xff;

	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %02x\n\n", dev, where, *val);

	return PCIBIOS_SUCCESSFUL;
}


void pci_bus0_read_config_word (pci_dev_t dev, int where, unsigned short *val)
{
    unsigned int tmp;
	if(conf_trace) printf("pci_bus0_read_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where);

    __raw_writel( where & 0xffc, soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl(soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_DATA);
    *val = (tmp >> (8 * (where & 3))) & 0xffff;
	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %04x\n\n", dev, where, *val);
}

void pci_bus0_read_config_byte (pci_dev_t dev, int where, unsigned char *val)
{
    unsigned int tmp;
	if(conf_trace) printf("pci_bus0_read_config_byte: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where);

    __raw_writel( where & 0xffc, soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl(soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_DATA);
    *val = (tmp >> (8 * (where & 3))) & 0xff;
	if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %02x\n\n", dev, where, *val);
}

int pci_write_config_dword (pci_dev_t dev, int where, unsigned int val)
{
	int ret = PCIBIOS_SUCCESSFUL;
	struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));

	if (!hose) {
		return -1;
	}

	if(conf_trace) printf("pci_write_config_dword: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %08x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where),
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	__raw_writel(val, soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF);
	return ret;
}

int pci_write_config_word (pci_dev_t dev, int where, unsigned short val)
{
	int ret = PCIBIOS_SUCCESSFUL;
	struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));

	if (!hose) {
		return -1;
	}

	if(conf_trace) printf("pci_write_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %04x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where),
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	__raw_writew(val, soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF + (where & 3) );
	return ret;
}

int pci_write_config_byte (pci_dev_t dev, int where, unsigned char val)
{
	int ret = PCIBIOS_SUCCESSFUL;
	struct pci_controller *hose = pci_bus_to_hose(PCI_BUS(dev));

	if (!hose) {
		return -1;
	}

	if(conf_trace) printf("pci_write_config_byte: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %02x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val); 
	__raw_writel(PCIE_CONF_BUS( PCI_BUS(dev) ) |
					PCIE_CONF_DEV( PCI_DEV(dev) ) |
					PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
					PCIE_CONF_REG(where),
					soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_ADDR_OFF);

	__raw_writeb(val, soc_pcie_ports[PCI_BUS(dev)].reg_base + PCIE_CONF_DATA_OFF + (where & 3) );
	return ret;
}

void pci_bus0_write_config_word (pci_dev_t dev, int where, unsigned short val)
{
    unsigned int tmp;

	if(conf_trace) printf("pci_bus0_write_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %04x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val);

    __raw_writel( where & 0xffc, soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl(soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_DATA);
 
	if(conf_trace) printf("pci_bus0_write_config_word read first: dev: %08x <B%d, D%d, F%d>, where: %08x, tmp_val: %04x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, tmp);
    tmp &= ~(0xffff << (8 * (where & 3)) );
    tmp |= (val << (8 * (where & 3)) );

	if(conf_trace) printf("pci_bus0_write_config_word write back: dev: %08x <B%d, D%d, F%d>, where: %08x, tmp_val: %04x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, tmp);
	__raw_writel( where & 0xffc, soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_ADDR );
	__raw_writel(tmp, soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_DATA);

    if(conf_trace) printf("pci_bus0_write_config_word write done\n");
}

void pci_bus0_write_config_byte (pci_dev_t dev, int where, unsigned char val)
{
    unsigned int tmp;

	if(conf_trace) printf("pci_bus0_write_config_byte: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %02x\n", 
				dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val);

    __raw_writel( where & 0xffc, soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl(soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_DATA);

    tmp &= ~(0xff << (8 * (where & 3)) );
    tmp |= (val << (8 * (where & 3)) );

	__raw_writel( where & 0xffc, soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_ADDR );
	__raw_writel(tmp, soc_pcie_ports[PCI_BUS(dev)].reg_base + SOC_PCIE_EXT_CFG_DATA);
}


/*
 * Setup the address translation
 */
static void soc_pcie_map_init(struct soc_pcie_port * port)
{
	unsigned size, i ;
	u32 addr;

	/*
	 * NOTE:
	 * All PCI-to-CPU address mapping are 1:1 for simplicity
	 */

	/* Outbound address translation setup */
	size = SZ_128M;
	addr = (unsigned)port->out_pci_addr;

	for(i=0 ; i < 3; i++)
		{
		const unsigned win_size = SZ_64M;
		/* 64-bit LE regs, write low word, high is 0 at reset */
		__raw_writel( addr,	port->reg_base + SOC_PCIE_SYS_OMAP(i));
		__raw_writel( addr|0x1,	port->reg_base + SOC_PCIE_SYS_OARR(i));
		addr += win_size;
		if( size >= win_size )
			size -= win_size;
		if( size == 0 )
			break;
		}

	/* 
	 * Inbound address translation setup
	 * Northstar only maps up to 128 MiB inbound, DRAM could be up to 1 GiB.
	 *
	 * For now allow access to entire DRAM, assuming it is less than 128MiB,
	 * otherwise DMA bouncing mechanism may be required.
	 * Also consider DMA mask to limit DMA physical address
	 */

	size = SZ_64M;
	addr = IN_DDR_ADDR;

	size >>= 20;	/* In MB */
	size &= 0xff;	/* Size is an 8-bit field */

	/* 64-bit LE regs, write low word, high is 0 at reset */
	__raw_writel(addr | size | 0x1,
		port->reg_base + SOC_PCIE_SYS_IMAP1(0));
	__raw_writel(addr | 0x1,
		port->reg_base + SOC_PCIE_SYS_IARR(1));
}


/*
 * Setup PCIE Host bridge
 */
static void soc_pcie_bridge_init(struct soc_pcie_port * port)
{
        u32 devfn = PCI_BDF(port->busno, 0, 0);
        u8 tmp8;
	    u16 tmp16;
        unsigned base_addr;

	base_addr = (unsigned)port->out_pci_addr;

	    /* Fake <bus> object */
        pci_bus0_read_config_byte(devfn, PCI_PRIMARY_BUS, &tmp8);
        pci_bus0_read_config_byte(devfn, PCI_SECONDARY_BUS, &tmp8);
        pci_bus0_read_config_byte(devfn, PCI_SUBORDINATE_BUS, &tmp8);

        pci_bus0_write_config_byte(devfn, PCI_PRIMARY_BUS, 0);
        pci_bus0_write_config_byte(devfn, PCI_SECONDARY_BUS, 1);
        pci_bus0_write_config_byte(devfn, PCI_SUBORDINATE_BUS, 1);

        pci_bus0_read_config_byte(devfn, PCI_PRIMARY_BUS, &tmp8);
        pci_bus0_read_config_byte(devfn, PCI_SECONDARY_BUS, &tmp8);
        pci_bus0_read_config_byte(devfn, PCI_SUBORDINATE_BUS, &tmp8);

#ifdef DEBUG
        printf("membase %#x memlimit %#x\n", 
               base_addr, base_addr + SZ_128M);
#endif

        pci_bus0_read_config_word(devfn, PCI_CLASS_DEVICE, &tmp16);
        pci_bus0_read_config_word(devfn, PCI_MEMORY_BASE, &tmp16);
        pci_bus0_read_config_word(devfn, PCI_MEMORY_LIMIT, &tmp16);

        pci_bus0_write_config_word(devfn, PCI_MEMORY_BASE, 
               base_addr >> 16 );
        pci_bus0_write_config_word(devfn, PCI_MEMORY_LIMIT, 
               (base_addr + SZ_128M) >> 16 );

	/* Force class to that of a Bridge */
        pci_bus0_write_config_word(devfn, PCI_CLASS_DEVICE,
		PCI_CLASS_BRIDGE_PCI);

        pci_bus0_read_config_word(devfn, PCI_CLASS_DEVICE, &tmp16);
        pci_bus0_read_config_word(devfn, PCI_MEMORY_BASE, &tmp16);
        pci_bus0_read_config_word(devfn, PCI_MEMORY_LIMIT, &tmp16);
	
}

void pci_set_ops(struct pci_controller *hose,
				   int (*read_byte)(struct pci_controller*,
						    pci_dev_t, int where, u8 *),
				   int (*read_word)(struct pci_controller*,
						    pci_dev_t, int where, u16 *),
				   int (*read_dword)(struct pci_controller*,
						     pci_dev_t, int where, u32 *),
				   int (*write_byte)(struct pci_controller*,
						     pci_dev_t, int where, u8),
				   int (*write_word)(struct pci_controller*,
						     pci_dev_t, int where, u16),
				   int (*write_dword)(struct pci_controller*,
						      pci_dev_t, int where, u32)) {
	hose->read_byte   = read_byte;
	hose->read_word   = read_word;
	hose->read_dword  = read_dword;
	hose->write_byte  = write_byte;
	hose->write_word  = write_word;
	hose->write_dword = write_dword;
}


void pci_iproc_init_board (struct pci_controller * hose, int busno)
{
	soc_pcie_hw_init(&soc_pcie_ports[busno]);

	hose->first_busno = busno;
	hose->last_busno = busno;
	hose->current_busno = busno;

	pci_set_ops(hose,
		    iproc_pcie_rd_conf_byte,
		    iproc_pcie_rd_conf_word,
		    iproc_pcie_rd_conf_dword,
		    iproc_pcie_wr_conf_byte,
		    iproc_pcie_wr_conf_word,
		    iproc_pcie_wr_conf_dword );

	pci_register_hose(hose);

	udelay(1000);

    soc_pcie_map_init(&soc_pcie_ports[busno]);

    if( soc_pcie_check_link(&soc_pcie_ports[busno]) != 0 )
	{
//    	printf("\n**************\n port %d is not active!!\n**************\n",busno);
	printf("link down\n");
		return;
	}
	printf("link up\n");
//    printf("\n**************\n port %d is active!!\n**************\n",busno);
        
    soc_pcie_bridge_init( &soc_pcie_ports[busno]);

    //hose->last_busno = pci_hose_scan(hose);
//    hose->last_busno = 1;

	pci_hose_config_device(hose, PCI_BDF(busno,0,0), 0, (unsigned)soc_pcie_ports[busno].out_pci_addr, 0x146);

    /* Set IRQ */
	pci_bus0_write_config_word(PCI_BDF(busno,0,0), 0x3c, 0x1a9);
	pci_write_config_word(PCI_BDF(busno,0,0), 0x3c, 0x1a9);

    /* Set bridge */
	pci_bus0_write_config_byte(PCI_BDF(busno,0,0), 0x1b, 0x00);  // latency timer
	pci_bus0_write_config_word(PCI_BDF(busno,0,0), 0x28, 0x00);  // prefetch_base
	pci_bus0_write_config_word(PCI_BDF(busno,0,0), 0x2a, 0x00);
	pci_bus0_write_config_word(PCI_BDF(busno,0,0), 0x4, 0x146);  // cmd

#ifdef CONFIG_PCI_SCAN_SHOW
#ifndef CONFIG_APBOOT
    printf("\nExtended Config\n");
    pci_dump_extended_conf(&soc_pcie_ports[busno]);

	printf("\nStandard Config\n");
    pci_dump_standard_conf(&soc_pcie_ports[busno]);


	printf("PCI:   Bus Dev VenId DevId Class Int\n");
#endif
#endif
	hose->last_busno = pci_hose_scan_bus(hose, hose->current_busno);

//	printf("Done PCI initialization\n");
//    return 0;
}


#ifdef DEBUG
static void pci_dump_standard_conf(struct soc_pcie_port * port)
{
    unsigned int i, val, cnt;
//	unsigned short val16;
//	unsigned char  val8;

	/* Disable trace */
	conf_trace = 0;

    for(i=0; i<64; i++)
    {      
		iproc_pcie_rd_conf_dword(NULL, PCI_BDF(port->busno, 0, 0), i*4, &val);

		cnt = i % 4;
		if(cnt==0) printf("i=%d <%x> \t 0x%08x \t", i, i, val);
		else if(cnt==3) printf("0x%08x \n", val);
		else printf("0x%08x \t", val);
	}
	printf("\n");

    for(i=0; i<6; i++)
    {      
		iproc_pcie_rd_conf_dword(NULL, PCI_BDF(port->busno, 0, 0), 0x10+i*4, &val);
		printf(" BAR-%d: 0x%08x\n\n", i, val);
	}
}

static void pci_dump_extended_conf(struct soc_pcie_port * port)
{
    unsigned int i, /*val,*/ cnt;
	unsigned short val16;

	conf_trace = 0;

    for(i=0; i<128; i++)
    {
		pci_bus0_read_config_word(PCI_BDF(port->busno, 0, 0), i*2, &val16);

		cnt = i % 8;
		if(cnt==0) printf("i=%d <%x> \t 0x%04x  ", i, i, val16);
		else if(cnt==7) printf("0x%04x \n", val16);
		else printf("0x%04x  ", val16);
	}
	printf("\n");
}
#endif

int pci_skip_dev(struct pci_controller *hose, pci_dev_t dev)
{
	return 0;
}

#ifdef CONFIG_PCI_SCAN_SHOW
int pci_print_dev(struct pci_controller *hose, pci_dev_t dev)
{
	return 1;
}
#endif /* CONFIG_PCI_SCAN_SHOW */
#endif /* CONFIG_IPROC */

