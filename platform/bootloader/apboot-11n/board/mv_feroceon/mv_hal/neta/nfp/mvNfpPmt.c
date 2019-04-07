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

#include "mvCommon.h"  /* Should be included before mvSysHwConfig */
#include "mvTypes.h"
#include "mvDebug.h"
#include "mvOs.h"

#include "gbe/mvNeta.h"
#include "pmt/mvPmt.h"
#include "nfp/mvNfp.h"

MV_BOOL		mvNfpPmtFlows[MV_ETH_MAX_PORTS][CONFIG_MV_ETH_PMT_FLOWS];

/*
 * PMT topology definition.
 * The PMT is divided into sections.
 * First section: is one entry per flow (PNC entry) - only jump
 * Second section: 7 entries for each flow
 * Third section: subrotines (e.g. MAC header replace, PPPoE header add/remove, etc)
 */
enum {
    PMT_JUMP_FIRST = 0,
    PMT_JUMP_LAST = PMT_JUMP_FIRST + CONFIG_MV_ETH_PMT_FLOWS - 1,
    PMT_FLOW_FIRST,
    PMT_FLOW_LAST = PMT_FLOW_FIRST + (CONFIG_MV_ETH_PMT_CMD_PER_FLOW) - 1,
    PMT_SUB_FIRST,
    PMT_SUB_LAST = MV_ETH_PMT_SIZE - 1,
};


MV_STATUS	mvNfpPmtInit(void)
{
	int i, port;

	mvOsPrintf("NFP PMT Topology: Total=%d, Flows=%d, CmdsPerFlow=%d\n",
				MV_ETH_PMT_SIZE, CONFIG_MV_ETH_PMT_FLOWS, CONFIG_MV_ETH_PMT_CMD_PER_FLOW);
	mvOsPrintf("PMT Jumps section: from %d to %d\n", PMT_JUMP_FIRST, PMT_JUMP_LAST);
	mvOsPrintf("PMT Flows section: from %d to %d\n", PMT_FLOW_FIRST, PMT_FLOW_LAST);
	mvOsPrintf("PMT Subrs section: from %d to %d\n", PMT_SUB_FIRST, PMT_SUB_LAST);

	if (PMT_SUB_FIRST > PMT_SUB_LAST) {
		mvOsPrintf("Wrong PMT topology: PMT_SUB_FIRST (%d) > PMT_SUB_LAST (%d)\n",
					PMT_SUB_FIRST, PMT_SUB_LAST);
		return MV_FAIL;
	}

	for (port = 0; port < MV_ETH_MAX_PORTS; port++) {
		for (i = 0; i < CONFIG_MV_ETH_PMT_FLOWS; i++)
			mvNfpPmtFlows[port][i] = MV_FALSE;
	}
	return MV_OK;
}


/* Get PMT flow match for number of PMT commands */
int			mvNfpPmtAlloc(int port, int number)
{
	int	i, j, entries = 0, first = -1;

	for (i = 0; i < CONFIG_MV_ETH_PMT_FLOWS; i++) {
		if (entries >= number)
			break;

		if (mvNfpPmtFlows[port][i] == MV_FALSE) {
			if (first == -1)
				first = i;

			mvNfpPmtFlows[port][i] = MV_TRUE;
			entries += CONFIG_MV_ETH_PMT_CMD_PER_FLOW;
		} else {
			if (first != -1) {
				/* Rollback */
				first = -1;
				entries = 0;
				for (j = first; j < i; j++)
					mvNfpPmtFlows[port][j] = MV_FALSE;

			}
		}
	}
	return first;
}

void	mvNfpPmtFree(int port, int idx, int number)
{
	while (number > 0) {
		if (mvNfpPmtFlows[port][idx] == MV_FALSE)
			mvOsPrintf("%s WARNING: port=%d, flow=%d is free\n", __func__, port, idx);

		mvNfpPmtFlows[port][idx] = MV_FALSE;
		number -= CONFIG_MV_ETH_PMT_CMD_PER_FLOW;
		idx++;
	}
}

MV_STATUS   mvNfpPmtFibUpdate(int idx, NFP_RULE_FIB *pFib)
{
    MV_NETA_PMT     pmtEntry;
    int             first, cmd, offset;

    first = PMT_FLOW_FIRST + idx * CONFIG_MV_ETH_PMT_CMD_PER_FLOW;

    /* replace MAC header */
    offset = 0;
    cmd = mvNetaPmtDataReplace(pFib->outport, first, offset, pFib->da,
				sizeof(pFib->da)+sizeof(pFib->sa), MV_FALSE);

    /* decrement TTL + replace IP CSUM */
    offset = 2; /* TBD - calculate start of IP header using HW */
    cmd = mvNetaPmtTtlDec(pFib->outport, cmd, offset, MV_TRUE);

    /* Set jump command with skip of MH */
    MV_NETA_PMT_CLEAR(&pmtEntry);
    mvNetaPmtJump(&pmtEntry, first, 0, 0);
    mvNetaPmtWrite(pFib->outport, idx, &pmtEntry);

    return MV_OK;
}

