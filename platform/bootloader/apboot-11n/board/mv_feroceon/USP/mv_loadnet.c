/*
 * (C) Copyright 2001
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
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
 
/* #define DEBUG */
#include <common.h>
#include <command.h>
#include <net.h>
#include "mvSysHwConfig.h"
#if defined(MV_INCLUDE_MONT_EXT) && defined (MV_INCLUDE_MONT_LOAD_NET)

#if defined(CONFIG_CMD_BSP)

#include "mvTypes.h"
#include "idma/mvIdma.h"



extern void d_i_cache_clean(unsigned int Address, unsigned int byteCount);
extern unsigned int VIRTUAL_TO_PHY(unsigned int address); 

/* defines relevant for the protocol between the loadnet and the fileloader.*/
#define ENTRY_ADDR_OFFSET 0x2c - ETHER_HDR_SIZE - IP_HDR_SIZE
#define PKT_HEADER_SIZE 0x38 - ETHER_HDR_SIZE - IP_HDR_SIZE
#define LAST_PACKET_FLAG         0xffffffff

#define NL_HASHES_PER_LINE 65

/* indication for loadnet reached EOF */
static MV_BOOL loadnet_done;
/* the real load address */
static MV_U32 loadnet_addr_gl;
/* the files Size that was receied by loadnet. */
static MV_U32 fileSize;
/* in case we received an srec file, the srec program Entry. */
static MV_U32 programAdressEntry;
/* for print banner */
static MV_U32 NetloadBlock;

/* This function will be called from the NetReceive (net.c) whenever we will receive
 * a UDP packet with our IP address. 
 * This function know how to process the packets received from the Marvell file 
 * Loader utility.
 * input - pointer to the packet buffer after stripping the IP header.
 *	   (other input aren't relevant for us in this function)
 * output - update the global parameters, see above.
 */
static void
mv_private_loadnet_handler(uchar * pkt, unsigned dest, unsigned src, unsigned len)
{

    MV_U32 entryAddress,byteCount,sourceAddress,destAddress;
    MV_U32 packetEnd;

    /* print progress.*/
    NetloadBlock++;
    putc ('#');
    if ((NetloadBlock % (NL_HASHES_PER_LINE)) == 0) {
                        puts ("\n");
    }

    /* in case the file Loader transfer Srec to Bin it will send us the 
       entry address of each packet data. */
    /* fixed for MIPS alignment was: entryAddress = (*(MV_U32 *)(pkt + ENTRY_ADDR_OFFSET)); */
    entryAddress = 0;
#ifdef CONFIG_MV_LE
	entryAddress |= *(unsigned char *)(pkt + ENTRY_ADDR_OFFSET + 0) << 24;
	entryAddress |= *(unsigned char *)(pkt + ENTRY_ADDR_OFFSET + 1) << 16;
	entryAddress |= *(unsigned char *)(pkt + ENTRY_ADDR_OFFSET + 2) << 8;
	entryAddress |= *(unsigned char *)(pkt + ENTRY_ADDR_OFFSET + 3) << 0; 
#else
    {
    int i;
    for(i = 0; i < 2; i++ ){
	entryAddress |= (*(unsigned short *)(pkt + ENTRY_ADDR_OFFSET + i*2)) << ((1-i)*16);}
    }
#endif
        /* last packet of a received file is marked. */
    if(entryAddress == LAST_PACKET_FLAG){
            loadnet_done = 1;
	    return;}
    /* the byte count of the packet data */
#ifdef CONFIG_MV_LE
    byteCount = 0;
    byteCount |= *(unsigned char *)(pkt + ENTRY_ADDR_OFFSET +6) << 8;
    byteCount |= *(unsigned char *)(pkt + ENTRY_ADDR_OFFSET +7);
#else
    byteCount = (*(short *)(pkt + ENTRY_ADDR_OFFSET +6)) ; 
#endif
    packetEnd = entryAddress + byteCount;

    /* should be set only in the first time */
    if(programAdressEntry == 0x0){
		programAdressEntry = entryAddress;
		debug("programAdressEntry = %x\n",programAdressEntry);}
    if(loadnet_addr_gl == 0x0){
		loadnet_addr_gl = entryAddress;
		debug("loadnet_addr_gl = %x\n",loadnet_addr_gl);}

    destAddress = (MV_U32)(entryAddress - programAdressEntry +
					loadnet_addr_gl);
    sourceAddress = (MV_U32)(pkt + PKT_HEADER_SIZE);

    debug(" Transfering from Source %x to Dest %x %x bytes\n",\
			sourceAddress,destAddress,byteCount);

    memcpy((MV_U32*)destAddress,(MV_U32*)sourceAddress,byteCount);

    fileSize = packetEnd - programAdressEntry;

    return;
}
/* load a file through the network interface, using UDP protocol only!!
 * ( this function is basically the client for Marvell fileLoader utility. )
 * this function doesn't transmit any thing, only receive!!
 * input - the address you want the file to be loaded to.
 *         if set to 0, the file will be loaded to the address given by the file
 *         loader according to the srec entry address.
 * output - the address that the file was loaded to.
 * return - the file size received from the file loader.
 */ 
int load_net(MV_U32 *loadnet_addr){

        DECLARE_GLOBAL_DATA_PTR;
        bd_t *bd = gd->bd;

	/* delay for MIPS LE stuck issue - probably unstable board. ??? */	
	printf("\n");
        eth_halt();
	/* initilize the ethernet port */
        if(eth_init(bd) < 0)
            return(-1);
	/* set the Network parameters */ 
        memcpy (NetOurEther, bd->bi_enetaddr, 6);
        NetCopyIP(&NetOurIP, &bd->bi_ip_addr);
        NetOurGatewayIP = getenv_IPaddr ("gatewayip");
        NetOurSubnetMask= getenv_IPaddr ("netmask");
        NetServerIP = getenv_IPaddr ("serverip");
   	/* set the Rx handler */
        NetSetHandler (mv_private_loadnet_handler);
	/* initialize global parameters. */
	programAdressEntry = 0;
	loadnet_done = 0;
	fileSize = 0;
	loadnet_addr_gl = *loadnet_addr;
	NetloadBlock = 0;

	printf("Load File - Send your file when ready...\n");
	/* main loop */
        for (;;) {
                /*      Check the ethernet for a new packet.  The ethernet
                 *      receive routine will process it.  */
                        eth_rx();
 
                 /*      Abort if ctrl-c was pressed. */
                if (ctrlc()) {
                        eth_halt();
                        printf("\nAbort\n");
                        return (-1);
                }
		/* if load net detected EOF*/
		if(loadnet_done == 1) break;
	}
	/* close the ethernet port */
 	eth_halt();
	*loadnet_addr = loadnet_addr_gl;
	return fileSize;
}

int
loadnet_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        int   	size;
	MV_U32   tmp = 0;	

	if(!enaMonExt()){
		printf("This command can be used only if enaMonExt is set!\n");
		return 0;}

    	size = load_net(&tmp);
    	printf("\nFile loaded successfully...\n");
	printf("Entry Address: 0x%x\n",tmp);
    	printf("File size: %d bytes.\n",size);
	return 1;

}
 

U_BOOT_CMD(
	ln,      1,     1,      loadnet_cmd,
	"ln	- Load S-Record executable file through the network interface. \n",
	"\t \n"
	"\tLoad S-Record executable file via thr first avilable ethernet port.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

#endif

#endif /* #if defined(MV_INCLUDE_MONT_EXT) */
