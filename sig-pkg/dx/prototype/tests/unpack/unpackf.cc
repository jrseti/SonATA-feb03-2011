/*******************************************************************************

 File:    unpackf.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/tests/unpack/unpackf.cc,v $
**
** DADD unpacking
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/tests/unpack/unpackf.cc,v 1.1.1.1 2006/08/02 05:30:39 kes Exp $
** $Log: unpackf.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:39  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 06:04:22  kes
** *** empty log message ***
**
**********************************************************************/

#define NSPECTRA		(4096)
#define SLICEWIDTH		(12288)

void unpack(unsigned short (*packed)[SLICEWIDTH/8],
		unsigned long long (*unpacked)[SLICEWIDTH/8], int rows, int cols,
		unsigned long long *xlat)
{
	register int i, j;

	// now unpack the array
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols / 8; j++)
			unpacked[i][j] = xlat[packed[i][j]];
	}
}