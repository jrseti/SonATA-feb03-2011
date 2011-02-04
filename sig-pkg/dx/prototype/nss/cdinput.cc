/*******************************************************************************

 File:    cdinput.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/cdinput.cc,v $
**
** Task to simulate DADD input from DSP
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/cdinput.cc,v 1.1.1.1 2006/08/02 05:30:24 kes Exp $
** $Log: cdinput.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:24  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:24  kes
** *** empty log message ***
**
Revision 1.2  98/10/29  11:08:23  kes
Changed sendQ to dataQ.


Revision 1.1  98/10/27  08:16:22  kes
Initial revision

**********************************************************************/

#include <string.h>
#include <unistd.h>
#include "cdinput.h"
#include "cdwrite.h"
#include "globals.h"
#include "obs.h"

// function prototypes
void StartCDInput(nssmsg_t *msg, obs_t *obsInfo, cd_obs_input_t *obs,
		cd_info_t *info);
void FillCDBuf(cd_obs_input_t *obs, void *buf);

task_t
CDInputTask(void *arg)
{
	FUNCTION("CDInputTask");
	cd_info_t info;
	error_t err;
	nssmsg_t *msg;
	obs_t obsInfo;
	cd_obs_input_t obs;

	if (!arg)
		SysFatal(NSE_NTA, function, NULL);

	// get the initialization information
	info = *((cd_info_t *) arg);

	// initialize the observation write structure
	bzero((char *) &obs, sizeof(obs));
	obs.state = Idle;

	// run forever, waiting for messages
	while (1) {
		if (!(msg = (nssmsg_t *) QReceive(*info.recvQ)))
			SysFatal(err, function, "");
		switch (msg->header.code) {
		case Start:
			if (obs.state != Idle)
				SysFatal(NSE_OIP, function, NULL);
			obsInfo = *((obs_t *) msg->data);
			// start the observation
			StartCDInput(msg, &obsInfo, &obs, &info);
			break;
		case AbortObs:
			obs.state = Idle;
			break;
		default:
			SysFatal(NSE_IMT, function, "msg type = %d", msg->header.code);
		}
		FreeMsg(msg);
	}
}

void
StartCDInput(nssmsg_t *msg, obs_t *obsInfo, cd_obs_input_t *obs,
		cd_info_t *info)
{
	void *buf;
	int i;
	int32_t bufFrames;

	// initialize the observation structure
	obs->nChannels = obsInfo->bins / CHANNEL_WIDTH;
	obs->channelBlk = CHANNEL_WIDTH;
	bufFrames = CDBuf->Size() / obsInfo->bins;
	obs->channelLen = bufFrames * obs->channelBlk;
	obs->channelOfs = 0;
	obs->obsBufs = obsInfo->frames / bufFrames;

	for (i = 0; i < obs->obsBufs; i++) {
		buf = CDBuf->Alloc();
		msg = AllocMsg(0);
		bzero((char *) msg, sizeof(nssmsg_t));
		FillCDBuf(obs, buf);
		msg->header.code = BufferFull;
		msg->data = buf;
		QSend(*info->dataQ, msg);
	}
}

void
FillCDBuf(cd_obs_input_t *obs, void *buf)
{
	int i, j, blks;
	uint32_t ofs, len = 0;

	blks = obs->channelLen / obs->channelBlk;
	for (i = 0; i < blks; i++) {
		ofs = obs->channelOfs + i * obs->channelBlk;
		for (j = 0; j < obs->nChannels; j++) {
			bzero((char *) buf + ofs, obs->channelBlk);
			ofs += obs->channelLen;
			len += obs->channelBlk;
		}
	}
#ifndef notdef
		susleep(CD_SLEEP_SEC, CD_SLEEP_USEC);
#endif
}