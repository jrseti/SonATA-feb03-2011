/*******************************************************************************

 File:    queue.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/queue.cc,v $
**
** Queue functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/queue.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: queue.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:55  kes
** *** empty log message ***
**
 * Revision 1.11  98/10/29  11:43:00  kes
 * NSS version
 * 
 * Revision 1.10  96/11/10  14:06:49  kes
 * Changes to convert from FUDD to TC.
 * 
 * Revision 1.9  96/05/15  18:31:26  kes
 * Added function headers, overloaded QSend and QReceive with functions
 * that do not take a timeout argument; remove QMsgs, QSpace.
 * 
 * Revision 1.8  96/01/31  11:57:20  kes
 * Made changes for C++, changed header files.
 * 
 * Revision 1.7  96/01/26  10:46:47  kes
 * Added test code for SysLogQ errors, changed QReceive to return an
 * error code if message pointer is NULL.
 * 
 * Revision 1.6  96/01/09  13:40:33  kes
 * Added include of TSSheader.h, fuddmsg.h, modified QFlush to release
 * packets which are flushed.
 * 
 * Revision 1.5  95/03/09  13:31:32  kes
 * Change error returns.
 * 
 * Revision 1.4  95/02/04  10:06:41  kes
 * Added QFlush.
 * 
 * Revision 1.3  94/09/12  13:08:31  kes
 * Fixed bug in QCreate which was allocating multiple queue structures;
 * changed mutex handling from pthread_mutex to csem; added code to
 * increment sendct and recvct.
 * 
 * Revision 1.2  94/03/23  18:40:07  alan
 * Originally created by Ken Smolek, Maxima Software.
 * Updated and modified by Alan Patrick, SETI Institute
 * 
 * Revision 1.1  94/03/23  18:40:00  alan
 * Initial revision
 * 
**********************************************************************/

/* set this to 1 to allow checking of SysLogQ message length (debug) */
#define TEST_SYSLOGQ		0

#include <stdlib.h>
#include "nss.h"
#include "nssmsg.h"
#include "queue.h"
#include "syslog.h"

error_t QInit(queue_t *, const char *, int);

/*
** QCreate: create a queue
**
** Synopsis:
**		q = QCreate(qname, qslots);
**		queue_t *q;				ptr to queue
**		char *qname;			name of the queue
**		int qslots;				# of message slots in the queue
** Returns:
**		Ptr to queue structure if successful.
** Description:
**		Creates a queue, allocating a buffer for queue messages.
** Notes:
**		None.
*/
queue_t *
QCreate(const char *qname, int qslots)
{
	queue_t *q = (queue_t *) calloc(1, sizeof(queue_t));

	if (q) {
		if (QInit(q, qname, qslots)) {
			free(q);
			q = NULL;
		}
	}
	return (q);
}		

/*
** QInit: initialize a queue
**
** Synopsis:
**		q = QInit(qname, qslots);
**		queue_t *q;				ptr to queue
**		char *qname;			name of the queue
**		int qslots;				# of message slots in the queue
** Returns:
**		Ptr to queue structure if successful.
** Description:
**		Creates and initializes a queue, allocating a buffer for
**		queue messages.
** Notes:
**		None.
*/
error_t
QInit(queue_t *q, const char *qname, int qslots)
{
	if (!(q->sSem = csem_create_val(qslots)))
		return (errno);
	else q->slots = qslots;

	if (!(q->rSem = csem_create_val(0)))
		return (errno);
	if (!(q->mutex = csem_create()))
		return (errno);
	q->data = new void *[qslots];
	if (!q->data)
		return (NSE_NEM);
	q->sIndex = 0;
	q->rIndex = 0;
	strcpy(q->name, qname);
	return (0);
}

#ifdef notdef
/*
** QDelete: delete a queue
**
** Synopsis:
**		err = QDelete(q);
**		error_t err;			return value
**		queue_t *q;				ptr to queue
** Returns:
**		0 if successful.
**		error code if the queue could not be deleted.
** Description:
**		Deletes the specified queue, after deleting the semaphores
**		associated with the queue.
** Notes:
**		The queue will not be deleted if there are any messages
**		on it or if there are any tasks waiting for messages.
*/
error_t
QDelete(queue_t *q)
{
	FUNCTION("QDelete");
	error_t err;

	if (!q)
		SysFatal(NSE_QNE, function, "");

	/* need exclusive control of the queue */	
	if (csem_wait(q->mutex, NULL))
		SysFatal(errno, function, "queue %s, mutex = 0x%x", QName(q), q->mutex);
	if (err = csem_delete(q->sSem))
		return (SysLog(err, function, "ssem, queue %s", q->qname));
	if (err = csem_delete(q->rSem))
		return (SysLog(err, function, "rsem, queue %s", q->qname));

#endif
/*
** QSend: send a message to a queue
**
** Synopsis:
**		err = QSend(q, msg, timeout);
**		error_t err;			return value
**		queue_t *q;				ptr to queue
**		void *msg;				message
**		timeval_t timeout;		timeout
** Returns:
**		0 if successful.
**		error code if the message could not be sent.
** Description:
**		Sends a message to the specified queue.
** Notes:
**		An attempt to send a NULL pointer is a fatal error.
**		An attempt to send to a NULL q is a fatal error.
**		If a csem_wait fails for any reason other than timeout,
**		it is a fatal error.
*/
error_t
QSend(queue_t *q, void *msg, timeval_t *timeout)
{
	FUNCTION("QSend");
	nssmsg_t *p;

#if (TEST_SYSLOGQ)
	extern queue_t *SysLogQ;

	if (q == SysLogQ) {
		p = (nssmsg_t *) msg;
		if (p->header.length != sizeof(tss_error_t))
			return (0);
	}
#endif
	if (!q)
		SysFatal(NSE_QNE, function, "");
	if (csem_wait(q->sSem, timeout)) {
		if (errno == EINTR)
			return (errno);
		SysFatal(errno, function, "queue %s, sSem = 0x%x", QName(q), q->sSem);
	}
	if (csem_wait(q->mutex, NULL))
		SysFatal(errno, function, "queue %s, mutex = 0x%x", QName(q), q->mutex);
	q->data[q->sIndex++] = msg;
	q->sIndex %= q->slots;
	csem_signal(q->mutex);
	csem_signal(q->rSem);
	q->sendct++;
	return (0);
}

/*
** QSend: send a message to a queue
**
** Synopsis:
**		void QSend(q, msg);
**		queue_t *q;				ptr to queue
**		void *msg;				message
** Returns:
**		Nothing.
** Description:
**		Sends a message to the specified queue.
** Notes:
**		There is no timeout, so any error is fatal.
*/
void
QSend(queue_t *q, void *msg)
{
	FUNCTION("QSend");
	nssmsg_t *p;

#if (TEST_SYSLOGQ)
	extern queue_t *SysLogQ;

	if (q == SysLogQ) {
		p = (nssmsg_t *) msg;
		if (p->header.length != sizeof(tss_error_t))
			return (0);
	}
#endif
	if (!q)
		SysFatal(NSE_QNE, function, "");
	if (csem_wait(q->sSem, NULL))
		SysFatal(errno, function, "queue %s, sSem = 0x%x", QName(q), q->sSem);
	if (csem_wait(q->mutex, NULL))
		SysFatal(errno, function, "queue %s, mutex = 0x%x", QName(q), q->mutex);
	q->data[q->sIndex++] = msg;
	q->sIndex %= q->slots;
	csem_signal(q->mutex);
	csem_signal(q->rSem);
	q->sendct++;
}

/*
** QReceive: receive a message from a queue
**
** Synopsis:
**		p = QReceive(q, err, timeout);
**		void *p;				ptr to message
**		queue_t *q;				ptr to queue
**		error_t *err;			ptr to storage for error
**		timeval_t timeout;		time to wait
** Returns:
**		Ptr to message if successful.
**		Null, err set if timeout.
** Description:
**		Returns the next message on the queue
** Notes:
**		If there is an error other than a timeout, a fatal error
**		has occurred.
**		A NULL message on the queue is a fatal error.
*/
void *
QReceive(register queue_t *q, error_t *err, timeval_t *timeout)
{
	FUNCTION("QReceive");
	void *msg = NULL;

	*err = 0;
	if (!q)
		SysFatal(NSE_QNE, function, "");
	if (csem_wait(q->rSem, timeout)) {
		if (errno != EINTR) {
			SysFatal(errno, function, "queue %s, rSem = 0x%x", QName(q),
					q->rSem);
		}
		*err = errno;
	}
	else {
		if (csem_wait(q->mutex, NULL)) {
			SysFatal(errno, function, "queue %s, mutex 0x%x", QName(q),
					q->mutex);
		}
		if (!(msg = q->data[q->rIndex++]))
			SysFatal(NSE_NMP, function, "queue %s", QName(q));
		q->rIndex %= q->slots;
		csem_signal(q->mutex);
		csem_signal(q->sSem);
		q->recvct++;
	}
	return (msg);
}

/*
** QReceive: receive a message from a queue
**
** Synopsis:
**		p = QReceive(q);
**		void *p;				ptr to message
**		queue_t *q;				ptr to queue
** Returns:
**		Ptr to message if successful.
**		Null, err set if timeout.
** Description:
**		Returns the next message on the queue
** Notes:
**		This is the simplified version of QReceive above; there is
**		no timeout, so any error is fatal.
*/
void *
QReceive(register queue_t *q)
{
	FUNCTION("QReceive");
	void *msg = NULL;

	if (!q)
		SysFatal(NSE_QNE, function, "");
	if (csem_wait(q->rSem, NULL))
		SysFatal(errno, function, "queue %s, rSem = 0x%x", QName(q), q->rSem);
	if (csem_wait(q->mutex, NULL))
		SysFatal(errno, function, "queue %s, mutex 0x%x", QName(q), q->mutex);
	if (!(msg = q->data[q->rIndex++]))
		SysFatal(NSE_NMP, function, "queue %s", QName(q));
	q->rIndex %= q->slots;
	csem_signal(q->mutex);
	csem_signal(q->sSem);
	q->recvct++;
	return (msg);
}

/*
** QFlush: flush any messages on a queue
**
** Synopsis:
**		void QFlush(q);
**		queue_t *q;				ptr to queue
** Returns:
**		Ptr to queue structure if successful.
** Description:
**		Creates a queue, allocating a buffer for queue messages.
** Notes:
**		None.
*/
void
QFlush(queue_t *q)
{
	error_t err;
	static timeval_t timeout;
	nssmsg_t *p;

	do {
		if (p = (nssmsg_t *) QReceive(q, &err, &timeout))
			FreeMsg(p);
	} while (!err);
}

/*
** QName: return the name of the specified queue
**
** Synopsis:
**		qname = QName(q);
**		const char *qname;		ptr to name of queue
**		queue_t *q;				ptr to queue
** Returns:
**		ptr to name of queue.
** Description:
**		Creates a queue, allocating a buffer for queue messages.
** Notes:
**		None.
*/
const char *
QName(queue_t *q)
{
	FUNCTION("QName");

	if (!q)
		SysFatal(NSE_QNE, function, NULL);
	return (q->name);
}
