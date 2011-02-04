#!/usr/bin/perl
################################################################################
#
# File:    passthruStats.pl
# Project: OpenSonATA
# Authors: The OpenSonATA code is the result of many programmers
#          over many years
#
# Copyright 2011 The SETI Institute
#
# OpenSonATA is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OpenSonATA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
# 
# Implementers of this code are requested to include the caption
# "Licensed through SETI" with a link to setiQuest.org.
# 
# For alternate licensing arrangements, please contact
# The SETI Institute at www.seti.org or setiquest.org. 
#
################################################################################

#
# Script to read and process the output from PdmSpectrometryTask
#
# -Read log files from passthru mode and calcualte the:
#		Number of half frames
#		The total processing time
#		The total time inside half frame processing
#		The total time outside half frame processing
#		The number of '8th half frames'
# -Calcualte and print information based on these numbers
#
#
# HOW TO USE THIS SCRIPT:
#	Run the pdm in passthru mode and store the output in a file
#	Use the output from passthru mode as intput to this script
#	Input files to this script can be grouped together by associating 
#		them with the same value in the fHash table below.
#
# TODO: XXX
#	Review data structures especially the hash that contains the tagged data
#	Come up with a more efficient way of printing statistics. Too much
#		cut and past used.
#


# INPUT FILES: Modify this list to process a different set of files.
#
#  Key   = Output file name
#  Value = Tag to sort information on
#
%fHash = qw ( 
	../standalone/raw-1024-op-oFlag11		BW-1024 
	../standalone/raw-512-op-oFlag11		BW-512 );
	#raw-1024-op-oFlag3		BW-1024
	#raw-512-op-oFlag3		BW-512
	#raw-1024-op-oFlag27		BW-1024
	#raw-512-op-oFlag27		BW-512
	#);
#


# The line most recently read line from a file.
$line ="";
$d = 1000000;
#
# process: Left and Right Spectrometry
#
$p1 = "LSpectrometry";
$p2 = "RSpectrometry";

# Global counts for file averages
%nHF1 = 0;		# Number of Half Frames in a file
%nHF8 = 0;		# Number of 8th half frames in a file
%tTime1 = 0;	# total time (inside & out) per half frame in a file
%tTime1_8 = 0;	# total time (inside & out) per 8th hf in a file
%inTime1_8 = 0; # amount of time inside all 8th hfs in a file
%inHFTime1 = 0; # amount of time inside all hfs in a file


# Global counts for multi-file statistics
%nHFAll = 0; 
%nHF8_A = 0;
%tTimeA = 0; 
%tTimeA_8 = 0;
%inTimeA_8 = 0;
%inHFTimeA = 0;

# Global counts for tagged-file statistics
%TAG_nHFAll_p1;
%TAG_nHF8_A_p1;
%TAG_tTimeA_p1;
%TAG_tTimeA_8_p1;
%TAG_inTimeA_8_p1;
%TAG_inHFTimeA_p1;

%TAG_nHFAll_p2;
%TAG_nHF8_A_p2;
%TAG_tTimeA_p2;
%TAG_tTimeA_8_p2;
%TAG_inTimeA_8_p2;
%TAG_inHFTimeA_p2;

use Getopt::Std;
sub adjust {
	my ($i) = @_;
	return ($i/$d);
};
	
#
# Print tagged statistics.
#
sub printTaggedStats_p1 {
	my ($k) = @_;

	$timeOut = $TAG_tTimeA_p1{$k} - $TAG_inHFTimeA_p1{$k};
	$avg    = int($TAG_tTimeA_p1{$k}/$TAG_nHFAll_p1{$k});
	$avg8   = int($TAG_tTimeA_8_p1{$k}/$TAG_nHF8_A_p1{$k});
	$avgIn  = int($TAG_inHFTimeA_p1{$k}/$TAG_nHF8_A_p1{$k});
	$avgOut = int($timeOut/$TAG_nHF8_A_p1{$k});
	$avgIn8 =int($TAG_inTimeA_8_p1{$k}/$nHF8_A{$task});

	$avgP = adjust($avg); 
	$avg8P = adjust($avg8);
	$avgInP = adjust($avgIn);
	$avgIn8P = adjust($avgIn8);
	print OP "TAG: '$k' stats for $p1,$avgP,$avg8P,$avgInP,$avgIn8P\n";

	print "\nTAGGED statistics for $p1 key: $k  \n";
	print "\thalfFrames:   $TAG_nHFAll_p1{$k}\n";
	print "\tTotal Time:   $TAG_tTimeA_p1{$k} \n";
	print "\tInside time:   $TAG_inHFTimeA_p1{$k}\n"; 
	print "\tOutside time: $timeOut\n";

	print "\n\tAverage total time for all half frames:  $avg \n";
	print "\tAverage inside time per half frame:      $avgIn \n";
	print "\tAverage outside time per half frame:     $avgOut \n";

	print "\n\tNumber of 8th frame values:              $TAG_nHF8_A_p1{$k} \n";
	print "\tAverage total time for eighth frames:    $avg8\n";
	print "\tInside time all 8th frames:              $TAG_inTimeA_8_p1{$k}";
	print "\n\tTotal time all 8th frames:               $TAG_tTimeA_8_p1{$k}";
	print "\n";

	$inP = ($TAG_inHFTimeA_p1{$k}/$TAG_tTimeA_p1{$k}) * 100;
	$oP =  ($timeOut /$TAG_tTimeA_p1{$k}) * 100;

	printf ("\n\tPercent of time not processing frames         %.2f\n",$oP);
	printf ("\tPercent of total time processing half frames  %.2f\n",$inP);
	print "\n";
}

#
# Print tagged statistics.
#
sub printTaggedStats_p2 {
	my ($k) = @_;

	$timeOut = $TAG_tTimeA_p2{$k} - $TAG_inHFTimeA_p2{$k};
	$avg    = int($TAG_tTimeA_p2{$k}/$TAG_nHFAll_p2{$k});
	$avg8   = int($TAG_tTimeA_8_p2{$k}/$TAG_nHF8_A_p2{$k});
	$avgIn  = int($TAG_inHFTimeA_p2{$k}/$TAG_nHF8_A_p2{$k});
	$avgOut = int($timeOut/$TAG_nHF8_A_p2{$k});
	$avgIn8 = int($TAG_inTimeA_8_p2{$k}/$nHF8_A{$task});

	$avgP = adjust($avg); 
	$avg8P = adjust($avg8);
	$avgInP = adjust($avgIn);
	$avgIn8P = adjust($avgIn8);
	print OP "TAG: '$k' stats for $p2,$avgP,$avg8P,$avgInP,$avgIn8P\n";

	print "\nTAGGED statistics for $p2 key: $k  \n";
	print "\thalfFrames:   $TAG_nHFAll_p2{$k}\n";
	print "\tTotal Time:   $TAG_tTimeA_p2{$k} \n";
	print "\tInside time:   $TAG_inHFTimeA_p2{$k}\n"; 
	print "\tOutside time: $timeOut\n";

	print "\n\tAverage total time for all half frames:  $avg \n";
	print "\tAverage inside time per half frame:      $avgIn \n";
	print "\tAverage outside time per half frame:     $avgOut \n";

	print "\n\tNumber of 8th frame values:              $TAG_nHF8_A_p2{$k} \n";
	print "\tAverage total time for eighth frames:    $avg8\n";
	print "\tInside time all 8th frames:              $TAG_inTimeA_8_p2{$k}";
	print "\n\tTotal time all 8th frames:               $TAG_tTimeA_8_p2{$k}";
	print "\n";

	$inP = ($TAG_inHFTimeA_p2{$k}/$TAG_tTimeA_p2{$k}) * 100;
	$oP =  ($timeOut /$TAG_tTimeA_p2{$k}) * 100;

	printf ("\n\tPercent of time not processing frames         %.2f\n",$oP);
	printf ("\tPercent of total time processing half frames  %.2f\n",$inP);
	print "\n";

}
#
# Print statistics for ALL the files processed.
#
sub printAllFileStats {
	my ($task) = @_;

	$timeOut = $tTimeA{$task} - $inHFTimeA{$task};
	$avg = int($tTimeA{$task}/$nHFAll{$task});
	$avg8 =  int($tTimeA_8{$task}/$nHF8_A{$task});
	$avgIn  = int($inHFTimeA{$task}/$nHFAll{$task});
	$avgIn8 = int($inTimeA_8{$task}/$nHF8_A{$task});
	$avgOut = int($timeOut/$nHFAll{$task});

	$avgP = adjust($avg); 
	$avg8P = adjust($avg8);
	$avgInP = adjust($avgIn);
	$avgIn8P = adjust($avgIn8);
	
	print OP "AllFileStats-$task,$avgP,$avg8P,$avgInP,$avgIn8P\n";
	print "\n$task - stats: halfFrames: $nHFAll{$task}\n";
	$tmp = adjust($tTimeA{$task});
	print "\tTotal Time:   $tmp / $tTimeA{$task} \n";
	$tmp =  adjust($inHFTimeA{$task});
	print "\tInside time:   $tmp / $inHFTimeA{$task}\n"; 
	$tmp = adjust($timeOut);
	print "\tOutside time: $tmp / $timeOut\n\n";
	$tmp = adjust($avg);
	print "\tAverage total time for all half frames:   $tmp / $avg \n";
	$tmp = adjust($avgIn);
	print "\tAverage inside time per half frames:      $tmp / $avgIn \n";
	$tmp = adjust($avgOut);
	print "\tAverage outside time per half frames:     $tmp / $avgOut \n";

	print "\n\tNumber of 8th frame values:               $nHF8_A{$task} \n";
	$tmp = adjust($avg8);
	print "\tAverage total time for eighth frames:     $tmp / $avg8\n";
	$tmp = adjust($avgIn8);
	print "\tAverage inside time for eighth frames:    $tmp / $avgIn8\n";
	$tmp =  adjust($inTimeA_8{$task}); 
	print "\tInside time all 8th frames:               $tmp / $inTimeA_8{$task}";
	$tmp = adjust($tTimeA_8{$task}); 
	print "\n\tTotal time all 8th frames:                $tmp / $tTimeA_8{$task}";
	print "\n";

	$inP = ($inHFTimeA{$task}/$tTimeA{$task}) * 100;
	$oP = ($timeOut /$tTimeA{$task}) * 100;

	printf ("\n\tPercent of time not processing frames         %.2f\n",$oP);
	printf ("\tPercent of total time processing half frames  %.2f\n",$inP);
	print "\n";
}

#
# Print statistics one file at a time.
#
sub printFileStats {
	my ($task,$file) = @_;

	#
	# Print stats
	#
	$timeOut = $tTime1{$task} - $inHFTime1{$task};
	$avg    = int($tTime1{$task}/$nHF1{$task});
	$avg8   = int($tTime1_8{$task}/$nHF8{$task});
	$avgIn  = int($inHFTime1{$task}/$nHF1{$task});
	$avgIn8 = int($inTime1_8{$task}/$nHF8{$task});
	$avgOut = int($timeOut/$nHF8{$task});

	$avgP = adjust($avg); 
	$avg8P = adjust($avg8);
	$avgInP = adjust($avgIn);
	$avgIn8P = adjust($avgIn8);
	print OP "File: $file stats for $task,$avgP,$avg8P,$avgInP,$avgIn8P\n";

	print "\n$task - stats: halfFrames: $nHF1{$task}\n";
	print "\tTotal Time:   $tTime1{$task} \n";
	print "\tInside time:   $inHFTime1{$task}";
	print "\n\tOutside time: $timeOut\n\n";
	print "\tAverage total time for all half frames:   $avg \n";
	print "\tAverage inside time per half frame:       $avgIn\n";
	print "\tAverage outside time per half frame:     $avgOut\n";
	print "\n\tNumber of 8th frame values:                   $nHF8{$task} \n";
	print "\tAverage total time for 8th frames:        $avg8 \n";
	print "\tInside time all 8th frame:                $inTime1_8{$task}\n";
	print "\tTotal time all 8th frames:               $tTime1_8{$task}\n";

	$inP = ($inHFTime1{$task}/$tTime1{$task}) * 100;
	$oP = ($timeOut /$tTime1{$task}) * 100;
	printf ("\n\tPercent of time not processing frames         %.2f\n",$oP);
	printf ("\tPercent of time processing half frames         %.2f\n",$inP);
	print "\n";
}
#
# findNextMatch - if a return value is needed be sure to surround the
#                 value wanted with () so $1 will have a value.
sub findNextMatch {
    my ($lookFor, $fd) = @_;

    while ($line = <$fd>)  {
			chomp ($line);
            if ($line =~ /$lookFor/) {
			# XXX print "Found it: Returning '$1'\n";
                return $1
            }
    }
    return "NotFound";
}
#
# Find a specified value in the most current line read from a file
#
sub findValueInCurrLine {
    my ($val) = @_;
    if ($line =~ /($val)/) {
        return $1;
    }
    return "NotFound";
}
#
# split $line using : as a delimiter

# Use the split function to find the value following the
# specified target.
#
sub findValFor {
	my ($target) = @_;
	
	@fields = split (/:/,$line);   
	for ($i=0;  $i< $#fields; $i++) {
		if ( $fields[$i] eq $target )  {
			# XXX print "findValFor: Looking for: '$target' found '$fields[$i+1]'\n";
			return $fields[$i+1];
		}
	}
	return "NotFound";
}
#
# Fatal error routine
#
sub errorExit {
	my ($msg) = @_;
	print "$msg \n";
	exit (3);
}
#
# subroutine to get the total processing time per halfFrame.
# from the specified trace file.
#
sub getHFtime {
	my ($fid, $task) = @_;

	# XXXprint "getHFtime: task: $task fid: $fid\n";
	while (findNextMatch($task, $fid) ne "NotFound") {
		chomp($line);
		# XXXprint "getHFtime line: '$line'\n";
		if ( $line =~ /Enter doSpec/) {
			$outsideTime = findValFor("time outside doSpec");
			if ($outsideTime eq "NotFound") {
				$message = "time outside - NOT FOUND";
				errorExit($message);
			}
			while (($temp = findNextMatch("(Leave doSpec)", $fid)) 
					ne "NotFound") {
				$insideTime = findValFor("time inside doSpec "); 
				if ($insideTime eq "NotFound") {
					errorExit("time inside - NOT FOUND");
				}
				$frame = findValFor("hf");
				$totTime = $insideTime + $outsideTime;
				++$nHF1{$task};
				$tTime1{$task}  += $totTime;
				if (($frame % 8) == 0) {
					++$nHF8{$task};
					$tTime1_8{$task} += $totTime;
					$inTime1_8{$task} += $insideTime;
					$inHFTime1{$task} += $insideTime;

				if ($fileDetails eq "true") {print "---\n";};
				} else {
					$inHFTime1{$task} += $insideTime;
				}
				if ($fileDetails eq "true") 
				{ 
					$c = $totTime / $d;
					print "$task hf $frame Time for this hf: $totTime";
					$c = $tTime1{$task} / $d;
					print "\trunning total $c/$tTime1{$task}\n";
				}
				#return ($insideTime + $outsideTime);
				return ($totTime);
			}
			}
	}
	return "NotFound";
}

#
# main
#
$num_args = scalar(@ARGV);
$usage ="Usage: $0 [options] \n\n\toptions are:\n\t  -d show details for each file\n\t  -a show stats for all files\n\t  -t show tagged statistics \n\t  -f <filename> override default name of file that contains CSV output\n";

	$fileStats = "false";
	$fileDetails = "false";
	$taggedStats = "false";
	$statFile = "passThruStat.csv";
	$ret = getopts("datf:",\%opts);
	if ($ret ne "") {
		if (defined($opts{"t"})) { $taggedStats = "true"; };
		if (defined($opts{"a"})) { $fileStats = "true"; };
		if (defined($opts{"d"})) { $fileDetails = "true"; };
		if (defined($opts{"f"})) { $statFile = $opts{"f"}; };

	} else { die $usage; };
	$num_args = scalar(@ARGV);
	if ((scalar(@ARGV) != 0)) { die $usage; };

	open(OP, ">$statFile") ||
		 die "Error: cannot open file '$statFile' for output.";
	print OP "FILE NAME or TAG,AVG time/hf,AVG time/8th hf,AVG inside time/hf,AVG inside time/8th hf\n";

	$nHF1{$p1} = 0;
	$nHF1{$p2} = 0;
	$nHF8{$p1} = 0;
	$nHF8{$p2} = 0;
	$tTime1{$p1} = 0;
	$tTime1{$p2} = 0;
	$tTime1_8{$p1} = 0;
	$tTime1_8{$p2} = 0;
	$fc = 0;

	#
	# Process each file indivudually. First calcualte statistics 
	# for LSpectrometry than for RSpectrometry.
	#
	foreach $f (keys (%fHash)) {
		$logFile = $f;
		print "\nCalculating statistics from file: $logFile tag: $fHash{$f}\n";
		open (TRACE_FD, $logFile)
			|| die "$logFile not found. Can not calculate hf time.\n";
		$task = $p1; 

		$time = "";
		while (($time  ne "NotFound")) {
			$time = getHFtime(TRACE_FD, $task);
		}
		close (TRACE_FD);

		open (TRACE_FD, $logFile)
			|| die "$logFile not found. Can not calculate hf time.\n";
		$task = $p2;
		$time = "";
		while (($time  ne "NotFound")) {
			$time = getHFtime(TRACE_FD, $task);
		}

		#
		# Print file level stats
		#
		if ($fileStats eq "true") {
			printFileStats($p1,$logFile);
		};

		if ($fileStats eq "true") {
			printFileStats($p2,$logFile);
		}
		close (TRACE_FD);

		# Keep a running total individual file level counters
		$nHFAll{$p1} += $nHF1{$p1};
		$nHFAll{$p2} += $nHF1{$p2};
		$nHF8_A{$p1} += $nHF8{$p1};
		$nHF8_A{$p2} += $nHF8{$p2};

		$tTimeA{$p1} += $tTime1{$p1};
		$tTimeA{$p2} += $tTime1{$p2};
		$tTimeA_8{$p1} += $tTime1_8{$p1};
		$tTimeA_8{$p2} += $tTime1_8{$p2};

		$inHFTimeA{$p1}+= $inHFTime1{$p1};
		$inHFTimeA{$p2}+= $inHFTime1{$p2};
		$inTimeA_8{$p1}+= $inTime1_8{$p1};
		$inTimeA_8{$p2}+= $inTime1_8{$p2};
#
# XXX There has to be a beter way to do this! A hash of a hash?
#
		# Keep a running total of type level counters
		$TAG_nHFAll_p1{$fHash{$f}} 		+= $nHF1{$p1};
		$TAG_nHF8_A_p1{$fHash{$f}}  	+= $nHF8{$p1};
		$TAG_tTimeA_p1{$fHash{$f}} 		+= $tTime1{$p1}; 
		$TAG_tTimeA_8_p1{$fHash{$f}} 	+= $tTime1_8{$p1};
		$TAG_inTimeA_8_p1{$fHash{$f}} 	+= $inTime1_8{$p1};
		$TAG_inHFTimeA_p1{$fHash{$f}} 	+= $inHFTime1{$p1};

		# Keep a running total of type level counters
		$TAG_nHFAll_p2{$fHash{$f}} 		+= $nHF1{$p2};
		$TAG_nHF8_A_p2{$fHash{$f}}  	+= $nHF8{$p2};
		$TAG_tTimeA_p2{$fHash{$f}} 		+= $tTime1{$p2}; 
		$TAG_tTimeA_8_p2{$fHash{$f}} 	+= $tTime1_8{$p2};
		$TAG_inTimeA_8_p2{$fHash{$f}} 	+= $inTime1_8{$p2};
		$TAG_inHFTimeA_p2{$fHash{$f}} 	+= $inHFTime1{$p2};

	
		# Reset file level counters for next file	
		$nHF1{$p1} = 0;
		$nHF1{$p2} = 0;
		$nHF8{$p1} = 0;
		$nHF8{$p2} = 0;
		$tTime1{$p1} = 0;
		$tTime1{$p2}  = 0;
		$tTime1_8{$p1} = 0;
		$tTime1_8{$p2} = 0;
		$inTime1_8{$p1} = 0;
		$inTime1_8{$p2} = 0;
		$inHFTime1{$p1} = 0;
		$inHFTime1{$p2} = 0;
		$fc++;
	}

	#
	# Print tagged stats
	#

	if ($taggedStats eq "true") {
		foreach $k (keys (%TAG_tTimeA_p1)) {
			printTaggedStats_p1($k);
			printTaggedStats_p2($k);
		}
	}			

	#
	# Print multiple file level statistics
	#
	print ("\n---------- $fc files processed     ----------\n");
	print ("---------- Multiple file results ----------\n");
	printAllFileStats($p1);

	printAllFileStats($p2);
	close (OP);
