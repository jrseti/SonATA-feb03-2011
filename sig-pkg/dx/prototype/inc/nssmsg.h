/*******************************************************************************

 File:    nssmsg.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/nssmsg.h,v $
**
** Message header file
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/nssmsg.h,v 1.1.1.1 2006/08/02 05:29:55 kes Exp $
** $Log: nssmsg.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:55  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:34  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:30  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _NSSMSG_H
#define _NSSMSG_H

#include "nss.h"

#define RECORD_MSG_TID		1
#define TEST_ALLOC_MSG		0

/*
** NSS packet structure
**
** IMPORTANT NOTE: the first member of this structure MUST be a
** msg_header_t (struct MSG_HEADER) so that inet_recv will
** work properly.
*/
typedef struct nssmsg {
	struct nssmsg		*next;
	msg_header_t		header;
	void				*data;
#if (RECORD_MSG_TID)
	tid_t				tid;
#endif
} nssmsg_t;

/* function prototypes */
error_t AllocMsgInit(int);
nssmsg_t *AllocMsg(uint32_t);
nssmsg_t *DupMsgHdr(nssmsg_t *, uint32_t);
nssmsg_t *DupMsg(nssmsg_t *);
void AllocMsgData(nssmsg_t *, uint32_t);
void FreeMsg(nssmsg_t *);

#endif /* _NSSMSG_H */