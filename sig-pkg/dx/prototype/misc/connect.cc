/*******************************************************************************

 File:    connect.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/connect.cc,v $
**
** Network functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/connect.cc,v 1.1.1.1 2006/08/02 05:30:27 kes Exp $
** $Log: connect.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:27  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:53  kes
** *** empty log message ***
**
 * Revision 1.8  98/10/29  11:42:57  kes
 * NSS version
 * 
 * Revision 1.7  96/11/10  13:59:38  kes
 * Many changes to support different connection types: serial, keyboard
 * and display in addition to network.
 * 
 * Revision 1.6  96/05/15  18:25:34  kes
 * SetUnitState is now called through StateP; many changes related to
 * spontaneous disconnect problem; added error codes.
 * 
 * Revision 1.5  96/02/13  09:35:01  kes
 * Changed calls to ChangeUnitState to SetUnitState.
 * 
 * Revision 1.4  96/02/09  09:07:40  kes
 * Added Select to allow detection of disconnect in all situations.
 * 
 * Revision 1.3  96/02/05  09:53:50  kes
 * Added Close() calls where required; added ChangeUnitState calls;
 * replaced recv, send with read, write.
 * 
 * Revision 1.2  96/01/31  11:53:57  kes
 * Changed inet_connection_t to fudd_connection_t, added Close(), Recv(),
 * and Send().
 * 
 * Revision 1.1  96/01/24  13:01:01  kes
 * Initial revision
 * 
 * Revision 1.1  96/01/22  17:35:39  kes
 * Initial revision
 * 
**********************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <netdb.h>
#include <socket.h>
#include <unistd.h>
#include "nss.h"
#include "connect.h"
#include "globals.h"
#include "syslog.h"
#include "unit.h"

/* internal function prototypes */
error_t Select(connection_t *);
error_t GetError(connection_t *);

/*
** Establish: establish a connection
**
** Synopsis:
**		err = Establish(c);
**		error_t err;		return value
**		connection_t *c;
** Returns:
**		0 if successful.
**		error code if an error occurred.
** Description:
**		If the connection is passive, creates a listening sockets,
**		then waits for a connection.  If the connection is active,
**		an attempt is made to connect to the other end.
** Notes:
**		The socket is blocking, so EWOULDBLOCK should never be
**		returned.
*/
error_t
Establish(connection_t *c)
{
	/* if already have a connection, nothing to do */
	if (c->connection > 0)
		return (0);

	switch (c->type) {
	case NET_PASSIVE:
		return (Accept(c));
    case NET_ACTIVE:
		return (Connect(c));
	case SERIAL:
		return (OpenSerial(c));
	case KEYBOARD:
		return (OpenKeyboard(c));
	case DISPLAY:
		return (OpenDisplay(c));
	default:
		return (NSE_ICT);
	}
}

/*
** Accept: accept a connection
**
** Synopsis:
**		err = Accept(c);
**		error_t err;		return value
**		connection_t *c;
** Returns:
**		0 if successful.
** Notes:
**		The socket is blocking, so EWOULDBLOCK should never be
**		returned.
**
*/
error_t
Accept(connection_t *c)
{
	int s, len = sizeof(struct sockaddr_in);
	error_t err;
	struct sockaddr_in sin;

	/* create the listening socket if necessary */
	if (c->net.socket <= 0) {
		if ((err = CreateSocket(c)) || (err = Bind(c)) || (err = Listen(c)))
			return (err);
	}

	/* accept a connection */
	if ((s = accept(c->net.socket, (struct sockaddr *) &sin, &len)) < 0) {
		err = errno;
		if (errno != EWOULDBLOCK)
			Close(c);
		return (err);
	}
#ifndef notdef
	/* close the listening socket, then record the connection */
	close(c->net.socket);
	c->net.socket = 0;
#endif
	c->connection = s;
	State.SetUnitState(c->unit, IDLE);
	return (0);
}

/*
** Connect: connect to a socket
**
** Synopsis:
**		err = Connect(c);
**		error_t err;		return value
**		connection_t *c; socket to connect to
** Returns:
**		0 if successful.
*/
error_t
Connect(connection_t *c)
{
	error_t err;
	struct hostent *hp;
	struct sockaddr_in sin;

	if (c->net.socket <= 0 && (err = CreateSocket(c)))
		return (err);

	/* look up the hostname */
	if (!strlen(c->name))
		return (NSE_IHN);
	if (!(hp = gethostbyname(c->name)))
		return (NSE_HNF);

	/* create the sockaddr_in structure */
	bzero((char *) &sin, sizeof(sin));
	sin.sin_family = (short) AF_INET;
	sin.sin_port = (u_short) htons(c->net.port);
	sin.sin_addr.s_addr = *((int *) hp->h_addr);
	if (connect(c->net.socket, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		err = errno;
		Close(c);
		return (err);
	}
	c->connection = c->net.socket;
	State.SetUnitState(c->unit, IDLE);
	return (0);
}

/*
** CreateSocket: create an internet socket
**
** Synopsis:
**		err = CreateSocket(connection);
**		error_t err;			return value
**		connection_t *c;		ptr to connection structure
** Returns:
**		0 if successful.
**		error code if failure.
** Description:
**		Opens a socket, then sets standard FUDD socket options.
** Notes:
**		This function is executed whenever a socket must be created.
*/
error_t
CreateSocket(connection_t *c)
{
	int s, optval = 1;
	error_t err;
	struct linger linger;

	/* open the socket */
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return (errno);

	/* set socket options: SO_REUSEADDR, SO_KEEPALIVE, SO_LINGER */
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
			(char *) &optval, sizeof(optval)) < 0) {
		err = errno;
		close(s);
		return (err);
	}
	if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE,
			(char *) &optval, sizeof(optval)) <	0) {
		err = errno;
		close(s);
		return (err);
	}

	/* enable SO_LINGER */
	linger.l_onoff = 1;
	linger.l_linger = 0;
	if (setsockopt(s, SOL_SOCKET, SO_LINGER,
			(char *) &linger, sizeof(linger)) <	0) {
		err = errno;
		close(s);
		return (err);
	}

	/* socket was successfully created: record it */
	c->net.socket = s;
	return (0);
}

/*
** Bind: bind a socket to an address
**
** Synopsis:
**		err = Bind(connection);
**		error_t err;		return value
**		connection_t *connection; socket to bind
** Returns:
**		0 if successful.
*/
error_t
Bind(connection_t *c)
{
	error_t err;
	struct sockaddr_in sin;

	if (c->net.socket <= 0)
		return (NSE_NS);

	/* set up the socket structure */
	bzero((char *) &sin, sizeof(sin));
	sin.sin_family = (short) AF_INET;
	sin.sin_port = (u_short) htons(c->net.port);
	sin.sin_addr.s_addr = INADDR_ANY;

	if (bind(c->net.socket, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		err = errno;
		Close(c);
		return (err);
	}
	return (0);
}

/*
** Listen: listen for connections
**
** Synopsis:
**		err = Listen(c);
**		error_t err;		return value
**		connection_t *c; socket to listen on
** Returns:
**		0 if successful.
** Description:
**		Listens on the specified socket.
** Notes:
**		The socket is left as a blocking socket.
*/
error_t
Listen(connection_t *c)
{
	error_t err;

	if (c->net.socket <= 0)
		return (NSE_NS);

	/* listen on the socket */
	if (listen(c->net.socket, 1) < 0) {
		err = errno;
		Close(c);
		return (err);
	}
	return (0);
}

/*
** Close: close the specified socket
**
** Synopsis:
**		err = Close(c);
**		error_t err;		return value
**		connection_t *c; socket to close
** Returns:
**		0 if successful.
** Description:
**		Listens on the specified socket.
** Notes:
**		The socket is left as a blocking socket.
*/
error_t
Close(connection_t *c)
{
	State.SetUnitState(c->unit, DOWN);
	switch (c->type) {
	case SERIAL:
		return (CloseSerial(c));
	case KEYBOARD:
		return (CloseKeyboard(c));
	case DISPLAY:
		return (CloseDisplay(c));
	case NET_PASSIVE:
	case NET_ACTIVE:
		break;
	default:
		return (NSE_ICT);
	}

	if (c->net.socket > 0) {
		close(c->net.socket);
		c->net.socket = 0;
		if (c->type == NET_ACTIVE)
			c->connection = 0;
	}
	if (c->connection > 0) {
		close(c->connection);
		c->connection = 0;
	}
	return (0);
}

/*
** Recv: receive data from a connection
**
** Synopsis:
**		err = Recv(c, p, len);
**		error_t err;		return value
**		connection_t *c; socket to close
**		char *p;			ptr to data storage
**		int32_t len;		data length
** Returns:
**		0 if successful.
**		error code if an error occurred.
** Description:
**		Waits for the specified amount of data to be received.
** Notes:
**		If the message is too long to be sent as a single
**		send, this function can reassemble the message from
**		the constituent pieces.
**		Flags is always set to 0.
*/
error_t
Recv(connection_t *c, char *p, int32_t len)
{
	error_t err;
	int32_t cc;

	if (c->connection <= 0)
		return (NSE_NC);

	switch (c->type) {
	case SERIAL:
		return (RecvSerial(c, p, len));
	case KEYBOARD:
		return (RecvKeyboard(c, p, len));
	case DISPLAY:
		return (NSE_CRD);
	case NET_PASSIVE:
	case NET_ACTIVE:
		break;
	default:
		return (NSE_ICT);
	}
	
#ifdef notdef
	if (err = Select(c))
		return (err);
#endif

	while (len > 0) {
#ifdef notdef
		if (err = Select(c))
			return (err);
#endif
#ifndef notdef
		cc = recv(c->connection, p, len, 0);
#else
		cc = read(c->connection, p, len);
#endif
		if (cc <= 0) {
			if (errno != EWOULDBLOCK) {
				err = errno;
				Close(c);
				return (err);
			}
			else
				cc = 0;
		}
		p += cc;
		len -= cc;
	}
	return (0);
}

/*
** Send: send data over a connection
**
** Synopsis:
**		err = Send(c, p, len);
**		error_t err;		return value
**		connection_t *c; socket to close
**		char *p;			ptr to data
**		int32_t len;		data length
** Returns:
**		0 if successful.
**		error code if an error occurred.
** Description:
**		Sends the data to the specified connection.
** Notes:
**		The message is sent in pieces, if it is too long for
**		a single send.
**		Flags is always set to 0.
*/
error_t
Send(connection_t *c, char *p, int32_t len)
{
	error_t err;
	int32_t cc;

	if (c->connection <= 0)
		return (NSE_NC);

	switch (c->type) {
	case SERIAL:
		return (SendSerial(c, p, len));
	case KEYBOARD:
		return (NSE_CSK);
	case DISPLAY:
		return (SendDisplay(c, p, len));
	case NET_PASSIVE:
	case NET_ACTIVE:
		break;
	default:
		return (NSE_ICT);
	}
	
	while (len > 0) {
		cc = send(c->connection, p, len, 0);
		if (cc < 0) {
			err = errno;
			return (err);
		}
		p += cc;
		len -= cc;
	}
	return (0);
}

/*
** Select: perform a read select on the specified connection
**
** Synopsis:
**		err = Select(c);
**		error_t err;		return value
**		connection_t *c; socket to close
** Returns:
**		0 if successful.
**		error code if an error occurred.
** Description:
**		Performs a blocking select on read for the specified socket;
**		if an error has occurred, or the connection has been broken,
**		it returns an error code.
** Notes:
**		Determination of the loss of a connection turns out to be
**		very complicated.  In the first place, loss of the connection
**		does not appear as an exception condition in the select return
**		mask.  Nor does getsockopt(SO_ERROR) return any indication.
**		Instead, the read vector indicates that read data is available,
**		but an ioctl(FIONREAD) returns an indication that 0 bytes are
**		available; this is then used to deduce that the socket is
**		no longer connected.
*/
error_t
Select(connection_t *c)
{
	FUNCTION("Select");
	char buf;
	error_t err;
	int32_t cc;
	fd_set rfds, efds;

	if (c->connection <= 0) {
		return (SysLog(NSE_NC, LOG_SYS, function, "no connection to %s",
				GetUnitAlias(c->unit)));
	}

	FD_ZERO(&rfds);
	FD_ZERO(&efds);
	FD_SET(c->connection, &rfds);
	FD_SET(c->connection, &efds);

	while (select(64, &rfds, NULL, NULL, NULL) < 0) {
		if (errno != EINTR) {
			SysLog(NSE_NE, LOG_SYS, function, "select on %s, err %d",
					GetUnitAlias(c->unit), errno);
			err = errno;
			Close(c);
			return (err);
		}
	}

#ifdef notdef
	if (err = GetError(c))
		return (err);

	if (FD_ISSET(c->connection, &efds)) {
		if (recv(c->connection, &buf, 0, 0) < 0)
			SysLog(NSE_NE, LOG_SYS, function, "efd, recv errno = %d", errno);
		else
			SysLog(NSE_NE, LOG_SYS, function, "efd, recv succeeded");
		Close(c);
		SysLog(NSE_NE, LOG_SYS, function, "efd on %s, err %d",
				GetUnitAlias(c->unit), NSE_ESC);
		return (NSE_ESC);
	}
#endif
	if (FD_ISSET(c->connection, &rfds)) {
		if (ioctl(c->connection, FIONREAD, (char *) &cc)) {
			err = errno;
			SysLog(NSE_NE, LOG_SYS, function, "ioctl on %s, err %d",
					GetUnitAlias(c->unit), err);
			return (err);
		}
		if (cc == 0) {
			SysLog(NSE_NE, LOG_SYS, function, "cc = 0 on %s",
					GetUnitAlias(c->unit));
			return (NSE_PCC);
		}
		errno = 0;
		if (recv(c->connection, &buf, 0, 0) < 0) {
			err = errno;
			SysLog(NSE_NE, LOG_SYS, function, "read on %s, err %d",
					GetUnitAlias(c->unit), err);
			return (err);
		}
		return (0);
	}

	/* shouldn't get here */
	SysLog(NSE_NE, LOG_SYS, function, "%s, err %d", GetUnitAlias(c->unit),
			NSE_SMS);
	return (NSE_SMS);
}

#ifdef notdef
error_t
GetError(connection_t *c)
{
	int err, size;

	size = sizeof(err);

	if (getsockopt(c->connection, SOL_SOCKET, SO_ERROR, &err, &size))
		return (errno);
	return (err);
}
#endif

/*
** OpenSerial: open the serial port
**
** Synopsis:
**		err = OpenSerial(connection);
**		error_t err;			return value
**		connection_t *c;		ptr to connection structure
** Returns:
**		0 if successful.
**		error code if failure.
** Description:
**		Opens the specified port and sets the operating mode.
** Notes:
**		None.
*/
error_t
OpenSerial(connection_t *c)
{
	error_t err = 0;
	struct termios oldtermio;

	// check for already open
	if (c->connection > 0)
		return (0);

	// open the port
	if ((c->connection = open(c->name, c->serial.mode)) < 0)
		return (errno);

	// flush all output
	if (tcflush(c->connection, TCIOFLUSH)) {
		err = (error_t) errno;
		close(c->connection);
		c->connection = -1;
		return (err);
	}

	// port has been successfully opened; set its speed and flags
	if (tcgetattr(c->connection, &oldtermio)) {
		err = (error_t) perror;
		close(c->connection);
		c->connection = -1;
		return (err);
	}

	tcflush(c->connection, TCIFLUSH);

	// set the port attributes
	if (tcsetattr(c->connection, TCSANOW, &c->serial.termio)) {
		err = (error_t) perror;
		close(c->connection);
		c->connection = -1;
		return (err);
	}
	return (0);
}
	
/*
** CloseSerial: close the specified serial port
**
** Synopsis:
**		err = Close(c);
**		error_t err;		return value
**		connection_t *c; 	connection to close
** Returns:
**		0 if successful.
** Description:
**		Flushes all input and output, then closes the port.
** Notes:
**		None.
*/
error_t
CloseSerial(connection_t *c)
{
	error_t err = 0;

	if (c->connection > 0) {
		// flush all pending I/O
		tcflush(c->connection, TCIOFLUSH);
		if (close(c->connection))
			err = (error_t) perror;
	}
	c->connection = -1;
	return (err);
}
	
/*
** RecvSerial: receive from the serial port
**
** Synopsis:
**		err = RecvSerial(c, p, len);
**		error_t err;		return value
**		connection_t *c;	connection to receive from
**		char *p;			ptr to data storage
**		int32_t len;		data length
** Returns:
**		0 if successful.
**		error code if an error occurred.
** Description:
**		Waits for the specified amount of data to be received.
** Notes:
**		This will probably normally be called with to read a single
**		character.
*/
error_t
RecvSerial(connection_t *c, char *p, int32_t len)
{
	error_t err;
	int32_t cc;

	while (len > 0) {
		cc = read(c->connection, p, len);
		if (cc <= 0) {
			if (errno != EWOULDBLOCK) {
				err = errno;
				return (err);
			}
			else
				cc = 0;
		}
		p += cc;
		len -= cc;
	}
	return (0);
}

/*
** SendSerial: send data to a serial port
**
** Synopsis:
**		err = SendSerial(c, p, len);
**		error_t err;		return value
**		connection_t *c; socket to close
**		char *p;			ptr to data
**		int32_t len;		data length
** Returns:
**		0 if successful.
**		error code if an error occurred.
** Description:
**		Sends the data to the specified serial port.
** Notes:
**		Could be modified to send the data in chunks as in
**		Send(), if necessary.
*/
error_t
SendSerial(connection_t *c, char *p, int32_t len)
{
	int cc;

	if ((cc = write(c->connection, p, len)) != len)
		return ((error_t) perror);
	return (0);
}

/*
** OpenKeyboard: open the console keyboard
**
** Synopsis:
**		err = OpenKeyboard(connection);
**		error_t err;			return value
**		connection_t *c;		ptr to connection structure
** Returns:
**		0 if successful.
**		error code if failure.
** Description:
**		Duplicates the keyboard (standard input) into another file
**		descriptor.
** Notes:
**		None.
*/
error_t
OpenKeyboard(connection_t *c)
{
	// duplicate the standard input
	if ((c->connection = dup(0)) < 0)
		return (errno);
	return (0);
}
	
/*
** OpenDisplay: open the console display
**
** Synopsis:
**		err = OpenDisplay(connection);
**		error_t err;			return value
**		connection_t *c;		ptr to connection structure
** Returns:
**		0 if successful.
**		error code if failure.
** Description:
**		Duplicates the console display (standard output) into another file
**		descriptor.
** Notes:
**		None.
*/
error_t
OpenDisplay(connection_t *c)
{
	// duplicate the standard input
	if ((c->connection = dup(1)) < 0)
		return (errno);
	return (0);
}
	
/*
** CloseKeyboard: close the console keyboard
**
** Synopsis:
**		err = CloseKeyboard(c);
**		error_t err;		return value
**		connection_t *c; 	connection to close
** Returns:
**		0 if successful.
** Description:
**		Closes the keyboard port, which is a duplicate of standard input.
** Notes:
**		None.
*/
error_t
CloseKeyboard(connection_t *c)
{
	if (c->connection > 0)
		close(c->connection);
	c->connection = -1;
	return (0);
}

/*
** CloseDisplay: close the console display
**
** Synopsis:
**		err = CloseDisplay(c);
**		error_t err;		return value
**		connection_t *c; 	connection to close
** Returns:
**		0 if successful.
** Description:
**		Closes the console display, which is a duplicate of standard output.
** Notes:
**		None.
*/
error_t
CloseDisplay(connection_t *c)
{
	if (c->connection > 0) {
		// flush remaining output
		fsync(c->connection);
		close(c->connection);
	}
	c->connection = -1;
	return (0);
}

/*
** RecvKeyboard: receive data from the console keyboard
**
** Synopsis:
**		err = RecvKeyboard(c, p, len);
**		error_t err;		return value
**		connection_t *c;	connection to receive from
**		char *p;			ptr to data storage
**		int32_t len;		data length
** Returns:
**		0 if successful.
**		error code if an error occurred.
** Description:
**		Waits for the specified amount of data to be received.
** Notes:
**		This will probably normally be called with to read a single
**		character.
*/
error_t
RecvKeyboard(connection_t *c, char *p, int32_t len)
{
	error_t err;
	int32_t cc;

	while (len > 0) {
		cc = read(c->connection, p, len);
		if (cc <= 0) {
			if (errno != EWOULDBLOCK) {
				err = errno;
				Close(c);
				return (err);
			}
			else
				cc = 0;
		}
		p += cc;
		len -= cc;
	}
	return (0);
}

/*
** SendDisplay: send data to the console display
**
** Synopsis:
**		err = SendDisplay(c, p, len);
**		error_t err;		return value
**		connection_t *c; socket to close
**		char *p;			ptr to data
**		int32_t len;		data length
** Returns:
**		0 if successful.
**		error code if an error occurred.
** Description:
**		Sends the data to the console display.
** Notes:
**		Could be modified to send the data in chunks as in
**		Send(), if necessary.
*/
error_t
SendDisplay(connection_t *c, char *p, int32_t len)
{
	int cc;

	if ((cc = write(c->connection, p, len)) != len)
		return ((error_t) perror);
	return (0);
}

