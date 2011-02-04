/*******************************************************************************

 File:    collect.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/collect.cc,v $
**
** Data collection task
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/collect.cc,v 1.1.1.1 2006/08/02 05:30:23 kes Exp $
** $Log: collect.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:23  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:24  kes
** *** empty log message ***
**
Revision 1.1  98/10/29  11:08:44  kes
Initial revision
**********************************************************************/

#include "buffer.h"
#include "collect.h"
#include "collector.h"
#include "globals.h"
#include "syslog.h"

// function prototypes
void StartCollection(obs_t *obs, collect_info_t *info);

//
// CollectionTask: data collection task
//
task_t
CollectionTask(void *arg)
{
	FUNCTION("CollectionTask");
	collect_info_t info;
	error_t err;
	nssmsg_t *msg;
	obs_state_t state = Idle;
	obs_t obs;

	if (!arg)
		SysFatal(NSE_NTA, function, NULL);

	// get the initialization information
	info = *((collect_info_t *) arg);

	// run forever, waiting for messages
	while (1) {
		msg = (nssmsg_t *) QReceive(*info.recvQ);
		switch (msg->header.code) {
		case Start:
			if (state != Idle)
				SysFatal(NSE_OIP, function, NULL);
			obs = *((obs_t *) msg->data);
			// start the observation
			StartCollection(&obs, &info);
			break;
		case AbortObs:
			state = Idle;
			break;
		default:
			SysFatal(NSE_IMT, function, "msg type = %d", msg->header.code);
		}
		FreeMsg(msg);
	}
}

void
StartCollection(obs_t *obs, collect_info_t *info)
{
	FUNCTION("StartCollect");
	collector_t collect;
	Collector *collector;
	error_t err;
	nssmsg_t *msg;

	collect.type = info->type;
	collect.pol = info->pol;
	collect.dataQ = *info->dataQ;
	collect.sem = info->sem;
	collect.buffer = *info->buf;
	switch (info->type) {
	case Dadd:
		collect.fd = obs->daddFd[info->pol];
		break;
	case CD:
		collect.fd = obs->cdFd[info->pol];
		break;
	default:
		SysFatal(NSE_ICT, function, "type = %d", info->type);
	}
	collect.obs = *obs;

	collector = new Collector(&collect);
	if (err = (error_t) *collector)
		SysFatal(err, function, NULL);

	if (err = collector->Collect())
		SysFatal(err, function, NULL);
	delete collector;

	// notify done
	msg = AllocMsg(0);
	msg->header.code = Collecting;
	QSend(*info->sendQ, msg);
}
