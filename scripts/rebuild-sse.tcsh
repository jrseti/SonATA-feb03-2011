#!/bin/tcsh

cd ${HOME}/OpenSonATA/sse-pkg

./reconfig

make -j17 install
