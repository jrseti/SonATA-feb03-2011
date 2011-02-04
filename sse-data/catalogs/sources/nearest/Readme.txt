
Nearest star systems list obtained from
RECONS (Research Consortium on Nearby Stars)

http://www.chara.gsu.edu/RECONS/TOP100.posted.htm

Conversion steps:

Original recons data file is "recons-nearest-orig.txt".

1. Trimmed off headers, removed "duplicate" star entries (stars very close together).
Note: kept in the stars that were formerly on the "top 100" list, because some
will be lost later due to non-visibility at Hat Creek.

Result: recons-nearest-data-only-no-dups.txt


2. Two targets had blank chars where the Dec seconds value should have been,
so '00' was added in each case:

2MA 0415-0935
2MA 0937+2931

3. Extracted fields of interest and converted them to comma separated values:

./convert-recons-to-csv recons-nearest-data-only-no-dups.txt > recons-nearest.csv

4. Converted targets to TargetCat format, eliminating those not visible at Hat Creek:

Notes:
- seeker catalog name = 'nearest'
- first assigned target id = 130000

${HOME}/nss/sse-pkg/database/convertReconsToTargetCat \
 nearest 130000 recons-nearest.csv > seeker.nearest.load-data


5. Habcat2 paper includes Teegarden's star,
aliases: 2MASS J02530084+1652532, SO025300.5+165258

See: Discovery of a New Nearby Star
http://arxiv.org/abs/astro-ph/0302206

This target is not in the RECONS list.

Use a simbad query to pick up the values, and then
convertSimbadToTargetCat to convert it:

${HOME}/nss/sse-pkg/database/convertSimbadToTargetCat \
nearest 131000 teegarden-simbad-query-results-noheader.txt > \
teegardens-star.load-data


Edit resulting file:
  a. remove header
  b. add parallax from Teegarden's paper:

Parallax = 0.43 arcsec, or 430 mas


6. Concat to the end of seeker.nearest.load-data:

cat teegardens-star.load-data >>  seeker.nearest.load-data

