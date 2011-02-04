/*******************************************************************************

 File:    ddetector.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/ddetector.h,v $
**
** Dadd detector class definition
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/ddetector.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: ddetector.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:33  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/29  11:30:49  kes
 * Initial revision
 * 
***********************************************************************/

#ifndef _DDETECTOR_H
#define _DDETECTOR_H

#include "nss.h"
#include "buffer.h"
#include "globals.h"
#include "obs.h"
#include "queue.h"
#include "slice.h"

typedef struct dadd_detector {
	pol_t pol;
	queue_t *dataQ;
	obs_t obs;
	slice_t slice;
} dadd_detector_t;

class DaddDetector {
private:
	error_t err;
	pol_t pol;
	queue_t *dataQ;
	void *buf;
	Buffer *buffer;
	obs_t obs;
	slice_t slice;

	int curSlice;
	uint16_t *sliceBuf0, *sliceBuf1;
	uint32_t overlapBins, procWidth, procBufSize;
	uint64_t *procBuf;

	void ProcessSlice();

public:
	DaddDetector(dadd_detector_t *detector);
	~DaddDetector();
	
	error_t Detect();

	operator error_t() { return (err); }
};

#endif
