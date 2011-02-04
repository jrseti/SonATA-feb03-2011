/*******************************************************************************

 File:    detect.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/detect.cc,v $
**
** Signal detection task
**
** Signal detection is performed sequentially: DADD left pol, DADD right
** pol, PD, then confirmation.  Thus, a single large buffer can be used
** for the entire detection process, rather than separate buffers.
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/detect.cc,v 1.1.1.1 2006/08/02 05:30:25 kes Exp $
** $Log: detect.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:25  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:25  kes
** *** empty log message ***
**
Revision 1.1  98/10/29  11:11:09  kes
Initial revision
**********************************************************************/

#include "buffer.h"
#include "ddetector.h"
#include "detect.h"
#include "globals.h"
#include "nssmsg.h"
#include "obs.h"
#include "read.h"
#include "slice.h"
#include "syslog.h"

typedef struct slice_data {
	int slices;						// total # of slices in observation
	int spectra;					// total # of spectra in observation
	uint32_t width;					// width of a slice in bins
	uint32_t blks;					// # of blocks in a slice
	uint32_t blkSpectra;			// # of spectra in a block
	uint32_t blkSize;				// length of a slice block on disk
	uint32_t bufBlkLen;				// bytes between slice blocks in buffer
	uint32_t fileBlkLen;			// bytes between slice blocks on disk
} slice_data_t;

slice_data_t sliceData;
slice_t slice;

// function prototypes	
void StartDetection(obs_t *obs, detect_info_t *info);
void StartDaddRead(pol_t pol, obs_t *obs, detect_info_t *info);
void DetectDadd(pol_t pol, obs_t *obs, detect_info_t *info);

task_t
DetectionTask(void *arg)
{
	FUNCTION("DetectionTask");
	detect_info_t info;
	error_t err;
	nssmsg_t *msg, *msg1;
	obs_state_t state = Idle;
	obs_t obs;

	if (!arg)
		SysFatal(NSE_NTA, function, NULL);

	// get the initialization information
	info = *((detect_info_t *) arg);

	// run forever, waiting for messages
	while (1) {
		msg = (nssmsg_t *) QReceive(*info.recvQ);
		switch (msg->header.code) {
		case Start:
			if (state != Idle)
				SysFatal(NSE_OIP, function, NULL);
			obs = *((obs_t *) msg->data);
			state = Detecting;
			StartDetection(&obs, &info);
			msg1 = AllocMsg(0);
			msg1->header.code = Detecting;
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
StartDetection(obs_t *obs, detect_info_t *info)
{
	FUNCTION("StartDetect");
#ifdef notdef
	detector_t detect;
	DaddDetector *daddDetector;
	CDDetector *cdDetector;
#endif
	error_t err;
	nssmsg_t *msg;

	// perform left pol DADD
	// we have to start retrieval of slice data by the read task
	StartDaddRead(Left, obs, info);
	DetectDadd(Left, obs, info);

	StartDaddRead(Right, obs, info);
	DetectDadd(Right, obs, info);
}

void
StartDaddRead(pol_t pol, obs_t *obs, detect_info_t *info)
{
	FUNCTION("StartDaddRead");
	diskio_t ioReq;
	nssmsg_t *msg;
	obs_read_t obsRead;
	uint32_t spectrumBytes;

	// compute slice parameters
	switch (obs->cwRes) {
	case CW_1HZ:
		sliceData.spectra = obs->frames * 2;
		sliceData.width = 32768;
		spectrumBytes = obs->bins / 4;
		break;
	case CW_2HZ:
		sliceData.spectra = obs->frames * 4;
		sliceData.width = 16384;
		spectrumBytes = obs->bins / 8;
		break;
	case CW_4HZ:
		sliceData.spectra = obs->frames * 8;
		sliceData.width = 8192;
		spectrumBytes = obs->bins / 16;
		break;
	default:
		SysFatal(NSE_ICWR, function, "type = %d", obs->cwRes);
	}
	sliceData.slices = spectrumBytes / (sliceData.width / 4);
	sliceData.blkSpectra = DaddBuf->Size() / spectrumBytes;
	sliceData.blkSize = sliceData.blkSpectra * (sliceData.width / 4);
	sliceData.bufBlkLen = sliceData.blkSize;
	sliceData.blks = (sliceData.spectra + sliceData.blkSpectra / 2) / sliceData.blkSpectra;
	sliceData.fileBlkLen = sliceData.blkSpectra * spectrumBytes;
	
	// build the observation control block for the read task
	obsRead.buffer = SliceBuf;
	obsRead.dataQ = *info->dataQ;
	obsRead.blks = sliceData.slices;
#ifdef notdef
	obsRead.fd = obs->daddFd[pol];
	obsRead.sem = &DaddSem;
	obsRead.obs = *obs;
	obsRead.bufSize = DaddBuf->Size();
#endif

	ioReq.buf = 0;
	ioReq.bufOfs = 0;
	ioReq.bufBlkLen = sliceData.bufBlkLen;
	ioReq.fd = obs->daddFd[pol];
	ioReq.fileOfs = 0;
	ioReq.fileBlkLen = DaddBuf->Size();
	ioReq.sem = &DaddSem;
	ioReq.ioBlks = sliceData.blks;
	ioReq.ioBlkLen = sliceData.blkSize;

	obsRead.ioReq = ioReq;

	// start the read
	msg = (nssmsg_t *) AllocMsg(sizeof(obs_read_t));
	msg->header.code = Start;
	*((obs_read_t *) msg->data) = obsRead;
	QSend(*info->readQ, msg);
}

void
DetectDadd(pol_t pol, obs_t *obs, detect_info_t *info)
{
	FUNCTION("DetectDadd");
	int i;
	DaddDetector *detector;
	dadd_detector_t detect;
	error_t err;
	nssmsg_t *msg;

	detect.pol = pol;
	detect.dataQ = *info->dataQ;
	detect.obs = *obs;
	detect.slice.slices = sliceData.slices;
	detect.slice.spectra = sliceData.spectra;
	detect.slice.width = sliceData.width;

	detector = new DaddDetector(&detect);
	if (err = (error_t) *detector)
		SysFatal(err, function, NULL);

	if (err = detector->Detect())
		SysFatal(err, function, NULL);
	delete detector;
}
