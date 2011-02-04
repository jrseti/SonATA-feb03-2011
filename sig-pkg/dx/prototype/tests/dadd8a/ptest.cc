/*******************************************************************************

 File:    ptest.cc
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

#include <stdio.h>
#include <sys/time.h>

#define BINS		33792

extern "C" {
	TopDownPairSum(int drift, int bins, unsigned char *lower, unsigned char *upper,
			int shift);
	DaddShift(int shift, unsigned long bins, unsigned char *data);
};

#ifdef notdef
unsigned char *lower, *upper;
#else
unsigned char lower[BINS];
unsigned char upper[BINS];
#endif

int
main(int argc, char **argv)
{
	int i, drift;
	struct timeval start, end;
	double fstart, fend;

#ifdef notdef
	lower = new unsigned char[BINS+1];
	upper = new unsigned char[BINS+1];
#endif

	if ((((unsigned long) lower) & 0x7) || (((unsigned long) upper) & 0x7))
		printf("nonaligned array(s)\n");

#ifdef notdef
    gettimeofday(&start, NULL);
	for (i = 0; i < 10240; i++) {
		DaddShift(2, 32768, lower);
		DaddShift(2, 32768, upper);
	}
    gettimeofday(&end, NULL);
   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("time to shift %d rows = %.3lf\n", 20480, fend - fstart);
#endif

	// initialize the lower and upper arrays
	for (i = 0; i < BINS; i++) {
		lower[i] = i & 0x7f;
		upper[i] = (i + 1) & 0x7f;
	}

	TopDownPairSum(0, BINS, lower, upper, 0);

	// initialize the lower and upper arrays
	for (i = 0; i < BINS; i++) {
		lower[i] = i & 0x7f;
		upper[i] = (i + 1) & 0x7f;
	}

	TopDownPairSum(0, BINS, lower, upper, 1);

	// initialize the lower and upper arrays
	for (i = 0; i < BINS; i++) {
		lower[i] = i & 0x7f;
		upper[i] = i & 0x7f;
	}

	TopDownPairSum(1, BINS, lower, upper, 0);

	// initialize the lower and upper arrays
	for (i = 0; i < BINS; i++) {
		lower[i] = i & 0x7f;
		upper[i] = i & 0x7f;
	}

	TopDownPairSum(7, BINS, lower, upper, 0);

    gettimeofday(&start, NULL);
	for (i = 0; i < 10240; i++)
		TopDownPairSum(1, BINS, lower, upper, 0);
    gettimeofday(&end, NULL);
   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("time for %d spectra = %.3lf\n", 1024, fend - fstart);
}