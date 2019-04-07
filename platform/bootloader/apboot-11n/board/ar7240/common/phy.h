#ifndef _PHY_H
#define _PHY_H

#include <config.h>
/*
 * This file defines the interface between MAC and various phy switches.
 */
#define ag7240_unit2name(_unit) _unit ?  "eth1" : "eth0"
extern int  ag7240_miiphy_read(char *devname, unsigned char phaddr,
		unsigned char reg, unsigned short *val);
extern int  ag7240_miiphy_write(char *devname, unsigned char phaddr,
		unsigned char reg, unsigned short data);

#if 0
#define phy_reg_read(base, addr, reg)                    \
    ag7240_miiphy_read(ag7240_unit2name(base), addr, reg)
#else
static inline unsigned short
phy_reg_read(unsigned char base, unsigned char addr, unsigned char reg)
{
    unsigned short val;

    ag7240_miiphy_read(ag7240_unit2name(base), addr, reg, &val);
    return val;
}
#endif

#define phy_reg_write(base, addr, reg, data)                   \
        ag7240_miiphy_write(ag7240_unit2name(base), addr, reg, data)

#endif
