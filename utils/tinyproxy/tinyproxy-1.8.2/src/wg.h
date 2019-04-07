/*
 * Walled Garden
 */

#ifndef _TINYPROXY_WG_H_
#define _TINYPROXY_WG_H_

extern void wg_init (void);
extern void wg_add_wl (char *buf);
extern void wg_add_bl (char *buf);
extern int request_in_wl (const char *host, const char *url);
extern int request_in_bl (const char *host, const char *url);

/* dynamic walled garden */
extern void auto_wg_init(int create);
extern int request_in_auto_wl (char *host, char *url);
extern void auto_wg_add_wl (char *domain);
extern void auto_wg_reset(void);

void *malloc_wg_shared_memory(size_t size);

#endif
