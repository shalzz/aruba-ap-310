/*===========================================================================
FILE:
   GobiUSBNet.c

DESCRIPTION:
   Qualcomm USB Network device for Gobi 3000

FUNCTIONS:
   GobiNetSuspend
   GobiNetResume
   GobiNetDriverBind
   GobiNetDriverUnbind
   GobiUSBNetURBCallback
   GobiUSBNetTXTimeout
   GobiUSBNetAutoPMThread
   GobiUSBNetStartXmit
   GobiUSBNetOpen
   GobiUSBNetStop
   GobiUSBNetProbe
   GobiUSBNetModInit
   GobiUSBNetModExit

Copyright (c) 2011, Code Aurora Forum. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Code Aurora Forum nor
      the names of its contributors may be used to endorse or promote
      products derived from this software without specific prior written
      permission.

Alternatively, provided that this notice is retained in full, this software
may be relicensed by the recipient under the terms of the GNU General Public
License version 2 ("GPL") and only version 2, in which case the provisions of
the GPL apply INSTEAD OF those given above.  If the recipient relicenses the
software under the GPL, then the identification text in the MODULE_LICENSE
macro must be changed to reflect "GPLv2" instead of "Dual BSD/GPL".  Once a
recipient changes the license terms to the GPL, subsequent recipients shall
not relicense under alternate licensing terms, including the BSD or dual
BSD/GPL terms.  In addition, the following license statement immediately
below and between the words START and END shall also then apply when this
software is relicensed under the GPL:

START

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 and only version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

END

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
===========================================================================*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include "Structs.h"
#include "QMIDevice.h"
#include "QMI.h"
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/module.h>
#include <net/ip.h>

#include <asm/siginfo.h>	//siginfo
#include <linux/rcupdate.h>	//rcu_read_lock
#include <linux/sched.h>	//find_task_by_pid_type

#define BIT_9X15    (31)
//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Version Information
#define DRIVER_VERSION "2013-10-08/NTGR_2.21"
#define DRIVER_AUTHOR "Qualcomm Innovation Center"
#define DRIVER_DESC "GobiNet"
#define QOS_HDR_LEN (6)

// Debug flag
int debug = 0;
int qos_debug = 0;

/* The following definition is disabled (commented out) by default.
 * When uncommented it enables a feature that provides 'feedback' to an 
 * application about allocated and freed resources on transmit path  provided
 * CONFIG_PM is enabled in the kernel*/
/*#define TX_URB_MONITOR*/
#ifdef TX_URB_MONITOR
void (*URB_monitor) (bool);
EXPORT_SYMBOL(URB_monitor);
#endif

// Allow user interrupts
int interruptible = 1;

// Number of IP packets which may be queued up for transmit
int txQueueLength = 100;

// Class should be created during module init, so needs to be global
static struct class * gpClass;

/* atomic counter partially included in MAC address to make sure 2 devices
 * do not end up with the same MAC - concept breaks in case of > 255 ifaces
 */
static  atomic_t iface_counter = ATOMIC_INIT(0);

int threshold;
#ifdef CONFIG_PM
/*===========================================================================
METHOD:
   GobiNetSuspend (Public Method)

DESCRIPTION:
   Stops QMI traffic while device is suspended

PARAMETERS
   pIntf          [ I ] - Pointer to interface
   powerEvent     [ I ] - Power management event

RETURN VALUE:
   int - 0 for success
         negative errno for failure
===========================================================================*/
int GobiNetSuspend(
   struct usb_interface *     pIntf,
   pm_message_t               powerEvent )
{
   struct usbnet * pDev;
   sGobiUSBNet * pGobiDev;

   if (pIntf == 0)
   {
      return -ENOMEM;
   }

#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,23 ))
   pDev = usb_get_intfdata( pIntf );
#else
   pDev = (struct usbnet *)pIntf->dev.platform_data;
#endif

   if (pDev == NULL || pDev->net == NULL)
   {
      DBG( "failed to get netdevice\n" );
      return -ENXIO;
   }

   pGobiDev = (sGobiUSBNet *)pDev->data[0];
   if (pGobiDev == NULL)
   {
      DBG( "failed to get QMIDevice\n" );
      return -ENXIO;
   }

   // Is this autosuspend or system suspend?
   //    do we allow remote wakeup?
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,33 ))
   if (pDev->udev->auto_pm == 0)
#else
   if ((powerEvent.event & PM_EVENT_AUTO) == 0)
#endif
   {
      DBG( "device suspended to power level %d\n",
           powerEvent.event );
      GobiSetDownReason( pGobiDev, DRIVER_SUSPENDED );
   }
   else
   {
      DBG( "device autosuspend\n" );
   }

   if (powerEvent.event & PM_EVENT_SUSPEND)
   {
      // Stop QMI read callbacks
      KillRead( pGobiDev );
      pDev->udev->reset_resume = 0;

      // Store power state to avoid duplicate resumes
      pIntf->dev.power.power_state.event = powerEvent.event;
   }
   else
   {
      // Other power modes cause QMI connection to be lost
      pDev->udev->reset_resume = 1;
   }

   // Run usbnet's suspend function
   return usbnet_suspend( pIntf, powerEvent );
}

/*===========================================================================
METHOD:
   GobiNetResume (Public Method)

DESCRIPTION:
   Resume QMI traffic or recreate QMI device

PARAMETERS
   pIntf          [ I ] - Pointer to interface

RETURN VALUE:
   int - 0 for success
         negative errno for failure
===========================================================================*/
int GobiNetResume( struct usb_interface * pIntf )
{
   struct usbnet * pDev;
   sGobiUSBNet * pGobiDev;
   int nRet;
   int oldPowerState;

   if (pIntf == 0)
   {
      return -ENOMEM;
   }

#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,23 ))
   pDev = usb_get_intfdata( pIntf );
#else
   pDev = (struct usbnet *)pIntf->dev.platform_data;
#endif

   if (pDev == NULL || pDev->net == NULL)
   {
      DBG( "failed to get netdevice\n" );
      return -ENXIO;
   }

   pGobiDev = (sGobiUSBNet *)pDev->data[0];
   if (pGobiDev == NULL)
   {
      DBG( "failed to get QMIDevice\n" );
      return -ENXIO;
   }

   oldPowerState = pIntf->dev.power.power_state.event;
   pIntf->dev.power.power_state.event = PM_EVENT_ON;
   DBG( "resuming from power mode %d\n", oldPowerState );

   if (oldPowerState & PM_EVENT_SUSPEND)
   {
      // It doesn't matter if this is autoresume or system resume
      GobiClearDownReason( pGobiDev, DRIVER_SUSPENDED );

      nRet = usbnet_resume( pIntf );
      if (nRet != 0)
      {
         DBG( "usbnet_resume error %d\n", nRet );
         return nRet;
      }

      // Restart QMI read callbacks
      nRet = StartRead( pGobiDev );
      if (nRet != 0)
      {
         DBG( "StartRead error %d\n", nRet );
         return nRet;
      }

      // Kick Auto PM thread to process any queued URBs
      complete( &pGobiDev->mAutoPM.mThreadDoWork );
   }
   else
   {
      DBG( "nothing to resume\n" );
      return 0;
   }

   return nRet;
}
#endif /* CONFIG_PM */

/* very simplistic detection of IPv4 or IPv6 headers */
static bool possibly_iphdr(const char *data)
{
	return (data[0] & 0xd0) == 0x40;
}

/*===========================================================================
METHOD:
   GobiNetDriverBind (Public Method)

DESCRIPTION:
   Setup in and out pipes

PARAMETERS
   pDev           [ I ] - Pointer to usbnet device
   pIntf          [ I ] - Pointer to interface

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
static int GobiNetDriverBind(
   struct usbnet *         pDev,
   struct usb_interface *  pIntf )
{
   int numEndpoints;
   int endpointIndex;
   struct usb_host_endpoint * pEndpoint = NULL;
   struct usb_host_endpoint * pIn = NULL;
   struct usb_host_endpoint * pOut = NULL;

   // Verify one altsetting
   if (pIntf->num_altsetting != 1)
   {
      DBG( "invalid num_altsetting %u\n", pIntf->num_altsetting );
      return -ENODEV;
   }

   /* We only accept certain interfaces */
   if (pIntf->cur_altsetting->desc.bInterfaceClass != USB_CLASS_VENDOR_SPEC )
   {
      DBG( "Ignoring non vendor class interface #%d\n",
           pIntf->cur_altsetting->desc.bInterfaceNumber );
      return -ENODEV;
   }
   else if (pDev->driver_info->data &&
          !test_bit(pIntf->cur_altsetting->desc.bInterfaceNumber, &pDev->driver_info->data)) {
      DBG( "invalid interface %d\n",
           pIntf->cur_altsetting->desc.bInterfaceNumber );
      return -ENODEV;
   }

   // Collect In and Out endpoints
   numEndpoints = pIntf->cur_altsetting->desc.bNumEndpoints;
   for (endpointIndex = 0; endpointIndex < numEndpoints; endpointIndex++)
   {
      pEndpoint = pIntf->cur_altsetting->endpoint + endpointIndex;
      if (pEndpoint == NULL)
      {
         DBG( "invalid endpoint %u\n", endpointIndex );
         return -ENODEV;
      }

      if (usb_endpoint_dir_in( &pEndpoint->desc ) == true
      &&  usb_endpoint_xfer_int( &pEndpoint->desc ) == false)
      {
         pIn = pEndpoint;
      }
      else if (usb_endpoint_dir_out( &pEndpoint->desc ) == true)
      {
         pOut = pEndpoint;
      }
   }

   if (pIn == NULL || pOut == NULL)
   {
      DBG( "invalid endpoints\n" );
      return -ENODEV;
   }

   if (usb_set_interface( pDev->udev,
                          pIntf->cur_altsetting->desc.bInterfaceNumber,
                          0 ) != 0)
   {
      DBG( "unable to set interface\n" );
      return -ENODEV;
   }

   pDev->in = usb_rcvbulkpipe( pDev->udev,
                   pIn->desc.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK );
   pDev->out = usb_sndbulkpipe( pDev->udev,
                   pOut->desc.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK );

   DBG( "in %x, out %x\n",
        pIn->desc.bEndpointAddress,
        pOut->desc.bEndpointAddress );

   // In later versions of the kernel, usbnet helps with this
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,23 ))
   pIntf->dev.platform_data = (void *)pDev;
#endif

   /* make MAC addr easily distinguishable from an IP header */
   if (possibly_iphdr(pDev->net->dev_addr)) {
       pDev->net->dev_addr[0] |= 0x02;	/* set local assignment bit */
       pDev->net->dev_addr[0] &= 0xbf;	/* clear "IP" bit */
   }

   return 0;
}

/*===========================================================================
METHOD:
   GobiNetDriverUnbind (Public Method)

DESCRIPTION:
   Deregisters QMI device (Registration happened in the probe function)

PARAMETERS
   pDev           [ I ] - Pointer to usbnet device
   pIntfUnused    [ I ] - Pointer to interface

RETURN VALUE:
   None
===========================================================================*/
static void GobiNetDriverUnbind(
   struct usbnet *         pDev,
   struct usb_interface *  pIntf)
{
   sGobiUSBNet * pGobiDev = (sGobiUSBNet *)pDev->data[0];

   // Should already be down, but just in case...
   netif_carrier_off( pDev->net );

   DeregisterQMIDevice( pGobiDev );

#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 2,6,29 ))
   kfree( pDev->net->netdev_ops );
   pDev->net->netdev_ops = NULL;
#endif

#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,23 ))
   pIntf->dev.platform_data = NULL;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 2,6,19 ))
   pIntf->needs_remote_wakeup = 0;
#endif

   kfree( pGobiDev );
   pGobiDev = NULL;
}

struct sk_buff *GobiNetDriverTxQoS(
   struct usbnet  *pDev, 
   struct sk_buff *pSKB,
   gfp_t flags)
{
   unsigned char *p_qos_hdr;
   struct sGobiUSBNet * pGobiDev;

   pGobiDev = (sGobiUSBNet *)pDev->data[0];
   if (pGobiDev == NULL)
   {
      DBG( "failed to get QMIDevice NETDEV_TX_BUSY\n" );
      return NULL;
   }

   if (qos_debug)
   {
       print_hex_dump(KERN_INFO,  "QoS Header : ", DUMP_PREFIX_OFFSET,
               16, 1, pSKB->data, pSKB->len, true);
   }

   if (skb_headroom(pSKB) < QOS_HDR_LEN) {
       struct sk_buff *skb_new;

       skb_new = skb_realloc_headroom(pSKB, QOS_HDR_LEN);
       if (!skb_new) {
           dev_kfree_skb_any(pSKB);
           return NULL;
       }
       kfree_skb(pSKB);
       pSKB = skb_new;
   }

   // add QoS header(6B) at the beginning
   if ( (p_qos_hdr = skb_push(pSKB, QOS_HDR_LEN)) )
   {
      int j;
      struct iphdr* iph = ip_hdr(pSKB);
      QDBG("tos 0x%x\n", iph->tos);
      memset(p_qos_hdr, 0, QOS_HDR_LEN);
      /*
       * version : 1
       * reserve : 0
       */
      p_qos_hdr[0] = 1;
      p_qos_hdr[1] = 0;

      for(j=0;j<MAX_MAP;j++)
      {
          if ( iph->tos == pGobiDev->maps[j].tos )
          {
              switch( pGobiDev->maps[j].state) {
                  case FLOW_ACTIVATED:
                  case FLOW_ENABLED:
                      memcpy(&p_qos_hdr[2], &pGobiDev->maps[j].qosId, 4);
                      break;

                  case FLOW_DELETED:
                      //use default qos id, ie.zero (already resetted earlier)
                      break;

                  case FLOW_SUSPENDED:
                  default:
                      dev_kfree_skb_any(pSKB);
                      return NULL;
              };
              break;
          }
      }

      if (qos_debug)
      {
          print_hex_dump(KERN_INFO,  "QoS Header : ", DUMP_PREFIX_OFFSET,
                  16, 1, p_qos_hdr, QOS_HDR_LEN, true);
      }
      return pSKB;
   }
   else
   {
      QDBG( "Fail to preprend QoS header ");
   }

   // Filter the packet out, release it
   dev_kfree_skb_any(pSKB);
   return NULL;
}

#ifdef DATA_MODE_RP
/*===========================================================================
METHOD:
   GobiNetDriverTxFixup (Public Method)

DESCRIPTION:
   Handling data format mode on transmit path

PARAMETERS
   pDev           [ I ] - Pointer to usbnet device
   pSKB           [ I ] - Pointer to transmit packet buffer
   flags          [ I ] - os flags

RETURN VALUE:
   None
===========================================================================*/
struct sk_buff *GobiNetDriverTxFixup(
   struct usbnet  *pDev, 
   struct sk_buff *pSKB,
   gfp_t flags)
{
   DBG( "\n" );

   // Skip Ethernet header from message
   if (skb_pull(pSKB, ETH_HLEN))
   {
      DBG( "For sending to device modified: ");
      PrintHex (pSKB->data, pSKB->len);
      return pSKB;
   }
   else
   {
      DBG( "Packet Dropped ");
   }
   // Filter the packet out, release it
   dev_kfree_skb_any(pSKB);
   return NULL;
}

/*===========================================================================
METHOD:
   GobiNetDriverRxFixup (Public Method)

DESCRIPTION:
   Handling data format mode on receive path

PARAMETERS
   pDev           [ I ] - Pointer to usbnet device
   pSKB           [ I ] - Pointer to received packet buffer

RETURN VALUE:
   None
===========================================================================*/
static int GobiNetDriverRxFixup(
    struct usbnet  *pDev, 
    struct sk_buff *pSKB )
{
    sGobiUSBNet  *pGobiDev;
    char *pTempbuffer; /* temp buffer for data packets */
    struct ethhdr *pEth;
    struct iphdr *pIp;

    DBG( "\n" );

    pGobiDev = (sGobiUSBNet *)pDev->data[0];
    if (pGobiDev == NULL)
    {
       DBG( "failed to get Device\n" );
       return -ENXIO;
    }

    DBG( "RX From Device: ");
    PrintHex (pSKB->data, pSKB->len);

    pTempbuffer = kmalloc(pSKB->len, GFP_ATOMIC);

    /* Copy data section to a temporary buffer */
    memcpy(pTempbuffer, pSKB->data, pSKB->len);

    /* Prepare room for ethernet header */
    skb_put(pSKB, ETH_HLEN);

    /* Copy data with offset equal to Ethernet header length */
    memcpy((pSKB->data + ETH_HLEN), pTempbuffer, (pSKB->len - ETH_HLEN));

    /* Free the internal buffer; not needed */
    kfree(pTempbuffer);

    pSKB->dev = pDev->net;

    /* If the packet is IPv4 then add corresponding Ethernet header */
    if (((*(u8 *)(pSKB->data + ETH_HLEN)) & 0xF0) == 0x40)
    {
        /* IPV4 packet  */
        memcpy(pSKB->data, pGobiDev->eth_hdr_tmpl_ipv4, ETH_HLEN);
        pSKB->protocol = cpu_to_be16(ETH_P_IP);
        DBG( "IPv4 header added: ");
    }
    else  if (((*(u8 *)(pSKB->data + ETH_HLEN)) & 0xF0) == 0x60)
    {
        memcpy(pSKB->data, pGobiDev->eth_hdr_tmpl_ipv6, ETH_HLEN);
        pSKB->protocol = cpu_to_be16(ETH_P_IPV6);
        DBG( "IPv6 header added: ");
    }

    pIp = (struct iphdr *)((char *)pSKB->data + ETH_HLEN);
    if(pIp->version == 6) 
    {
        pEth = (struct ethhdr *)pSKB->data;
        pEth->h_proto = cpu_to_be16(ETH_P_IPV6);
    }

    else if(pIp->version == 4) 
    {
        pEth = (struct ethhdr *)pSKB->data;
        pEth->h_proto = cpu_to_be16(ETH_P_IP);
    }

    PrintHex (pSKB->data, pSKB->len);

    return 1;
}
#endif

#ifdef CONFIG_PM
/*===========================================================================
METHOD:
   GobiUSBNetURBCallback (Public Method)

DESCRIPTION:
   Write is complete, cleanup and signal that we're ready for next packet

PARAMETERS
   pURB     [ I ] - Pointer to sAutoPM struct

RETURN VALUE:
   None
===========================================================================*/
void GobiUSBNetURBCallback( struct urb * pURB )
{
   unsigned long activeURBflags;
   sAutoPM * pAutoPM = (sAutoPM *)pURB->context;
   if (pAutoPM == NULL)
   {
      // Should never happen
      DBG( "bad context\n" );
      return;
   }

   if (pURB->status != 0)
   {
      // Note that in case of an error, the behaviour is no different
      DBG( "urb finished with error %d\n", pURB->status );
   }

   // Remove activeURB (memory to be freed later)
   spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );

   // EAGAIN used to signify callback is done
   pAutoPM->mpActiveURB = ERR_PTR( -EAGAIN );

   spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

   complete( &pAutoPM->mThreadDoWork );

   usb_free_urb( pURB );

#ifdef TX_URB_MONITOR
   if (URB_monitor)
   {
       URB_monitor(false);
   }
#endif
}

/*===========================================================================
METHOD:
   GobiUSBNetTXTimeout (Public Method)

DESCRIPTION:
   Timeout declared by the net driver.  Stop all transfers

PARAMETERS
   pNet     [ I ] - Pointer to net device

RETURN VALUE:
   None
===========================================================================*/
void GobiUSBNetTXTimeout( struct net_device * pNet )
{
   struct sGobiUSBNet * pGobiDev;
   sAutoPM * pAutoPM;
   sURBList * pURBListEntry;
   unsigned long activeURBflags, URBListFlags;
   struct usbnet * pDev = netdev_priv( pNet );
   struct urb * pURB;

   if (pDev == NULL || pDev->net == NULL)
   {
      DBG( "failed to get usbnet device\n" );
      return;
   }

   pGobiDev = (sGobiUSBNet *)pDev->data[0];
   if (pGobiDev == NULL)
   {
      DBG( "failed to get QMIDevice\n" );
      return;
   }
   pAutoPM = &pGobiDev->mAutoPM;

   DBG( "\n" );

   // Grab a pointer to active URB
   spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );
   pURB = pAutoPM->mpActiveURB;
   spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

   // Stop active URB
   if (pURB != NULL)
   {
      usb_kill_urb( pURB );
   }


#ifdef TX_URB_MONITOR
   if (URB_monitor)
   {
       URB_monitor(false);
   }
#endif

   // Cleanup URB List
   spin_lock_irqsave( &pAutoPM->mURBListLock, URBListFlags );

   pURBListEntry = pAutoPM->mpURBList;
   while (pURBListEntry != NULL)
   {
      pAutoPM->mpURBList = pAutoPM->mpURBList->mpNext;
      atomic_dec( &pAutoPM->mURBListLen );
      usb_free_urb( pURBListEntry->mpURB );
      kfree( pURBListEntry );
      pURBListEntry = pAutoPM->mpURBList;
   }

   spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );

#ifdef TX_URB_MONITOR
   if (URB_monitor)
   {
       URB_monitor(false);
   }
#endif

   complete( &pAutoPM->mThreadDoWork );

   return;
}

/*===========================================================================
METHOD:
   GobiUSBNetAutoPMThread (Public Method)

DESCRIPTION:
   Handle device Auto PM state asynchronously
   Handle network packet transmission asynchronously

PARAMETERS
   pData     [ I ] - Pointer to sAutoPM struct

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
static int GobiUSBNetAutoPMThread( void * pData )
{
   unsigned long activeURBflags, URBListFlags;
   sURBList * pURBListEntry;
   int status;
   struct usb_device * pUdev;
   sAutoPM * pAutoPM = (sAutoPM *)pData;
   struct urb * pURB;

   if (pAutoPM == NULL)
   {
      DBG( "passed null pointer\n" );
      return -EINVAL;
   }

   pUdev = interface_to_usbdev( pAutoPM->mpIntf );

   DBG( "traffic thread started\n" );

   while (pAutoPM->mbExit == false)
   {
      // Wait for someone to poke us
      wait_for_completion_interruptible( &pAutoPM->mThreadDoWork );

      // Time to exit?
      if (pAutoPM->mbExit == true)
      {
         // Stop activeURB
         spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );
         pURB = pAutoPM->mpActiveURB;
         spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

         if (pURB != NULL)
         {
            usb_kill_urb( pURB );
         }
         // Will be freed in callback function

#ifdef TX_URB_MONITOR
         if (URB_monitor)
         {
            URB_monitor(false);
         }
#endif
         // Cleanup URB List
         spin_lock_irqsave( &pAutoPM->mURBListLock, URBListFlags );

         pURBListEntry = pAutoPM->mpURBList;
         while (pURBListEntry != NULL)
         {
            pAutoPM->mpURBList = pAutoPM->mpURBList->mpNext;
            atomic_dec( &pAutoPM->mURBListLen );
            usb_free_urb( pURBListEntry->mpURB );
            kfree( pURBListEntry );
            pURBListEntry = pAutoPM->mpURBList;
         }

         spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );
#ifdef TX_URB_MONITOR
         if (URB_monitor)
         {
             URB_monitor(false);
         }
#endif

         break;
      }

      // Is our URB active?
      spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );

      // EAGAIN used to signify callback is done
      if (IS_ERR( pAutoPM->mpActiveURB )
      &&  PTR_ERR( pAutoPM->mpActiveURB ) == -EAGAIN )
      {
         pAutoPM->mpActiveURB = NULL;

         // Restore IRQs so task can sleep
         spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

         // URB is done, decrement the Auto PM usage count
         usb_autopm_put_interface( pAutoPM->mpIntf );

         // Lock ActiveURB again
         spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );
      }

      if (pAutoPM->mpActiveURB != NULL)
      {
         // There is already a URB active, go back to sleep
         spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );
         continue;
      }

      // Is there a URB waiting to be submitted?
      spin_lock_irqsave( &pAutoPM->mURBListLock, URBListFlags );
      if (pAutoPM->mpURBList == NULL)
      {
         // No more URBs to submit, go back to sleep
         spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );
         spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );
         continue;
      }

      // Pop an element
      pURBListEntry = pAutoPM->mpURBList;
      pAutoPM->mpURBList = pAutoPM->mpURBList->mpNext;
      atomic_dec( &pAutoPM->mURBListLen );
      spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );

      // Set ActiveURB
      pAutoPM->mpActiveURB = pURBListEntry->mpURB;
      spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

      // Tell autopm core we need device woken up
      status = usb_autopm_get_interface( pAutoPM->mpIntf );
      if (status < 0)
      {
         DBG( "unable to autoresume interface: %d\n", status );

         // likely caused by device going from autosuspend -> full suspend
         if (status == -EPERM)
         {
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,33 ))
            pUdev->auto_pm = 0;
#endif
            GobiNetSuspend( pAutoPM->mpIntf, PMSG_SUSPEND );
         }

         // Add pURBListEntry back onto pAutoPM->mpURBList
         spin_lock_irqsave( &pAutoPM->mURBListLock, URBListFlags );
         pURBListEntry->mpNext = pAutoPM->mpURBList;
         pAutoPM->mpURBList = pURBListEntry;
         atomic_inc( &pAutoPM->mURBListLen );
         spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );

         spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );
         pAutoPM->mpActiveURB = NULL;
         spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );

         // Go back to sleep
         continue;
      }

      // Submit URB
      status = usb_submit_urb( pAutoPM->mpActiveURB, GFP_KERNEL );
      if (status < 0)
      {
         // Could happen for a number of reasons
         DBG( "Failed to submit URB: %d.  Packet dropped\n", status );
         spin_lock_irqsave( &pAutoPM->mActiveURBLock, activeURBflags );
         usb_free_urb( pAutoPM->mpActiveURB );
         pAutoPM->mpActiveURB = NULL;
         spin_unlock_irqrestore( &pAutoPM->mActiveURBLock, activeURBflags );
#ifdef TX_URB_MONITOR
         if (URB_monitor)
         {
             URB_monitor(false);
         }
#endif
         usb_autopm_put_interface( pAutoPM->mpIntf );

         // Loop again
         complete( &pAutoPM->mThreadDoWork );
      }

      kfree( pURBListEntry );
   }

   DBG( "traffic thread exiting\n" );
   pAutoPM->mpThread = NULL;
   return 0;
}

/*===========================================================================
METHOD:
   GobiUSBNetStartXmit (Public Method)

DESCRIPTION:
   Convert sk_buff to usb URB and queue for transmit

PARAMETERS
   pNet     [ I ] - Pointer to net device

RETURN VALUE:
   NETDEV_TX_OK on success
   NETDEV_TX_BUSY on error
===========================================================================*/
int GobiUSBNetStartXmit(
   struct sk_buff *     pSKB,
   struct net_device *  pNet )
{
   unsigned long URBListFlags;
   struct sGobiUSBNet * pGobiDev;
   sAutoPM * pAutoPM;
   sURBList * pURBListEntry, ** ppURBListEnd;
   void * pURBData;
   struct usbnet * pDev = netdev_priv( pNet );
   struct driver_info *info = pDev->driver_info;


   DBG( "\n" );

   if (pDev == NULL || pDev->net == NULL)
   {
      DBG( "failed to get usbnet device\n" );
      return NETDEV_TX_BUSY;
   }

   pGobiDev = (sGobiUSBNet *)pDev->data[0];
   if (pGobiDev == NULL)
   {
      DBG( "failed to get QMIDevice\n" );
      return NETDEV_TX_BUSY;
   }
   pAutoPM = &pGobiDev->mAutoPM;

   if( NULL == pSKB )
   {
       DBG( "Buffer is NULL \n" );
       return NETDEV_TX_BUSY;
   }

   if (GobiTestDownReason( pGobiDev, DRIVER_SUSPENDED ) == true)
   {
      // Should not happen
      DBG( "device is suspended\n" );
      dump_stack();
      return NETDEV_TX_BUSY;
   }

   // Convert the sk_buff into a URB

   // Check if buffer is full
   pGobiDev->tx_qlen  = atomic_read( &pAutoPM->mURBListLen );
   if ( pGobiDev->tx_qlen >= txQueueLength)
   {
      DBG( "not scheduling request, buffer is full\n" );
      return NETDEV_TX_BUSY;
   }

#if defined(DATA_MODE_RP) || defined(QOS_MODE)
   if (info->tx_fixup)
   {
      pSKB = info->tx_fixup( pDev, pSKB, GFP_ATOMIC);
      if (pSKB == NULL)
      {
         DBG( "unable to tx_fixup skb\n" );
         return NETDEV_TX_BUSY;
      }
   }
#endif

   // Allocate URBListEntry
   pURBListEntry = kmalloc( sizeof( sURBList ), GFP_ATOMIC );
   if (pURBListEntry == NULL)
   {
      DBG( "unable to allocate URBList memory\n" );
      if (pSKB)
         dev_kfree_skb_any ( pSKB );
      return NETDEV_TX_BUSY;
   }
   pURBListEntry->mpNext = NULL;

   // Allocate URB
   pURBListEntry->mpURB = usb_alloc_urb( 0, GFP_ATOMIC );
   if (pURBListEntry->mpURB == NULL)
   {
      DBG( "unable to allocate URB\n" );
      // release all memory allocated by now 
      if (pURBListEntry)
         kfree( pURBListEntry );
      if (pSKB)
         dev_kfree_skb_any ( pSKB );
      return NETDEV_TX_BUSY; 
   }

   // Allocate URB transfer_buffer
   pURBData = kmalloc( pSKB->len, GFP_ATOMIC );
   if (pURBData == NULL)
   {
      DBG( "unable to allocate URB data\n" );
      // release all memory allocated by now
      if (pURBListEntry)
      {
         usb_free_urb(pURBListEntry->mpURB);
#ifdef TX_URB_MONITOR
         if (URB_monitor)
         {
             URB_monitor(false);
         }
#endif
         kfree( pURBListEntry );
      }
      if (pSKB)
         dev_kfree_skb_any ( pSKB );
      return NETDEV_TX_BUSY; 
 
   }
   // Fill with SKB's data
   memcpy( pURBData, pSKB->data, pSKB->len );

   usb_fill_bulk_urb( pURBListEntry->mpURB,
                      pGobiDev->mpNetDev->udev,
                      pGobiDev->mpNetDev->out,
                      pURBData,
                      pSKB->len,
                      GobiUSBNetURBCallback,
                      pAutoPM );

   /* Handle the need to send a zero length packet and release the
    * transfer buffer
    */
    pURBListEntry->mpURB->transfer_flags |= (URB_ZERO_PACKET | URB_FREE_BUFFER);

   // Aquire lock on URBList
   spin_lock_irqsave( &pAutoPM->mURBListLock, URBListFlags );

   // Add URB to end of list
   ppURBListEnd = &pAutoPM->mpURBList;
   while ((*ppURBListEnd) != NULL)
   {
      ppURBListEnd = &(*ppURBListEnd)->mpNext;
   }
   *ppURBListEnd = pURBListEntry;
   atomic_inc( &pAutoPM->mURBListLen );

   spin_unlock_irqrestore( &pAutoPM->mURBListLock, URBListFlags );
#ifdef TX_URB_MONITOR
   if (URB_monitor)
   {
       URB_monitor(true);
   }
#endif

   complete( &pAutoPM->mThreadDoWork );

   // Start transfer timer
   pNet->trans_start = jiffies;
   // Free SKB
   if (pSKB)
      dev_kfree_skb_any ( pSKB );

   return NETDEV_TX_OK;
}
#endif /* CONFIG_PM */


/*===========================================================================
METHOD:
   GobiUSBNetOpen (Public Method)

DESCRIPTION:
   Wrapper to usbnet_open, correctly handling autosuspend
   Start AutoPM thread (if CONFIG_PM is defined)

PARAMETERS
   pNet     [ I ] - Pointer to net device

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
int GobiUSBNetOpen( struct net_device * pNet )
{
   int status = 0;
   struct sGobiUSBNet * pGobiDev;
   struct usbnet * pDev = netdev_priv( pNet );

   if (pDev == NULL)
   {
      DBG( "failed to get usbnet device\n" );
      return -ENXIO;
   }

   pGobiDev = (sGobiUSBNet *)pDev->data[0];
   if (pGobiDev == NULL)
   {
      DBG( "failed to get QMIDevice\n" );
      return -ENXIO;
   }

   DBG( "\n" );

#ifdef CONFIG_PM
   // Start the AutoPM thread
   pGobiDev->mAutoPM.mpIntf = pGobiDev->mpIntf;
   pGobiDev->mAutoPM.mbExit = false;
   pGobiDev->mAutoPM.mpURBList = NULL;
   pGobiDev->mAutoPM.mpActiveURB = NULL;
   spin_lock_init( &pGobiDev->mAutoPM.mURBListLock );
   spin_lock_init( &pGobiDev->mAutoPM.mActiveURBLock );
   atomic_set( &pGobiDev->mAutoPM.mURBListLen, 0 );
   init_completion( &pGobiDev->mAutoPM.mThreadDoWork );

   pGobiDev->mAutoPM.mpThread = kthread_run( GobiUSBNetAutoPMThread,
                                               &pGobiDev->mAutoPM,
                                               "GobiUSBNetAutoPMThread" );
   if (IS_ERR( pGobiDev->mAutoPM.mpThread ))
   {
      DBG( "AutoPM thread creation error\n" );
      return PTR_ERR( pGobiDev->mAutoPM.mpThread );
   }
#endif /* CONFIG_PM */

   // Allow traffic
   GobiClearDownReason( pGobiDev, NET_IFACE_STOPPED );

   // Pass to usbnet_open if defined
   if (pGobiDev->mpUSBNetOpen != NULL)
   {
      status = pGobiDev->mpUSBNetOpen( pNet );
#ifdef CONFIG_PM
      // If usbnet_open was successful enable Auto PM
      if (status == 0)
      {
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,33 ))
         usb_autopm_enable( pGobiDev->mpIntf );
#else
         usb_autopm_put_interface( pGobiDev->mpIntf );
#endif
      }
#endif /* CONFIG_PM */
   }
   else
   {
      DBG( "no USBNetOpen defined\n" );
   }

   return status;
}

/*===========================================================================
METHOD:
   GobiUSBNetStop (Public Method)

DESCRIPTION:
   Wrapper to usbnet_stop, correctly handling autosuspend
   Stop AutoPM thread (if CONFIG_PM is defined)

PARAMETERS
   pNet     [ I ] - Pointer to net device

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
int GobiUSBNetStop( struct net_device * pNet )
{
   struct sGobiUSBNet * pGobiDev;
   struct usbnet * pDev = netdev_priv( pNet );

   if (pDev == NULL || pDev->net == NULL)
   {
      DBG( "failed to get netdevice\n" );
      return -ENXIO;
   }

   pGobiDev = (sGobiUSBNet *)pDev->data[0];
   if (pGobiDev == NULL)
   {
      DBG( "failed to get QMIDevice\n" );
      return -ENXIO;
   }

   // Stop traffic
   GobiSetDownReason( pGobiDev, NET_IFACE_STOPPED );

#ifdef CONFIG_PM
   // Tell traffic thread to exit
   pGobiDev->mAutoPM.mbExit = true;
   complete( &pGobiDev->mAutoPM.mThreadDoWork );

   // Wait for it to exit
   while( pGobiDev->mAutoPM.mpThread != NULL )
   {
      msleep( 100 );
   }
   DBG( "thread stopped\n" );
#endif /* CONFIG_PM */

   // Pass to usbnet_stop, if defined
   if (pGobiDev->mpUSBNetStop != NULL)
   {
      return pGobiDev->mpUSBNetStop( pNet );
   }
   else
   {
      return 0;
   }
}
#ifndef DATA_MODE_RP
/* Make up an ethernet header if the packet doesn't have one.
 *
 * A firmware bug common among several devices cause them to send raw
 * IP packets under some circumstances.  There is no way for the
 * driver/host to know when this will happen.  And even when the bug
 * hits, some packets will still arrive with an intact header.
 *
 * The supported devices are only capably of sending IPv4, IPv6 and
 * ARP packets on a point-to-point link. Any packet with an ethernet
 * header will have either our address or a broadcast/multicast
 * address as destination.  ARP packets will always have a header.
 *
 * This means that this function will reliably add the appropriate
 * header iff necessary, provided our hardware address does not start
 * with 4 or 6.
 *
 * Another common firmware bug results in all packets being addressed
 * to 00:a0:c6:00:00:00 despite the host address being different.
 * This function will also fixup such packets.
 */
static int GobiNetDriverLteRxFixup(struct usbnet *dev, struct sk_buff *skb)
{
    __be16 proto;

    DBG( "From Modem: ");
    PrintHex (skb->data, skb->len);

    /* usbnet rx_complete guarantees that skb->len is at least
     * hard_header_len, so we can inspect the dest address without
     * checking skb->len
     */
    switch (skb->data[0] & 0xf0) {
        case 0x40:
            proto = htons(ETH_P_IP);
            break;
        case 0x60:
            proto = htons(ETH_P_IPV6);
            break;
        case 0x00:
            if (is_multicast_ether_addr(skb->data))
                return 1;
            /* possibly bogus destination - rewrite just in case */
            skb_reset_mac_header(skb);
            goto fix_dest;
        default:
            /* pass along other packets without modifications */
            return 1;
    }
    if (skb_headroom(skb) < ETH_HLEN)
        return 0;
    skb_push(skb, ETH_HLEN);
    skb_reset_mac_header(skb);
    eth_hdr(skb)->h_proto = proto;
    memset(eth_hdr(skb)->h_source, 0, ETH_ALEN);
fix_dest:
    memcpy(eth_hdr(skb)->h_dest, dev->net->dev_addr, ETH_ALEN);
    DBG( "To IP Stack: ");
    PrintHex (skb->data, skb->len);
    return 1;
}
#endif /* !DATA_MODE_RP */
/*=========================================================================*/
// Struct driver_info
/*=========================================================================*/
static const struct driver_info GobiNetInfo_qmi = {
   .description   = "QmiNet Ethernet Device",
   .flags         = FLAG_ETHER,
   .bind          = GobiNetDriverBind,
   .unbind        = GobiNetDriverUnbind,
//FIXME refactor below fixup handling at cases below
#ifdef DATA_MODE_RP
   .rx_fixup      = GobiNetDriverRxFixup,
   .tx_fixup      = GobiNetDriverTxFixup,
#elif defined(QOS_MODE)
   .tx_fixup      = GobiNetDriverTxQoS,
   //WORD AROUND integrated from qmi_wwan.c::qmi_wwan_rx_fixup
   .rx_fixup      = GobiNetDriverLteRxFixup,
#else
   .rx_fixup      = GobiNetDriverLteRxFixup,
#endif
   .data          = BIT(8) | BIT(19) |
                     BIT(10), /* MDM9x15 PDNs */
};

static const struct driver_info GobiNetInfo_gobi = {
   .description   = "GobiNet Ethernet Device",
   .flags         = FLAG_ETHER,
   .bind          = GobiNetDriverBind,
   .unbind        = GobiNetDriverUnbind,
#ifdef DATA_MODE_RP
   .rx_fixup      = GobiNetDriverRxFixup,
   .tx_fixup      = GobiNetDriverTxFixup,
#elif defined(QOS_MODE)
   .tx_fixup      = GobiNetDriverTxQoS,
   //WORD AROUND integrated from qmi_wwan.c::qmi_wwan_rx_fixup
   .rx_fixup      = GobiNetDriverLteRxFixup,
#else
   .rx_fixup      = GobiNetDriverLteRxFixup,
#endif
   .data          = BIT(0) | BIT(5),
};

static const struct driver_info GobiNetInfo_9x15 = {
   .description   = "GobiNet Ethernet Device",
   .flags         = FLAG_ETHER,
   .bind          = GobiNetDriverBind,
   .unbind        = GobiNetDriverUnbind,
#ifdef DATA_MODE_RP
   .rx_fixup      = GobiNetDriverRxFixup,
   .tx_fixup      = GobiNetDriverTxFixup,
#elif defined(QOS_MODE)
   .tx_fixup      = GobiNetDriverTxQoS,
   //WORD AROUND integrated from qmi_wwan.c::qmi_wwan_rx_fixup
   .rx_fixup      = GobiNetDriverLteRxFixup,
#else
   .rx_fixup      = GobiNetDriverLteRxFixup,
#endif
   .data          = BIT(8) | BIT(10) | BIT(11) | BIT(BIT_9X15),
};

#define QMI_G3K_DEVICE(vend, prod) \
	USB_DEVICE(vend, prod), \
	.driver_info = (unsigned long)&GobiNetInfo_gobi

#define QMI_9X15_DEVICE(vend, prod) \
	USB_DEVICE(vend, prod), \
	.driver_info = (unsigned long)&GobiNetInfo_9x15

/*=========================================================================*/
// Qualcomm Gobi 3000 VID/PIDs
/*=========================================================================*/
static const struct usb_device_id GobiVIDPIDTable [] =
{
   // Sierra Wireless MC7750 QMI Device VID/PID
   {
      USB_DEVICE( 0x1199, 0x68a2 ),
      .driver_info = (unsigned long)&GobiNetInfo_qmi,
   },
   // Sierra Wireless MC780x QMI Device VID/PID
   {
      USB_DEVICE( 0x1199, 0x68c0 ),
      .driver_info = (unsigned long)&GobiNetInfo_qmi,
   },

   // Gobi 3000
   {QMI_G3K_DEVICE(0x05c6, 0x920d)},
   {QMI_G3K_DEVICE(0x1199, 0x9011)},
   {QMI_G3K_DEVICE(0x1199, 0x9013)},
   {QMI_G3K_DEVICE(0x1199, 0x9015)},
   {QMI_G3K_DEVICE(0x1199, 0x9019)},
   {QMI_G3K_DEVICE(0x03f0, 0x371d)},
   // 9x15
   {QMI_9X15_DEVICE(0x1199, 0x9041)},
   {QMI_9X15_DEVICE(0x1199, 0x9051)},
   {QMI_9X15_DEVICE(0x1199, 0x9053)},
   {QMI_9X15_DEVICE(0x1199, 0x9054)},
   {QMI_9X15_DEVICE(0x1199, 0x9055)},
   {QMI_9X15_DEVICE(0x1199, 0x9056)},
   {QMI_9X15_DEVICE(0x1199, 0x9061)},
   {QMI_9X15_DEVICE(0x1199, 0x9057)},
   {QMI_9X15_DEVICE(0x0846, 0x68A2)},

   //Terminating entry
   { }
};

MODULE_DEVICE_TABLE( usb, GobiVIDPIDTable );

/*===========================================================================
METHOD:
   GobiUSBNetProbe (Public Method)

DESCRIPTION:
   Run usbnet_probe
   Setup QMI device

PARAMETERS
   pIntf        [ I ] - Pointer to interface
   pVIDPIDs     [ I ] - Pointer to VID/PID table

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
int GobiUSBNetProbe(
   struct usb_interface *        pIntf,
   const struct usb_device_id *  pVIDPIDs )
{
   int is9x15 = 0;
   unsigned char    ifacenum;
   int status;
   struct usbnet * pDev;
   sGobiUSBNet * pGobiDev;
   struct ethhdr *eth;

#if 0
   /* There exists a race condition in the firmware that sometimes results
    * in the absence of Ethernet framing of packets received from the device.
    * Therefore, a firmware work-around currently hard-codes the MAC address
    * to ensure valid Ethernet frames are sent to the host. We therefore
    * hard-code the network device MAC address to comply with the firmware
    */
   const char default_addr[6] = {0x00, 0xa0, 0xc6, 0x00, 0x00, 0x00};
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 2,6,29 ))
   struct net_device_ops * pNetDevOps;
#endif

   ifacenum =  pIntf->cur_altsetting->desc.bInterfaceNumber;

   status = usbnet_probe( pIntf, pVIDPIDs );
   if (status < 0)
   {
      DBG( "usbnet_probe failed %d\n", status );
      return status;
   }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 2,6,19 ))
   pIntf->needs_remote_wakeup = 1;
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,23 ))
   pDev = usb_get_intfdata( pIntf );
#else
   pDev = (struct usbnet *)pIntf->dev.platform_data;
#endif

   if (pDev == NULL || pDev->net == NULL)
   {
      DBG( "failed to get netdevice\n" );
      usbnet_disconnect( pIntf );
      return -ENXIO;
   }

   pGobiDev = kmalloc( sizeof( sGobiUSBNet ), GFP_KERNEL );
   if (pGobiDev == NULL)
   {
      DBG( "falied to allocate device buffers" );
      usbnet_disconnect( pIntf );
      return -ENOMEM;
   }

   pDev->data[0] = (unsigned long)pGobiDev;

   pGobiDev->mpNetDev = pDev;

#ifdef DATA_MODE_RP
   pDev->net->flags |= IFF_NOARP;
   DBG( "RawIP mode\n" );
#endif

   // Clearing endpoint halt is a magic handshake that brings 
   // the device out of low power (airplane) mode
   // NOTE: FCC verification should be done before this, if required
   usb_clear_halt( pGobiDev->mpNetDev->udev, pDev->out );

   // Overload PM related network functions
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,29 ))
   pGobiDev->mpUSBNetOpen = pDev->net->open;
   pDev->net->open = GobiUSBNetOpen;
   pGobiDev->mpUSBNetStop = pDev->net->stop;
   pDev->net->stop = GobiUSBNetStop;
#if 0
   pDev->net->hard_start_xmit = GobiUSBNetStartXmit;
   pDev->net->tx_timeout = GobiUSBNetTXTimeout;
#endif
   pDev->net->hard_start_xmit = redir_ether_usbnet_start_xmit;
   pDev->net->tx_timeout = usbnet_tx_timeout;
#else
   pNetDevOps = kmalloc( sizeof( struct net_device_ops ), GFP_KERNEL );
   if (pNetDevOps == NULL)
   {
      DBG( "falied to allocate net device ops" );
      usbnet_disconnect( pIntf );
      return -ENOMEM;
   }
   memcpy( pNetDevOps, pDev->net->netdev_ops, sizeof( struct net_device_ops ) );

   pGobiDev->mpUSBNetOpen = pNetDevOps->ndo_open;
   pNetDevOps->ndo_open = GobiUSBNetOpen;
   pGobiDev->mpUSBNetStop = pNetDevOps->ndo_stop;
   pNetDevOps->ndo_stop = GobiUSBNetStop;
#ifdef CONFIG_PM
   pNetDevOps->ndo_start_xmit = redir_ether_usbnet_start_xmit;
   pNetDevOps->ndo_tx_timeout = usbnet_tx_timeout;
   //pNetDevOps->ndo_start_xmit = GobiUSBNetStartXmit;
   //pNetDevOps->ndo_tx_timeout = GobiUSBNetTXTimeout;
#else
   pNetDevOps->ndo_start_xmit = redir_ether_usbnet_start_xmit;
   pNetDevOps->ndo_tx_timeout = usbnet_tx_timeout;
#endif /* CONFIG_PM */

   pDev->net->netdev_ops = pNetDevOps;
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,31 ))
   memset( &(pGobiDev->mpNetDev->stats), 0, sizeof( struct net_device_stats ) );
#else
   memset( &(pGobiDev->mpNetDev->net->stats), 0, sizeof( struct net_device_stats ) );
#endif

   pGobiDev->mpIntf = pIntf;
   memset( &(pGobiDev->mMEID), '0', 14 );

/*bug 145265: MAC address has been copyed from bond0 to ppp0 in usbnet.c, donot change this*/
#ifndef CONFIG_ARUBA_AP
   /* change MAC addr to include, ifacenum, and to be unique */
   pGobiDev->mpNetDev->net->dev_addr[ETH_ALEN-2] = atomic_inc_return(&iface_counter);
   pGobiDev->mpNetDev->net->dev_addr[ETH_ALEN-1] = ifacenum;
#endif

   DBG( "Mac Address:\n" );
   PrintHex( &pGobiDev->mpNetDev->net->dev_addr[0], 6 );
#if 0 /* interfers with multiple interface support and no longer appears to be necessary */
   /* Hard-code the host MAC address to comply with the firmware workaround */
   memcpy(&pGobiDev->mpNetDev->net->dev_addr[0], &default_addr[0], 6);
   DBG( "Default Mac Address:\n" );
   PrintHex( &pGobiDev->mpNetDev->net->dev_addr[0], 6 );
#endif
   /* Create ethernet header for IPv4 packets */
   eth = (struct ethhdr *)pGobiDev->eth_hdr_tmpl_ipv4;
   memcpy(&eth->h_dest, &pGobiDev->mpNetDev->net->dev_addr[0], ETH_ALEN);
   memcpy(&eth->h_source, &pGobiDev->mpNetDev->net->dev_addr[0], ETH_ALEN);
   eth->h_proto = cpu_to_be16(ETH_P_IP);

   /* Create ethernet header for IPv6 packets */
   eth = (struct ethhdr *)pGobiDev->eth_hdr_tmpl_ipv6;
   memcpy(&eth->h_dest, &pGobiDev->mpNetDev->net->dev_addr[0], ETH_ALEN);
   memcpy(&eth->h_source, &pGobiDev->mpNetDev->net->dev_addr[0], ETH_ALEN);
   eth->h_proto = cpu_to_be16(ETH_P_IPV6);

   pGobiDev->mbQMIValid = false;
   memset( &pGobiDev->mQMIDev, 0, sizeof( sQMIDev ) );
   pGobiDev->mQMIDev.mbCdevIsInitialized = false;

   pGobiDev->mQMIDev.mpDevClass = gpClass;

#ifdef CONFIG_PM
   init_completion( &pGobiDev->mAutoPM.mThreadDoWork );
#endif /* CONFIG_PM */
   spin_lock_init( &pGobiDev->mQMIDev.mClientMemLock );

   // Default to device down
   pGobiDev->mDownReason = 0;
   GobiSetDownReason( pGobiDev, NO_NDIS_CONNECTION );
   GobiSetDownReason( pGobiDev, NET_IFACE_STOPPED );

   // Register QMI
   if (pDev->driver_info->data &&
          test_bit(BIT_9X15, &pDev->driver_info->data)) {
       is9x15 = 1;
   }
   status = RegisterQMIDevice( pGobiDev, is9x15 );
   if (status != 0)
   {
      // usbnet_disconnect() will call GobiNetDriverUnbind() which will call
      // DeregisterQMIDevice() to clean up any partially created QMI device
      usbnet_disconnect( pIntf );
      return status;
   }

   // Success
   return 0;
}

static struct usb_driver GobiNet =
{
   .name       = "GobiNet",
   .id_table   = GobiVIDPIDTable,
   .probe      = GobiUSBNetProbe,
   .disconnect = usbnet_disconnect,
#ifdef CONFIG_PM
   .suspend    = GobiNetSuspend,
   .resume     = GobiNetResume,
   .supports_autosuspend = true,
#else
   .suspend    = NULL,
   .resume     = NULL,
   .supports_autosuspend = false,
#endif /* CONFIG_PM */
};

/*===========================================================================
METHOD:
   GobiUSBNetModInit (Public Method)

DESCRIPTION:
   Initialize module
   Create device class
   Register out usb_driver struct

RETURN VALUE:
   int - 0 for success
         Negative errno for error
===========================================================================*/
static int __init GobiUSBNetModInit( void )
{
   gpClass = class_create( THIS_MODULE, "GobiQMI" );
   if (IS_ERR( gpClass ) == true)
   {
      DBG( "error at class_create %ld\n",
           PTR_ERR( gpClass ) );
      return -ENOMEM;
   }

   // This will be shown whenever driver is loaded
   printk( KERN_INFO "%s: %s\n", DRIVER_DESC, DRIVER_VERSION );

   return usb_register( &GobiNet );
}
module_init( GobiUSBNetModInit );

/*===========================================================================
METHOD:
   GobiUSBNetModExit (Public Method)

DESCRIPTION:
   Deregister module
   Destroy device class

RETURN VALUE:
   void
===========================================================================*/
static void __exit GobiUSBNetModExit( void )
{
   usb_deregister( &GobiNet );

   class_destroy( gpClass );
}
module_exit( GobiUSBNetModExit );

MODULE_VERSION( DRIVER_VERSION );
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE( "Dual BSD/GPL" );

#ifdef bool
#undef bool
#endif

module_param( debug, int, S_IRUGO | S_IWUSR );
MODULE_PARM_DESC( debug, "Debuging enabled or not" );
module_param( qos_debug, int, S_IRUGO | S_IWUSR );
MODULE_PARM_DESC( qos_debug, "QoS Debuging enabled or not" );

module_param( interruptible, int, S_IRUGO | S_IWUSR );
MODULE_PARM_DESC( interruptible, "Listen for and return on user interrupt" );
module_param( txQueueLength, int, S_IRUGO | S_IWUSR );
MODULE_PARM_DESC( txQueueLength, 
                  "Number of IP packets which may be queued up for transmit" );

