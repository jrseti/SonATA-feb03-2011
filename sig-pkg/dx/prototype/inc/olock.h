/*******************************************************************************

 File:    olock.h
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

#ifndef _OLOCK_H
#define _OLOCK_H

#include "nsstypes.h"
#include "lock.h"

class OwnedLock {
protected:
	flock_t lock;						// lock
	tid_t owner;						// lock owner
	int count;							// # of times owner has locked
	error_t err;						// error state

public:
	// ctor and dtor
	OwnedLock();
	~OwnedLock();

	// access methods
	void Lock();						// acquire the lock
	void Unlock();						// release the lock
	operator error_t() const { return (err); }
};

#endif // _OLOCK_H