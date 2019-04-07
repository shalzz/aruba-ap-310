#ifndef _UTIL_ARP_H_
#define _UTIL_ARP_H_

/*
 * Defines for the Address Resolution Protocol interface to the Linux
 * kernel.
 */

typedef struct {
    unsigned int   ip;
    unsigned char  mac[8];
} arp_entry_t;

/* ARP commands for ioctl call */
#define ARP_ENTRY_ADD    0
#define ARP_ENTRY_DEL    1
#define ARP_CLEAR_ALL    2

/* Software prototypes */
extern int hapiArpCommand(int command, arp_entry_t *entry, int entry_len);

/* Kernel function prototypes */
extern int arp_entry_add(arp_entry_t *entry);
extern int arp_entry_del(arp_entry_t *entry);
extern int arp_clear_all(void);

#endif /* _UTIL_ARP_H_ */
