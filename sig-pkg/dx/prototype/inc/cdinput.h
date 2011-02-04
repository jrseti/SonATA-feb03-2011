/*******************************************************************************

 File:    cdinput.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/cdinput.h,v $
**
** Task to simulate CD input from DSP
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/cdinput.h,v 1.1.1.1 2006/08/02 05:29:53 kes Exp $
** $Log: cdinput.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:53  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:31  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:20  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _CDINPUT_H
#define _CDINPUT_H

#include "nsstypes.h"

typedef struct cd_obs_input {
	obs_state_t state;			// current obs state
	uint32_t nChannels;			// # of channels
	uint32_t channelBlk;		// length of a channel block
	uint32_t channelLen;		// total length of a channel in buffer
	uint32_t channelOfs;		// current ofs from beg of a channel
	uint32_t obsBufs;			// total # of buffers in obs
} cd_obs_input_t;

// function prototypes
task_t CDInputTask(void *arg);

#endif