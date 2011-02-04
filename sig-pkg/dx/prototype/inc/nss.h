/*******************************************************************************

 File:    nss.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/nss.h,v $
**
** TC general header file
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/nss.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: nss.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:34  kes
** *** empty log message ***
**
 * Revision 1.2  98/10/29  11:32:16  kes
 * Added XLSIZE.
 * 
 * Revision 1.1  98/10/27  08:18:28  kes
 * Initial revision
 * 
 * Revision 1.1  96/11/10  14:28:43  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _NSS_H
#define _NSS_H

#include <errno.h>
#include "nsstypes.h"
#include "nsserr.h"

/* eternal constants */
/* clear out some old constants */
/* constants */
#define KHz						((double) 1e3)		/* convert KHz to Hz */
#define MHz						((double) 1e6)		/* convert MHz to Hz */
#undef PI
#undef TWOPI
#undef SQRT2
#define PI						((double) 3.14159265358979323846)
#define PIOVR2					(PI / 2.0)
#define TWOPI					(2 * PI)
#define SQRT2					((double) 1.41421356237309504880)
#define MAX_EXP10				((double) 307.0)
#define LOG10					((double) 2.30258509299)
#define HALFLOG2PI				((double) 0.918938533)
#define DOVERFLOW				-1000.0
#define SEC_TO_DEG				(1.0 / 3600.0)
#define DEG_TO_RAD				(PI / 180.0)
#define MIN_TO_RAD				(DEG_TO_RAD / 60.0)
#define SEC_TO_RAD				(DEG_TO_RAD / 3600.0)
#define RAD_TO_DEG				(180.0 / PI)
#define RAD_TO_MIN				((180.0 * 60.0) / PI)
#define RAD_TO_SEC				((180.0 * 3600.0) / PI)
#define DEG_PER_RAD				(RAD_TO_DEG)
#define RAD_PER_DEG				(DEG_TO_RAD)
#define RAD_TO_HR				(24.0 / TWOPI)

#define USEC_PER_SEC			((double) (1e6))
#define SEC_PER_USEC			((double) (1e-6))
#define SEC_PER_DAY				(24 * 60 * 60)
#define DAY_PER_CENTURY			((int32_t) 36525)

#define FALSE					0
#define TRUE					1

/*
** system parameters which may change
*/
#define MAX_NAME_LEN			256
#define MAX_STR_LEN				256
#define MAX_MSG_STRING			127
#define XLSIZE					65536

/*
** values for testing
*/
#define SLICE_WIDTH				32768
#define DADD_SLEEP_SEC			12
#define DADD_SLEEP_USEC			0
#define CHANNEL_WIDTH			1024
#define CD_SLEEP_SEC			6
#define CD_SLEEP_USEC			0

/* queue defaults */
#define	DEFAULT_QUEUE_SIZE		10
#define SYSLOG_QUEUE_SIZE		128		/* # of slots in SysLogQ */

/* socket definitions */
#define CONSOLE_PORT			(3800)
#define MONITOR_PORT			(CONSOLE_PORT+1)

/* internal message codes */
#define NSS_CONNECTION_MADE	5331
#define NSS_CONNECTION_LOST	5332
#define NSS_ERROR_MESSAGE	5333
#define RESET				5334
#define SHUTDOWN			5335
#define COMMAND				5336
#define NSS_ACK				5337
#define NSS_NAK				5338
#define NSS_ERR				5339
#define NSS_ECHO			5340

#endif /* _NSS_H */