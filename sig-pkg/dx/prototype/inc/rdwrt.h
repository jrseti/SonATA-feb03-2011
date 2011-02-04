/*******************************************************************************

 File:    rdwrt.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/rdwrt.h,v $ 
**
** Write and read tasks
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/rdwrt.h,v 1.1.1.1 2006/08/02 05:29:53 kes Exp $
** $Log: rdwrt.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:53  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:35  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/29  11:34:03  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _RDWRT_H
#define _RDWRT_H

#include "nsstypes.h"
#include "queue.h"

// task structures
typedef struct write_info {
	queue_t **recvQ;				// queue for receiving messages
	queue_t **sendQ;				// queue for sending messages
} write_info_t;

typedef struct read_info {
	queue_t **recvQ;				// queue for receiving messages
	queue_t **sendQ;				// queue for sending messages
	queue_t **dataQ;				// queue for sending data
} read_info_t;

// function prototypes
task_t WriteTask(void *arg);
task_t ReadTask(void *arg);

#endif