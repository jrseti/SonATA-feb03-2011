/*******************************************************************************

 File:    cdetector.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/cdetector.cc,v $
**
** Confirmation detector class definition
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/cdetector.cc,v 1.1.1.1 2006/08/02 05:30:23 kes Exp $
** $Log: cdetector.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:23  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:23  kes
** *** empty log message ***
**
**********************************************************************/

#include <string.h>
#include "cdetector.h"
#include "nssmsg.h"
#include "syslog.h"
#include "unpack.h"

// constructor
CDDetector::CDDetector(cd_detector_t *detector)
{
	pol = detector->pol;
	curSlice = 0;
	dataQ = detector->dataQ;
	obs = detector->obs;
	slice = detector->slice;
	err = 0;

	overlapBins = slice.spectra;
	procWidth = slice.width + slice.spectra;
}

// destructor
DaddDetector::~DaddDetector()
{
}

// perform data collection
error_t
CDDetector::Detect()
{
	FUNCTION("CDDetector::Detect");
	nssmsg_t *msg;

	curSlice = 0;
	sliceBuf0 = sliceBuf1 = 0;
	procBuf = (uint64_t *) ProcBuf->Alloc();
	procBufSize = ProcBuf->Size();

	// process N+1 slices due to overlap
	for (curSlice = 0; curSlice < slice.slices; curSlice++) {
		msg = (nssmsg_t *) QReceive(dataQ);
		switch (msg->header.code) {
		case BufferFull:
			sliceBuf0 = sliceBuf1;
			sliceBuf1 = (uint16_t *) msg->data;
			ProcessSlice();
			if (sliceBuf0)
				SliceBuf->Free(sliceBuf0);
			break;
		default:
			SysFatal(NSE_IMT, function, "type = %d", msg->header.code);
		}
		msg->data = 0;
		FreeMsg(msg);
	}
	if (sliceBuf1)
		SliceBuf->Free(sliceBuf1);
	ProcBuf->Free((void *) procBuf);
	return (0);
}

void
CDDetector::ProcessSignal()
{
	int i;

	// do both positive and negative slopes
	for (i = 0; i < 2; i++) {
		// unpack the left edge
		if (curSlice) {
			unpack(sliceBuf0 + overlapBins / 8, procBuf, slice.spectra,
					overlapBins, slice.width, procWidth, DaddTable);
		}
		else
			bzero(procBuf, procBufSize);
		unpack(sliceBuf1, procBuf + overlapBins / 8, slice.spectra,
				slice.width, slice.width, procWidth, DaddTable);
	}
}