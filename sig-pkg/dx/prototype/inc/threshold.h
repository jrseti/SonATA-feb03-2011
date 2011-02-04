/*******************************************************************************

 File:    threshold.h
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

/**************************************************************************
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/threshold.h,v $
**
** Threshold
**
** Author: Ken Smolek
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/threshold.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: threshold.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:37  kes
** *** empty log message ***
**
**************************************************************************/

#ifndef _THRESHOLD_H
#define _THRESHOLD_H

#include "nsstypes.h"

// function prototypes
extern "C" {
	void Threshold(int rows, uint32_t bins, uint32_t rowbins,
			uint8_t *data, uint8_t threshold);
};

#endif
