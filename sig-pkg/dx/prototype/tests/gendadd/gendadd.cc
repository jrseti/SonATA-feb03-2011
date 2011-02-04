/*******************************************************************************

 File:    gendadd.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/gendadd/gendadd.cc,v $
**
** General top-down DADD
**
** Author: Rick Stauduhar
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/gendadd/gendadd.cc,v 1.1.1.1 2006/08/02 05:30:39 kes Exp $
** $Log: gendadd.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:39  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:59:35  kes
** *** empty log message ***
**
**************************************************************************/

#include "daddutil.h"

// internal function prototypes
void TopDownPairSum(unsigned char *lowRow, unsigned char *highRow, int length,
		int drift);
void TopDownSingleSum(unsigned char *lowRow, unsigned char *highRow,
		int length, int height);

//
// TopDownDadd: top down general DADD
//
// Synopsis:
//		void TopDownDadd(data, length, rows);
//		unsigned char *data;	bin data
//		int length;				length of the row in bins
//		int rows;				row count of the data block
// Returns:
//		Nothing.
// Description:
//
//
void
TopDownDadd(unsigned char *data, int length, int rows)
{
	int drift, iBitRevLow, iBitRevHigh, lowRows, highRows, lowBase, highBase;

	if (rows < 1)
		return;

	// if blocks are unequal, upper one is larger
	lowRows = rows / 2;
	highRows = rows - lowRows;

	// run DADD on the two subblocks
	TopDownDadd(&data[0], length, lowRows);
	TopDownDadd(&data[lowRows*length], length, highRows);

	// now combine the paths from the subblocks; if the subblocks
	// are unequal, do the leftover case first
	if (highRows > lowRows) {
			lowBase = (lowRows - 1) * length;
			highBase = (2 * lowRows) * length;
			TopDownSingleSum(&data[lowBase], &data[highBase], length, highRows);
	}

	// add the matched pairs of drifts from subblocks; this requires
	// getting the index of drift i in both lower and upper blocks
	for (drift = 0; drift < lowRows; drift++) {
		iBitRevLow = GetPosition(drift, lowRows);
		iBitRevHigh = GetPosition(drift, highRows);
		lowBase = iBitRevLow * length;
		highBase = iBitRevHigh * length;
		TopDownPairSum(&data[lowBase], &data[highBase], length, drift);
	}
}

//
// TopDownPairSum: add a pair of row vectors
//
// Synopsis:
//		void TopDownPairSum(lowRow, highRow, length, drift);
//		unsigned char *lowRow;		ptr to lower row vector
//		unsigned char *highRow;		ptr to upper row vector
//		int length;					length of the vectors
//		int drift;					drift
// Returns:
//		Nothing.
// Description:
//		Adds a pair of row vectors of drift k to build row vectors of
//		drift 2k and (2k + 1).  The drift 2k vector replaces the first
//		drift k vector, and the drift (2k + 1) vector replaces the
//		second drift k vector.
//
void
TopDownPairSum(unsigned char *lowRow, unsigned char *highRow, int length,
		int drift)
{
	int i, n, temp;

	// compute the total number of bins we can do without running off
	// the edge of the vector
	n = length - drift;

	for (i = 0; i < n; i++) {
		temp = lowRow[i];
		lowRow[i] = temp + highRow[drift+i];
		highRow[i] = temp + highRow[drift+i+1];
	}
	temp = lowRow[n-1];

	if (n > 0)
		lowRow[n-1] = temp + highRow[drift+n-1];

	for (i = n; i < length; i++)
		highRow[i] = lowRow[i];
	highRow[n-1] = temp;
}

//
// TopDownSingleSum: sum the maximum-drift paths from the lower
//		and upper blocks
//
// Synopsis:
//		void TopDownSingleSum(lowRow, highRow, length, height);
//		unsigned char *lowRow;
//		unsigned char *highRow;
//		int length;
//		int height;
// Returns:
//		Nothing.
// Description:
//		This handles the eccentric case which arises when adding an
//		even block of rows to an odd block of rows in the non-2^n
//		DADD algorithm.  It sums the maximum drift from the larger
//		(upper) block with the maximum drift from the smaller (lower)
//		block.
// Notes:
//		This is the only case where paths of different total drift
//		are combined.
//
void
TopDownSingleSum(unsigned char *lowRow, unsigned char *highRow, int length,
		int height)
{
	int i, n;

	height--;
	n = length - height;
	for (i = 0; i < n; i++)
		highRow[i] = lowRow[i] + highRow[i+height];
	for (i = n; i < length; i++)
		highRow[i] = lowRow[i];
}