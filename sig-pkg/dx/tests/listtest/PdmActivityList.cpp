/*******************************************************************************

 File:    PdmActivityList.cpp
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

//
// DxActivityList: class to manage a list of activities
//
// $Header: /home/cvs/nss/sonata-pkg/dx/tests/listtest/DxActivityList.cpp,v 1.2 2009/02/12 18:52:47 kes Exp $
// $Log: DxActivityList.cpp,v $
// Revision 1.2  2009/02/12 18:52:47  kes
// First compilable version of DX1.  Massive changes from the original
// DX0 (SonATA demo) code.  Compiles and links, but many known bugs and
// is not yet runnable.
//
// Revision 1.1.1.1  2006/08/02 05:28:57  kes
// pdm-1_59
//
// Revision 1.1  2002/09/23 16:21:53  kes
// *** empty log message ***
//
//
#include "DxActivity.h"
#include "DxActivityList.h"
#include "DxErr.h"

ActivityList::ActivityList(int32_t nAct_): nActivities(nAct_)
{
	*actArray = new Activity[nActivities];

	for (int i = 0; i < nActivities; ++i)
		freeList.push_back(&act[i]);
}

ActivityList::~ActivityList()
{
	ActivityList::iterator pos;

	// remove all entries in both the free and allocated list
	freeList.clear();
	allocList.clear();

	// delete the activity array itself
	delete [] actArray;
}

Activity *
ActivityList::alloc()
{
	Activity *act = 0;

	if (!freeList.empty()) {
		act = freeList.front();
		freeList.pop_front();
	}
	return (act);
}

void
ActivityList::free(Activity *act_)
{
	ActivityList::iterator pos;
	Activity *act;

	// the activity must be in the allocated list
	if ((pos = allocList.find(act_)) != allocList.end()) {
		act = *pos;
		allocList.erase(pos);
		freeList.push_back(act);
	}
	else
		DxFatal(
	