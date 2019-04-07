/* ========================================================================== */
/*                                                                            */
/*   cde_plat.h                                                               */
/*   (c) 2012 Broadcom Corporation                                                         */
/*                                                                            */
/*                                                              */
/*                                                                            */
/* ========================================================================== */
#ifndef MDE_MT_H
#define MDE_MT_H

#define ADDR_FAIL_CNT 64

#if 0
#define DBGP(format, arg...) printf("DEBUG: " format "\n", ## arg)
#else
#define DBGP(format, arg...) do {} while (0)
#endif
#define get_random_seed  host_get_elapsed_msec

#endif // MDE_MT_H



