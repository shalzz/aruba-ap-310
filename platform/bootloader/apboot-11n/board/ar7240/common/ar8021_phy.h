#ifndef _AR8021_PHY_H
#define _AR8021_PHY_H

#ifndef CEXTERN
#define  CEXTERN static inline
#endif

int ar8021_phy_setup(int unit);

unsigned int 
ar8021_phy_get_link_status(int unit, int *link, int *fdx, int *speed, unsigned int *cfg);

#endif
