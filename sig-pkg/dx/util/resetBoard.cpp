/*******************************************************************************

 File:    resetBoard.cpp
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
// simple program to reset the DSP board
//
// $Header: /home/cvs/nss/sonata-pkg/dx/util/resetBoard.cpp,v 1.1.1.1 2006/08/02 05:28:28 kes Exp $
// $Log: resetBoard.cpp,v $
// Revision 1.1.1.1  2006/08/02 05:28:28  kes
// pdm-1_59
//
// Revision 1.4  2002/01/29 17:08:23  carl
// Add support for boards 4 & 5.
//
// Revision 1.3  2001/10/24 19:44:40  carl
// Merge Ken's code with standalong pdm modifications.
//
// Revision 1.2  2001/08/01 00:16:11  kes
// Added Header and Log
//
//
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "linux-defs.h"

char *board1 = "/dev/pcic6600/node0";
char *board2 = "/dev/pcic6600/node3";
char *board3 = "/dev/pcic6600/node6";
char *board4 = "/dev/pcic6600/node9";
char *board5 = "/dev/pcic6600/node12";

char *Pgm;

void
usage()
{
	printf("usage: %s [boardNumber]\n",Pgm);
	printf("\tWhere boardNumber defaults to 1 when not specified.\n");
	exit(1);
}

int
main(int argc, char **argv)
{
	int fd, boardNum;
	int bootmode = 1;
	char *board;

	Pgm = argv[0];

		/* get board from args */
	if (argc == 1)
		boardNum = 1;
	else if (argc == 2)
		boardNum = atoi(argv[1]);
	else
		usage();

	switch (boardNum) 
	{
		case 1: board = board1; break;
		case 2: board = board2; break;
		case 3: board = board3; break;
		case 4: board = board4; break;
		case 5: board = board5; break;
		default:
			printf("%d is an invalid board number.  1, 2, 3, 4 and 5 are the only valid choices.\n",atoi(argv[1]));
			exit(-1);
	}
	printf ("Invoking board reset on: %s\n",board);
	if ((fd = open(board, O_RDWR)) < 0) {
		perror("opening board");
		exit(1);
	}

	// set boot mode
	if (ioctl(fd, LINIO_SETBOOTMODE, &bootmode) < 0) {
		perror("setting boot mode");
		exit(2);
	}

	// reset the board
	if (ioctl(fd, LINIO_ASSERTRESET) < 0) {
		perror("asserting reset");
		exit(3);
	}

	// release reset
	if (ioctl(fd, LINIO_DEASSERTRESET) < 0) {
		perror("deasserting reset");
		exit(4);
	}
	return (0);
}
