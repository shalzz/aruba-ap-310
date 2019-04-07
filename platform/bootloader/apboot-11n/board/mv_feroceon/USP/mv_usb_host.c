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
#include "mvDebug.h"
#include "mvBoardEnvSpec.h"
#include "mvCtrlEnvLib.h"

#include "mvUsbHostApi.h"

extern USB_HOST_STATE_STRUCT_PTR usb_host_state_struct_ptr;
extern void _usb_hci_vusb20_isr(void);
extern void hidMouseMain(void);

static void    print_args(int argc, char** argv)
{
    int     i;

    for(i=0; i<argc; i++)
    {
        mvOsPrintf("argv[%d]=%s, ", i, argv[i]);
    }
    mvOsPrintf("\n");
}


int mvUsbHostMain(int argc, char *argv[])
{
    int                     i, mode;
    unsigned char           error;
    USB_STATUS              status;
    _usb_host_handle        host_handle;

    if(argc <= 1)
    {
        mvOsPrintf("usbTest: not enough parameters\n");
        return 2;
    }
    print_args(argc, argv);

	ARC_INIT_DEBUG_TRACE();

	ARC_START_DEBUG_TRACE(ARC_DEBUG_FLAG_STALL 
					  | ARC_DEBUG_FLAG_ERROR 
					  | ARC_DEBUG_FLAG_RESET 
                      | ARC_DEBUG_FLAG_ISR 
					  | ARC_DEBUG_FLAG_SETUP 
					  | ARC_DEBUG_FLAG_RX | ARC_DEBUG_FLAG_TX 
					  | ARC_DEBUG_FLAG_ADDR);

    if( (usb_host_state_struct_ptr == NULL) && 
        (memcmp(argv[1], "load", strlen(argv[1])) == 0) )
    {
		MV_REG_WRITE(MV_USB_CORE_MODE_REG, 0x3);
        status = _usb_host_init(0, 1024, &host_handle);             
        if (status != USB_OK) 
        {
            printf("\nUSB Host Initialization failed. STATUS: %x", status);
        } /* Endif */

        mvOsPrintf("USB HOST core is initialized\n");
        return 0;
    }

    if( (usb_host_state_struct_ptr != NULL) && 
        (memcmp(argv[1], "show", strlen(argv[1])) == 0))
    {
        mode = 0;
        if(argc > 2)
            mode = strtol(argv[2], NULL, 16);

        if( MV_BIT_CHECK(mode, 3) )
            ARC_PRINT_DEBUG_TRACE();

        return 0;
    }

    if( (usb_host_state_struct_ptr != NULL) && 
        (memcmp(argv[1], "poll", strlen(argv[1])) == 0))
    {
        i = 0;
        while(TRUE)
        {
            if( (i & 0xFFFFF) == 0)
            {
                mvOsPrintf(".");
            }
            _usb_hci_vusb20_isr();
            i++;

            if (ctrlc()) 
            {
                puts ("\nAbort\n");
                return 0;
            }
        }
        mvOsPrintf("\n");

        return 0;
    }

    if( (usb_host_state_struct_ptr == NULL) && 
        (memcmp(argv[1], "mouse", strlen(argv[1])) == 0))
    {
        mvOsPrintf("Start hidmouse driver\n");
		MV_REG_WRITE(MV_USB_CORE_MODE_REG, 0x3);
        hidMouseMain();
        return 0;
    }
    
    mvOsPrintf("USB test: Unexpected command %s\n", argv[1]);
    return 3;
}

int mvUsbHost(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    if( (argc == 0) || (argc == 1) )
    {
        mvOsPrintf ("Usage:\n%s\n", cmdtp->usage); 
        return 1;
    }
    mvUsbHostMain(argc, argv);
    return 1;
}

U_BOOT_CMD(
    usbHost,    5,  0,  mvUsbHost,
    "usbHost  - USB Host specific commands\n",
    "\t usbHost mouse    <1 | 0> - Load/Unload USB HID mouse driver\n"
    "\t usbHost show     [mask]  - Print out all USB information\n"
);
