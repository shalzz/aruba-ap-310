/*
 * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.
 */

#include <common.h>
#include <net.h>
#include <asm-generic/errno.h>
#include <asm/io.h>
#include <asm/arch-ipq806x/nss/msm_ipq806x_gmac.h>
#include <asm/arch-ipq806x/gpio.h>
#include <malloc.h>
#include <asm/arch/phy.h>
#include <asm/arch-ipq806x/nss/clock.h>
#include <asm/arch-ipq806x/nss/nss_reg.h>
#include <asm/arch-ipq806x/gpio.h>
#include <asm/arch-ipq806x/clock.h>
#include "ipq_gmac.h"
#include "../board/qca/common/athrs17_phy.h"
#include <miiphy.h>

#define ipq_info	printf
#define ipq_dbg		printf
#define DESC_SIZE   (sizeof(ipq_gmac_desc_t))
#define DESC_FLUSH_SIZE    (((DESC_SIZE + (CONFIG_SYS_CACHELINE_SIZE - 1)) \
                       / CONFIG_SYS_CACHELINE_SIZE) * \
                   (CONFIG_SYS_CACHELINE_SIZE))

struct ipq_eth_dev *ipq_gmac_macs[IPQ_GMAC_NMACS];

static void ipq_gmac_mii_clk_init(struct ipq_eth_dev *priv, uint clk_div,
                        ipq_gmac_board_cfg_t *gmac_cfg);
int ipq_mdio_read_wrap(char *devname, unsigned char addr, unsigned char reg, unsigned short *value);
int ipq_mdio_write_wrap(char *devname, unsigned char addr, unsigned char reg, unsigned short value);

static void config_auto_neg(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	ipq_mdio_write(priv->phy_address[0],
			PHY_CONTROL_REG,
			AUTO_NEG_ENABLE);
}

#ifdef CONFIG_EIGER
static int eiger_sfp_sgmii = 0;
static int eiger_sfp_down = 0;
extern void gpio_out(unsigned pin, unsigned value);
static int ipq_phy_link_status(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	int port_status;
	ushort phy_status;
	int retry = 0;

	/* Marvel 88E1512 on GMAC1: AMD Copper/SGMII  */
	if (priv->mac_unit == GMAC_UNIT1) {
		while (retry < 200) {
			/* SGMII */
			ipq_mdio_write(priv->phy_address[0], PHY_EXT_ADDR_CBL_DIAG, 1);
			udelay(1000);
			ipq_mdio_read(priv->phy_address[0], PHY_SPECIFIC_STATUS_REG, &phy_status);
			/* resolved ? */
			if (phy_status & (PHY_SPEED_DUPLEX_RESOLVED_MASK << PHY_SPEED_DUPLEX_RESOLVED_SHIFT)) {
				port_status = ((phy_status & Mii_phy_status_link_up) >> (MII_PHY_STAT_SHIFT));
				priv->speed = (phy_status >> PHY_SPEED_SHIFT) & PHY_SPEED_MASK;
				priv->duplex = (phy_status >> PHY_DUPLEX_SHIFT) & PHY_DUPLEX_MASK;

				if (port_status == 1) {
					eiger_sfp_down = 0;
					priv->mode = PHY_MEDIA_SGMII;
					return 0;
				}
			}

#if 0
			/* Copper */
			ipq_mdio_write(priv->phy_address[0], PHY_EXT_ADDR_CBL_DIAG, 0);
			udelay(1000);
			ipq_mdio_read(priv->phy_address[0], PHY_SPECIFIC_STATUS_REG, &phy_status);
			/* resolved ? */
			if (phy_status & (PHY_SPEED_DUPLEX_RESOLVED_MASK << PHY_SPEED_DUPLEX_RESOLVED_SHIFT)) {
				port_status = ((phy_status & Mii_phy_status_link_up) >> (MII_PHY_STAT_SHIFT));
				priv->speed = (phy_status >> PHY_SPEED_SHIFT) & PHY_SPEED_MASK;
				priv->duplex = (phy_status >> PHY_DUPLEX_SHIFT) & PHY_DUPLEX_MASK;
				if (port_status == 1) {
					priv->mode = PHY_MEDIA_COPPER;
					return 0;
				}
			}
#endif

			retry++;
			udelay(2000);
		}
		eiger_sfp_down++;
		if (eiger_sfp_down >= 3) {
			gpio_out(GPIO_PHY_0_RST_L, 0);
			eiger_sfp_sgmii = 1 - eiger_sfp_sgmii;
			eiger_sfp_down = 0;
			udelay(10000);
			gpio_out(GPIO_PHY_0_RST_L, 1);
			m88e1512_reg_init(priv->phy_address[0], eiger_sfp_sgmii, 1);
		}
	}
	/* CM on GMAC0: no PHY, always UP */
	else if (priv->mac_unit == GMAC_UNIT0) {
		priv->speed = SPEED_1000M;
		priv->duplex = 1;
		priv->mode = PHY_MEDIA_COPPER;
		return 0;
	}
	return -1;
}
#else
static int ipq_phy_link_status(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	int port_status;
	ushort phy_status;
	uint i;

	udelay(1000);

	for (i = 0; i < priv->no_of_phys; i++) {
		ipq_mdio_read(priv->phy_address[i], PHY_SPECIFIC_STATUS_REG,
				&phy_status);
		port_status = ((phy_status & Mii_phy_status_link_up) >>
				(MII_PHY_STAT_SHIFT));
		if (port_status == 1)
			return 0;
	}

	return -1;
}
#endif

static void get_phy_speed_duplexity(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	uint phy_status;
	uint start;
	const uint timeout = 2000;

	start = get_timer(0);
	while (get_timer(start) < timeout) {
#if defined(CONFIG_APBOOT)
		if (!ipq_phy_link_status(dev)) {

			phy_status = (uint)miiphy_speed(dev->name, priv->phy_address[0]);
			switch (phy_status)
			{
				case _1000BASET:
					priv->speed = SPEED_1000M;
				break;
				case _100BASET:
					priv->speed = SPEED_100M;
				break;
				case _10BASET:
					priv->speed = SPEED_10M;
				break;
				default:
					priv->speed = SPEED_10M;
			}

			phy_status = (uint)miiphy_duplex(dev->name, priv->phy_address[0]);
			switch (phy_status)
			{
				case FULL:
					priv->duplex = 1;
				break;
				case HALF:
					priv->duplex = 0;
				break;
				default:
					priv->duplex = 0;
			}

            break;
		}
#else
		phy_status = readl(QSGMII_REG_BASE +
				PCS_QSGMII_MAC_STAT);

		if (PCS_QSGMII_MAC_LINK(phy_status, priv->mac_unit)) {

			priv->speed =
			PCS_QSGMII_MAC_SPEED(phy_status,
			priv->mac_unit);

			priv->duplex =
			PCS_QSGMII_MAC_DUPLEX(phy_status,
			priv->mac_unit);

			if (priv->duplex)
				ipq_info("Full duplex link\n");
			else
				ipq_info("Half duplex link\n");

			ipq_info("Link %x up, Phy_status = %x\n",
			priv->mac_unit,phy_status);

			break;
		}
#endif
		udelay(10);
	}
}

static int ipq_eth_wr_macaddr(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	struct eth_mac_regs *mac_p = (struct eth_mac_regs *)priv->mac_regs_p;
	u32 macid_lo, macid_hi;
	u8 *mac_id = &dev->enetaddr[0];

	macid_lo = mac_id[0] + (mac_id[1] << 8) +
		   (mac_id[2] << 16) + (mac_id[3] << 24);
	macid_hi = mac_id[4] + (mac_id[5] << 8);

	writel(macid_hi, &mac_p->macaddr0hi);
	writel(macid_lo, &mac_p->macaddr0lo);

	return 0;
}

#ifdef CONFIG_EIGER
static int ipq_mac_reset(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	struct eth_dma_regs *dma_reg = (struct eth_dma_regs *)priv->dma_regs_p;
	u32 val, tout = 0;

	writel(DMAMAC_SRST, &dma_reg->busmode);
	do {
		udelay(100);
		val = readl(&dma_reg->busmode);
		tout++;
		if ((tout >= 10000) && (priv->mac_unit == GMAC_UNIT0)) {
			return -1;
		}
	} while (val & DMAMAC_SRST);
	return 0;

}
#else
static void ipq_mac_reset(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	struct eth_dma_regs *dma_reg = (struct eth_dma_regs *)priv->dma_regs_p;
	u32 val;

	writel(DMAMAC_SRST, &dma_reg->busmode);
	do {
		udelay(10);
		val = readl(&dma_reg->busmode);
	} while (val & DMAMAC_SRST);

}
#endif

static void ipq_eth_mac_cfg(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	struct eth_mac_regs *mac_reg = (struct eth_mac_regs *)priv->mac_regs_p;

	uint ipq_mac_cfg;
	//uint ipq_frame_filter_cfg;

	if (priv->mac_unit > GMAC_UNIT1) {
		ipq_mac_cfg = (priv->mac_ps | FULL_DUPLEX_ENABLE);
	} else {
		ipq_mac_cfg = (GMII_PORT_SELECT | FULL_DUPLEX_ENABLE);
	}

	ipq_mac_cfg |= (FRAME_BURST_ENABLE | TX_ENABLE | RX_ENABLE);

	//ipq_frame_filter_cfg = GMAC_FRAME_RX_ALL | PROMISCUOUS_MODE_ON;

	writel(ipq_mac_cfg, &mac_reg->conf);
	//writel(ipq_frame_filter_cfg, &mac_reg->framefilt);
}

static void ipq_eth_dma_cfg(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	struct eth_dma_regs *dma_reg = (struct eth_dma_regs *)priv->dma_regs_p;
	uint ipq_dma_bus_mode;
	uint ipq_dma_op_mode;

	ipq_dma_op_mode = DmaStoreAndForward | DmaRxThreshCtrl128 |
				DmaTxSecondFrame;
	ipq_dma_bus_mode = DmaFixedBurstEnable | DmaBurstLength16 |
				DmaDescriptorSkip0 | DmaDescriptor8Words |
				DmaArbitPr;

	writel(ipq_dma_bus_mode, &dma_reg->busmode);
	writel(ipq_dma_op_mode, &dma_reg->opmode);
}

static void ipq_eth_flw_cntl_cfg(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	struct eth_mac_regs *mac_reg = (struct eth_mac_regs *)priv->mac_regs_p;
	struct eth_dma_regs *dma_reg = (struct eth_dma_regs *)priv->dma_regs_p;
	uint ipq_dma_flw_cntl;
	uint ipq_mac_flw_cntl;

	ipq_dma_flw_cntl = DmaRxFlowCtrlAct3K | DmaRxFlowCtrlDeact4K |
				DmaEnHwFlowCtrl;
	ipq_mac_flw_cntl = GmacRxFlowControl | GmacTxFlowControl | 0xFFFF0000;

	setbits_le32(&dma_reg->opmode, ipq_dma_flw_cntl);
	setbits_le32(&mac_reg->flowcontrol, ipq_mac_flw_cntl);
}

static int ipq_gmac_alloc_fifo(int ndesc, ipq_gmac_desc_t **fifo)
{
	int i;
    void *addr;

    addr = memalign((CONFIG_SYS_CACHELINE_SIZE),
            (ndesc * DESC_FLUSH_SIZE));
    for (i = 0; i < ndesc; i++) {
        fifo[i] = (ipq_gmac_desc_t *)((unsigned long)addr +
                (i * DESC_FLUSH_SIZE));
        if (fifo[i] == NULL) {
            printf("Can't allocate desc fifos\n");
            return -1;
        }
    }

	return 0;
}

static int ipq_gmac_rx_desc_setup(struct ipq_eth_dev  *priv)
{
	struct eth_dma_regs *dma_reg = (struct eth_dma_regs *)priv->dma_regs_p;
	ipq_gmac_desc_t *rxdesc;
	int i;

	for (i = 0; i < NO_OF_RX_DESC; i++) {
		rxdesc = priv->desc_rx[i];
		rxdesc->length |= ((ETH_MAX_FRAME_LEN << DescSize1Shift) &
					DescSize1Mask);
		rxdesc->buffer1 = virt_to_phys((uchar *)NetRxPackets[i]);
		rxdesc->data1 = (unsigned long)priv->desc_rx[(i + 1) %
				NO_OF_RX_DESC];

		rxdesc->extstatus = 0;
		rxdesc->reserved1 = 0;
		rxdesc->timestamplow = 0;
		rxdesc->timestamphigh = 0;
		rxdesc->status = DescOwnByDma;


		flush_dcache_range((unsigned long)rxdesc,
			(unsigned long)rxdesc + DESC_SIZE);

	}
	/* Assign Descriptor base address to dmadesclist addr reg */
	writel((uint)priv->desc_rx[0], &dma_reg->rxdesclistaddr);

	return 0;
}

static int ipq_gmac_tx_rx_desc_ring(struct ipq_eth_dev  *priv)
{
	int i;
	ipq_gmac_desc_t *desc;

	if (ipq_gmac_alloc_fifo(NO_OF_TX_DESC, priv->desc_tx))
		return -1;

	for (i = 0; i < NO_OF_TX_DESC; i++) {
		desc = priv->desc_tx[i];
		memset(desc, 0, DESC_SIZE);

		desc->status =
		(i == (NO_OF_TX_DESC - 1)) ? TxDescEndOfRing : 0;

		desc->status |= TxDescChain;

		desc->data1 = (unsigned long)priv->desc_tx[(i + 1) %
				NO_OF_TX_DESC ];

		flush_dcache_range((unsigned long)desc,
			(unsigned long)desc + DESC_SIZE);

	}

	if (ipq_gmac_alloc_fifo(NO_OF_RX_DESC, priv->desc_rx))
		return -1;

	for (i = 0; i < NO_OF_RX_DESC; i++) {
		desc = priv->desc_rx[i];
		memset(desc, 0, DESC_SIZE);
		desc->length =
		(i == (NO_OF_RX_DESC - 1)) ? RxDescEndOfRing : 0;
		desc->length |= RxDescChain;

		desc->data1 = (unsigned long)priv->desc_rx[(i + 1) %
				NO_OF_RX_DESC];

		flush_dcache_range((unsigned long)desc,
			(unsigned long)desc + DESC_SIZE);
	}

	priv->next_tx = 0;
	priv->next_rx = 0;

	return 0;
}

static inline void ipq_gmac_give_to_dma(ipq_gmac_desc_t *fr)
{
	fr->status |= DescOwnByDma;
}

static inline u32 ipq_gmac_owned_by_dma(ipq_gmac_desc_t *fr)
{
	return (fr->status & DescOwnByDma);
}

static inline u32 ipq_gmac_is_desc_empty(ipq_gmac_desc_t *fr)
{
	return ((fr->length & DescSize1Mask) == 0);
}

static int ipq_eth_init(struct eth_device *dev, bd_t *this)
{
	struct ipq_eth_dev *priv = dev->priv;
	struct eth_dma_regs *dma_reg = (struct eth_dma_regs *)priv->dma_regs_p;
	u32 data;
    uint clk_div_val;
    int phy_status;

	priv->next_rx = 0;
	priv->next_tx = 0;

#ifdef CONFIG_EIGER
	if (ipq_mac_reset(dev)) {
		ipq_info("%s: N/A\n", dev->name);
		return 0;
	}
#else
	ipq_mac_reset(dev);
#endif

	if ((priv->mac_unit == GMAC_UNIT2) || (priv->mac_unit == GMAC_UNIT3))
		config_auto_neg(dev);

#if defined(CONFIG_APBOOT)
    phy_status = ipq_phy_link_status(dev);
    if (phy_status != 0) {/* link down */
        ipq_info("%s: link down\n", dev->name);
    } else {
#ifdef CONFIG_EIGER
		if (priv->mac_unit == GMAC_UNIT1) {
			char phy_info_str[128];
			switch (priv->speed) {
				case SPEED_1000M:
					sprintf(phy_info_str, "%s: 1000M %s %s\n", dev->name, 
							priv->mode ? "SFP" : "Copper", 
							priv->duplex ? "full-duplex" : "half-duplex");
					break;
				case SPEED_100M:
					sprintf(phy_info_str, "%s: 100M %s %s\n", dev->name, 
							priv->mode ? "SFP" : "Copper", 
							priv->duplex ? "full-duplex" : "half-duplex");
					break;
				case SPEED_10M:
					sprintf(phy_info_str, "%s: 10M %s %s\n", dev->name,
							priv->mode ? "SFP" : "Copper", 
							priv->duplex ? "full-duplex" : "half-duplex");
					break;
				default:
					sprintf(phy_info_str, "%s: unknown %s %s\n", dev->name,
							priv->mode ? "SFP" : "Copper", 
							priv->duplex ? "full-duplex" : "half-duplex");
					break;
			}
			ipq_info(phy_info_str);
		}
		else {
			ipq_info("%s: 1000M full-duplex\n", dev->name);
		}
#endif
        if ((priv->mac_unit == GMAC_UNIT2 || priv->mac_unit == GMAC_UNIT3)) {
            ipq_info("%s: link up", dev->name);
            get_phy_speed_duplexity(dev);

            switch (priv->speed) {
                case SPEED_1000M:
                    ipq_info(", speed 1 Gb/s, ");
                    priv->mac_ps = GMII_PORT_SELECT;
                    clk_div_val = (CLK_DIV_SGMII_1000M - 1);
                    break;
                case SPEED_100M:
                    ipq_info(", speed 100 Mb/s, ");
                    priv->mac_ps = MII_PORT_SELECT;
                    clk_div_val = (CLK_DIV_SGMII_100M - 1);
                    break;
                case SPEED_10M:
                    ipq_info(", speed 10 Mb/s, ");
                    priv->mac_ps = MII_PORT_SELECT;
                    clk_div_val = (CLK_DIV_SGMII_10M - 1);
                    break;
                default:
                    ipq_info(", speed unknown\n");
                    return -1;
            }
            ipq_info("%s duplex\n", priv->duplex ? "full" : "half");
        } else {
            /* Force it to zero for GMAC 0 & 1 */
            clk_div_val = 0;
        }

        ipq_gmac_mii_clk_init(priv, clk_div_val, priv->gmac_cfg);
    }
#endif

	ipq_eth_wr_macaddr(dev);


	/* DMA, MAC configuration for Synopsys GMAC */
	ipq_eth_dma_cfg(dev);
	ipq_eth_mac_cfg(dev);
	ipq_eth_flw_cntl_cfg(dev);

	/* clear all pending interrupts if any */
	data = readl(&dma_reg->status);
	writel(data, &dma_reg->status);

	/* Setup Rx fifos and assign base address to */
	ipq_gmac_rx_desc_setup(priv);

	writel((uint)priv->desc_tx[0], &dma_reg->txdesclistaddr);
	setbits_le32(&dma_reg->opmode, (RXSTART));
	setbits_le32(&dma_reg->opmode, (TXSTART));

    return (phy_status == 0);
}

static int ipq_eth_send(struct eth_device *dev, volatile void *packet, int length)
{
	struct ipq_eth_dev *priv = dev->priv;
	struct eth_dma_regs *dma_p = (struct eth_dma_regs *)priv->dma_regs_p;
	ipq_gmac_desc_t *txdesc = priv->desc_tx[priv->next_tx];
	int i = 0;
    int ret = 0;


	invalidate_dcache_range((unsigned long)txdesc,
			(unsigned long)txdesc + DESC_FLUSH_SIZE);


	/* Check if the dma descriptor is still owned by DMA */
	if (ipq_gmac_owned_by_dma(txdesc)) {
		ipq_info("BUG: Tx descriptor is owned by DMA %p\n", txdesc);
		return NETDEV_TX_BUSY;
	}

	txdesc->length |= ((length <<DescSize1Shift) & DescSize1Mask);
	txdesc->status |= (DescTxFirst | DescTxLast | DescTxIntEnable);
	txdesc->buffer1 = virt_to_phys((void *)packet);
	ipq_gmac_give_to_dma(txdesc);

	flush_dcache_range((unsigned long)txdesc,
			(unsigned long)txdesc + DESC_SIZE);

	flush_dcache_range((unsigned long)(txdesc->buffer1),
		(unsigned long)(txdesc->buffer1) + PKTSIZE_ALIGN);

	/* Start the transmission */
	writel(POLL_DATA, &dma_p->txpolldemand);

	for (i = 0; i < MAX_WAIT; i++) {

		udelay(10);


		invalidate_dcache_range((unsigned long)txdesc,
		(unsigned long)txdesc + DESC_FLUSH_SIZE);

		if (!ipq_gmac_owned_by_dma(txdesc))
			break;
	}

	if (i == MAX_WAIT) {
		ret = 1;
	}

	/* reset the descriptors */
	txdesc->status = (priv->next_tx == (NO_OF_TX_DESC - 1)) ?
	TxDescEndOfRing : 0;
	txdesc->status |= TxDescChain;
	txdesc->length = 0;
	txdesc->buffer1 = 0;

	priv->next_tx = (priv->next_tx + 1) % NO_OF_TX_DESC;

	txdesc->data1 = (unsigned long)priv->desc_tx[priv->next_tx];


	flush_dcache_range((unsigned long)txdesc,
		(unsigned long)txdesc + DESC_SIZE);

	return ret;
}

static int ipq_eth_recv(struct eth_device *dev)
{
	struct ipq_eth_dev *priv = dev->priv;
	struct eth_dma_regs *dma_p = (struct eth_dma_regs *)priv->dma_regs_p;
	int length = 0;
	ipq_gmac_desc_t *rxdesc = priv->desc_rx[priv->next_rx];
	uint status;

	invalidate_dcache_range((unsigned long)(priv->desc_rx[0]),
			(unsigned long)(priv->desc_rx[NO_OF_RX_DESC - 1]) +
			DESC_FLUSH_SIZE);

	for (rxdesc = priv->desc_rx[priv->next_rx];
		!ipq_gmac_owned_by_dma(rxdesc);
		rxdesc = priv->desc_rx[priv->next_rx]) {

		status = rxdesc->status;
		length = ((status & DescFrameLengthMask) >>
				DescFrameLengthShift);


		invalidate_dcache_range(
			(unsigned long)(NetRxPackets[priv->next_rx]),
			(unsigned long)(NetRxPackets[priv->next_rx]) +
			PKTSIZE_ALIGN);

		NetReceive(NetRxPackets[priv->next_rx], length - 4);


		rxdesc->length = ((ETH_MAX_FRAME_LEN << DescSize1Shift) &
				   DescSize1Mask);

		rxdesc->length |= (priv->next_rx == (NO_OF_RX_DESC - 1)) ?
					RxDescEndOfRing : 0;
		rxdesc->length |= RxDescChain;

		rxdesc->buffer1 = virt_to_phys((uchar *)NetRxPackets[priv->next_rx]);

		priv->next_rx = (priv->next_rx + 1) % NO_OF_RX_DESC;
		rxdesc->data1 = (unsigned long)priv->desc_rx[priv->next_rx];

		rxdesc->extstatus = 0;
		rxdesc->reserved1 = 0;
		rxdesc->timestamplow = 0;
		rxdesc->timestamphigh = 0;
		rxdesc->status = DescOwnByDma;


		flush_dcache_range((unsigned long)rxdesc,
			(unsigned long)rxdesc + DESC_SIZE);


		writel(POLL_DATA, &dma_p->rxpolldemand);
	}

	return length;
}

static void ipq_eth_halt(struct eth_device *dev)
{
	if (dev->state != ETH_STATE_ACTIVE)
		return;
	/* reset the mac */
	ipq_mac_reset(dev);
}

static void
gmac_sgmii_clk_init(uint mac_unit, uint clk_div, ipq_gmac_board_cfg_t *gmac_cfg)
{
	uint gmac_ctl_val;
	uint nss_eth_clk_gate_val;

	gmac_ctl_val = (NSS_ETH_GMAC_PHY_INTF_SEL |
			NSS_ETH_GMAC_PHY_IFG_LIMIT |
			NSS_ETH_GMAC_PHY_IFG);


	nss_eth_clk_gate_val = (GMACn_GMII_RX_CLK(mac_unit) |
				GMACn_GMII_TX_CLK(mac_unit) |
				GMACn_PTP_CLK(mac_unit));

	writel(gmac_ctl_val, (NSS_REG_BASE + NSS_GMACn_CTL(mac_unit)));

	switch (mac_unit) {
	case GMAC_UNIT1:
		setbits_le32((QSGMII_REG_BASE + PCS_ALL_CH_CTL),
				PCS_CHn_FORCE_SPEED(mac_unit));
		clrbits_le32((QSGMII_REG_BASE + PCS_ALL_CH_CTL),
				PCS_CHn_SPEED_MASK(mac_unit));
		setbits_le32((QSGMII_REG_BASE + PCS_ALL_CH_CTL),
				PCS_CHn_SPEED(mac_unit,
					PCS_CH_SPEED_1000));

		setbits_le32((NSS_REG_BASE + NSS_ETH_CLK_GATE_CTL),
			nss_eth_clk_gate_val);
		break;
	case GMAC_UNIT2:
	case GMAC_UNIT3:
		if (gmac_cfg->mac_conn_to_phy) {

			setbits_le32((QSGMII_REG_BASE + PCS_ALL_CH_CTL),
				(PCS_CHn_SPEED_FORCE_OUTSIDE(mac_unit) |
				PCS_DEBUG_SELECT));

			setbits_le32((NSS_REG_BASE + NSS_ETH_CLK_SRC_CTL),
				(1 << mac_unit));

			if (clk_div == 0) {
				clrbits_le32((NSS_REG_BASE +
					NSS_ETH_CLK_DIV0),
					(NSS_ETH_CLK_DIV(
					NSS_ETH_CLK_DIV_MASK,
					mac_unit)));
			} else {
				clrsetbits_le32((NSS_REG_BASE +
					NSS_ETH_CLK_DIV0),
					(NSS_ETH_CLK_DIV(
					NSS_ETH_CLK_DIV_MASK,
					mac_unit)),
					(NSS_ETH_CLK_DIV(clk_div,
					mac_unit)));
			}
			setbits_le32((NSS_REG_BASE + NSS_ETH_CLK_GATE_CTL),
					nss_eth_clk_gate_val);
		} else {
			/* this part of code forces the speed of MAC 2 to
			 * 1000Mbps disabling the Autoneg in case
			 * of AP148/DB147 since it is connected to switch
			 */
			setbits_le32((QSGMII_REG_BASE + PCS_ALL_CH_CTL),
				PCS_CHn_FORCE_SPEED(mac_unit));

			clrbits_le32((QSGMII_REG_BASE + PCS_ALL_CH_CTL),
				PCS_CHn_SPEED_MASK(mac_unit));

			setbits_le32((QSGMII_REG_BASE + PCS_ALL_CH_CTL),
				PCS_CHn_SPEED(mac_unit,
					PCS_CH_SPEED_1000));

			setbits_le32((NSS_REG_BASE + NSS_ETH_CLK_GATE_CTL),
				nss_eth_clk_gate_val);
		}
		break;
	}
}

static void ipq_gmac_mii_clk_init(struct ipq_eth_dev *priv, uint clk_div,
				ipq_gmac_board_cfg_t *gmac_cfg)
{
	u32 nss_gmac_ctl_val;
	u32 nss_eth_clk_gate_ctl_val;
	int gmac_idx = priv->mac_unit;
	u32 interface = priv->interface;

	switch (interface) {
	case PHY_INTERFACE_MODE_RGMII:
		nss_gmac_ctl_val = (GMAC_PHY_RGMII | GMAC_IFG |
				GMAC_IFG_LIMIT(GMAC_IFG));
		nss_eth_clk_gate_ctl_val =
			(GMACn_RGMII_RX_CLK(gmac_idx) |
			 GMACn_RGMII_TX_CLK(gmac_idx) |
			 GMACn_PTP_CLK(gmac_idx));
		setbits_le32((NSS_REG_BASE + NSS_GMACn_CTL(gmac_idx)),
				nss_gmac_ctl_val);
		setbits_le32((NSS_REG_BASE + NSS_ETH_CLK_GATE_CTL),
				nss_eth_clk_gate_ctl_val);
		setbits_le32((NSS_REG_BASE + NSS_ETH_CLK_SRC_CTL),
				(0x1 << gmac_idx));
		writel((NSS_ETH_CLK_DIV(1, gmac_idx)),
				(NSS_REG_BASE + NSS_ETH_CLK_DIV0));
		break;
	case PHY_INTERFACE_MODE_SGMII:
		gmac_sgmii_clk_init(gmac_idx, clk_div, gmac_cfg);
		break;
	default :
		ipq_info(" default : no rgmii sgmii for gmac %d  \n", gmac_idx);
		return;
	}
}

int ipq_gmac_init(ipq_gmac_board_cfg_t *gmac_cfg)
{
#ifndef CONFIG_APBOOT
	static int s17_init_done = 0;
#endif
	struct eth_device *dev[IPQ_GMAC_NMACS];
	uint clk_div_val;
	uchar enet_addr[IPQ_GMAC_NMACS * 6];
	int i;
	int ret;

	memset(enet_addr, 0, sizeof(enet_addr));

	/* Getting the MAC address from ART partition */
	ret = get_eth_mac_address(enet_addr, IPQ_GMAC_NMACS);

	for (i = 0; gmac_cfg_is_valid(gmac_cfg); gmac_cfg++, i++) {

		dev[i] = malloc(sizeof(struct eth_device));
		if (dev[i] == NULL)
			goto failed;

		ipq_gmac_macs[i] = malloc(sizeof(struct ipq_eth_dev));
		if (ipq_gmac_macs[i] == NULL)
			goto failed;

		memset(dev[i], 0, sizeof(struct eth_device));
		memset(ipq_gmac_macs[i], 0, sizeof(struct ipq_eth_dev));

		dev[i]->iobase = gmac_cfg->base;
		dev[i]->init = ipq_eth_init;
		dev[i]->halt = ipq_eth_halt;
		dev[i]->recv = ipq_eth_recv;
		dev[i]->send = ipq_eth_send;
		dev[i]->write_hwaddr = ipq_eth_wr_macaddr;
		dev[i]->priv = (void *) ipq_gmac_macs[i];

		sprintf(dev[i]->name, "eth%d", i);

		/*
		 * Setting the Default MAC address if the MAC read from ART partition
		 * is invalid.
		 */
#ifdef CONFIG_APBOOT
		if ((ret < 0) ||
		    (!is_valid_ether_addr(&enet_addr[i * 6]))) {
			memcpy(&dev[i]->enetaddr[0], ipq_def_enetaddr, 6);
			dev[i]->enetaddr[5] = gmac_cfg->unit & 0xff;
		} else {
			memcpy(&dev[i]->enetaddr[0],
			       &enet_addr[i * 6],
			       6);
		}
#else
		if ((ret < 0) ||
		    (!is_valid_ether_addr(&enet_addr[gmac_cfg->unit * 6]))) {
			memcpy(&dev[i]->enetaddr[0], ipq_def_enetaddr, 6);
			dev[i]->enetaddr[5] = gmac_cfg->unit & 0xff;
		} else {
			memcpy(&dev[i]->enetaddr[0],
			       &enet_addr[gmac_cfg->unit * 6],
			       6);
		}

		ipq_info("MAC%x addr:%x:%x:%x:%x:%x:%x\n",
			gmac_cfg->unit, dev[i]->enetaddr[0],
			dev[i]->enetaddr[1],
			dev[i]->enetaddr[2],
			dev[i]->enetaddr[3],
			dev[i]->enetaddr[4],
			dev[i]->enetaddr[5]);
#endif
#ifdef CONFIG_EIGER
		if (i)
			ipq_info("       GMAC%x, ", gmac_cfg->unit);
		else
			ipq_info("GMAC%x, ", gmac_cfg->unit);
#endif
		ipq_gmac_macs[i]->dev = dev[i];
		ipq_gmac_macs[i]->mac_unit = gmac_cfg->unit;
		ipq_gmac_macs[i]->mac_regs_p =
			(struct eth_mac_regs *)(gmac_cfg->base);
		ipq_gmac_macs[i]->dma_regs_p =
			(struct eth_dma_regs *)(gmac_cfg->base + DW_DMA_BASE_OFFSET);
		ipq_gmac_macs[i]->interface = gmac_cfg->phy;
		ipq_gmac_macs[i]->phy_address = gmac_cfg->phy_addr.addr;
		ipq_gmac_macs[i]->no_of_phys = gmac_cfg->phy_addr.count;
		ipq_gmac_macs[i]->gmac_cfg = gmac_cfg;

#ifdef CONFIG_EIGER
		if (gmac_cfg->unit == GMAC_UNIT1) {
			miiphy_register(dev[i]->name, ipq_mdio_read_wrap, ipq_mdio_write_wrap);
			m88e1512_reg_init(ipq_gmac_macs[i]->phy_address[0], eiger_sfp_sgmii, 0);
		}
#endif
#ifdef CONFIG_OCTOMORE
		m88e1514_reg_init(ipq_gmac_macs[i]->phy_address[0]);
#endif
#if  defined(CONFIG_GLENFARCLAS) || defined(CONFIG_SIERRA) || defined(CONFIG_TOBA)		
		ar8033_reg_init(ipq_gmac_macs[i]->phy_address[0]);
#endif
#if defined(CONFIG_OCTOMORE) || defined(CONFIG_GLENFARCLAS) || defined(CONFIG_SIERRA) || defined(CONFIG_TOBA)
		miiphy_register(dev[i]->name, ipq_mdio_read_wrap, ipq_mdio_write_wrap);
#endif
		/* tx/rx Descriptor initialization */
		if (ipq_gmac_tx_rx_desc_ring(dev[i]->priv) == -1)
			goto failed;

		if ((gmac_cfg->unit == GMAC_UNIT2 ||
		    gmac_cfg->unit == GMAC_UNIT3) &&
		    (gmac_cfg->mac_conn_to_phy)) {

#ifdef CONFIG_APBOOT
			get_phy_speed_duplexity(dev[i]);

			switch (ipq_gmac_macs[i]->speed) {
			case SPEED_1000M:
				ipq_gmac_macs[i]->mac_ps = GMII_PORT_SELECT;
				clk_div_val = (CLK_DIV_SGMII_1000M - 1);
				break;
			case SPEED_100M:
				ipq_gmac_macs[i]->mac_ps = MII_PORT_SELECT;
				clk_div_val = (CLK_DIV_SGMII_100M - 1);
				break;
			case SPEED_10M:
				ipq_gmac_macs[i]->mac_ps = MII_PORT_SELECT;
				clk_div_val = (CLK_DIV_SGMII_10M - 1);
				break;
			default:
				ipq_info("Port speed unknown\n");
				goto failed;
			}
#else
			ipq_gmac_macs[i]->mac_ps = MII_PORT_SELECT;
			clk_div_val = (CLK_DIV_SGMII_10M - 1);
#endif
		} else {
			/* Force it to zero for GMAC 0 & 1 */
			clk_div_val = 0;
		}

		ipq_gmac_mii_clk_init(ipq_gmac_macs[i], clk_div_val, gmac_cfg);

		eth_register(dev[i]);
#ifndef CONFIG_APBOOT
		if (!s17_init_done) {
			ipq_switch_init(gmac_cfg);
			s17_init_done = 1;
			ipq_info("S17 inits done\n");
		}
#endif
	}

	return 0;

failed:
	for (i = 0; i < IPQ_GMAC_NMACS; i++) {
		if (dev[i]) {
			eth_unregister(dev[i]);
			free(dev[i]);
		}
		if (ipq_gmac_macs[i])
			free(ipq_gmac_macs[i]);
	}

	return -ENOMEM;
}



static void ipq_gmac_core_reset(ipq_gmac_board_cfg_t *gmac_cfg)
{
	for (; gmac_cfg_is_valid(gmac_cfg); gmac_cfg++) {
		writel(0, GMAC_CORE_RESET(gmac_cfg->unit));
		if (gmac_cfg->is_macsec)
			writel(0, GMACSEC_CORE_RESET(gmac_cfg->unit));
	}

	writel(0, (void *)GMAC_AHB_RESET);
	writel(0, (MSM_TCSR_BASE + TCSR_PXO_SEL));
}

uint ipq_mdio_read(uint phy_addr, uint reg_offset, ushort *data)
{
	uint reg_base = NSS_GMAC0_BASE;
	uint timeout = MII_MDIO_TIMEOUT;
	uint miiaddr;
	uint start;
	uint ret_val;

	miiaddr = (((phy_addr << MIIADDRSHIFT) & MII_ADDRMSK) |
	((reg_offset << MIIREGSHIFT) & MII_REGMSK));

	miiaddr |= (MII_BUSY | MII_CLKRANGE_250_300M);
	writel(miiaddr, (reg_base + MII_ADDR_REG_ADDR));
	udelay(10);

	start = get_timer(0);
	while (get_timer(start) < timeout) {
		if (!(readl(reg_base + MII_ADDR_REG_ADDR) & MII_BUSY)) {
			ret_val = readl(reg_base + MII_DATA_REG_ADDR);
			if (data != NULL)
				*data = ret_val;
			return ret_val;
		}
		udelay(1000);
	}
	return -1;
}

uint ipq_mdio_write(uint phy_addr, uint reg_offset, ushort data)
{
	const uint reg_base = NSS_GMAC0_BASE;
	const uint timeout = MII_MDIO_TIMEOUT;
	uint miiaddr;
	uint start;

	writel(data, (reg_base + MII_DATA_REG_ADDR));

	miiaddr = (((phy_addr << MIIADDRSHIFT) & MII_ADDRMSK) |
			((reg_offset << MIIREGSHIFT) & MII_REGMSK) |
			(MII_WRITE));

	miiaddr |= (MII_BUSY | MII_CLKRANGE_250_300M);
	writel(miiaddr, (reg_base + MII_ADDR_REG_ADDR));
	udelay(10);

	start = get_timer(0);
	while (get_timer(start) < timeout) {
		if (!(readl(reg_base + MII_ADDR_REG_ADDR) & MII_BUSY)) {
			return 0;
		}
		udelay(1000);
	}
	return -1;
}

int ipq_mdio_read_wrap(char *devname, unsigned char addr, unsigned char reg, unsigned short *value)
{
    int ret;
    ret = ipq_mdio_read(addr, reg, value);
    if (ret != -1) {
        return 0;
    }
    return ret;
}

int ipq_mdio_write_wrap(char *devname, unsigned char addr, unsigned char reg, unsigned short value)
{
    return ipq_mdio_write(addr, reg, value);
}

void ipq_gmac_common_init(ipq_gmac_board_cfg_t *gmac_cfg)
{
	uint pcs_qsgmii_ctl_val;
	uint pcs_mode_ctl_val;

	pcs_mode_ctl_val = (PCS_CHn_ANEG_EN(GMAC_UNIT1) |
				PCS_CHn_ANEG_EN(GMAC_UNIT2) |
				PCS_CHn_ANEG_EN(GMAC_UNIT3) |
				PCS_CHn_ANEG_EN(GMAC_UNIT0) |
				PCS_SGMII_MAC);

	pcs_qsgmii_ctl_val = (PCS_QSGMII_ATHR_CSCO_AUTONEG |
				PCS_QSGMII_SW_VER_1_7 |
				PCS_QSGMII_SHORT_THRESH |
				PCS_QSGMII_SHORT_LATENCY |
				PCS_QSGMII_DEPTH_THRESH(1) |
				PCS_CHn_SERDES_SN_DETECT(0) |
				PCS_CHn_SERDES_SN_DETECT(1) |
				PCS_CHn_SERDES_SN_DETECT(2) |
				PCS_CHn_SERDES_SN_DETECT(3) |
				PCS_CHn_SERDES_SN_DETECT_2(0) |
				PCS_CHn_SERDES_SN_DETECT_2(1) |
				PCS_CHn_SERDES_SN_DETECT_2(2) |
				PCS_CHn_SERDES_SN_DETECT_2(3));

	writel(MACSEC_BYPASS_EXT_EN,(NSS_REG_BASE + NSS_MACSEC_CTL));
	writel(0, (QSGMII_REG_BASE + QSGMII_PHY_MODE_CTL));
	writel(0, (QSGMII_REG_BASE + PCS_QSGMII_SGMII_MODE));

	setbits_le32((QSGMII_REG_BASE + NSS_PCS_MODE_CTL), pcs_mode_ctl_val);
	writel(pcs_qsgmii_ctl_val, (QSGMII_REG_BASE + PCS_QSGMII_CTL));

	/*
	 * MDIO lines for all the MACs are connected through MAC0.
	 * Regardless of MAC 0 being used or not, it has to be pulled
	 * out of reset. Else, MDIO writes to configure other MACs
	 * will fail.
	 */
	writel(0, GMAC_CORE_RESET(0));

	/*
	 * Pull out of reset the MACs that are applicable to the
	 * current board.
	 */
	ipq_gmac_core_reset(gmac_cfg);
}
