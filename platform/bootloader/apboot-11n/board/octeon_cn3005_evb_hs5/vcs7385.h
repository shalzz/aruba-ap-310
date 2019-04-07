#ifndef __VCS7385_H_INCLUDED_
#define __VCS7385_H_INCLUDED_

#include "mpi.h"

extern int vcs7385_read (uint block, uint subblock, const uint reg, ulong * const value);
extern int vcs7385_write (uint block, uint subblock, const uint reg, ulong const value);

extern void cn3005_evb_hs5_vcs_chip_select(int onoff);
extern void cn3005_evb_hs5_vcs_reset(int onoff);

int vitesse_init(void);

#endif /* __VCS7385_H_INCLUDED_ */
