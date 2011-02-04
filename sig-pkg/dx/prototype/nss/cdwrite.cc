/*******************************************************************************

 File:    cdwrite.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/cdwrite.cc,v $
**
** Task to write DADD data buffers to disk as they fill
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/cdwrite.cc,v 1.1.1.1 2006/08/02 05:30:24 kes Exp $
** $Log: cdwrite.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:24  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:24  kes
** *** empty log message ***
**
Revision 1.2  98/10/27  08:16:42  kes

Revision 1.1  98/10/26  04:30:49  kes
Initial revision

**********************************************************************/

#include <string.h>
#include "cdwrite.h"
#include "diskio.h"
#include "globals.h"
#include "obs.h"

// function prototypes
void StartCDObs(nssmsg_t *msg, cd_write_obs_t *obsInfo,
		obs_write_t *obs);
void WriteCDBuf(nssmsg_t *msg, obs_write_t *obs, cd_info_t *info);

task_t
CDWriteTask(void *arg)
{
	FUNCTION("CDWriteTask");
	cd_info_t info;
	error_t err;
	nssmsg_t *msg;
	cd_write_obs_t obsInfo;
	obs_write_t obs;

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
		case StartCollection:
			if (obs.state != Idle)
				SysFatal(NSE_OIP, function, NULL);
			obsInfo = *((cd_write_obs_t *) msg->data);
			// start the observation
			StartCDObs(msg, &obsInfo, &obs);
			break;
		case BufferFull:
			if (obs.state != Collect)
				SysFatal(NSE_NOIP, function, NULL);
			WriteCDBuf(msg, &obs, &info);
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
StartCDObs(nssmsg_t *msg, cd_write_obs_t *obsInfo, obs_write_t *obs)
{
	// initialize the observation structure
	// compute the block length; for testing, this is the length of
	// the buffer
	obs->blkLen = CDBuf->Size();

	// start at beginning of file
	obs->fd = obsInfo->fd;
	obs->fileOfs = 0;

	// # of bytes with overlap
	obs->dataLen = obsInfo->obs.frames * obsInfo->obs.bins;

	obs->state = Collect;
}

void
WriteCDBuf(nssmsg_t *msg, obs_write_t *obs, cd_info_t *info)
{
	diskio_t *ioReq;
	nssmsg_t *msg1;

	// allocate a disk I/O request structure
	ioReq = new diskio_t;
	bzero((char *) ioReq, sizeof(diskio_t));

	// initialize the structure
	ioReq->buf = (uint8_t *) msg->data;
	ioReq->bufBlkLen = 0;				// write as a single block
	ioReq->fd = obs->fd;
	ioReq->fileOfs = obs->fileOfs;
	ioReq->fileBlkLen = 0;				// write as a single block
	ioReq->sem = info->sem;
	ioReq->ioBlks = 1;					// write as a single block
	ioReq->ioBlkLen = obs->blkLen;

	// write the buffer
	WriteBuf(ioReq);
	delete ioReq;						// release the I/O request structure
	CDBuf->Free((uint8_t *) msg->data); // release the data buffer

	// bump to the next buffer location, check for done with obs
	obs->fileOfs += obs->blkLen;
	if (obs->fileOfs >= obs->dataLen) {
		obs->state = Idle;
		msg1 = AllocMsg(sizeof(obs_write_t));
		msg1->header.code = Collect;
		*((obs_write_t *) msg1->data) = *obs;
		QSend(*info->sendQ, msg1);
	}
}