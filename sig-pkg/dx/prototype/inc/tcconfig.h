/*******************************************************************************

 File:    tcconfig.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/tcconfig.h,v $
**
** Configuration file
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/tcconfig.h,v 1.1.1.1 2006/08/02 05:29:55 kes Exp $
** $Log: tcconfig.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:55  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:37  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:41  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#include "sysparms.h"

typedef struct sysconfig {
	int version;						// version #

	// encoder specifications
	encoder_spec_t azEncoder;
	encoder_spec_t elEncoder;

	// site location
	location_t location;

	// gear ratios, expressed as radians per motor revolution
	axis_pair_t gearRatio;

	// PID coefficients
	pid_coeff_t azPID;
	pid_coeff_t elPID;

	// telescope stow position
	axis_pair_t stowPosition;

	// telescope limits
	limit_t azLimit;
	limit_t elLimit;

	// position and velocity tracking tolerances
	axis_pair_t maxPositionErr;
	axis_pair_t maxVelocityErr;

	// wind stow parameters
	wind_stow_t windStow;

	// pointing models
	pointing_model_t radioModel;		// radio pointing model
	pointing_model_t opticalModel;		// optical pointing model
} sysconfig_t;

#endif	