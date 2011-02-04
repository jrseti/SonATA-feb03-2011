/*******************************************************************************

 File:    init.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/init.cc,v $
**
** Initialization functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/init.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: init.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:54  kes
** *** empty log message ***
**
 * Revision 1.5  98/10/29  11:42:58  kes
 * NSS version
 * 
 * Revision 1.4  96/11/10  14:04:28  kes
 * Changes to convert from FUDD to TC.
 * 
 * Revision 1.3  96/02/09  09:17:34  kes
 * Modified InitConnections to set port values.
 * 
 * Revision 1.2  96/01/31  11:43:18  kes
 * Major changes to allow code reuse for detector clients, moved
 * arrays to minittab.cc.
 * 
 * Revision 1.1  96/01/24  13:13:54  kes
 * Initial revision
 * 
**********************************************************************/

#include "nss.h"
#include "nssmsg.h"
#include "connect.h"
#include "init.h"
#include "syslog.h"
#include "unit.h"
#include "util.h"

/*
** InitQueues: initialize system queues
**
** Synopsis:
**		void InitQueues(qsp);
**		queue_spec_t *qsp;			ptr to queue spec array
** Returns:
**		Nothing.
** Description:
**		Creates the set of system queues required for operation.  If
**		queue creation fails, the system logs a fatal error and terminates.
** Notes:
**		None.
*/
void
InitQueues(queue_spec_t *qsp)
{
	FUNCTION("InitQueues");

	for ( ; qsp->queue; qsp++) {
		*qsp->queue = QCreate(qsp->name, qsp->slots);
		if (!*qsp->queue) {
			SysLog(NSE_QCE, FATAL, function, "queue = %s, slots = %d",
					qsp->name, qsp->slots);
#ifdef notdef
			SysLogExit();
#endif
			TaskSuicide();
		}
	}
}

/*
** InitSems: initialize system semaphores
**
** Synopsis:
**		void InitSems(ssp);
**		sem_spec_t *ssp;			ptr to semaphore spec array
** Returns:
**		Nothing.
** Description:
**		Creates the set of system semaphores required for operation.  If
**		semaphore creation fails, the system logs a fatal error and terminates.
** Notes:
**		None.
*/
void
InitSems(sem_spec_t *ssp)
{
	FUNCTION("InitSems");

	for ( ; ssp->sem; ssp++) {
		if (sem_init(ssp->sem, 0, ssp->value))
			SysFatal(NSE_CCS, function, NULL);
	}
}

/*
** InitFiles: initialize system files
**
** Synopsis:
**		void InitFiles(fsp);
**		sem_spec_t *fsp;			ptr to file spec array
** Returns:
**		Nothing.
** Description:
**		Opens the set of files required for operation.  If
**		file creation fails, the system logs a fatal error and terminates.
** Notes:
**		In general, the files will be raw disk partitions for performance
**		reasons, although they may be any file type.
*/
void
InitFiles(file_spec_t *fsp)
{
	FUNCTION("InitFiles");

	for ( ; fsp->fd; fsp++) {
		*fsp->fd = open(fsp->name, fsp->oflags, fsp->mode);
		if (*fsp->fd < 0)
			SysFatal(errno, function, NULL);
	}
}

/*
** InitBuffers: initialize system buffers
**
** Synopsis:
**		void InitBuffers(bsp);
**		buffer_spec_t *bsp;			ptr to file spec array
** Returns:
**		Nothing.
** Description:
**		Opens the set of buffers required for operation.  If
**		buffer creation fails, the system logs a fatal error and terminates.
** Notes:
**		None.
*/
void
InitBuffers(buffer_spec_t *bsp)
{
	FUNCTION("InitBuffers");

	for ( ; bsp->buf; bsp++) {
		*bsp->buf = new Buffer(bsp->bufLen, bsp->nBufs);
		if (!*bsp->buf)
			SysFatal(errno, function, NULL);
		(*bsp->buf)->Init();
	}
}

/*
** InitConnections: initialize connection port entries
**
** Synopsis:
**		void InitConnections(c);
**		connection_t **c;	ptr to array of connections
** Returns:
**		Nothing.
** Description:
**		Determines and stores the port specification for the entry.
** Notes:
**		Does not attempt to establish the connection; this is done
**		when each connection is first used.
**		Called only by the detectors, not the master.
*/
void
InitConnections(connection_t **c)
{
	FUNCTION("InitConnections");
	int i;
#ifdef notdef
	for (i = 0 ; c[i]; i++)
		c[i]->conn.net.port = GetUnitPort(c[i]->unit);
#endif
}

/*
** InitTasks: initialize system tasks
**
** Synopsis:
**		void InitTasks(tsp);
**		task_spec_t *tsp;		ptr to array of task specifiers
** Returns:
**		Nothing.
** Description:
**		Creates the set of system tasks required for operation.  If
**		task creation fails, the system logs a fatal error and terminates.
** Notes:
**		None.
*/
void
InitTasks(task_spec_t *tsp)
{
	FUNCTION("InitTasks");
	error_t err;

	/* create each task */
	for ( ; tsp->func; tsp++) {
		if (err = CreateTask(tsp->tid, tsp->priority, tsp->func, tsp->arg)) {
			SysLog(err, FATAL, function, "Creating task");
			TaskSuicide();
		}
	}
}