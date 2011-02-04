/*******************************************************************************

 File:    nssprio.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/nssprio.h,v $
**
** NSS priorities
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/nssprio.h,v 1.1.1.1 2006/08/02 05:29:55 kes Exp $
** $Log: nssprio.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:55  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:35  kes
** *** empty log message ***
**
 * Revision 1.2  98/10/29  11:32:45  kes
 * Added new task priorities.
 * 
 * Revision 1.1  98/10/27  08:18:33  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _NSSPRIO_H
#define _NSSPRIO_H

/*
** task priorities.  These are anything but arbitrary; they must
** be chosen very carefully to ensure that data is processed in
** a timely manner.  In particular, the RECV_PRIO and SEND_PRIO
** are higher than
** DET_DETECT_PRIO to ensure that network traffic is processed
** at a higher priority than signal detection, which is CPU-intensive.
*/

#define CPU_TASK_PRIO			17		/* CPU-intensive task */
#define XSERVER_PRIO			18		/* X server */
#define CHKTIME_PRIO			15		/* CheckTime */
#define MAIN_PRIO				18		/* main task priority */
#define LOG_PRIO				80		/* error logging task */
#define CONTROL_PRIO			28		/* control task */
#define DADD_INPUT_PRIO			30		/* dadd write task */
#define DADD_WRITE_PRIO			32		/* dadd write task */
#define CD_INPUT_PRIO			30		/* cd write task */
#define CD_WRITE_PRIO			33		/* cd write task */
#define DADD_READ_PRIO			31		/* dadd read task */
#define CD_READ_PRIO			31		/* cd read task */
#define DADD_PROC_PRIO			31		/* dadd processing task */
#define CD_PROC_PRIO			31		/* cd processing task */
#define DETECTION_PRIO			29		/* detector priority */
#define CONFIRMATION_PRIO		29		/* confirmation detector priority */
#define READ_PRIO				31		/* read task priority */

#endif /* _NSSPRIO_H */