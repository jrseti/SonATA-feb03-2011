/*******************************************************************************

 File:    init.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/init.h,v $
**
** NSS initialization structures
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/init.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: init.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:34  kes
** *** empty log message ***
**
 * Revision 1.2  98/10/29  11:31:53  kes
 * Added new queue sizes.
 * 
 * Revision 1.1  98/10/27  08:18:26  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _INIT_H
#define _INIT_H

#include "buffer.h"
#include "client.h"
#include "queue.h"

/* queue slots */
#define COLLECT_SLOTS		10
#define DETECT_SLOTS		10
#define CONFIRM_SLOTS		10
#define READ_SLOTS			5
#define WAIT_SLOTS			10
#define DADD_INPUT_SLOTS	5
#define DADD_WRITE_SLOTS	5
#define CD_INPUT_SLOTS		5
#define CD_WRITE_SLOTS		5
#define READ_DATA_SLOTS		16

/*
** queue specification structure: an array of these is processed
** to create system queues during initialization.  The array MUST be
** terminated with an array entry of NULL.
*/
typedef struct queue_spec {
	queue_t **queue;					/* address of queue pointer */
	char name[MAX_NAME_LEN];			/* queue name */
	int slots;							/* # of message slots */
} queue_spec_t;

/*
** semaphore specification structure: an array of these is processed
** to create system semaphores during initialization.  The array MUST be
** terminated with an array entry of NULL.
*/
typedef struct sem_spec {
	sem_t *sem;							/* address of semaphore ptr */
	char name[MAX_NAME_LEN];			/* queue name */
	int value;							/* initial value */
} sem_spec_t;

/*
** file specification structure: an array of these is processed
** to open system files during initialization.  The array MUST be
** terminated with an array entry of NULL.
*/
typedef struct file_spec {
	int *fd;							/* address of file descriptor */
	char name[MAX_NAME_LEN];			/* file name */
	int oflags;							/* file open flags */
	int mode;							/* open mode */
} file_spec_t;

/*
** buffer specification structure: an array of these is processed
** to create system buffers during initialization.  The array MUST be
** terminated with an array entry of NULL.
*/
typedef struct buffer_spec {
	Buffer **buf;						/* address of buffer ptr */
	char name[MAX_NAME_LEN];			/* buffer name */
	uint32_t bufLen;					/* length of each buffer */ 
	int nBufs;							/* number of buffers */
} buffer_spec_t;

/*
** task specification structure: an array of these is processed
** to create system tasks during initialization.  The array MUST be
** terminated with an array entry of NULL.
*/
typedef struct task_spec {
	task_t (*func)(void *);				/* task function pointer */
	int priority;						/* task priority */
	void *arg;							/* ptr to task argument */
	tid_t *tid;							/* ptr to task ID storage */
} task_spec_t;

/* function prototypes */
void InitQueues(queue_spec_t *);
void InitSems(sem_spec_t *);
void InitFiles(file_spec_t *);
void InitBuffers(buffer_spec_t *);
void InitConnections(connection_t **);
void InitTasks(task_spec_t *);

#endif /* _INIT_H */