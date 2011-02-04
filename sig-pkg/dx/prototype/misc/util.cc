/*******************************************************************************

 File:    util.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/util.cc,v $
**
** Utility functions for shared memory and semaphores
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/util.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: util.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:56  kes
** *** empty log message ***
**
 * Revision 1.9  98/10/29  11:43:03  kes
 * NSS version
 * 
 * Revision 1.8  96/11/10  14:11:35  kes
 * Changes to convert from FUDD to TC.
 * 
 * Revision 1.7  96/05/15  18:35:44  kes
 * Added DetachTask.
 * 
 * Revision 1.6  96/02/09  09:16:46  kes
 * Removed unused functions.
 * 
 * Revision 1.5  96/01/31  11:59:22  kes
 * Added code to detach thread in TaskCreate, added RequestId().
 * 
 * Revision 1.4  96/01/26  10:49:22  kes
 * Removed unnecessary functions: semaphore, shared memory, and checksum.
 * 
 * Revision 1.3  96/01/09  13:42:48  kes
 * Added CreateArchiveName.
 * 
 * Revision 1.2  95/03/09  13:35:56  kes
 * Added error returns, replaced pthread_kill with pthread_exit.
 * 
 * Revision 1.1  95/02/09  11:28:07  kes
 * Initial revision
 * 
 * Revision 1.4  94/09/09  17:40:50  kes
 * Added PrintUsage.
 * 
 * Revision 1.3  94/09/01  08:52:02  kes
 * Modified GetUsage to accept a pointer to the resource structure.
 * 
 * Revision 1.2  94/08/19  07:24:37  kes
 * Added CreateTask.
 * 
 * Revision 1.1  94/08/12  14:41:22  kes
 * Initial revision
 * 
**********************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include "nss.h"
#include "util.h"

static struct rusage Usage;

/*
** CreateTask: create a task
*/
error_t
CreateTask(tid_t *tidp, int priority, void *(*func)(void *), void *arg)
{
	pthread_attr_t attr;
	pthread_mutexattr_t mattr;

	/* create the attribute */
	pthread_attr_create(&attr);
	if (pthread_attr_setinheritsched(&attr, PTHREAD_DEFAULT_SCHED) == -1)
		return (errno);
	if (pthread_attr_setsched(&attr, SCHED_DEFAULT) == -1)
		return (errno);
	if (pthread_attr_setprio(&attr, priority) == -1)
		return (errno);

	/* create the task */
	if (pthread_create(tidp, attr, func, arg) == -1) {
		return (errno);
	}
	return (pthread_detach(tidp));
}
	
/*
** DetachTask: detach the specified task
*/
error_t
DetachTask(tid_t *tidp)
{
	return (pthread_detach(tidp));
}
	
/*
** GetTaskId: return current task id
*/
tid_t
GetTaskId(void)
{
	return (pthread_self());
}

/*
** GetTaskPriority: return a task priority
*/
int
GetTaskPriority(tid_t tid)
{
	if (tid)
		return (pthread_getprio(tid));
	else
		return (pthread_getprio(pthread_self()));
}

/*
** SetTaskPriority: set a task priority
*/
int
SetTaskPriority(tid_t tid, int prio)
{
	if (tid)
		return (pthread_setprio(tid, prio));
	else
		return (pthread_setprio(pthread_self(), prio));
}

/*
** TaskSuicide: commit suicide
*/
void
TaskSuicide(void)
{
	pthread_exit(0);
}

/*
** KillTask: kill another task
*/
error_t
KillTask(tid_t tid)
{
	if (pthread_kill(tid, SIGQUIT))
		return ((error_t) errno);
}

/*
** RequestId: return next request ID
*/
int32_t
RequestId(void)
{
	static int32_t requestId = 0;

	return (++requestId);
}

/*
** GetUsage: get the program usage
*/
int
GetUsage(struct rusage *usage)
{
	if (usage)
		return (getrusage(RUSAGE_SUM,  usage));
	else
		return (getrusage(RUSAGE_SUM, &Usage));
}

/*
** PrintUsage: print the program usage
*/
void
PrintUsage(char *str, struct rusage *usage)
{
	printf("%s: %d.%03du, %d.%03ds\n", str,
			usage->ru_utime.tv_sec, usage->ru_utime.tv_usec / 1000,
			usage->ru_stime.tv_sec, usage->ru_stime.tv_usec / 1000);
}

/*
** StrUpper: convert a string to upper case
*/
void
StrUpper(char *s)
{
	while (*s) {
		*s = toupper(*s);
		s++;
	}
}