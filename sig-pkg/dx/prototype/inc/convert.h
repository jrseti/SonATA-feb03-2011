/*******************************************************************************

 File:    convert.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/convert.h,v $
**
** Convert function prototypes
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/convert.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: convert.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:32  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:22  kes
 * Initial revision
 * 
 * Revision 1.3  96/11/10  14:20:34  kes
 * Changes to convert from FUDD to TC.
 * 
 * Revision 1.2  96/05/15  18:19:52  kes
 * Moved from control to inc.
 * 
 * Revision 1.1  96/01/31  11:41:46  kes
 * Initial revision
 * 
**********************************************************************/
#ifndef _CONVERT_H
#define _CONVERT_H

#include "tc.h"
#include "fuddmsg.h"
#include "tcmon.h"

/* function prototypes */
void ConvertMsgHeader(msg_header_t *);
error_t ConvertMsgData(fudd_packet_t *);
void ConvertTimevalStruct(timeval_t *);
#ifdef notdef
void ConvertBCTMTime(tc_bc_tm_t *);
#endif

void ConvertTCMonStruct(tc_mon_t *);
void ConvertTMStruct(struct tm *);
void ConvertPositionStruct(position_t *);
void ConvertAxisPairStruct(axis_pair_t *);
void ConvertEncoderDataStruct(encoder_data_t *);
void ConvertDriveDataStruct(drive_data_t *);
void ConvertWeatherDataStruct(weather_data_t *);
void ConvertWindDataStruct(wind_data_t *);
void ConvertTempDataStruct(temperature_data_t *);
double ntohd(volatile double);

#endif /* _CONVERT_H */