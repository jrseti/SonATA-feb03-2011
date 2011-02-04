# Makefile

# Create UML sequence diagrams in pdf format from
# 'pic' files.  See the umlgraph package for details:
# http://www.spinellis.gr/sw/umlgraph/

targets = PdmInitialization.pdf \
	pdmSseStartupMsgs.pdf \
	pdmSseDataCollMsgs.pdf \
	DxSseDataCollMsgs.pdf \
	pdmSseSigDetMsgs.pdf \
	pdmSseSigDetMsgsMultibeamPart1.pdf \
	pdmSseSigDetMsgsMultibeamPart2.pdf \
	schedulerRunActSeq.pdf \
	PdmActivityStartup-Baseline-Accum.pdf \
	PdmDataCollection.pdf \
	DxDataCollection.pdf \
	DspCDDataCollection.pdf \
	DspCWDDataCollection.pdf \
	DspBslDataCollecion.pdf \
	PdmDetection.pdf \
	DxDetection.pdf \
	PdmConfirmation.pdf \
	DxConfirmation.pdf \
	Pdm2ndConfirmation.pdf \
	Dx2ndConfirmation.pdf \
	PdmArchive.pdf \
	DxArchive.pdf \
	ataPrimaryPointingLocks.pdf \
	beamformerCalAct.pdf \
	beamformerObsAct.pdf \
	channelizerSseMsgs.pdf \
	observeActivity.pdf

all: $(targets)

$(targets): %.pdf: %.pic
	pic2plot -Tps $< | ps2pdf - > $@

clean:
	-rm -f $(targets)
