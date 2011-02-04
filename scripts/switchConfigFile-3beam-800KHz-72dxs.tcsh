#!/bin/tcsh

# switchConfigFile-3beam-800KHz-72dxs/tcsh

cd ~/OpenSonATA/sse-pkg/setup
cp expectedSonATAComponents-3beam-800KHz-72dx.cfg expectedSonATAComponents.cfg
cp expectedSonATAComponents.cfg ~/sonata_install/setup
echo "expectedComponents = expectedSonATAComponents-3beam-800KHz-72dx.cfg" 
