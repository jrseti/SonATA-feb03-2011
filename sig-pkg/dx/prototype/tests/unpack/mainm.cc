/*******************************************************************************

 File:    mainm.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/unpack/mainm.cc,v $
**
** DADD unpacking
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/unpack/mainm.cc,v 1.1.1.1 2006/08/02 05:30:39 kes Exp $
** $Log: mainm.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:39  kes
** pdm-1_59
**
** Revision 1.2  2001/11/09 06:09:58  kes
** Added include files
**
** Revision 1.1  2001/11/09 06:04:21  kes
** *** empty log message ***
**
** Revision 1.2  1998/10/14 04:15:04  kes
** Added memory B/W test.  Working version of partial slice load.
**
** Revision 1.1  1998/10/13 15:57:40  kes
** Initial revision
**
Revision 1.1  98/10/12  01:21:08  kes
Initial revision

**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define XLSIZE		65536
#define SLICEBINS	8192
#define DADDBINS	(SLICEBINS + SPECTRA)
#define SPECTRA		4096

int spectra, sliceBins, daddBins, overlapBins;
unsigned char (*dadd)[DADDBINS];
unsigned short (*slice0)[SLICEBINS/8];
unsigned short (*slice1)[SLICEBINS/8];
unsigned long long xlat[XLSIZE];

// function prototypes
extern "C" {
void movquad(long long *src, long long *dest, int n);
void unpack(unsigned short (*slice)[SLICEBINS/8],
		unsigned char (*dadd)[DADDBINS], int rows, int bins,
		int srcbins, int destbins, unsigned long long *xlat);
}

int
main(int argc, char **argv)
{
	unsigned char val[8];
	unsigned short *p;
	register int i, j, k;
	unsigned long long *up;
	double fstart, fend, dt;
	struct timeb start, end;

	slice0 = new unsigned short[SPECTRA][SLICEBINS/8];
	slice1 = new unsigned short[SPECTRA][SLICEBINS/8];
	dadd = new unsigned char[SPECTRA][DADDBINS];

	spectra = SPECTRA;
	sliceBins = SLICEBINS;
	daddBins = DADDBINS;
	overlapBins = daddBins - sliceBins;

	// build the slice arrays
	for (i = 0; i < spectra; i++) {
		for (j = 0; j < sliceBins / 2; j++)
			slice0[i][j] = rand();
	}
	for (i = 0; i < spectra; i++) {
		for (j = 0; j < sliceBins / 2; j++)
			slice1[i][j] = rand();
	}

#ifdef MEMORY_BW
	// see what the memory bandwidth is
	int n = spectra * sliceBins / 32;
	long long *src = (long long *) slice0;
	long long *dest = (long long *) slice1;

	ftime(&start);
	for (i = 0; i < 100; i++)
		movquad(src, dest, n);
	ftime(&end);
	double mb = ((double) i * sizeof(short) * spectra * sliceBins / 8) / 1e6;
	fstart = (double) start.time + (double) start.millitm / 1000;
	fend = (double) end.time + (double) end.millitm / 1000;
	printf("dt = %lf, bw = %le MB/s\n", fend - fstart, 2 *mb / (fend - fstart));

#endif
	
	// initialize the dadd array
	for (i = 0; i < spectra; i++) {
		for (j = 0; j < daddBins; j++)
			dadd[i][j] = (unsigned char) -1;
	}

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

	ftime(&start);
#ifndef notdef
	unpack((unsigned short (*)[SLICEBINS/8]) &slice0[0][overlapBins/8], dadd,
			spectra, overlapBins, sliceBins, daddBins, xlat);
	unpack(slice1, (unsigned char (*)[DADDBINS]) &dadd[0][overlapBins],
			spectra, sliceBins, sliceBins, daddBins, xlat);
#else
	unpack((unsigned short (*)[SLICEBINS/8]) &slice0[0][0], dadd,
			spectra, daddBins, sliceBins, daddBins, xlat);
#endif
	ftime(&end);
	fstart = (double) start.time + (double) start.millitm / 1000;
	fend = (double) end.time + (double) end.millitm / 1000;
	printf("%d spectra, %d bins, %d overlap, dt = %lf\n", spectra, daddBins,
			overlapBins, fend - fstart);
}