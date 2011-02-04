/*******************************************************************************

 File:    chan1Hz.cpp
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

//
// convert the confirmation channel to 1Hz bins
//
// $Header: /home/cvs/nss/sonata-pkg/dx/tests/confirmation/chan1Hz.cpp,v 1.1.1.1 2006/08/02 05:29:08 kes Exp $
// $Log: chan1Hz.cpp,v $
// Revision 1.1.1.1  2006/08/02 05:29:08  kes
// pdm-1_59
//
// Revision 1.1  2002/06/13 04:57:20  kes
// *** empty log message ***
//
//
#include <complex>
#include <stdio.h>
#include <stream.h>
#include "../../third-party/fftw/include/fftw.h"

char infile[128], outfile[128];
int subbands = 16, halfFrames = 33;

// function prototypes
int getArgs(int argc, char **argv);

int
main(int argc, char **argv)
{
	int sample, samples, spectrum, spectra, bin, bins, dummy;
	float re, im, binPower;
	fftw_plan sigPlan;
	FILE *iFp, *oFp;

	if (getArgs(argc, argv))
		exit (1);

	if (!(iFp = fopen(infile, "r"))) {
		cout << " opening input file" << endl;
		exit(2);
	}

	if (!(oFp = fopen(outfile, "w"))) {
		cout << " opening output file" << endl;
		exit(3);
	}

	samples = subbands * halfFrames * 512;
	bins = 2 * subbands * 512;
	spectra = samples / bins;

	sigPlan = fftw_create_plan(bins, FFTW_FORWARD, FFTW_ESTIMATE);

	double power[bins];
	float_complex td[samples];
	float_complex fd[samples];

	// extract the samples from the channel file
	for (sample = 0; sample < samples; sample++) {
		fscanf(iFp, "%d (%f, %f)\n", &dummy, &re, &im);
		td[sample] = float_complex(re, im);
	}

	// now perform a full-width fft to create the signal bins
	fftw(sigPlan, spectra, (fftw_complex *) td, 1, bins, (fftw_complex *) fd,
			1, bins);

	// rearrange the data so that DC is in the middle of the spectrum
	float_complex temp[bins/2];

	for (spectrum = 0; spectrum < spectra; spectrum++) {
		memcpy(temp, &fd[spectrum*bins], sizeof(float_complex) * bins / 2);
		memcpy(&fd[spectrum*bins], &fd[spectrum*bins+bins/2],
				sizeof(float_complex) * bins / 2);
		memcpy(&fd[spectrum*bins+bins/2], &temp, sizeof(float_complex)
				* bins / 2);
	}

	for (bin = 0; bin < bins; bin++)
		power[bin] = 0;
	
	// now compute the total power in each frequency bin
	for (spectrum = 0; spectrum < spectra; spectrum++) {
		for (bin = 0; bin < bins; bin++) {
			binPower = norm(fd[spectrum*bins+bin]);
#ifdef notdef
			fprintf(oFp, "%03d:%05d (%.3f, %.3f) (%.3f)\n", spectrum, bin,
					fd[spectrum*bins+bin].real(), fd[spectrum*bins+bin].imag(),
					binPower);
#endif
			power[bin] += binPower;
		}
	}

	// print the powers in the bins
	for (bin = 0; bin < bins; bin++)
		fprintf(oFp, "%05d: %.3le\n", bin, power[bin]);

	fclose(iFp);
	fclose(oFp);
}

static char *usage = "chan1Hz -s subbands -h halfframes\n";

int
getArgs(int argc, char **argv)
{
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			cout << usage;
			return (1);
		}
		switch (argv[i][1]) {
		case 's':
			if (strlen(argv[i]) != 2)
				subbands = atoi(&argv[i][2]);
			else
				subbands = atoi(&argv[++i][0]);
			break;
		case 'h':
			if (strlen(argv[i]) != 2)
				halfFrames = atoi(&argv[i][2]);
			else
				halfFrames = atoi(&argv[++i][0]);
			break;
		case 'i':
			if (strlen(argv[i]) != 2)
				strcpy(infile, &argv[i][2]);
			else
				strcpy(infile, &argv[++i][0]);
			break;
		case 'o':
			if (strlen(argv[i]) != 2)
				strcpy(outfile, &argv[i][2]);
			else
				strcpy(outfile, &argv[++i][0]);
			break;
		default:
			cout << usage;
			return (2);
		}
	}
	return (0);
}
