/*******************************************************************************

 File:    nsstypes.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/nsstypes.h,v $
**
** New SETI System types
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/nsstypes.h,v 1.1.1.1 2006/08/02 05:29:55 kes Exp $
** $Log: nsstypes.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:55  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:35  kes
** *** empty log message ***
**
 * Revision 1.2  98/10/29  11:33:04  kes
 * Added uint64_t, changed obs states, added collector_type.
 * 
 * Revision 1.1  98/10/27  08:18:34  kes
 * Initial revision
 * 
**********************************************************************/
#ifndef _NSSTYPES_H
#define _NSSTYPES_H

#include <pthread.h>
#include <semaphore.h>

/* standard function name */
#define FUNCTION(name)	static const char *function = name

/* length-specific types */

typedef unsigned char		uint8_t;
typedef short				int16_t;
typedef unsigned short		uint16_t;
typedef int					bool_t;
typedef unsigned long		uint32_t;
typedef unsigned long long	uint64_t;

typedef void *				task_t;
typedef bool_t				flag_t;
typedef int32_t				error_t;
typedef uint32_t			bufsize_t;

typedef sem_t				flock_t;
typedef sem_t				fsem_t;

typedef struct timeval		timeval_t;

/* enumerated data types */

/*
** overall FUDD states; these are used for the Master, the individual
** detectors, and for the clients.
*/
typedef enum nss_state {
	DOWN = 0,
	STARTING = 1,						/* performing initialization */
	IDLE = 2,							/* not doing anything */
	OBSERVING = 3,						/* running an observation */
	TESTING = 4,						/* running internal tests */
	RESETTING = 5,						/* preparing to restart */
	QUITTING = 6						/* preparing to shut down */
} nss_state_t;

/* type of client */
typedef enum client_type {
	RECV,
	SEND
} client_type_t;

/* message types */
typedef enum msgtype {
	Define = 1,
	Start = 2,
	BufferFull = 3,
	WriteData = 4,
	WriteDone = 5,
	ReadData = 6,
	ReadDone = 7,
	AbortObs = 10,
	Shutdown = 11
} msgtype_t;

typedef enum obs_state {
	Idle = 0,
	Collecting = 1,
	Detecting = 2,
	Confirming = 3,
	Archiving = 4
} obs_state_t;
	
/* unit types */
typedef enum unit {
	Control = 0,
	DaddWrite = 1,
	PDWrite = 2,
	CDWrite = 3,
	DaddRead = 4,
	PDRead = 5,
	CDRead = 6,
	Monitor = 7,
	Any = 8,
	All = 9,
	None = 10
} unit_t;

/* CW resolutions */
typedef enum cw_resolution {
	CW_1HZ = 0,
	CW_2HZ = 1,
	CW_4HZ = 2
} cw_resolution_t;

/* PD resolutions */
typedef enum pd_resolution {
	PD_1HZ = 0,
	PD_2HZ = 1,
	PD_4HZ = 2,
	PD_8HZ = 3,
	PD_16HZ = 4,
	PD_32HZ = 5,
	PD_64HZ = 6,
	PD_128HZ = 6,
	PD_256HZ = 8,
	PD_512HZ = 9,
	PD_1KHZ = 10
} pd_resolution_t;

/* byte ordering */
typedef enum endian {
	BIG = 0,
	LITTLE = 1
} endian_t;

/* polarizations */
typedef enum pol {
	Left = 0,
	Right = 1
} pol_t;

/* collector types */
typedef enum collector_type {
	Dadd = 0,
	PD = 1,
	CD = 2
} collector_type_t;

/* client connection types */
typedef enum conn_type {
	NET_PASSIVE = 0,					/* passive (listening) socket */
	NET_ACTIVE = 1,						/* active (connecting) socket */
	SERIAL = 2,							/* serial port connection */
	KEYBOARD = 3,						/* console keyboard */
	DISPLAY = 4,						/* console display */
	CONSOLE = 5							/* network console */
} conn_type_t;

/* julian time structure: borrowed from BC630 time structure */
/* Note: this should have the year in it somewhere */
typedef struct nss_bc_tm {
	int	tm_nsec;						/* nanoseconds */
	int	tm_sec;							/* seconds */
	int	tm_min;							/* minutes */
	int	tm_hour;						/* hours */
	int	tm_yday;						/* year day */
	int tm_year;						/* year */
} nss_bc_tm_t;

/* message header structure definition: borrowed from SETI SCS */
struct MSG_HEADER						/* message header 		*/
 {
 long code;                            	/* Trace and ack flags 		*/
                                       	/* flags ORed over the 		*/
								       	/* Message Code 		*/
                                       	/* 0x0600CCCC			*/
 short sender;                         	/* Sending process 		*/
 short receiver;                       	/* Receiving process 		*/
 long length;                          	/* Length of associated 	*/
                                       	/* data immediately 		*/
                                       	/* following this msg 		*/
                                       	/* in bytes 			*/
 long activity_id;			 			/* Activity/Observation ID 	*/
 long request_id;						/* unique counter from each 
										   process			*/
                                       	/* Date, time of day with 	*/
                                       	/* micro seconds, from 		*/
                                       	/* gettimeofday() 		*/
 struct timeval time_sent;            	/* 8 bytes 			*/
 struct timeval time_recv;            	/* 8 bytes 			*/
 struct timeval time_proc;            	/* 8 bytes 			*/
};
typedef struct MSG_HEADER msg_header_t;

#define MAX_ERROR_STRING		256

struct NSS_ERROR
      {
      short code;                      /* error code */
      short severity;                  /* severity */
      char description[MAX_ERROR_STRING]; /* error description */
      };
typedef struct NSS_ERROR nss_error_t;

struct NSS_ERROR_MSG                   /* Header & Error Data */
      {
      struct MSG_HEADER header;
      struct NSS_ERROR error;
      };
typedef struct NSS_ERROR_MSG nss_error_msg_t;

#endif	/*	_NSSTYPES_H	*/