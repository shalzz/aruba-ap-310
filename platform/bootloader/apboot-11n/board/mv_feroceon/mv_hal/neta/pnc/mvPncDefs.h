/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

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
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __PNC_HW_H__
#define __PNC_HW_H__

#ifdef CONFIG_MV_ETH_PNC

#include "mvOs.h"

#include "mvTcam.h"

/*
 * Support features
 */
#define DWORD_LEN       32

#define TAG_LEN  	2		/* Marvell tag len */
#define MAC_LEN		6		/* MAC address len */
#define LLC_LEN		6		/* LLC/SNAP header len */
#define VLAN_LEN	4		/* VLAN header len */
#define TYPE_LEN    2       /* ETH type len */
#define PPPOE_LEN   8       /* PPPoE header len */


#define TCP_PROTO	0x06	/* TCP */
#define UDP_PROTO	0x11	/* UDP */
#define IGMP_PROTO	0x02	/* IGMP */
#define ESP_PROTO	0x50	/* ESP */
#define ARP_ETYPE	0x0806

/*
 * TCAM topology definition.
 * The TCAM is divided into sections per protocol encapsulation. 
 * Usually each section is designed to be to a lookup.
 * Change sizes of sections according to the target product.
 */
enum {
	/* Marvell/PON header Lookup */
#ifdef CONFIG_MV_ETH_PNC_MH
	TE_MH,
	TE_MH_EOF = TE_MH + 2,
#endif /* CONFIG_MV_ETH_PNC_MH*/

	/* MAC Lookup */
	TE_MAC_BC,	    /* broadcast */
    TE_MAC_FLOW_CTRL, /* Flow Control PAUSE frames */
	TE_MAC_MC_ALL,    /* first multicast entry (always reserved for all MCASTs) */
	TE_MAC_MC_L = TE_MAC_MC_ALL + CONFIG_MV_ETH_PNC_MCAST_NUM,    /* last multicast entry */
	TE_MAC_ME,	    /* mac to me per port */
	TE_MAC_ME_END = TE_MAC_ME + CONFIG_MV_ETH_PORTS_NUM - 1,	
	TE_MAC_EOF,	

	/* SNAP/LLC Lookup */
#ifdef CONFIG_MV_ETH_PNC_SNAP
	TE_SNAP,
	TE_SNAP_END = TE_SNAP + 4 - 1,
#endif /* CONFIG_MV_ETH_PNC_SNAP */

	/* VLAN Lookup */
	TE_VLAN,
	TE_VLAN_END = TE_VLAN + CONFIG_MV_ETH_PNC_VLAN_PRIO - 1,
	TE_VLAN_EOF,

	/* Ethertype Lookup */
	TE_ETYPE_ARP,
	TE_ETYPE_IP4,
	TE_ETYPE_IP6,
	TE_ETYPE_PPPOE,
	TE_ETYPE,	/* custom ethertype */
	TE_ETYPE_EOF = TE_ETYPE + CONFIG_MV_ETH_PNC_ETYPE,

	/* IP4 Lookup */
#if (CONFIG_MV_ETH_PNC_DSCP_PRIO > 0)
    TE_IP4_DSCP,
	TE_IP4_DSCP_END = TE_IP4_DSCP + CONFIG_MV_ETH_PNC_DSCP_PRIO - 1,
#endif /* CONFIG_MV_ETH_PNC_DSCP_PRIO > 0 */

	TE_IP4_TCP,					
	TE_IP4_TCP_FRAG_F,
	TE_IP4_TCP_FRAG_1,
	TE_IP4_UDP,					
	TE_IP4_UDP_FRAG_F,
	TE_IP4_UDP_FRAG_1,
	TE_IP4_IGMP,
	TE_IP4_ESP,
	TE_IP4_EOF,

	/* IP6 Lookup */
	TE_IP6_TCP,
	TE_IP6_UDP,
	TE_IP6_EOF,

	/* Session Lookup */
    /* NFP session use all rest entries */
	TE_FLOW_NFP,
	TE_FLOW_NFP_END = MV_ETH_TCAM_LINES - 2,
	TE_FLOW_EOF,
};
/*
 * Pre-defined FlowId assigment
 */
#define FLOWID_EOF_LU_PORT 	0xFFF0
#define FLOWID_EOF_LU_MAC 	0xFFF1
#define FLOWID_EOF_LU_VLAN	0xFFF2
#define FLOWID_EOF_LU_ETHER	0xFFF3
#define FLOWID_EOF_LU_IP4	0xFFF4
#define FLOWID_EOF_LU_IP6	0xFFF5
#define FLOWID_EOF_LU_FLOW	0xFFF6


/*
 * Errors
 */
#define ERR_ON_OOR(cond) if (cond) { mvOsPrintf("%s: out of range\n", __FUNCTION__); return PNC_ERR_OOR;}
#define WARN_ON_OOR(cond) if (cond) { mvOsPrintf("%s: out of range\n", __FUNCTION__); return;}
#define WARN_ON_OOM(cond) if (cond) { mvOsPrintf("%s: out of memory\n", __FUNCTION__);return NULL;}

#endif /* CONFIG_MV_ETH_PNC */
#endif /* __PNC_HW_H__ */
