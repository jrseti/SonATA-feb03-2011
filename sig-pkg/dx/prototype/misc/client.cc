/*******************************************************************************

 File:    client.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/client.cc,v $
**
** Functions to provide access to client information
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/client.cc,v 1.1.1.1 2006/08/02 05:30:27 kes Exp $
** $Log: client.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:27  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:53  kes
** *** empty log message ***
**
Revision 1.4  98/10/29  11:42:57  kes
NSS version

Revision 1.3  96/11/10  13:55:40  kes
Changed fudd_unit_t to unit_t, removed GetRoute.

Revision 1.2  96/05/15  18:24:56  kes
Added GetRoute.

Revision 1.1  96/02/09  09:07:33  kes
Initial revision

**********************************************************************/

#include "nss.h"
#include "client.h"
#include "globals.h"
#include "unit.h"

client_t *
GetClient(unit_t unit, client_type_t type)
{
	int i;

	for (i = 0; Clients[i] && (Clients[i]->unit != unit
			|| Clients[i]->type != type); i++)
		;
	return (Clients[i]);
}