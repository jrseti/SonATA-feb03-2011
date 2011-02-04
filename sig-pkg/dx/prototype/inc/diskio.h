/*******************************************************************************

 File:    diskio.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/diskio.h,v $
**
** Disk I/O header file
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/diskio.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: diskio.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:33  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:24  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _DISKIO_H
#define _DISKIO_H

#include <semaphore.h>
#include <unistd.h>
#include "nsstypes.h"

// disk task initialization structure
typedef struct diskInfo {
	int fd;						// file descriptor
	sem_t *sem;					// serialization semaphore
} diskInfo_t;

// structure defining a disk transfer.  There may be multiple
// blocks scattered in either memory or on disk, or both.
typedef struct diskio {
	uint8_t *buf;				// ptr to buffer
	uint32_t bufOfs;			// starting offset in buffer
	uint32_t bufBlkLen;			// length of a buffer block

	int fd;						// file descriptor
	uint32_t fileOfs;			// starting offset in file
	uint32_t fileBlkLen;		// length of a file block
	sem_t *sem;					// disk serialization semaphore

	uint32_t ioBlks;			// # of I/O blocks
	uint32_t ioBlkLen;			// length of an I/O block
} diskio_t;

// fuction prototypes
error_t WriteBuf(diskio_t *ioReq);
error_t ReadBuf(diskio_t *ioReq);

#endif /* _DISKIO_H */