/*******************************************************************************

 File:    daddsub.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/dadd8a/daddsub.cc,v $
**
** DADD subroutines
**
** Author: Rick Stauduhar
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/dadd8a/daddsub.cc,v 1.1.1.1 2006/08/02 05:30:35 kes Exp $
** $Log: daddsub.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:35  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:44:21  kes
** *** empty log message ***
**
**************************************************************************/

#include "daddsub.h"

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
**		Shifts a row of data the specified # of bits to the right
**
*/
void
DaddShift(int shift, int bins, uint8_t *data)
{
	int i;

	for (i = 0; i < bins; i++)
		data[i] >>= shift;
}

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
/*
int
GetPosition(int drift, int blocksize)
{
	int rval, middleRow;

	if (drift == 0 || drift == blocksize - 1)
		return (drift);

	middleRow = blocksize / 2;
	if (drift & 1)
		return (middleRow + GetPosition(drift / 2, blocksize - middleRow));
	else
		return (GetPosition(drift / 2, middleRow);
}

/*
** GeneralUnscramble: unscramble the rows
**
** Synopsis:
**		void GeneralUnscramble(rows, bins, data, temp);
**		int rows;					# of rows in the block
**		int bins;					# of bins in each row
**		uint8_t *data;				# beginning of first row
**		uint8_t *temp;				# ptr to work space
** Returns:
**		Nothing.
** Description:
**		Unscrambles rows using the general row index calculator.
** Notes:
**		This is not generally used in production code, since we don't
**		care what order the rows are in.
*/
void
GeneralUnscramble(int rows, int cols, uint8_t *data, uint8_t *temp)
{
	register int i, j;
	int ibitrev;

	// unscramble the rows into a second (temporary) array
	for (i = 0; i < rows; i++) {
		ibitrev = GetPosition(i, rows);
		for (j = 0; j < bins; j++)
			temp[i*bins+j] = data[ibitrev*bins+j];
	}
	// copy the results back into the original array
	for (i = 0; i < rows; i++) {
		for (j = 0; j < bins; j++)
			data[i*bins+j] = data[i*bins+j];
	}
}

/*
** MergeHalves: copy half of an array
**
** Synopsis:
**		void MergeHalves(rows, bins, right, left);
**		int rows;					# of rows in the array
**		int bins;					# of bins in each row
**		uint8_t *right;				# right half array
**		uint8_t *left;				# left half array
** Returns:
**		Nothing.
** Description:
**		Copies the data from the right half array to the right half of
**		the left array.
** Notes:
**		Not used in production code.
*/
void
MergeHalves(int rows, int bins, uint8_t *right, uint8_t *left)
{
	register int i, j;

	for (i = 0; i < rows; i++) {
		for (j = bins / 2; j < bins; j++)
			left[i*bins+j] = right[i*bins+j];
	}
}

/*
** CompareArrays: compare two arrays
**
** Synopsis:
**		void CompareArrays(rows, bins, array1, array2);
**		int rows;					# of rows in the arrays
**		int bins;					# of bins in each row
**		uint8_t *array1;			# first array
**		uint8_t *array2;			# second array
*/
void
CompareArrays(int rows, int bins, uint8_t *array1, uint8_t *array2)
{
	int i, j, c;

	for (i = 0; i < rows; i++) {
		for (j = 0; j < bins; j++) {
			if (array1[i*bins+j] != array2[i*bins+j]) {
				printf("array compare failed at index pair (%d, %d), i, j);
				c = getchar();
				return;
			}
		}
	}
	printf("arrays compared perfectly!\n");
}

/*
** PrintArray: print the contents of an array
**
** Synopsis:
**		void PrintArray(int rows, int bins, uint8_t *array)
**
*/
void
PrintArray(int rows, int bins, uint8_t *array)
{
	int i, j, c;

	for (i = 0; i < rows; i++) {
		for (j = 0; j < bins; j++)
			printf("%02u ", array[i*bins+j]);
		c = getchar();
	}
}

/*
** RReverse: reverse the order of bytes in all rows of an array
**
** Synopsis:
**		void RReverse(rows, bins, array1, array2);
**		int rows;					# of rows in the array
**		int bins;					# of bins in each row
**		uint8_t *array1;			# source array
**		uint8_t *array2;			# destination array
** Notes:
**		array1 and array2 may be the same array.
*/
void
RReverse(int rows, int bins, uint8_t *array1, uint8_t *array2)
{
	int i, j, tmp;

	for (i = 0; i < rows; i++) {
		for (j = 0; j < (bins + 1) / 2; j++) {
			tmp = array1[i*bins+j];
			array2[i*bins+j] = array1[(i+1)*bins-j-1];
			array2[(i+1)*bins-j-1] = tmp;
		}
	}
}

/*
** Odd: return odd or even
*/
int
Odd(int m)
{
	return (m & 1);
}

/*
** compute time difference
*/
void
TimeDiff(struct timeval *start, struct timeval *end)
{
	double fstart, fend;

	fstart = (double) start->tv_sec + (double) start->tv_usec / 1e6;
	fend = (double) end->tv_sec + (double) end->tv_usec / 1e6;
	printf("time difference = %.3lf\n", fend - fstart);
}