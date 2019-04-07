#ifndef _AG7240_PHY_H
#define _AG7240_PHY_H

#ifdef CFG_AR8021_PHY
#include "../board/ar7240/common/ar8021_phy.h"

#define ag7240_phy_setup(unit) do { \
if(!unit) \
        ar8021_phy_setup(unit); \
} while (0);

static inline void ag7240_phy_link(int unit, unsigned *link)
{
    if (!unit)
        *link=miiphy_link("eth0", CFG_PHY_ADDR);
}

static inline void ag7240_phy_duplex(int unit, unsigned *duplex)
{
    if (!unit)
        *duplex = miiphy_duplex("eth0", CFG_PHY_ADDR);
}

static inline void ag7240_phy_speed(int unit, unsigned *speed)
{
    if (!unit)
        *speed = miiphy_speed("eth0", CFG_PHY_ADDR);
}

#else
static inline void ag7240_phy_setup(int unit)
{
#ifdef CONFIG_AR7242_S16_PHY
    if ((is_ar7242() || is_wasp()) && (unit==0)) {
        athrs16_phy_setup(unit);
    } else
#endif
    {
        athrs26_phy_setup(unit);
#ifdef CONFIG_F1E_PHY
        athr_phy_setup(unit);
#endif
    }
}
  
static inline void ag7240_phy_link(int unit, unsigned *link)
{
#ifdef CONFIG_AR7242_S16_PHY
    if ((is_ar7242() || is_wasp()) && (unit==0)) {
         *link = athrs16_phy_is_up(unit);
    } else
#endif
    {
         *link = athrs26_phy_is_up(unit);
#ifdef CONFIG_F1E_PHY
         *link = athr_phy_is_up(unit);
#endif
    }
}
  
static inline void ag7240_phy_duplex(int unit, unsigned *duplex)
{
#ifdef CONFIG_AR7242_S16_PHY
    if ((is_ar7242() || is_wasp()) && (unit==0)) {
        *duplex = athrs16_phy_is_fdx(unit) ? FULL : HALF;
    } else
#endif
    {
        *duplex = athrs26_phy_is_fdx(unit) ? FULL : HALF;
#ifdef CONFIG_F1E_PHY
        *duplex = athr_phy_is_fdx(unit) ? FULL : HALF;
#endif
    }
}
  
static inline void ag7240_phy_speed(int unit, unsigned *speed)
{
#ifdef CONFIG_AR7242_S16_PHY
    if ((is_ar7242() || is_wasp()) && (unit==0)) {
        *speed = athrs16_phy_speed(unit);
    } else
#endif
    {
        *speed = athrs26_phy_speed(unit);
#ifdef CONFIG_F1E_PHY
        *speed = athr_phy_speed(unit);
#endif
    }
}
#endif
#endif /*_AG7240_PHY_H*/
