/*******************************************************************************

 File:    unit.cc
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/misc/unit.cc,v $
**
** Functions to provide access to unit information
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/misc/unit.cc,v 1.1.1.1 2006/08/02 05:30:27 kes Exp $
** $Log: unit.cc,v $
** Revision 1.1.1.1  2006/08/02 05:30:27  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:35:56  kes
** *** empty log message ***
**
Revision 1.6  98/10/29  11:43:02  kes
NSS version

Revision 1.5  96/11/10  14:11:08  kes
Changes to convert from FUDD to TC.

Revision 1.4  96/05/15  18:35:19  kes
Moved UnitActive, UnitState, SetUnitState to State class.

Revision 1.3  96/02/13  09:37:49  kes
Changed ChangeUnitState to SetUnitState.

Revision 1.2  96/02/09  09:13:20  kes
Changed definition of scs_unit to add connection port.
Added GetUnitByName, GetUnitPort, UnitActive, UnitState, and
ChangeUnitState.  It's not clear whether GetUnitPort is necessary,
and ChangeUnitState should be SetUnitState..

Revision 1.1  96/01/31  11:59:11  kes
Initial revision

**********************************************************************/

#include <stdio.h>
#include "nss.h"
#include "globals.h"
#include "unit.h"

// a list of all valid units
unit_info_t Unit[] = {
	{ None, "None", "NSS" }
};

// default units
static unit_t thisUnit = None;

//
// SetUnit: set the current unit ID
//
// Synopsis:
//		err = SetUnit(unit);
//		error_t err;			return value
//		unit_t unit;		unit
// Returns:
//		0 if the unit specified is valid.
//		error code if not valid.
// Description:
//		Sets the internal unit ID to the specified value after validating
//		it.  The OtherUnit value, which specifies the opposite detector
//		(i.e., MainA => RemoteA, RemoteB => MainB), is also set.
// Notes:
//		Access to unit ID's is via GetUnit() and GetOtherUnit().
//		Only detector clients have a valid OtherUnit.
//		The list of valid units will depend upon the unit type; for
//		example, the Master will have entries for the SCS and all
//		detectors, while a detector will have entires only for the
//		SCS, the Master, and its opposite-site counterpart.
//
error_t
SetUnit(unit_t unit)
{
	int i;

	// scan the list of valid clients
	for (i = 0; Unit[i].unit != None && Unit[i].unit != unit ; i++)
		;
	if (Unit[i].unit == None) {
		thisUnit = None;
		return (NSE_IU);
	}

	// got a valid unit
	thisUnit = unit;
	return (0);
}

unit_t
GetUnit(void)
{
	return (thisUnit);
}

char *
GetUnitName(unit_t unit)
{
	int i;

	// scan the list of valid clients
	for (i = 0; Unit[i].unit != None && Unit[i].unit != unit ; i++)
		;
	return (Unit[i].name);
}

char *
GetUnitAlias(unit_t unit)
{
	int i;

	// scan the list of valid clients
	for (i = 0; Unit[i].unit != None && Unit[i].unit != unit ; i++)
		;
	return (Unit[i].alias);
}

unit_t
GetUnitByName(const char *name)
{
	int i;

	// scan the list of valid clients
	for (i = 0; Unit[i].unit != None; i++) {
		if (!strcmp(Unit[i].name, name) || !strcmp(Unit[i].alias, name))
			return (Unit[i].unit);
	}
	return (None);
}

uint16_t
GetUnitPort(unit_t unit)
{
	int i;

#ifdef notdef
	for (i = 0; ScsUnit[i].unit != None; i++) {
		if (ScsUnit[i].unit == unit)
			return (ScsUnit[i].port);
	}
#endif
	return (-1);
}
