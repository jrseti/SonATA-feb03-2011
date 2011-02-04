/*******************************************************************************

 File:    SpectrometryTask.cpp
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
//  This task does the spectrometry calculations
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/SpectrometryTask.cpp,v 1.1 2008/01/17 19:46:15 kes Exp $
//
#include <math.h>
#include <string.h>
#include <sys/time.h> 
#include <DxOpsBitset.h>
#include "SpectrometryTask.h"
#include "SignalIdGenerator.h"

//#define DO_TIMING_TESTS   
#ifdef DO_TIMING_TESTS
struct timeval tvE;
struct timeval tvL;
struct timezone tz;
#endif

SpectrometryTask::SpectrometryTask(string name_) :
	QTask(name_, SPECTROMETRY_PRIO), activity(0), CTBufsFilled(0),
			CTBufsEmptied(0), initWave(false)
{
	float twopi, w;
	twopi = 8.0 * atan(1.0);
	ObsData obsData;

#ifdef DO_TIMING_TESTS
	tvE.tv_usec = 0;
	tvL.tv_usec = 0;
#endif

	// allocate the frame array
	int32_t size = FRAME_SAMPLES * sizeof(complexFloat);
	Frame = static_cast<complexFloat *> (fftwf_malloc(size));

	if (!initWave) {
		/* initialize the fake signal array */
		for (int i=0; i<FRAME_SAMPLES; i++) {
			w = (i+0.5)/FRAME_SAMPLES * twopi;
			Wave[i] = sinf(w);
		}
		initWave = true;

		// create the fftw plans
		plan1024 = fftwf_plan_dft_1d(FRAME_SAMPLES, (fftwf_complex *) Frame,
				(fftwf_complex *) Frame, FFTW_FORWARD, FFTW_MEASURE);
		plan512 = fftwf_plan_dft_1d(FRAME_SAMPLES / 2, (fftwf_complex *) Frame,
				(fftwf_complex *) Frame, FFTW_FORWARD, FFTW_MEASURE);
	}
}

SpectrometryTask::~SpectrometryTask() 
{
	fftwf_free(Frame);
}

void * SpectrometryTask::getCornerturnLoc(int halfFrame, int subchannel) {
	return (&(CornerturnBuf[halfFrame][subchannel][0]));
}

void SpectrometryTask::extractArgs() 
{
	SpectrometryArgs *spectrometryArgs;

	// extract startup args
	spectrometryArgs = static_cast<SpectrometryArgs *> (args);
	msgList = *spectrometryArgs->msgList;
	cdTask = (InputTask *)(*spectrometryArgs->cdTask);
	cwTask = (InputTask *)(*spectrometryArgs->cwTask);
	blTask = (InputTask *)(*spectrometryArgs->blTask);
	//	dspIpTask = (DspInputTask *)(*spectrometryArgs->dspIpTask);
	pol = (Polarization)(spectrometryArgs->pol);
	pulseTask = (InputTask *)(*spectrometryArgs->pdTask);

}

void SpectrometryTask::handleMsg(Msg *msg)
{
	ActivityParameters params;
	OpsBitset operations;
	ObsData obsData;

	Debug(DEBUG_PASS_THRU, 1, "in SpectrometryTask::handleMsg");
	Debug(DEBUG_PASS_THRU, (int)msg->getCode(), getName());

	switch (msg->getCode()) {
	case START_TIME: 
	{
		// Save the activityId
		SseInterfaceHeader hdr = msg->getHeader();
		activityId = hdr.activityId;
		Debug(DEBUG_PASS_THRU, activityId, "START_TIME, activityId:");
		activity = State.findActivity(hdr.activityId);
		Debug(DEBUG_PASS_THRU, (void *) activity, "START_TIME, activity:");
		activity->getObsData(&obsData);
		char msg[128];
		sprintf(msg, "set activityId: %d", activityId);
		Debug(DEBUG_SPECTROMETRY, 1, msg);
		postBLIdx = 0;
		postBLBin = (obsData.postBLStartBin+512)%1024;
		postBLDrift = obsData.postBLObsDriftBins /(obsData.nFrames
				*FRAME_SAMPLES);
		postBLDutyCycle = 0;
		halfFrame = 0;
		zeroDCBin = (obsData.specialObsFlags & BW_OPT_ZERO_DC_BIN) ? true
				: false;
		startSubchannel = (SUBCHANNELS_PER_SPECTRUM - obsData.bandwidth) / CPUS;
		endSubchannel = startSubchannel + obsData.bandwidth;
		if (obsData.resetBaseline)
			memset(Baseline, 0, sizeof(Baseline));
		// Set the passthru bandwidth
		//		dspIpTask->setPassthruBandwidth(obsData.bandwidth);
		break;
	}
	case HalfFrameReady: 
	{
		if (activity) {
			HalfFrameReadyData *data =
					static_cast<HalfFrameReadyData *> (msg->getData());
			int ipBufN = data->bufNum;
			uint32_t status = data->status;
			Debug(DEBUG_PASS_THRU, ipBufN, "HalfFrameReady, buffer:");
		} else
			doSpectrometry(ipBufN, status);
		else
		Debug(DEBUG_PASS_THRU, 1, "***Error*** no activity has been started.");
		break;
	}
	default:
		break;
	}
}
/*
 ** (Possible) RACE CONDITION:
 **
 ** There is a prossible race condition when running in passthru mode if 
 ** both DEBUG_PASS_THRU and DEBUG_SPECTROMETRY are on and logging is 
 ** going to an xterm.  
 **
 ** Disabling logging, or re-directing output to a file makes the problem
 ** go away.  In the test case, the problem shows up as a dropping of the 
 ** last of 16 half-frames of data  (an under-reporting of hits in the 
 ** passthru case by about 6 %). 
 **
 ** PD completion isn't reported the same way as for other data streams 
 ** (by counting buffers).  Instead, it's sent from 
 ** PDInputTask::processDspInputComplete().
 **
 ** In the non-PD case, readDspBuffer() and passDspBlock() both call 
 ** testHostWriteBuf(), which calls testCollectionComplete(), which sends 
 ** the DATA_COLLECTION_COMPLETE message.  The function processDspInputComplete() 
 ** does nothing,
 **
 ** In the PD case, testHostWriteBuf is overridden with a version that never 
 ** calls testCollectionComplete, and processDspInputComplete() is overriden 
 ** with a version that sends the DATA_COLLECTION_COMPLETE directly.
 **
 ** The problem in the PD case is that the last data buffer follows a path 
 ** from DspInputTask to SpectrometryTask to PDInputTask, but the completion 
 ** follows a shorter path from DspInputTask directly to PDInputTask.  Thus if
 ** SpectrometryTask is slow, the completion message can arrive at PDInputTask 
 ** first, causing the pulse data buffer to be written to disk before it 
 ** finishes filling.
 **
 ** It might be possible to fix this by adjusting task priorities, or we might 
 ** need to change the handshaking somewhat. 
 */
void SpectrometryTask::doSpectrometry(int idx, uint32_t status) 
{
	char msg[256];
	ObsData obsData;

#ifdef DO_TIMING_TESTS

	if (!gettimeofday( &tvE, &tz))
	{
		long adj = 0;
		if (tvL.tv_usec > tvE.tv_usec)
		adj = 1000000;

		LogInfo(DXE_NE, 0,
				"--> :Enter doSpec:hf:%d:%s:timeofday:secs %ld:usecs %ld:"
				"time outside doSpec:%ld",
				halfFrame,getName(),tvE.tv_sec, tvE.tv_usec,
				((tvE.tv_usec+adj)-tvL.tv_usec));
	}
#endif

	activity->getObsData(&obsData);
	sprintf(
			msg,
			"doSpectrometry: activityId: %d IpBuf[%d], bandwidth %d, halfFrame %d ",
			activityId, idx, obsData.bandwidth, halfFrame);
	Debug(DEBUG_PASS_THRU, halfFrame, msg);

	if (idx != halfFrame%NUM_DDC_BUFFERS) {
		ErrStr( (int)halfFrame%NUM_DDC_BUFFERS, "Unexpected input buffer");
		ErrStr(idx, msg);
	}
	nPulsesThisFrame = 0;
	for (int subchannel = startSubchannel; subchannel < endSubchannel;
			++subchannel) {
		// load data for this subchannel from InputBuffer to HalfFrame
		int prev = (idx + NUM_DDC_BUFFERS - 1) % NUM_DDC_BUFFERS;
		if (halfFrame == 0)
			memset(&HalfFrame[0][0], 0, CD_SPECTRA_PER_HALF_FRAME
					*sizeof(complexFloat));
		else
			memcpy(&HalfFrame[0][0], &(CornerturnBuf[prev][subchannel][0]),
					CD_SPECTRA_PER_HALF_FRAME*sizeof(complexFloat));

		memcpy(&HalfFrame[1][0], &(CornerturnBuf[idx][subchannel][0]),
				CD_SPECTRA_PER_HALF_FRAME*sizeof(complexFloat));
#if 1			
		processSubchannel(subchannel, status);
#endif
	}
	halfFrame++;

	Debug(DEBUG_SPECTROMETRY, (int)halfFrame,
			"SpectrometryTask storing Baseline data");
	blTask->passDspBlock(0, status, halfFrame, 0, &Baseline[0], 1);

	Debug(DEBUG_SPECTROMETRY, (int)halfFrame,
			"SpectrometryTask storing Pulse data");

	pulseTask->passDspBlock(0, status, halfFrame, 0, &PulseData[0],
			nPulsesThisFrame);
	++CTBufsEmptied;
#ifdef DO_TIMING_TESTS
	if (!gettimeofday( &tvL, &tz)) {
		long adj = 0;
		if (tvE.tv_usec > tvL.tv_usec)
		adj = 1000000;
		LogInfo(DXE_NE, 0,
				"--> :Leave doSpec:hf:%d:%s:timeofday:secs %ld:usecs %ld:time inside doSpec :%ld",
				halfFrame, getName(),tvL.tv_sec, tvL.tv_usec,
				((tvL.tv_usec+adj)-tvE.tv_usec));
		LogInfo(DXE_NE, 0, "--> timeofday: %s",getName());
	}
#endif
}
bool SpectrometryTask::SubchannelMasked(int subchannel) {
	ObsData obsData;
	activity->getObsData(&obsData);
	/*
	 **
	 ** Find the index in the subchannel array for this subchannel.
	 ** Assume subchannels start at 0.
	 */
	uint8_t mask;
	mask = obsData.subchannelMask[subchannel/8];
	mask = mask >> (subchannel % 8);
	mask = mask & 1;

	if (mask)
		return (true);

	return (false);
}

/*
 ** ProcessSubchannel: perform spectrum processing for a single subchannel
 **
 ** Synopsis:
 **		void ProcessSubchannel(int subchannel);
 ** Returns:
 **		Nothing.
 ** Description:
 **		Performs all spectrum processing for a single subchannel.
 ** Notes:
 **		This code was ported from the Blue Wave.
 */
void SpectrometryTask::processSubchannel(int subchannel, uint32_t status)
{
	bool masked;
	ObsData obsData;

	activity->getObsData(&obsData);

	masked = SubchannelMasked(subchannel);

#ifdef notdef
	if (obsData.ipPattern != none) {
		/* 
		 ** Only do this check when we are using the ddc pattern generator AND
		 ** we want to make sure the pattern is correct AND we don't generate
		 ** any output.  This bypasses all calculation of output values. 
		 */

		/*
		 ** Do pattern generator data verification (ddc4buf internal)
		 ** - only check new data (2nd half of frame)
		 ** - for now, only check every 16th sample (2k reset problem)
		 ** - don't check 1st sample (patgen restart problem)
		 ** - after check, set "masked" to skip remainder of processing
		 ** - if already masked, we didn't read, so don't check
		 */
		if (!masked) {
			int i;
			int *data = (int *)HalfFrame[1];
			if (obsData.ipPattern == ddc_patgen_local)
			{
				/* 
				 ** Testing the ddc patten generator in local mode
				 ** means causing a fatal error when the data contains errors.
				 */
				for (i = 0; i < 512; i++)
				{
					int expect = subchannel | ((0xf & i) << 12);
					expect = (expect << 16) | expect;
					if (data[i] != expect)
					Fatal(666);
				}
			}
		}
		masked = true;
	}
#endif

#if 1  /* Is this the Baseline Problem? Bugzilla #33? XX REMOVE for discrempency testing*/
	if ((subchannel == obsData.preBLSubchannel)
			&& (obsData.ipOption == sw_pattern)) {
		/* Inject a 256 Hz signal in a single subchannel in the sw pattern case */
		complexFloat *hf0, *hf1;
		complexFloat rPower(4.0, 0.0);
		complexFloat iPower = (0.0, 4.0);

		hf1 = HalfFrame[1];
		hf0 = HalfFrame[0];
		for (int i=0; i < CD_SPECTRA_PER_HALF_FRAME; i += 4) {
			hf0[i+1] = hf0[i+1] + rPower;
			hf0[i+3] = hf0[i+3] - rPower;
			hf1[i+1] = hf1[i+1] + rPower;
			hf1[i+3] = hf1[i+3] - rPower;
			hf0[i+0] = hf0[i+0] + iPower;
			hf0[i+2] = hf0[i+2] - iPower;
			hf1[i+0] = hf1[i+0] + iPower;
			hf1[i+2] = hf1[i+2] + iPower;
		}
	}
#endif

	/*
	 ** load and convert the time-domain data for a pair
	 ** of half frames for the specified subchannel
	 */
	if (!masked)
		ConvertInputData(subchannel, obsData.baselineWeighting);

	/* 
	 ** Inject a signal in the frame based on configuration values
	 ** specified in the start observation command.
	 */
	if ((subchannel == obsData.postBLSubchannel)
			&& (obsData.ipOption == sw_pattern)) {
		uint32_t dutyCycle;
		float idx, delta, drift;
		int i, idx2;

		idx = postBLIdx; /* running loc w/in Wave tbl	   */
		delta = postBLBin; /* num signals to skip in wave tbl */
		drift = postBLDrift;
		dutyCycle = postBLDutyCycle; /*running loc w/in on/off cycle */

#if 0 // Commented out for checkin
		LogInfo(DXE_NE, 0, "inject subchannel %d, idx %f, delta %f, drift %f",
				subchannel, idx, delta, drift);
#endif

		for (i=0; i< FRAME_SAMPLES; i++) {
			/* Check duty cycle to see if signal s:b injected */
			delta += drift;
			idx += delta;
			if (idx > FRAME_SAMPLES)
				idx -= FRAME_SAMPLES;
			if (dutyCycle < obsData.postBLOnTime) {
				idx2 = (int)(idx + 0.5);
				Frame[i].real() += obsData.postBLSignalPower*(Wave[(idx2+FRAME_SAMPLES/4)%FRAME_SAMPLES]);
				Frame[i].imag() += obsData.postBLSignalPower
						*(Wave[idx2%FRAME_SAMPLES]);
			}

			/* indicate current sample has been processed */
			if (++dutyCycle == (obsData.postBLOnTime + obsData.postBLOffTime))
				dutyCycle = 0;

			if (i == CD_SPECTRA_PER_HALF_FRAME) {
				/* Save the current settings for the next half frame */
				postBLDutyCycle = dutyCycle;
				postBLBin = delta;
				postBLIdx = idx;
			}
		}

	}
	if (subchannel == 0)
		Debug(DEBUG_SPECTROMETRY, (int)obsData.baselineDelay,
				"SpectrometryTask::processSubchannel baseline delay check");

	if (halfFrame < obsData.baselineDelay)
		return;

	/*
	 ** At this point, we have the baselined, floating-point subchannel in
	 ** Frame[].  We now do the following:
	 ** 1) output it as Confirmation data
	 ** 2) do a canned FFT to get 1Hz bins
	 ** 3) output the 1Hz data in Pulse and CW formats.
	 */

	if (obsData.cdOutputOption == from_ddc) {
		/* output the "real" cd_data */
		if (masked)
			ZeroCDData();
		else
			ComputeCDData(&Frame[FRAME_SAMPLES/2]);
	} else if (obsData.cdOutputOption == tagged_data) {
		LoadCDTestPattern(subchannel);
	}

	if (subchannel == 0)
		Debug(DEBUG_SPECTROMETRY, halfFrame,
				"calling SpectrometryTask::StoreCDData");

	StoreCDData(subchannel, status);

	/* Only output CD data for the 0th half frame */
	if (halfFrame == obsData.baselineDelay)
		return;

#if 1 // disable FFT for timing tests
	if (!masked) {
		int n = FRAME_SAMPLES;
		int isign = -1;

		// Check for 2HZ.
		if (obsData.cwResolution == RES_2HZ) {

			//XXX	-Use the 2nd half of Frame to hold the data that
			//XXX	 will be used for the 2nd FFT.  I need to do this
			//XXX	 coz when the FFT is done the orig data is lost.
			//XXX	 The 4th Q of Frame get the original 3rd Q
			//XXX	 The 3rd Q of Frame gets the original 2nd Q
			//XXX	-For 2Hz do 2 512 fft's using wsave512

			/*
			 ** For 2Hz call 2 512 ffts... but first reorder the
			 ** data in the frame array so that  when the 2nd
			 ** 512 fft is called the desired time data is in the
			 ** second half of the frame array.
			 ** 
			 ** The reorder needs to be done because when the fft
			 ** is called it overwrites the original data in the
			 ** Frame with new data.
			 */

			/* 3rd quarter of Frame data goes to the 4th quarter */
			int srcIdx = FRAME_SAMPLES/2;
			int dstIdx = FRAME_SAMPLES - (FRAME_SAMPLES/4);

			for (int j=0; j<FRAME_SAMPLES/4; j++) {
				Frame[dstIdx] = Frame[srcIdx];
				//				Frame[dstIdx].im = Frame[srcIdx].im;
				++srcIdx;
				++dstIdx;
			}

			/* 2nd quarter of Frame data goes to the 3rd quarter */
			srcIdx = FRAME_SAMPLES/4;
			dstIdx = FRAME_SAMPLES/2;
			for (int j =0; j<FRAME_SAMPLES/4; j++) {
				Frame[dstIdx] = Frame[srcIdx];
				//				Frame[dstIdx].im = Frame[srcIdx].im;
				++srcIdx;
				++dstIdx;
			}
			n= FRAME_SAMPLES/2;

			// First do a 512 fft on the 1st half of Frame
			fftwf_execute_dft(plan512, (fftwf_complex *) Frame,
					(fftwf_complex *) Frame);

			// Second 512 fft on second half of data
			fftwf_execute_dft(plan512, (fftwf_complex *) &Frame[n],
					(fftwf_complex *) &Frame[n]);

			if (zeroDCBin) {
				Frame[512] = 0.0f + 0.0fi;
				//				Frame[512].im = 0;
			};

		}
		else if (obsData.cwResolution == RES_1HZ){

			// For 1HZ call the 1kfft... look in the fftw library
			fftwf_execute_dft(plan1024, (fftwf_complex *) Frame,
					(fftwf_complex *) Frame);

		}
		else {
			LogInfo(0, -1, "Invalid cwResolution: %d", (int)obsData.cwResolution);
			Fatal(DXE_PT);
		}

		if (zeroDCBin) {
			Frame[0] = 0.0f + 0.0fi;;
			//			Frame[0].im = 0;
		}
		float scale = sqrt(1.0/n);
		for (int i = 0; i < FRAME_SAMPLES; i++) {
			Frame[i] *= scale;
			//			Frame[i].im *= scale;
		}
#if 0 // XXX debug only -- don't check in
		if (subchannel == obsData.postBLSubchannel) {
			for (int i = 0; i < FRAME_SAMPLES; i++)
			{
				float power = Frame[i].re * Frame[i].re
				+ Frame[i].im * Frame[i].im;
				if (power > 10) {
					LogInfo(0, -1, "High Power: %f bin %d", power, i);
				}
			}
		}
#endif
	}
#endif

	/*
	 ** Calculate and possibly store CW data. 
	 */

	Debug(DEBUG_SPECTROMETRY, halfFrame, "SpectrometryTask calculating CW data");

	if (obsData.cwOutputOption == from_ddc) {
		/* output the "real" cw_data */
		if (masked)
		ZeroCwData();
		else
		ComputeCwData(&Frame[0], obsData.cwResolution);
	}
	else if (obsData.cwOutputOption == tagged_data) {
		/* output the tagged CW pattern */
		LoadCwTestPattern(subchannel);
	}

	if (obsData.cwOutputOption == by_pass) {
		/* 
		 ** Don't generate output in the by_pass case. So skip the Store stage
		 ** and simply kick off the next subchannel.
		 */
	}
	else {
		/* Store CW data to SRAMA */
		StoreCwData(subchannel, status, obsData.cwResolution);
	}

	/*
	 ** Calculate and possibly store Pulse data.
	 ** This is structured a little differently from the other 
	 ** cases, because pulse output is variable-length.  
	 */
	bool doStore = true;
	if (obsData.pulseOutputOption == by_pass)
	doStore = false;
	else if (obsData.pulseOutputOption == from_ddc) {

		if (masked)
			doStore = false;
		else {

			if (nPulsesThisFrame >= obsData.maxPulsesPerHalfFrame) {
				doStore = false;
			}
			else 			{

				ComputePulseData(Frame, obsData.cwResolution, subchannel,
						obsData.pulseThreshold);
			}
		}
	}
	else if (obsData.pulseOutputOption == tagged_data) {
		LogInfo(0, -1, "pulse tagged_data not supported.");
		Fatal(DXE_PT);
	}

	if (doStore) {
		/** Transfer pulse data **/
	}
}
/*
 ** ConvertInputData: convert input time-domain data
 **
 ** Synopsis:
 **	  rval = ConvertInputData(subchannel, smoothing);
 **	  error_t rval;	   return value
 **	  int subchannel;		subchannel number to load
 **	  float smoothing;	baseline weighting factor
 ** Returns:
 **	  0 if successful.
 **	  Error code if error.
 ** Description:
 **	  Loads a frame of integer time-domain data for a single
 **	  subchannel from the two half-frame buffers H0 and H1, converts
 **	  it to floating point, applies the power in each
 **	  bin to the time-domain baseline for the subchannel,
 **	  and stores the data in the frame buffer in internal data RAM.
 ** Notes:
 **	  Input data from the DDC is buffered in SDRAM, then
 **	  loaded a subchannel at a time for processing.  In order
 **	  process a subchannel, two half frames worth of data must
 **	  be available; three buffers are allocated in SDRAM to
 **	  allow incoming data in one buffer while the other two
 **	  buffers are being read.
 */
int SpectrometryTask::ConvertInputData(int subchannel, float smoothing)
{
	complexFloat *intX;
	complexFloat *fpX;
	int i, samplesPerHalfFrame;
	float re, im, baseline, power = 0;

	samplesPerHalfFrame = CD_SPECTRA_PER_HALF_FRAME;
	/*
	 ** load half-frame 0; we must use the same baseline as
	 ** the last time it was loaded.
	 */
	baseline = Baseline[subchannel];

	if (baseline == 0) {
		/*
		 ** we don't have a baseline -- estimate a starting value
		 ** using the second HalfFrame (the first is all zeros)
		 */
		intX = HalfFrame[1];
		for (i = 0; i < samplesPerHalfFrame; i++)
			power += std::norm(intX[i]);
		baseline = sqrt(samplesPerHalfFrame/power);
		Baseline[subchannel] = NewBaseline[subchannel] = baseline;
		power = 0;
	}

	intX = HalfFrame[0];
	fpX = &Frame[0];
	for (i = 0; i < samplesPerHalfFrame; i++)
		fpX[i] = intX[i] * baseline;

	/*
	 ** apply the new baseline for the second half-frame
	 */
	intX = HalfFrame[1];
	fpX = &Frame[CD_SPECTRA_PER_HALF_FRAME];
	baseline = Baseline[subchannel] = NewBaseline[subchannel];
	for (i = 0; i < samplesPerHalfFrame; i++) {
		power += std::norm(intX[i]);
		fpX[i] = intX[i] * baseline;
	}
	/* compute the next baseline for the subchannel */
	NewBaseline[subchannel] = sqrt(samplesPerHalfFrame/power);
	NewBaseline[subchannel] = smoothing * Baseline[subchannel] + (1 - smoothing)
			* NewBaseline[subchannel];
	return (0);
}
/*
 ** ComputeCDData: convert subchannel data to complex integer CD format
 **
 ** Synopsis:
 **	  void ComputeCDData(data);
 **	  complexFloat *data;	 subchannel data
 ** Returns:
 **	  Nothing.
 ** Description:
 **	  Converts complex float subchannel data to complex integer
 **	  confirmation data format, saturating all values which
 **	  are larger than the representable value.  
 ** Notes:
 **	  Confirmation data format consists of a 4-bit integer
 **	  complex value packed into a single 8-bit byte.
 **	  There is no overlapped spectrum for subchannel data.
 */
void SpectrometryTask::ComputeCDData(complexFloat *data) 
{
	int re, im, max = MAX_CD_VAL;
	uint8_t *cdData = CDData;
	complexFloat *fData, *end;

	end = data + CD_SPECTRA_PER_HALF_FRAME;
	for (fData = data; fData < end; fData++) {
		//XXX Timing tests
		// At some point we switched from rint to the addition and cast to try
		// to speed up this function, but this may be causing a DC artifact in
		// the waterfall.  Switch back and re-test.
#if 1
		re = (int)rint(fData->real());
		im = (int)rint(fData->imag());
#else
		re = (int)(fData->real()+0.5);
		im = (int)(fData->imag()+0.5);
#endif 
		if (re > MAX_CD_VAL)
			re = MAX_CD_VAL;
		if (re < -MAX_CD_VAL)
			re = -MAX_CD_VAL;
		if (im > MAX_CD_VAL)
			im = MAX_CD_VAL;
		if (im < -MAX_CD_VAL)
			im = -MAX_CD_VAL;
		*cdData++ = (re << 4) | (im & 0xf);
	}
}
/*
 ** ZeroCDData: Set the CDBuffer to zero for subchannels that are masked
 */
void SpectrometryTask::ZeroCDData() 
{
	/* Zero out the CDData buffer */
	int i;

	/* Set all spectra in the buffer to 0 */

	for (i = 0; i < CD_SPECTRA_PER_HALF_FRAME; i++) {
		CDData[i] = 0;
	};
}
/*
 ** build a test pattern in the CD output buffer
 ** bits 31-22 Spectrum
 ** bits 21-10 Subchannel
 ** bits 9-0 subchannel spectrum
 */
void SpectrometryTask::LoadCDTestPattern(int subchannel)
{
	int i, j=0;
	uint32_t *p = (uint32_t *)CDData;

	uint32_t seed = (halfFrame << 8) | ((subchannel) << 16);

	for (i = 0; i < CD_SPECTRA_PER_HALF_FRAME; i += 4)
		p[j++] = seed | (i / 4);

}
/*
 ** StoreCDData: store the CD data for a single subchannel
 **
 ** Synopsis:
 **	  void StoreCDData(data, subchannel);
 **	  uint32_t *data;	 confirmation data
 **	  int subchannel;		subchannel number
 ** Returns:
 **	  Nothing.
 ** Description:
 **	  Writes a half-frame of subchannel data to a buffer
 ** Notes:
 **	  The data has been converted by ComputeCDData.
 **	  Only the second half-frame of data in the frame buffer is
 **	  written, because the first half-frame was converted
 **	  and output the last time through.
 */
void SpectrometryTask::StoreCDData(int subchannel, uint32_t status)
{
	cdTask->passDspBlock(subchannel >= SUBCHANNELS_PER_CPU_SPECTRUM ? 1 : 0, status,
			halfFrame, subchannel % SUBCHANNELS_PER_CPU_SPECTRUM, CDData, 1);
}
/*
 ** ZeroCWData: Set the CWBuffer to zero for subchannels that are masked
 */
void SpectrometryTask::ZeroCwData() 
{
	int i;

	/* Set all spectra in the buffer to 0 */

	for (i = 0; i < sizeof(CwData)/sizeof(CwData[0]); i++) {
		CwData[i] = 0;
	}
}
/*
 ** ComputeCWData: convert bins to CW power format and pack
 **  
 ** Description:
 **	  Converts bins to 2-bit power and packs 16 bins in a
 **	  32-bit word, then writes the word to a buffer 
 **
 ** This is a hard-coded 1Hz-only version.  It may also work for
 ** higher resolutions, but the data layout has not been decided yet.
 */
void SpectrometryTask::ComputeCwData(complexFloat *data, Resolution res) 
{
	complexFloat *ptr, *end;

	/*
	 ** The FFT'ed data in Frame has positive frequencies first, and then
	 ** negative frequencies.  We do the second half of the array first, so
	 ** the CW data is reported in frequency order.
	 ** The transfer is done in a sub routine to increase efficiency.
	 */

	moveCwData(data, data+FRAME_SAMPLES, 0, res);

}
/*
 ** moveCWData: convert bins to CW power format and pack into the output buffer
 **
 ** Description:
 **	  Converts bins to 2-bit power and packs 16 bins in a
 **	  32-bit word, then writes the word to a buffer 
 */
void SpectrometryTask::moveCwData(complexFloat *ptr, complexFloat *end,
		int offset, Resolution res) 
{
	int power, max = (1 << CW_BITS_PER_BIN) - 1;
	uint32_t cwVal, *cwData;
	int cwBinsPerWord = (sizeof(uint32_t) * CWD_BINS_PER_BYTE), j, k, mask;
	//	float re, im;
	cwData = CwData + offset/cwBinsPerWord;
	cwVal = j = k = 0;

	switch (res) {
	case RES_1HZ: 
		mask = (sizeof(CwData)/sizeof(CwData[0])) / 2;
		break;
	case RES_2HZ: 
		mask = (sizeof(CwData)/sizeof(CwData[0])) / 4;
		break;
	case RES_4HZ: 
		mask = (sizeof(CwData)/sizeof(CwData[0])) / 8;
		break;
	default: 
		LogInfo(0, -1, "Invalid cwResolution: %d", res);
		Fatal(DXE_PT);
	}

	for (; ptr < end; ptr++) {
		//		re = __real__ (*ptr);
		//		im = __imag__ (*ptr);

		power = (int)(std::norm(*ptr));

		/* saturate to max power, then threshold */
		if (power > max)
			power = max;

		cwVal |= (power << (k*CW_BITS_PER_BIN));
		if (++k == cwBinsPerWord) {
			/* This looks ugly but it is efficient.
			 ** cwData++ = cwVal was 5 times slower!
			 ** go figure
			 */
			cwData[mask ^ j++] = cwVal;
			cwVal = 0;
			k = 0;
		}
	}
}

/*
 ** build a test pattern in the CW output buffer
 ** bits 31-22 Spectrum
 ** bits 21-10 Subchannel
 ** bits 9-0 Bin
 */
void SpectrometryTask::LoadCwTestPattern(int subchannel)
{
	int cwBinsPerWord = (sizeof(uint32_t) * CWD_BINS_PER_BYTE);
	int i, j=0;

	uint32_t seed = (halfFrame << 8) | ((subchannel) << 16);

	for (i = 0; i < FRAME_SAMPLES; i += cwBinsPerWord)
		CwData[j++] = seed | (i / 4);

}
/*
 ** StoreCwData: store packed CW power data
 **
 ** Description:
 **	  The CW buffer, which consists of 2-bit data,
 */
void SpectrometryTask::StoreCwData(int subchannel, uint32_t status,
		Resolution res)
{
	Debug(DEBUG_SPECTROMETRY, (void *) cwTask,
			"SpectrometryTask storing CW data");
	int segsPerSlice = CWD_SUBCHANNELS_PER_SLICE;
	int bytesPerSubchannel = CWD_BINS_PER_SUBCHANNEL_1HZ/CWD_BINS_PER_BYTE;
	int segment = subchannel % segsPerSlice;

	if (res == RES_1HZ) {
		char *tmp = ((char *)CWSlice) + segment * bytesPerSubchannel;
		memcpy(tmp, CwData, bytesPerSubchannel);
	} else if (res == RES_2HZ) {
		char *tmp = ((char *)CWSlice) + segment * bytesPerSubchannel/2;
		// copy 1st half of CWData to current segment in 1st half of CWSlice
		memcpy(tmp, CwData, bytesPerSubchannel/2);
		// copy 2nd half of CWData to corresponding segment in 2nd half of CWSlice
		memcpy(tmp + segsPerSlice*bytesPerSubchannel/2, CwData
				+((bytesPerSubchannel/2)/sizeof(CwData[0])), bytesPerSubchannel
				/2);
	}
	if (segment == segsPerSlice-1) {
		cwTask->passDspBlock(subchannel >= SUBCHANNELS_PER_CPU_SPECTRUM ? 1 : 0,
				status, halfFrame, subchannel % SUBCHANNELS_PER_CPU_SPECTRUM,
				CWSlice, 1);
	}
}

/*
 ** computePulseData
 **
 **	Converts spectrum bins to power and thresholds each bin to the 
 **	current pulse threshold, with a 7-bit integer power.
 **	All bins which are below threshold are 0.
 **
 **	Input:
 **	complexFloat *data;	 spectrum data
 **	Resolution res			resolution
 */
void SpectrometryTask::ComputePulseData(complexFloat *data, Resolution res,
		int subchannel, float32_t threshold)
{
	ObsData obsData;
	activity->getObsData(&obsData);

	int spectrumNum, binInSpectrum, binsPerSubchannel;
	float32_t power;
	int pulseIdx, binMask, halfMask;
	complexFloat *end, *start;
	//	float re, im;
	int nPulsesThisSubchannel = 0;

	end = data + FRAME_SAMPLES;
	start = data;

	for (; data < end; data++) {
		//		re = __real__ (*data);
		//		im = __imag__ (*data);

		//power = (int)(re * re + im * im + 0.5);
		power = std::norm(*data);

		/* Check for a pulse */
		if (power <= threshold)
			continue;

		/*
		 ** Generate the pulse report and store in the PulseDataArray
		 */

		pulseIdx = data - start;
		switch (res) {
		case RES_1HZ:
			binsPerSubchannel = CWD_BINS_PER_SUBCHANNEL_1HZ;
			spectrumNum = halfFrame;
			break;
		case RES_2HZ:
			binsPerSubchannel = CWD_BINS_PER_SUBCHANNEL_2HZ;
			spectrumNum = halfFrame*2 + (pulseIdx >> 9);
			break;
		case RES_4HZ:
			binsPerSubchannel = CWD_BINS_PER_SUBCHANNEL_4HZ;
			spectrumNum = halfFrame*4 + (pulseIdx >> 8);
			break;
		default:
			LogInfo(0, -1, "ComputePD: Invalid cwResolution: %d", res);
			Fatal(DXE_PT);
		}
		binMask = binsPerSubchannel -1;
		halfMask = binsPerSubchannel /2;
		binInSpectrum = (subchannel - startSubchannel) * binsPerSubchannel
				+ ((pulseIdx & binMask) ^ halfMask);

		PulseData[nPulsesThisFrame].resPolSpectrum = RES_POL_SPECTRUM(res, pol,
				spectrumNum);
		PulseData[nPulsesThisFrame].powerBin = POWER_BIN((int)power,
				binInSpectrum);
		PulseData[nPulsesThisFrame].power = power;

		++nPulsesThisFrame;
		if (nPulsesThisFrame >= obsData.maxPulsesPerHalfFrame)
			return;
		++nPulsesThisSubchannel;
		if (nPulsesThisSubchannel >= obsData.maxPulsesPerSubchannelPerHalfFrame)
			return;
	}
}
