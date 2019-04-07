/*
 * (C) Copyright 2001-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <net.h>
#include <miiphy.h>
#ifdef CONFIG_MACH_IPQ806x
#include <asm/arch/phy.h>
#endif

#if (CONFIG_COMMANDS & CFG_CMD_NET) && defined(CONFIG_NET_MULTI)

#ifdef CFG_GT_6426x
extern int gt6426x_eth_initialize(bd_t *bis);
#endif

extern int au1x00_enet_initialize(bd_t*);
extern int dc21x4x_initialize(bd_t*);
extern int e1000_initialize(bd_t*);
extern int eepro100_initialize(bd_t*);
extern int eth_3com_initialize(bd_t*);
extern int fec_initialize(bd_t*);
extern int inca_switch_initialize(bd_t*);
extern int mpc5xxx_fec_initialize(bd_t*);
extern int mpc8220_fec_initialize(bd_t*);
extern int mv6436x_eth_initialize(bd_t *);
extern int mv6446x_eth_initialize(bd_t *);
extern int natsemi_initialize(bd_t*);
extern int ns8382x_initialize(bd_t*);
extern int pcnet_initialize(bd_t*);
extern int plb2800_eth_initialize(bd_t*);
extern int xlr_eth_initialize(bd_t*);
extern int ppc_4xx_eth_initialize(bd_t *);
extern int rtl8139_initialize(bd_t*);
extern int rtl8169_initialize(bd_t*);
extern int scc_initialize(bd_t*);
extern int skge_initialize(bd_t*);
extern int tsec_initialize(bd_t*, int, char *);

#ifdef CONFIG_MARVELL
extern int mv_eth_initialize(bd_t *);
#ifdef MV78200
extern int mvSocUnitIsMappedToThisCpu(int unit);
#define GIGA0 4
#endif

extern unsigned int whoAmI(void);
#endif
static struct eth_device *eth_devices;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
static struct eth_device *eth_current[2];
#else
static struct eth_device *eth_current[1];
#endif
extern int octeon_eth_initialize(bd_t*);
int octeon_spi4000_initialize(bd_t * bis);
extern int ag7100_enet_initialize(bd_t*);
extern int ag7240_enet_initialize(bd_t*);
extern int ath_gmac_enet_initialize(bd_t*);

#if defined(CONFIG_MACH_IPQ806x)
/*
 * CPU and board-specific Ethernet initializations.  Aliased function
 * signals caller to move on
 */
static int __def_eth_init(bd_t *bis)
{
	return -1;
}
int cpu_eth_init(bd_t *bis) __attribute__((weak, alias("__def_eth_init")));
int board_eth_init(bd_t *bis) __attribute__((weak, alias("__def_eth_init")));

#ifdef CONFIG_API
static struct {
	uchar data[PKTSIZE];
	int length;
} eth_rcv_bufs[PKTBUFSRX];

static unsigned int eth_rcv_current, eth_rcv_last;
#endif
#endif

struct eth_device *eth_get_dev(void)
{
      unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
      cpu = whoAmI();
#endif

	return eth_current[cpu];
}

struct eth_device *eth_get_dev_by_name(char *devname)
{
	struct eth_device *dev, *target_dev;

	if (!eth_devices)
		return NULL;

	dev = eth_devices;
	target_dev = NULL;
	do {
		if (strcmp(devname, dev->name) == 0) {
			target_dev = dev;
			break;
		}
		dev = dev->next;
	} while (dev != eth_devices);

	return target_dev;
}

struct eth_device *eth_get_dev_by_index(int index)
{
	struct eth_device *dev, *target_dev;

	if (!eth_devices)
		return NULL;

	dev = eth_devices;
	target_dev = NULL;
	do {
		if (dev->index == index) {
			target_dev = dev;
			break;
		}
		dev = dev->next;
	} while (dev != eth_devices);

	return target_dev;
}

int eth_get_dev_index (void)
{
	struct eth_device *dev;
	int num = 0;
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	if (!eth_devices) {
		return (-1);
	}

	for (dev = eth_devices; dev; dev = dev->next) {
		if (dev == eth_current[cpu])
			break;
		++num;
	}

	if (dev) {
		return (num);
	}

	return (0);
}

void eth_current_changed(void)
{
	char *act = getenv("ethact");
    uint32_t cpu = 0;

	/* update current ethernet name */
	if (eth_current[cpu]) {
		if (act == NULL || strcmp(act, eth_current[cpu]->name) != 0)
			setenv("ethact", eth_current[cpu]->name);
	}
	/*
	 * remove the variable completely if there is no active
	 * interface
	 */
	else if (act != NULL)
		setenv("ethact", NULL);
}


int eth_getenv_enetaddr_by_index(const char *base_name, int index,
				 uchar *enetaddr)
{
	char enetvar[32];
	sprintf(enetvar, index ? "%s%daddr" : "%saddr", base_name, index);
	return eth_getenv_enetaddr(enetvar, enetaddr);
}

static int eth_mac_skip(int index)
{
	char enetvar[15];
	char *skip_state;
	sprintf(enetvar, index ? "eth%dmacskip" : "ethmacskip", index);
	return ((skip_state = getenv(enetvar)) != NULL);
}
int eth_unregister(struct eth_device *dev)
{
	struct eth_device *cur;
    int cpu = 0;

	/* No device */
	if (!eth_devices)
		return -1;

	for (cur = eth_devices; cur->next != eth_devices && cur->next != dev;
	     cur = cur->next)
		;

	/* Device not found */
	if (cur->next != dev)
		return -1;

	cur->next = dev->next;

	if (eth_devices == dev)
		eth_devices = dev->next == eth_devices ? NULL : dev->next;

	if (eth_current[cpu] == dev) {
		eth_current[cpu] = eth_devices;
		eth_current_changed();
	}

	return 0;
}

#ifdef CONFIG_RANDOM_MACADDR
void eth_random_enetaddr(uchar *enetaddr)
{
	uint32_t rval;

	srand(get_timer(0));

	rval = rand();
	enetaddr[0] = rval & 0xff;
	enetaddr[1] = (rval >> 8) & 0xff;
	enetaddr[2] = (rval >> 16) & 0xff;

	rval = rand();
	enetaddr[3] = rval & 0xff;
	enetaddr[4] = (rval >> 8) & 0xff;
	enetaddr[5] = (rval >> 16) & 0xff;

	/* make sure it's local and unicast */
	enetaddr[0] = (enetaddr[0] | 0x02) & ~0x01;
}
#endif

int eth_write_hwaddr(struct eth_device *dev, const char *base_name,
		   int eth_number)
{
	unsigned char env_enetaddr[6];
	int ret = 0;

	eth_getenv_enetaddr_by_index(base_name, eth_number, env_enetaddr);

	if (memcmp(env_enetaddr, "\0\0\0\0\0\0", 6)) {
		if (memcmp(dev->enetaddr, "\0\0\0\0\0\0", 6) &&
				memcmp(dev->enetaddr, env_enetaddr, 6)) {
			printf("\nWarning: %s MAC addresses don't match:\n",
				dev->name);
			printf("Address in SROM is         %pM\n",
				dev->enetaddr);
			printf("Address in environment is  %pM\n",
				env_enetaddr);
		}

		memcpy(dev->enetaddr, env_enetaddr, 6);
	}

	if (dev->write_hwaddr &&
			!eth_mac_skip(eth_number) &&
			is_valid_ether_addr(dev->enetaddr))
		ret = dev->write_hwaddr(dev);

	return ret;
}

int eth_register(struct eth_device* dev)
{
	struct eth_device *d;
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	if (!eth_devices) {
		eth_current[cpu] = eth_devices = dev;
#ifdef CONFIG_NET_MULTI
		/* update current ethernet name */
		{
			char *ea = cpu ? "ethact2" : "ethact";
			char *act = getenv(ea);
			if (act == NULL || strcmp(act, eth_current[cpu]->name) != 0)
				setenv(ea, eth_current[cpu]->name);
		}
#endif
	} else {
		for (d=eth_devices; d->next!=eth_devices; d=d->next);
		d->next = dev;
	}

	dev->state = ETH_STATE_INIT;
	dev->next  = eth_devices;

#ifdef CONFIG_EIGER
	printf("%s\n", dev->name);
#endif

	return 0;
}

#if defined(powerpc) || defined(CONFIG_IPROC)
extern void board_eth_init(bd_t *);
#endif

#if defined(CONFIG_MACH_IPQ806x)
static void eth_env_init(bd_t *bis)
{
	char *s;

	if ((s = getenv("bootfile")) != NULL)
		copy_filename(BootFile, s, sizeof(BootFile));
}
#endif


int eth_initialize(bd_t *bis)
{
	int eth_number = 0;
#ifndef CONFIG_MARVELL
	unsigned char enetvar[32], env_enetaddr[6];
	int i;
	char *tmp;
	char *end;
#endif
 	unsigned int cpu = 0;
#ifdef CONFIG_NET_MULTI
	char *ea;
#endif
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif
	ea = cpu ? "ethact2" : "ethact";

	eth_devices = NULL;
        eth_current[cpu] = NULL;

#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)
	miiphy_init();
#endif

#ifdef CONFIG_PHYLIB
	phy_init();
#endif


#if defined(powerpc) || defined(CONFIG_IPROC)
	board_eth_init(bis);
#endif
#ifdef  CONFIG_MARVELL
#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH)
      /* move to the begining so in case we have a PCI NIC it will
        read the env mac addresses correctlly. */
        mv_eth_initialize(bis);
#endif
#endif

#ifdef CONFIG_DB64360
	mv6436x_eth_initialize(bis);
#endif
#ifdef CONFIG_CPCI750
	mv6436x_eth_initialize(bis);
#endif
#ifdef CONFIG_DB64460
	mv6446x_eth_initialize(bis);
#endif
#if defined(CONFIG_4xx) && !defined(CONFIG_IOP480) && !defined(CONFIG_AP1000)
	ppc_4xx_eth_initialize(bis);
#endif
#ifdef CONFIG_INCA_IP_SWITCH
	inca_switch_initialize(bis);
#endif
#ifdef CONFIG_PLB2800_ETHER
	plb2800_eth_initialize(bis);
#endif
#ifdef CONFIG_XLR732_ETHER
	xlr_eth_initialize(bis);
#endif
#ifdef SCC_ENET
	scc_initialize(bis);
#endif
#if defined(CONFIG_MPC5xxx_FEC)
	mpc5xxx_fec_initialize(bis);
#endif
#if defined(CONFIG_MPC8220_FEC)
	mpc8220_fec_initialize(bis);
#endif
#if defined(CONFIG_SK98)
	skge_initialize(bis);
#endif
#if defined(CONFIG_MPC85XX_TSEC1)
	tsec_initialize(bis, 0, CONFIG_MPC85XX_TSEC1_NAME);
#elif defined(CONFIG_MPC83XX_TSEC1)
	tsec_initialize(bis, 0, CONFIG_MPC83XX_TSEC1_NAME);
#endif
#if defined(CONFIG_MPC85XX_TSEC2)
	tsec_initialize(bis, 1, CONFIG_MPC85XX_TSEC2_NAME);
#elif defined(CONFIG_MPC83XX_TSEC2)
	tsec_initialize(bis, 1, CONFIG_MPC83XX_TSEC2_NAME);
#endif
#if defined(CONFIG_MPC85XX_FEC)
	tsec_initialize(bis, 2, CONFIG_MPC85XX_FEC_NAME);
#else
#    if defined(CONFIG_MPC85XX_TSEC3)
	tsec_initialize(bis, 2, CONFIG_MPC85XX_TSEC3_NAME);
#    elif defined(CONFIG_MPC83XX_TSEC3)
	tsec_initialize(bis, 2, CONFIG_MPC83XX_TSEC3_NAME);
#    endif
#    if defined(CONFIG_MPC85XX_TSEC4)
	tsec_initialize(bis, 3, CONFIG_MPC85XX_TSEC4_NAME);
#    elif defined(CONFIG_MPC83XX_TSEC4)
	tsec_initialize(bis, 3, CONFIG_MPC83XX_TSEC4_NAME);
#    endif
#endif
#if defined(FEC_ENET) || defined(CONFIG_ETHER_ON_FCC)
	fec_initialize(bis);
#endif
#if defined(CONFIG_AU1X00)
	au1x00_enet_initialize(bis);
#endif
#ifdef CONFIG_E1000
	e1000_initialize(bis);
#endif
#ifdef CONFIG_EEPRO100
	eepro100_initialize(bis);
#endif
#ifdef CONFIG_TULIP
	dc21x4x_initialize(bis);
#endif
#ifdef CONFIG_3COM
	eth_3com_initialize(bis);
#endif
#ifdef CONFIG_PCNET
	pcnet_initialize(bis);
#endif
#ifdef CFG_GT_6426x
	gt6426x_eth_initialize(bis);
#endif
#ifdef CONFIG_NATSEMI
	natsemi_initialize(bis);
#endif
#ifdef CONFIG_NS8382X
	ns8382x_initialize(bis);
#endif
#if defined(CONFIG_RTL8139)
	rtl8139_initialize(bis);
#endif
#if defined(CONFIG_RTL8169)
	rtl8169_initialize(bis);
#endif
#if defined(OCTEON_RGMII_ENET)
        if (!getenv("disable_networking"))
            octeon_eth_initialize(bis);
#endif
#if defined(OCTEON_SPI4000_ENET)
        if (!getenv("disable_networking") && !getenv("disable_spi4000"))
            octeon_spi4000_initialize(bis);
#endif
#if defined(CONFIG_AR7100)
    ag7100_enet_initialize(bis);
#endif
#if defined(CONFIG_AR7240)
    ag7240_enet_initialize(bis);
#endif
#if defined(CONFIG_ATHEROS) && !defined(CONFIG_ATH_EMULATION)
	ath_gmac_enet_initialize(bis);
#endif
#if defined(CONFIG_MACH_IPQ806x)
	eth_env_init(bis);

	/*
	 * If board-specific initialization exists, call it.
	 * If not, call a CPU-specific one
	 */
	if (board_eth_init != __def_eth_init) {
		if (board_eth_init(bis) < 0) {
			printf("Board Net Initialization Failed\n");
        }
	} else if (cpu_eth_init != __def_eth_init) {
		if (cpu_eth_init(bis) < 0)
			printf("CPU Net Initialization Failed\n");
	} else
		printf("Net Initialization Skipped\n");
#endif

	if (!eth_devices) {
		puts ("No ethernet found.\n");
	} else {
		struct eth_device *dev = eth_devices;
		char *ethprime = getenv ("ethprime");

		do {
#if !defined(CONFIG_NEBBIOLO) && !defined(CONFIG_EIGER)
			if (eth_number)
				puts (", ");

			printf("%s", dev->name);
#endif

			if (ethprime && strcmp (dev->name, ethprime) == 0) {
				eth_current[cpu] = dev;
#if !defined(CONFIG_NEBBIOLO) && !defined(CONFIG_EIGER)
				puts (" [PRIME]");
#endif
			}
#ifndef  CONFIG_MARVELL

			sprintf((char *)enetvar, eth_number ? "eth%daddr" : "ethaddr", eth_number);
			tmp = getenv ((char *)enetvar);

			for (i=0; i<6; i++) {
				env_enetaddr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
				if (tmp)
					tmp = (*end) ? end+1 : end;
			}
			if (memcmp(env_enetaddr, "\0\0\0\0\0\0", 6)) {
				if (memcmp(dev->enetaddr, "\0\0\0\0\0\0", 6) &&
				    memcmp(dev->enetaddr, env_enetaddr, 6))
				{
					printf ("\nWarning: %s MAC addresses don't match:\n",
						dev->name);
					printf ("Address in SROM is         "
					       "%02X:%02X:%02X:%02X:%02X:%02X\n",
					       dev->enetaddr[0], dev->enetaddr[1],
					       dev->enetaddr[2], dev->enetaddr[3],
					       dev->enetaddr[4], dev->enetaddr[5]);
					printf ("Address in environment is  "
					       "%02X:%02X:%02X:%02X:%02X:%02X\n",
					       env_enetaddr[0], env_enetaddr[1],
					       env_enetaddr[2], env_enetaddr[3],
					       env_enetaddr[4], env_enetaddr[5]);
				}
				memcpy(dev->enetaddr, env_enetaddr, 6);
			}
#endif
			eth_number++;
			dev = dev->next;
		} while(dev != eth_devices);

#ifdef CONFIG_NET_MULTI
		/* update current ethernet name */
		if (eth_current[cpu]) {
			char *act = getenv(ea);
			if (act == NULL || strcmp(act, eth_current[cpu]->name) != 0)
				setenv(ea, eth_current[cpu]->name);
		} else
			setenv(ea, NULL);
#endif

#ifdef CONFIG_APBOOT
		putc ('\n');
#endif
	}

	return eth_number;
}

void eth_set_enetaddr(int num, char *addr) {
	struct eth_device *dev;
	unsigned char enetaddr[6];
	char *end;
	int i;

	debug ("eth_set_enetaddr(num=%d, addr=%s)\n", num, addr);

	if (!eth_devices)
		return;

	for (i=0; i<6; i++) {
		enetaddr[i] = addr ? simple_strtoul(addr, &end, 16) : 0;
		if (addr)
			addr = (*end) ? end+1 : end;
	}

	dev = eth_devices;
	while(num-- > 0) {
		dev = dev->next;

		if (dev == eth_devices)
			return;
	}

	debug ( "Setting new HW address on %s\n"
		"New Address is             %02X:%02X:%02X:%02X:%02X:%02X\n",
		dev->name,
		enetaddr[0], enetaddr[1],
		enetaddr[2], enetaddr[3],
		enetaddr[4], enetaddr[5]);

	memcpy(dev->enetaddr, enetaddr, 6);
}

int eth_init(bd_t *bis)
{
	struct eth_device* old_current;
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	if (!eth_current[cpu])
		return 0;

	old_current = eth_current[cpu];
	do {
		debug ("Trying %s\n", eth_current[cpu]->name);

		if (eth_current[cpu]->init(eth_current[cpu], bis)) {
			eth_current[cpu]->state = ETH_STATE_ACTIVE;

			return 1;
		}
		debug  ("FAIL\n");

		eth_try_another(0);
	} while (old_current != eth_current[cpu]);

	return 0;
}

void eth_halt(void)
{
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif
	
	if (!eth_current[cpu])
		return;

	eth_current[cpu]->halt(eth_current[cpu]);

	eth_current[cpu]->state = ETH_STATE_PASSIVE;
}

int eth_send(volatile void *packet, int length)
{
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	if (!eth_current[cpu])
		return -1;

	return eth_current[cpu]->send(eth_current[cpu], packet, length);
}

int eth_rx(void)
{
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif
	if (!eth_current[cpu])
		return -1;

	return eth_current[cpu]->recv(eth_current[cpu]);
}

void eth_try_another(int first_restart)
{
	static struct eth_device *first_failed = NULL;
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	if (!eth_current[cpu])
		return;

	if (first_restart) {
		first_failed = eth_current[cpu];
	}

	eth_current[cpu] = eth_current[cpu]->next;

#ifdef CONFIG_NET_MULTI
	/* update current ethernet name */
	{
		char *ea = cpu ? "ethact2" : "ethact";
		char *act = getenv(ea);
		if (act == NULL || strcmp(act, eth_current[cpu]->name) != 0)
			setenv(ea, eth_current[cpu]->name);
	}
#endif

	if (first_failed == eth_current[cpu]) {
		NetRestartWrap = 1;
	}
}

#ifdef CONFIG_NET_MULTI
void eth_set_current(void)
{
	char *act;
	struct eth_device* old_current;
	unsigned int cpu = 0;
	char *ea = cpu ? "ethact2" : "ethact";
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	if (!eth_current[cpu])	/* XXX no current */
		return;

	act = getenv(ea);
	if (act != NULL) {
		old_current = eth_current[cpu];
		do {
			if (strcmp(eth_current[cpu]->name, act) == 0)
				return;
			eth_current[cpu] = eth_current[cpu]->next;
		} while (old_current != eth_current[cpu]);
	}

	setenv(ea, eth_current[cpu]->name);
}
#endif

char *eth_get_name (void)
{
	unsigned int cpu = 0;
#if defined(MV78200) && defined(DUAL_OS_SHARED_MEM_78200)
	cpu = whoAmI();
#endif

	return (eth_current[cpu] ? eth_current[cpu]->name : "unknown");
}
#elif (CONFIG_COMMANDS & CFG_CMD_NET) && !defined(CONFIG_NET_MULTI)

extern int at91rm9200_miiphy_initialize(bd_t *bis);
extern int emac4xx_miiphy_initialize(bd_t *bis);
extern int mcf52x2_miiphy_initialize(bd_t *bis);
extern int ns7520_miiphy_initialize(bd_t *bis);

int eth_initialize(bd_t *bis)
{
#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)
	miiphy_init();
#endif

#if defined(CONFIG_AT91RM9200)
	at91rm9200_miiphy_initialize(bis);
#endif
#if defined(CONFIG_4xx) && !defined(CONFIG_IOP480) \
	&& !defined(CONFIG_AP1000) && !defined(CONFIG_405)
	emac4xx_miiphy_initialize(bis);
#endif
#if defined(CONFIG_MCF52x2)
	mcf52x2_miiphy_initialize(bis);
#endif
#if defined(CONFIG_NETARM)
	ns7520_miiphy_initialize(bis);
#endif
	return 0;
}
#endif

void eth_parse_enetaddr(const char *addr, uchar *enetaddr)
{
	char *end;
	int i;

	for (i = 0; i < 6; ++i) {
		enetaddr[i] = addr ? simple_strtoul(addr, &end, 16) : 0;
		if (addr)
			addr = (*end) ? end + 1 : end;
	}
}

int eth_getenv_enetaddr(char *name, uchar *enetaddr)
{
	eth_parse_enetaddr(getenv(name), enetaddr);
	return is_valid_ether_addr(enetaddr);
}
