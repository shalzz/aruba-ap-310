/*
 * drivers/net/phy/at803x.c
 *
 * Driver for Atheros 803x PHY
 *
 * Author: Matus Ujhelyi <ujhelyi.m@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/phy.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#define AT803X_INTR_ENABLE			0x12
#define AT803X_INTR_STATUS			0x13
#define AT803X_INER				0x0012
#define AT803X_INER_INIT			0xec00
#define AT803X_INSR				0x0013
#define AT803X_DEBUG_ADDR			0x1D
#define AT803X_DEBUG_DATA			0x1E
#define AT803X_DEBUG_SYSTEM_MODE_CTRL		0x05
#define AT803X_DEBUG_RGMII_TX_CLK_DLY		BIT(8)

#ifdef CONFIG_ARUBA_AP
#define AT803X_CHIP_CFG	0x1F
#endif
#define ATH8030_PHY_ID 0x004dd076
#define ATH8031_PHY_ID 0x004dd074
#define ATH8035_PHY_ID 0x004dd072

MODULE_DESCRIPTION("Atheros 803x PHY driver");
MODULE_AUTHOR("Matus Ujhelyi");
MODULE_LICENSE("GPL");

static int at803x_config_init(struct phy_device *phydev)
{
	int ret;
#ifdef CONFIG_ARUBA_AP
	int val;
#endif
	
	ret = genphy_config_init(phydev);
	if (ret < 0)
		return ret;

	if (phydev->interface == PHY_INTERFACE_MODE_RGMII_TXID) {
		ret = phy_write(phydev, AT803X_DEBUG_ADDR,
				AT803X_DEBUG_SYSTEM_MODE_CTRL);
		if (ret)
			return ret;
		ret = phy_write(phydev, AT803X_DEBUG_DATA,
				AT803X_DEBUG_RGMII_TX_CLK_DLY);
		if (ret)
			return ret;
	}
#ifdef CONFIG_ARUBA_AP
	val = phy_read(phydev,AT803X_CHIP_CFG);
	if( (val & 0xF) == 2 || (val & 0xF) == 3)
	{
		/*init sierra fiber port features*/ 
		phydev->supported   = (SUPPORTED_1000baseT_Full | SUPPORTED_1000baseT_Half |
						SUPPORTED_FIBRE |SUPPORTED_Autoneg );
		phydev->advertising = (ADVERTISED_1000baseT_Full | ADVERTISED_1000baseT_Half |
				  ADVERTISED_FIBRE | ADVERTISED_Autoneg );
	}
	/*QA need ethernet phy disable flow control,refer bug 168680*/
	val = phy_read(phydev,MII_ADVERTISE);
	if (val & (ADVERTISE_PAUSE_CAP | ADVERTISE_PAUSE_ASYM))
	{
		val &= (~(ADVERTISE_PAUSE_CAP | ADVERTISE_PAUSE_ASYM));
		ret = phy_write(phydev, MII_ADVERTISE,
				val);
		if (ret)
			return ret;
	}
#endif
	return 0;
}
#ifdef CONFIG_ARUBA_AP
static int at803x_config_aneg(struct phy_device *phydev)
{
	int val,result,ctl;

	val = phy_read(phydev,AT803X_CHIP_CFG);
	/*config Sierra fiber port as 1000M  autoneg*/
	if( (val & 0xF) == 2 || (val & 0xF) == 3)
	{
		result = phy_write(phydev,MII_ADVERTISE,0x60);
		

		if (result < 0) /* error */
			return result;

		
		/* Advertisement hasn't changed, but maybe aneg was never on to
		 * begin with?  Or maybe phy was isolated? */
		ctl = phy_read(phydev, MII_BMCR);

		if (ctl < 0)
			return ctl;

		if (!(ctl & BMCR_ANENABLE) || (ctl & BMCR_ISOLATE))
			result = 1; /* do restart aneg */
	

		/* Only restart aneg if we are advertising something different
		 * than we were before.	 */
		if (result > 0)
			result = genphy_restart_aneg(phydev);

		return result;

	}
	else
		return genphy_config_aneg(phydev);

	
}

static int at803x_read_status(struct phy_device *phydev)
{
	int adv;
	int err;
	int lpa,val;
	int lpagb = 0;
	

	/* Update the link, but return if there
	 * was an error */
	err = genphy_update_link(phydev);
	if (err)
		return err;


	if (AUTONEG_ENABLE == phydev->autoneg) {
		val = phy_read(phydev,AT803X_CHIP_CFG);
		/*get sierra fiber port link speed and duplex*/
		if ((val & 0xF) == 2 || (val & 0xF) == 3)
		{
			lpa = phy_read(phydev, MII_LPA);
			if (lpa < 0)
				return lpa;

			adv = phy_read(phydev, MII_ADVERTISE);

			if (adv < 0)
				return adv;

			lpa &= adv;
			if (lpa & 0x20)
			{
				phydev->speed = SPEED_1000;
				phydev->duplex = DUPLEX_FULL;
			}
			else if (lpa & 0x40)
			{
				phydev->speed = SPEED_1000;
				phydev->duplex = DUPLEX_HALF;
			}
			else
			{
				phydev->speed = SPEED_UNKNOWN;
				phydev->duplex = DUPLEX_UNKNOWN;
			}
		}
		else
		{
			if (phydev->supported & (SUPPORTED_1000baseT_Half
						| SUPPORTED_1000baseT_Full)) {
				lpagb = phy_read(phydev, MII_STAT1000);

				if (lpagb < 0)
					return lpagb;

				adv = phy_read(phydev, MII_CTRL1000);

				if (adv < 0)
					return adv;

				lpagb &= adv << 2;
			}

			lpa = phy_read(phydev, MII_LPA);

			if (lpa < 0)
				return lpa;

			adv = phy_read(phydev, MII_ADVERTISE);

			if (adv < 0)
				return adv;

			lpa &= adv;

			phydev->speed = SPEED_10;
			phydev->duplex = DUPLEX_HALF;
			phydev->pause = phydev->asym_pause = 0;

			if (lpagb & (LPA_1000FULL | LPA_1000HALF)) {
				phydev->speed = SPEED_1000;

				if (lpagb & LPA_1000FULL)
					phydev->duplex = DUPLEX_FULL;
			} else if (lpa & (LPA_100FULL | LPA_100HALF)) {
				phydev->speed = SPEED_100;

				if (lpa & LPA_100FULL)
					phydev->duplex = DUPLEX_FULL;
			} else
			{
				if (lpa & LPA_10FULL)
					phydev->duplex = DUPLEX_FULL;
			}

			if (phydev->duplex == DUPLEX_FULL){
				phydev->pause = lpa & LPA_PAUSE_CAP ? 1 : 0;
				phydev->asym_pause = lpa & LPA_PAUSE_ASYM ? 1 : 0;
			}
		}
	} else {
		int bmcr = phy_read(phydev, MII_BMCR);
		if (bmcr < 0)
			return bmcr;

		if (bmcr & BMCR_FULLDPLX)
			phydev->duplex = DUPLEX_FULL;
		else
			phydev->duplex = DUPLEX_HALF;

		if (bmcr & BMCR_SPEED1000)
			phydev->speed = SPEED_1000;
		else if (bmcr & BMCR_SPEED100)
			phydev->speed = SPEED_100;
		else
			phydev->speed = SPEED_10;

		phydev->pause = phydev->asym_pause = 0;
	}

	return 0;
}
#endif
static int at803x_ack_interrupt(struct phy_device *phydev)
{
	int err;

	err = phy_read(phydev, AT803X_INSR);

	return (err < 0) ? err : 0;
}

static int at803x_config_intr(struct phy_device *phydev)
{
	int err;
	int value;

	value = phy_read(phydev, AT803X_INER);

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED)
		err = phy_write(phydev, AT803X_INER,
				value | AT803X_INER_INIT);
	else
		err = phy_write(phydev, AT803X_INER, 0);

	return err;
}

static struct phy_driver at803x_driver[] = {
{
	/* ATHEROS 8035 */
	.phy_id			= ATH8035_PHY_ID,
	.name			= "Atheros 8035 ethernet",
	.phy_id_mask		= 0xffffffef,
	.config_init		= at803x_config_init,
	.features		= PHY_GBIT_FEATURES,
	.flags			= PHY_HAS_INTERRUPT,
	.config_aneg		= genphy_config_aneg,
	.read_status		= genphy_read_status,
	.ack_interrupt		= at803x_ack_interrupt,
	.config_intr		= at803x_config_intr,
	.driver			= {
		.owner = THIS_MODULE,
	},
}, {
	/* ATHEROS 8030 */
	.phy_id			= ATH8030_PHY_ID,
	.name			= "Atheros 8030 ethernet",
	.phy_id_mask		= 0xffffffef,
	.config_init		= at803x_config_init,
	.features		= PHY_GBIT_FEATURES,
	.flags			= PHY_HAS_INTERRUPT,
	.config_aneg		= genphy_config_aneg,
	.read_status		= genphy_read_status,
	.ack_interrupt		= at803x_ack_interrupt,
	.config_intr		= at803x_config_intr,
	.driver			= {
		.owner = THIS_MODULE,
	},
}, {
#ifdef CONFIG_ARUBA_AP
	/* ATHEROS 8031 */
	.phy_id			= ATH8031_PHY_ID,
	.name			= "Atheros 8031 ethernet",
	.phy_id_mask		= 0xffffffef,
	.config_init		= at803x_config_init,
	.features		= PHY_GBIT_FEATURES,
	.flags			= PHY_HAS_INTERRUPT,
	.config_aneg		= at803x_config_aneg,
	.read_status		= at803x_read_status,
	.ack_interrupt		= &at803x_ack_interrupt,
	.config_intr		= &at803x_config_intr,
	.driver			= {
		.owner = THIS_MODULE,
#else
	.phy_id			= ATH8031_PHY_ID,
	.name			= "Atheros 8031 ethernet",
	.phy_id_mask		= 0xffffffef,
	.config_init		= at803x_config_init,
	.features		= PHY_GBIT_FEATURES,
	.flags			= PHY_HAS_INTERRUPT,
	.config_aneg		= genphy_config_aneg,
	.read_status		= genphy_read_status,
	.ack_interrupt		= &at803x_ack_interrupt,
	.config_intr		= &at803x_config_intr,
	.driver			= {
		.owner = THIS_MODULE,
#endif
	},
} };

static int __init at803x_init(void)
{
	int ret;
	int i;

	for (i = 0; i < ARRAY_SIZE(at803x_driver); i++) {
		ret = phy_driver_register(&at803x_driver[i]);

		if (ret) {
			while (i-- > 0)
				phy_driver_unregister(&at803x_driver[i]);
			return ret;
		}
	}

	return 0;
}

static void __exit at803x_exit(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(at803x_driver); i++)
		phy_driver_unregister(&at803x_driver[i]);
}

module_init(at803x_init);
module_exit(at803x_exit);

static struct mdio_device_id __maybe_unused atheros_tbl[] = {
	{ ATH8030_PHY_ID, 0xffffffef },
	{ ATH8031_PHY_ID, 0xffffffef },
	{ ATH8035_PHY_ID, 0xffffffef },
	{ }
};

MODULE_DEVICE_TABLE(mdio, atheros_tbl);
