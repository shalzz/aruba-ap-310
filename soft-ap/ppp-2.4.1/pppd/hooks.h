#ifndef ARUBA_HOOKS_H
#define ARUBA_HOOKS_H

#define   HOOKS_TERMINATE_PPPD phase=PHASE_DEAD

extern void hooks_set_addr(unsigned int client_ip, unsigned int netmask, 
                           unsigned int server_ip, unsigned int *dns_ip, unsigned int *wins_ip,
                           unsigned int unit);

int my_new_phase_hook(int phase);

#endif /* ARUBA_HOOKS_H */
