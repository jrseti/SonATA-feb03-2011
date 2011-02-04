/*******************************************************************************

 File:    confirm.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/confirm.cc,v $
**
** Confirmation task
**
** Signal detection is performed sequentially: DADD left pol, DADD right
** pol, PD, then confirmation.  Thus, a single large buffer can be used
** for the entire detection process, rather than separate buffers.
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/confirm.cc,v 1.1.1.1 2006/08/02 05:30:24 kes Exp $
** $Log: confirm.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:24  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:24  kes
** *** empty log message ***
**
**********************************************************************/

#include "buffer.h"
#include "confirm.h"
#include "globals.h"
#include "nssmsg.h"
#include "obs.h"
#include "read.h"
#include "syslog.h"

typedef struct channel_data {
	int channel;					// first channel to read
	int channels;					// total # of channels to read
	int channelBlks;				// # of blocks for each channel			
	int blkFrames;					// # of frames in a buffer
	uint32_t channelBlkLen;			// length of a channel block
	uint32_t fileBlkLen;			// bytes between channel blocks on disk
} channel_data_t;

channel_data_t channelData;

typedef struct sig_desc {
	uint32_t id;					// signal id
} sig_desc_t;

sig_desc_t sigDesc[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7
};

// function prototypes	
void StartConfirmation(obs_t *obs, sig_desc_t *sigDesc, confirm_info_t *info);
void StartCDRead(pol_t pol, obs_t *obs, sig_desc_t *sigDesc,
		confirm_info_t *info);
void Confirm(pol_t pol, obs_t *obs, sig_desc_t *sigDesc, confirm_info_t *info);

task_t
ConfirmationTask(void *arg)
{
	FUNCTION("ConfirmationTask");
	confirm_info_t info;
	error_t err;
	nssmsg_t *msg, *msg1;
	obs_state_t state = Idle;
	obs_t obs;

	if (!arg)
		SysFatal(NSE_NTA, function, NULL);

	// get the initialization information
	info = *((confirm_info_t *) arg);

	// run forever, waiting for messages
	while (1) {
		msg = (nssmsg_t *) QReceive(*info.recvQ);
		switch (msg->header.code) {
		case Start:
			if (state != Idle)
				SysFatal(NSE_OIP, function, NULL);
			obs = *((obs_t *) msg->data);
			state = Confirming;
			StartConfirmation(&obs, sigDesc, &info);
			msg1 = AllocMsg(0);
			msg1->header.code = Confirming;
			QSend(*info.sendQ, msg1);
			state = Idle;
			break;
		case AbortObs:
			state = Idle;
			break;
		case ReadDone:
#ifdef notdef
			if (state != Detecting)
				SysFatal(NSE_NOIP, function, "msg type = %d", msg->header.code);
			state = Idle;
#endif
			break;
			default:
			SysFatal(NSE_IMT, function, "msg type = %d", msg->header.code);
		}
		FreeMsg(msg);
	}
}

void
StartConfirmation(obs_t *obs, sig_desc_t *sigDesc, confirm_info_t *info)
{
	FUNCTION("StartConfirmation");
	int i;
	error_t err;
	nssmsg_t *msg;

	// process all 8 signals to be confirmed; these are dummy signals
	// for testing
	// perform left pol DADD
	for (i = 0; i < 8; i++) {
		StartCDRead(Left, obs, sigDesc, info);
		Confirm(Left, obs, sigDesc, info);
	}
	// now right pol
	for (i = 0; i < 8; i++) {
		StartCDRead(Right, obs, sigDesc, info);
		Confirm(Right, obs, sigDesc, info);
	}
}

void
StartCDRead(pol_t pol, obs_t *obs, sig_desc_t *sigDesc, confirm_info_t *info)
{
	FUNCTION("StartCDRead");
	diskio_t ioReq;
	nssmsg_t *msg;
	obs_read_t obsRead;
	uint32_t spectrumBytes;

	// read a block of channels at a time (1 - 16)
	// the number of channels read will be a function of the total drift
	// of the signal and its relation to a band center.
	
	channelData.channel = sigDesc->id * 20;
	channelData.channels = 16;
	channelData.blkFrames = CDBuf->Size() / obs->bins;
	channelData.channelBlks = obs->frames / channelData.blkFrames;
	channelData.channelBlkLen = 4096;
	//////////////////////////////////////////////////////////
	// this is not correct for obs not a multiple of 4 frames
	/////////////////////////////////////////////////////////
	channelData.fileBlkLen = CDBuf->Size();
	
	// build the observation control block for the read task
	obsRead.buffer = ChanBuf;
	obsRead.dataQ = *info->dataQ;
	obsRead.blks = channelData.channelBlks;

	ioReq.buf = 0;
	ioReq.bufOfs = 0;
	ioReq.bufBlkLen = channelData.channelBlkLen * channelData.channels;
	ioReq.fd = obs->cdFd[pol];
	ioReq.fileOfs = channelData.channel * channelData.channelBlkLen;
	ioReq.fileBlkLen = channelData.fileBlkLen;
	ioReq.sem = &CDSem;
	ioReq.ioBlks = 1;
	ioReq.ioBlkLen = ioReq.bufBlkLen;

	obsRead.ioReq = ioReq;

	// start the read
	msg = (nssmsg_t *) AllocMsg(sizeof(obs_read_t));
	msg->header.code = Start;
	*((obs_read_t *) msg->data) = obsRead;
	QSend(*info->readQ, msg);
}

void
Confirm(pol_t pol, obs_t *obs, sig_desc_t *sigDesc, confirm_info_t *info)
{
	FUNCTION("Confirm");
	int i;
#ifdef notdef
	CDDetector *detector;
	cd_detector_t detect;
#endif
	error_t err;
	nssmsg_t *msg;

	for (i = 0; i < channelData.channelBlks; i++) {
		msg = (nssmsg_t *) QReceive(*info->dataQ);
		switch (msg->header.code) {
		case BufferFull:
			ChanBuf->Free(msg->data);
			msg->data = 0;
			break;
		default:
			SysFatal(NSE_IMT, function, "type = %d", msg->header.code);
		}
		FreeMsg(msg);
	}
	
#ifdef notdef
	detect.pol = pol;
	detect.dataQ = *info->dataQ;
	detect.obs = *obs;

	detector = new CDDetector(&detect);
	if (err = (error_t) *detector)
		SysFatal(err, function, NULL);

	if (err = detector->Detect())
		SysFatal(err, function, NULL);
	delete detector;
#endif
}
