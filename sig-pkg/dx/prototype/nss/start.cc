/*******************************************************************************

 File:    start.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/start.cc,v $
**
** Main control task
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/start.cc,v 1.1.1.1 2006/08/02 05:30:25 kes Exp $
** $Log: start.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:25  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:26  kes
** *** empty log message ***
**
Revision 1.2  98/10/29  11:12:32  kes
Added code for StartCollection and StartDetection.

Revision 1.1  98/10/27  08:17:25  kes
Initial revision

**********************************************************************/

#include "cdinput.h"
#include "cdwrite.h"
#include "daddinput.h"
#include "daddwrite.h"
#include "globals.h"
#include "nsstypes.h"
#include "obs.h"
#include "queue.h"
#include "start.h"

void
StartDataCollection(obs_t *startObs)
{
	obs_t obs;
	nssmsg_t *msg;

	// record the observation parameters
	obs = *startObs;
	obs.state = Idle;

	////////////////////////////////////////////
	// write tasks
	///////////////////////////////////////////

	// start data collection for the DADD write tasks
	// dadd left
	msg = AllocMsg(sizeof(obs_t));
	msg->header.code = Start;
	*((obs_t *) msg->data) = obs;
	QSend(LDaddCollectQ, msg);

	// dadd right
	msg = AllocMsg(sizeof(obs_t));
	msg->header.code = Start;
	*((obs_t *) msg->data) = obs;
	QSend(RDaddCollectQ, msg);

	// start data collection for the CD write tasks
	// CD left
	msg = AllocMsg(sizeof(obs_t));
	msg->header.code = Start;
	*((obs_t *) msg->data) = obs;
	QSend(LCDCollectQ, msg);

	// CD right
	msg = AllocMsg(sizeof(obs_t));
	msg->header.code = Start;
	*((obs_t *) msg->data) = obs;
	QSend(RCDCollectQ, msg);

	/////////////////////////////////////////////////
	// input tasks
	////////////////////////////////////////////////
	// start data collection for the DADD write tasks
	// dadd left
	msg = AllocMsg(sizeof(obs_t));
	msg->header.code = Start;
	*((obs_t *) msg->data) = obs;
	QSend(LDaddInputQ, msg);

	// dadd right
	msg = AllocMsg(sizeof(obs_t));
	msg->header.code = Start;
	*((obs_t *) msg->data) = obs;
	QSend(RDaddInputQ, msg);

	// start data collection for the CD write tasks
	// CD left
	msg = AllocMsg(sizeof(obs_t));
	msg->header.code = Start;
	*((obs_t *) msg->data) = obs;
	QSend(LCDInputQ, msg);

	// CD right
	msg = AllocMsg(sizeof(obs_t));
	msg->header.code = Start;
	*((obs_t *) msg->data) = obs;
	QSend(RCDInputQ, msg);

}

void
StartDetection(obs_t *obs)
{
	nssmsg_t *msg;

	msg = AllocMsg(sizeof(obs_t));
	msg->header.code = Start;
	*((obs_t *) msg->data) = *obs;
	QSend(DetectQ, msg);
}

void
StartConfirmation(obs_t *obs)
{
	nssmsg_t *msg;

	msg = AllocMsg(sizeof(obs_t));
	msg->header.code = Start;
	*((obs_t *) msg->data) = *obs;
	QSend(ConfirmQ, msg);
}