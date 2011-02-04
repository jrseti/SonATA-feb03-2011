/*******************************************************************************

 File:    output.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tasks/output.cc,v $
**
** Input simulation task
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tasks/output.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: output.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:26:41  kes
** *** empty log message ***
**
***********************************************************************/

#include "input.h"

//
// OutputTask: simulate input from DSP
//
task_t
StoreTask(void *arg)
{
	FUNCTION("StoreTask");
	int i;
	unsigned char *buff;
	uint32_t ofs = 0, delay;
	error_t err;
	input_t input;
	bufOutput_t *outmsg;
	int nBufs;
	fuddmsg_t *msg;

	output = *((bufOutput *) arg);

	// wait for a write message
	while (1) {
		if (!(msg = QReceive(output.writeQ, &err, &tv))
			SysFatal(err, function, "receiving output buf msg");
		outmsg = msg->data;
		lseek(outmsg->fd, outmsg->ofs, 0);
		if (write(outmsg->fd, outmsg->buf, outmsg->length) != outmsg->length)
			SysFatal(errno, function, "output task");
	}
}