/*******************************************************************************

 File:    errlist.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/errlist.cc,v $
**
** NSS error list
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/errlist.cc,v 1.1.1.1 2006/08/02 05:30:25 kes Exp $
** $Log: errlist.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:25  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:54  kes
** *** empty log message ***
**
 * Revision 1.11  98/10/29  11:42:57  kes
 * NSS version
 * 
**********************************************************************/

#include "nss.h"
#include "errlist.h"

/* definitions */
#define NSS_MAX_ERROR_STRING		40

/* Error entry structure: the array of these must be ordered  by code */
typedef struct t_error {
	error_t code;						/* event code */
	char msg[NSS_MAX_ERROR_STRING];		/* event message */
} t_error_t;

/* error entries */
t_error_t NSSError[] = {
	{ 0,			"", },
	{ NSE_IM,		"Informational message", },
	{ NSE_QSE,		"Queue send error" },
	{ NSE_QRE,		"Queue receiver error" },
	{ NSE_IBP,		"Invalid buffer pointer" },
	{ NSE_NTA,		"Null task argument" },
	{ NSE_ILL,		"Illegal logging level" },
	{ NSE_ILT,		"Invalid logging target" },
	{ NSE_OIP,		"Observation in progress" },
	{ NSE_NOIP,		"No observation in progress" },
	{ NSE_IEC,		"Invalid error code" },
	{ NSE_LCE,		"lock creation error" },
	{ NSE_IMT,		"invalid message type" },
	{ NSE_NCA,		"no clock active" },
	{ NSE_NEM,		"not enough memory" },
	{ NSE_CAM,		"can't allocate message" },
	{ NSE_NMP,		"NULL message ptr" },
	{ NSE_IMC,		"invalid message code" },
	{ NSE_IU,		"invalid Unit" },
	{ NSE_QCE,		"queue creation error" },
	{ NSE_IML,		"invalid message length" },
	{ NSE_IMD,		"invalid message data" },
	{ NSE_COA,		"can't open archive file" },
	{ NSE_CWA,		"can't write archive file" },
	{ NSE_IET,		"invalid event type" },
	{ NSE_NSBP,		"NULL sample buffer ptr" },
	{ NSE_DSE,		"Data send error" },
	{ NSE_IOL,		"Invalid observation length" },
	{ NSE_MDNE,		"MDNE??" },
	{ NSE_SCE,		"semaphore creation error" },
	{ NSE_AAM,		"already allocated messages" },
	{ NSE_QNE,		"queue does not exist" },
	{ NSE_CCS,		"can't create semaphore" },
	{ NSE_CCSM,		"can't create shared memory" },
	{ NSE_TAP,		"time is already past" },
	{ NSE_ILTP,		"invalid log type" },
	{ NSE_ULT,		"unmodifiable log type" },
	{ NSE_ILS,		"invalid log state" },
	{ NSE_COLF,		"can't open log file" },
	{ NSE_ILFN,		"invalid log file name" },
	{ NSE_TCE,		"task creation error" },
	{ NSE_FIP,		"freeing invalid msg ptr" },
	{ NSE_FNP,		"freeing NULL msg ptr" },
	{ NSE_OA,		"observation aborted" },
	{ NSE_OC,		"observation complete" },
	{ NSE_ONC,		"observation not complete" },
	{ NSE_CSE,		"clock synchronization error" },
	{ NSE_NTC,		"no time code" },
	{ NSE_ICF,		"invalid command format" },
	{ NSE_IIS,		"invalid input string" },
	{ NSE_ICT,		"invalid connection type" },
	{ NSE_NC,		"no connection" },
	{ NSE_IUD,		"invalid UTC" },
	{ NSE_IDUD,		"invalid delta UTC" },
	{ NSE_IBV,		"invlaid bin value" },
	{ NSE_CST,		"can't set time" },
	{ NSE_IYD,		"invalid year day" },
	{ NSE_IC,		"invalid command" },
	{ NSE_IAC,		"invalid argument count" },
	{ NSE_IAL,		"invalid argument list" },
	{ NSE_LOF,		"drive fault or out of limits" },
	{ NSE_IY,		"Invalid year" },
	{ NSE_IF,		"Invalid format" },
	{ NSE_STM,		"system time mismatch" },

	{ NSE_FSC,		"force socket close" },
	{ NSE_CE,		"connection error" },
	{ NSE_SNB,		"socket non-blocking" },
	{ NSE_ESC,		"error in socket communication" },
	{ NSE_OSF,		"open socket failure" },
	{ NSE_SNC,		"socket not connected" },
	{ NSE_PCC,		"peer closed connection" },
	{ NSE_CNE,		"client thread not active" },
	{ NSE_ICID,		"invalid client ID" },
	{ NSE_ICMD,		"invalid command" },
	{ NSE_SMS,		"socket mask select error" },
	{ NSE_ISS,		"invalid system state" },
	{ NSE_NIMP,		"function not implemented" },
	{ NSE_SHUT,		"shutdown system call" },
	{ NSE_NS,		"no socket" },
	{ NSE_HNF,		"host not found" },
	{ NSE_IHN,		"invalid host name" },
	{ NSE_NTD,		"nothing to do for observation" },
	{ NSE_IREQ,		"invalid request" },
	{ NSE_CVOR,		"clock value out of range" },
	{ NSE_WDTF,		"watchdog timer fired" },

	{ NSE_LAST,		"No corresponding message" },
};

int NSSErrorCount = sizeof(NSSError) / sizeof(t_error_t);

/*
** ErrLookup: look up an error code
**
*/
const char *
ErrLookup(error_t err)
{
	int lower, upper, mid, dir;

	lower = -1;
	upper = NSSErrorCount;
	while (upper - lower > 1) {
		mid = (upper + lower) / 2;
		if (err > NSSError[mid].code)
			lower = mid;
		else if (err < NSSError[mid].code)
			upper = mid;
		else
			break;
	}
	mid = (NSSError[mid].code == err ? mid : NSSErrorCount - 1);
	return (NSSError[mid].msg);
}