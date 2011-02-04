/*******************************************************************************

 File:    watchdog.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/watchdog.cc,v $
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/watchdog.cc,v 1.1.1.1 2006/08/02 05:30:26 kes Exp $
** $Log: watchdog.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:26  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:56  kes
** *** empty log message ***
**
Revision 1.1  98/10/29  11:43:03  kes
NSS version

**********************************************************************/

#include "nss.h"
#include "olock.h"
#include "watchdog.h"


WD::WD()
{
	err = (error_t) lock;
	count = 0;
}

WD::~WD()
{
}

/*
**		Each time the watchdog timer fires, decrement the count
**	and return the result.
**
**	If the decremented count = 0, no characters came in the alloted time.
**	If the decremented count > 0, characters arrived; some during the
**	timeout interval of the current timer.
*/
int32_t
WD::GetCount()
{
	int32_t c;
	Lock();
	c = --count;
	Unlock();
	return (c);
}

/*
**	Each time a "TEST" messageis received, increment the count.
*/
void
WD::TestMsgRecvd()
{
	Lock();
	++count;
	Unlock();
}
