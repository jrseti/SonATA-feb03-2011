/*******************************************************************************

 File:    wrt.cpp
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern "C" {
	loff_t lseek64(int fd, loff_t offset, int whence);
};

int
main(int argc, char **argv)
{
	char filename[256];
	int i, fd, length = 1, wcnt;
	unsigned long *buf;
	loff_t offset;

	if (argc < 3) {
		printf("Usage: %s device offset length\n", argv[0]);
		exit(1);
	}

	strcpy(filename, argv[1]);
	if ((fd = open(filename, O_WRONLY)) < 0) {
		perror("opening file");
		exit(2);
	}

	sscanf(argv[2], "0x%Lx", &offset);
	if (argc > 3)
		length = atoi(argv[3]);

	// allocate the buffer
	buf = new unsigned long[length+1024];
	// force buffer to a page boundary
	buf = (unsigned long *) (((unsigned long) buf + 4096) & ~0xfff);

	// create a ramp in the data before writing it to disk
	for (i = 0; i < length; i++)
		buf[i] = i;

	if (lseek64(fd, offset, SEEK_SET) < 0) {
		perror("seeking file");
		exit(4);
	}

	if ((wcnt = write(fd, (void *) buf, 4096)) != 4096) {
		perror("writing file");
		exit(3);
	}
	close(fd);
}