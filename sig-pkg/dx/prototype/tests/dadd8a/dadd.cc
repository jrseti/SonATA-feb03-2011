/*******************************************************************************

 File:    dadd.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/dadd8a/dadd.cc,v $
**
** Generalized DADD
**
** Author: Rick Stauduhar
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/dadd8a/dadd.cc,v 1.1.1.1 2006/08/02 05:30:37 kes Exp $
** $Log: dadd.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:37  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:44:21  kes
** *** empty log message ***
**
Revision 1.3  98/11/12  09:05:38  kes
Changed calls to TopDownSingleSum, TopDownPairSum to add shift count.

Revision 1.2  98/11/11  14:13:42  kes
ifdef'ed out DaddShift.

Revision 1.1  98/11/10  14:32:27  kes
Initial revision

**************************************************************************/

#include "dadd.h"
#include "daddutil.h"
#include "pairsum.h"
#include "singlesum.h"

/*
** TopDownDadd: compute DADD on an array of power bins
**
** Synopsis:
**		void TopDownDadd(rows, bins, data);
**		int rows;					# of rows in the block
**		int bins;					# of bins in each row
**		uint8_t *data;				# beginning of first row
** Returns:
**		Nothing.
** Description:
**		Computes DADD on a general (i.e., non-power-of-2) array
** Notes:
**		This version of dadd is not restricted to power_of_two length
**		blocks.  It splits the incoming data block into equal length
**		subblocks if rows is even, or nearly equal subblocks if rows is
**		odd, runs dadd on the subblocks, then sums the resulting vectors.
**		The argument rows is the actual row count of the data block,
**		not the base 2 log of the row count.
**		Note that when subblocks are unequal, the upper subblock is
**		always taken as the larger.
*/
void
TopDownDadd(int rows, int bins, uint8_t *data)
{
	int i, iBitRev1, iBitRev2;
	int lowerBase, upperBase, lowerRows, upperRows, shift;

	if (rows <= 1)
		return;

	// determine whether normalization shifts are necessary
	if (rows > 85)
		shift = 1;
	else
		shift = 0;

	// if blocks are unequal, upper one is bigger
	lowerRows = rows / 2;
	upperRows = rows - lowerRows;

	// run DADD on the two subblocks
	TopDownDadd(lowerRows, bins, data);
	TopDownDadd(upperRows, bins, &data[lowerRows*bins]);

	// now combine the paths from the subblocks
	if (upperRows > lowerRows) {
		// subbblocks are unequal; do leftover case first
		lowerBase = (lowerRows - 1) * bins;
		upperBase = (lowerRows * 2) * bins;
		TopDownSingleSum(upperRows, bins, &data[lowerBase], &data[upperBase],
				shift);
	}

	// add the matched pairs of drifts from subblocks
	for (i = 0; i < lowerRows; i++) {
		iBitRev1 = GetPosition(i, lowerRows);
		iBitRev2 = GetPosition(i, upperRows);
		lowerBase = iBitRev1 * bins;
		upperBase = (lowerRows + iBitRev2) * bins;
		TopDownPairSum(i, bins, &data[lowerBase], &data[upperBase], shift);
	}
}

/*
** DaddDivide: divide and conquer
**
** Synopsis:
**		void DaddDivide(rows, bins, data);
**		int rows;					# of rows in the block
**		int bins;					# of bins in each row
**		uint8_t *data;				# beginning of each row
** Returns:
**		Nothing.
** Description:
**		Performs a modified version of TopDownDadd which attempts to maintain
**		row locality to improve cache hits.
** Notes:
**		This version IS restricted to powers of two.
*/
void
DaddDivide(int rows, int bins, uint8_t *data, int cacheRows)
{
	int i, iBitRev;

	if (rows > cacheRows) {
		// cut the block into subblocks and run DADD
		for (i = 0; i < cacheRows; i++) {
			DaddDivide(rows / cacheRows, bins,
					data + i * rows / cacheRows * bins, cacheRows);
		}
		// run DADD on the set of drift i vectors
		for (i = 0; i < rows / cacheRows; i++) {
			iBitRev = BitReverser(i, rows / cacheRows);
			DaddConquer(cacheRows, bins, rows / cacheRows, iBitRev,
					data + i * bins);
		}
	}
	else {
		// row count is too small to divide
		DaddConquer(rows, bins, 1, 0, data);
	}
}
			
/*
** DaddConquer: conquer
**
** Synopsis:
**		void DaddConquer(rows, bins, gap, drift, data);
**		int rows;					# of rows in the block
**		int bins;					# of bins in each row
**		int gap;					# of bins to skip in each row
**		int drift;					drift of the rows
**		uint8_t *data;				# beginning of each row
** Returns:
**		Nothing.
** Description:
**		???
** Notes:
**		This version IS restricted to powers of two.
*/
void
DaddConquer(int rows, int bins, int gap, int drift, uint8_t *data)
{
	uint8_t *lowRow, *highRow;
	int i, iBitRev, half, shift;

	if (rows > 85)
		shift = 1;
	else
		shift = 0;

	if (rows > 2) {
		half = rows / 2;
		DaddConquer(half, bins, gap, drift, data);
		DaddConquer(half, bins, gap, drift, data + half * gap * bins);
	}
	else
		half = 1;

	for (i = 0; i < half; i++) {
		iBitRev = BitReverser(i, half);
		lowRow = data + iBitRev * gap * bins;
		highRow = data + (half + iBitRev) * gap * bins;
		TopDownPairSum(i + half * drift, bins, lowRow, highRow, shift);
	}
}
			
#ifdef notdef
/*
** TopDownSingleSum: add the maximum-drift rows from the upper and lower blocks
**
** Synopsis:
**		void TopDownSingleSum(drift, bins, lower, upper);
**		int drift;					???
**		int bins;					# of bins in a row
**		uint8_t *lower;				lower block row data
**		uint8_t *upper;				upper block row data
** Returns:
**		Nothing.
** Description:
**		?
** Notes:
**		This handles the eccentric case which arises when adding an
**		even block of rows to an odd block of rows in the non-power-of-2
**		DADD algorithm.  It sums the maximum drift from the larger
**		(upper) block with the maximum drift from the lower (smaller)
**		block.  This is the only case where paths of different total drift
**		are combined.
*/
void
TopDownSingleSum(int drift, int bins, uint8_t *lower, uint8_t *upper)
{
	int i;

	drift--;
	for (i = 0; i < bins - drift; i++)
		upper[i] = lower[i] + upper[i+drift];
	for (i = bins - drift; i < bins; i++)
		upper[i] = lower[i];
}
#endif

#ifdef notdef
/*
** TopDownPairSum: add a pair of rows
**
** Synopsis:
**		void TopDownPairSum(drift, bins, lower, upper);
**		int drift;					# drift of this pair of rows
**		int bins;					# of bins in a row
**		uint8_t *lower;				lower block row data
**		uint8_t *upper;				upper block row data
** Returns:
**		Nothing.
** Description:
**		Adds a pair of rows of drift k from the upper and lower blocks.
**		The drift 2k sum is stored in the lower block, while the drift
**		2k+1 sum is stored in the lower block, shifted by one bin.
** Notes:
**		The adds are actually performed 8 bins at a time.
*/
void
TopDownPairSum(int drift, int bins, uint8_t *lower, uint8_t *upper)
{
	uint8_t tmp[8];
	int i, len, adds, offset;

	len = bins - drift;
	adds = (len - 1) / 8;
	offset = (len - 1) % 8;

	for (i = 0; i < adds * 8; i += 8) {
		mov8[tmp, &lower[i]);
		add8([&lower[i], tmp, &upper[i+drift]);
		add8([&upper[i], tmp, &upper[i+drift+1]);
	}
	if (offset) {
		for (i = adds * 8; i < len - 1; i += 8) {
			tmp = lower[i];
			lower[i] = tmp + upper[i+drift];
			upper[i] = tmp + upper[i+drift+1];
		}
	}
	tmp = lower[len-1];
	if (len > 0)
		lower[len-1] = tmp + upper[len-1+drift];

	for (i = len; i < bins; i++)
		upper[i] = lower[i];
	upper[len-1] = tmp;
}

/*
** DaddShift: perform a normalization shift for a row of DADD data
**
** Synopsis:
**		void DaddShift(shift, bins, data);
**		int shift;					# of bits to shift
**		int bins;					# of bins in a row
**		uint8_t *data;				# beginning of row of data
** Returns:
**		Nothing.
** Description:
**		Shifts a row of data the specified # of bits to the right.
** Notes:
**		Efficient implementation of this function in assembler relies
**		upon quad-word alignment of the data to be shifted.
**
*/
void
DaddShift(int shift, int bins, uint8_t *data)
{
	int i;

	for (i = 0; i < bins; i++)
		data[i] >>= shift;
}
#endif
