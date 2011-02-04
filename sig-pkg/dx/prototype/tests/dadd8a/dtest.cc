/*******************************************************************************

 File:    dtest.cc
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

/**************************************************************************
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/dadd8a/dtest.cc,v $
**
** Generalized DADD test program
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/dadd8a/dtest.cc,v 1.1.1.1 2006/08/02 05:30:36 kes Exp $
** $Log: dtest.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:36  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:44:21  kes
** *** empty log message ***
**
Revision 1.2  98/11/12  09:02:16  kes
Changed slice length, added tests.

Revision 1.1  98/11/10  14:32:43  kes
Initial revision

**************************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include "dadd.h"
#include "daddshift.h"
#include "daddutil.h"
#include "pairsum.h"
#include "threshold.h"

#define SPECTRA			4096
#define BINS			8192
#ifndef notdef
#define SLICE			(BINS + ((SPECTRA + 7) / 8) * 8)
#else
#define SLICE			(BINS)
#endif
#define ALLOCSLICE		(((SLICE + 7) / 8) * 8)
#define CACHE_ROWS		8

uint8_t (*test)[ALLOCSLICE];
uint8_t (*test1)[ALLOCSLICE];
int CacheRows = CACHE_ROWS;

extern "C" {
	extern void mmxmov(uint8_t *src, uint8_t *dest, uint32_t len);
	extern void mmxrd(uint8_t *src, uint8_t *dest, uint32_t len);
	extern void mmxwrt(uint8_t *src, uint8_t *dest, uint32_t len);
	extern void lwmov(uint8_t *src, uint8_t *dest, uint32_t len);
	extern void lwrd(uint8_t *src, uint8_t *dest, uint32_t len);
	extern void lwwrt(uint8_t *src, uint8_t *dest, uint32_t len);
};

int
main(int argc, char **argv)
{
	int i, j, bins;
	double fstart, fend;
	struct timeval start, end;

	// allocate and build the test array
	test = new uint8_t[SPECTRA+1][ALLOCSLICE];

#ifndef notdef
	if (((unsigned long) test) & 0x7) {
		printf("nonaligned array, aligning\n");
		test = (uint8_t (*)[ALLOCSLICE]) (((unsigned long) test + 8) & ~7);
	}
#endif

	// allocate and build the test array
	test1 = new uint8_t[SPECTRA+1][ALLOCSLICE];
	if (((unsigned long) test1) & 0x7)
		test1 = (uint8_t (*)[ALLOCSLICE]) (((unsigned long) test1 + 8) & ~7);


	printf("starting MMX move\n");
	gettimeofday(&start, NULL);
	mmxmov((uint8_t *) test, (uint8_t *) test1, SPECTRA * ALLOCSLICE);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("MMX move time for %d bytes = %.3f\n", SPECTRA * ALLOCSLICE,
			fend - fstart);

	printf("starting MMX move (src=dest)\n");
	gettimeofday(&start, NULL);
	mmxmov((uint8_t *) test, (uint8_t *) test, SPECTRA * ALLOCSLICE);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("MMX move time for %d bytes = %.3f\n", SPECTRA * ALLOCSLICE,
			fend - fstart);

	printf("starting MMX read\n");
	gettimeofday(&start, NULL);
	mmxrd((uint8_t *) test, (uint8_t *) test1, SPECTRA * ALLOCSLICE);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("MMX read time for %d bytes = %.3f\n", SPECTRA * ALLOCSLICE,
			fend - fstart);

	printf("starting MMX write\n");
	gettimeofday(&start, NULL);
	mmxwrt((uint8_t *) test, (uint8_t *) test1, SPECTRA * ALLOCSLICE);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("MMX write time for %d bytes = %.3f\n", SPECTRA * ALLOCSLICE,
			fend - fstart);

	printf("starting LW move\n");
	gettimeofday(&start, NULL);
	lwmov((uint8_t *) test, (uint8_t *) test1, SPECTRA * ALLOCSLICE);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("LW mov time for %d bytes = %.3f\n", SPECTRA * ALLOCSLICE,
			fend - fstart);

	printf("starting LW move (src=dest)\n");
	gettimeofday(&start, NULL);
	lwmov((uint8_t *) test, (uint8_t *) test, SPECTRA * ALLOCSLICE);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("LW mov time for %d bytes = %.3f\n", SPECTRA * ALLOCSLICE,
			fend - fstart);

	printf("starting LW read\n");
	gettimeofday(&start, NULL);
	lwrd((uint8_t *) test, (uint8_t *) test1, SPECTRA * ALLOCSLICE);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("LW read time for %d bytes = %.3f\n", SPECTRA * ALLOCSLICE,
			fend - fstart);

	printf("starting LW write\n");
	gettimeofday(&start, NULL);
	lwwrt((uint8_t *) test, (uint8_t *) test1, SPECTRA * ALLOCSLICE);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("LW wrt time for %d bytes = %.3f\n", SPECTRA * ALLOCSLICE,
			fend - fstart);

	for (i = 0; i < SPECTRA; i++) {
		for (j = 0; j < SLICE; j++)
			test[i][j] = 1;
	}

	printf("starting\n");
	gettimeofday(&start, NULL);
	for (i = 0; i < 1; i++)
		TopDownDadd(SPECTRA, SLICE, (uint8_t *) test);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("TopDownDadd time for %d spectra = %.3f\n", SPECTRA, fend - fstart);

	for (i = 0; i < SPECTRA; i++) {
		for (j = 0; j < SLICE; j++)
			test[i][j] = 1;
	}

	printf("starting\n");
	gettimeofday(&start, NULL);
	for (i = 0; i < 1; i++)
		DaddDivide(SPECTRA, SLICE, (uint8_t *) test, CacheRows);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("DaddDivide time for %d spectra = %.3f\n", SPECTRA, fend - fstart);

	gettimeofday(&start, NULL);
	Threshold(SPECTRA, SLICE, SLICE, (uint8_t *) test, 0x00);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("threshold time for %d spectra = %.3f\n", SPECTRA, fend - fstart);

	gettimeofday(&start, NULL);
	DaddShift(1, (uint32_t) SPECTRA, (uint32_t) SLICE, (uint32_t) ALLOCSLICE,
			(uint8_t *) test);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("shift time for %d spectra = %.3f\n", SPECTRA, fend - fstart);

	gettimeofday(&start, NULL);
	for (i = 0; i < 1024; i++) {
		DaddShift(1, 1, (uint32_t) SLICE, (uint32_t) ALLOCSLICE,
				(uint8_t *) test);
	}
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("shift time for %d spectra = %.3f\n", SPECTRA, fend - fstart);
}