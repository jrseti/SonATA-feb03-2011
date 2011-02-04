/*******************************************************************************

 File:    sigwait.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/sigwait.cc,v $
**
** Task to wait for signals.  This is necessary to guarantee that
** we always see ^C.
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/sigwait.cc,v 1.1.1.1 2006/08/02 05:30:25 kes Exp $
** $Log: sigwait.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:25  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:55  kes
** *** empty log message ***
**
Revision 1.1  98/10/29  11:43:00  kes
NSS version

Revision 1.1  96/11/10  14:40:47  kes
Initial revision

**********************************************************************/

#include <signal.h>
#include "nsstypes.h"
#include "exit.h"

task_t
SigWaitTask(void *arg)
{
	void *value;
	long sig;
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGPIPE);
	while (1) {
		sig = sigwait(&set, &value);
		switch (sig) {
		case SIGPIPE:
			break;
		case SIGQUIT:
			Quit();
			break;
		default:
		case SIGINT:
			Interrupt();
			break;
		}
	}
}