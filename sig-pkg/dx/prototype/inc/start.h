/*******************************************************************************

 File:    start.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/start.h,v $
**
** Start functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/start.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: start.h,v $
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

#ifndef _START_H
#define _START_H

#include "nsstypes.h"
#include "obs.h"

void StartDataCollection(obs_t *obs);
void StartDetection(obs_t *obs);
void StartConfirmation(obs_t *obs);

#endif