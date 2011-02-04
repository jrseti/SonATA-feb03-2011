/*******************************************************************************

 File:    lock.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/lock.cc,v $
**
** Lock functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/lock.cc,v 1.1.1.1 2006/08/02 05:30:26 kes Exp $
** $Log: lock.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:26  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:55  kes
** *** empty log message ***
**
 * Revision 1.7  98/10/29  11:42:58  kes
 * NSS version
 * 
 * Revision 1.6  96/11/10  14:06:11  kes
 * Changes to convert from FUDD to TC.
 * 
 * Revision 1.5  96/01/31  11:56:58  kes
 * Changed header files.
 * 
 * Revision 1.4  96/01/26  10:43:26  kes
 * Moved to new directory.
 * 
 * Revision 1.3  95/03/09  13:31:19  kes
 * Return error code.
 * 
 * Revision 1.2  95/02/09  11:27:38  kes
 * Changed LockCreate to CreateLock.
 * 
 * Revision 1.1  94/09/12  13:19:13  kes
 * Initial revision
 * 
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "nss.h"
#include "lock.h"

/*
** CreateLock: create a lock
**
** Synopsis:
**		err = CreateLock(lock);
**		lock_t *lock;			ptr to lock
** Returns:
**		0 if successful.
**		Error code if creation failed
** Description:
**		Creates a lock, initially unlocked.
** Notes:
**		None.
*/
error_t
CreateLock(flock_t *lock)
{
	if (sem_init(lock, 0, 1))
		return (errno);
	return (0);
}

error_t
Lock(flock_t *lock)
{
	return (sem_wait(lock));
}

error_t
Unlock(flock_t *lock)
{
	return (sem_post(lock));
}