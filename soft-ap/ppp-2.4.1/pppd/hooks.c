
/* Taken from src/ppp-2.4.1/pppd */

#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include "pppd.h"
#include "hooks.h"
#include <util/stddefs.h>
#include "dispatcher/dispatcher.h"
#include "msgHandler/msgHandler.h"
#include "papi/MHApi.h"
#include "util/sibyte_msg.h"
#define USERLOGS_INCMSGS 1
#include "include/userlogs.h"
#include "pppd.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include "mgmtcommon.h"

#ifdef ARUBA_USE_ARUBA_LOG
#include <syslog.h>
#include <syslog/syslogutil.h>
#include <syslog/arubalog.h>
#endif /* ARUBA_USE_ARUBA_LOG */

#define IPTOSTR(a) (inet_ntoa(*(struct in_addr *)(&(a))))

static int             hooks_init = 0;
static DISPATCHER_ID   hooks_disp;
static Opaque          hooks_papihdl;

static void hooks_sndstatcb(char *buffer, int error, Opaque appArg);
static int  hooks_rcvcb(    char *buffer, int bufLen);
static int send_message_remote (unsigned short service, char *data, int datalen,
                            unsigned int ipaddr);

#define HOOKS_PAPI_PREALLOC_PACKET_SIZE 1024
#define HOOKS_PAPI_PREALLOC_PACKET_NUM    64
#define HOOKS_PAPI_TIMEOUT               100
#define HOOKS_PAPI_RETRY                   2

int my_new_phase_hook(int phase)
{
  int                 rc = 0;

  switch(phase)
  {
    case PHASE_ESTABLISH:
    {
      PAPI_Info           papiInfo;
      unsigned int        myservice;

      if (hooks_init)
      {
        /* Not setting rc because this isn't a fatal error */
        error("Trying to initialize the hooks more than once !!!");
        goto exit;
      }

      hooks_disp = DispatcherCreate();
      if (!hooks_disp)
      {
        error("Failed in Dispatcher Create");
        rc = -1;
        goto exit;
      }

      memset(&papiInfo, 0, sizeof(papiInfo));

      // PAPI port is based on ppp unit #. This is shared with pptp/l2tp's 
      // version of pppd. Unit # is assigned by the kernel and should not conflict
      // You cannot have more than PPPD_END - PPPD_START + 1 concurrent pppd
      myservice = (PPPD_START + (ifunit % (PPPD_END - PPPD_START + 1)));
      
      papiInfo.my_service              = htons(myservice);
      papiInfo.PAPI_SendStatusCallback = &hooks_sndstatcb;
      papiInfo.PAPI_ReceiveCallback    = &hooks_rcvcb;
      papiInfo.PacketSize              = HOOKS_PAPI_PREALLOC_PACKET_SIZE;
      papiInfo.NumPackets              = HOOKS_PAPI_PREALLOC_PACKET_NUM;
      papiInfo.args.timeout            = HOOKS_PAPI_TIMEOUT;
      papiInfo.args.retryCount         = HOOKS_PAPI_RETRY;
      
      hooks_papihdl = PAPI_Init (&papiInfo, hooks_disp);
      if (!hooks_papihdl) {  
          error("Failed in PAPI_Init"); rc = -1; goto exit;  
      }

      hooks_init = 1;
    }
    break;
    
    case PHASE_AUTHENTICATE:
        break;
    case PHASE_NETWORK:
        break;
    case PHASE_RUNNING :
        break;
    case PHASE_DISCONNECT :
        //TODO: Notify FPApps that we have disconnected. 
        printf("ppp: phase_disconnect\n");
        break;
    default:
        break;
  }

exit: 
  if (rc)
    HOOKS_TERMINATE_PPPD;

  return 0;
}

/* Send a raw message */
static int
send_message_remote (unsigned short service, char *data, int datalen,
                     unsigned int ipaddr)
{
        char *rv;
        int totlen;
        msgHndl_Hdr *h;
        int msgtype = * (int *) data;

        totlen = sizeof(msgHndl_Hdr) + datalen;
        h = (msgHndl_Hdr *) PAPI_Alloc(hooks_papihdl, totlen);

        if (h == NULL)
          return -1;

        /* fill in message header */
        h->DestIpAddr  = htonl(ipaddr);
        h->DestPortNum = htons(service);
        h->MessageCode = htons((short) msgtype);

        /* copy message body after header */
        memcpy(h + 1, data, datalen);

        rv = PAPI_Send(hooks_papihdl, 0, (char *) h, totlen, NON_BLOCKING_WO_ACK, 0);
        PAPI_Free(hooks_papihdl, (char *) h);
        if ((int) rv < 0) {
                arubaLog(LOG_CRIT, "PPPD: Error in sending PAPI message");
                return ERROR;
        }
        return OK;
}


void hooks_sndstatcb(char *buffer, int error, Opaque appArg)
{
  return;
}

static int  hooks_rcvcb(char *buffer, int bufLen)
{
  return 0;
}

void
hooks_set_addr(uint32_t client_ip, uint32_t netmask, uint32_t server_ip, 
                  uint32_t *dns_ip, uint32_t *wins_ip, uint32_t unit)
{
    fpappsMsgDynamicIP_t ppp_params;

    memset(&ppp_params, 0, sizeof(ppp_params));
    ppp_params.owner = IP_DYNAMIC_IP_OWNER_PPP;
    ppp_params.ipaddr = client_ip;
    ppp_params.ipmask = netmask;
    ppp_params.unit = ifunit;
    ppp_params.pid = getpid();
    strlcpy(ppp_params.tty, ppp_devnam, TTY_LEN);

    ppp_params.server_ip = server_ip;
    ppp_params.router_ip[0] = server_ip;
    if (dns_ip) {
        ppp_params.dns_ip[0] = dns_ip[0];
        if (dns_ip[0] != dns_ip[1]) {
            ppp_params.dns_ip[1] = dns_ip[1];
        }
    }

    if (wins_ip) {
        ppp_params.netbios_ip[0] = wins_ip[0];
        if (wins_ip[0] != wins_ip[1]) {
            ppp_params.netbios_ip[1] = wins_ip[1];
        }
    }

    ppp_params.msgType = FPAPPS_PPP_CONN_COMPLETE;
    send_message_remote(FASTPATH_SERVER, (char *)&ppp_params, sizeof(ppp_params),
                        INADDR_LOOPBACK);
}

