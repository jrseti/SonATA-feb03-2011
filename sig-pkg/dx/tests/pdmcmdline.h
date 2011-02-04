/*******************************************************************************

 File:    pdmcmdline.h
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

//123    
// PDM command line args class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/tests/pdmcmdline.h,v 1.2 2009/02/12 18:52:46 kes Exp $
// $Log: pdmcmdline.h,v $
// Revision 1.2  2009/02/12 18:52:46  kes
// First compilable version of DX1.  Massive changes from the original
// DX0 (SonATA demo) code.  Compiles and links, but many known bugs and
// is not yet runnable.
//
// Revision 1.1.1.1  2006/08/02 05:28:48  kes
// pdm-1_59
//
// Revision 1.2  2001/02/01 22:15:54  kes
// Added meaningless letters
//
// Revision 1.1.1.1  2001/01/02 07:10:58  kes
// Initial version
//
//
#ifndef _PDMCMDLINE_H
#define _PDMCMDLINE_H

#include <string>

class CmdLine {
public:
	CmdLine(const string& defaultHost_, int defaultPort_,
			int defaultPdmId_);
	~CmdLine();

	bool parse(int argc_, char *argv_[]);
	void usage();

	const string& getName() { return (pname); }
	string& getHost(); { return (host); }
	int getPort() { return (port); }
	int getId() { return (id); }
	PdmMode getMode() { return (mdoe) };
	bool simulation() { return (simulate) };

private:
	bool simulate;
	string pname;
	string host;
	int port;
	int id;

	// keywords for command line arguments
	string idKey;
	string hostKey;
	string modeKey;
	string portKey;
	string simulateKey;
	string unitKey;

	// methods for extracting data
	bool getIdValue(int argc_&, char	
private:
	CmdLine(const CmdLine& cmd_);
	CmdLine& operator= (const CmdLine& cmd_);
};

#endif