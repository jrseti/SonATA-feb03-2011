/*******************************************************************************

 File:    fuddmsg.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/fuddmsg.cc,v $
**
** Fudd message allocation/deallocation
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/fuddmsg.cc,v 1.1.1.1 2006/08/02 05:30:26 kes Exp $
** $Log: fuddmsg.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:26  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:54  kes
** *** empty log message ***
**
 * Revision 1.10  96/11/10  14:04:04  kes
 * Changed error codes.
 * 
 * Revision 1.9  96/05/15  18:30:06  kes
 * Addition of function headers, code to keep track of allocations and
 * frees, maintain list of allocated messages.
 * 
 * Revision 1.8  96/02/09  09:09:28  kes
 * Removed unused code.
 * 
 * Revision 1.7  96/01/31  11:56:27  kes
 * Removed checksum functions, made changes for C++.
 * 
 * Revision 1.6  96/01/26  10:42:39  kes
 * Minor changes for task id logging.
 * 
 * Revision 1.5  96/01/09  13:39:01  kes
 * Added functions to help trace lost messages, using TEST_ALLOC_MSG
 * to control activation at compile time.
 * 
 * Revision 1.4  95/03/09  13:30:51  kes
 * Added debugging code, return error codes.
 * 
 * Revision 1.2  95/02/04  10:05:42  kes
 * Added header files, DupMsg.
 * 
 * Revision 1.1  94/09/12  13:19:10  kes
 * Initial revision
 * 
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "tc.h"
#include "fuddmsg.h"
#include "lock.h"
#include "syslog.h"
#include "util.h"

flock_t FuddPktLock;
fudd_packet_t *FuddPktHead;

#if (TEST_ALLOC_MSG)
/* allocated message list */
flock_t AllocPktLock;
fudd_packet_t *AllocPktHead;

void AddAllocMsg(fudd_packet_t *);
int RemoveAllocMsg(fudd_packet_t *);
#endif

int32_t MsgAllocs = 0;
int32_t MsgFrees = 0;

/*
** AllocMsgInit: allocate fudd_packet_t free list
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
	register fudd_packet_t *msg;

	if (FuddPktHead)
		return (TE_AAM);

	if (CreateLock(&FuddPktLock) < 0)
		return (TE_LCE);

#if (TEST_ALLOC_MSG)
	if (CreateLock(&AllocPktLock) < 0)
		return (TE_LCE);
	AllocPktHead = 0;
#endif
	
	/* allocate and link the buffers */
	msg = new fudd_packet_t[msgs];
	if (msg) {
		for (i = 0; i < msgs - 1; i++)
			msg[i].next = &msg[i+1];
		msg[i].next = NULL;
		FuddPktHead = msg;
		return (0);
	}
	else
		return (TE_NEM);
}

/*
** AllocMsg: allocate a message packet
**
** Synopsis:
**		p = AllocMsg(datalen);
**		fudd_packet_t *p;	ptr to message packet
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
fudd_packet_t *
AllocMsg(uint32_t datalen)
{
	FUNCTION("AllocMsg");
	void *data = NULL;
	register fudd_packet_t *p;
#if (RECORD_MSG_TID)
	tid_t tid;
#endif

	Lock(&FuddPktLock);
	if (p = FuddPktHead) {
		FuddPktHead = p->next;
		bzero((char *) p, sizeof(fudd_packet_t));
		Unlock(&FuddPktLock);
		if (datalen && !(data = (void *) malloc(datalen))) {
			FreeMsg(p);
			SysFatal(TE_CAM, function, "allocating data area, size = %d",
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
	Unlock(&FuddPktLock);
	SysFatal(TE_CAM, function, "allocating message packet");
}

#ifdef notdef
/*
** DupMsgHdr: duplicate a message header
**
** Synopsis:
**		q = DupMsgHdr(datalen);
**		fudd_packet_t *p;	ptr to message packet
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
fudd_packet_t *
DupMsgHdr(register fudd_packet_t *p, uint32_t datalen)
{
	register fudd_packet_t *p2;

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
**		fudd_packet_t *q;	ptr to duplicated message
**		fudd_packet_t *p;	ptr to original message
** Returns:
**		ptr to duplicated packet
** Description:
**		Allocates a new message and copies both the header and data from
**		the original message.
** Notes:
**		Fatal error if the packet cannot be allocated.
*/
fudd_packet_t *
DupMsg(register fudd_packet_t *p)
{
	register fudd_packet_t *q;

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
**		fudd_packet_t *p;	ptr to message packet
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
AllocMsgData(register fudd_packet_t *p, uint32_t datalen)
{
	FUNCTION("AllocMsgData");

	if (p->header.length && p->data)
		free(p->data);
	if (!(p->data = (void *) malloc(datalen)))
		SysFatal(TE_CAM, function, "allocating data area, size = %d", datalen);
	bzero((char *) p->data, datalen);
	p->header.length = datalen;
}

/*
** FreeMsg: allocate a message packet
**
** Synopsis:
**		void FreeMsg(p);
**		fudd_packet_t *p;	ptr to message packet
** Returns:
**		Nothing.
** Description:
**		Frees the message header and data areas.
** Notes:
**		If there is a data area, frees it with free(); the message
**		header is placed back on the message free list.
*/
void
FreeMsg(register fudd_packet_t *p)
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
	Lock(&FuddPktLock);
	if (p) {
		if (!p->next) {
			p->next = FuddPktHead;
			FuddPktHead = p;
			MsgFrees++;
		}
		else {
			SysLog(TE_FIP, WARNING, function,
					"p = 0x%x, next = 0x%x, size = %d, alloc = %d, free = %d",
					p, p->next, p->header.length, MsgAllocs, MsgFrees);
		}
	}
	else {
		SysLog(TE_FNP, WARNING, function, "alloc = %d, free = %d",
				MsgAllocs, MsgFrees);
	}
	Unlock(&FuddPktLock);
}

#if (TEST_ALLOC_MSG)
void
AddAllocMsg(register fudd_packet_t *p)
{
	Lock(&AllocPktLock);
	p->next = AllocPktHead;
	AllocPktHead = p;
	Unlock(&AllocPktLock);
}

int
RemoveAllocMsg(register fudd_packet_t *q)
{
	int rval = 0, i, j;
	register fudd_packet_t *p, *last, *p1;

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
	fudd_packet_t *p;

	for (i = 0, p = FuddPktHead; p; i++, p = p->next)
		;
	return (i);
}
#endif