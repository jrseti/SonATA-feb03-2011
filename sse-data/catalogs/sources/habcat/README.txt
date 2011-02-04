
Habcat catalog preparation:

1. APJ-HABCAT.txt.gz is the original published Habcat starlist.

2. Extracted hipparcos numbers:

awk -F'|' '{printf "HIP%s\n", $1}' < APJ-HABCAT.txt > APJ-HABCAT-ids-only.txt

3. The Habcat2 paper lists the addition of 4 stars to the
original list:

  GJ 752 A
  GJ 338 A
  GJ 338 B
  GJ 380

4. Submitted combined starlist (17133 stars total) in a query to Simbad
to get RA & DEC in the J2000 equinox and epoch, and to get other
star characteristics, such as spectral type.

Input script:  habcat-simbad-query-script.txt
Results: habcat-simbad-results.txt

5. Trimmed simbad results so header lines are removed.

6. Converted list to seeker TargetCat database table format,
filtering those targets visible at Hat Creek:

Note on arguments: habcat is output catalog name, first assigned target id is 100,000.

${HOME}/nss/sse-pkg/database/convertSimbadToTargetCat habcat 100000 habcat-simbad-results-trimmed-header.txt > seeker.habcat.load-data

7. Moved final file to nss/sse-data/catalogs/observing

