/*******************************************************************************

 File:    tcstatus.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/tcstatus.h,v $
**
** Canonical status structure for export to other processes
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/tcstatus.h,v 1.1.1.1 2006/08/02 05:29:55 kes Exp $
** $Log: tcstatus.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:55  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:37  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:43  kes
 * Initial revision
 * 
 * Revision 1.1  96/11/10  14:28:45  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _TCSTATUS_H
#define _TCSTATUS_H

#include "tc.h"
#ifdef notdef
#include "encoder.h"
#endif
#include "setimon.h"

typedef struct tc_status {
	long state;							/* current state */
	long control;						/* input source (SetiObs, Console, etc */
	long activity_id;					/* current activity id */
	long err;							/* most recent error */
	double time;						/* UTC time of report */
	position_t position;				/* latest telescope position */
	position_t position_error;			/* latest position error */
	data_1_t data1;						/* DATA1 for SetiMon */
	data_2_t data2;						/* DATA2 for SetiMon */
} tc_status_t;

#endif /* _TCSTATUS_H */