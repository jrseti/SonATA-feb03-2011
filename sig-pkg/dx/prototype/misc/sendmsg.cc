/*******************************************************************************

 File:    sendmsg.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/sendmsg.cc,v $
**
** Scheduler task
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/sendmsg.cc,v 1.1.1.1 2006/08/02 05:30:27 kes Exp $
** $Log: sendmsg.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:27  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:55  kes
** *** empty log message ***
**
Revision 1.7  98/10/29  11:43:00  kes
NSS version

Revision 1.6  96/11/10  14:07:23  kes
Changes to convert from FUDD to TC.

Revision 1.5  96/05/24  18:29:16  kes
Added include of exit.h.

Revision 1.4  96/05/15  18:32:30  kes
Simplified calls to DupMsg, QSend; added SendFatalMsg, which uses
the buffer provided rather than attempting to duplicate it.

Revision 1.3  96/02/13  09:35:29  kes
Changed client->sendQ to client->queue.

Revision 1.2  96/02/09  09:10:07  kes
Removed SendToScs, SendToMaster, SendToDetectors, ClientInfo.

Revision 1.1  96/01/31  11:57:41  kes
Initial revision

**********************************************************************/

#include "nss.h"
#include "nssmsg.h"
#include "connect.h"
#include "exit.h"
#include "globals.h"
#include "queue.h"
#include "sendmsg.h"
#include "syslog.h"
#include "unit.h"

/*
** SendMsg: send a message to the specified client
**
** Synopsis:
**		err = SendMsg(p);
**		error_t err;			return value
**		nssmsg_t *p;			message to send
** Returns:
**		0 if successful.
**		error code if the message could not be sent.
** Description:
**		Sends the message to the specified receiver.
**		The message is duplicated before transmission, so the caller
**		must release the original message.
**		If the client is not active, the message is silently
**		discarded.
*/
error_t
SendMsg(nssmsg_t *p)
{
	FUNCTION("SendMsg");
	int i;
	client_t *client;
	nssmsg_t *q;

	if (!(client = GetClient((unit_t) p->header.receiver, SEND))) {
		return (SysLog(NSE_IU, HI_ERROR, function,
				"code = 0x%x, receiver = 0x%0x",
				p->header.code, p->header.receiver));
	}
	if (!State.UnitActive(client->unit))
		return (0);
	q = DupMsg(p);
	QSend(*client->queue, q);
	return (0);
}

/*
** SendFatalMsg: send a "fatal" message to the specified client
**
** Synopsis:
**		err = SendFatalMsg(p);
**		error_t err;			return value
**		nssmsg_t *p;			message to send
** Returns:
**		0 if successful.
**		error code if the message could not be sent.
** Description:
**		Sends the message to the specified receiver.
**		The message is NOT duplicated before transmission, so the caller
**		must NOT release the original message.
**		If the client is not active, the message is silently
**		discarded.
*/
error_t
SendFatalMsg(nssmsg_t *p)
{
	FUNCTION("SendFatalMsg");
	int i;
	client_t *client;
	nssmsg_t *q;

	if (!(client = GetClient((unit_t) p->header.receiver, SEND)))
		Exit(RESET);
	if (!State.UnitActive(client->unit))
		return (0);
	QSend(*client->queue, p);
	return (0);
}