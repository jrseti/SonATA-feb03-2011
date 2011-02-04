/*******************************************************************************

 File:    unpack.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/unpack/unpack.cc,v $
**
** DADD unpacking
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/unpack/unpack.cc,v 1.1.1.1 2006/08/02 05:30:39 kes Exp $
** $Log: unpack.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:39  kes
** pdm-1_59
**
** Revision 1.2  2001/11/09 06:09:58  kes
** Added include files
**
** Revision 1.1  2001/11/09 06:04:21  kes
** *** empty log message ***
**
Revision 1.1  98/10/12  03:14:52  kes
Initial revision

**********************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define XLSIZE		(65536)
#define SLICEWIDTH	12288
#define NSPECTRA	4096
#define NPOINTS		(SLICEWIDTH * NSPECTRA)

unsigned long long xlat[XLSIZE];
unsigned short (*packed)[SLICEWIDTH/8];
unsigned long long (*unpacked)[SLICEWIDTH/8];

// function prototypes
void unpack(unsigned short (*packed)[SLICEWIDTH/8],
		unsigned long long (*unpacked)[SLICEWIDTH/8], int rows, int cols,
		unsigned long long *xlat);


int
main(int argc, char **argv)
{
	unsigned char val[8];
	unsigned short *p;
	register int i, j, k;
	unsigned long long *up;
	struct timeb start, end;

	packed = new unsigned short[NSPECTRA][SLICEWIDTH/8];
	unpacked = new unsigned long long[NSPECTRA][SLICEWIDTH/8];

	// build the translation array
	up = (unsigned long long *) val;
	for (i = 0; i < XLSIZE; i++) {
		val[0] = i & 3;
		val[1] = (i >> 2) & 3;
		val[2] = (i >> 4) & 3;
		val[3] = (i >> 6) & 3;
		val[4] = (i >> 8) & 3;
		val[5] = (i >> 10) & 3;
		val[6] = (i >> 12) & 3;
		val[7] = (i >> 14) & 3;
		xlat[i] = *up;
	}

	// build the packed array
	for (i = 0; i < NSPECTRA; i++) {
		for (j = 0; j < SLICEWIDTH / 8; j++)
			packed[i][j] = (unsigned short) i;
	}

	// initialize the unpacked array
	for (i = 0; i < NSPECTRA; i++) {
		for (j = 0; j < SLICEWIDTH / 8; j++)
			unpacked[i][j] = (unsigned long long) -1;
	}

	ftime(&start);
#ifdef notdef
	// now unpack the array
	for (i = 0; i < NSPECTRA; i++) {
		for (j = 0; j < SLICEWIDTH / 8; j++)
			unpacked[i][j] = xlat[packed[i][j]];
	}
#else
	unpack(packed, unpacked, NSPECTRA, SLICEWIDTH, xlat);
#endif
	ftime(&end);
	double fstart = (double) start.time + (double) start.millitm / 1000;
	double fend = (double) end.time + (double) end.millitm / 1000;
	printf("dt = %lf\n", fend - fstart);
}

void unpack(unsigned short (*packed)[SLICEWIDTH/8],
		unsigned long long (*unpacked)[SLICEWIDTH/8], int rows, int cols,
		unsigned long long *xlat)
{
	register int i, j;

	// now unpack the array
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols / 8; j++)
			unpacked[i][j] = xlat[packed[i][j]];
	}
}
