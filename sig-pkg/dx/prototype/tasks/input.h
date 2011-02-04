/*******************************************************************************

 File:    input.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tasks/input.h,v $
**
** buffer definitions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tasks/input.h,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: input.h,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:26:41  kes
** *** empty log message ***
**
***********************************************************************/

#ifndef _INPUT_H
#define _INPUT_H

typedef struct bufInput {
	pol_t pol;					// polarization
	int fd;						// device to write to
	queue_t *bufQ;				// buffer queue
	queue_t *writeQ;			// buffer write message queue
	uint32_t bufLength;			// size of buffer in bytes
	uint32_t bufRate;			// rate at which the buffer fills (bytes/sec)
	uint32_t obsLength;			// length of observation in bytes
} bufInput_t;

// storage structure
typedef struct storage {
	pol_t pol;
	int fd;
	unsigned char *buf;
	uint32_t ofs;
	uint32_t length;
} storage_t;

// retrieval structure
typedef struct retrieval {
	pol_t pol;					// polarization
	unsigned char *buf;			// memory buffer to fill
	int fd;						// device to read from
	uint32_t startOfs;			// initial offset
	uint32_t blockOfs;			// block offset on disk
	int nBlocks;				// total number of blocks to read
	uint32_t blockLength;		// length of read blocks
} retrieval_t;

#endif