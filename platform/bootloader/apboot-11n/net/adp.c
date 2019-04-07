#include <config.h>
#if (CONFIG_COMMANDS & CFG_CMD_NET)
/*
 * Copyright (C) 2002-2007 by Aruba Networks, Inc.
 * All Rights Reserved.
 * 
 * This software is an unpublished work and is protected by copyright and 
 * trade secret law.  Unauthorized copying, redistribution or other use of 
 * this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */
#include <common.h>
#include <command.h>
#include "net.h"
#include "adp.h"
#include "tftp.h"

#include "../../../../cmn/util/adp.h"

extern int net_check_prereq (proto_t protocol);

extern void ArpRequest(void);

#define TIMEOUT_COUNT 5
#define TIMEOUT       1

ushort		AdpID = 0x5555;
int		AdpMcastTry = 0;
int		AdpBcastTry = 0;
int             AdpTry = 0;
uchar           AdpMcastAddr[6] = { 0x01, 0x00, 0x5e, 0x00, 0x52, 0x0b };

static int AdpCheckPkt(uchar *pkt, unsigned dest, unsigned src, unsigned len)
{
    adp_hdr_t *ap = (adp_hdr_t *) pkt;
    int retval = 0;

    if (dest != ADP_UDP_PORT)	/* should already be in host order */
        retval = -1;
    if (len < sizeof (*ap) + 4)
        retval = -2;
    if ((ap->version != htons(ADP_VERSION_NUM)) ||
        (ap->type != htons(ADP_TYPE_RESPONSE)) ||
        (ap->id != htons(AdpID)))
        retval = -3;

    return retval;
}


/*
 *	Handle a BOOTP received packet.
 */
static void
AdpHandler(uchar * pkt, unsigned dest, unsigned src, unsigned len)
{
  adp_hdr_t *ap;

  if (AdpCheckPkt(pkt, dest, src, len))	/* Filter out pkts we don't want */
      return;

  /*
   *	Got a good ADP reply.  Copy the data into our variables.
   */
  ap = (adp_hdr_t *)pkt;

  memcpy(&NetServerIP, ap->data, sizeof(NetServerIP));
  if (!NetServerIP) 
      return;

  printf("Controller address: ");
  print_IPaddr(NetServerIP);
  printf("\n");

  NetSetTimeout(0, (thand_f *)0);

  // kick off TFTP
  TftpStart();
}

/*
 *	Timeout on BOOTP/DHCP request.
 */
static void
AdpTimeout(void)
{
    extern void DnsStart(void);

    char *s;
    if (AdpTry >= TIMEOUT_COUNT) {
        puts ("\nRetry count exceeded\n");
        if (((s = getenv("servername"))) && 
            strncpy(NetServerName, s, sizeof(NetServerName)-1) &&
            (NetOurDNSIP || 
             ((NetOurDNSIP = getenv_IPaddr ("dnsip"))))) {

            if (!NetOurGatewayIP) {
                NetOurGatewayIP = getenv_IPaddr ("gatewayip");
            }
            if (!NetOurSubnetMask) {
                NetOurSubnetMask= getenv_IPaddr ("netmask");
            }

            if ((*NetOurDomainName == 0) &&
                ((s = getenv("domainname")))) {
                strncpy(NetOurDomainName, s, sizeof(NetOurDomainName)-1);
            }

            if (net_check_prereq (DNS)) {
                goto again;
            }

            DnsStart ();
        } else {
        again:
            NetStartAgain ();
        }
    } else {
        NetSetTimeout (TIMEOUT * CFG_HZ, AdpTimeout);
        AdpRequest ();
    }
}

void
AdpRequest (void)
{
    volatile uchar *pkt, *iphdr;
    adp_hdr_t *ap;
    int pktlen, iplen;
    int mcast;

    pkt = NetTxPacket;
    memset ((void*)pkt, 0, PKTSIZE);

    if (AdpMcastTry > AdpBcastTry) {
      printf("ADP broadcast %d\n", ++AdpBcastTry);
      NetSetEther(pkt, NetBcastAddr, PROT_IP);
      mcast = 0;
    } else {
      printf("ADP multicast %d\n", ++AdpMcastTry);
      NetSetEther(pkt, AdpMcastAddr, PROT_IP);
      mcast = 1;
    }
    AdpTry++;
    pkt += ETHER_HDR_SIZE;

    iphdr = pkt;	/* We need this later for NetSetIP() */
    pkt += IP_HDR_SIZE;

    ap = (adp_hdr_t *)pkt;
    ap->version = htons(ADP_VERSION_NUM);
    ap->type = htons(ADP_TYPE_QUERY);
    ap->id = htons(++AdpID);
    memcpy (ap->data, NetOurEther, 6);

    /*
     * Calculate proper packet lengths taking into account the
     * variable size of the options field
     */
    pktlen = ETHER_HDR_SIZE + IP_HDR_SIZE + sizeof(*ap) + sizeof(NetOurEther);
    iplen = sizeof(*ap) + sizeof(NetOurEther);

    if (mcast) {
        NetSetIP(iphdr, htonl(ADP_IP_ADDR), ADP_UDP_PORT, ADP_UDP_PORT, iplen);
    } else {
        NetSetIP(iphdr, 0xffffffffL, ADP_UDP_PORT, ADP_UDP_PORT, iplen);
    }
    NetSetTimeout(TIMEOUT * CFG_HZ, AdpTimeout);

    NetSetHandler(AdpHandler);
    NetSendPacket(NetTxPacket, pktlen);
}
#endif /* (CONFIG_COMMANDS & CFG_CMD_NET) */
