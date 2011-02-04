/*******************************************************************************

 File:    lock.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/lock.h,v $
**
** Lock definitions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/lock.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: lock.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:34  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:27  kes
 * Initial revision
 * 
 * Revision 1.3  96/11/10  14:25:16  kes
 * *** empty log message ***
 * 
 * Revision 1.2  96/01/31  12:23:52  kes
 * Removed ifdef _cplusplus.
 * 
 * Revision 1.1  95/02/09  11:09:28  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _LOCK_H
#define _LOCK_H

/* function prototypes */
error_t CreateLock(flock_t *);
error_t Lock(flock_t *);
error_t Unlock(flock_t *);

#endif /* _LOCK_H */