/*******************************************************************************

 File:    control.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/control.cc,v $
**
** Main control task
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/control.cc,v 1.1.1.1 2006/08/02 05:30:24 kes Exp $
** $Log: control.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:24  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:24  kes
** *** empty log message ***
**
Revision 1.2  98/10/29  11:09:10  kes
Removed SelectFds, changed obs handling.

Revision 1.1  98/10/27  08:17:08  kes
Initial revision

**********************************************************************/

#include "control.h"
#include "create.h"
#include "exit.h"
#include "globals.h"
#include "start.h"
#include "syslog.h"
#include "waitdone.h"

// function prototypes
void SelectFds();

task_t
ControlTask(void *arg)
{
	FUNCTION("ControlTask");
	obs_t *obs1, *obs2;

	/* simulate observations */
	obs1 = CreateObs();
	SysLog(NSE_NE, INFO, function, "start data collection obs %d",
			obs1->id);
	StartDataCollection(obs1);
	while (1) {
		WaitDone(Collecting, obs1);
		SysLog(NSE_NE, INFO, function, "data collection complete obs %d",
				obs1->id);
		SysLog(NSE_NE, INFO, function, "start signal detection obs %d",
				obs1->id);
		StartDetection(obs1);
		obs2 = CreateObs();
		SysLog(NSE_NE, INFO, function, "start data collection obs %d",
				obs2->id);
		StartDataCollection(obs2);
		WaitDone(Detecting, obs1);
		SysLog(NSE_NE, INFO, function, "signal detection complete obs %d",
				obs1->id);
		SysLog(NSE_NE, INFO, function, "start confirmation obs %d",
				obs1->id);
		StartConfirmation(obs1);
		WaitDone(Confirming, obs1);
		SysLog(NSE_NE, INFO, function, "confirmation complete obs %d",
				obs1->id);
		obs1 = obs2;
	}
}