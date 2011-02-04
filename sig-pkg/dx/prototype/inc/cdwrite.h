/*******************************************************************************

 File:    cdwrite.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/cdwrite.h,v $
**
** Task to write DADD data buffers to disk as they fill
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/cdwrite.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: cdwrite.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:31  kes
** *** empty log message ***
**
 * Revision 1.2  98/10/29  11:29:49  kes
 * Added dataQ.
 * 
 * Revision 1.1  98/10/27  08:18:20  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _CDWRITE_H
#define _CDWRITE_H

#include <semaphore.h>
#include "nsserr.h"
#include "nssmsg.h"
#include "nsstypes.h"
#include "obs.h"
#include "queue.h"
#include "syslog.h"

typedef struct cd_info {
	queue_t **recvQ;			// ptr to receive queue
	queue_t **sendQ;			// ptr to send queue
	queue_t **dataQ;			// ptr to data queue
	sem_t *sem;					// disk serialization semaphore
} cd_info_t;

// obs definition structre for dadd input task
typedef struct cd_input_obs {
	obs_t obs;
} cd_input_obs_t;

// obs definition structure for dadd write task
typedef struct cd_write_obs {
	int fd;						// file for buffering
	obs_t obs;					// observation description
} cd_write_obs_t;

// function prototypes
task_t CDWriteTask(void *arg);
task_t CDInputTask(void *arg);

#endif