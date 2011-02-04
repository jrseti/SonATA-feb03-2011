/*******************************************************************************

 File:    acttest.cpp
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
#include <stream.h>
#include <string>

class Activity {
public:
	PdmActivity(int actId_);
	~Activity() {};

	void setActivityId(int actId_) { activityId = actId_; }
	int getActivityId() { return (activityId); }
	int getFrames() { return (frames); }
	float getLength() { return (length); }

private:
	int activityId;
	int frames;
	float length;

	Activity(const Activity&);
	Activity& operator=(const Activity&);
};

Activity::PdmActivity(int actId_): activityId(actId_), frames(0),
		length(0)
{
}
		
typedef list<Activity *>	ActivityList;

int
main(int argc, char **argv)
{
	ActivityList freeList, allocList;
	ActivityList::iterator pos;
	Activity *act;

	// create the free list of activities
	for (int i = 0; i < 10; ++i)
		freeList.push_back(new PdmActivity(i));

	cout << "free list" << endl;
	for (pos = freeList.begin(); pos != freeList.end(); ++pos)
		cout << (*pos)->getActivityId() << endl;

	// allocate an activity by removing it from the head of the free
	// and adding it to the back of the allocated list
	while (!freeList.empty()) {
		act = freeList.front();
		freeList.pop_front();
		allocList.push_front(act);
		cout << "alloc list" << endl;
		for (pos = allocList.begin(); pos != allocList.end(); ++pos)
			cout << (*pos)->getActivityId() << endl;
	}
}