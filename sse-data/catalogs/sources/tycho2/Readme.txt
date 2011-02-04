
Tycho2 catalog fetched from 
ftp://adc.astro.umd.edu/pub/adc/archives/catalogs/1/1259/

Steps for creating SETI observing catalogs:

1. Convert tycho catalog to a format compatible with seeker.schema.TargetCat
table, and filter by minimum visible Dec at Hat Creek:

% ${HOME}/nss/sse-pkg/database/convertTychoToTargetCat tycho2_catalog.dat > tycho2.load-data

Note: targets are set to TargetCat.catalog = 'tycho2remainder'.


2. Mark those targets that belong to the 'tycho2 subset' by
changing their catalog entry to "tycho2subset":


Using mysql and the Habcat2 "tycho subset" target id list, identify all the
Tycho2 targets that match:

----------------------------------------

-- create a 'seeker' database

mysqladmin -h <host> create <dbname>
mysql -h <host> <dbname> < nss/sse-pkg/database/seeker.schema
mysql -h <host> <dbname>

-- load tycho2 data

mysql> load data local infile 'tycho2.load-data' into table TargetCat fields terminated by ',' lines terminated by '\n' ignore 2 lines; show warnings;

-- load tycho2 subset list of tycho target ids

mysql> drop table if exists TychoSubset;

mysql> create table TychoSubset
(
   id tinytext not null                     
);

mysql> load data local infile 'seti-tycho-subset-ids.txt' into table TychoSubset fields terminated by ',' lines terminated by '\n' ignore 2 lines; show warnings;

-- add indexes for speed

mysql> alter table TychoSubset add index id (id(15));
mysql> alter table TargetCat add index aliases (aliases(15));


-- change catalog identifier for those targets on the subset list:

mysql> update TargetCat, TychoSubset set TargetCat.catalog = 'tycho2subset' where TargetCat.aliases = TychoSubset.id;

-- extract the 2 lists into separate data files:

mysql> select * into outfile '/tmp/tycho2subset.load-data'
fields terminated by ',' lines terminated by '\n' from TargetCat where catalog = 'tycho2subset';

mysql> select * into outfile '/tmp/tycho2remainder.load-data'
fields terminated by ',' lines terminated by '\n' from TargetCat where catalog = 'tycho2remainder';

3. The files will be created on the mysql server.  Copy them
to nss/sse-data/catalogs

4. Add 2 line headers to the tycho2subset.load-data and tycho2remainder.load-data files:

# $Id: Readme.txt,v 1.1 2007/12/21 00:49:49 xxx Exp $
# <target cat column headings>

