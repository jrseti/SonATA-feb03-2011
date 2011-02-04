/*******************************************************************************

 File:    test.cc
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

/**********************************************************************
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/rdwrt/test.cc,v $
**
** Read/write tests
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/rdwrt/test.cc,v 1.1.1.1 2006/08/02 05:30:34 kes Exp $
** $Log: test.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:34  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 06:02:19  kes
** *** empty log message ***
**
Revision 1.3  98/12/04  11:30:33  kes
*** empty log message ***

Revision 1.2  98/10/23  11:40:11  kes
Does all DADD and CD storage and retrieval; rudimentary, but
gives a good estimate of I/O speeds.

Revision 1.1  98/10/23  03:54:13  kes
Initial revision

**********************************************************************/

// use these defines to configure the tests to be run
#define DADD_OUTPUT
#define CD_OUTPUT
#define PROCESS_DADD
#define PROCESS_CD

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define SPECTRUM_BINS	(4194304)
#define BUFF_SPECTRA	16
#define FRAMES			64
#define SPECTRA			(FRAMES * 2)
#define SLICE_WIDTH		32768
#define BUFFSIZE		(BUFF_SPECTRA * SPECTRUM_BINS / 4)
#define CD_CHANNELS		4096
#define CD_CHAN_BLOCKSIZE (CD_BUFFSIZE / CD_CHANNELS)
#define CD_BUFFSIZE		BUFFSIZE
#define CD_BUFF_FRAMES	4
#define CD_PROC_CHANNELS 16
#define CD_PROC_BUFFSIZE (CD_PROC_CHANNELS * FRAMES * (SPECTRUM_BINS / CD_CHANNELS))
#define SLICE_BUFFSIZE	(SPECTRA * SLICE_WIDTH / 4)
#define SLICES			(SPECTRUM_BINS / SLICE_WIDTH)
#define SLICE_BLOCKS	(SPECTRA / BUFF_SPECTRA)
#define SLICE_BLOCKSIZE	(BUFF_SPECTRA * SLICE_WIDTH / 4)

#define CANDIDATES		64

#define DADD_SLEEP_SEC	0
#define DADD_SLEEP_USEC	0
#define CD_SLEEP_SEC	0
#define CD_SLEEP_USEC	0
#define PROC_SLEEP_SEC	0
#define PROC_SLEEP_USEC	0
#define CD_PROC_SLEEP_SEC	0
#define CD_PROC_SLEEP_USEC	0

// task priorities
#define LDADD_OUT_PRIO	31
#define RDADD_OUT_PRIO	31
#define LCD_OUT_PRIO	31
#define RCD_OUT_PRIO	31
#define PROC_PRIO		31
#define LDADD_BUFF_PRIO	32
#define RDADD_BUFF_PRIO	32
#define LCD_BUFF_PRIO	32
#define RCD_BUFF_PRIO	32

typedef void *			task_t;

// DADD output buffers
unsigned char *lDaddBuff0;
unsigned char *lDaddBuff1;
unsigned char *rDaddBuff0;
unsigned char *rDaddBuff1;

// CD output buffers
unsigned char *lCDBuff0;
unsigned char *lCDBuff1;
unsigned char *rCDBuff0;
unsigned char *rCDBuff1;

// slice buffers
unsigned char *sliceBuff0;
unsigned char *sliceBuff1;

// CD buffer
unsigned char *cdBuff;

// misc
int Done = 5;
unsigned int spectrumBins = SPECTRUM_BINS;
unsigned int frames = FRAMES;
unsigned int spectra = SPECTRA;
unsigned int buffSpectra = BUFF_SPECTRA;
unsigned int buffSize = BUFFSIZE;
unsigned int cdBuffSize = CD_BUFFSIZE;
unsigned int cdBuffFrames = CD_BUFF_FRAMES;
unsigned int sliceBuffSize = SLICE_BUFFSIZE;
unsigned int slices = SLICES;
unsigned int sliceWidth = SLICE_WIDTH;
unsigned int sliceBlocks = SLICE_BLOCKS;
unsigned int sliceBlockSize = SLICE_BLOCKSIZE;
unsigned int cdProcChannels = CD_PROC_CHANNELS;
unsigned int cdChanBlockSize = CD_CHAN_BLOCKSIZE;
unsigned int cdProcBuffSize = CD_PROC_BUFFSIZE;
double fstart, fend;

sem_t *daddSem, *lDaddBuffSem, *rDaddBuffSem;
sem_t *cdSem, *lCDBuffSem, *rCDBuffSem;

struct timeval start, end;

// task IDs
tid_t lDaddTid, rDaddTid;
tid_t lCDTid, rCDTid;
tid_t procTid;
tid_t lDaddBuffTid, rDaddBuffTid;
tid_t lCDBuffTid, rCDBuffTid;

// device file descriptors
int lDaddFd0, lDaddFd1;
int rDaddFd0, rDaddFd1;
int lCDFd0, lCDFd1;
int rCDFd0, rCDFd1;

// prototypes
task_t LeftDaddOutputTask(void *arg);
task_t RightDaddOutputTask(void *arg);
task_t LeftCDOutputTask(void *arg);
task_t RightCDOutputTask(void *arg);
task_t ProcessingTask(void *arg);
task_t LeftDaddBuffTask(void *arg);
task_t RightDaddBuffTask(void *arg);
task_t LeftCDBuffTask(void *arg);
task_t RightCDBuffTask(void *arg);

int CreateTask(tid_t *tidp, int priority, void *(*func)(void *), void *arg);
void TaskSuicide(void);

// main driver
int
main(int argc, char **argv)
{
	int i, lFd0, lFd1, rFd0, rFd1;
	long lOfs0, rOfs0, lOfs1, rOfs1;

	// allocate DADD output buffers
	lDaddBuff0 = new unsigned char[buffSize];
	lDaddBuff1 = new unsigned char[buffSize];
	rDaddBuff0 = new unsigned char[buffSize];
	rDaddBuff1 = new unsigned char[buffSize];

	// allocate CD output buffers
	lCDBuff0 = new unsigned char[cdBuffSize];
	lCDBuff1 = new unsigned char[cdBuffSize];
	rCDBuff0 = new unsigned char[cdBuffSize];
	rCDBuff1 = new unsigned char[cdBuffSize];

	// allocate slice input buffers
	sliceBuff0 = new unsigned char[sliceBuffSize];
	sliceBuff1 = new unsigned char[sliceBuffSize];

	// allocate confirmation data input buffer
	cdBuff = new unsigned char[cdProcBuffSize];

	// open file descriptors
	lDaddFd0 = open("/dev/rsd2940.1a", O_RDWR);
	lDaddFd1 = open("/dev/rsd2940.1b", O_RDWR);
	rDaddFd0 = open("/dev/rsd2940.1d", O_RDWR);
	rDaddFd1 = open("/dev/rsd2940.1c", O_RDWR);

	lCDFd0 = open("/dev/rsd2940.2a", O_RDWR);
	lCDFd1 = open("/dev/rsd2940.2b", O_RDWR);
	rCDFd0 = open("/dev/rsd2940.2d", O_RDWR);
	rCDFd1 = open("/dev/rsd2940.2c", O_RDWR);

	// create semaphores
	sem_unlink("DaddSem");
	daddSem = sem_open("DaddSem", O_CREAT, 0777, 1);
	sem_unlink("CDSem");
	cdSem = sem_open("CDSem", O_CREAT, 0777, 1);

	sem_unlink("LDaddSem");
	lDaddBuffSem = sem_open("LDaddSem", O_CREAT, 0777, 0);
	sem_unlink("RDaddSem");
	rDaddBuffSem = sem_open("RDaddSem", O_CREAT, 0777, 0);
	sem_unlink("LCDSem");
	lCDBuffSem = sem_open("LCDSem", O_CREAT, 0777, 1);
	sem_unlink("RCDSem");
	rCDBuffSem = sem_open("RCDSem", O_CREAT, 0777, 1);

	// time the entire operation
	gettimeofday(&start, NULL);

	// create DADD and CD output tasks
	CreateTask(&lDaddTid, LDADD_OUT_PRIO, LeftDaddOutputTask, NULL);
	CreateTask(&rDaddTid, RDADD_OUT_PRIO, RightDaddOutputTask, NULL);
	CreateTask(&lCDTid, LCD_OUT_PRIO, LeftCDOutputTask, NULL);
	CreateTask(&rCDTid, RCD_OUT_PRIO, RightCDOutputTask, NULL);

	// create tasks to control read timing
	CreateTask(&lDaddBuffTid, LDADD_BUFF_PRIO, LeftDaddBuffTask, NULL);
	CreateTask(&rDaddBuffTid, RDADD_BUFF_PRIO, RightDaddBuffTask, NULL);
	CreateTask(&lCDBuffTid, LCD_BUFF_PRIO, LeftCDBuffTask, NULL);
	CreateTask(&rCDBuffTid, RCD_BUFF_PRIO, RightCDBuffTask, NULL);

	// create processing task
	CreateTask(&procTid, PROC_PRIO, ProcessingTask, NULL);

	while (Done)
		sleep(1);
    gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("time for %d spectra = %.3lf\n", SPECTRA, fend - fstart);
}

// write data for the left DADD
task_t
LeftDaddOutputTask(void *arg)
{
	int i;
	long lOfs = 0;

#ifdef DADD_OUTPUT
	for (i = 0; i < spectra; i += buffSpectra) {
		sem_wait(lDaddBuffSem);
		sem_wait(daddSem);
		lseek(lDaddFd0, lOfs, 0);
		if (write(lDaddFd0, lDaddBuff0, buffSize) != buffSize) {
			perror("left Dadd output");
			break;
		}
		sem_post(daddSem);
		lOfs += buffSize;
	}
#endif
	Done--;
	TaskSuicide();
}

// write data for the right DADD
task_t
RightDaddOutputTask(void *arg)
{
	int i;
	long rOfs = 0;

#ifdef DADD_OUTPUT
	for (i = 0; i < spectra; i += buffSpectra) {
		sem_wait(rDaddBuffSem);
		sem_wait(daddSem);
		lseek(rDaddFd0, rOfs, 0);
		if (write(rDaddFd0, rDaddBuff0, buffSize) != buffSize) {
			perror("right Dadd output");
			break;
		}
		sem_post(daddSem);
		rOfs += buffSize;
	}
#endif
	Done--;
	TaskSuicide();
}

// write data for the left CD
task_t
LeftCDOutputTask(void *arg)
{
	int i;
	long lOfs = 0;

#ifdef CD_OUTPUT
	for (i = 0; i < frames; i += cdBuffFrames) {
		sem_wait(lCDBuffSem);
		sem_wait(cdSem);
		lseek(lCDFd0, lOfs, 0);
		if (write(lCDFd0, lCDBuff0, cdBuffSize) != cdBuffSize) {
			perror("left CD output");
			break;
		}
		sem_post(cdSem);
		lOfs += cdBuffSize;
	}
#endif
	Done--;
	TaskSuicide();
}

// write data for the right CD
task_t
RightCDOutputTask(void *arg)
{
	int i;
	long rOfs = 0;

#ifdef CD_OUTPUT
	for (i = 0; i < frames; i += cdBuffFrames) {
		sem_wait(rCDBuffSem);
		sem_wait(cdSem);
		lseek(rCDFd0, rOfs, 0);
		if (write(rCDFd0, rCDBuff0, cdBuffSize) != cdBuffSize) {
			perror("right CD output");
			break;
		}
		sem_post(cdSem);
		rOfs += cdBuffSize;
	}
#endif
	Done--;
	TaskSuicide();
}

// simulate processing
task_t
ProcessingTask(void *arg)
{
	int i, j, candidate;
	unsigned int daddBytes = 0, cdBytes = 0;
	long bufOfs, ofs;

#ifdef PROCESS_DADD
	// do left DADD processing
	for (i = 0; i < slices; i++) {
		bufOfs = 0;
		ofs = i * sliceBlockSize;
		sem_wait(daddSem);
		for (j = 0; j < sliceBlocks; j++) {
			lseek(lDaddFd1, ofs, 0);
			read(lDaddFd1, &sliceBuff0[bufOfs], sliceBlockSize);
			bufOfs += sliceBlockSize;
			ofs += buffSize;
			daddBytes += sliceBlockSize;
		}
		sem_post(daddSem);
		susleep(PROC_SLEEP_SEC, PROC_SLEEP_USEC);
	}
#ifdef notdef
    gettimeofday(&end, NULL);
   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("Left DADD time for %d spectra (%u bytes) = %.3lf\n", spectra, daddBytes,
			fend - fstart);
	// do right DADD processing
#endif
	for (i = 0; i < slices; i++) {
		bufOfs = 0;
		ofs = i * sliceBlockSize;
		sem_wait(daddSem);
		for (j = 0; j < sliceBlocks; j++) {
			lseek(rDaddFd1, ofs, 0);
			read(rDaddFd1, &sliceBuff1[bufOfs], sliceBlockSize);
			bufOfs += sliceBlockSize;
			ofs += buffSize;
			daddBytes += sliceBlockSize;
		}
		sem_post(daddSem);
		susleep(PROC_SLEEP_SEC, PROC_SLEEP_USEC);
	}
    gettimeofday(&end, NULL);
   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("Total DADD time (L&R) for %d spectra (%u bytes) = %.3lf\n", spectra, daddBytes,
			fend - fstart);
#endif

#ifdef PROCESS_CD
	// do confirmation signals (8, dual-pol)
	for (candidate = 0; candidate < CANDIDATES; candidate++) {
		// read left pol data
		bufOfs = 0;
		ofs = candidate * 8 * cdChanBlockSize * cdProcChannels;
		sem_wait(cdSem);
		for (i = 0; i < frames; i += cdBuffFrames) {
			lseek(lCDFd1, ofs, 0);
			read(lCDFd1, &cdBuff[bufOfs], cdChanBlockSize * cdProcChannels);
			bufOfs += cdChanBlockSize * cdProcChannels;
			ofs += cdBuffSize;
			cdBytes += cdChanBlockSize * cdProcChannels;
		}
		sem_post(cdSem);
		// read right pol data
		bufOfs = 0;
		ofs = candidate * 8 * cdChanBlockSize * cdProcChannels;
		sem_wait(cdSem);
		for (i = 0; i < frames; i += cdBuffFrames) {
			lseek(rCDFd1, ofs, 0);
			read(rCDFd1, &cdBuff[bufOfs], cdChanBlockSize * cdProcChannels);
			bufOfs += cdChanBlockSize * cdProcChannels;
			ofs += cdBuffSize;
			cdBytes += cdChanBlockSize * cdProcChannels;
		}
		sem_post(cdSem);
		susleep(CD_PROC_SLEEP_SEC, CD_PROC_SLEEP_USEC);
	}
    gettimeofday(&end, NULL);
   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("CD time for %d frames (%u bytes) = %.3lf\n", frames, cdBytes,
			fend - fstart);
#endif
	Done--;
	TaskSuicide();
}

// simulate left DADD buffer loading by DSP
task_t
LeftDaddBuffTask(void *arg)
{
	int i;

	for (i = 0; i < spectra; i += buffSpectra) {
		susleep(DADD_SLEEP_SEC, DADD_SLEEP_USEC);
		sem_post(lDaddBuffSem);
	}
	TaskSuicide();
}

// simulate right DADD buffer loading by DSP
task_t
RightDaddBuffTask(void *arg)
{
	int i;

	for (i = 0; i < spectra; i += buffSpectra) {
		susleep(DADD_SLEEP_SEC, DADD_SLEEP_USEC);
		sem_post(rDaddBuffSem);
	}
	TaskSuicide();
}

// simulate left CD buffer loading by DSP
task_t
LeftCDBuffTask(void *arg)
{
	int i;

	for (i = 0; i < frames; i += cdBuffFrames) {
		susleep(CD_SLEEP_SEC, CD_SLEEP_USEC);
		sem_post(lCDBuffSem);
	}
	TaskSuicide();
}

// simulate right DADD buffer loading by DSP
task_t
RightCDBuffTask(void *arg)
{
	int i;

	for (i = 0; i < frames; i += cdBuffFrames) {
		susleep(CD_SLEEP_SEC, CD_SLEEP_USEC);
		sem_post(rCDBuffSem);
	}
	TaskSuicide();
}

//
// CreateTask: create a task
//
int
CreateTask(tid_t *tidp, int priority, void *(*func)(void *), void *arg)
{
	pthread_attr_t attr;
	pthread_mutexattr_t mattr;

	/* create the attribute */
	pthread_attr_create(&attr);
	if (pthread_attr_setinheritsched(&attr, PTHREAD_DEFAULT_SCHED) == -1)
		return (errno);
	if (pthread_attr_setsched(&attr, SCHED_DEFAULT) == -1)
		return (errno);
	if (pthread_attr_setprio(&attr, priority) == -1)
		return (errno);

	/* create the task */
	if (pthread_create(tidp, attr, func, arg) == -1) {
		return (errno);
	}
	return (pthread_detach(tidp));
}

/*
** TaskSuicide: commit suicide
*/
void
TaskSuicide(void)
{
	pthread_exit(0);
}