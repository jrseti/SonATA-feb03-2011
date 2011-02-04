/*******************************************************************************

 File:    pid.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/pid.h,v $
**
** PID class: performs PID algorithm for an axis
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/pid.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/pid.h,v $
**********************************************************************/

#ifndef _PID_H
#define _PID_H

#include "tctypes.h"
#include "axis.h"
#include "olock.h"

// PID coefficients
typedef struct pid_coeff {
	double Kproportional;			/* proportional coefficient */
	double Kintegral;				/* integral coefficient */
	double Kderivative;				/* derivative coefficient */
	double tolerance;				/* position error tolerance for integral */
} pid_coeff_t;

//
// PID class definition
//
class PID {
protected:
	OwnedLock lock;						// access lock
	axis_type_t type;					// axis (Az, El)
	error_t err;						// current error state
	int nValues;						// # of points to average
	int next;							// next entry into buffer
	pid_coeff_t pid;					// PID coefficients
	double errSum;						// accumulated position error
	double *velocity;					// ptr to ring buffer

public:
	PID(axis_type_t, int);				// ctor and dtor
	~PID();

	// access methods
	void Lock() { lock.Lock(); }		// lock the structure
	void Unlock() { lock.Unlock(); }	// unlock the structure
	void Reset();						// reset ring buffer to zero values
	double Velocity(double, double);	// compute the PID velocity and insert
	double Filter();					// compute the filtered velocity

	// overloaded operators
	operator error_t() const { return (err); }
};

#endif // _PID_H