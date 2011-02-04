#!/bin/tcsh

# switchConfigFile-1beam-800KHz-24dxs.tcsh

cd ~/OpenSonATA/sse-pkg/setup
cp expectedSonATAComponents-1beam-800KHz-24dx.cfg expectedSonATAComponents.cfg
cp expectedSonATAComponents.cfg ~/sonata_install/setup
echo "expectedComponents = expectedSonATAComponents-1beam-800KHz-24dx.cfg" 
