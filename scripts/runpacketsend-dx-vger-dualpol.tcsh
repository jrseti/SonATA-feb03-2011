#!/bin/tcsh

# runpacketsend-dx-vger-dualpol.tcsh

sudo packetsend -c -f ${HOME}/OpenSonATA/sse-data/pktdata/vger-dualpol-2010-07-14-406.pktdata -J 229.1.1.1 -j 51100 -n 1 -i 1050 -b 1
