/*******************************************************************************

 File:    SseSimulator.cpp
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
// SSE->DX input handler task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/SseSimulator.cpp,v 1.4 2009/07/12 18:47:51 kes Exp $
//

#include <ssePorts.h>
#include "DxErr.h"
//#include "DxStruct.h"
#include "Log.h"
#include "PulseFollowupSignal.h"
//#include "SseBroadcastTask.h"
#include "SseInputTask.h"
#include "Timer.h"
#include <fstream>

using std::ifstream;
using std::ws;

namespace dx {

//
// The passthru command files has the following format:
//
// # Optional comment lines start with a # and can be any place in the file
// command=<number>
// activityId=<number>
// [optional parameters in the name=value format]
// parameters for a specific command follow that command in the file
//
// 3 commands are required to run one observation: CONFIGURE_DX,
//		SEND_DX_ACTIVITY_PARAMETERS and START_TIME.
//
// A sample configuration file looks like:
//
//		# Lines that start with # are treated as comment lines
//		Command=40003
//		activityId=0
//		Command=40010
//		activityId=100
//		ObsLen=16
//		ifcSkyFreq=4
//		dxSkyFreq=4
//		channelNumber=0
//		baselineDelay=0
//		operationsFlag=11
//		#This is a comment...
//		daddResolution=0
//		pulseThreshold=.5
//		maxPulsesPerHalfFrame=5000
//		maxPulsesPerSubchannelPerHalfFrame=10
//		daddThreshold=7
//		Command=40013
//		#This is a comment...
//		activityId=100
//
//
// Each parameter that will in a configuration file must be specified
// in the paramsTable below.
//
#define LINE_LEN 512
struct paramsEntry {
    int command;
    const char *paramName;
    const char *dflt;
    string val;
};
struct paramsEntry paramsTable[] = {
	/* Command					    Name    Dflt Value in config file */
    {SEND_DX_ACTIVITY_PARAMETERS, "ObsLen", "8", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "ifcSkyFreq", "4", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "dxSkyFreq", "4", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "channelNumber", "1", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "baselineHalfFrames", "0", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "baselineDelay", "0", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "operationsFlag", "11", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "pulseThreshold", ".5", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "maxPulsesPerHalfFrame", "5000", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "maxPulsesPerSubchannelPerHalfFrame", "10", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "daddThreshold", "7.0", ""},
    {SEND_DX_ACTIVITY_PARAMETERS, "daddResolution", "0", ""},

		/* End the table with a blank line */
    {0,"", "", ""},
};

class  passThruCfg {
public:
    passThruCfg();
    ~passThruCfg();
		// Returns the next command in the config file
    int getNxtCmd();
		// Returns activitiy id associated with the current command
    int getActivityId();
		// Returns specified parameter as an int
    int getInt(const char *,  int);
		// Returns specified param as a float
    float getFloat(const char *, int);

private:
    ifstream configFile;
    int nxtCmd, nxtActId, thisCmd, thisActId;
    string fName;
    int lineCounter;

    void getNextLine( string *, string *);
    int getIdx(string, int);
    int getCmd();
    int getActId();

};

//
// Open the passthru configuration file.
//
passThruCfg::passThruCfg() {


    configFile.open(Args::getInstance()->getIpFile().c_str());
    if (!configFile) {
		printf("*Error* '%s' not found.\n",
				Args::getInstance()->getIpFile().c_str());
		Fatal(ERR_IPFM);
    }
    nxtCmd = 0;
    nxtActId = 0;
    thisCmd = 0;
    thisActId = 0;
}
passThruCfg::~passThruCfg()
{
    configFile.close();
}

//
// Read the next line for the configuration file.
//
void
passThruCfg::getNextLine(string * pName, string * pVal)
{
    string ip, sep;
    char buf [LINE_LEN];


    while (1) {
        ++lineCounter;
        configFile.getline(buf, LINE_LEN, '\n');
        if (configFile.eof()) {
            return;
        }
        ip = buf;
        if (ip.substr(0,1) == "#")  {
            //XXXprintf("IGNORE COMMENT LINE '%s'\n",ip.c_str());
            continue;
        } else {
            //XXXprintf("PROCESSING LINE: '%s'\n",ip.c_str());
            // Parse the line
            int pos = ip.find("=",0);
            if (pos  == (int) string::npos) {
				printf("*Error* In file: %s\n",
						Args::getInstance()->getIpFile().c_str());
                printf("Invalid line at line %d. "
					"Expected command=value format. got: '%s'\n"
                    ,lineCounter,ip.c_str());
                exit(1);
            } else {
                *pName = ip.substr(0,pos);
                sep = ip.substr(pos, 1);
                int l = ip.length() - pos;
                *pVal = ip.substr(pos+1,l);
                break;
            }
        }
    }
}
//
// Ignoring comments the next line must be a command
//
int
passThruCfg::getCmd()
{
    string n, v;
    string tmp;
    tmp = "Command";

    getNextLine(&n, &v);
    if (n != tmp) { //.matches("Command")) {
        printf(" %s != %s \n",n.c_str(),tmp.c_str());
        printf("*Error* expected 'Command' got '%s'\n",n.c_str());
        exit(1);
    }
    return atoi(v.c_str());
}

//
// Ignoring comments the next line must be an activityId
//
int
passThruCfg::getActId()
{
    string n, v, tmp;

    getNextLine(&n,&v);
    tmp = "activityId";
    if (n != tmp) {
        printf("*Error* expected 'activityId' got %s\n",n.c_str());
        exit(1);
    }
    return atoi(v.c_str());
}
//
// Verify the parm is in the parmTable and return its index
int
passThruCfg::getIdx(string parm, int cmd)
{
    int i =0;
    while (paramsTable[i].command != 0){
        if (paramsTable[i].paramName == parm &&
			paramsTable[i].command == cmd ) return(i);
        i++;
    }
    printf("*Error* Parameter name '%s' at line %d in config file '%s'\n",
        parm.c_str(),lineCounter, fName.c_str());
    printf("        is not defined for command %d\n",cmd);
    printf("*Error* If this is a valid name add it to the paramsTable.\n");
    exit(1);
}
//
// Read the next command from the configuration file.
// Validate the parameters assoicated with the command.
// Update the val in the paramsTable.
//
int
passThruCfg::getNxtCmd()
{


    // Read the next command, its activity ID and
    // parameters.  Validate the parameters and
    // save the associated values
    string pName, pVal;
    int idx;


    if (!nxtCmd) {
        thisCmd = getCmd();
        thisActId = getActId();

    } else if (nxtCmd == -1) {
        // nothing left to read in the file.
        printf("Finished reading '%s' \n",fName.c_str());
        return 0;
    } else {
        thisCmd = nxtCmd;
        thisActId = nxtActId;
        nxtCmd = nxtActId = 0;
    }
    while (!nxtCmd) {
        getNextLine(&pName,&pVal);


        // The file contains no more data. Return the
        // previously read cmd and activity
        if (configFile.eof()) {
            nxtCmd = -1;
            continue;
        }


        if (pName == "Command") {
            nxtCmd = atoi(pVal.c_str());
            nxtActId = getActId();
           break;
        }

        idx = getIdx(pName,thisCmd);
        paramsTable[idx].val = pVal;

    }
    return (thisCmd);
}

// Return the specified parameter as an int
int
passThruCfg::getInt(const char * pName, int cmd)
{
    string n;
    n="";
    int idx = getIdx(pName, cmd);
    if (paramsTable[idx].val == n)
        return atoi(paramsTable[idx].dflt);
    else
        return atoi(paramsTable[idx].val.c_str());
};

// Return the specified parameter as a float.
float
passThruCfg::getFloat(const char * pName, int cmd)
{
    string n;
    n="";
    int idx = getIdx(pName, cmd);

    if (paramsTable[idx].val == n)
        return atof(paramsTable[idx].dflt);
    else
        return atof(paramsTable[idx].val.c_str());
};

// Return the activityId associated with the current command
int
passThruCfg::getActivityId() {
    return thisActId;
}

// Stand alone test file format:
//
//	command=value
//  #optional commands
//
//	Sample format:
//
//		# Start with a header.code of CONFIGURE_DX (40003)
//		Command=40003
//		# This activityId is not used, but it is read outside the switch
//		activityId=0
//		Command=40010
//		# The activityId for this command is a random number.
//		#  Must match the activityId for the START_TIME command.
//		activityId=100
//		ObsLen=16
//		ifcSkyFreq=4
//		dxSkyFreq=4
//		channelNumber=0
//		baselineDelay=0
//		operationsFlag=11
//		daddResolution=0
//		pulseThreshold=.5
//		maxPulsesPerHalfFrame=5000
//		maxPulsesPerSubchannelPerHalfFrame=10
//		daddThreshold=7
//
//		#	operations flag
// 		#			For CD_ONLY use a value of 1
//		#			For Baseline only use a value of 2
// 		#			For CD and CWD use a value of 17
//		#			For Baseline, CD & CWD use a value of 19
//		#			Any mode can be used with -s (simulated data)
//		#			Only CD_ONLY mode can be used with -z (DDC)
//		#				 operations flag is a bitmask, where the bit numbers
//		#				 are defined by the OperationsMask enum in
// 		#				 sseInterface.h.  To check the mask for a
//		#				 particular value, you need to mask it with
//		#				 (1 << enum_value).
//		#				 For some (but not all) useful masks, there are a
//		#				 separate set of #defines in SseInputTask.h with
//		#				 the conversion from bit numbers to masks already done.
//		#				Anything except "Baseline only" requires CD.
//		#				Pulses require Baselines to work properly, so...
//		#			For PULSE_DETECTION use a value of 11 (CD & Baseline needed)
//		#			For PULSE, CW, and CD use a value of 27
//
//		# header.code of START_TIME (40013)
//		Command=40013
//		# activityId for this command must be the same as for the
//		# SEND_DX_ACTIVITY_PARAMETERS command
//		activityId=100
//
//
void
SseInputTask::processIpFromFile()
{
	SseInterfaceHeader hdr;
	Msg *msg = 0;
	MemBlk *blk;
	void *data;
	int i;
	int counter = 0;
	static int lastMsg = 0;
	Timer timer;
	int obsLength = 0;
	int baseLineDelay = 0;
	int additionalAlarmDelay = 30;

		// Wait a bit...
	timer.sleep(1000);

	passThruCfg *pt = new passThruCfg();

	/*
	** Kludge --
	** Originally all standalone files started with the
	** CONFIGURE_DX command (40003) and had 13 fixed
	** entries. The config file should be changed to be
	** parameter based... but in the mean time (which
	** might be a long time)... An integer version number
	** has been added to the top of the file.
	**
	** Thus, if the first entry is 40003 this code looks
	** for the original 13 fixed entries and uses a
	** resolution of 1HZ.  If the first entry is a
	** version number than the version number is checked
	** as needed and new entries can be read from the file.
	*/
	/*
	** Remove ALL the version code. Configuration
	** file is now parameterized.
	*/

	int cmd = pt->getNxtCmd();
	while (cmd)
	{
		timer.sleep(3000);
		if (++counter == 10)
			Fatal(ERR_IPIV); 	// Something is wrong!

		hdr.activityId = pt->getActivityId();
		hdr.code = cmd;

		LogInfo(0, -1, "Reading standalone Config File cmd %d actId %d",
				hdr.code, hdr.activityId);

		hdr.messageNumber = lastMsg++;
		switch (hdr.code) {
			case CONFIGURE_DX:
			{
				hdr.dataLength = sizeof(DxConfiguration);
				msg = msgList->alloc();
				msg->setHeader(hdr);

					/* Allocate space for the cmd specific data */
				blk = partitionSet->alloc((size_t) hdr.dataLength);
				Assert(blk);
				if (!blk)
					Fatal(ERR_MAF);

				data = blk->getData();

				/* Initialize data.... */
				DxConfiguration *config = (DxConfiguration *)data;
				config->a2dClockrate = 104.16;

				msg->setData(data, hdr.dataLength, blk);
				break;
			}
			case SEND_DX_ACTIVITY_PARAMETERS:
			{
				hdr.dataLength = sizeof(DxActivityParameters);
				msg = msgList->alloc();
				msg->setHeader(hdr);

					/* Allocate space for the cmd specific data */
				blk = partitionSet->alloc((size_t) hdr.dataLength);
				Assert(blk);
				if (!blk)
					Fatal(ERR_MAF);

				data = blk->getData();

				/* Initialize data.... */
				DxActivityParameters *params = (DxActivityParameters *)data;
				params->activityId = hdr.activityId;
				obsLength = pt->getInt("ObsLen",cmd);;
				params->dataCollectionLength = obsLength; // seconds
					// RF Freq/MHz of IF band center
				params->ifcSkyFreq = pt->getInt("ifcSkyFreq",cmd);
				; 	// RF Freq/MHz of DX band center
				params->dxSkyFreq = pt->getInt("dxSkyFreq",cmd);
					// Number of subchannels. bins
				params->channelNumber = pt->getInt("channelNumber",cmd);
				baseLineDelay = pt->getInt("baselineDelay",cmd);
				cmdArgs->setBaselineDelay(baseLineDelay);
				int optParams = pt->getInt("operationsFlag",cmd);
					// optParams is a bitmask, where the bit numbers are
					// defined by the OperationsMask enum in
					// sseInterface.h.  To check the mask for a particular
					// value, you need to mask it with (1 << enum_value).
					// For some (but not all) useful masks, there are a
					// separate set of #defines in SseInputTask.h with
					// the conversion from bit numbers to masks already done.
LogInfo(0, -1, "processIpFromFile: optParams = %d ",optParams);
//					Either mode can be used with -s (simulated data)
//					Only CD_ONLY mode can be used with -z (DDC)
					//
					// For CD_ONLY use a value of 1
					// For CD and CWD use a value of 9
					// For CD, CWD & Baseline use a value of 11
					//
				params->operations = optParams;		// bit mask for DX operations
				if ((optParams & (1 << POWER_CWD)))
					additionalAlarmDelay += 30;

				params->sensitivityRatio = 0;       // main/remote rel sensitivity
				params->maxNumberOfCandidates = 0;
				params->clusteringFreqTolerance = 0; // Hz
				params->zeroDriftTolerance = 0;      // Hz/Sec
				params->zeroDriftTolerance = 0;      // Hz/Sec
				params->cwClusteringDeltaFreq = 0;   // bins
				params->badBandCwPathLimit = 12;    // max # paths /kHz (default approx == 256/slice)
					// Add daddResoultion to config file
				int res = pt->getInt("daddResolution",cmd);
				params->daddResolution = (Resolution)res;

				float32_t pulseThresh = pt->getFloat("pulseThreshold", cmd);
				params->maxPulsesPerHalfFrame =
						pt->getInt("maxPulsesPerHalfFrame",cmd);
				params->maxPulsesPerSubchannelPerHalfFrame =
						pt->getInt("maxPulsesPerSubchannelPerHalfFrame",cmd);

				params->daddThreshold =
						pt->getFloat("daddThreshold",cmd);
									// Sigma
									// Sigma 0==115 clusters
									// daddThreshold = 2 1137 clusters power.5
									// daddThreshold = 5 391 clusters power.5
									// daddThreshold = 7 11 clusters power.5
									// daddThreshold = 7  10 clusters power 0
									// daddThreshold = 8  1 clusters power 0
									// daddThreshold = 8.1  1 clusters power 0
									// *** 8.2 is where noise is gone ****
									// daddThreshold = 8.2  0 clusters power 0
									// daddThreshold = 8.5  0 clusters power 0
									// daddThreshold = 10 0 clusters power.5

				params->cwCoherentThreshold = 0;    // Sigma
				params->limitsForCoherentDetection = 0;
				params->badBandPulseTripletLimit = 0;  // # of paths
				params->badBandPulseLimit = 0;
				params->pulseClusteringDeltaFreq = 0;    // bins
				params->pulseTrainSignifThresh = 0;
				for (i = 0; i < MAX_RESOLUTIONS; ++i)
					params->requestPulseResolution[i] = SSE_FALSE;
				params->requestPulseResolution[RES_1HZ] = SSE_TRUE;
				params->alignPad2 = 0;         // alignment padding for marshalling
				for (i=0; i<MAX_RESOLUTIONS; ++i) {
					params->pd[i].pulseThreshold = pulseThresh;
					params->pd[i].tripletThreshold = 0;
					params->pd[i].singletThreshold = 0;
				}
				params->scienceDataRequest.sendBaselines = SSE_FALSE;
				params->scienceDataRequest.baselineReportingHalfFrames = 0;
#if 1
				params->scienceDataRequest.sendComplexAmplitudes = SSE_FALSE;
				params->scienceDataRequest.requestType = REQ_FREQ;
				params->scienceDataRequest.subchannel = 0;
#else
				// for waterfall debug
				params->scienceDataRequest.sendComplexAmplitudes = SSE_TRUE;
				params->scienceDataRequest.requestType = REQ_SUBCHANNEL;
				params->scienceDataRequest.subchannel = 1200;
#endif
				params->scienceDataRequest.rfFreq = 0;
				params->baselineSubchannelAverage = -1;
				params->baselineInitAccumHalfFrames = pt->getInt("baselineHalfFrames",cmd);
				LogInfo(0, -1, "actId: %d, obsLen: %d sendComplexAmplitudes: %d, ifcSkyFreq: %d", hdr.activityId,obsLength,params->scienceDataRequest.sendComplexAmplitudes,params->ifcSkyFreq);
				msg->setData(data, hdr.dataLength, blk);
				break;
				}
			case START_TIME:
			{
					LogInfo(0, -1, "processIpFromFile/START_TIME");
					hdr.dataLength = sizeof(StartActivity);
					msg = msgList->alloc();
					msg->setHeader(hdr);

						/* Allocate space for the cmd specific data */
					blk = partitionSet->alloc((size_t) hdr.dataLength);
					Assert(blk);
				if (!blk)
					Fatal(ERR_MAF);

				data = blk->getData();

				/* Initialize data.... */
				StartActivity *params = (StartActivity *)data;
				params->startTime.tv_sec = 0;
				params->startTime.tv_usec = 0;
				msg->setData(data, hdr.dataLength, blk);
				break;
			}

			default:
				FatalStr(hdr.code, "msg code");
		}
		cmdQ->send(msg);
		cmd = pt->getNxtCmd();
		hdr.code = cmd;
	}
		// Give the dx only so long to show its stuff...
//	double mtt = 2.5*(obsLength+baseLineDelay) + additionalAlarmDelay;
	double mtt = 100*(obsLength+baseLineDelay) + additionalAlarmDelay;
	int max_time_to_run = ((int) mtt);
	alarm(max_time_to_run);
	string n;
	name(n);
	LogInfo(0, -1, "Start Alarm on stand alone version: %d seconds. %d frames %d baseline delay\n",
			max_time_to_run, obsLength, baseLineDelay);

}

}