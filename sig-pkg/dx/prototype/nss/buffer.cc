/*******************************************************************************

 File:    buffer.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/buffer.cc,v $
**
** Buffer handler class
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/buffer.cc,v 1.1.1.1 2006/08/02 05:30:24 kes Exp $
** $Log: buffer.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:24  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:23  kes
** *** empty log message ***
**
Revision 1.2  98/10/27  08:15:00  kes
Added mutex, cond variable.  Init was added due to an apparent
bug in initialization: new Buffer causes an error because the
pthread_mutex_init fails, thinking that the memory does not
belong to the process.

Revision 1.1  98/10/27  02:58:13  kes
Initial revision

**********************************************************************/

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "buffer.h"
#include "nsserr.h"
#include "syslog.h"

Buffer::Buffer(bufsize_t size, int nbufs)
{
	void *buf;
	bufEntry_t *next;

	err = 0;
	bufSize = size;

	// allocate all the buffers and link them into a list
	freeHead = allocHead = NULL;
	for (nBufs = 0; nBufs < nbufs; nBufs++) {
		buf = new uint8_t[size];
		next = new bufEntry_t;
		bzero((char *) next, sizeof(bufEntry_t));
		next->next = freeHead;
		next->buf = buf;
		freeHead = next;
	}
}

Buffer::~Buffer()
{
	bufEntry_t *entry, *next;

	pthread_mutex_trylock(&lock);

	for (entry = freeHead; entry; entry = next) {
		next = entry->next;
		delete entry->buf;
		delete entry;
	}

	for (entry = allocHead; entry; entry = next) {
		next = entry->next;
		delete entry->buf;
		delete entry;
	}
	pthread_mutex_destroy(&lock);
	pthread_mutexattr_delete(&mattr);
	pthread_cond_destroy(&cond);
	pthread_condattr_delete(&cattr);
}

void
Buffer::Init()
{
	// create the access lock
	if (pthread_mutexattr_create(&mattr)) {
		err = errno;
		return;
	}
	if (pthread_mutex_init(&lock, mattr)) {
		err = errno;
		return;
	}

	// create the condition variable
	if (pthread_condattr_create(&cattr)) {
		err = errno;
		return;
	}
	if (pthread_cond_init(&cond, cattr)) {
		err = errno;
		return;
	}
}	

void *
Buffer::Alloc()
{
	FUNCTION("Buffer::Alloc");
	void *buf;
	bufEntry_t *entry;

	// gain exclusive access to buffer list
	while (pthread_mutex_lock(&lock)) {
		if (errno != EINTR)
			SysFatal(errno, function, NULL);
	}

	// see if there is a buffer; fatal error if not
	if (!freeHead)
		SysFatal(errno, function, NULL);

	// remove the buffer from the free list and add it to alloc list
	entry = freeHead;
	freeHead = entry->next;
	entry->next = allocHead;
	allocHead = entry;
	buf = allocHead->buf;

	// signal change of state and release the list
#ifdef notdef
	pthread_cond_signal(&cond);
#endif
	pthread_mutex_unlock(&lock);
	return (buf);
}
	
void *
Buffer::AllocWait()
{
	FUNCTION("Buffer::AllocWait");
	void *buf;
	bufEntry_t *entry;

	// gain exclusive access to buffer list
	while (pthread_mutex_lock(&lock)) {
		if (errno != EINTR)
			SysFatal(errno, function, NULL);
	}

	while (!freeHead)
		pthread_cond_wait(&cond, &lock);

	// remove the buffer from the free list and add it to alloc list
	entry = freeHead;
	freeHead = entry->next;
	entry->next = allocHead;
	allocHead = entry;
	buf = allocHead->buf;

	// signal change of state and release the list
#ifdef notdef
	pthread_cond_signal(&cond);
#endif
	pthread_mutex_unlock(&lock);
	return (buf);
}
	
void
Buffer::Free(void *buf)
{
	FUNCTION("Buffer::Free");
	bufEntry_t *entry, *prev;

	// lock the list
	while (pthread_mutex_lock(&lock)) {
		if (errno != EINTR)
			SysFatal(errno, function, NULL);
	}

	// find the buffer in the allocated list
	entry = allocHead;
	while (entry && entry->buf != buf) {
		prev = entry;
		entry = entry->next;
	}
	if (entry) {
		if (entry == allocHead)
			allocHead = entry->next;
		else
			prev->next = entry->next;
		entry->next = freeHead;
		freeHead = entry;
	}
	else
		SysFatal(NSE_IBP, function, NULL);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&lock);
}		

bufsize_t
Buffer::Size()
{
	return (bufSize);
}

int
Buffer::Bufs()
{
	return (nBufs);
}