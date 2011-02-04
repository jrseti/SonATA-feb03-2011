/*******************************************************************************

 File:    maptest.cpp
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

#include <map>
#include <stream.h>

class Activity {
public:
	Activity(): activityId(0),  frames(0), length(0) {};
	~Activity() {};

private:
	int activityId;
	int frames;
	float length;

	Activity(const Activity&);
	Activity& operator=(const Activity&);
};

typedef map<int, Activity *>		ActivityList;

int
main(int argc, char **argv)
{
	ActivityList freeList, allocList;
	Activity *act = new Activity[10];

	for (int i = 0; i < 10; i++)
		freeList.insert(make_pair(i, &act[i]));

	ActivityList::iterator pos;
	for (pos = freeList.begin(); pos != freeList.end(); ++pos)
		cout << (*pos).first << endl;
}