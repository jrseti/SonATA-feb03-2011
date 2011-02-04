/*******************************************************************************

 File:    create.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/nss/create.cc,v $
**
** Main control task
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/nss/create.cc,v 1.1.1.1 2006/08/02 05:30:25 kes Exp $
** $Log: create.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:25  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 05:13:24  kes
** *** empty log message ***
**
Revision 1.2  98/10/29  11:09:56  kes
Fixed bug in obs pointer calc.

Revision 1.1  98/10/27  08:17:11  kes
Initial revision

**********************************************************************/

#include <string.h>
#include "nss.h"
#include "create.h"
#include "globals.h"
#include "obs.h"

//
// CreateObs: create an observation
//
obs_t *
CreateObs()
{
	int collect, detect;
	obs_t *obs;

	// clear everything
	obs = &Obs[++ObsId%2];
	bzero((char *) obs, sizeof(obs_t));

	obs->id = ObsId;
	obs->state = Idle;
	obs->frames = 64;
	obs->bins = 4 * 1024 * 1024;
	obs->cwRes = CW_1HZ;
	obs->centerFreq = 1420.;

	// assign fds
	obs->daddFd[Left] = LDaddFd[ObsId%2];
	obs->daddFd[Right] = RDaddFd[ObsId%2];
	obs->cdFd[Left] = LCDFd[ObsId%2];
	obs->cdFd[Right] = RCDFd[(ObsId+1)%2];
}