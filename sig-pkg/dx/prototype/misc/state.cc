/*******************************************************************************

 File:    state.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/state.cc,v $
**
** Class methods for generic State class
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/state.cc,v 1.2 2009/02/12 18:52:44 kes Exp $
** $Log: state.cc,v $
** Revision 1.2  2009/02/12 18:52:44  kes
** First compilable version of DX1.  Massive changes from the original
** DX0 (SonATA demo) code.  Compiles and links, but many known bugs and
** is not yet runnable.
**
** Revision 1.1.1.1  2006/08/02 05:30:27  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:55  kes
** *** empty log message ***
**
Revision 1.5  98/10/29  11:43:01  kes
NSS version

Revision 1.2  96/11/10  14:09:33  kes
Changes to convert from FUDD to TC.

Revision 1.1  96/05/15  18:33:41  kes
Initial revision

**********************************************************************/

#include <stdio.h>
#include <string.h>
#include "nss.h"
#include "globals.h"
#include "state.h"
#include "syslog.h"
#include "unit.h"

State::State()
{
	state = IDLE;
	bzero((char *) &unitStatus, sizeof(unitStatus));
	err = (error_t) lock;
}

State::~State()
{
}

error_t
State::SetState(nss_state_t newState)
{
	Lock();
	state = newState;
	Unlock();
	return (0);
}

nss_state_t
State::GetState()
{
	nss_state_t curState;

	Lock();
	curState = (nss_state_t) state;
	Unlock();
	return (curState);
}

void
State::SetActivityId(int32_t activity_id)
{
	Lock();
	activityId = activity_id;
	Unlock();
}

int32_t
State::GetActivityId()
{
	int32_t activity_id = 0;

	Lock();
#ifdef notdef
#endif
	Unlock();
	return (activity_id);
}

bool_t
State::ValidTransition(nss_state_t newState)
{
	return (1);
}

void
State::SetUnitState(unit_t unit, nss_state_t newState)
{
	int i;

	Lock();
	unitStatus[unit] = newState;
	Unlock();
}

nss_state_t
State::GetUnitState(unit_t unit)
{
	nss_state_t curState;

	Lock();
	curState = unitStatus[unit];
	Unlock();
	return (curState);
}

nss_state_t
State::GetConnStatus(int unitIndex)
{
	return (unitStatus[unitIndex]);
}

/*
** UnitActive: determine whether the specified unit is active
**
** Synopsis:
**		active = UnitActive(unit);
**		bool_t active;			return value
**		unit_t unit;			unit
** Returns:
**		1 if the specified unit is connected.
**		0 if not.
** Description:
**		Examines the connection status for the specified
**		unit; if the unit is connected, 1 is returned, 0 otherwise
** Notes:
**		None
*/
bool_t
State::UnitActive(unit_t unit)
{
	nss_state_t curState;

	Lock();
	curState = unitStatus[unit];
	Unlock();

	return (curState);
}

#if 0
array[] = {
	{ 
		control control control control control
	cmd	False	true	true	false	true
	cmd
	cmd
	cmd
};

typedef struct cmds {
	cmd_def_t cmd;
	bool_t invalid[4];
typedef
} cmds_t;

struct cntrl {
	unit_t units[MAX_CONTROL];
	cmds_t arr[MAX_CONTROL];
} cntrl_t;
 = { ConsoleConnection, TCConsole, SetiObs, None };

bool_t
State::GetValidity(command_def_t cmd)
{
	int i, j;

	Lock();
	for (i = 0; cmd[i].control; i++) {
		if (cmd[i][0].control == GetControl())
			for (j = 1; cmd[0][j].cmd; j++)
				if (cmd[0][j] == cmd)
					return (cmd[i][j]);
	}
	UnLock();
	return (-1);
}
#endif

//	bool_t CheckValidity(unit_t, cmd_def_t);// test for valid commands

bool_t
State::CheckValidity(unit_t u, int32_t c)
{
	return 1;
}
