/*******************************************************************************

 File:    ninittab.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/ninittab.cc,v $
**
** Initialization tables for NSS prototype
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/ninittab.cc,v 1.1.1.1 2006/08/02 05:30:25 kes Exp $
** $Log: ninittab.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:25  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:25  kes
** *** empty log message ***
**
Revision 1.2  98/10/29  11:11:22  kes
New queues, tasks, info structures.

Revision 1.1  98/10/27  08:17:24  kes
Initial revision

**********************************************************************/

#include <sys/file.h>
#include <termios.h>
#include "nss.h"
#include "cdwrite.h"
#include "collect.h"
#include "confirm.h"
#include "connect.h"
#include "daddwrite.h"
#include "detect.h"
#include "exit.h"
#include "globals.h"
#include "init.h"
#include "nssprio.h"
#include "queue.h"
#include "read.h"
#include "signals.h"

/* set of task IDs */
tid_t ControlTid;
tid_t LDaddInputTid, RDaddInputTid;
tid_t LDaddCollectTid, RDaddCollectTid;
tid_t LCDInputTid, RCDInputTid;
tid_t LCDCollectTid, RCDCollectTid;
tid_t DetectTid;
tid_t ConfirmTid;
tid_t ReadTid;
tid_t DaddReadTid;
tid_t CDReadTid;
tid_t DaddProcTid;
tid_t CDProcTid;
tid_t SigWaitTid;
tid_t CheckTimeTid;
tid_t SetTimeTid;

/* set of queues */
queue_t *CollectQ, *DetectQ, *ConfirmQ, *ReadQ;
queue_t *WaitCollectQ, *WaitDetectQ, *WaitConfirmQ;
queue_t *LDaddInputQ, *RDaddInputQ;
queue_t *LDaddCollectQ, *RDaddCollectQ;
queue_t *LDaddCollectDataQ, *RDaddCollectDataQ;
queue_t *LCDInputQ, *RCDInputQ;
queue_t *LCDCollectQ, *RCDCollectQ;
queue_t *LCDCollectDataQ, *RCDCollectDataQ;
queue_t *DetectDataQ;
queue_t *ConfirmDataQ;

/* set of task functions */
task_t ControlTask(void *);
task_t DaddInputTask(void *);
task_t DaddWriteTask(void *);
task_t CDInputTask(void *);
task_t CDWriteTask(void *);
task_t DaddReadTask(void *);
task_t CDReadTask(void *);
task_t DaddProcTask(void *);
task_t CDProcTask(void *);
task_t SigWaitTask(void *);
task_t CheckTimeTask(void *);
task_t SetTimeTask(void *);

/* system queues required */
queue_spec_t Queues[] = {
	{ &CollectQ, "CollectQ", COLLECT_SLOTS },
	{ &DetectQ, "DetectQ", DETECT_SLOTS },
	{ &ConfirmQ, "ConfirmQ", CONFIRM_SLOTS },
	{ &ReadQ, "ReadQ", READ_SLOTS },
	{ &WaitCollectQ, "WaitCollectQ", WAIT_SLOTS },
	{ &WaitDetectQ, "WaitDetectQ", WAIT_SLOTS },
	{ &WaitConfirmQ, "WaitConfirmQ", WAIT_SLOTS },
	{ &LDaddInputQ, "LDaddInputQ", DADD_INPUT_SLOTS },
	{ &RDaddInputQ, "RDaddInputQ", DADD_INPUT_SLOTS },
	{ &LCDInputQ, "LCDInputQ", CD_INPUT_SLOTS },
	{ &RCDInputQ, "RCDInputQ", CD_INPUT_SLOTS },

	{ &LDaddCollectQ, "LDaddCollectQ", DADD_WRITE_SLOTS },
	{ &RDaddCollectQ, "RDaddCollectQ", DADD_WRITE_SLOTS },
	{ &LDaddCollectDataQ, "LDaddCollectDataQ", DADD_WRITE_SLOTS },
	{ &RDaddCollectDataQ, "RDaddCollectDataQ", DADD_WRITE_SLOTS },
	{ &LCDCollectQ, "LCDCollectQ", CD_WRITE_SLOTS },
	{ &RCDCollectQ, "RCDCollectQ", CD_WRITE_SLOTS },
	{ &LCDCollectDataQ, "LCDCollectDataQ", CD_WRITE_SLOTS },
	{ &RCDCollectDataQ, "RCDCollectDataQ", CD_WRITE_SLOTS },

	{ &DetectDataQ, "DetectDataQ", READ_DATA_SLOTS },
	{ &ConfirmDataQ, "ConfirmDataQ", READ_DATA_SLOTS },
	{ NULL }
};

/* system semaphores required */
sem_spec_t Sems[] = {
	{ &DaddSem, "DaddSem", 1 },
	{ &CDSem, "CDSem", 1 },
	{ NULL }
};

/* system files opened */
file_spec_t Files[] = {
	{ &LDaddFd[0], "/dev/rsd2940.1a", O_RDWR },
	{ &LDaddFd[1], "/dev/rsd2940.1b", O_RDWR },
	{ &RDaddFd[0], "/dev/rsd2940.1d", O_RDWR },
	{ &RDaddFd[1], "/dev/rsd2940.1c", O_RDWR },
	{ &LCDFd[0], "/dev/rsd2940.2a", O_RDWR },
	{ &LCDFd[1], "/dev/rsd2940.2b", O_RDWR },
	{ &RCDFd[0], "/dev/rsd2940.2d", O_RDWR },
	{ &RCDFd[1], "/dev/rsd2940.2c", O_RDWR },
	{ NULL }
};

/* system buffers created */
buffer_spec_t Buffers[] = {
	{ &DaddBuf, "DaddBuf", 16 * 4096 * (1024 / 4), 4 },
	{ &CDBuf, "CDBuf", 4 * 4096 * 1024, 4 },
	{ &SliceBuf, "SliceBuf", 1024 * (32768 / 4), 3 },
	{ &ProcBuf, "ProcBuf", 4096 * (8192 + 4096), 1 },
	{ &ChanBuf, "ChanBuf", 4096 * 16, 8 }
};

/* set of connections required */

/* initialization array for connections */
connection_t *Connections[] = {
	NULL
};

/* set of clients required for operation */
/* low-level I/O I/F clients */

/*
** The following list of clients is limited to those who may actually
** send a command or request.  Passive clients, such as the encoders,
** drives, and PLC do not require entries in this table.
*/
client_t *Clients[] = {
	NULL
};

/* set of task initialization structures */
dadd_info_t LDaddInputInfo = { &LDaddInputQ, &LDaddCollectQ,
		&LDaddCollectDataQ, 0 };
dadd_info_t RDaddInputInfo = { &RDaddInputQ, &RDaddCollectQ,
		&RDaddCollectDataQ, 0 };
cd_info_t LCDInputInfo = { &LCDInputQ, &LCDCollectQ, &LCDCollectDataQ, 0 };
cd_info_t RCDInputInfo = { &RCDInputQ, &RCDCollectQ, &RCDCollectDataQ, 0 };

collect_info_t LDaddCollectInfo = { Dadd, Left, &LDaddCollectQ, &WaitCollectQ,
	&LDaddCollectDataQ, &DaddSem, &DaddBuf };
collect_info_t RDaddCollectInfo = { Dadd, Right, &RDaddCollectQ, &WaitCollectQ,
	&RDaddCollectDataQ, &DaddSem, &DaddBuf };
collect_info_t LCDCollectInfo = { CD, Left, &LCDCollectQ, &WaitCollectQ,
	&LCDCollectDataQ, &CDSem, &CDBuf };
collect_info_t RCDCollectInfo = { CD, Right, &RCDCollectQ, &WaitCollectQ,
	&RCDCollectDataQ, &CDSem, &CDBuf };

detect_info_t DetectInfo = { &DetectQ, &WaitDetectQ, &ReadQ, &DetectDataQ };
confirm_info_t ConfirmInfo = { &ConfirmQ, &WaitConfirmQ, &ReadQ, &ConfirmDataQ };
read_info_t ReadInfo = { &ReadQ, &DetectQ, &DetectDataQ };

/* set of tasks required */
task_spec_t Tasks[] = {
	{ ControlTask, CONTROL_PRIO, 0, &ControlTid },
#ifdef notdef
	{ CheckTimeTask, CHKTIME_PRIO, 0, &CheckTimeTid },
#endif
	{ DaddInputTask, DADD_INPUT_PRIO, &LDaddInputInfo, &LDaddInputTid },
	{ DaddInputTask, DADD_INPUT_PRIO, &RDaddInputInfo, &RDaddInputTid },
	{ CDInputTask, CD_INPUT_PRIO, &LCDInputInfo, &LCDInputTid },
	{ CDInputTask, CD_INPUT_PRIO, &RCDInputInfo, &RCDInputTid },
	{ CollectionTask, DADD_WRITE_PRIO, &LDaddCollectInfo, &LDaddCollectTid },
	{ CollectionTask, DADD_WRITE_PRIO, &RDaddCollectInfo, &RDaddCollectTid },
	{ CollectionTask, CD_WRITE_PRIO, &LCDCollectInfo, &LCDCollectTid },
	{ CollectionTask, CD_WRITE_PRIO, &RCDCollectInfo, &RCDCollectTid },
	{ DetectionTask, DETECTION_PRIO, &DetectInfo, &DetectTid },
	{ ConfirmationTask, CONFIRMATION_PRIO, &ConfirmInfo, &ConfirmTid },
	{ ReadTask, READ_PRIO, &ReadInfo, &ReadTid },
#ifdef notdef
	{ DaddReadTask, DADD_READ_PRIO, 0, &DaddReadTid },
	{ CDReadTask, CD_READ_PRIO, 0, &CDReadTid },
	{ DaddProcTask, DADD_PROC_PRIO, 0, &DaddProcTid },
	{ CDProcTask, CD_PROC_PRIO, 0, &CDProcTid },
#endif
	{ NULL }
};

/*
** set of signals and their handlers: must be terminated with
** a signal number < 0.
*/
sig_spec_t Handlers[] = {
	{ SIGPIPE, SIG_IGN },
	{ SIGINT, SIG_IGN },
	{ SIGTSTP, SIG_IGN },
	{ SIGQUIT, Quit },
	{ -1 }
};