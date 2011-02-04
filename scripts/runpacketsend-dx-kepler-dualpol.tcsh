#!/bin/tcsh

# runpacketsend-dx-kepler-dualpol.tcsh

sudo packetsend -c -f ${HOME}/OpenSonATA/sse-data/pktdata/kepler-dualpol-2010-07-14-309.pktdata -J 229.1.1.1 -j 51100 -n 1 -i 1000 -b 1
