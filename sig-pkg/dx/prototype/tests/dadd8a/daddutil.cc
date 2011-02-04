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

/**************************************************************************
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/dadd8a/daddutil.cc,v $
**
** DADD utilities
**
** Author: Rick Stauduhar
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/dadd8a/daddutil.cc,v 1.1.1.1 2006/08/02 05:30:37 kes Exp $
** $Log: daddutil.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:37  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:44:21  kes
** *** empty log message ***
**
Revision 1.1  98/11/10  14:32:33  kes
Initial revision

**************************************************************************/

#include "daddutil.h"

/*
** GetPosition: compute a row index given the row drift
**
** Synopsis:
**		index = GetPosition(drift, blocksize);
**		int index;				index of the row
**		int drift;				drift of the row
**		int blocksize;			# of rows in the block
** Returns:
**		index of the row containing the specified drift
** Description:
**		When the DADD algorithm is run in its own memory space, the results
**		appear not in their natural order.  In the case of power of 2 DADD,
**		the row containing sums of drift k appears in row bitreverse(k).
**		When the number of rows is not a power of two, the correspondence is
**		more complicated.
** Notes:
**		This generalizes the bit reversal calculation in a power of 2 DADD
**		computation.
**		WHY THE ABOVE CALCULATION WORKS:  By induction, one can see that in
**		an m row block, the drift 0 row is always the bottom row, and the
**		drift m-1 row is always the top row.  Because of the way subblocks
**		are combined, even drifts will lie in the lower subblock, and odd
**		drifts will lie in the upper subblock, with the possible exception
**		the maximum drift row, which lies at the top of the upper subblock
**		independent of the parity of its index.
**		Thus, if drift is even, the row in question will be placed in
**		the lower subblock, in the position previously occupied by the row
**		of drift = drift / 2 in that subblock, and if drift is odd, then
**		the row in question will lie in the upper subblock in the
**		position previously occupied by the row of drift = drift / 2 in
**		that subblock.
*/
int
GetPosition(int drift, int blocksize)
{
	int middleRow;

	if (drift == 0 || drift == blocksize - 1)
		return (drift);

	middleRow = blocksize / 2;
	if (drift & 1)
		return (middleRow + GetPosition(drift / 2, blocksize - middleRow));
	else
		return (GetPosition(drift / 2, middleRow));
}

/*
** BitReverser: compute a row index given the row drift
**
** Synopsis:
**		index = BitReverser(drift, blocksize);
**		int index;				index of the row
**		int drift;				drift of the row
**		int blocksize;			# of rows in the block
** Returns:
** Description:
** Notes:
*/
int
BitReverser(int drift, int blocksize)
{
	int i, rev = 0;

	for (i = 0; blocksize > 1; i++) {
		rev <<= 1;
		rev |= drift & 1;
		drift >>= 1;
		blocksize /= 2;
	}
	return (rev);
}
