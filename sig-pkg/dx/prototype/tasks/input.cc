/*******************************************************************************

 File:    input.cc
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

/***********************************************************************
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tasks/input.cc,v $
**
** Input simulation task
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tasks/input.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: input.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:26:41  kes
** *** empty log message ***
**
***********************************************************************/

#include "input.h"

//
// InputTask: simulate input from DSP
//
task_t
InputTask(void *arg)
{
	FUNCTION("InputTask");
	int i, nBufs;
	unsigned char *buff;
	uint32_t ofs = 0, delay;
	error_t err;
	fuddmsg_t *msg;
	input_t input;
	output_t output;

	input = *((bufInput_t *) arg);

	// compute # of buffers to write
	nBufs = (input.obsLength + input.bufLength / 2) / input.bufLength;

	// compute the time to fill a buffer
	delay = input.bufLength / input.bufRate;

	// process all the buffers in the observation
	for (i = 0; i < nBufs; i++) {
		// make sure a buffer is available
		if (!(buff = QReceive(bufQ, &err, &tv))
			SysFatal(err, function, "accessing buffer");

		// wait for filling of the buffer
		sleep(delay);

		// send a message to indicate the buffer is full and
		// must be written
		msg = AllocMsg(0);
		msg->header.code = 1;
		msg->data = ofs;
		if (err = QSend(queue, msg, &tv))
			SysFatal(err, function, "writing to queue");
		ofs += input.bufLength;
		FreeMsg(msg);
	}
	// allocate a message to signal done with data collection
	msg = AllocMsg(0);
	msg->header.code = 2;
	msg->data = (uint32_t) pol;
	QSend(input.doneQ, msg, &tv);
	TaskSuicide();
}