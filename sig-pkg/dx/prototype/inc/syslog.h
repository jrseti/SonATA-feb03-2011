/*******************************************************************************

 File:    syslog.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/syslog.h,v $
**
** NSS error code and structure definitions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/syslog.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** Log: $
**********************************************************************/

#ifndef _SYSLOG_H
#define _SYSLOG_H

#include <varargs.h>

/* system message logging types: used only in SysMsg calls */
/* note that all message types except MsgResp and MsgOk echo to the console */
/* as well as the target */
typedef enum sysmsg_type {
	MsgNone = 0,						/* no message header, echo to console */
	MsgResp = 1,						/* no header, don't echo to console */
	MsgOk = 2,							/* "OK" message header */
	MsgInvalid = 3,						/* "INVALID" message header */
	MsgError = 4,						/* "ERROR:" message header */
	MsgMsg = 5							/* "MSG:" message header */
} sysmsg_type_t;

/*
** log types; we use negative numbers to distinguish our types from
** the standard SCS types, which start at 0.  Feel free to add your
** own class types, but keep in mind this IMPORTANT NOTE: when
** a new type is added, it MUST be one less than the lowest previous
** type, and LOG_MIN_TYPE must be set to its value.
*/
/* Standard error types: compatible with SCS */
#define INFO				0
#define WARNING				1
#define HI_ERROR			2
#define FATAL				3
/* TC internal log types */
#define LOG_INET			-1			/* network I/O */
#define LOG_SERIAL			-2			/* serial I/O */
#define LOG_GENERAL			-3			/* general logging */
#define LOG_TRACE			-4			/* trace */
#define LOG_DEBUG			-5			/* debug logging */
#define LOG_SYS				-6			/* system messages (start, abort, etc)*/
#define LOG_MOTION			-7			/* log of telescope movement */
#define LOG_CMD				-8			/* log of command handling */
#define LOG_ENCODER			-9			/* log of encoder data */
#define LOG_PLC				-10			/* log of PLC data */
#define LOG_DRIVE			-11			/* log of Siemens drive data */
#define LOG_RAWENCODER		-12			/* log of raw encoder data */
#define LOG_DATA			-13			/* log of motion data */

#define LOG_TYPE_MIN		LOG_DATA
#define LOG_TYPE_MAX		FATAL
#define LOG_ALL				(LOG_TYPE_MIN - 1)

/*
** log enable disable definitions; by default, all types except WARNING,
** HI_ERROR, and FATAL are disabled.
*/
#define LOG_OFF				0			/* disable logging */
#define LOG_ON				1			/* enable logging */

/*
** the logging target specifies where the output from a given log
** type will be sent.  The default for any enabled type is LOG_TO_LOGFILE.
*/
#define LOG_TO_CONSOLE		0			/* log to the program console */
#define LOG_TO_CONTROL		1			/* log to control system */
#define LOG_TO_LOGFILE		2			/* log to standard log file */
#define LOG_TO_FILE			3			/* log to a special file */
#define LOG_TO_PRINTER		4			/* log to a printer */
#define LOG_TO_PORT			5			/* log to a serial port */
#define LOG_TO_UNIT			6			/* log to a unit */

/* function prototypes */
error_t SysLogInit(void);
void SysLogExit(void);
error_t SysLog(error_t, int32_t, const char *, const char *, ...);
error_t SysMsg(error_t, sysmsg_type_t, unit_t, const char *, ...);
error_t SysMsg(error_t);
error_t SysMsg(const char *, ...);
void SysFatal(error_t, const char *, const char *, ...);
void SetLogUnit(unit_t);
error_t SetLogType(int32_t, int32_t, int32_t, char *);

#endif /* _SYSLOG_H */