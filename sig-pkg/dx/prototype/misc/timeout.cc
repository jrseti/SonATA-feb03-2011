/*******************************************************************************

 File:    timeout.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/timeout.cc,v $
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/timeout.cc,v 1.1.1.1 2006/08/02 05:30:27 kes Exp $
** $Log: timeout.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:27  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:56  kes
** *** empty log message ***
**
Revision 1.1  98/10/29  11:43:02  kes
NSS version

**********************************************************************/

#include <unistd.h>
#include "nsstypes.h"
#include "nsserr.h"
#include "syslog.h"
#include "timeout.h"
#include "util.h"

typedef struct timeout {
	void (*func)();					// function to call
	timeval_t tv;					// sleep time before calling
} timeout_t;

task_t TimeoutTask(void *);

//
// Timeout: call a function after a timeout
//
// Synopsis:
//		err = Timeout(func, tv);
//		error_t err;		return value
//		void (*func)();		ptr to function to call
//		timeval_t tv;		sleep time before calling function
// Returns:
//		0 if successful.
//		error code if timeout could not be performed
// Description:
//		Creates a task to perform a timeout then call a function.
// Notes:
//		This is used to provide a limited callback capability;
//		a more sophisticated version would allow cancellation
//		of the timeout, but there are certain implementation
//		problems in this (cancelling a non-existent thread is
//		dangerous), and the current requirements do not
//		make it worth the trouble.  Should it be necessary,
//		it would probably be necessary to find an unambiguous
//		method of registering the timeout thread so it could
//		be killed safely.
//
error_t
Timeout(void (*func)(), timeval_t& tv)
{
	FUNCTION("Timeout");
	error_t err;
	tid_t tid;
	timeout_t *timeout;

	timeout = new timeout_t;
	timeout->func = func;
	timeout->tv = tv;
	if (err = CreateTask(&tid, GetTaskPriority(0), TimeoutTask, timeout))
		SysFatal(err, function, "Creating timeout task");
	return (0);
}

//
// TimeoutTask: delay for specified time, then call a function
//
// Synopsis:
//		task_t TimeoutTask(timeout);
//		timeout_t *timeout;		timeout specification
// Returns:
//		A task, never returns.
// Description:
//		Waits for to expire, then calls a function.
// Notes:
//		None.
//
task_t
TimeoutTask(void *arg)
{
	FUNCTION("TimeoutTask");
	timeout_t *timeout;

	if (!arg)
		SysFatal(NSE_ITA, function, NULL);
	timeout = (timeout_t *) arg;

	susleep(timeout->tv.tv_sec, timeout->tv.tv_usec);
	timeout->func();
	delete timeout;
	TaskSuicide();
}