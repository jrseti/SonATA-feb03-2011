/*******************************************************************************

 File:    dadd_mmap_write_test.cpp
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
// dadd_mmap_write_test: write disk data to the DADD
//		memory and print the result
//
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "daddio.h"

extern "C" {
	loff_t lseek64(int fd, loff_t offset, int whence);
};

int
main(int argc, char **argv)
{
	string daddName, diskName;
	int i, length = 1, rcnt;
	int daddFd, diskFd;
	unsigned long *daddBuf;
	u_int32_t ioData;
	DADDioctrl daddIoctl;
	loff_t diskOfs;
	off_t daddOfs;

	if (argc < 6) {
		cout << "Usage: " << argv[0] << "daddDev diskDev daddOfs diskOfs length\n";
		exit(1);
	}

	daddName = argv[1];
	diskName = argv[2];
	sscanf(argv[3], "0x%x", &daddOfs);
	sscanf(argv[4], "0x%Lx", &diskOfs);
	length = atoi(argv[5]);

	// open the DADD board
	if ((daddFd = open(daddName.c_str(), O_RDWR)) < 0) {
		perror("opening dadd");
		exit(1);
	}

	// open the disk device
	if ((diskFd = open(diskName.c_str(), O_RDONLY)) < 0) {
		perror("opening disk");
		exit(2);
	}

	daddIoctl.host_addr = &ioData;
	daddIoctl.dadd_addr = 0;
	daddIoctl.count = sizeof(ioData);

	if (ioctl(daddFd, DADDIO_RUN, &daddIoctl) < 0) {
		perror("running DADD");
		exit(3);
	}

	// map the DADD memory into user space
	if ((daddBuf = (unsigned long *) mmap(0, length, PROT_READ | PROT_WRITE,
			MAP_SHARED, daddFd, 0)) < 0) {
		perror("mapping DADD memory");
		exit(4);
	}

	// now read data from the disk into DADD memory
	if (lseek64(diskFd, diskOfs, SEEK_SET) < 0) {
		perror("seeking file");
		exit(5);
	}

	if ((rcnt = read(diskFd, (void *) daddBuf, 4096)) != 4096) {
		perror("reading file");
		exit(3);
	}
	
	close(daddFd);
	close(diskFd);
}