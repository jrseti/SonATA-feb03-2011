/*******************************************************************************

 File:    bldarray.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/bldarray.cc,v $
**
** Build the dadd short -> long long translation array
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/bldarray.cc,v 1.1.1.1 2006/08/02 05:30:25 kes Exp $
** $Log: bldarray.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:25  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:23  kes
** *** empty log message ***
**
Revision 1.1  98/10/29  11:08:11  kes
Initial revision

**********************************************************************/

#include "nss.h"
#include "bldarray.h"

void
BuildDaddArray(uint64_t **xlat)
{
	int i;
	uint8_t val[8];
	uint64_t *xp;

	// allocate the array
	*xlat = new uint64_t[XLSIZE];

	// build the translation array
	xp = (uint64_t *) val;
	for (i = 0; i < XLSIZE; i++) {
		val[0] = i & 3;
		val[1] = (i >> 2) & 3;
		val[2] = (i >> 4) & 3;
		val[3] = (i >> 6) & 3;
		val[4] = (i >> 8) & 3;
		val[5] = (i >> 10) & 3;
		val[6] = (i >> 12) & 3;
		val[7] = (i >> 14) & 3;
		(*xlat)[i] = *xp;
	}
}
	
