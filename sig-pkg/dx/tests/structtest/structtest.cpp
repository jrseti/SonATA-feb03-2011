/*******************************************************************************

 File:    structtest.cpp
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

#include <iostream>

struct base {
	int x;
	int y;

	base() { x = 0; y = 0; }
	~base() { };

	int getX() { return (x); }
	int getY() { return (y); }
};

struct vbase {
	int x;
	int y;

	vbase() { x = 0; y = 0; }
	~vbase() { }

	virtual int getX() { return (x); }
	virtual int getY() { return (y); }
};

int
main(int argc, char *argv[])
{
	base one;
	vbase two;

	cout << "one = " << sizeof(one) << endl;
	cout << "two = " << sizeof(two) << endl;
}	