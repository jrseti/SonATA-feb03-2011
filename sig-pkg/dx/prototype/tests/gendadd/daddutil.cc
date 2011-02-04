/*******************************************************************************

 File:    daddutil.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/gendadd/daddutil.cc,v $
**
** General DADD utilities
**
** Author: Rick Stauduhar
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/gendadd/daddutil.cc,v 1.1.1.1 2006/08/02 05:30:39 kes Exp $
** $Log: daddutil.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:39  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:59:35  kes
** *** empty log message ***
**
***********************************************************************/

#include "daddutil.h"

//
// GetPosition: compute the row number of a given drift
//
// Synopsis:
//		int GetPosition(drift, rows);
//		int drift;				drift to find
//		int rows;				# of rows in the block
// Returns:
//		row index of the row containing drift
// Description:
//		When the DADD algorithm is run in its own memory space,
//		the results appear not in their natural order.  In the case
//		of power of 2 DADD, the row containing sums of drift k appears
//		in row bitreverse(k). When the number of rows is not a power
//		of 2, the correspondence is more complicated.  This function
//		returns the index of the row containing pathsums with total drift
//		drift in a block of size rows. This generalizes the bit reversal
//		calculation in a power of 2 dadd computation.
// Notes:
//		WHY THE ABOVE CALCULATION WORKS:  By induction, one can see that in
//		an m row block, the drift 0 row is always the bottom row, and the
//		drift m-1 row is always the top row.  Because of the way subblocks
//		are combined, even drifts will lie in the lower subblock, and odd
//		drifts will lie in the upper subblock, with the possible exception
//		of the maximum drift row, which lies at the top of the upper
//		subblock independent of the parity of its index.  Thus, if the
//		drift is even, the row in question will be placed in the lower
//		subblock, in the position previously occupied by the row of
//		drift = drift/2 in that subblock, and if driftrate is odd, then
//		the row in question will lie in the upper subblock in the position
//		previously occupied by the row of driftrate = drift/2 in that
//		subblock.
//
int
GetPosition(int drift, int rows)
{
	int halfRows, halfDrift;

	if (!drift || (drift == rows - 1))
		return (drift);

	halfRows = rows / 2;
	halfDrift = drift / 2;

	if (drift & 1)
		return (halfRows + GetPosition(halfDrift, rows - halfRows));
	else
		return (GetPosition(halfDrift, halfRows));
}