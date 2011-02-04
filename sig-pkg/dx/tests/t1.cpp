/*******************************************************************************

 File:    t1.cpp
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

class test {
public:
	test() { x = y = 0; }
	~test() { }

	void setX(int x_) { x = x_; }
	void setY(int y_) { y = y_; }
	void setZ(int z_) { z = z_; }

	int getX() { return (x); }
	int getY() { return (y); }
	int getZ() { return (z); }
	
private:
	int x;
	int y;
	int z;
};

int
main(int argc, char *argv[])
{
	test t1;

	t1.setX(10);
	t1.setY(5);
	t1.setZ(3);

	cout << "sizeof(test) = " << sizeof(test) << endl;
	cout << "sizeof(t1) = " << sizeof(t1) << endl;
	cout << "x = " << t1.getX() << endl;
	cout << "y = " << t1.getY() << endl;
	cout << "z = " << t1.getZ() << endl;
}