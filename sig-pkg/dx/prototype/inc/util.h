/*******************************************************************************

 File:    util.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/util.h,v $
**
** Utility functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/util.h,v 1.1.1.1 2006/08/02 05:29:55 kes Exp $
** $Log: util.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:55  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:38  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:45  kes
 * Initial revision
 * 
 * Revision 1.5  96/11/10  14:29:54  kes
 * Changes to convert from FUDD to TC; added KillTask, StrUpper,
 * removed CreateArchiveName.
 * 
 * Revision 1.4  96/05/15  18:17:17  kes
 * Added DetachTask.
 * 
 * Revision 1.3  96/01/31  12:31:30  kes
 * Removed ifdef _cplusplus, removed several function prototypes.
 * 
 * Revision 1.2  96/01/09  13:32:52  kes
 * Added prototype for CreateArchive.
 * 
 * Revision 1.1  95/02/09  11:10:43  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _UTIL_H
#define _UTIL_H

#include <resource.h>

/* function prototypes */
error_t CreateTask(tid_t *, int, void *(*)(void *), void *);
error_t DetachTask(tid_t *);
tid_t GetTaskId(void);
int GetTaskPriority(tid_t);
int SetTaskPriority(tid_t, int);
void TaskSuicide(void);
error_t KillTask(tid_t);
int32_t RequestId(void);
int GetUsage(struct rusage *);
void PrintUsage(char *, struct rusage *);
void StrUpper(char *);

#endif /* _UTIL_H */