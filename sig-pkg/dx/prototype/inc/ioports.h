/*******************************************************************************

 File:    ioports.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/ioports.h,v $
**
** Serial port assignments
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/ioports.h,v 1.1.1.1 2006/08/02 05:29:53 kes Exp $
** $Log: ioports.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:53  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:34  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:26  kes
 * Initial revision
 * 
 * Revision 1.1  96/11/10  14:25:07  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _IOPORTS_H
#define _IOPORTS_H

/* port addresses */
#define AZ_DRIVE_PORTNAME	"/dev/pcxem0"	/* drive controller */
#define DRIVE_PORTNAME		"/dev/pcxem0"	/* multidrop drives */
#define EL_DRIVE_PORTNAME	"/dev/pcxem1"	/* drive controller */
#define AZ_ENCODER_PORTNAME	"/dev/pcxem2"	/* azimuth encoder */
#define EL_ENCODER_PORTNAME	"/dev/pcxem3"	/* elevation encoder */
#define PLC_PORTNAME		"/dev/pcxem4"	/* PLC */
#define WEATHER_PORTNAME	"/dev/pcxem5"	/* weather station */
#define UPS1_PORTNAME		"/dev/pcxem6"	/* ups 1 */
#define UPS2_PORTNAME		"/dev/pcxem7"	/* ups 2 */
#define SETIOBS_PORTNAME	"/dev/pcxem8"	/* SetiObs */
#define SETIMON_PORTNAME	"/dev/pcxem9"	/* SetiMon */
#define ALARM_PORTNAME		"/dev/pcxem14"	/* slew alarm */

/* port speeds */
#define DRIVE_SPEED			B19200
#define ENCODER_SPEED		B9600
#define PLC_SPEED			B9600
#define WEATHER_SPEED		B2400
#define UPS1_SPEED			B9600
#define UPS2_SPEED			B9600
#define SETIOBS_SPEED		B9600
#define SETIMON_SPEED		B9600
#define ALARM_SPEED			B9600

#endif _IOPORTS_H