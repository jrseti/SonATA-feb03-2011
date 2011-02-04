/*******************************************************************************

 File:    read.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/read.h,v $ 
**
** Read task
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/read.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: read.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:35  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/29  11:34:07  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _READ_H
#define _READ_H

#include "nss.h"
#include "buffer.h"
#include "diskio.h"
#include "obs.h"
#include "queue.h"

typedef struct read_info {
	queue_t **recvQ;				// queue for receiving messages
	queue_t **sendQ;				// queue for sending messages
	queue_t **dataQ;				// queue for sending data
} read_info_t;

// obsevation control structure for read
typedef struct obs_read {
	Buffer *buffer;					// ptr to buffer
	queue_t *dataQ;					// output data queue
#ifdef notdef
	int fd;							// file descriptor
	sem_t *sem;						// serialization semaphore
	obs_t obs;						// obs description
#endif
	diskio_t ioReq;					// transfer description
	int blks;						// total # of blocks to read
#ifdef notdef
	bufsize_t bufSize;				// original write buffer size
#endif
} obs_read_t;

// function prototypes
task_t ReadTask(void *arg);

#endif