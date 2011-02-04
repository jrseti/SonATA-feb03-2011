/*******************************************************************************

 File:    unit.h
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
** $Source: /home/cvs/nss/sonata-pkg/dx/prototype/inc/unit.h,v $
**
** Unit function prototypes
**
** $Header: /home/cvs/nss/sonata-pkg/dx/prototype/inc/unit.h,v 1.1.1.1 2006/08/02 05:29:54 kes Exp $
** $Log: unit.h,v $
** Revision 1.1.1.1  2006/08/02 05:29:54  kes
** pdm-1_59
**
** Revision 1.1  2001/11/09 04:32:38  kes
** *** empty log message ***
**
 * Revision 1.1  98/10/27  08:18:44  kes
 * Initial revision
 * 
 * Revision 1.4  96/11/10  14:28:57  kes
 * Changes to convert from FUDD to TC; removed unit_t definition,
 * changed nickname to alias, removed scs_unit_t definition.
 * 
 * Revision 1.3  96/05/15  18:16:26  kes
 * Added struct unit, struct scs_unit, added UnitObserving.
 * 
 * Revision 1.2  96/02/13  09:51:11  kes
 * Added prototypes for GetUnitByName, GetUnitPort, UnitState, SetUnitState.
 * 
 * Revision 1.1  96/01/31  12:31:08  kes
 * Initial revision
 * 
**********************************************************************/

#ifndef _UNIT_H
#define _UNIT_H

#include "nsstypes.h"
#include "client.h"

//
// A table of unit_info structures specifies the valid units in the system.
// Note that units and clients are not equivalent: a unit is a logical
// processing entity, while a client is a uni-directional connection.
//
typedef struct unit_info {
	unit_t unit;						/* unit type */
	char name[MAX_NAME_LEN];			/* unit name (Master, MainA, etc.) */
	char alias[MAX_NAME_LEN];			/* unit nickname (M0, M1, etc.) */
} unit_info_t;

// external arrays
extern unit_info_t Unit[];

/* function prototypes */
error_t SetUnit(unit_t);
unit_t GetUnit(void);
char *GetUnitName(unit_t);
char *GetUnitAlias(unit_t);
unit_t GetUnitByName(const char *);
uint16_t GetUnitPort(unit_t);
#ifdef notdef
bool_t UnitActive(unit_t);
bool_t UnitObserving(unit_t);
tcstate_t UnitState(unit_t);
void SetUnitState(unit_t, tcstate_t);
client_t *GetClient(unit_t, client_type_t);
#endif

#endif /* _UNIT_H */