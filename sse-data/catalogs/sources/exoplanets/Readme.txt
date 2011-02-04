
20 Oct 2009

From exoplanet.eu (Extrasolar Planets Encyclopaedia)
------------
exoplanetData-19oct2009.csv

1. Extract planet ids from exoplanet.eu list, eliminating quotes:
  
cat exoplanetData-19oct2009.csv | awk -F',' '{printf "%s\n", $1}' | tr -d '"' > exoplanet.eu.id.txt 

2. edit out duplicates (ie, multiple planets per system, where id suffix > b)

3. create simbad query, submit it.  Look for errors, edit names to fix them,
and resubmit.
    
      exoplanet.eu.simbad-query.txt

4. edit out all headers and trailing blank lines

   exoplanet.eu.simbad.results-trimmed-header.txt

5. convert results to target cat format:

Catalog=exoplanets
Starting target id=140500

sse-pkg/database/convertSimbadToTargetCat exoplanets 140500 exoplanet.eu.simbad.results-trimmed-header.txt > seeker.exoplanets.load-data


---------------------

7 Jan 2010

1. Download most recent copy of data - exoplanetData-7jan2010.csv.

2. Extract ids as above.

3. Diff against previous list to find new planets and retracted planets.

   Retracted:  
      HD 136118 b
      VB 10 b (aka  V* V1298 Aql)

4. Edit out duplicates (multiple planets per system)

5. Create simbad query just for new planet systems.

6. Edit out all headers and trailing blank lines.

7. Convert results to target cat format

   Catalog=exoplanets
   Starting target id=140850

8. Append results to original seeker.exoplanets.load-data file
   to make a single list.
      Set autoschedule to No for retracted planets.
      Edit aliases as needed.









