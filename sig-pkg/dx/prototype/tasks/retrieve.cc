/*******************************************************************************

 File:    retrieve.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tasks/retrieve.cc,v $
**
** Retrieval simulation task
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tasks/retrieve.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: retrieve.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:26:41  kes
** *** empty log message ***
**
***********************************************************************/

#include "nsstypes.h"

//
// RetrievalTask: simulate retrieval from disk
//
task_t
RetrievalTask(void *arg)
{
	FUNCTION("RetrievalTask");
	unsigned char *buf;
	int i;
	uint32_t ofs;
	fuddmsg_t *msg;
	retrieval_t *retrieval;

	
	while (1) {
		// receive a storage message
		if (!(msg = QReceive(retrieval.queue, &err, &tv)))
			SysFatal(err, function, "retrieving");

		retrieval = (retrieval_t *) msg->data;
		buf = retrieval->buf;
		ofs = retrieval->startOfs;
		for (i = 0; i < retrieval->nBlocks; i++) {
			lseek(retrieval->fd, ofs, 0);
			write(retrieval->fd, retrieval->buf, retrieval->blockLength);
			buf += retrieval->blockLength;
			ofs += retrieval->blockOfs;
		}
		FreeMsg(msg);
	}
}