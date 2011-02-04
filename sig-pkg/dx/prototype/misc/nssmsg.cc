/*******************************************************************************

 File:    nssmsg.cc
 Project: OpenSonATA
 Authors: The OpenSonATA code is the result of many programmers
          over many years

 Copyright 2011 The SETI Institute

 OpenSonATA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 OpenSonATA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
 
 Implementers of this code are requested to include the caption
 "Licensed through SETI" with a link to setiQuest.org.
 
 For alternate licensing arrangements, please contact
 The SETI Institute at www.seti.org or setiquest.org. 

*******************************************************************************/

/**********************************************************************
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/nssmsg.cc,v $
**
** NSS message allocation/deallocation
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/nssmsg.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: nssmsg.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:55  kes
** *** empty log message ***
**
Revision 1.1  98/10/29  11:42:58  kes
NSS version

**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nss.h"
#include "nssmsg.h"
#include "lock.h"
#include "syslog.h"
#include "util.h"

flock_t NSSPktLock;
nssmsg_t *NSSPktHead;

#if (TEST_ALLOC_MSG)
/* allocated message list */
flock_t AllocPktLock;
nssmsg_t *AllocPktHead;

void AddAllocMsg(nssmsg_t *);
int RemoveAllocMsg(nssmsg_t *);
#endif

int32_t MsgAllocs = 0;
int32_t MsgFrees = 0;

/*
** AllocMsgInit: allocate nssmsg_t free list
**
** Synopsis:
**		err = AllocMsgInit(msgs);
**		error_t er;			return value
**		int msgs;			# of messages to allocate
** Returns:
**		0 if successful.
**		error code if messages cannot be allocated.
** Description:
**		Allocates a message free list containing the specified
**		number of messages.
** Notes:
**		The list is singly-linked.
*/
error_t
AllocMsgInit(int msgs)
{
	int i;
	register nssmsg_t *msg;

	if (NSSPktHead)
		return (NSE_AAM);

	if (CreateLock(&NSSPktLock) < 0)
		return (NSE_LCE);

#if (TEST_ALLOC_MSG)
	if (CreateLock(&AllocPktLock) < 0)
		return (NSE_LCE);
	AllocPktHead = 0;
#endif
	
	/* allocate and link the buffers */
	msg = new nssmsg_t[msgs];
	if (msg) {
		for (i = 0; i < msgs - 1; i++)
			msg[i].next = &msg[i+1];
		msg[i].next = NULL;
		NSSPktHead = msg;
		return (0);
	}
	else
		return (NSE_NEM);
}

/*
** AllocMsg: allocate a message packet
**
** Synopsis:
**		p = AllocMsg(datalen);
**		nssmsg_t *p;	ptr to message packet
**		uint32_t datalen;	size of data area
** Returns:
**		ptr to allocated packet
** Description:
**		Allocates and clears a message packet; if datalen is nonzero,
**		also allocates a data area, sets the length member to the data
**		size  and points the data pointer in the header to it.
** Notes:
**		To minimize execution time, the packet is allocated from a list
**		of free packets.  The data area is allocated using malloc.
**		If either allocation fails, it is a fatal error and the system
**		terminates operation; thus, this function can never return a
**		NULL pointer.
*/
nssmsg_t *
AllocMsg(uint32_t datalen)
{
	FUNCTION("AllocMsg");
	void *data = NULL;
	register nssmsg_t *p;
#if (RECORD_MSG_TID)
	tid_t tid;
#endif

	Lock(&NSSPktLock);
	if (p = NSSPktHead) {
		NSSPktHead = p->next;
		bzero((char *) p, sizeof(nssmsg_t));
		Unlock(&NSSPktLock);
		if (datalen && !(data = (void *) malloc(datalen))) {
			FreeMsg(p);
			SysFatal(NSE_CAM, function, "allocating data area, size = %d",
					datalen);
		}
		p->data = data;
		p->header.length = datalen;
#if (RECORD_MSG_TID)
		p->tid = GetTaskId();
#endif
#if (TEST_ALLOC_MSG)
		AddAllocMsg(p);
#endif
		MsgAllocs++;
		return (p);
	}
	Unlock(&NSSPktLock);
	SysFatal(NSE_CAM, function, "allocating message packet");
}

#ifdef notdef
/*
** DupMsgHdr: duplicate a message header
**
** Synopsis:
**		q = DupMsgHdr(datalen);
**		nssmsg_t *p;	ptr to message packet
**		uint32_t datalen;	size of data area
** Returns:
**		ptr to allocated packet
** Description:
**		Allocates and clears a message packet; if datalen is nonzero,
**		also allocates a data area, sets the length member to the data
**		size  and points the data pointer in the header to it.
** Notes:
**		To minimize execution time, the packet is allocated from a list
**		of free packets.  The data area is allocated using malloc.
**		If either allocation fails, it is a fatal error and the system
**		terminates operation; thus, this function can never return a
**		NULL pointer.
*/
nssmsg_t *
DupMsgHdr(register nssmsg_t *p, uint32_t datalen)
{
	register nssmsg_t *p2;

#if (TEST_ALLOC_MSG)
	tid_t tid;
	tid = GetTaskId();
#endif
	p2 = AllocMsg(p->header.length);
	p2->header = p->header;
#if (TEST_ALLOC_MSG)
	p->tid = tid;
#endif
	return (p2);
}
#endif
	
/*
** DupMsg: duplicate a message packet
**
** Synopsis:
**		q = AllocMsg(p);
**		nssmsg_t *q;	ptr to duplicated message
**		nssmsg_t *p;	ptr to original message
** Returns:
**		ptr to duplicated packet
** Description:
**		Allocates a new message and copies both the header and data from
**		the original message.
** Notes:
**		Fatal error if the packet cannot be allocated.
*/
nssmsg_t *
DupMsg(register nssmsg_t *p)
{
	register nssmsg_t *q;

	q = AllocMsg(p->header.length);
	q->header = p->header;
	if (p->header.length)
		bcopy((char *) p->data, (char *) q->data, q->header.length);
	return (q);
}

/*
** AllocMsgData: allocate a data area for an empty message
**
** Synopsis:
**		void AllocMsgData(p, datalen);
**		nssmsg_t *p;	ptr to message packet
**		uint32_t datalen;	size of data area
** Returns:
**		Nothing.
** Description:
**		Allocates a data area for the specified message; if the message
**		already has a data area, it is freed first.
** Notes:
**		Fatal error if the data area cannot be allocated.
*/
void
AllocMsgData(register nssmsg_t *p, uint32_t datalen)
{
	FUNCTION("AllocMsgData");

	if (p->header.length && p->data)
		free(p->data);
	if (!(p->data = (void *) malloc(datalen)))
		SysFatal(NSE_CAM, function, "allocating data area, size = %d", datalen);
	bzero((char *) p->data, datalen);
	p->header.length = datalen;
}

/*
** FreeMsg: allocate a message packet
**
** Synopsis:
**		void FreeMsg(p);
**		nssmsg_t *p;	ptr to message packet
** Returns:
**		Nothing.
** Description:
**		Frees the message header and data areas.
** Notes:
**		If there is a data area, frees it with free(); the message
**		header is placed back on the message free list.
*/
void
FreeMsg(register nssmsg_t *p)
{
	FUNCTION("FreeMsg");

#if (TEST_ALLOC_MSG)
	if (RemoveAllocMsg(p)) {
		printf("attempt to free unallocated msg, ptr = 0x%x, tid = 0x%x\n",
				p, p->tid);
	}
#endif
	if (p->data && p->header.length)
		free(p->data);
	Lock(&NSSPktLock);
	if (p) {
		if (!p->next) {
			p->next = NSSPktHead;
			NSSPktHead = p;
			MsgFrees++;
		}
		else {
			SysLog(NSE_FIP, WARNING, function,
					"p = 0x%x, next = 0x%x, size = %d, alloc = %d, free = %d",
					p, p->next, p->header.length, MsgAllocs, MsgFrees);
		}
	}
	else {
		SysLog(NSE_FNP, WARNING, function, "alloc = %d, free = %d",
				MsgAllocs, MsgFrees);
	}
	Unlock(&NSSPktLock);
}

#if (TEST_ALLOC_MSG)
void
AddAllocMsg(register nssmsg_t *p)
{
	Lock(&AllocPktLock);
	p->next = AllocPktHead;
	AllocPktHead = p;
	Unlock(&AllocPktLock);
}

int
RemoveAllocMsg(register nssmsg_t *q)
{
	int rval = 0, i, j;
	register nssmsg_t *p, *last, *p1;

	/* scan the allocated message list, looking for the packet */
	Lock(&AllocPktLock);
	for (j = 0, p = AllocPktHead; p && p != q; j++, last = p, p = p->next)
		;
	if (p) {
		/* we found it; remove it from the list */
		if (p == AllocPktHead)
			AllocPktHead = p->next;
		else
			last->next = p->next;
		for (i = 0, p1 = AllocPktHead; p1; i++, p1 = p1->next)
			;
		if (i != (MsgAllocs - MsgFrees - 1)) {
			p->next = 0;
			return (-1);
		}
		p->next = 0;
	}
	else
		rval = -1;
	Unlock(&AllocPktLock);
	return (rval);
}

int
CountMsgs(void)
{
	int i;
	nssmsg_t *p;

	for (i = 0, p = NSSPktHead; p; i++, p = p->next)
		;
	return (i);
}
#endif