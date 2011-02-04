/*******************************************************************************

 File:    daddwrt.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/daddwrt.cc,v $
**
** Dadd write class
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/daddwrt.cc,v 1.1.1.1 2006/08/02 05:30:25 kes Exp $
* $Log: daddwrt.cc,v $
* Revision 1.1.1.1  2006/08/02 05:30:25  kes
* pdm-1_59
*
* Revision 1.1  2001/11/09 05:13:25  kes
* *** empty log message ***
*
Revision 1.1  98/10/29  11:10:57  kes
Initial revision
********************************************************************/

class DaddWrite {
private:
	pol_t pol;					// polarization
	queue_t **recvQ;			// ptr to receive queue
	queue_t **sendQ;			// ptr to send queue
	sem_t *sem;					// disk serialization semaphore
	obs_t obs;					// observation structure
	int fd;						// file descriptor
	uint32_t fileOfs;			// current file offset
	uint32_t blkLen;			// write block length
	uint32_t dataLen;			// total length of data written

public:
	DaddWrite(dadd_info_t *info);
	~DaddWrite();

	error_t Collect(obs_t *obsDef);
};

DaddWrite::DaddWrite(dadd_info_t *info)
{
	pol = info->pol;
	recvQ = info->recvQ;
	sendQ = info->sendQ;
	sem = info ->sem;
	bzero((void *) &obs, sizeof(obs));
	fd = fileOfs = blkLen = dataLen = 0;
}

~DaddWrite::DaddWrite()
{
}

