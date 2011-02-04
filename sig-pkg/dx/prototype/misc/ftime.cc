/*******************************************************************************

 File:    ftime.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/ftime.cc,v $
**
** Time-related functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/ftime.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: ftime.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:54  kes
** *** empty log message ***
**
 * Revision 1.8  98/10/29  11:42:58  kes
 * NSS version
 * 
**********************************************************************/

#include <unistd.h>
#include <string.h>
#include <sys/bcioctl.h>
#include <time.h>
#include <mem.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "nss.h"
#include "ftime.h"

/* constants */
#define HR_PER_DAY			24
#define MIN_PER_HR			60
#define SEC_PER_MIN			60
#define BILLION					1000000000

/* Clock-related variables */
int TimeMemFd;							/* shared memory fd */
int Clock;								/* clock flag */
sem_t *TimeSem;							/* clock tick semaphore */
nss_bc_tm_t *Time;						/* ptr to time value */
nss_bc_tm_t SysStartTime;				/* system startup time */

/*
** ComputeTime: compute the specified time
**
** Synopsis:
**		void ComputeTime(newtm, reftm, seconds);
**		nss_bc_tm_t *reftm;	new time
**		nss_bc_tm_t *newtm;	reference time
**		double seconds;			# of seconds between the two times
** Returns:
**		Nothing.
** Description:
**		Computes the new time given a reference time and the number
**		of seconds between.
** Notes:
**		seconds can be either positive or negative.
*/
void
ComputeTime(nss_bc_tm_t *reftm, nss_bc_tm_t *newtm, double seconds)
{
	int32_t sec, nsec;

	*newtm = *reftm;

	if (seconds < 0) {
		sec = (int32_t) -seconds;
		nsec = (int32_t) ((-seconds - sec) * 1e9);
	}
	else {
		sec = (int32_t) seconds;
		nsec = (int32_t) ((seconds - sec) * 1e9);
	}
	if (seconds > 0) {
		if ((newtm->tm_nsec += nsec) >= BILLION) {
			newtm->tm_nsec -= BILLION;
			newtm->tm_sec++;
		}
		if ((newtm->tm_sec += sec) >= SEC_PER_MIN) {
			newtm->tm_min += (newtm->tm_sec / SEC_PER_MIN);
			newtm->tm_sec %= SEC_PER_MIN;
		}
		if (newtm->tm_min >= MIN_PER_HR) {
			newtm->tm_hour += (newtm->tm_min / MIN_PER_HR);
			newtm->tm_min /= MIN_PER_HR;
		}
		if (newtm->tm_hour >= HR_PER_DAY) {
			newtm->tm_yday += (newtm->tm_hour / HR_PER_DAY);
			newtm->tm_hour %= HR_PER_DAY;
		}
	}		
	else {
		newtm->tm_nsec -= nsec;
		while (newtm->tm_nsec < 0) {
			newtm->tm_nsec += BILLION;
			newtm->tm_sec--;
		}
		newtm->tm_sec -= sec;
		while (newtm->tm_sec < 0) {
			newtm->tm_sec += SEC_PER_MIN;
			newtm->tm_min--;
		}
		while (newtm->tm_min < 0) {
			newtm->tm_min += MIN_PER_HR;
			newtm->tm_hour--;
		}
		while (newtm->tm_hour < 0) {
			newtm->tm_hour += HR_PER_DAY;
			newtm->tm_yday--;
		}
	}		
}		

/*
** ComputeDeltaTime: compute the number of seconds between two times
**
** Synopsis:
**		seconds = ComputeDeltaTime(end, start);
**		double seconds;			# of seconds between end and start
**		nss_bc_tm_t *end;		end time
**		nss_bc_tm_t *start;	start time
** Returns:
**		# of seconds between start time and end time
** Description:
**		Computes the delta time in seconds between the start time
**		and the end time.
** Notes:
**		Does not handle year-end processing correctly.
*/
double
ComputeDeltaTime(register nss_bc_tm_t *end, register nss_bc_tm_t *start)
{
	double seconds;

	seconds = (end->tm_yday - start->tm_yday)
			* HR_PER_DAY * MIN_PER_HR * SEC_PER_MIN;
	seconds += (end->tm_hour - start->tm_hour) * MIN_PER_HR * SEC_PER_MIN;
	seconds += (end->tm_min - start->tm_min) * SEC_PER_MIN;
	seconds += (end->tm_sec - start->tm_sec);
	seconds += (end->tm_nsec - start->tm_nsec) / 1e9;
	return (seconds);	
}

/*
** The following functions are related to the system time, which is
** provided by a separate process in a shared memory variable
*/

/*
** InitTime: initialize the time facility
**
** Synopsis:
**		rval = InitTime();
**		error_t rval;				return value
** Returns:
**		0 if the clock is running.
**		Error code if the clock is not running.
** Description:
**		Gets the time semaphore and the time shared memory
**		segment, then checks to see whether the clock is
**		running.
** Notes:
**		We cannot call SysLog at this point, because the
**		clock is initialized before logging.
*/
error_t
InitTime(void)
{
	FUNCTION("InitTime");
	nss_bc_tm_t tm, tm1;

	if (Clock)
		return (0);

	if (!Time) {
		/* get the time semaphore */
		if ((TimeSem = sem_open("time_sem", O_CREAT, 0)) < 0)
			return (NSE_CCS);

		/* get the time memory segment */
		if ((TimeMemFd = shm_open("time_value", O_RDONLY)) < 0)
			return (NSE_CCSM);
		Time = (nss_bc_tm_t *) mmap((void *) 0, sizeof(nss_bc_tm_t), PROT_READ,
				MAP_SHARED, TimeMemFd, 0);
		if (Time == (nss_bc_tm_t *) -1)
			return (NSE_CCSM);
	}

	/* see if the clock is running */
	tm = *Time;
	sleep(2);
	tm1 = *Time;
	if (Clock = (CmpTime(&tm1, &tm) > 0))
		SysStartTime = tm;
	else
		LynxTime(&SysStartTime);
	return (Clock ? 0 : NSE_NCA);
}

/*
** GetStartupTime: get the system startup time
**
** Synopsis:
**		void GetStartupTime(tm);
**		nss_bc_tm_t *tm;		ptr to storage for startup time
** Returns:
**		Nothing.
** Description:
**		Reads the system startup time from the internal time variable.
** Notes:
**		The startup time is set by InitTime.
*/
void
GetStartupTime(time_t *t)
{
	time((time_t *) t);
}

/*
** GetStartupBCTime: get the system startup time
**
** Synopsis:
**		void GetStartupBCTime(tm);
**		nss_bc_tm_t *tm;		ptr to storage for startup time
** Returns:
**		Nothing.
** Description:
**		Reads the system startup time from the internal time variable.
** Notes:
**		The startup time is set by InitTime.
*/
void
GetStartupBCTime(nss_bc_tm_t *tm)
{
	*tm = SysStartTime;
}

/*
** GetTime: get the reference time
**
** Synopsis:
**		void GetTime(reftm, flag);
**		nss_bc_tm_t *reftm;	ptr to storage for time
**		int flag;				"wait for tick" flag
** Returns:
**		Nothing.
** Description:
**		Reads the time from the shared memory location.  If flag is
**		nonzero, this function blocks until a 1PPS tick occurs, then
**		returns the value
** Notes:
**		The time is updated once per second, upon receipt of the 1PPS
**		pulse.  If synchronization to the 1PPS is not used (i.e.,
**		flag = 0), then the time returned may be up to 1 second off.
**		Task switching must be disabled during the operation, to ensure
**		that we do not get a partially updated value.
*/
void
GetTime(nss_bc_tm_t *reftm, int flag)
{
	if (Clock) {
		if (flag)
			sem_wait(TimeSem);
		*reftm = *Time;
	}
	else
		bzero((char *) reftm, sizeof(*reftm));
}

/*
** AddTime: add the specified # of hr/min/sec to the reference time
**
** Synopsis:
**		void AddTime(reftm, newtm, hour, min, sec);
**		nss_bc_tm_t *reftm;			ptr to reference time
**		nss_bc_tm_t *newtm;			ptr to computed time
**		int hour;					# of hours
**		int min;					# of minutes
**		int sec;					# of seconds
** Returns:
**		Nothing.
** Description:
**		Adds the specified number of hours, minutes, and seconds to
**	 	the reference time.
** Notes:
**		Assumes all values are non-negative.
**		Does not handle yearend processing correctly.
*/
void
AddTime(nss_bc_tm_t *reftm, nss_bc_tm_t *newtm, int hour, int min, int sec)
{
	*newtm = *reftm;

	newtm->tm_sec += sec;
	newtm->tm_min += min;
	newtm->tm_hour += hour;

	if (newtm->tm_sec >= SEC_PER_MIN) {
		newtm->tm_min += (newtm->tm_sec / SEC_PER_MIN);
		newtm->tm_sec %= SEC_PER_MIN;
	}
	if (newtm->tm_min >= MIN_PER_HR) {
		newtm->tm_hour += (newtm->tm_min / MIN_PER_HR);
		newtm->tm_min %= MIN_PER_HR;
	}
	if (newtm->tm_hour >= HR_PER_DAY) {
		newtm->tm_yday += (newtm->tm_hour / HR_PER_DAY);
		newtm->tm_hour %= HR_PER_DAY;
	}
}

/*
** SubTime: subtract the specified # of hr/min/sec from the reference time
**
** Synopsis:
**		void SubTime(reftm, newtm, hour, min, sec);
**		nss_bc_tm_t *reftm;		ptr to reference time
**		nss_bc_tm_t *newtm;		ptr to computed time
**		int hour;					# of hours
**		int min;					# of minutes
**		int sec;					# of seconds
** Returns:
**		Nothing.
** Description:
**		Adds the specified number of hours, minutes, and seconds to
**	 	the reference time.
** Notes:
**		Assumes all values are non-negative.
**		Does not handle year beginning processing correctly.
*/
void
SubTime(nss_bc_tm_t *reftm, nss_bc_tm_t *newtm, int hour, int min, int sec)
{
	*newtm = *reftm;

	newtm->tm_sec -= sec;
	newtm->tm_min -= min;
	newtm->tm_hour -= hour;

	while (newtm->tm_sec < 0) {
		newtm->tm_sec += SEC_PER_MIN;
		newtm->tm_min--;
	}
	while (newtm->tm_min < 0) {
		newtm->tm_min += MIN_PER_HR;
		newtm->tm_hour--;
	}
	while (newtm->tm_hour < 0) {
		newtm->tm_yday--;
		newtm->tm_hour += HR_PER_DAY;
	}
}

/*
** CmpTime: compare two times
**
** Synopsis:
**		rval = CmpTime(tm1, tm2);
**		int rval;					return value
**		nss_bc_tm_t *tm1;			ptr to reference time
**		nss_bc_tm_t *tm2;			ptr to computed time
** Returns:
**		0 if the times are equal.
**		-1 if tm1 is earlier than tm2.
**		1 if tm1 is later than tm2.
** Description:
**		Compares the two times to see which is earlier.
** Notes:
**		None.
*/
int
CmpTime(nss_bc_tm_t *tm1, nss_bc_tm_t *tm2)
{
	if (tm1->tm_yday < tm2->tm_yday)
		return (-1);
	if (tm1->tm_yday > tm2->tm_yday)
		return (1);
	if (tm1->tm_hour < tm2->tm_hour)
		return (-1);
	if (tm1->tm_hour > tm2->tm_hour)
		return (1);
	if (tm1->tm_min < tm2->tm_min)
		return (-1);
	if (tm1->tm_min > tm2->tm_min)
		return (1);
	if (tm1->tm_sec < tm2->tm_sec)
		return (-1);
	if (tm1->tm_sec > tm2->tm_sec)
		return (1);
	return (0);
}

/*
** WaitTime: wait for the specified time
**
** Synopsis:
**		rval = WaitTime(start);
**		error_t rval;				return value
**		nss_bc_tm_t *start;		start time
** Returns:
**		0 if successful.
**		Error code if no clock is available or time has already passed.
** Description:
**		Blocks the calling task until the specified time occurs.
** Notes:
**		None.
*/
error_t
WaitTime(nss_bc_tm_t *start)
{
	if (!Clock)
		return (NSE_NC);

	while (1) {
		sem_wait(TimeSem);
		if (CmpTime(Time, start) > 0)
			return (NSE_TAP);
		if (!CmpTime(Time, start))
			return (0);
	}
}

/*
** LynxTime: get the LynxOS system time
**
** Synopsis:
**		void LynxTime(tm)
**		nss_bc_tm_t *tm;		ptr to storage for time
** Returns:
**		Nothing.
** Description:
**		Reads the system time using the time() command, converts
**		it to tm version, then stores the values in the nss_bc_tm_t
**		given in the call.
** Notes:
**		This is to be used only when the reference clock is not
**		running.
*/
void
LynxTime(nss_bc_tm_t *tm)
{
	time_t t;
	struct tm stm;

	time(&t);
	gmtime_r(&stm, &t);
	bzero((char *) tm, sizeof(nss_bc_tm_t));
	tm->tm_yday = stm.tm_yday;
	tm->tm_hour = stm.tm_hour;
	tm->tm_min = stm.tm_min;
	tm->tm_sec = stm.tm_sec;
}