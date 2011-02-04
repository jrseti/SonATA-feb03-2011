/*******************************************************************************

 File:    lock.cpp
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

#include <pthread.h>
#include <stdlib.h>
#include <iostream>

using std::cout;
using std::endl;

int
main(int argc, char **argv)
{
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

	pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);

	// now lock the mutex twice
	int err;
	if (err = pthread_mutex_lock(&mutex))
		exit(1);

	if (err = pthread_mutex_lock(&mutex))
		exit(2);

	cout << "recursive lock ok" << endl;
}