/*******************************************************************************

 File:    queue.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/queue.h,v $
**
** Queue structure definition
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/queue.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: queue.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:35  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:37  kes
 * Initial revision
 * 
 * Revision 1.8  96/11/10  14:26:57  kes
 * Changes to convert from FUDD to TC.
 * 
 * Revision 1.7  96/05/15  18:11:41  kes
 * Overloaded QSend, QReceive with no timeout, removed QMsgs, QSpace,
 * made QFlush a void function.
 * 
 * Revision 1.6  96/01/31  12:24:57  kes
 * Removed ifdef _cplusplus.
 * 
 * Revision 1.5  96/01/26  15:52:00  kes
 * Moved some defines, eliminated error structure.
 * 
 * Revision 1.4  95/02/09  11:10:10  kes
 * Changed several prototypes.
 * 
 * Revision 1.3  95/02/04  07:36:59  kes
 * Added prototype for QFlush.
 * 
 * Revision 1.2  94/09/12  12:49:25  kes
 * Changed type of queue locking mutex, added send and receive counts,
 * made it C++ safe.
 * 
 * Revision 1.1  94/03/25  13:28:18  alan
 * Initial revision
 * 
**********************************************************************/

#ifndef _QUEUE_H
#define _QUEUE_H

#include <pthread.h>
#include <sem.h>
#include <time.h>

/* queue structure */
typedef struct queue {
	char name[MAX_NAME_LEN];			/* name of queue */
	csem_t mutex;						/* locking mutex */
	csem_t sSem;						/* sender semaphore */
	csem_t rSem;						/* receiver semaphore */
	int slots;							/* # of message slots in queue */
	int sIndex;							/* sender index */
	int rIndex;							/* receiver index */
	int sendct;							/* # of messages sent */
	int recvct;							/* # of messages received */
	void **data;						/* ptr to data array */
} queue_t;

/* function declarations */
queue_t *QCreate(const char *, int);
error_t QSend(queue_t *, void *, timeval_t *);
void QSend(queue_t *, void *);
void *QReceive(queue_t *, error_t *, timeval_t *);
void *QReceive(queue_t *);
void QFlush(queue_t *);
const char *QName(queue_t *);

#endif /* _QUEUE_H */