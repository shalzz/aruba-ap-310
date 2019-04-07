#ifndef _ASAP_CMN_H
#define _ASAP_CMN_H

/***************************************************************
 * Nothing in this file should depend on any other Header file.
 * This file can be used to define types/macros that can be used
 * in sos, asap, fpapps etc
 * ************************************************************/

#define ASAP_ACL_NAME_LEN 64

/*please check that skb->flags(tunnel_id.h) & session_entry_t->flags
 *bit flags will work fine, when changing uplink reservation queues
 * >>>>>>>>>>>>>>>>>>>>>>>*/
#define MAX_UPLINK_BW_RESV 3
/* <<<<<<<<<<<<<<<<<<<<<<<*/

#define UPLINK_SYSCTL_COMMAND_INET_BW 0
#define UPLINK_SYSCTL_COMMAND_ACL_SET 1
#define UPLINK_SYSCTL_COMMAND_LOW_PRIORITY_LINK_ACTIVE 2
#define UPLINK_SYSCTL_COMMAND_HI_PRIORITY_LINK_ACTIVE 3

// Hard-coding here as inclusion of "msgHandler/msgHandler.h" causes trouble.
#ifdef ARUBA_PORT
#if (ARUBA_PORT != 8211)
#error "ARUBA_PORT = 8211 in this file!!"
#endif
#else
#define ARUBA_PORT		8211
#endif

#ifndef NUM_REPLAY_COUNTERS
#define NUM_REPLAY_COUNTERS 4
#endif

#if (NUM_REPLAY_COUNTERS == 1)
#define RSN_CAP_REPLAY 0x0000 /* 1  replay counter  */
#elif (NUM_REPLAY_COUNTERS == 2)
#define RSN_CAP_REPLAY 0x0014 /* 2  replay counters */
#elif (NUM_REPLAY_COUNTERS == 4) /* replay counters */
#define RSN_CAP_REPLAY 0x0028 /* 4  replay counters */
#elif (NUM_REPLAY_COUNTERS == 8)
#define RSN_CAP_REPLAY 0x003C /* 16 replay counters */
#elif (NUM_REPLAY_COUNTERS == 16)
#define RSN_CAP_REPLAY 0x003C /* 16 replay counters */
#else
#define RSN_CAP_REPLAY 0x0000 /* 1 replay counter   */
#error "Number of replay counters should be 1, 2, 4, 8 or 16"
#endif

#define ASAP_AM_MAX_PKT_LEN 2048

#endif /*_ASAP_CMN_H*/
