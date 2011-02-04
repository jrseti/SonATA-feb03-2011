/*******************************************************************************

 File:    rc.cpp
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
	char filename[256], outfile[256];
	int i, fd, ofd, length = 1, rcnt;
	unsigned long *buf;
	loff_t offset, result;

	if (argc < 4) {
		printf("Usage: %s device 0xoffset 0xlength outfile\n", argv[0]);
		exit(1);
	}

	strcpy(filename, argv[1]);
	if ((fd = open(filename, O_RDONLY)) < 0) {
		perror("opening file");
		exit(2);
	}

	sscanf(argv[2], "0x%Lx", &offset);
	sscanf(argv[3], "0x%x", &length);

	strcpy(outfile, argv[4]);
	if ((ofd = open(outfile, O_CREAT | O_WRONLY + O_TRUNC, 0666)) < 0) {
		perror("opening outfile");
		exit(3);
	}
	
	// allocate the buffer
	buf = new unsigned long[length/sizeof(unsigned long)+1024];
	// force buffer to a page boundary
	buf = (unsigned long *) (((unsigned long) buf + 4096) & ~0xfff);

	if (result = lseek64(fd, offset, SEEK_SET) < 0) {
		perror("seeking file");
		exit(4);
	}

	if ((rcnt = read(fd, (void *) buf, length)) != length) {
		printf("%d bytes read\n", rcnt);
		perror("reading file");
		exit(3);
	}

	// write the data to a file
	if (write(ofd, (void *) buf, length) < 0) {
		perror("writing file");
		exit(4);
	}
	close(fd);
	close(ofd);
}