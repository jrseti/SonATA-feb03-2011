/*******************************************************************************

 File:    SpectrometryTask.h
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
// Spectrometry task
//
// This task does the spectrometry calculations
// 
// $Header: /home/cvs/nss/sonata-pkg/dx/src/SpectrometryTask.h,v 1.1 2008/01/17 19:48:53 kes Exp $
// $Log: SpectrometryTask.h,v $
// Revision 1.1  2008/01/17 19:48:53  kes
// *** empty log message ***
//
// Revision 1.2  2007/04/27 22:18:19  kes
// Input data is now complexFloat instead of complexShort.  Modified
// code to access complex values.
//
// Revision 1.1.1.1  2006/08/02 05:28:18  kes
// dx-1_59
//
// Revision 1.11  2004/02/02 19:00:31  nss
// Add timeval data for timing tests
//
// Revision 1.10  2003/12/18 23:18:59  srenner
// Add maxPulsesPerHalfFrame, maxPulsesPerSubchannelPerHalfFrame
//      and pulse power to passthru mode.
// Add obsData.bandwidth to the passThru case.
// Implement version 2 of the config file used in passthru mode.
//
// Revision 1.9  2003/12/02 03:03:36  kes
// ComputePulseData expects float32_t threshold, not int.
//
// Revision 1.8  2003/08/23 00:11:27  srenner
// add pulse data (1 & 2 Hz) to passthru mode
//
// Revision 1.7  2003/08/13 21:22:44  srenner
// First pass at 2hz support
//
// Revision 1.6  2003/07/31 23:10:20  carl
// Remove the unused cpu parameter
//
// Revision 1.5  2003/07/31 22:26:26  carl
// Passthru data structures (ie, the cornerturn buffer) is now part of the Spectrometry task to support multiple bw boards.
//
// Revision 1.4  2003/07/10 23:00:07  carl
// merge high-speed passthrough code, except:
// - Intel FFT is commented out in Makefile and SpectrometryTask
// - DiskFile.h has lseek64 decl needed in 2.2 (but not in 2.4)
// - not yet tested with PD data stream
// - not yet tested at full speed (-s only)
// - not yet tested with 2 BW boards
// - preBL signal injection needs to be uncommented in BW, and added to
//   SpectrometryTask
// uses bw_nvram-0_42
//
// Revision 1.2  2003/05/20 22:19:06  kes
// Merged passthru code; calls to Intel FFT are ifdefed out, so passthru
// will not run.  Need to install library and uncomment Makefile.am and
// SpectrometryTask.cpp.
//
// Revision 1.1.2.3  2003/01/23 22:03:33  carl
// Get the passthruBandwidth from the StartObs structure
// Move dxHalfFrameReadyData to Struct
//
// Revision 1.1.2.2  2003/01/22 22:32:17  carl
// Continue passthru development.
//
// Revision 1.1.2.1  2003/01/15 19:12:12  carl
// include file for Spectrometry task
//
//
//
#ifndef _SpectrometryTaskH
#define _SpectrometryTaskH

#include <fftw3.h>
#include "Struct.h"
#include "Task.h"
#include "QTask.h"
#include <sys/time.h>

// forward declarations

#define MAX_CD_VAL                        (0x7ffffff8)
#define MAX_CW_POWER                      (0x7ffffffc)
#define CW_BITS_PER_BIN                   (2)
#define MAX_PULSES_PER_FRAME 			  5000
#define MAX_PULSES_PER_SUBCHANNEL 			  10
//
// startup arguments
//
struct SpectrometryArgs {
	QTask **cdTask;
	QTask **cwTask;
	QTask **blTask;
	Polarization pol;
	QTask **pdTask; 
};

//
// This is a generic task which handles data collection
// for a given data stream.
//
class SpectrometryTask: public QTask {
public:
	SpectrometryTask(string name_);
	~SpectrometryTask();
#ifdef notdef
	struct complexFloat {
		float re;  
		float im;
	};
#endif

	void * getCornerturnLoc(int halfFrame, int subchannel);
	void incBufsFilled()   { ++CTBufsFilled;        };
	int  getCTBufsFilled() { return (CTBufsFilled); };
	int  getCTBufsEmpty()  { return (CTBufsEmptied);};
	void resetBufs()	   {CTBufsFilled=0;CTBufsEmptied=0;};

private:
	Activity *activity;				// current activity
	int32_t activityId;
	int startSubchannel, endSubchannel;
	float postBLIdx;
	float postBLBin;
	float postBLDrift;
	uint32_t postBLDutyCycle;
	int halfFrame;
	bool zeroDCBin;
	int nPulsesThisFrame;

	complexFloat CornerturnBuf[NUM_DDC_BUFFERS][SUBCHANNELS_PER_SPECTRUM][CD_SPECTRA_PER_HALF_FRAME];
	int CTBufsFilled;	/* The number of corner turn buffers that have been */
	int CTBufsEmptied;	/* filled and emptied in passthru mode */

	complexFloat HalfFrame[HALF_FRAMES_PER_FRAME][CD_SPECTRA_PER_HALF_FRAME];

	pulse_t PulseData[MAX_PULSES_PER_FRAME];

	float Baseline[SUBCHANNELS_PER_SPECTRUM];
	float NewBaseline[SUBCHANNELS_PER_SPECTRUM];
	complexFloat *Frame;

	uint8_t CDData[CD_SPECTRA_PER_HALF_FRAME];	
	uint32_t CwData[FRAME_SAMPLES/(sizeof(uint32_t)*CWD_BINS_PER_BYTE)];
	uint32_t CWSlice[CWD_BYTES_PER_SLICE_1HZ/sizeof(uint32_t)];

	InputTask *cdTask;
	InputTask *cwTask;
	InputTask *blTask;
	Polarization pol;
	InputTask *pulseTask;

	// Structures used to measure spectrometry timing
    struct timeval tvE;
    struct timeval tvL;
    struct timezone tz;

	float Wave[FRAME_SAMPLES];
//	float wsave1024[3*FRAME_SAMPLES/2];  	// For 1Hz
//	float wsave512[(3*FRAME_SAMPLES/2)/2]; 	// For 2Hz
	bool initWave;

	// FFTW plans
	fftwf_plan plan1024;
	fftwf_plan plan512;

	// methods
	void extractArgs();
	void handleMsg(Msg *msg);

	void doSpectrometry(int, uint32_t status);
	void processSubchannel(int, uint32_t status);
	bool SubchannelMasked(int);

    int ConvertInputData(int, float);

	void ZeroCDData();
	void LoadCDTestPattern(int);
	void ComputeCDData(complexFloat *);
	void StoreCDData(int, uint32_t);
	
	void ZeroCwData();
	void ComputeCwData(complexFloat *, Resolution);
	void LoadCwTestPattern(int);
	void StoreCwData(int subchannel, uint32_t status, Resolution);
	void moveCwData(complexFloat *,  complexFloat *, int, Resolution );
	void ComputePulseData(complexFloat *, Resolution, int subchannel,
			float32_t threshold);
	// forbidden
	SpectrometryTask(const SpectrometryTask&);
	SpectrometryTask& operator=(const SpectrometryTask&);
};

#endif