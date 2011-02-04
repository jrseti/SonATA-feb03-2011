#!/bin/tcsh

# switchConfigFile-jane-test.tcsh

cd ~/OpenSonATA/sse-pkg/setup
cp expectedSonATAComponents-jane-test.cfg expectedSonATAComponents.cfg
cp expectedSonATAComponents.cfg ~/sonata_install/setup
echo "expectedComponents = expectedSonATAComponents-jane-test.cfg" 
