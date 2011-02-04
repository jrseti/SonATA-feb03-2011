/*******************************************************************************

 File:    globals.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/globals.h,v $
**
** global variable definitions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/globals.h,v 1.1.1.1 2006/08/02 05:29:55 kes Exp $
** $Log: globals.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:55  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:33  kes
** *** empty log message ***
**
 * Revision 1.2  98/10/29  11:31:10  kes
 * Added queues, DaddTable.
 * 
 * Revision 1.1  98/10/27  08:18:25  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "buffer.h"
#include "init.h"
#include "nss.h"
#include "obs.h"
#include "signals.h"
#include "state.h"
#include "sysparms.h"

// initialization structures
extern queue_spec_t Queues[];			/* array of system queues */
extern sem_spec_t Sems[];				/* array of system semaphores */
extern file_spec_t Files[];				/* array of files */
extern buffer_spec_t Buffers[];			/* array of buffers */
extern connection_t *Connections[];		/* array of connections */
extern client_t *Clients[];				/* array of clients */
extern task_spec_t Tasks[];				/* array of system tasks */
extern sig_spec_t Handlers[];			/* array of signal handlers */

#ifdef GLOBAL_VARS

int ObsId = -1;
int LDaddFd[2], RDaddFd[2];
int LCDFd[2], RCDFd[2];
int DaddCollectFd[2], DaddDetectFd[2];
int CDCollectFd[2], CDDetectFd[2];
uint64_t *DaddTable;					/* DADD ushort -> long long table */
Buffer *DaddBuf;
Buffer *CDBuf;
Buffer *SliceBuf;
Buffer *ProcBuf;
Buffer *ChanBuf;
obs_t Obs[2];
queue_t *CommandQ;						/* command processor queue */
sem_t DaddSem;
sem_t CDSem;
State State;							/* system state */
SysParms SysParms;						/* system configuration parameters */

#else

extern int ObsId;
extern int LDaddFd[2], RDaddFd[2];
extern int LCDFd[2], RCDFd[2];
extern int DaddCollectFd[2], DaddDetectFd[2];
extern int CDCollectFd[2], CDDetectFd[2];
extern uint64_t *DaddTable;
extern Buffer *DaddBuf;
extern Buffer *CDBuf;
extern Buffer *SliceBuf;
extern Buffer *ProcBuf;
extern Buffer *ChanBuf;
extern obs_t Obs[2];
extern queue_t *CommandQ;				/* command processor queue */
extern queue_t *WaitCollectQ, *WaitDetectQ, *WaitConfirmQ;
extern queue_t *LDaddInputQ, *RDaddInputQ;
extern queue_t *LDaddCollectQ, *RDaddCollectQ;
extern queue_t *LCDInputQ, *RCDInputQ;
extern queue_t *LCDCollectQ, *RCDCollectQ;
extern queue_t *DetectQ;
extern queue_t *ConfirmQ;
extern sem_t DaddSem;
extern sem_t CDSem;
extern State State;
extern SysParms SysParms;

#endif /* GLOBAL_VARS */

#endif /* _GLOBALS_H */