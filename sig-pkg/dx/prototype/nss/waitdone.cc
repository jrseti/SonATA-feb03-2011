/*******************************************************************************

 File:    waitdone.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/waitdone.cc,v $
**
** Wait for process done
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/waitdone.cc,v 1.1.1.1 2006/08/02 05:30:25 kes Exp $
** $Log: waitdone.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:25  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:26  kes
** *** empty log message ***
**
Revision 1.2  98/10/29  11:13:01  kes
Made change to wait on a different queue for each wait state.

Revision 1.1  98/10/27  08:17:26  kes
Initial revision

**********************************************************************/

#include "nss.h"
#include "globals.h"
#include "nssmsg.h"
#include "obs.h"
#include "queue.h"
#include "syslog.h"
#include "waitdone.h"

// function prototypes
void WaitCollect(obs_t *obs);
void WaitDetect(obs_t *obs);
void WaitConfirm(obs_t *obs);

void
WaitDone(obs_state_t state, obs_t *obs)
{
	FUNCTION("WaitDone");

	switch (state) {
	case Collecting:
		WaitCollect(obs);
		break;
	case Detecting:
		WaitDetect(obs);
		break;
	case Confirming:
		WaitConfirm(obs);
		break;
	default:
		SysFatal(NSE_ISS, function, "state = %d", state);
	}
}

void
WaitCollect(obs_t *obs)
{
	FUNCTION("WaitCollect");
	int n = 0;
	nssmsg_t *msg;

	while (n < 4) {
		msg = (nssmsg_t *) QReceive(WaitCollectQ);
		switch (msg->header.code) {
		case Collecting:
			n++;
			break;
		default:
			SysFatal(NSE_IMT, function, "type = %d", msg->header.code);
		}
		FreeMsg(msg);
	}
}

void
WaitDetect(obs_t *obs)
{
	FUNCTION("WaitDetect");
	nssmsg_t *msg;

	while (1) {
		msg = (nssmsg_t *) QReceive(WaitDetectQ);
		switch (msg->header.code) {
		case Detecting:
			return;
		default:
			SysFatal(NSE_IMT, function, "type = %d", msg->header.code);
		}
		FreeMsg(msg);
	}
}

void
WaitConfirm(obs_t *obs)
{
	FUNCTION("WaitConfirm");
	nssmsg_t *msg;

	while (1) {
		msg = (nssmsg_t *) QReceive(WaitConfirmQ);
		switch (msg->header.code) {
		case Confirming:
			return;
		default:
			SysFatal(NSE_IMT, function, "type = %d", msg->header.code);
		}
		FreeMsg(msg);
	}
}

