/*******************************************************************************

 File:    olock.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/olock.cc,v $
**
** Owned lock access methods
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/olock.cc,v 1.1.1.1 2006/08/02 05:30:25 kes Exp $
** $Log: olock.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:25  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:55  kes
** *** empty log message ***
**
Revision 1.3  98/10/29  11:43:00  kes
NSS version

Revision 1.2  96/11/10  14:06:32  kes
Changes to convert from FUDD to TC.

Revision 1.1  96/05/15  18:31:20  kes
Initial revision

**********************************************************************/

#include "nss.h"
#include "lock.h"
#include "olock.h"
#include "syslog.h"
#include "util.h"

OwnedLock::OwnedLock()
{
	owner = count = 0;
	err = CreateLock(&lock);
}

OwnedLock::~OwnedLock()
{
}

void
OwnedLock::Lock()
{
	tid_t tid = GetTaskId();

	// if the lock is unowned or owned by another task, wait for it
	if (owner != tid) {
		::Lock(&lock);
		owner = tid;
		count = 1;
	}
	else
		count++;
}

void
OwnedLock::Unlock()
{
	FUNCTION("OwnedLock::Unlock");
	tid_t tid = GetTaskId();

	// we'd better own this lock
	if (owner != tid)
		SysFatal(NSE_LCE, function, "tid = %d, owner = %d", tid, owner);

	if (--count == 0) {
		owner = 0;
		::Unlock(&lock);
	}
}