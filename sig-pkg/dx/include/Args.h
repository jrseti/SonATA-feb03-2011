/*******************************************************************************

 File:    Args.h
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

//
// Command-line arguments class; singleton
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/Args.h,v 1.10 2009/06/16 15:13:11 kes Exp $
//
#ifndef _ArgsH
#define _ArgsH

#include <stdlib.h>
#include <string>
#include <ssePorts.h>
#include "Err.h"
#include "ReadFilter.h"
#include "System.h"

using std::string;
using namespace sonata_lib;

namespace dx {

class Args {
public:
	static Args *getInstance(int argc, char **argv);
	static Args *getInstance();

	~Args();

	bool singlePol() {
		return (flags.onePol);
	}
	Polarization getPol() {
		return (polarization);
	}
	bool simulateInput() {
		return (flags.simulateInput);
	}
	bool useTaggedData() {
		return (flags.taggedData);
	}
	bool swapInputs() {
		return (flags.swap);
	}
	bool doCacheEfficientDadd() {
		return (dadd.cacheEfficient);
	}
	bool initBuffers() {
		return (flags.initBufs);
	}
	bool insertAbsolutePath() {
		return (flags.absolutePath);
	}
	bool insertPath() {
		return (sim.insertPath);
	}
	bool runTest() {
		return (flags.test);
	}
	bool loadSliceNum() {
		return (flags.loadSlice);
	}
	bool loadSubchannelNum() {
		return (flags.loadSubchannel);
	}
	bool logSseMessages() {
		return (flags.logSseMsgs);
	}
	bool useHanning() {
		return (flags.hanning);
	}
	bool reportCwStats() {
		return (flags.cwStats);
	}
	bool reportHits() {
		return (flags.logHits);
	}
	bool suppressBaselineReport() {
		return (flags.suppressBaselines);
	}
	bool noSSE() {
		return (noSse.ipFile != "");
	}
	bool zeroDCBins() {
		return (flags.zeroDCBin);
	}
//	bool useBaselineActivity() {
//		return (baselineActivity);
//	}
	int32_t getSubchannels() {
		return (subchannels);
	}
	int32_t getCacheRows() {
		return (dadd.cacheRows);
	}
	int32_t getSimulationNoise() {
		return (sim.noise);
	}
	int32_t getInsertBin() {
		return (sim.insertBin);
	}
	int32_t getOffHalfFrames() {
		return (sim.offHalfFrames);
	}
	int32_t getOnHalfFrames() {
		return (sim.onHalfFrames);
	}
	int32_t getPreBLSubchannel() {
		return (sim.preBLSubchannel);
	}
	int32_t getPostBLSubchannel() {
		return (sim.postBLSubchannel);
	}
	int32_t getPostBLStartBin() {
		return (sim.postBLStartBin);
	}
	float32_t getPostBLDriftRate() {
		return (sim.postBLDriftRate);
	}
	float32_t getPostBLPower() {
		return (sim.postBLPower);
	}
	uint8_t getBinVal() {
		return (sim.binVal);
	}
	float32_t getDriftRate() {
		return (sim.driftRate);
	}
	string getDxName() {
		return (dxName);
	}
	string getHost() {
		return (host);
	}
	int32_t getPort() {
		if (port)
			return (port);
		return (atoi(DEFAULT_DX_PORT));
	}
	string getMcAddr() {
		return (mcAddr);
	}
	int32_t getMcPort() {
		return (mcPort);
	}
	string& getIpFile() {
		return (noSse.ipFile);
	}
	string& getOpDir() {
		return (noSse.opDir);
	}
	int getBaselineDelay() {
		return (noSse.baselineDelay);
	}
	void setBaselineDelay(int d) {
		noSse.baselineDelay = d;
	}
	int32_t getMaxFrames() {
		return (maxFrames);
	}
	int32_t getFoldings() {
		return (foldings);
	}
	float64_t getOversampling() {
		return (oversampling);
	}
	float64_t getChannelOversampling() {
		return (chanOversampling);
	}
	float64_t getChannelBandwidth() {
		return (chanBandwidth);
	}
	uint32_t getSrc() {
		return (src);
	}
	const string& getFilterFile() {
		return (filterFile);
	}
	bool useCustomFilter() {
		return (flags.customFilter);
	}
	const FilterSpec& getFilter() {
		return (filter);
	}

private:
	static Args *instance;

	struct Flags {
		bool onePol;					// run a single polarization (clone it)
		bool simulateInput;				// simulate channelizer input
		bool taggedData;				// use tagged data
		bool swap;						// swap input (real and imaginary)
		bool absolutePath;				// insert an absolute path
//		bool baselineActivity;			// use separate activity to baseline
		bool initBufs;					// initialize buffers
		bool logSseMsgs;				// log messages from SSE
		bool hanning;					// use Hanning window for CW
		bool cwStats;					// report CW stats
		bool logHits;					// log hits to SSE
		bool loadSlice;					// store slice number in CWD data
		bool loadSubchannel;			// store subchannel number in CD data
		bool suppressBaselines;			// suppress baseline reports
		bool test;						// run timing test on startup
		bool zeroDCBin;					// zero the DC bins in CWD
		bool customFilter;				// use a custom filter file

		Flags(): onePol(false), simulateInput(false), taggedData(false),
				swap(false), absolutePath(false), initBufs(false),
				logSseMsgs(false), hanning(true), cwStats(true),
				logHits(true), loadSlice(false), loadSubchannel(false),
				suppressBaselines(false), test(false), zeroDCBin(false),
				customFilter(false)
				{}
	} flags;
	struct Sim {
		bool insertPath;				// insert a path
		uint8_t binVal;					// bin value to insert
		int32_t insertBin;				// bin at which to insert a path
		int32_t offHalfFrames;			// off time for pulse signal
		int32_t onHalfFrames;			// on time for pulse signal
		int32_t preBLSubchannel;		// non-drifting pre-BL test signal
		int32_t postBLSubchannel;		// drifting post-BL test signal
		int32_t postBLStartBin;			// drifting post-BL start bin
		int32_t noise;					// simulation noise power
		float32_t postBLDriftRate;		// drifting post-BL drift rate
		float32_t postBLPower;			// drifting post-BL power
		float32_t driftRate;			// drift rate in bins/spectrum

		Sim(): insertPath(false), binVal(0), insertBin(0), offHalfFrames(1),
				onHalfFrames(1), preBLSubchannel(0), postBLSubchannel(0),
				postBLStartBin(0), noise(1), postBLDriftRate(0), postBLPower(0),
				driftRate(0)
				{}
	} sim;
	struct Dadd {
		bool cacheEfficient;			// use divide and conquer DADD
		int32_t cacheRows;				// # of DADD rows to cache

		Dadd(): cacheEfficient(true), cacheRows(DADD_CACHE_ROWS) {}
	} dadd;
	struct NoSse {
		string ipFile;					// ipFile that contains burnIn cmds
		string opDir;					// output directory
		int baselineDelay;				// The baseline delay comes from the
										// ipfile
		NoSse(): ipFile(""), opDir(""), baselineDelay(0) {}
	} noSse;

	string dxName;						// name of this dx
	string host;						// host name
	string mcAddr;						// multicast base address
	string filterFile;					// filter file
	int32_t port;						// host port number
	int32_t mcPort;						// multicast port
	uint32_t src;						// source
	int32_t subchannels;				// total # of subchannels
	int32_t usableSubchannels;			// # of usable subchannels
	int32_t maxFrames;					// max # of frames in an activity
	int32_t foldings;					// # of foldings in DFB filter
	float64_t oversampling;				// percentage of oversampling
	float64_t chanOversampling;			// percentage of channel oversampling
	float64_t chanBandwidth;			// nominal channel bandwidth
	Polarization polarization;			// polarization
	FilterSpec filter;					// custom filter specification

	void parse(int argc, char **argv);
	void usage();

	// hidden
	Args(int argc, char **argv);

	// forbidden
	Args(const Args&);
	Args& operator=(const Args&);
};

}

#endif