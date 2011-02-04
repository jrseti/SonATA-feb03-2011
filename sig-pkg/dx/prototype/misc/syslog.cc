/*******************************************************************************

 File:    syslog.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/syslog.cc,v $
**
** System error logging functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/syslog.cc,v 1.1.1.1 2006/08/02 05:30:26 kes Exp $
** $Log: syslog.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:26  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:55  kes
** *** empty log message ***
**
 * Revision 1.19  98/10/29  11:43:01  kes
 * NSS version
 * 
 * Revision 1.18  97/01/31  03:52:23  kes
 * Removed SysReply, modified SysMsg.
 * 
 * Revision 1.17  97/01/30  18:42:43  kes
 * Added SysMsg, SysReply.  SysReply will be removed.
 * 
 * Revision 1.16  97/01/29  11:34:17  kes
 * Added several new log types.
 * 
 * Revision 1.15  96/11/10  14:10:07  kes
 * Changes to convert from FUDD to TC.
 * 
 * Revision 1.14  96/05/15  18:33:48  kes
 * Changed log types, added immediate flush of output to files, changed
 * message format, added SysFatal.
 * 
 * Revision 1.13  96/04/02  09:12:07  kes
 * Changed order of formatted log output; result is horrible, and
 * will be modified in the next version.
 * 
 * Revision 1.12  96/02/13  09:36:02  kes
 * Changed SetLogType to allow setting of all log types with one call,
 * and allow changing of target for HI_ERROR and FATAL.
 * 
 * Revision 1.11  96/02/09  09:12:46  kes
 * Replaced manual thread creation with call to CreateTask.
 * 
 * Revision 1.10  96/02/05  22:34:19  kes
 * Added SetLogUnit, added name string to error structure, changed
 * SendToScs to SendMsg.
 * 
 * Revision 1.9  96/01/31  11:58:21  kes
 * Changed SysLog prototype function definition, made changes for C++,
 * changed header files, changed calls to send messages.
 * 
 * Revision 1.8  96/01/26  10:47:44  kes
 * Major changes to allow logging to a user-specified file; changed
 * SysLogLookup to ErrLookup.
 * 
 * Revision 1.7  96/01/09  13:41:43  kes
 * Added printf where error return is inappropriate.
 * 
 * Revision 1.6  95/04/03  21:34:50  kes
 * Print more detail if invalid message type received by SysLogTask.
 * 
 * Revision 1.5  95/03/09  13:32:29  kes
 * Added handling for system errors, eliminated all level and
 * mask-related code.
 * 
 * Revision 1.4  95/03/05  16:04:34  kes
 * Added code to handle multiple targets, changed message format, added code
 * to open logfile when first LOG_DISK type is enabled.
 * 
 * Revision 1.3  95/02/25  08:47:46  kes
 * Installed error codes, fixed search function.
 * 
 * Revision 1.2  95/02/12  18:51:06  kes
 * Added production code: SetLogLevel, etc.  Need to fix the
 * binary search in SysLogLookup.
 * 
 * Revision 1.1  95/02/11  22:13:04  kes
 * Initial revision
 * 
 * Revision 1.1  95/02/09  11:27:52  kes
 * Initial revision
 * 
 * Revision 1.5  95/02/04  11:22:45  kes
 * Changed to send messages to a log task instead of printing
 * them directly.
 * 
 * Revision 1.4  94/11/06  15:55:00  kes
 * Added include of fftsig.h.
 * 
 * Revision 1.3  94/10/31  11:53:01  kes
 * Increased size of message buffers.
 * 
 * Revision 1.2  94/08/19  07:24:06  kes
 * Fixed bug in sprintf call.
 * 
 * Revision 1.1  94/08/12  14:40:30  kes
 * Initial revision
 * 
**********************************************************************/

#include <varargs.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "nss.h"
#include "errlist.h"
#include "exit.h"
#include "ftime.h"
#include "globals.h"
#include "lock.h"
#include "nssmsg.h"
#include "queue.h"
#include "sendmsg.h"
#include "syslog.h"
#include "nssprio.h"
#include "unit.h"
#include "util.h"

/* misc defines */
#define LOGQ_TIMEOUT			10		/* timeout before disk flush */

tid_t SysLogTid;
char SysLogName[MAX_NAME_LEN];
FILE *SysLogFp = NULL;
queue_t *SysLogQ;
unit_t SysLogUnit = Control;
flock_t SysFatalLock;

/*
** current log type state array; the order of this array is critical;
** it must be ordered by ascending value of log type, and there must be
** no holes in the array.
*/
/* log type control structure */
typedef struct loginfo {
	int32_t type;						/* log type */
	char name[16];						/* log type name */
	int32_t state;						/* log state (ON/OFF) */
	int32_t target;						/* log target */
	FILE *logFp;						/* log file pointer */
	char logName[MAX_NAME_LEN];			/* log file name */
} loginfo_t;

loginfo_t SysLogInfo[] = {
	{ LOG_DATA,			"DATA",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_RAWENCODER,	"R",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_DRIVE,		"DRV",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_PLC,			"PLC",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_ENCODER,		"ENC",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_CMD,			"C",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_MOTION,		"M",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_SYS,			"S",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_DEBUG,		"D",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_TRACE,		"T",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_GENERAL,		"G",			LOG_OFF,	LOG_TO_FILE, NULL },
	{ LOG_SERIAL,		"SIO",			LOG_OFF,	LOG_TO_LOGFILE, NULL },
	{ LOG_INET,			"NET",			LOG_OFF,	LOG_TO_LOGFILE, NULL },
	{ INFO,				"I",		 	LOG_OFF,	LOG_TO_LOGFILE, NULL },
	{ WARNING,			"W",			LOG_ON,		LOG_TO_CONTROL, NULL },
	{ HI_ERROR,			"E",			LOG_ON,		LOG_TO_CONTROL, NULL },
	{ FATAL,			"F",			LOG_ON, 	LOG_TO_CONTROL, NULL }
};

#define NLOGTYPES	(sizeof(SysLogInfo) / sizeof(loginfo_t))

/* function prototypes */
task_t SysLogTask(void *);
static error_t OpenLogFile(loginfo_t *);
static void MakeSysLogName(char *);

extern int Clock;

/*
** SysLogInit: initialize system error logging
**
** Synopsis:
**		err = SysLogInit(void);
**		error_t err;		return value
** Returns:
**		0 if successful.
**		error code if an error occcurred.
** Description:
**		Initializes the system error logging mechanism by creating
**		the system logging task and creating the system log queue.
** Notes:
**		This function should be called only once at the beginning
**		of execution.
*/
error_t
SysLogInit(void)
{
	static int first = 1;
	int i;
	error_t err;

	/* create the Logging Queue */
	if (!(SysLogQ = QCreate("SysLogQ", SYSLOG_QUEUE_SIZE))) {
		printf("Can't create logging queue\n");
		return (NSE_QCE);
	}

	/* create the fatal error lock */
	if (err = CreateLock(&SysFatalLock))
		return (err);

	/* execute initialization only one time */
	if (first) {
		/* scan the list of log types, looking for log files to open */
		for (i = 0; i < NLOGTYPES; i++) {
			switch (SysLogInfo[i].target) {
			case LOG_TO_LOGFILE:
			case LOG_TO_CONTROL:
			case LOG_TO_FILE:
				if (SysLogInfo[i].state == LOG_ON
						&& (err = OpenLogFile(&SysLogInfo[i]))) {
					return (err);
				}
				break;
			default:
				break;
			}
		}

		/* create the task */
		if (err = CreateTask(&SysLogTid, LOG_PRIO, SysLogTask,
				(void *) SysLogQ)) {
			return (err);
		}
		first = 0;
	}
	return (0);
}

/*
** SysLogExit: shut down the system error logging
**
** Synopsis:
**		void SysLogExit(void);
** Returns:
**		Nothing.
** Description:
**		Flushes, then closes all open log files.
** Notes:
**		This function should be called only once at the end
**		of system operation
*/
void
SysLogExit(void)
{
	int i;

	for (i = 0; i < NLOGTYPES; i++) {
		switch (SysLogInfo[i].target) {
		case LOG_TO_LOGFILE:
		case LOG_TO_FILE:
			if (SysLogInfo[i].state == LOG_ON) {
				fflush(SysLogInfo[i].logFp);
				fclose(SysLogInfo[i].logFp);
			}
			break;
		default:
			break;
		}
	}
}
		
/*
** SysLog: log an event
**
** Synopsis:
**		rval = SysLog(err, type, function, va_alist);
**		error_t rval;			return value
**		int32_t type;			event type
**		const char *function;	function name
**		va_alist;				varargs arg list
** Returns:
**		original error code.
** Description:
**		If the error type is currently enabled for logging, formats an
**		error message then sends it to the log task queue for later
**		disposal.
** Notes:
**		This function must be very fast, since it may run at a high
*		priority; therefore, it simply determines whether logging is
**		enabled, then formats the message and sends it.  Actual
**		targeting of the message is controlled by the logging task
**		which receives the messages.
*/		
error_t
SysLog(error_t err, int32_t type, const char *function, const char *va_alist, ...)
{
	static struct timeval timeout;
	const char *str, *fmt;
	char *unit, *desc, msg[MAX_ERROR_STRING*2];
	int i;
	int32_t target;
	nssmsg_t *p;
	loginfo_t *log;
	nss_bc_tm_t tm;
	nss_error_t errmsg;
	va_list ap;

	/* make sure this is a legal log type */
	if (type < LOG_TYPE_MIN || type > LOG_TYPE_MAX) {
		printf("Invalid logging type: %d\n", type);
		return (err);
	}

	/* see if logging is enabled for this type */
	log = &SysLogInfo[type-LOG_TYPE_MIN];
	if (log->state != LOG_ON)
		return (err);

	/* look up the error code; treat system errors as a special case */
	if (err >= EPERM && err <= ENOTSUP) {
		if (!(str = (char *) strerror(err)))
			str = "Unknown system error";
	}
	else if (err >= 0)
		str = ErrLookup(err);

	/* build the ERROR_MSG structure */
	bzero((char *) &errmsg, sizeof(errmsg));
	errmsg.code = err;
	errmsg.severity = type;
	desc = errmsg.description;

	/* first build the standard message */
	if (Clock)
		GetTime(&tm, 0);
	else
		LynxTime(&tm);
	unit = GetUnitAlias(GetUnit());

	if (!function)
		function = "";

	if (err >= 0) {
		if (!err) {
			sprintf(desc, "%2s %03d.%02d:%02d:%02d <%.15s>: ", unit,
					tm.tm_yday, tm.tm_hour, tm.tm_min, tm.tm_sec, function);
		}
		else {
			sprintf(desc, "%2s %03d.%02d:%02d:%02d <%.15s>(%s%d): %s\n", unit,
					tm.tm_yday, tm.tm_hour, tm.tm_min, tm.tm_sec, function,
					log->name, err, str);
		}
	}

	bzero(msg, sizeof(msg));

	/* now format the optional arguments */
	va_start(ap);
	fmt = va_arg(ap, char *);
	if (fmt) {
		vsprintf(msg, fmt, ap);
		strcat(msg, "\n");
	}
	strncat(desc, msg, MAX_ERROR_STRING - (strlen(desc) + 1));

	/* allocate a message packet */
	p = AllocMsg(sizeof(nss_error_t));

	/*
	** if we are logging to the SCS, but there is no connection,
	** log to the logfile instead.
	*/
	target = log->target;
	if (target == LOG_TO_CONTROL && !State.UnitActive(SysLogUnit))
		target = LOG_TO_LOGFILE;
	p->header.code = NSS_ERROR_MESSAGE;
	p->header.sender = GetUnit();
	p->header.receiver = target;
	p->header.activity_id = State.GetActivityId();
	if (target == LOG_TO_LOGFILE || target == LOG_TO_FILE)
		p->header.request_id = (long) log->logFp;
	else
		p->header.request_id = RequestId();
	bcopy((char *) &errmsg, (char *) p->data, sizeof(nss_error_t));

	/* send it to the log task, which runs in the background */
	if (QSend(SysLogQ, p, &timeout))
		FreeMsg(p);
	return (err);
}
	
/*
** SysMsg: send a message to the specified unit
**
** Synopsis:
**		rval = SysMsg(err, type, unit, va_alist);
**		error_t rval;			return value
**		error_t err;			error code
**		sysmsg_type_t type;		message type
**		unit_t unit;			destination unit
**		va_alist;				varargs arg list
** Returns:
**		original error code.
** Description:
**		Sends the message to the unit currently in control; if no
**		unit is currently in control, logs the message to the log
**		file.
** Notes:
**		This function is intended to allow easy writing of formatted
**		messages to the controlling unit.
**		Message type is used to control the type of header string
**		preceding the message output; if MsgNone is used, the message
**		will be sent to the log file if SetiObs is in control.
**		This function must be very fast, since it may run at a high
*		priority; therefore, it simply determines whether logging is
**		enabled, then formats the message and sends it.  Actual
**		targeting of the message is controlled by the logging task
**		which receives the messages.
*/		
error_t
SysMsg(error_t err, sysmsg_type_t type, unit_t unit, const char *va_alist, ...)
{
	static struct timeval timeout;
	const char *str, *fmt;
	char *desc, msg[MAX_ERROR_STRING*2];
	int i;
	int32_t target;
	nssmsg_t *p, *q;
	nss_error_t errmsg;
	va_list ap;

	/* look up the error code; treat system errors as a special case */
	if (err >= EPERM && err <= ENOTSUP) {
		if (!(str = (char *) strerror(err)))
			str = "Unknown system error";
	}
	else if (err >= 0)
		str = ErrLookup(err);

	/* build the ERROR_MSG structure */
	bzero((char *) &errmsg, sizeof(errmsg));
	errmsg.code = 0;
	errmsg.severity = INFO;
	desc = errmsg.description;

	/* the header string depends on the message type */
	switch (type) {
	case MsgOk:
		strcpy(desc, "OK");
		break;
	case MsgInvalid:
		strcpy(desc, "INVALID");
		break;
	case MsgError:
		strcpy(desc, "ERROR:");
		break;
	case MsgMsg:
		strcpy(desc, "MSG:");
		break;
	default:
		break;
	}

	/* copy the error string to the destination */
	if (err > 0) {
		if (strlen(desc))
			strcat(desc, " ");
		strcat(desc, str);
	}

	bzero(msg, sizeof(msg));

	/* now format the optional arguments */
	va_start(ap);
	fmt = va_arg(ap, char *);
	if (fmt) {
		if (strlen(desc))
			strcat(desc, " ");
		vsprintf(msg, fmt, ap);
	}
	/* if there's no message, quit now */
	if (!strlen(desc) && !strlen(msg))
		return (err);

	strcat(msg, "\n");
	strncat(desc, msg, MAX_MSG_STRING - (strlen(desc) + 1));

	/* allocate a message packet */
	p = AllocMsg(sizeof(nss_error_t));

	/* log to specified unit */
	target = LOG_TO_LOGFILE;
	p->header.request_id = (long) SysLogFp;
	p->header.code = NSS_ERROR_MESSAGE;
	p->header.sender = GetUnit();
	p->header.receiver = target;
	p->header.activity_id = State.GetActivityId();
	bcopy((char *) &errmsg, (char *) p->data, sizeof(nss_error_t));

	/* send it to the log task, which runs in the background */
	if (QSend(SysLogQ, p, &timeout))
		FreeMsg(p);

	return (err);
}

/*
** SysMsg: send a message to the specified unit
**
** Synopsis:
**		rval = SysMsg(err);
**		error_t rval;			return value
**		error_t err;			error code
** Returns:
**		original error code.
** Description:
**		Sends the message to the unit currently in control; if no
**		unit is currently in control, logs the message to the log
**		file.
** Notes:
**		This function is intended to allow easy writing of standard
**		messages to the controlling unit.
**		The message type is MsgNone, the destination is Control,
**		and there is no formatted message.
**		If the controlling unit is SetiObs, the message is written
**		to the log file.
*/		
error_t
SysMsg(error_t err)
{
	return (SysMsg(err, MsgNone, Control, NULL));
}

/*
** SysMsg: send a message to the specified unit
**
** Synopsis:
**		rval = SysMsg(va_alist);
**		error_t rval;			return value
**		va_alist;				varargs arg list
** Returns:
**		original error code.
** Description:
**		Sends the message to the unit currently in control; if no
**		unit is currently in control, logs the message to the log
**		file.
** Notes:
**		This function is intended to allow easy writing of formatted
**		messages to the controlling unit.
**		The error code is 0, the message type is MsgNone, and
**		the destination is the controlling unit.
*/		
error_t
SysMsg(const char *va_alist, ...)
{
	static struct timeval timeout;
	const char *str, *fmt;
	char *desc, msg[MAX_ERROR_STRING*2];
	int i;
	int32_t target;
	nssmsg_t *p, *q;
	nss_error_t errmsg;
	unit_t unit;
	va_list ap;

	/* build the ERROR_MSG structure */
	bzero((char *) &errmsg, sizeof(errmsg));
	errmsg.code = 0;
	errmsg.severity = INFO;
	desc = errmsg.description;

	bzero(msg, sizeof(msg));

	/* now format the optional arguments */
	va_start(ap);
	fmt = va_arg(ap, char *);
	if (fmt)
		vsprintf(msg, fmt, ap);
	/* if there's no message, quit now */
	if (!strlen(msg))
		return (0);

	strcat(msg, "\n");
	strncat(desc, msg, MAX_MSG_STRING - 1);

	/* allocate a message packet */
	p = AllocMsg(sizeof(nss_error_t));

	/* log to controlling unit */
	target = LOG_TO_LOGFILE;
	p->header.request_id = (long) SysLogFp;
	p->header.code = NSS_ERROR_MESSAGE;
	p->header.sender = GetUnit();
	p->header.receiver = target;
	p->header.activity_id = State.GetActivityId();
	bcopy((char *) &errmsg, (char *) p->data, sizeof(nss_error_t));

	/* send it to the log task, which runs in the background */
	if (QSend(SysLogQ, p, &timeout))
		FreeMsg(p);

	return (0);
}

/*
** SysLogTask: process an error message
**
** Synopsis:
**		task_t SysLogTask(queue);
**		queue_t *queue;			input queue
** Returns:
**		A task, never returns.
** Description:
**		Waits on its input queue for log messages; when one is
**		received sends it to the appropriate or simply prints
**		it on the console.
**		simply prints on the console.
** Notes:
**		None.
*/
task_t
SysLogTask(void *arg)
{
	char *msg, fname[128];
	int i;
	error_t err;
	nssmsg_t *p;
	queue_t *inQ = (queue_t *) arg;
	nss_error_t *errmsg;

	while (1) {
		p = (nssmsg_t *) QReceive(inQ);

		if (p->header.code != NSS_ERROR_MESSAGE) {
			printf("SysLogTask: invalid message type = %d (0x%x)\n",
					p->header.code);
			continue;
		}

		/* send the message to the appropriate target */
		switch (p->header.receiver) {
#ifdef notdef
		case LOG_TO_CONTROL:
			p->header.receiver = State.GetControl();
			SendMsg(p);
			break;
		case LOG_TO_CONSOLE:
			errmsg = (nss_error_t *) p->data;
			msg = errmsg->description;
			printf(msg);
			break;
#endif
		case LOG_TO_LOGFILE:
		case LOG_TO_FILE:
			/* logging to a file; request_id contains the file pointer */
			errmsg = (nss_error_t *) p->data;
			msg = errmsg->description;
			fwrite(msg, strlen(msg), 1, (FILE *)  p->header.request_id);
			fflush((FILE *) p->header.request_id);
			break;
		case LOG_TO_UNIT:
			/* logging to a unit; request_id contains the unit id */
			p->header.receiver = p->header.request_id;
			SendMsg(p);
			break;
		default:
			break;
		}
		FreeMsg(p);
	}
}

/*
** SysFatal: log a fatal event, then exit
**
** Synopsis:
**		rval = SysLog(err, function, va_alist);
**		error_t rval;			return value
**		const char *function;	function name
**		va_alist;				varargs arg list
** Returns:
**		Calls Exit; Does not return.
** Description:
**		Formats an error message, sends it to the log destination,
**		waits a little for the message to be sent, then exits.
** Notes:
**		No allocation is done; instead, a local buffer is used.
**		A lock prevents mutltiple or recursive calls to SysFatal.
*/		
void
SysFatal(error_t err, const char *function, const char *va_alist, ...)
{
	const char *str, *fmt;
	char *unit, *desc, msg[MAX_ERROR_STRING*2];
	int i;
	int32_t target;
	nssmsg_t *p, pkt;
	loginfo_t *log;
	nss_bc_tm_t tm;
	nss_error_t errmsg;
	va_list ap;

	/* this code may be entered only one time, so try to get the lock */
	Lock(&SysFatalLock);

	/* get the control structure for a fatal error */
	log = &SysLogInfo[FATAL-LOG_TYPE_MIN];

	/* look up the error code; treat system errors as a special case */
	if (err >= EPERM && err <= ENOTSUP) {
		if (!(str = (char *) strerror(err)))
			str = "Unknown system error";
	}
	else if (err >= 0)
		str = ErrLookup(err);

	/* build the ERROR_MSG structure */
	bzero((char *) &errmsg, sizeof(errmsg));
	errmsg.code = err;
	errmsg.severity = FATAL;
	desc = errmsg.description;

	/* first build the standard message */
	if (Clock)
		GetTime(&tm, 0);
	else
		LynxTime(&tm);
	unit = GetUnitAlias(GetUnit());

	if (err >= 0) {
		if (!err) {
			sprintf(desc, "%s %03d.%02d:%02d:%02d <%.15s>:", unit,
					tm.tm_yday, tm.tm_hour, tm.tm_min, tm.tm_sec, function);
		}
		else {
			sprintf(desc, "%s %03d.%02d:%02d:%02d <%.15s> (%s%d): %s\n", unit,
					tm.tm_yday, tm.tm_hour, tm.tm_min, tm.tm_sec, function,
					log->name, err, str);
		}
	}

	bzero(msg, sizeof(msg));

	/* now format the optional arguments */
	va_start(ap);
	fmt = va_arg(ap, char *);
	if (fmt) {
		vsprintf(msg, fmt, ap);
		strcat(msg, "\n");
	}
	strncat(desc, msg, MAX_ERROR_STRING - (strlen(desc) + 1));

	/*
	** if we are logging to the SCS, but there is no connection,
	** log to the logfile instead.
	*/
	target = log->target;
	if (target == LOG_TO_CONTROL && !State.UnitActive(SysLogUnit))
		target = LOG_TO_LOGFILE;

	/* send the message to the appropriate target */
	switch (target) {
	case LOG_TO_CONTROL:
		/* initialize the message packet */
		p = &pkt;
		bzero((char *) p, sizeof(pkt));
		p->header.code = NSS_ERROR_MESSAGE;
		p->header.sender = GetUnit();
		p->header.receiver = SysLogUnit;
		p->header.length = sizeof(nss_error_t);
		p->header.activity_id = State.GetActivityId();
		p->header.request_id = RequestId();
		p->data = &errmsg;
		if (target == LOG_TO_LOGFILE || target == LOG_TO_FILE)
			p->header.request_id = (long) log->logFp;
		SendFatalMsg(p);
		break;
	case LOG_TO_CONSOLE:
		printf(desc);
		break;
	case LOG_TO_LOGFILE:
	case LOG_TO_FILE:
		/* logging to a file; request_id contains the file pointer */
		fwrite(desc, strlen(desc), 1, log->logFp);
		fflush(log->logFp);
		break;
	default:
		break;
	}

	/* shut down the system logger */
	SysLogExit();
	sleep(5);
	Exit(RESET);
}

/*
** SetLogUnit: set the unit to which SCS logging is sent
**
** Synopsis:
**		void SetLogUnit(unit);
**		fudd_unit_t unit;		unit id
** Returns:
**		Nothing.
** Description:
**		Assigns all SCS logging to the specified unit.
** Notes:
**		The default is SCS; this is the correct unit for the Master.
**		The detectors should set Unit to Master.
**		If the unit is invalid, SCS logging will always be directed
**		to the log file.
*/
void
SetLogUnit(unit_t unit)
{
	SysLogUnit = unit;
}

/*
** SetLogType: set the logging parameters for a log type
**
** Synopsis:
**		rval = SetLogType(type, state, target);
**		error_t rval;			return value
**		int32_t type;			type to set
**		int32_t state;			turn it on or off
**		int32_t target;			target to log to
**		char *logname;			name of special log file
** Returns:
**		0 if a legal type is being set.
**		error code if invalid type.
** Description:
**		Sets the logging parameters for the specified log type; this
**		includes turning logging on or off for the type and setting
**		the device to which the type will be logged
**		Sets the logging level to the specified value; this level is
**		the minimum level of error which will be logged (CONSOLE,
**		SCS, or LOGFILE).
** Notes:
**		The SCS defines four levels of error: INFO, WARN, HI_ERROR
**		and FATAL, and the application may define an arbitrary number
**		of additional level.  HI_ERROR and FATAL cannot be disabled or
**		redirected; an attempt to do so will cause an error return.
**		All log types can be set to the same state and target by
**		using the SetLogType(LOG_ALL, ...) form of the call.
*/
error_t
SetLogType(int32_t type, int32_t state, int32_t target, char *logname)
{
	int i;
	error_t rval;

	/*
	** to initialize all log types with the same state and target,
	** call each one recursively.
	*/
	if (type == LOG_ALL) {
		for (i = 0; i < NLOGTYPES; i++)
			SetLogType(SysLogInfo[i].type, state, target, logname);
	}

	/* make sure the type is legal */
	if (type < LOG_TYPE_MIN || type > LOG_TYPE_MAX)
		return (NSE_ILTP);

	if (state == LOG_OFF) {
		switch (type) {
		case FATAL:
		case HI_ERROR:
			/* these cannot be disabled */
			return (NSE_ULT);
		default:
			break;
		}
	}
	i = type - LOG_TYPE_MIN;

	SysLogInfo[i].state = state;

	switch (state) {
	case LOG_OFF:
		return (0);
	case LOG_ON:
		break;
	default:
		SysLogInfo[i].state = LOG_OFF;
		return (NSE_ILS);
	}

	switch (SysLogInfo[i].target = target) {
	case LOG_TO_CONTROL:
	case LOG_TO_CONSOLE:
		break;
	case LOG_TO_LOGFILE:
		/* logging to the standard log file */
		if (rval = OpenLogFile(&SysLogInfo[i])) {
			SysLogInfo[i].state = LOG_OFF;
			return (rval);
		}
		break;
	case LOG_TO_FILE:
		/* logging to a special file */
		if (!logname || !strlen(logname))
			return (NSE_ILFN);
		strcpy(SysLogInfo[i].logName, logname);
		if (rval = OpenLogFile(&SysLogInfo[i])) {
			SysLogInfo[i].state = LOG_OFF;
			return (rval);
		}
		break;
	default:
		return (NSE_ILT);
	}

	return (0);
}

/*
** OpenLogFile: open a log file
**
** Synopsis:
**		err = OpenLogFile(logType);
**		error_t err;		return value
**		loginfo_t *logType;	log type entry
** Returns:
**		0 if successful.
**		error code if an error occurred.
** Description:
**		Opens a log file.
** Notes:
*/
static error_t
OpenLogFile(loginfo_t *logType)
{
	int i;

	/*
	** if this entry logs to the standard system log file, create
	** the filename and open the file only if it is not already open.
	** We also use the log file for LOG_TO_CONTROL if the connection to
	** the SCS is not active.
	*/
	if (logType->target == LOG_TO_LOGFILE || logType->target == LOG_TO_CONTROL) {
		if (!SysLogFp) {
			MakeSysLogName(SysLogName);
			if (!(SysLogFp = fopen(SysLogName, "a")))
				return (NSE_COLF);
		}
		logType->logFp = SysLogFp;
		strcpy(logType->logName, SysLogName);
		return (0);
	}
	else if (logType->target == LOG_TO_FILE) {
		/* special log file: name must be valid */
		if (!logType->logName || !strlen(logType->logName))
			return (NSE_ILFN);
		/* see if another entry has already opened this file */
		for (i = 0; i < NLOGTYPES; i++) {
			if (&SysLogInfo[i] != logType
					&& SysLogInfo[i].target == LOG_TO_FILE
					&& !strcmp(SysLogInfo[i].logName, logType->logName)
					&& SysLogInfo[i].logFp)
				break;
		}
		if (i < NLOGTYPES && &SysLogInfo[i] != logType)
			logType->logFp = SysLogInfo[i].logFp;
		else if (!(logType->logFp = fopen(logType->logName, "a")))
			return (NSE_COLF);
		return (0);
	}
	else
		return (NSE_ILT);
}

static void
MakeSysLogName(char *fname)
{
	char *unit;
	nss_bc_tm_t tm;

	unit = GetUnitAlias(GetUnit());
		
	if (Clock)
		GetTime(&tm, 0);
	else
		LynxTime(&tm);
#ifdef notdef
	sprintf(fname, "%s_%03d.%02d.%02d.%02d", unit,
			tm.tm_yday, tm.tm_hour, tm.tm_min, tm.tm_sec);
#else
	sprintf(fname, "%sLog.%03d", unit, tm.tm_yday);
#endif
}