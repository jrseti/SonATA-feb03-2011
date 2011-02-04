/*******************************************************************************

 File:    nsserr.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/nsserr.h,v $
**
** NSS error code definitions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/nsserr.h,v 1.1.1.1 2006/08/02 05:29:55 kes Exp $
** $Log: nsserr.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:55  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:34  kes
** *** empty log message ***
**
 * Revision 1.2  98/10/29  11:32:32  kes
 * Added new error code.
 * 
 * Revision 1.1  98/10/27  08:18:29  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _NSSERR_H
#define _NSSRR_H

/* Error codes */

#define NSE_NH			-1				/* no header info */
#define NSE_NE			0				/* no error */

#define NSE_IM			0x1000			/* information message */
#define NSE_QSE			(0x1001)		/* queue send error */
#define NSE_QRE			(0x1002)		/* queue receive error */
#define NSE_IBP			(0x1003)		/* invalid buffer ptr */
#define NSE_NTA			(0x1004)		/* null task argument */
#define NSE_ILL			(0x1005)		/* invalid logging level */
#define NSE_ILT			(0x1006)		/* invalid logging target */
#define NSE_IEC			(0x1007)		/* invalid error code */
#define NSE_OIP			(0x1008)		/* observation in progress */
#define NSE_NOIP		(0x1009)		/* no obs in progress */
#define NSE_LCE			(0x100a)		/* lock creation error */
#define NSE_IMT			(0x100b)		/* invalid message type */
#define NSE_NCA			(0x100c)		/* no clock active */
#define NSE_NEM			(0x100d)		/* not enough memory */
#define NSE_CAM			(0x100e)		/* can't allocate message */
#define NSE_NMP			(0x100f)		/* NULL message ptr */
#define NSE_IMC			(0x1010)		/* invalid message code */
#define NSE_IU			(0x1011)		/* invalid Unit */
#define NSE_QCE			(0x1012)		/* queue creation error */
#define NSE_ICWR		(0x1013)		/* invalid CW resolution */
#define NSE_IML			(0x1018)		/* invalid message length */
#define NSE_IMD			(0x101c)		/* invalid message data */
#define NSE_COA			(0x101f)		/* can't open archive file */
#define NSE_CWA			(0x1020)		/* can't write archive file */
#define NSE_IET			(0x1029)		/* invalid event type */
#define NSE_NSBP		(0x102a)		/* NULL sample buffer ptr */
#define NSE_DSE			(0x102b)		/* data send error */
#define NSE_IOL			(0x1036)		/* invalid observation length */
#define NSE_MDNE		(0x1037)		/* invalid observation length */
#define NSE_SCE			(0x1038)		/* semaphore creation error */
#define NSE_AAM			(0x1039)		/* already allocated message */
#define NSE_QNE			(0x103a)		/* queue does not exist */
#define NSE_CCS			(0x103b)		/* can't create semaphore */
#define NSE_CCSM		(0x103c)		/* can't create shared memory */
#define NSE_TAP			(0x103f)		/* time is already past */
#define NSE_ILTP		(0x1041)		/* invalid log type */
#define NSE_ULT			(0x1042)		/* unmodifiable log type */
#define NSE_ILS			(0x1043)		/* invalid log state */
#define NSE_COLF		(0x104d)		/* can't open log file */
#define NSE_ILFN		(0x104e)		/* invalid log file name */
#define NSE_TCE			(0x104f)		/* task creation error */
#define NSE_FIP			(0x1050)		/* freeing invalid msg ptr */
#define NSE_FNP			(0x1051)		/* freeing NULL msg ptr */
#define NSE_OA			(0x1052)		/* observation aborted */
#define NSE_OC			(0x1053)		/* observation completed */
#define NSE_ONC			(0x1055)		/* observation not complete */
#define NSE_CSE			(0x1056)		/* clock synchronization error */
#define NSE_NTC			(0x105b)		/* no time code */
#define NSE_ITA			(0x105c)		/* invalid task argument */
#define NSE_ICF			(0x1062)		/* invalid command format */
#define NSE_IIS			(0x1063)		/* invalid input string */
#define NSE_ICT			(0x1064)		/* invalid connection type */
#define NSE_CSK			(0x1065)		/* can't send to keyboard */
#define NSE_CRD			(0x1066)		/* can't receive from display */
#define NSE_NC			(0x1067)		/* no connection */
#define NSE_IUD			(0x106b)		/* invalid UTC */
#define NSE_IDUD		(0x106c)		/* invalid delta UTC */
#define NSE_IBV			(0x1075)		/* invalid bin value */
#define NSE_IA			(0x1076)		/* invalid command line argument */
#define NSE_CST			(0x107b)		/* can't set time */
#define NSE_IYD			(0x107c)		/* invalid year day */
#define NSE_IC			(0x107f)		/* invalid command */
#define NSE_IAC			(0x1084)		/* invalid argument count */
#define NSE_IAL			(0x1085)		/* invalid argument list */
#define NSE_LOF			(0x1086)		/* drive fault or out of limits */
#define NSE_IY			(0x108c)		/* invalid year */
#define NSE_IF			(0x1094)		/* Invalid format */
#define NSE_STM			(0x1097)		/* system time mismatch */

#define NSE_CNTL_BASE	(0x1800)		/* base for control error codes */
#define NSE_FSC			(0x1801)		/* force socket close */
#define NSE_CE			(0x1802)		/* error in connection */
#define NSE_SNB			(0x1803)		/* socket non-blocking */
#define NSE_MTLF		(0x1806)		/* mutex trylock failure */
#define NSE_ESC			(0x1807)		/* error in socket communication */
#define NSE_OSF			(0x1809)		/* open socket failure */
#define NSE_SNC			(0x180a)		/* socket not connected */
#define NSE_PCC			(0x180b)		/* peer closed connection */
#define NSE_CNE			(0x180c)		/* client thread not active */
#define NSE_ICID		(0x180e)		/* invalid client ID number */
#define NSE_ICCMD		(0x180f)		/* invalid client command */
#define NSE_SMS			(0x1810)		/* socket mask select error */
#define NSE_IOCTL		(0x1811)		/* ioctl error */
#define NSE_ICMD		(0x1812)		/* invalid command */
#define NSE_ISS			(0x1813)		/* invalid system state */
#define NSE_NIMP		(0x1814)		/* function not implemented */
#define NSE_SHUT		(0x1818)		/* shutdown system call */
#define NSE_NS			(0x1819)		/* no socket */
#define NSE_HNF			(0x181a)		/* host not found */
#define NSE_IHN			(0x181b)		/* invalid host name */
#define NSE_NTD			(0x181d)		/* nothing to do for obs */
#define NSE_IREQ		(0x181e)		/* invalid request */
#define NSE_CVOR		(0x181f)		/* clock value out of range */
#define NSE_WDTF		(0x1820)		/* watchdog timer fired */

#define NSE_LAST			(0x2000)		/* last (placeholder) */

#endif /* _NSSERR_H */