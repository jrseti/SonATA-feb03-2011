/*******************************************************************************

 File:    collector.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/collector.h,v $
**
** Collector class definition
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/collector.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: collector.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:31  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/29  11:30:07  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _COLLECTOR_H
#define _COLLECTOR_H

#include "nss.h"
#include "buffer.h"
#include "diskio.h"
#include "queue.h"
#include "obs.h"

typedef struct collector {
	collector_type_t type;
	pol_t pol;
	queue_t *dataQ;
	int fd;
	sem_t *sem;
	Buffer *buffer;
	obs_t obs;
} collector_t;

class Collector {
private:
	collector_type_t type;
	error_t err;
	int fd;
	obs_t obs;
	pol_t pol;
	queue_t *dataQ;
	sem_t *sem;
	uint32_t blkLen;
	uint32_t fileOfs;
	uint32_t dataLen;
	void *buf;
	Buffer *buffer;

	WriteBuf(diskio_t *ioReq);

public:
	Collector(collector_t *collector);
	~Collector();
	
	error_t Collect();

	operator error_t() { return (err); }
};

#endif