/*******************************************************************************

 File:    dadd.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/dadd.h,v $
**
** Generalized DADD
**
** Author: Rick Stauduhar
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/dadd.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: dadd.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:32  kes
** *** empty log message ***
**
**************************************************************************/

#ifndef _DADD_H
#define _DADD_H

#include "nsstypes.h"

// function prototypes
void TopDownDadd(int rows, int bins, uint8_t *data);
void DaddDivide(int rows, int bins, uint8_t *data, int cacheRows);
void DaddConquer(int rows, int bins, int gap, int drift, uint8_t *data);
#ifdef notdef
void TopDownSingleSum(int drift, int bins, uint8_t *lower, uint8_t *upper);
void TopDownPairSum(int drift, int bins, uint8_t *lower, uint8_t *upper);
#endif
void DaddShift(int shift, int bins, uint8_t *data);

#endif