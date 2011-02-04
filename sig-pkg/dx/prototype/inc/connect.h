/*******************************************************************************

 File:    connect.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/connect.h,v $
**
** Network defines
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/connect.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: connect.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:32  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:21  kes
 * Initial revision
 * 
 * Revision 1.4  96/11/10  14:19:27  kes
 * Changes to convert from FUDD to TC; fudd_connection changed
 * to connection, structures added to specify net and serial
 * connections.
 * 
**********************************************************************/

#ifndef _CONNECT_H
#define _CONNECT_H

#include <socket.h>
#include <termios.h>
#include "nsstypes.h"

/*
** connection structure: a set of these specifies the
** connections to be made by the program.
*/
typedef struct connection {
	unit_t unit;						/* unit at other end of connection */
	conn_type_t type;					/* connection type (ACTIVE/PASSIVE) */
	char name[MAX_NAME_LEN];			/* hostname (net), port name (serial) */
	/* used by net connections only */
	struct {
		int port;						/* TCP/IP port */
		int socket;						/* socket to listen to */
	} net;
	/* used by serial connections only */
	struct {
		int mode;						/* open mode (read, write, readwrite) */
		struct termios termio;			/* terminal control */
	} serial;
	int connection;						/* connection desc (socket/fd) */
} connection_t;

/* function prototypes */
error_t Establish(connection_t *);
error_t Accept(connection_t *);
error_t Connect(connection_t *);
error_t CreateSocket(connection_t *);
error_t Bind(connection_t *);
error_t Listen(connection_t *);
error_t Close(connection_t *);
error_t Recv(connection_t *, char *, int32_t);
error_t Send(connection_t *, char *, int32_t);

error_t OpenSerial(connection_t *);
error_t CloseSerial(connection_t *);
error_t RecvSerial(connection_t *, char *, int32_t);
error_t SendSerial(connection_t *, char *, int32_t);

error_t OpenKeyboard(connection_t *);
error_t OpenDisplay(connection_t *);
error_t CloseKeyboard(connection_t *);
error_t CloseDisplay(connection_t *);
error_t RecvKeyboard(connection_t *, char *, int32_t);
error_t SendDisplay(connection_t *, char *, int32_t);

#endif /* _CONNECT_H */