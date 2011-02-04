/*******************************************************************************

 File:    obs.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/obs.h,v $
**
** Observation header file
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/obs.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: obs.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:35  kes
** *** empty log message ***
**
 * Revision 1.2  98/10/29  11:33:43  kes
 * Removed obs_read_t.
 * 
 * Revision 1.1  98/10/27  08:18:35  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _OBS_H
#define _OBS_H

#include "nsstypes.h"

// a rudimentary observation control structure
// note the file descriptors; they travel with the observation: when
// it is doing data collection, they are write fds; when it is doing
// signal detection, they are read fds.
typedef struct obs {
	int32_t id;					// observation ID
	obs_state_t state;			// current observation state
	uint32_t frames;			// total # of frames in observation
	uint32_t bins;				// total # of bins per frame
	cw_resolution_t cwRes;		// CW resolution
	double centerFreq;			// center frequency
	int daddFd[2];				// dadd fds
	int cdFd[2];				// cd fds
} obs_t;

// structure to keep track of data collection
typedef struct obs_write {
	obs_state_t state;			// observation state
	uint32_t blkLen;			// write block length
	int fd;						// file descriptor
	uint32_t fileOfs;			// current offset in file
	uint32_t dataLen;			// total length of data
} obs_write_t;

#ifdef notdef
// structure to keep track of data retrieval
typedef struct obs_read {
	obs_state_t state;			// observation state
	uint32_t blkLen;			// write block length
	int fd;						// file descriptor
	uint32_t fileOfs;			// current offset in file
	uint32_t dataLen;			// total length of data
} obs_read_t;
#endif

#endif