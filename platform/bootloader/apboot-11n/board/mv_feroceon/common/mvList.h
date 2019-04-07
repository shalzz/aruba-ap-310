/*******************************************************************************
*                   Copyright 2003, Marvell Semiconductor Israel LTD.          *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), MARVELL SEMICONDUCTOR ISRAEL LTD (MSIL).                             *
********************************************************************************
* mvStack.h - Header File for :
*
* FILENAME:    $Workfile: mvStack.h $
*
* DESCRIPTION:
*     This file defines simple Stack (LIFO) functionality.
*
*******************************************************************************/

#ifndef __mvList_h__
#define __mvList_h__

#include "mvCommon.h"
#include "mvOs.h"

typedef struct mv_list {
	struct mv_list *prev;
	struct mv_list *next;
	unsigned long data;

} MV_LIST;

static INLINE MV_LIST *mvListFind(MV_LIST *head, MV_ULONG data)
{
	return NULL;
}

/* Add new element before head */
static INLINE MV_LIST *mvListAddHead(MV_LIST **head, MV_ULONG data)
{
	MV_LIST *element;

	element = mvOsMalloc(sizeof(MV_LIST));
	if (element) {
		element->data = data;
		element->next = (*head);
		element->prev = NULL;
		if (*head)
			(*head)->prev = element;

		*head = element;
	}
	return element;
}

static INLINE MV_LIST *mvListDel(MV_LIST **list)
{
	MV_LIST *prev = (*list)->prev;
	MV_LIST *next = (*list)->next;

	mvOsFree(*list);

	if (prev)
		prev->next = next;
	else
		(*list) = next;

	if (next)
		next->prev = prev;

	return next;
}

#endif /* __mvList_h__ */
