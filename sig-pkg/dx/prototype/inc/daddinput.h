/*******************************************************************************

 File:    daddinput.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/daddinput.h,v $
**
** Task to simulate DADD input from DSP
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/daddinput.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: daddinput.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:32  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:23  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _DADDINPUT_H
#define _DADDINPUT_H

#include "nsstypes.h"

typedef struct dadd_obs_input {
	obs_state_t state;			// current obs state
	uint32_t nSlices;			// # of slices
	uint32_t sliceBlk;			// length of a slice block
	uint32_t sliceLen;			// total length of a slice in buffer
	uint32_t sliceOfs;			// current ofs from beg of a slice
	uint32_t obsBufs;			// total # of buffers in obs
} dadd_obs_input_t;

// function prototypes
task_t DaddInputTask(void *arg);

#endif