/*******************************************************************************

 File:    tcmon.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/tcmon.h,v $
**
** Status structure for use by tcmon
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/tcmon.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: tcmon.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:37  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:42  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _TCMON_H
#define _TCMON_H

#include "tc.h"
#include "drive_data.h"
#include "encoder_data.h"
#include "position.h"
#include "weathertypes.h"

typedef struct tc_mon {
	struct tm tm;						/* time of report */
	unsigned long plcSW;				/* PLC status word */
	double lmst;						/* LMST in hours */
	long state;							/* current state */
	long control;						/* input source (SetiObs, Console, etc */
	long err;							/* most recent error */
	long pad2;							/* padding */
	position_t target;					/* current telescope target */
	position_t actual;					/* current telescope position */

	position_t targetAzEl;				/* current target in Az/El position */
	position_t actualAzEl;				/* current position in target coords */
	position_t actualHaDec;				/* current position in Ha/Dec coords */
	position_t actualRaDec;				/* current position in Ra/Dec coords */

	position_t error;					/* current position error */
	encoder_data_t az_encoder;			/* azimuth encoder */
	encoder_data_t el_encoder;			/* elevation encoder */
	drive_data_t az_drive;				/* azimuth drive data */
	drive_data_t el_drive;				/* elevation drive data */
	weather_data_t weather_data;		/* weather station info */
} tc_mon_t;

#endif /* _TCMON_H */
