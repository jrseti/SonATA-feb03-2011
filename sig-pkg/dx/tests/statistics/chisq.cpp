/*******************************************************************************

 File:    chisq.cpp
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

typedef float float32_t;
typedef double float64_t;

#include <stdio.h>
#include <stdlib.h>
#include "../../include/PdmStatistics.h"


int32_t n;
float64_t x;

// function prototypes
int getArgs(int argc, char **argv);

int
main(int argc, char **argv)
{
	float64_t chiSq;

	if (getArgs(argc, argv))
		exit(1);

	chiSq = ChiSquare(n, x);
	printf("%.3lf\n", chiSq);
}

static char *usage = "chisq -n n -x value\n";

int
getArgs(int argc, char **argv)
{
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			printf(usage);
			return (1);
		}
		switch (argv[i][1]) {
		case 'n':
			if (strlen(argv[i]) != 2)
				n = atoi(&argv[i][2]);
			else
				n = atoi(&argv[++i][0]);
			break;
		case 'x':
			if (strlen(argv[i]) != 2)
				x = atof(&argv[i][2]);
			else
				x = atof(&argv[++i][0]);
			break;
		default:
			printf(usage);
			return (2);
		}
	}
	return (0);
}