#ifndef __aruba_ap_h__
#define __aruba_ap_h__

typedef void (*ap_panic_dump_t)(void);
extern ap_panic_dump_t ap_panic_dump;
extern int ap_panic_dump_write(void *, loff_t, size_t, unsigned char *, int);
extern int ap_panic_dump_erase(void *, loff_t, size_t, int);
extern void aruba_panic_init(void);

#endif	/* __aruba_ap_h__ */
