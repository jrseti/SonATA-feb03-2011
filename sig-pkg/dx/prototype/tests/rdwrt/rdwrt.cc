/*******************************************************************************

 File:    rdwrt.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/rdwrt/rdwrt.cc,v $
**
** Read/write tests
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/rdwrt/rdwrt.cc,v 1.1.1.1 2006/08/02 05:30:34 kes Exp $
** $Log: rdwrt.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:34  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 06:02:18  kes
** *** empty log message ***
**
Revision 1.1  98/10/23  03:53:22  kes
Initial revision

**********************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define SPECTRUM_BINS	(4096*1024)
#define BUFF_SPECTRA	16
#define SPECTRA			1024
#define BUFFSIZE		(BUFF_SPECTRA * SPECTRUM_BINS / 4)

unsigned char *lBuff0;
unsigned char *lBuff1;
unsigned char *rBuff0;
unsigned char *rBuff1;

int
main(int argc, char **argv)
{
	int i, lFd0, lFd1, rFd0, rFd1;
	unsigned long lOfs0, rOfs0, lOfs1, rOfs1;
	double fstart, fend;
	struct timeval start, end;

	lOfs0 = rOfs0 = lOfs1 = rOfs1 = 0;

	lBuff0 = new unsigned char[BUFFSIZE];
	lBuff1 = new unsigned char[BUFFSIZE];
	rBuff0 = new unsigned char[BUFFSIZE];
	rBuff1 = new unsigned char[BUFFSIZE];
	
	lFd0 = open("/dev/rsd2940.2b", O_RDWR);
	rFd0 = open("/dev/rsd2940.2c", O_RDWR);
	lFd1 = open("/dev/rsd2940.1b", O_RDWR);
	rFd1 = open("/dev/rsd2940.1c", O_RDWR);

	// initialize the buffers
	for (i = 0; i < BUFFSIZE; i++) {
		lBuff0[i] = (unsigned char) i;
		rBuff0[i] = (unsigned char) i;
	}

	// time the write operation
	gettimeofday(&start, NULL);
	for (i = 0; i < SPECTRA; i += BUFF_SPECTRA) {
		lseek(lFd0, lOfs0, 0);
		write(lFd0, lBuff0, BUFFSIZE);
		lOfs0 += BUFFSIZE;
		lseek(rFd0, rOfs0, 0);
		write(rFd0, rBuff0, BUFFSIZE);
		rOfs0 += BUFFSIZE;
	}
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("sd2 time to write %d spectra (%lu bytes) = %.3lf\n", i,
			lOfs0 + rOfs0, fend - fstart);

	// time the read operation
	lOfs0 = rOfs0 = 0;
	gettimeofday(&start, NULL);
	for (i = 0; i < SPECTRA; i += BUFF_SPECTRA) {
		lseek(lFd0, lOfs0, 0);
		read(lFd0, lBuff1, BUFFSIZE);
		lOfs0 += BUFFSIZE;
		lseek(rFd0, rOfs0, 0);
		read(rFd0, rBuff1, BUFFSIZE);
		rOfs0 += BUFFSIZE;
	}
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("sd2 time to read %d spectra (%lu bytes) = %.3lf\n", i,
			lOfs0 + rOfs0, fend - fstart);

	// time the write operation for other drive
	gettimeofday(&start, NULL);
	for (i = 0; i < SPECTRA; i += BUFF_SPECTRA) {
		lseek(lFd1, lOfs1, 0);
		write(lFd1, lBuff1, BUFFSIZE);
		lOfs1 += BUFFSIZE;
		lseek(rFd1, rOfs1, 0);
		write(rFd1, rBuff1, BUFFSIZE);
		rOfs1 += BUFFSIZE;
	}
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("sd1 time to write %d spectra (%lu bytes) = %.3lf\n", i,
			lOfs1 + rOfs1, fend - fstart);

	// time the read operation
	lOfs1 = rOfs1 = 0;
	gettimeofday(&start, NULL);
	for (i = 0; i < SPECTRA; i += BUFF_SPECTRA) {
		lseek(lFd1, lOfs1, 0);
		read(lFd1, lBuff1, BUFFSIZE);
		lOfs1 += BUFFSIZE;
		lseek(rFd1, rOfs1, 0);
		read(rFd1, rBuff1, BUFFSIZE);
		rOfs1 += BUFFSIZE;
	}
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("sd1 time to read %d spectra (%lu bytes) = %.3lf\n", i,
			lOfs1 + rOfs1, fend - fstart);
}		
