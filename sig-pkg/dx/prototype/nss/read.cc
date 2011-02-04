/*******************************************************************************

 File:    read.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/read.cc,v $ 
**
** Read tasks
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/read.cc,v 1.1.1.1 2006/08/02 05:30:23 kes Exp $
** $Log: read.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:23  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:26  kes
** *** empty log message ***
**
Revision 1.1  98/10/29  11:12:20  kes
Initial revision
**********************************************************************/

#include "read.h"
#include "nssmsg.h"
#include "syslog.h"
#include "util.h"

// function prototypes
error_t ReadBuf(diskio_t *ioReq);

//
// ReadTask: handle read operations
//
// Synopsis:
//		task_t ReadTask(info);
//		read_info_t *info;
// Returns:
//		A task, never returns.
// Description:
//		Waits for read requests, then performs them.
// Notes:
//		A serialization semaphore in the I/O request avoids simultaneous
//		access by readers and writers.
//		This is a very low-level task, which simply reads the data
//		requested.
//
task_t
ReadTask(void *arg)
{
	FUNCTION("ReadTask");
	int i;
	diskio_t ioReq;
	error_t err;
	nssmsg_t *msg, *msg1, *msg2;
	read_info_t info;
	obs_read_t obs;

	if (!arg)
		SysFatal(NSE_NMP, function, NULL);

	info = *((read_info_t *) arg);

	while (1) {
		msg = (nssmsg_t *) QReceive(*info.recvQ);
		switch (msg->header.code) {
		case Start:
			obs = *((obs_read_t *) msg->data);
			for (i = 0; i < obs.blks; i++) {
				ioReq = obs.ioReq;
				ioReq.fileOfs = i * ioReq.bufBlkLen;
				ioReq.buf = (uint8_t *) obs.buffer->AllocWait();
				ReadBuf(&ioReq);
				msg1 = AllocMsg(0);
				msg1->header.code = BufferFull;
				msg1->data = ioReq.buf;
				QSend(obs.dataQ, msg1);
			}
			msg2 = AllocMsg(0);
			msg2->header.code = ReadDone;
			QSend(*info.sendQ, msg2);
			break;
		case Shutdown:
			TaskSuicide();
			break;
		default:
			SysFatal(NSE_IMT, function, "type = %d", msg->header.code);
		}
		FreeMsg(msg);
	}
}

//
// ReadBuf: fill a memory buffer from disk
//
// Synopsis:
//		err = ReadBuf(ioReq);
//		error_t err;			return code
//		diskio_t *ioReq;		I/O request block
// Returns:
//		0 if successful.
//		error code if an error occurred.
// Description:
//		Fills a buffer from disk, with scatter-gather for buffer,
//		disk, or both.
//
error_t
ReadBuf(diskio_t *ioReq)
{
	FUNCTION("ReadBuf");
	int i;
	uint32_t bufOfs, fileOfs;

	bufOfs = ioReq->bufOfs;
	fileOfs = ioReq->fileOfs;
	
	// gain control of the disk
	sem_wait(ioReq->sem);

	for (i = 0; i < ioReq->ioBlks; i++) {
		lseek(ioReq->fd, fileOfs, 0);
		if (read(ioReq->fd, &ioReq->buf[bufOfs], ioReq->ioBlkLen)
				!= ioReq->ioBlkLen) {
			SysFatal(errno, function, "reading %d bytes", ioReq->ioBlkLen);
		}
		bufOfs += ioReq->bufBlkLen;
		fileOfs += ioReq->fileBlkLen;
	}

	// release the disk
	sem_post(ioReq->sem);
	return (0);
}