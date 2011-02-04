/*******************************************************************************

 File:    nss.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/nss.cc,v $
**
** Main program
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/nss.cc,v 1.1.1.1 2006/08/02 05:30:24 kes Exp $
** $Log: nss.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:24  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:26  kes
** *** empty log message ***
**
Revision 1.2  98/10/29  11:11:48  kes
Added call to BuildDaddArray.

Revision 1.1  98/10/27  08:17:25  kes
Initial revision

**********************************************************************/

#define GLOBAL_VARS

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <socket.h>
#include <netinet/in.h>

#include "nss.h"
#include "nssmsg.h"
#include "bldarray.h"
#include "ftime.h"
#include "globals.h"
#include "init.h"
#include "state.h"
#include "signals.h"
#include "syslog.h"
#include "unit.h"
#include "util.h"

static char usage[] = "nss\n";

/* function prototypes */
static int GetArgs(int argc, char **argv);
void InitStatus(void);
void WaitForDone(void);

int
main(int argc, char **argv)
{
	FUNCTION("main");

	/* initialize the clock logic */
	InitTime();

	/* allocate queue message list */
	AllocMsgInit(512);

	SysLogInit();
	SetLogType(INFO, LOG_ON, LOG_TO_LOGFILE, NULL);
	SetLogType(WARNING, LOG_ON, LOG_TO_LOGFILE, NULL);
	SetLogType(HI_ERROR, LOG_ON, LOG_TO_LOGFILE, NULL);
	SetLogType(FATAL, LOG_ON, LOG_TO_LOGFILE, NULL);
	SysLog(NSE_NE, INFO, function, "system starting");

	SetLogType(LOG_CMD, LOG_OFF, LOG_TO_FILE, "TCCmdLog");
	SysLog(NSE_NE, LOG_CMD, function, "system starting");

	/* process the arguments after we have a way of logging an error */
	if (GetArgs(argc, argv))
		SysFatal(NSE_IA, function, usage);

	HandleSignals(Handlers);

	/* build the DADD byte unpack array */
	BuildDaddArray(&DaddTable);

	/* initialize the list of queues */
	InitQueues(Queues);

	/* initialize the list of semaphores */
	InitSems(Sems);

	/* initialize the list of files */
	InitFiles(Files);

	/* initialize the list of buffers */
	InitBuffers(Buffers);

#ifdef notdef
	/* initialize the connection structures */
	InitConnections(Connections);
#endif

	InitTasks(Tasks);

	WaitForDone();
}

/*
** GetArgs: process command-line arguments
**
** Synopsis:
**		rval = GetArgs(argc, argv);
**		int rval;			return value
**		int argc;			argument count
**		char **argv;		arguments
** Returns:
**		0 if successful.
**		-1 if invalid argument list
** Description:
**		Parses the command line to allow run-time control of operation.
** Notes:
**		The following command-line arguments are accepted:
**			None at present.
*/
static int
GetArgs(int argc, char **argv)
{
	int i;
	client_t *client;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-')
			return (-1);
		switch (argv[i][1]) {
		default:
			return (-1);
		}
	}
	return (0);
}

void
WaitForDone(void)
{
	tid_t tid = GetTaskId();
	
	DetachTask(&tid);
	TaskSuicide();
	while (1)
		sleep(10);
}