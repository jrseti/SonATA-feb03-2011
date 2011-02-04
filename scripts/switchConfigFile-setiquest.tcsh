#!/bin/tcsh

# switchConfigFile-setiquest.tcsh

cd ~/OpenSonATA/sse-pkg/setup
cp expectedSonATAComponents-546KHz-12dx.cfg expectedSonATAComponents.cfg
cp expectedSonATAComponents.cfg ~/sonata_install/setup
echo "expectedComponents = expectedSonATAComponents-546KHz-12dx.cfg" 
