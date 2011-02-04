/*******************************************************************************

 File:    cw.cpp
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

main(int argc, char **argv)
{
	unsigned int val = 0, halfFrame, subband, slice, bin;

	if (argc < 2) {
		printf("Usage: %s 0xnnnnnnnn\n", argv[0]);
		exit(1);
	}

	sscanf(argv[1], "0x%x", &val);
	halfFrame = val >> 22;
	subband = (val >> 10) & 0xfff;
	slice = subband / 32;
	bin = val & 0x3ff;
	printf("hf %d, subband %d, slice %d, bin 0x%x\n", halfFrame, subband,
			slice, bin);
}
	