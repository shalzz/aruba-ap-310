/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.

*******************************************************************************/

#include <config.h> 
#include <common.h>
#include <command.h>

#include "mvOs.h"
#include "mvCommon.h"
#include "mvDebug.h"
#include "mvBoardEnvSpec.h"
#include "mvUsb.h"
#include "mvUsbDevApi.h"
#include "disk.h"
#include "mouse.h"

#define USB_DISK_DEFAULT_SIZE   2048

extern MV_U32   mvUsbGetCapRegAddr(int devNo);

typedef struct
{
    _usb_device_handle  handle;
    MV_BOOL             isMouse;
    int     		    gppNo;

} MV_USB_DEVICE;

MV_BOOL         mvUsbIsFirst = MV_TRUE;
MV_USB_DEVICE   mvUsbDevice[MV_BOARD_MAX_USB_IF];
 

USB_IMPORT_FUNCS    usbImportFuncs = 
{
    printf,
    mvOsSPrintf,
    mvOsIoUncachedMalloc,
    mvOsIoUncachedFree,
    mvOsMalloc,
    mvOsFree,
    memset,
    memcpy,
    mvOsCacheFlush,
    mvOsCacheInvalidate,
    mvOsIoVirtToPhy,
    NULL,
    NULL,
    mvUsbGetCapRegAddr,
    NULL
};


static void    print_args(int argc, char** argv)
{
    int     i;

    for(i=0; i<argc; i++)
    {
        mvOsPrintf("argv[%d]=%s, ", i, argv[i]);
    }
    mvOsPrintf("\n");
}

static void     mvUsbDevsInit(void)
{
    int dev;

    for(dev=0; dev<mvCtrlUsbMaxGet(); dev++)
    {
        mvUsbDevice[dev].handle = NULL;
        mvUsbDevice[dev].gppNo = -1;
        mvUsbDevice[dev].isMouse = MV_FALSE;
        mvUsbInit(dev, MV_FALSE);

#ifdef MV_USB_VOLTAGE_FIX
        mvUsbDevice[dev].gppNo = mvUsbGppInit(dev);
#endif /* MV_USB_VOLTAGE_FIX */
    }
    /* First of all. */
    _usb_device_set_bsp_funcs(&usbImportFuncs);
}

static void     mvUsbPolling(void)
{
    int dev, counter = 0;

    mvOsPrintf("Start USB ISR polling:");                
    for(dev=0; dev<mvCtrlUsbMaxGet(); dev++)
    {
        mvOsPrintf(" dev-%d - handle=%p\n", dev, mvUsbDevice[dev].handle);
    }
    mvOsPrintf("\n");
        
    while (TRUE) 
    {
        counter++;
        for(dev=0; dev<mvCtrlUsbMaxGet(); dev++)
        {
            if(mvUsbDevice[dev].handle == NULL)
                continue;

#ifdef MV_USB_VOLTAGE_FIX
            /* Polling VBUS */
            if(mvUsbDevice[dev].gppNo != -1)
                mvUsbBackVoltageUpdate(dev, mvUsbDevice[dev].gppNo);
#endif /* MV_USB_VOLTAGE_FIX */

            /* Polling USB interrupts */
            _usb_dci_vusb20_isr(mvUsbDevice[dev].handle);

            if(mvUsbDevice[dev].isMouse)
            {
                if (counter >= 1000000) 
                { /* Body */
                    usbMousePeriodicResume(mvUsbDevice[dev].handle);
                }
            } /* Endbody */
        }
        if( (counter & 0xFFFFF) == 0)
            mvOsPrintf(".");

	    if (ctrlc()) 
        {
		    puts ("\nAbort\n");
			return;
		}
    } /* Endwhile */
}

static void mvUsbUnload(int dev)
{
    if(mvUsbDevice[dev].handle == NULL)
    {
        mvOsPrintf("USB-%d Device is not loaded\n", dev);
        return;
    }

    mvOsPrintf("Unload USB=%d Device: handle=%p\n", 
                dev, mvUsbDevice[dev].handle);
    if(mvUsbDevice[dev].isMouse) 
    {
        usbMouseUnload(mvUsbDevice[dev].handle);
    }
    else
    {
        usbDiskUnload(mvUsbDevice[dev].handle);
    }
    mvUsbDevice[dev].handle = NULL;
    mvUsbDevice[dev].gppNo = -1;
    mvUsbDevice[dev].isMouse = MV_FALSE;
}


static void mvUsbDisk(int dev, int diskSize)
{
    if(mvUsbDevice[dev].handle != NULL) 
    {
        mvOsPrintf("USB-%d device already in use\n", dev);
        return;
    }

    if(diskSize == 0)
        diskSize = USB_DISK_DEFAULT_SIZE;

    mvOsPrintf("Load USB-%d Disk example: disk size is %d Kbytes\n", 
                dev, diskSize);

	mvUsbDevice[dev].handle = usbDiskLoad(dev, diskSize);
    if(mvUsbDevice[dev].handle == NULL)
    {
        mvOsPrintf("usbDiskLoad-%d FAILED\n", dev);
        return;
    }
    mvUsbPolling();
}

static void mvUsbMouse(int dev)
{
    if(mvUsbDevice[dev].handle != NULL)
    {
        mvOsPrintf("USB-%d device is already in use\n", dev);
        return;
    }

    mvOsPrintf("Load USB-%d Mouse example\n", dev);

    mvUsbDevice[dev].handle = usbMouseLoad(dev);
    if(mvUsbDevice[dev].handle == NULL)
    {
        mvOsPrintf("usbMouseLoad-%d FAILED\n", dev);
        return;
    }
    mvUsbDevice[dev].isMouse = MV_TRUE;
    mvUsbPolling();
}

int mvUsbCmdMain(int argc, char *argv[])
{
    _usb_device_handle          handle = NULL;
    int                         i, mode, dev;

    if(mvUsbIsFirst == MV_TRUE)
    {
        mvUsbDevsInit();
        mvUsbIsFirst = MV_FALSE;
    }
    if(argc <= 1)
    {
        mvOsPrintf("usbTest: not enough parameters\n");
        return 2;
    }
    print_args(argc, argv);

    if(memcmp(argv[1], "flags", strlen(argv[1])) == 0)
    {
        mvOsPrintf("USB old debug flags = 0x%x\n", _usb_debug_get_flags());
        if(argc > 2)
        {
            mode = strtol(argv[2], NULL, 16);
            _usb_debug_set_flags(mode);
            mvOsPrintf("USB new debug flags = 0x%x\n", _usb_debug_get_flags());
        }
        return 0;
    }
    if(memcmp(argv[1], "poll", strlen(argv[1])) == 0)
    {
        mvUsbPolling();
        return 0;
    }

    dev = 0;
    if(argc > 2)
        dev = strtol(argv[2], NULL, 10);

    handle = mvUsbDevice[dev].handle;
    if(handle == NULL)
    {
        mvOsPrintf("USB Device core is not initialized\n");
        return 3;
    }

    if(memcmp(argv[1], "show", strlen(argv[1])) == 0)
    {
        mode = 0;
        if(argc > 3)
            mode = strtol(argv[3], NULL, 16);

        if( MV_BIT_CHECK(mode, 0) )
            _usb_regs(handle);

        if( MV_BIT_CHECK(mode, 1) )
            _usb_status(handle);

        if( MV_BIT_CHECK(mode, 2) )
            _usb_stats(handle);

        if( MV_BIT_CHECK(mode, 3) )
            _usb_debug_print_trace_log();

        for(i=0; i<_usb_device_get_max_endpoint(handle); i++)
        {
            if( MV_BIT_CHECK(mode, (8+i)) )
            {
                _usb_ep_status(handle, i, ARC_USB_RECV);
                _usb_ep_status(handle, i, ARC_USB_SEND);
            }
        }
        return 0;
    }

    if(memcmp(argv[1], "test", strlen(argv[1])) == 0)
    {
        /* Set Test mode */
        mode = 0;
        if(argc > 3)
            mode = strtol(argv[3], NULL, 10);

        mvOsPrintf("USB test: set test mode %d\n", mode);
        _usb_dci_vusb20_set_test_mode(handle, (mode & 0x7) << 8);
        
        return 0;
    }

    mvOsPrintf("USB test: Unexpected command %s\n", argv[1]);
    return 3;
}

int mvUsbDevMain(int argc, char *argv[])
{
    int                 dev, diskSize = 0;

    if(mvUsbIsFirst == MV_TRUE)
    {
        mvUsbDevsInit();
        mvUsbIsFirst = MV_FALSE;
    }

    dev = strtol(argv[1], NULL, 10);

    if(memcmp(argv[2], "disk", strlen(argv[2])) == 0)
    {
        if(argc > 3)
            diskSize = strtol(argv[3], NULL, 10);

        mvUsbDisk(dev, diskSize);
        return 0;
    }

    if(memcmp(argv[2], "mouse", strlen(argv[2])) == 0)
    {
        mvUsbMouse(dev);
        return 0;
    }

    if( memcmp(argv[2], "unload", strlen(argv[2])) == 0)
    {
        mvUsbUnload(dev);
        return 0;
    }

    mvOsPrintf("usbDev unknown command\n");
    return 1;
}

int mvUsbCmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    if( (argc == 0) || (argc == 1) )
    {
        mvOsPrintf ("Usage:\n%s\n", cmdtp->usage); 
        return 1;
    }
    return mvUsbCmdMain(argc, argv);
}

int mvUsbDev(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    if(argc < 2)
    {
        mvOsPrintf ("Usage:\n%s\n", cmdtp->usage); 
        return 1;
    }
    return mvUsbDevMain(argc, argv);
}


U_BOOT_CMD(
	usbCmd,	4,	1,	mvUsbCmd,
	"usbCmd  - USB Device sub-system\n",
    "\n"
    "\t usbCmd flags [mask]             - Get/Set debug flags\n"
	"\t usbCmd poll                     - Polling IRQs for all USB devices for never\n"
    "\t usbCmd show <devId> [mask]      - Print out USB device information\n"
    "\t usbCmd test <devId> [mode]      - Set USB device to test mode\n"
);

U_BOOT_CMD(
	usbDev,	5,	0,	mvUsbDev,
	"usbDev  - Load/Unload USB Devices\n",
	"\t usbDev <devId> disk [kBytes] - Load USB Disk device\n"
	"\t usbDev <devId> mouse         - Load USB Mouse device\n"
	"\t usbDev <devId> unload        - Unload USB device\n"
);
