/*******************************************************************************

 File:    sendmsg.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/sendmsg.h,v $
**
** SendMsg header file
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/sendmsg.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: sendmsg.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:36  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:37  kes
 * Initial revision
 * 
 * Revision 1.3  96/11/10  14:27:13  kes
 * Changes to convert from FUDD to TC.
 * 
 * Revision 1.2  96/05/15  18:14:03  kes
 * Removed SendToScs, SendToMaster, SendToDetectors, added SendFatalMsg.
 * 
 * Revision 1.1  96/01/31  12:27:08  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _SENDMSG_H
#define _SENDMSG_H

#include "nsstypes.h"
#include "nssmsg.h"

/* function protoypes */
error_t SendMsg(nssmsg_t *);
error_t SendFatalMsg(nssmsg_t *);

#endif /* _SENDMSG_H */