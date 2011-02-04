/*******************************************************************************

 File:    client.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/client.h,v $
**
** Fudd client header file
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/client.h,v 1.1.1.1 2006/08/02 05:29:55 kes Exp $
** $Log: client.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:55  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:31  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:21  kes
 * Initial revision
 * 
 * Revision 1.5  96/11/10  14:17:33  kes
 * Changes to convert from FUDD to TC; fudd_connection_t changed to
 * connection_t, fudd_unit_t changed to unit_t, route, GetRoute
 * removed.
 * 
 * Revision 1.4  96/05/15  18:00:15  kes
 * Added route structure, GetRoute.
 * 
 * Revision 1.3  96/02/13  09:42:47  kes
 * Merged commandQ and sendQ into queue, added prototype for GetClient().
 * 
 * Revision 1.2  96/01/31  12:02:29  kes
 * Changed header files, changed client_t definition.
 * 
 * Revision 1.1  96/01/24  13:14:42  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _CLIENT_H
#define _CLIENT_H

#include "nsstypes.h"
#include "queue.h"
#include "connect.h"

/*
** client specification structure.  This structure is used by the send
** and receive tasks to process communications from the SCS and the
** detectors.
** Note that the use of queue varies by task type.  Forr a receive task,
** it is the queue to which input messages are sent.  For a send
** queue, it is the queue from which output messages are received.
*/
typedef struct client {
	unit_t unit;					/* unit ID */
	client_type_t type;				/* client type (SEND, RECEIVE) */
	queue_t **queue;				/* communication queue */
	connection_t *connection;		/* connection to client */
} client_t;

/* function prototypes */
client_t *GetClient(unit_t, client_type_t);

#endif /* _CLIENT_H */