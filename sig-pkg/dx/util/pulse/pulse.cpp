/*******************************************************************************

 File:    pulse.cpp
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
#include "hostiface.h"
#include "sseDxInterface.h"

typedef struct pulse {
	uint32_t resPolSpectrum;
	uint32_t powerBin;
} pulse_t;

#define RES_MASK        0x1f
#define RES_SHIFT       27
#define POL_MASK        0x3
#define POL_SHIFT       24
#define SPEC_MASK       0x00ffffff
#define SPEC_SHIFT      0
#define POWER_MASK      0x7f
#define POWER_SHIFT     24
#define BIN_MASK        0x00ffffff
#define BIN_SHIFT       0

#define RESOLUTION(x)   (((x) >> RES_SHIFT) & RES_MASK)
#define POLARIZATION(x) (((x) >> POL_SHIFT) & POL_MASK)
#define SPECTRUM(x)     (((x) >> SPEC_SHIFT) & SPEC_MASK)
#define POWER(x)        (((x) >> POWER_SHIFT) & POWER_MASK)
#define BIN(x)          (((x) >> BIN_SHIFT) & BIN_MASK)

static char *resolution[] = {
	"RES_1HZ"
};

static char *polarization[] = {
	"RIGHT",
	"LEFT",
	"BOTH",
	"MIXED",
	"UNKNOWN"
};

int
main(int argc, char **argv)
{
	int i = 0;
	int32_t spectrum, bin, power;
	FILE *fp;
	Polarization pol;
	pulse_t pulse;
	Resolution res;

	if (argc < 2) {
		printf("Usage: pulse file\n");
		exit(1);
	}

	if (!(fp = fopen(argv[1], "rb"))) {
		perror("opening file");
		exit(2);
	}

	while (fread(&pulse, sizeof(pulse), 1, fp) == 1) {
		res = (Resolution) RESOLUTION(pulse.resPolSpectrum);
		pol = (Polarization) POLARIZATION(pulse.resPolSpectrum);
		spectrum = SPECTRUM(pulse.resPolSpectrum);
		bin = BIN(pulse.powerBin);
		power = POWER(pulse.powerBin);
		
		printf("pulse %d: res %s, pol %s, sp %d, bin %d, power %d\n",
				++i, resolution[res], polarization[pol], spectrum, bin, power);
	}
}