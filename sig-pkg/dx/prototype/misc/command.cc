/*******************************************************************************

 File:    command.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/command.cc,v $
**
** Generic Command processing task
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/command.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: command.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:53  kes
** *** empty log message ***
**
Revision 1.3  96/11/10  13:57:18  kes
Converted for TC; added command name string to cmd_proc_t to allow
string input (from SetiObs and console).

Revision 1.2  96/05/15  18:25:11  kes
Simplified call to QReceive.

Revision 1.1  96/01/31  11:39:23  kes
Initial revision
scheduler.c,v $
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include "tc.h"
#include "command.h"
#include "queue.h"
#include "globals.h"
#include "syslog.h"
#include "unit.h"
#include "util.h"

extern queue_t *CommandQ;

/* internal function prototypes */
int FindCommand(fudd_packet_t *, cmd_proc_t *);

/*
** CommandTask: perform FUDD control functions
**
** Synopsis:
**		task_t CommandTask(cmd);
**		cmd_proc_t *cmd;		ptr to list of valid commands
** Returns:
**		A task, never returns.
** Description:
**		Processes input command messages from the SCS and the detectors.
** Notes:
**		None.
*/
task_t
CommandTask(void *arg)
{
	FUNCTION("CommandTask");
	cmd_proc_t *cmd = (cmd_proc_t *) arg;
	fudd_packet_t *p;

	while (1) {
		p = (fudd_packet_t *) QReceive(CommandQ);
		ProcessInput(p, cmd);
		FreeMsg(p);
	}
}

/*
** ProcessInput: process command input
**
** Synopsis:
**		err = ProcessInput(p);
**		error_t err;			error code
**		fudd_packet_t *p;		ptr to message
** Returns:
**		0 if successful.
**		error code if an error occurred.
** Notes:
**		Input can be received from the SCS or any of the detectors.
**		The fudd_packet_t MUST be returned to the caller for release.
*/
error_t
ProcessInput(fudd_packet_t *p, cmd_proc_t *cmd)
{
	FUNCTION("ProcessInput");
	int i;
	error_t (*func)(fudd_packet_t *);

	/* find the code in the array */
	if ((i = FindCommand(p, cmd)) < 0) {
		return (SysLog(TE_ICMD, WARNING, function, "code = 0x%x, sender = %s",
				p->header.code, GetUnitName((unit_t) p->header.sender)));
	}
	return ((*cmd[i].func)(p));
};

/*
** FindCommand: find the command code in the array
**
** Synopsis:
**		i = ProcessInput(p, cmd);
*		int i;					index of command
**		fudd_packet_t *p;		ptr to message
**		cmd_proc_t *cmd;		ptr to array of commands
** Returns:
**		index >= 0 if successful.
**		-1 if not a valid command.
** Notes:
**		None.
*/
int
FindCommand(fudd_packet_t *p, cmd_proc_t *cmd)
{
	FUNCTION("ProcessInput");
	char cmdname[MAX_CMD_NAME];
	int i, len;

	/* if code is nonzero, find it; otherwise, match the string */
	if (p->header.code) {
		for (i = 0; cmd[i].code && cmd[i].code != p->header.code; i++)
			;
	}
	else {
		if (!p->header.length || !p->data) {
			SysLog(TE_ICF, WARNING, function,
					"code = %d, length = %d, data = 0x%x",
					p->header.code, p->header.length, p->data);
			return (-1);
			
		}
		if (!(len = strlen((char *) p->data))) {
			SysLog(TE_ICF, WARNING, function,
					"code = %d, length = %d, cmd length = %d",
					p->header.code, p->header.length, len);
			return (-1);
		}
		cmdname = new char[len+1];
		if (!cmdname)
			SysFatal(TE_NEM, function, "allocating command string");
		if (sscanf((char *) p->data, "%s", cmdname) != 1) {
			SysLog(TE_IIS, WARNING, function, "cmd = %s", (char *) p->data);
			delete [] cmdname;
			return (-1);
		}
		// convert the command to upper case
		StrUpper(cmdname);
		// find it in the list, skipping entries with no string equivalent
		for (i = 0; cmd[i].code; i++) {
			if (cmd[i].name && strlen(cmd[i].name)
					&& !strcmp(cmdname, cmd[i].name)) {
				p->header.code = cmd[i].code;
				break;
			}
		}
		delete [] cmdname;
	}
	return (i);
};
