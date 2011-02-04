
Beamformer calibration target catalog preparation

1. Created lists of targets

   caldelay-simbad-query-script.txt 
   calphase-simbad-query-script.txt

2. submitted to simbad

3. edited results:

   - trimmed off headers
   - removed extra spaces from aliases
   - added additional aliases 
     
4. Converted delay cal list to seeker TargetCat database table format

catalog name = caldelay
first target id = 400

${HOME}/nss/sse-pkg/database/convertSimbadToTargetCat caldelay 400 caldelay-simbad-query-results-edited.txt > seeker.caldelay.load-data

5. Converted phase cal list to seeker TargetCat database table format

catalog name = calphase
first target id = 500

${HOME}/nss/sse-pkg/database/convertSimbadToTargetCat calphase 500 calphase-simbad-query-results-edited.txt > seeker.calphase.load-data

6. edited result files
    changing autoschedule from 'Yes' to 'No'

7. Moved final files to nss/sse-data/catalogs/observing



