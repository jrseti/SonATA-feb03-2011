/*******************************************************************************

 File:    collector.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/collector.cc,v $
**
** Collector class definition
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/collector.cc,v 1.1.1.1 2006/08/02 05:30:24 kes Exp $
** $Log: collector.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:24  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:24  kes
** *** empty log message ***
**
Revision 1.1  98/10/29  11:08:49  kes
Initial revision
**********************************************************************/

#include "collector.h"
#include "nssmsg.h"
#include "syslog.h"

// constructor
Collector::Collector(collector_t *collector)
{
	type = collector->type;
	fd = collector->fd;
	pol = collector->pol;
	dataQ = collector->dataQ;
	sem = collector->sem;
	buffer = collector->buffer;
	obs = collector->obs;
	err = 0;

	blkLen = buffer->Size();

	// compute the total # of bytes in the observation
	switch (type) {
	case Dadd:
		dataLen = obs.frames * 2 * (obs.bins / 4);
		break;
	case CD:
		dataLen = obs.frames * obs.bins;
		break;
	}
}

// destructor
Collector::~Collector()
{
}

// perform data collection
error_t
Collector::Collect()
{
	FUNCTION("Collector::Collect");
	diskio_t ioReq;
	nssmsg_t *msg;

	fileOfs = 0;
	while (fileOfs < dataLen) {
		msg = (nssmsg_t *) QReceive(dataQ);
		switch (msg->header.code) {
		case BufferFull:
			ioReq.buf = (uint8_t *) msg->data;
			ioReq.bufBlkLen = 0;
			ioReq.fd = fd;
			ioReq.fileOfs = fileOfs;
			ioReq.fileBlkLen = 0;
			ioReq.sem = sem;
			ioReq.ioBlks = 1;
			ioReq.ioBlkLen = blkLen;
			WriteBuf(&ioReq);
			buffer->Free(msg->data);
			msg->data = 0;
			fileOfs += blkLen;
			break;
		default:
			SysFatal(NSE_IMT, function, "type = %d", msg->header.code);
		}
		FreeMsg(msg);
	}
	return (0);
}

//
// WriteBuf: write a memory buffer to disk
//
// Synopsis:
//		err = WriteBuf(ioReq);
//		error_t err;			return code
//		diskio_t *ioReq;		I/O request block
// Returns:
//		0 if successful.
//		error code if an error occurred.
// Description:
//		Writes a buffer to disk, with scatter-gather for buffer,
//		disk, or both.
//
error_t
Collector::WriteBuf(diskio_t *ioReq)
{
	FUNCTION("Collector::WriteBuf");
	int i, fd;
	uint32_t bufOfs, fileOfs, count;

	fd = ioReq->fd;
	bufOfs = ioReq->bufOfs;
	fileOfs = ioReq->fileOfs;
	
	// gain control of the disk
	sem_wait(ioReq->sem);

	for (i = 0; i < ioReq->ioBlks; i++) {
		lseek(fd, fileOfs, 0);
#ifndef notdef
		if ((count = write(fd, &ioReq->buf[bufOfs], ioReq->ioBlkLen)) != ioReq->ioBlkLen)
			SysFatal(errno, function, "writing %d bytes", ioReq->ioBlkLen);
#else
		write(fd, &ioReq->buf[bufOfs], ioReq->ioBlkLen);
#endif
		bufOfs += ioReq->bufBlkLen;
		fileOfs += ioReq->fileBlkLen;
	}

	// release the disk
	sem_post(ioReq->sem);
	return (0);
}

