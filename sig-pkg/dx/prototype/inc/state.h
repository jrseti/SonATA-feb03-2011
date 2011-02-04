/*******************************************************************************

 File:    state.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/state.h,v $
**
** State structures
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/state.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: state.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:36  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:39  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _STATE_H
#define _STATE_H

#include "nss.h"
#include "olock.h"

// state transition table entry
#define TRANSITION(from, to)		(from << 16 | to)

//
// this is the generic state class which is the base class
//
class State {
protected:
	OwnedLock lock;						// access lock
	int32_t activityId;					// current activity ID
	nss_state_t state;					// current system state
	error_t err;						// error state
	nss_state_t unitStatus[(int32_t) All]; // unit status

	int32_t setimon_delay_interval;		// delay interval for TCMON
public:
	// ctor and dtor
	State();
	~State();

	// access methods
	void Lock() { lock.Lock(); }		// lock the structure
	void Unlock() { lock.Unlock(); }	// unlock the structure
	error_t SetState(nss_state_t);		// set the state
	nss_state_t GetState();				// get the state
	void SetActivityId(int32_t);		// set the activity id
	int32_t GetActivityId();			// get the activity id
	
	bool_t ValidTransition(nss_state_t);	// test for valid state trans
	nss_state_t GetConnStatus(int);		// get connection status value
	void SetUnitState(unit_t, nss_state_t);
	nss_state_t GetUnitState(unit_t);		// get the unit state
	bool_t UnitActive(unit_t);			// test for unit alive
	bool_t CheckValidity(unit_t, int32_t);// test for valid commands

	// overloaded operators
	operator error_t() const { return (err); }
};

#endif // _STATE_H