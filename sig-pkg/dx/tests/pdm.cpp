/*******************************************************************************

 File:    pdm.cpp
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
// PDM entry point
//
// This file performs the initialization sequence for the PDM
// 
// $Header: /home/cvs/nss/sonata-pkg/dx/tests/pdm.cpp,v 1.2 2009/02/12 18:52:46 kes Exp $
// $Log: pdm.cpp,v $
// Revision 1.2  2009/02/12 18:52:46  kes
// First compilable version of DX1.  Massive changes from the original
// DX0 (SonATA demo) code.  Compiles and links, but many known bugs and
// is not yet runnable.
//
// Revision 1.1.1.1  2006/08/02 05:28:48  kes
// pdm-1_59
//
// Revision 1.3  2003/06/20 19:03:46  kes
// Updated
//
// Revision 1.2  2001/01/19 23:13:41  kes
// Change to allocation
//
// Revision 1.1.1.1  2001/01/02 07:10:58  kes
// Initial version
//
//
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include "Pdm.h"
#include "PdmErr.h"
#include "PdmLock.h"
#include "PdmMsg.h"
#include "PdmMsgList.h"
#include "PdmQueue.h"
#include "PdmSemaphore.h"
#include "PdmTcp.h"
#include "PdmUdp.h"
#include "recvtask.h"
#include "sendtask.h"
#include "struct.h"

using std::cout;
using std::endl;

static string defaultHost = DEFAULT_HOST;
static int defaultPort = DEFAULT_PORT;
static int defaultPdmID = DEFAULT_ID;

static PdmIntrinsics intrinsics;

static Semaphore *sem;
static Queue *queue;
static Lock *tlock;
static MsgList *msgList;
static Tcp *net;
static UdpConnection *udp;

// test a send/receive task
static SendTask *send;
static RecvTask *recv;

static taskArgs args;

//
// Initial PDM entry point, performing initialization and system
// startup
//
int
main(int argc, char *argv[])
{
	int msg;
	Error err;
	Msg  *pdmMsg, *pdmMsg1;
	string name;
	timeval time;

	gettimeofday(&time, NULL);

	memset(&intrinsics, 0, sizeof(intrinsics));
	strcpy(intrinsics.interfaceVersionNumber, SSE_PDM_INTERFACE_VERSION);
	intrinsics.maxSubbands = 2048;
	intrinsics.hzPerSubband = HZ_PER_SUBBAND;
	gethostname(intrinsics.pdmHostName, sizeof(intrinsics.pdmHostName));
	intrinsics.serialNumber = 1;
	strcpy(intrinsics.pdmCodeVersion, CODE_VERSION);
	intrinsics.birdieMaskDate.tv_sec = time.tv_sec;
	intrinsics.birdieMaskDate.tv_usec = time.tv_usec;
	intrinsics.permMaskDate.tv_sec = time.tv_sec;
	intrinsics.permMaskDate.tv_usec = time.tv_usec;

	tlock = new PdmLock("lock", RecursiveLock);
	msgList = new PdmMsgList("msglist");
	queue = new PdmQueue("queue");
	sem = new PdmSemaphore("semaphore");

	sem->name(name);
	cout << name << endl;

	err = sem->wait();
	sem->signal();
	err = sem->wait();
	if (err = sem->wait(50))
		cout << "wait err = " << err << endl;
	if (err = queue->send(&msg))
		cout << "send err = " << err << endl;

	if (err = queue->recv((void **) &msg))
		cout << "recv err = " << err << endl;
	tlock->lock();
	tlock->unlock();

#ifdef notdef
	pdmMsg = new PdmIntrinsics(&intrinsics);
	pdmMsg1 = msgList->alloc();

	pdmMsg->marshall();
	pdmMsg->demarshall();

	udp = new UdpConnection("udp", UnitNone);
	udp->setAddress("230.1.2.3", 8888);
	HereIAm hereIAm;
	strcpy(hereIAm.interfaceVersionNumber, SSE_PDM_INTERFACE_VERSION);
	cout << hereIAm << endl;
	hereIAm.marshall();

	if (err = udp->send((char *) &hereIAm, sizeof(hereIAm)))
		PdmErr(err);

	usleep(20000);
	ThereYouAre thereYouAre;
	if (err = udp->recv((char *) &thereYouAre, sizeof(thereYouAre)))
		PdmErr(err);
	else {
		thereYouAre.demarshall();
		cout << thereYouAre << endl;
	}

	net = new Tcp("net", UnitNone, ActiveTcpConnection, "localhost", DEFAULT_PORT);
	err = net->establish();
	err = net->terminate();
#endif

	send = new SendTask("send", 10);
	recv = new RecvTask("recv", 11);

	args.queue = queue;
	args.msgList = msgList;

	send->start((void *) &args);
	recv->start((void *) &args);

	while (1)
		;
}
