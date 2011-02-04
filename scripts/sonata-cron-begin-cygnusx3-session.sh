#!/bin/sh

# sonata-cron-begin-cygnusx3-session.sh

# start a SonATA observing session
# start up SSE
${HOME}/sonata_install/bin/sonata-startup-cron-wrapper.sh -batch

# allow enough time for all components to be ready
sleep 120

#send command to SSE to start Observing
${HOME}/sonata_install/bin/sonata-seeker-command-cron-wrapper.sh source ${HOME}/OpenSonATA/scripts/sonata-start-cygnusx3-obs.tcl
