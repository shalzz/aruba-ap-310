/*===========================================================================
FILE:
   GobiSerial.c

DESCRIPTION:
   Linux Qualcomm Serial USB driver Implementation

PUBLIC DRIVER FUNCTIONS:
   GobiProbe
   GobiOpen
   GobiClose
   GobiReadBulkCallback (if kernel is less than 2.6.25)
   GobiSerialSuspend
   GobiSerialResume (if kernel is less than 2.6.24)

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
==========================================================================*/
//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/slab.h>

/* Determine if we are in a particular kernel series */
#define KERNEL_SERIES(x, y) \
        ((LINUX_VERSION_CODE >> 8) == (KERNEL_VERSION( x,y,0 ) >> 8))

//---------------------------------------------------------------------------
// Global variables and definitions
//---------------------------------------------------------------------------

// Version Information
#define DRIVER_VERSION "2013-10-08/NTGR_2.12"
#define DRIVER_AUTHOR "Qualcomm Innovation Center"
#define DRIVER_DESC "GobiSerial"

#define NUM_BULK_EPS         1
#define MAX_BULK_EPS         6

#define SET_CONTROL_LINE_STATE_REQUEST_TYPE        0x21
#define SET_CONTROL_LINE_STATE_REQUEST             0x22
#define CONTROL_DTR                     0x01
#define CONTROL_RTS                     0x02


// Debug flag
static bool debug;
// flow control flag
static bool flow_control = 1;
// allow port open to success even when GPS control message failed
static bool ignore_gps_start_error = 1;

// Number of serial interfaces
static int nNumInterfaces;

// Global pointer to usb_serial_generic_close function
// This function is not exported, which is why we have to use a pointer
// instead of just calling it.
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,26 ))
   void (* gpClose)(
      struct usb_serial_port *,
      struct file * );
#elif (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,30 ))
   void (* gpClose)(
      struct tty_struct *,
      struct usb_serial_port *,
      struct file * );
#else // > 2.6.30
   void (* gpClose)( struct usb_serial_port * );
#endif

// DBG macro
#define DBG( format, arg... ) \
   if (debug == 1)\
   { \
      printk( KERN_INFO "GobiSerial::%s " format, __FUNCTION__, ## arg ); \
   } \

/*=========================================================================*/
// Function Prototypes
/*=========================================================================*/

// Attach to correct interfaces
static int GobiProbe(
   struct usb_serial * pSerial,
   const struct usb_device_id * pID );

// Start GPS if GPS port, run usb_serial_generic_open
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,26 ))
   int GobiOpen(
      struct usb_serial_port *   pPort,
      struct file *              pFilp );
#elif (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,31 ))
   int GobiOpen(
      struct tty_struct *        pTTY,
      struct usb_serial_port *   pPort,
      struct file *              pFilp );
#else // > 2.6.31
   int GobiOpen(
      struct tty_struct *        pTTY,
      struct usb_serial_port *   pPort );
#endif

// Stop GPS if GPS port, run usb_serial_generic_close
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,26 ))
   void GobiClose(
      struct usb_serial_port *,
      struct file * );
#elif (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,30 ))
   void GobiClose(
      struct tty_struct *,
      struct usb_serial_port *,
      struct file * );
#else // > 2.6.30
   void GobiClose( struct usb_serial_port * );
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,25 ))

// Read data from USB, push to TTY and user space
static void GobiReadBulkCallback( struct urb * pURB );

#endif

// Set reset_resume flag
int GobiSerialSuspend(
   struct usb_interface *     pIntf,
   pm_message_t               powerEvent );

#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,23 ))

// Restart URBs killed during usb_serial_suspend
int GobiSerialResume( struct usb_interface * pIntf );

#endif

#define MDM9X15_DEVICE(vend, prod) \
        USB_DEVICE(vend, prod), \
        .driver_info = BIT(1) | BIT(8) | BIT(10) | BIT(11)

#define G3K_DEVICE(vend, prod) \
        USB_DEVICE(vend, prod), \
        .driver_info = BIT(0)

#if ((KERNEL_SERIES( 3,4 ) && LINUX_VERSION_CODE >= KERNEL_VERSION( 3,4,34 )) || \
     (KERNEL_SERIES( 3,5 ) && LINUX_VERSION_CODE >= KERNEL_VERSION( 3,5,0 )) || \
     (KERNEL_SERIES( 3,7 ) && LINUX_VERSION_CODE >= KERNEL_VERSION( 3,7,10 )) || \
     (KERNEL_SERIES( 3,8 ) && LINUX_VERSION_CODE >= KERNEL_VERSION( 3,8,1) ))
/* workaround for upstream commit b2ca699076573c94fee9a73cb0d8645383b602a0 */
//#warning "Assuming disc_mutex is locked external to the module"
static inline void Gobi_lock_disc_mutex(struct usb_serial *serial) {
   WARN_ON(!mutex_is_locked(&serial->disc_mutex));
}
static inline void Gobi_unlock_disc_mutex(struct usb_serial *serial) {}
#else
/* use the legacy method of locking disc_mutex in this driver */
//#warning "Using legacy method of locking disc_mutex"
static inline void Gobi_lock_disc_mutex(struct usb_serial *serial) {
   WARN_ON(mutex_is_locked(&serial->disc_mutex));
   mutex_lock(&serial->disc_mutex);
}
static inline void Gobi_unlock_disc_mutex(struct usb_serial *serial) {
   mutex_unlock(&serial->disc_mutex);
}
#endif

/*=========================================================================*/
// Qualcomm Gobi 3000 VID/PIDs
/*=========================================================================*/
static struct usb_device_id GobiVIDPIDTable[] =
{
   { USB_DEVICE(0x05c6, 0x920c) },   // Gobi 3000 QDL
   { USB_DEVICE(0x05c6, 0x920d) },   // Gobi 3000 Composite
   /* Sierra Wireless QMI VID/PID */
   { USB_DEVICE(0x1199, 0x68A2),
      .driver_info = BIT(8) | BIT(19) | BIT(20) |
                     BIT(10) | BIT(11) /* in case a MDM9x15 switched to 0x68a2 */
   },
   /* Sierra Wireless QMI MC78/WP7/AR7 */
   { USB_DEVICE(0x1199, 0x68C0),
      .driver_info = BIT(8) | BIT(10) | BIT(11)
   },

   {G3K_DEVICE(0x1199, 0x9010)},
   {G3K_DEVICE(0x1199, 0x9011)},
   {G3K_DEVICE(0x1199, 0x9012)},
   {G3K_DEVICE(0x1199, 0x9013)},
   {G3K_DEVICE(0x1199, 0x9014)},
   {G3K_DEVICE(0x1199, 0x9015)},
   {G3K_DEVICE(0x1199, 0x9018)},
   {G3K_DEVICE(0x1199, 0x9019)},
   {G3K_DEVICE(0x03F0, 0x361D)},
   {G3K_DEVICE(0x03F0, 0x371D)},

   {MDM9X15_DEVICE(0x1199, 0x9040)},
   {MDM9X15_DEVICE(0x1199, 0x9041)},
   {MDM9X15_DEVICE(0x1199, 0x9051)},
   {MDM9X15_DEVICE(0x1199, 0x9053)},
   {MDM9X15_DEVICE(0x1199, 0x9054)},
   {MDM9X15_DEVICE(0x1199, 0x9055)},
   {MDM9X15_DEVICE(0x1199, 0x9056)},
   {MDM9X15_DEVICE(0x1199, 0x9060)},
   {MDM9X15_DEVICE(0x1199, 0x9061)},
   {MDM9X15_DEVICE(0x0846, 0x68A2)},
   /* Netgear AC341U IPT2 mode */
   { USB_DEVICE(0x1199, 0x9057),
      .driver_info = BIT(0) | BIT(1) |BIT(5)| BIT(8) | BIT(10) | BIT(11)
   },

   { }  // Terminating entry
};
MODULE_DEVICE_TABLE( usb, GobiVIDPIDTable );

/* per port private data */
struct sierra_port_private {
   /* Settings for the port */
   int rts_state;    /* Handshaking pins (outputs) */
   int dtr_state;
};


/*=========================================================================*/
// Struct usb_serial_driver
// Driver structure we register with the USB core
/*=========================================================================*/
static struct usb_driver GobiDriver =
{
   .name       = "GobiSerial",
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 3,5,0 ))
   .probe      = usb_serial_probe,
   .disconnect = usb_serial_disconnect,
#endif
   .id_table   = GobiVIDPIDTable,
#ifdef CONFIG_PM
   .suspend    = GobiSerialSuspend,
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,23 ))
   .resume     = GobiSerialResume,
#else
   .resume     = usb_serial_resume,
#endif
   .supports_autosuspend = true,
#else
   .suspend    = NULL,
   .resume     = NULL,
   .supports_autosuspend = false,
#endif
};

static int Gobi_calc_interface(struct usb_serial *serial)
{
   int interface;
   struct usb_interface *p_interface;
   struct usb_host_interface *p_host_interface;
   dev_dbg(&serial->dev->dev, "%s\n", __func__);

   /* Get the interface structure pointer from the serial struct */
   p_interface = serial->interface;

   /* Get a pointer to the host interface structure */
   p_host_interface = p_interface->cur_altsetting;

   /* read the interface descriptor for this active altsetting
    * to find out the interface number we are on
    */
   interface = p_host_interface->desc.bInterfaceNumber;

   return interface;
}

static int Gobi_send_setup(struct usb_serial_port *port)
{
   struct usb_serial *serial = port->serial;
   struct sierra_port_private *portdata;
   __u16 interface = 0;
   int val = 0;
   int retval;

   dev_dbg(&port->dev, "%s\n", __func__);

   portdata = usb_get_serial_port_data(port);

   if (portdata->dtr_state)
      val |= CONTROL_DTR;
   if (portdata->rts_state)
      val |= CONTROL_RTS;

   /* obtain interface for usb control message below */
   if (serial->num_ports == 1) {
      interface = Gobi_calc_interface(serial);
   }
   else {
      dev_err(&port->dev, 
            "flow control is not supported for %d serial port\n",
            serial->num_ports);
      return -ENODEV;
   }

   retval = usb_autopm_get_interface(serial->interface);
   if (retval < 0)
   {
      return retval;
   }

   retval = usb_control_msg(serial->dev, usb_rcvctrlpipe(serial->dev, 0),
         SET_CONTROL_LINE_STATE_REQUEST,
         SET_CONTROL_LINE_STATE_REQUEST_TYPE,
         val, interface, NULL, 0, USB_CTRL_SET_TIMEOUT);
   usb_autopm_put_interface(serial->interface);

   return retval;
}

static void Gobi_dtr_rts(struct usb_serial_port *port, int on)
{
   struct usb_serial *serial = port->serial;
   struct sierra_port_private *portdata;

   portdata = usb_get_serial_port_data(port);
   portdata->rts_state = on;
   portdata->dtr_state = on;
#if ((KERNEL_SERIES( 3,2 ) && LINUX_VERSION_CODE <= KERNEL_VERSION( 3,2,51 )))
   Gobi_send_setup(port);
#else
   /* only send down the usb control message if enabled */
   if (serial->dev && flow_control) {
      Gobi_lock_disc_mutex(serial);
      if (!serial->disconnected)
      {
         Gobi_send_setup(port);
      }
      Gobi_unlock_disc_mutex(serial);
   }
#endif
}

static int Gobi_startup(struct usb_serial *serial)
{
   struct usb_serial_port *port = NULL;
   struct sierra_port_private *portdata = NULL;
   int i;

   dev_dbg(&serial->dev->dev, "%s\n", __func__);

   if (serial->num_ports) {
      /* Note: One big piece of memory is allocated for all ports 
       * private data in one shot. This memory is split into equal 
       * pieces for each port. 
       */
      portdata = (struct sierra_port_private *)kzalloc
         (sizeof(*portdata) * serial->num_ports, GFP_KERNEL);
      if (!portdata) {
         dev_dbg(&serial->dev->dev, "%s: No memory!\n", __func__);
         return -ENOMEM;
      }
   }

   /* Now setup per port private data */
   for (i = 0; i < serial->num_ports; i++, portdata++) {
      port = serial->port[i];

      /* Set the port private data pointer */
      usb_set_serial_port_data(port, portdata);
   }

   return 0;
}

static void Gobi_release(struct usb_serial *serial)
{
   int i;
   struct usb_serial_port *port;
   struct sierra_intf_private *intfdata = serial->private;

   dev_dbg(&serial->dev->dev, "%s\n", __func__);

   if (serial->num_ports > 0) {
      port = serial->port[0];
      if (port)
      {
         /* Note: The entire piece of memory that was allocated 
          * in the startup routine can be released by passing
          * a pointer to the beginning of the piece.
          * This address corresponds to the address of the chunk
          * that was given to port 0.
          */
         kfree(usb_get_serial_port_data(port));
      }
   }

   for (i = 0; i < serial->num_ports; ++i) {
      port = serial->port[i];
      if (!port)
      {
         continue;
      }
      usb_set_serial_port_data(port, NULL);
   }
   kfree(intfdata);
}

/*=========================================================================*/
// Struct usb_serial_driver
/*=========================================================================*/
static struct usb_serial_driver gGobiDevice =
{
   .driver =
   {
      .owner     = THIS_MODULE,
      .name      = "GobiSerial driver",
   },
   .description         = "GobiSerial",
   .id_table            = GobiVIDPIDTable,
   .usb_driver          = &GobiDriver,
   .num_ports           = NUM_BULK_EPS,
   .probe               = GobiProbe,
   .open                = GobiOpen,
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,25 ))
   .num_interrupt_in    = NUM_DONT_CARE,
   .num_bulk_in         = 1,
   .num_bulk_out        = 1,
   .read_bulk_callback  = GobiReadBulkCallback,
#endif
   .dtr_rts             = Gobi_dtr_rts,
   .attach              = Gobi_startup,
   .release             = Gobi_release,
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION( 3,4,0 ))
static struct usb_serial_driver * const serial_drivers[] = {
   &gGobiDevice, NULL
};
#endif

//---------------------------------------------------------------------------
// USB serial core overridding Methods
//---------------------------------------------------------------------------
/*===========================================================================
METHOD:
   GobiProbe (Free Method)

DESCRIPTION:
   Attach to correct interfaces

PARAMETERS:
   pSerial    [ I ] - Serial structure
   pID        [ I ] - VID PID table

RETURN VALUE:
   int - negative error code on failure
         zero on success
===========================================================================*/
static int GobiProbe(
   struct usb_serial * pSerial,
   const struct usb_device_id * pID )
{
   // Assume failure
   int nRetval = -ENODEV;
   int nInterfaceNum;
   struct usb_host_endpoint * pEndpoint;
   int endpointIndex;
   int numEndpoints;

   DBG( "\n" );

   // Test parameters
   if ( (pSerial == NULL)
   ||   (pSerial->dev == NULL)
   ||   (pSerial->dev->actconfig == NULL)
   ||   (pSerial->interface == NULL)
   ||   (pSerial->interface->cur_altsetting == NULL)
   ||   (pSerial->type == NULL) )
   {
      DBG( "invalid parameter\n" );
      return -EINVAL;
   }

   nNumInterfaces = pSerial->dev->actconfig->desc.bNumInterfaces;
   DBG( "Num Interfaces = %d\n", nNumInterfaces );
   nInterfaceNum = pSerial->interface->cur_altsetting->desc.bInterfaceNumber;
   DBG( "This Interface = %d\n", nInterfaceNum );

   if (nNumInterfaces == 1)
   {
      // QDL mode?
      if ((nInterfaceNum == 0) || (nInterfaceNum == 1))
      {
         DBG( "QDL port found\n" );
         nRetval = usb_set_interface( pSerial->dev,
                                      nInterfaceNum,
                                      0 );
         if (nRetval < 0)
         {
            DBG( "Could not set interface, error %d\n", nRetval );
         }
      }
      else
      {
         DBG( "Incorrect QDL interface number\n" );
      }
   }
   else if (nNumInterfaces > 1)
   {
      /* Composite mode */
      if (pSerial->interface->cur_altsetting->desc.bInterfaceClass != USB_CLASS_VENDOR_SPEC )
      {
         DBG( "Ignoring non vendor class interface #%d\n", nInterfaceNum );
         return -ENODEV;
      }
      else if (pID->driver_info &&
             test_bit(nInterfaceNum, &pID->driver_info)) {
         DBG( "Ignoring blacklisted interface #%d\n", nInterfaceNum );
         return -ENODEV;
      }
      else
      {
         nRetval = usb_set_interface( pSerial->dev,
                                      nInterfaceNum,
                                      0 );
         if (nRetval < 0)
         {
            DBG( "Could not set interface, error %d\n", nRetval );
         }

         // Check for recursion
         if (pSerial->type->close != GobiClose)
         {
            // Store usb_serial_generic_close in gpClose
            gpClose = pSerial->type->close;
            pSerial->type->close = GobiClose;
         }
      }
   }
   if (nRetval == 0 && nNumInterfaces > 1 )
   {
      // Clearing endpoint halt is a magic handshake that brings
      // the device out of low power (airplane) mode
      // NOTE: FCC verification should be done before this, if required
      numEndpoints = pSerial->interface->cur_altsetting
                         ->desc.bInterfaceNumber;

      for (endpointIndex = 0; endpointIndex < numEndpoints; endpointIndex++)
      {
         pEndpoint = pSerial->interface->cur_altsetting->endpoint
                   + endpointIndex;

         if (pEndpoint != NULL
         &&  usb_endpoint_dir_out( &pEndpoint->desc ) == true)
         {
            int pipe = usb_sndbulkpipe( pSerial->dev,
                                        pEndpoint->desc.bEndpointAddress );
            nRetval = usb_clear_halt( pSerial->dev, pipe );

            // Should only be one
            break;
         }
      }
   }

   return nRetval;
}

/*===========================================================================
METHOD:
   IsGPSPort (Free Method)

DESCRIPTION:
   Determines whether the interface is GPS port

PARAMETERS:
   pPort   [ I ] - USB serial port structure

RETURN VALUE:
   bool- true if this is a GPS port
       - false otherwise
===========================================================================*/
bool IsGPSPort(struct usb_serial_port *   pPort )
{
   DBG( "Product=0x%x, Interface=0x%x\n",
        cpu_to_le16(pPort->serial->dev->descriptor.idProduct),
        pPort->serial->interface->cur_altsetting->desc.bInterfaceNumber);

   switch (cpu_to_le16(pPort->serial->dev->descriptor.idProduct))
   {
      case 0x68A2:  /* Sierra or Netgear QMI */
      case 0x68C0:  /* Sierra Wireless MC780x QMI Device PID*/
         if (pPort->serial->interface->cur_altsetting->desc.bInterfaceNumber == 2)
            return true;
         break;

      case 0x9011:  /* Sierra Wireless G3K */
      case 0x9013:  /* Sierra Wireless G3K */
      case 0x9015:  /* Sierra Wireless G3K */
      case 0x9019:  /* Sierra Wireless G3K */
      case 0x371D:  /* G3K */
         if (pPort->serial->interface->cur_altsetting->desc.bInterfaceNumber == 3)
            return true;
         break;

      default:
         return false;
         break;
  }
  return false;
}

/*===========================================================================
METHOD:
   GobiOpen (Free Method)

DESCRIPTION:
   Start GPS if GPS port, run usb_serial_generic_open

PARAMETERS:
   pTTY    [ I ] - TTY structure (only on kernels <= 2.6.26)
   pPort   [ I ] - USB serial port structure
   pFilp   [ I ] - File structure (only on kernels <= 2.6.31)

RETURN VALUE:
   int - zero for success
       - negative errno on error
===========================================================================*/
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,26 ))
int GobiOpen(
   struct usb_serial_port *   pPort,
   struct file *              pFilp )
#elif (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,31 ))
int GobiOpen(
   struct tty_struct *        pTTY,
   struct usb_serial_port *   pPort,
   struct file *              pFilp )
#else // > 2.6.31
int GobiOpen(
   struct tty_struct *        pTTY,
   struct usb_serial_port *   pPort )
#endif
{
   const char startMessage[] = "$GPS_START";
   int nResult;
   int bytesWrote;

   DBG( "\n" );

   // Test parameters
   if ( (pPort == NULL)
   ||   (pPort->serial == NULL)
   ||   (pPort->serial->dev == NULL)
   ||   (pPort->serial->interface == NULL)
   ||   (pPort->serial->interface->cur_altsetting == NULL) )
   {
      DBG( "invalid parameter\n" );
      return -EINVAL;
   }

   // Is this the GPS port?
   if ((IsGPSPort(pPort)) == true)
   {
      // Send startMessage, 1s timeout
      nResult = usb_bulk_msg( pPort->serial->dev,
                              usb_sndbulkpipe( pPort->serial->dev,
                                               pPort->bulk_out_endpointAddress ),
                              (void *)&startMessage[0],
                              sizeof( startMessage ),
                              &bytesWrote,
                              1000 );
      if (nResult != 0)
      {
         DBG( "error %d sending startMessage\n", nResult );
         if (!ignore_gps_start_error)
         {
            return nResult;
         }
      }
      if (bytesWrote != sizeof( startMessage ))
      {
         DBG( "invalid write size %d, %lu\n",
              bytesWrote,
              (unsigned long)sizeof( startMessage ) );
         if (!ignore_gps_start_error)
         {
            return -EIO;
         }
      }
   }

   // Clear endpoint halt condition
   if( nNumInterfaces > 1 )
   {
      nResult = usb_clear_halt(pPort->serial->dev,
                               usb_sndbulkpipe(pPort->serial->dev,
                               pPort->bulk_in_endpointAddress) | USB_DIR_IN );
      if (nResult != 0)
      {
         DBG( "usb_clear_halt return value = %d\n", nResult );
      }
   }

   // Pass to usb_serial_generic_open
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,26 ))
   return usb_serial_generic_open( pPort, pFilp );
#elif (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,31 ))
   return usb_serial_generic_open( pTTY, pPort, pFilp );
#else // > 2.6.31
   return usb_serial_generic_open( pTTY, pPort );
#endif
}

/*===========================================================================
METHOD:
   GobiClose (Free Method)

DESCRIPTION:
   Stop GPS if GPS port, run usb_serial_generic_close

PARAMETERS:
   pTTY    [ I ] - TTY structure (only if kernel > 2.6.26 and <= 2.6.29)
   pPort   [ I ] - USB serial port structure
   pFilp   [ I ] - File structure (only on kernel <= 2.6.30)
===========================================================================*/
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,26 ))
void GobiClose(
   struct usb_serial_port *   pPort,
   struct file *              pFilp )
#elif (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,30 ))
void GobiClose(
   struct tty_struct *        pTTY,
   struct usb_serial_port *   pPort,
   struct file *              pFilp )
#else // > 2.6.30
void GobiClose( struct usb_serial_port * pPort )
#endif
{
   const char stopMessage[] = "$GPS_STOP";
   int nResult;
   int bytesWrote;

   DBG( "\n" );

   // Test parameters
   if ( (pPort == NULL)
   ||   (pPort->serial == NULL)
   ||   (pPort->serial->dev == NULL)
   ||   (pPort->serial->interface == NULL)
   ||   (pPort->serial->interface->cur_altsetting == NULL) )
   {
      DBG( "invalid parameter\n" );
      return;
   }

   // Is this the GPS port?
   if ((IsGPSPort(pPort)) == true)
   {
      // Send stopMessage, 1s timeout
      nResult = usb_bulk_msg( pPort->serial->dev,
                              usb_sndbulkpipe( pPort->serial->dev,
                                               pPort->bulk_out_endpointAddress ),
                              (void *)&stopMessage[0],
                              sizeof( stopMessage ),
                              &bytesWrote,
                              1000 );
      if (nResult != 0)
      {
         DBG( "error %d sending stopMessage\n", nResult );
      }
      if (bytesWrote != sizeof( stopMessage ))
      {
         DBG( "invalid write size %d, %lu\n",
              bytesWrote,
              (unsigned long)sizeof( stopMessage ) );
      }
   }

   // Pass to usb_serial_generic_close
   if (gpClose == NULL)
   {
      DBG( "NULL gpClose\n" );
      return;
   }

#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,26 ))
   gpClose( pPort, pFilp );
#elif (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,30 ))
   gpClose( pTTY, pPort, pFilp );
#else // > 2.6.30
   gpClose( pPort );
#endif
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2,6,25 ))

/*===========================================================================
METHOD:
   GobiReadBulkCallback (Free Method)

DESCRIPTION:
   Read data from USB, push to TTY and user space

PARAMETERS:
   pURB  [ I ] - USB Request Block (urb) that called us

RETURN VALUE:
===========================================================================*/
static void GobiReadBulkCallback( struct urb * pURB )
{
   struct usb_serial_port * pPort = pURB->context;
   struct tty_struct * pTTY = pPort->tty;
   int nResult;
   int nRoom = 0;
   unsigned int pipeEP;

   DBG( "port %d\n", pPort->number );

   if (pURB->status != 0)
   {
      DBG( "nonzero read bulk status received: %d\n", pURB->status );

      return;
   }

   usb_serial_debug_data( debug,
                          &pPort->dev,
                          __FUNCTION__,
                          pURB->actual_length,
                          pURB->transfer_buffer );

   // We do no port throttling

   // Push data to tty layer and user space read function
   if (pTTY != 0 && pURB->actual_length)
   {
      nRoom = tty_buffer_request_room( pTTY, pURB->actual_length );
      DBG( "room size %d %d\n", nRoom, 512 );
      if (nRoom != 0)
      {
         tty_insert_flip_string( pTTY, pURB->transfer_buffer, nRoom );
         tty_flip_buffer_push( pTTY );
      }
   }

   pipeEP = usb_rcvbulkpipe( pPort->serial->dev,
                             pPort->bulk_in_endpointAddress );

   // For continuous reading
   usb_fill_bulk_urb( pPort->read_urb,
                      pPort->serial->dev,
                      pipeEP,
                      pPort->read_urb->transfer_buffer,
                      pPort->read_urb->transfer_buffer_length,
                      GobiReadBulkCallback,
                      pPort );

   nResult = usb_submit_urb( pPort->read_urb, GFP_ATOMIC );
   if (nResult != 0)
   {
      DBG( "failed resubmitting read urb, error %d\n", nResult );
   }
}

#endif

#ifdef CONFIG_PM
/*===========================================================================
METHOD:
   GobiSerialSuspend (Public Method)

DESCRIPTION:
   Set reset_resume flag

PARAMETERS
   pIntf          [ I ] - Pointer to interface
   powerEvent     [ I ] - Power management event

RETURN VALUE:
   int - 0 for success
         negative errno for failure
===========================================================================*/
int GobiSerialSuspend(
   struct usb_interface *     pIntf,
   pm_message_t               powerEvent )
{
   struct usb_serial * pDev;

   if (pIntf == 0)
   {
      return -ENOMEM;
   }

   pDev = usb_get_intfdata( pIntf );
   if (pDev == NULL)
   {
      return -ENXIO;
   }

   // Unless this is PM_EVENT_SUSPEND, make sure device gets rescanned
   if ((powerEvent.event & PM_EVENT_SUSPEND) == 0)
   {
      pDev->dev->reset_resume = 1;
   }

   // Run usb_serial's suspend function
   return usb_serial_suspend( pIntf, powerEvent );
}
#endif /* CONFIG_PM*/

#ifdef CONFIG_PM
#if (LINUX_VERSION_CODE <= KERNEL_VERSION( 2,6,23 ))

/*===========================================================================
METHOD:
   GobiSerialResume (Free Method)

DESCRIPTION:
   Restart URBs killed during usb_serial_suspend

   Fixes 2 bugs in 2.6.23 kernel
      1. pSerial->type->resume was NULL and unchecked, caused crash.
      2. set_to_generic_if_null was not run for resume.

PARAMETERS:
   pIntf  [ I ] - Pointer to interface

RETURN VALUE:
   int - 0 for success
         negative errno for failure
===========================================================================*/
int GobiSerialResume( struct usb_interface * pIntf )
{
   struct usb_serial * pSerial = usb_get_intfdata( pIntf );
   struct usb_serial_port * pPort;
   int portIndex, errors, nResult;

   if (pSerial == NULL)
   {
      DBG( "no pSerial\n" );
      return -ENOMEM;
   }
   if (pSerial->type == NULL)
   {
      DBG( "no pSerial->type\n" );
      return ENOMEM;
   }
   if (pSerial->type->resume == NULL)
   {
      // Expected behaviour in 2.6.23, in later kernels this was handled
      // by the usb-serial driver and usb_serial_generic_resume
      errors = 0;
      for (portIndex = 0; portIndex < pSerial->num_ports; portIndex++)
      {
         pPort = pSerial->port[portIndex];
         if (pPort->open_count > 0 && pPort->read_urb != NULL)
         {
            nResult = usb_submit_urb( pPort->read_urb, GFP_NOIO );
            if (nResult < 0)
            {
               // Return first error we see
               DBG( "error %d\n", nResult );
               return nResult;
            }
         }
      }

      // Success
      return 0;
   }

   // Execution would only reach this point if user has
   // patched version of usb-serial driver.
   return usb_serial_resume( pIntf );
}

#endif
#endif /* CONFIG_PM*/
/*===========================================================================
METHOD:
   GobiInit (Free Method)

DESCRIPTION:
   Register the driver and device

PARAMETERS:

RETURN VALUE:
   int - negative error code on failure
         zero on success
===========================================================================*/
static int __init GobiInit( void )
{
   int nRetval = 0;
   gpClose = NULL;

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 3,4,0 ))
   // Registering driver to USB serial core layer
   nRetval = usb_serial_register( &gGobiDevice );
   if (nRetval != 0)
   {
      return nRetval;
   }

   // Registering driver to USB core layer
   nRetval = usb_register( &GobiDriver );
   if (nRetval != 0)
   {
      usb_serial_deregister( &gGobiDevice );
      return nRetval;
   }

   // This will be shown whenever driver is loaded
   printk( KERN_INFO "%s: %s\n", DRIVER_DESC, DRIVER_VERSION );

#else
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 3,5,0 ))
   nRetval = usb_serial_register_drivers(serial_drivers, KBUILD_MODNAME, GobiVIDPIDTable);
#else
   nRetval = usb_serial_register_drivers(&GobiDriver, serial_drivers);
#endif
   if (nRetval == 0)
   {
      printk(KERN_INFO KBUILD_MODNAME ": " DRIVER_VERSION ":"
            DRIVER_DESC "\n");
   }
#endif


   return nRetval;
}

/*===========================================================================
METHOD:
   GobiExit (Free Method)

DESCRIPTION:
   Deregister the driver and device

PARAMETERS:

RETURN VALUE:
===========================================================================*/
static void __exit GobiExit( void )
{
   gpClose = NULL;
#if (LINUX_VERSION_CODE < KERNEL_VERSION( 3,4,0 ))
   usb_deregister( &GobiDriver );
   usb_serial_deregister( &gGobiDevice );
#else
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 3,5,0 ))
   usb_serial_deregister_drivers(serial_drivers);
#else
   usb_serial_deregister_drivers(&GobiDriver, serial_drivers);
#endif
#endif
}

// Calling kernel module to init our driver
module_init( GobiInit );
module_exit( GobiExit );

MODULE_VERSION( DRIVER_VERSION );
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE( "Dual BSD/GPL" );

module_param( debug, bool, S_IRUGO | S_IWUSR );
MODULE_PARM_DESC( debug, "Debug enabled or not" );
module_param( flow_control, bool, S_IRUGO | S_IWUSR );
MODULE_PARM_DESC( flow_control, "flow control enabled or not" );
module_param( ignore_gps_start_error, bool, S_IRUGO | S_IWUSR );
MODULE_PARM_DESC( ignore_gps_start_error, 
   "allow port open to success even when GPS control message failed");

