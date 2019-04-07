#ifndef _AG7100_PHY_H
#define _AG7100_PHY_H

#ifdef CFG_ATHRS26_PHY
#ifndef AR9100
#include "../board/ar7100/ap94/athrs26_phy.h"

#define ag7100_phy_setup(unit)          athrs26_phy_setup (unit)
#define ag7100_phy_is_up(unit)          athrs26_phy_is_up (unit)
#define ag7100_phy_speed(unit)          athrs26_phy_speed (unit)
#define ag7100_phy_is_fdx(unit)         athrs26_phy_is_fdx (unit)

#else
#define ag7100_phy_setup(unit) do { \
if(!unit) \
        athrs26_phy_setup(unit); \
} while (0);

#define ag7100_phy_link(unit,link,fdx,speed) do { \
if(!unit) \
        link=miiphy_link("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_duplex(unit,duplex) do { \
if(!unit) \
        duplex = miiphy_duplex("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_speed(unit,speed) do { \
if(!unit) \
        speed = miiphy_speed("eth0", CFG_PHY_ADDR); \
} while (0);

#endif // Hydra or Howl
#else
typedef enum {
    AG7100_PHY_SPEED_10T,
    AG7100_PHY_SPEED_100TX,
    AG7100_PHY_SPEED_1000T,
}ag7100_phy_speed_t;
#define ag7100_phy_link(unit,link,fdx,speed) do { \
if(!unit) \
        link=miiphy_link("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_duplex(unit,duplex) do { \
if(!unit) \
        duplex = miiphy_duplex("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_speed(unit,speed) do { \
if(!unit) \
        speed = miiphy_speed("eth0", CFG_PHY_ADDR); \
} while (0);

#endif

#ifdef CFG_ATHRS16_PHY

#include "../board/ar7100/common/athrs16_phy.h"

#define ag7100_phy_setup(unit)          athrs16_phy_setup (unit)
#define ag7100_phy_is_up(unit)          athrs16_phy_is_up (unit)
#define ag7100_phy_speed(unit)          athrs16_phy_speed (unit)
#define ag7100_phy_is_fdx(unit)         athrs16_phy_is_fdx (unit)
/*
#define ag7100_phy_ioctl(unit, args)    athr_ioctl(unit,args)
#define ag7100_phy_is_lan_pkt           athr_is_lan_pkt
#define ag7100_phy_set_pkt_port         athr_set_pkt_port
#define ag7100_phy_tag_len              ATHR_VLAN_TAG_SIZE
#define ag7100_phy_get_counters         athrs16_get_counters
*/

static inline unsigned int
ag7100_get_link_status(int unit, unsigned *link, unsigned *fdx, ag7100_phy_speed_t *speed)
{
  *link=ag7100_phy_is_up(unit);
  *fdx=ag7100_phy_is_fdx(unit);
  *speed=ag7100_phy_speed(unit);
  return 0;
}

static inline int
ag7100_print_link_status(int unit)
{
  return -1;
}

#endif /* CFG_ATHRS16_PHY */

#ifdef CFG_VSC8201_PHY

#define ag7100_phy_setup(unit) do { \
if(!unit) \
        vsc_phy_setup(unit); \
} while (0);

#define ag7100_phy_link(unit,link,fdx,speed) do { \
if(!unit) \
        link=miiphy_link("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_duplex(unit,duplex) do { \
if(!unit) \
        duplex = miiphy_duplex("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_speed(unit,speed) do { \
if(!unit) \
        speed = miiphy_speed("eth0", CFG_PHY_ADDR); \
} while (0);

#endif

#ifdef CFG_VSC8601_PHY
#include "../board/ar7100/common/vsc8601_phy.h"

#define ag7100_phy_setup(unit) do { \
if(!unit) \
        vsc8601_phy_setup(unit); \
} while (0);

#define ag7100_phy_link(unit,link,fdx,speed) do { \
if(!unit) \
        link=miiphy_link("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_duplex(unit,duplex) do { \
if(!unit) \
        duplex = miiphy_duplex("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_speed(unit,speed) do { \
if(!unit) \
        speed = miiphy_speed("eth0", CFG_PHY_ADDR); \
} while (0);

#endif

#ifdef CFG_AR8021_PHY
#include "../board/ar7100/common/ar8021_phy.h"

#define ag7100_phy_setup(unit) do { \
if(!unit) \
        ar8021_phy_setup(unit); \
} while (0);

#define ag7100_phy_link(unit,link,fdx,speed) do { \
if(!unit) \
        link=miiphy_link("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_duplex(unit,duplex) do { \
if(!unit) \
        duplex = miiphy_duplex("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_speed(unit,speed) do { \
if(!unit) \
        speed = miiphy_speed("eth0", CFG_PHY_ADDR); \
} while (0);

#endif

#ifdef CFG_VITESSE_8601_7395_PHY

#define ag7100_phy_setup(unit) do { \
if(unit) \
	vsc73xx_setup(unit); \
else \
	vsc8601_phy_setup(unit); \
} while (0);

#define ag7100_phy_link(unit,link,fdx,speed) do { \
if(unit) \
	vsc73xx_get_link_status(unit, &link, &fdx, &speed,0); \
else \
        link=miiphy_link("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_duplex(unit,duplex) do { \
if(unit) \
	vsc73xx_get_link_status(unit, 0, &duplex, 0,0); \
else \
	duplex = miiphy_duplex("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_speed(unit,speed) do { \
if(unit) \
	vsc73xx_get_link_status(unit, 0, 0, &speed,0); \
else \
	speed = miiphy_speed("eth0", CFG_PHY_ADDR); \
} while (0);

#endif


#ifdef CFG_IP175B_PHY

#define ag7100_phy_setup(unit) do { \
if(!unit) \
        ip_phySetup(unit); \
} while (0);

#define ag7100_phy_link(unit,link,fdx,speed) do { \
if(!unit) \
        link=ip_phyIsUp(unit); \
} while (0);

#define ag7100_phy_duplex(unit,duplex) do { \
if(!unit) \
        duplex = ip_phyIsFullDuplex(unit); \
} while (0);

#define ag7100_phy_speed(unit,speed) do { \
if(!unit) \
        speed = ip_phySpeed(unit); \
} while (0);

#endif

#ifdef CONFIG_ADMTEK_PHY

#define ag7100_phy_setup(unit) do { \
if(!unit) \
        miiphy_reset("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_link(unit,link) do { \
if(!unit) \
        link=miiphy_link("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_duplex(unit,duplex) do { \
if(!unit) \
        duplex = miiphy_duplex("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_speed(unit,speed) do { \
if(!unit) \
        speed = miiphy_speed("eth0", CFG_PHY_ADDR); \
} while (0);
#endif

#ifdef CFG_MV88E1XXX_PHY
#include "../board/ar7100/common/marvell_phy.h"

#define ag7100_phy_setup(unit)          mv_phySetup(unit)
#define ag7100_phy_is_up(unit)          mv_phyIsUp(unit)

#undef ag7100_phy_speed
#define ag7100_phy_speed(unit)          mv_phySpeed(unit)

#define ag7100_phy_is_fdx(unit)         mv_phyIsFullDuplex(unit)
#define ag7100_phy_on(unit)             mv_phy_on(unit)
#define ag7100_phy_off(unit)            mv_phy_off(unit)
#define ag7100_phy_get(unit,num)        mv_phyGet(unit,num)
#define ag7100_phy_set(unit,num,val)    mv_phySet(unit,num,val)
#define ag7100_phy_int_init(unit)       mv_phyIntInit(unit)

typedef enum {
	INT_TYPE_EDGE,
	INT_TYPE_LEVEL,
}ar7100_gpio_int_type_t;

typedef enum {
	INT_POL_ACTIVE_LOW,
	INT_POL_ACTIVE_HIGH,
}ar7100_gpio_int_pol_t;

static inline int ag7100_print_link_status(int unit)
{
        return -1;
}

#endif /* CONFIG_MV88E1XXX_PHY */

#if defined(CONFIG_TALISKER) && !defined(CFG_VSC8601_PHY) && !defined(CFG_AR8021_PHY)

#define ag7100_phy_setup(unit) do { \
if(!unit) \
        miiphy_reset("eth0", CFG_PHY_ADDR); \
} while (0);

#if 0
#define ag7100_phy_link(unit,link) do { \
if(!unit) \
        link=miiphy_link("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_duplex(unit,duplex) do { \
if(!unit) \
        duplex = miiphy_duplex("eth0", CFG_PHY_ADDR); \
} while (0);

#define ag7100_phy_speed(unit,speed) do { \
if(!unit) \
        speed = miiphy_speed("eth0", CFG_PHY_ADDR); \
} while (0);
#endif	// 0
#endif	// TALISKER

#include <miiphy.h>

static inline unsigned int 
ag7100_get_link_status(int unit, unsigned *link, unsigned *fdx, ag7100_phy_speed_t *speed)
{
  *link = miiphy_link("eth0", CFG_PHY_ADDR); \
  *fdx = miiphy_duplex("eth0", CFG_PHY_ADDR); \
  *speed = miiphy_speed("eth0", CFG_PHY_ADDR); \
  return 0;
}

#endif /*_AG7100_PHY_H*/
