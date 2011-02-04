/*******************************************************************************

 File:    test.cpp
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

#include <list>
#include <map>
#include <stream.h>
#include <string>

typedef list<string *> StringList;

string a("a");
string b("b");
string c("c");

int
main(int argc, char **argv)
{
	StringList stringList;
	StringList::iterator pos;

	stringList.push_back(&a);
	stringList.push_back(&b);
	pos = stringList.begin();
	stringList.insert(pos, &c);

	for (pos = stringList.begin(); pos != stringList.end(); ++pos) {
		cout << *pos << endl;
		string *s = *pos;
		cout << s->c_str() << (*pos)->c_str() << endl;
	}

	for (pos = stringList.begin(); pos != stringList.end(); ) {
		cout << *pos << endl;
		pos = stringList.erase(pos);
	}
}