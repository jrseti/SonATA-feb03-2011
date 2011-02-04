/*******************************************************************************

 File:    exit.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/exit.cc,v $
**
** Signal handling functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/exit.cc,v 1.1.1.1 2006/08/02 05:30:26 kes Exp $
** $Log: exit.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:26  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:54  kes
** *** empty log message ***
**
Revision 1.3  98/10/29  11:42:58  kes
NSS version

Revision 1.2  96/11/10  14:02:59  kes
Minor changes to convert from FUDD to TC.

Revision 1.1  96/05/15  18:36:34  kes
Initial revision

Revision 1.2  96/02/09  09:12:06  kes
Replaced BlockSignals, IgnoreSignals with HandleSignals driven by
an initialization table.

**********************************************************************/

#include <unistd.h>
#include "nss.h"
#include "exit.h"
#include "syslog.h"

/*
** Exit: terminate system operation
**
** Synopsis:
**		void Exit(status);
**		int status;				exit status
** Returns:
**		Does not return.
** Description:
**		Exits with the specified status code.
** Notes:
**		The status code can be used to control whether run restarts
**		the program or simply terminates.
*/
void
Exit(int status)
{
	SysLogExit();
	exit(status);
}

/*
** Interrupt: interrupt the program
**
** Synopsis:
**		void Interrupt(...);
** Returns:
**		Does not return.
** Description:
**		Exits with a RESET status code.
** Notes:
**		This will generally exit the program, then run will
**		restart it.
*/
void
Interrupt(...)
{
	Exit(RESET);
}

/*
** Quit: quit the system
**
** Synopsis:
**		void Quit(...);
** Returns:
**		Does not return.
** Description:
**		Exits with a SHUTDOWN status code.
** Notes:
**		This will generally exit the program, and run will also terminate.
*/
void
Quit(...)
{
	Exit(SHUTDOWN);
}

/*
** Continue: continue without stopping the program
**
** Synopsis:
**		void Continue(...);
** Returns:
**		Does not return.
** Description:
**		Returns without doing anything.
** Notes:
**		This is a workaround because SIG_IGN does not seem
**		to do what it should.
*/
void
Continue(...)
{
}
