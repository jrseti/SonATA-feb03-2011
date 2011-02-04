/*******************************************************************************

 File:    signals.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/signals.cc,v $
**
** Signal handling functions
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/signals.cc,v 1.1.1.1 2006/08/02 05:30:28 kes Exp $
** $Log: signals.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:28  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:55  kes
** *** empty log message ***
**
Revision 1.5  98/10/29  11:43:00  kes
NSS version

Revision 1.4  96/11/10  14:09:15  kes
Changes to convert from FUDD to TC.

Revision 1.3  96/05/15  18:33:28  kes
Added function header.

Revision 1.2  96/02/09  09:12:06  kes
Replaced BlockSignals, IgnoreSignals with HandleSignals driven by
an initialization table.

**********************************************************************/
#include <stdio.h>
#include <signal.h>
#include "nss.h"
#include "signals.h"

/*
** HandleSignals: set up signal handling
**
** Synopsis:
**		err = HandleSignals(spec);
**		error_t err;		return value
**		sig_spec_t *spec;	ptr to array of signals/handlers
** Returns:
**		0 if successful.
**		error code if an error.
** Description:
**		Initializes signal handling according to the array entries.
** Notes:
**		None.
*/
error_t
HandleSignals(sig_spec_t *spec)
{
	int i;
	struct sigaction act;

	for (i = 0; spec[i].sig >= 0; i++) {
		act.sa_handler = spec[i].handler;
		sigemptyset(&act.sa_mask);
		sigaddset(&act.sa_mask, spec[i].sig);
		act.sa_flags = 0;
		if (sigaction(spec[i].sig, &act, NULL))
			return (errno);
	}
	return (0);
}
