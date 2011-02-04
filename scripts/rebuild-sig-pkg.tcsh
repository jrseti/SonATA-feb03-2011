#!/bin/tcsh

cd ${HOME}/OpenSonATA/sse-pkg

./reconfig --dxlibsonly

make -j5 install

cd ${HOME}/OpenSonATA/sig-pkg

./reconfig

make -j17 install
