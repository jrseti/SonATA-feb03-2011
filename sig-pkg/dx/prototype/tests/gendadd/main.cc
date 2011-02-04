/*******************************************************************************

 File:    main.cc
 Project: OpenSonATA
 Authors: The OpenSonATA code is the result of many programmers
          over many years

 Copyright 2011 The SETI Institute

 OpenSonATA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 OpenSonATA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
 
 Implementers of this code are requested to include the caption
 "Licensed through SETI" with a link to setiQuest.org.
 
 For alternate licensing arrangements, please contact
 The SETI Institute at www.seti.org or setiquest.org. 

*******************************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include "daddsubs.cc"
#include "gendadd.h"
#define NROWS 1024
#define NCOLS 32768


unsigned short test1[NROWS][NCOLS],test2[NROWS][NCOLS],test3[NROWS][NCOLS];
unsigned short *test11 = &(test1[0][0]);
unsigned short *test22 = &(test2[0][0]); 
unsigned short *test33 = &(test3[0][0]);

/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
    THE FOLLOWING THREE SUBROUTINES CONSTITUTE THE GENERAL DADD ALGORITHM. THE
    ALGORITHM IS GENERAL IN THE SENSE THAT PATH LENGTHS ARE NOT RESTRICTED TO
    BE A POWER OF TWO.
   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ 
*/ 
    

/* This version of dadd is not restricted to power_of_two length blocks.
   It splits the incoming data block into equal length subblocks if m is
   even, or nearly equal subblocks if m is odd, runs dadd on the subblocks,
   then sums the resulting vectors. 
   The argument m is the actual row count of the data block, not the base 2
   log of the row count.

   Note that when subblocks are unequal, the upper subblock is always taken
   as the larger.

*/

void topdowngeneraldadd(unsigned short m,unsigned short length,unsigned short *data)
    {
     long lowbase,highbase;
     unsigned short i,ibitrev1,ibitrev2,mover2,mstar;

     if(m == 1) return;
     else 
	  {mover2 = m/2;
	   mstar = m - mover2; //if blocks are unequal, upper one is bigger

                             //run dadd on the two subblocks
	   topdowngeneraldadd(mover2,length,data);
	   topdowngeneraldadd(mstar,length,&data[mover2*length]);
	   }
                          //now combine the paths from the subblocks

       if(mstar > mover2) //if the subblocks are unequal,do leftover case first
	 {lowbase = (mover2-1)*length;
	  highbase = (mover2+mover2)*length;
	  topdownsinglesum(length,mstar,&data[lowbase],&data[highbase]);
	  }

     for(i=0;i<mover2;i++) //add the matched pairs of drifts from subblocks
	 {ibitrev1 = getposition(i,mover2);  //index of drift i row in lower blk
	  ibitrev2 = getposition(i,mstar);   // ditto for upper block
	  lowbase = ibitrev1*length;  //set pointer to drift i vector in lower blk 
	  highbase = (ibitrev2 + mover2)*length;  // ditto for upper block
	  topdowndaddpair(length,i,&data[lowbase],&data[highbase]); 
	  }

     }

/* add a pair of row vectors of drift k to build row vectors of
   drift 2 k and 2 k + 1.  the drift 2k vector replaces the first drift k
   vector, and the drift 2k+1 vector replaces the second drift k vector.

 */
void topdowndaddpair(unsigned short length,unsigned short drift,unsigned short *inrowlower,
		     unsigned short *inrowupper)
    {unsigned short i,lminusd,temp;
     lminusd = length - drift;

    
	    
     for(i=0;i<lminusd-1; i++)
	{temp = inrowlower[i];
	 inrowlower[i] = temp + inrowupper[i+drift];
	 inrowupper[i] = temp + inrowupper[i+drift+1];
	}
    temp = inrowlower[lminusd - 1];

    if(lminusd>0)
      inrowlower[lminusd - 1] = temp + inrowupper[lminusd-1+drift];

    for(i=lminusd; i<length;i++)
	inrowupper[i] = inrowlower[i];
	
    inrowupper[lminusd-1] = temp;
    }   



/* this handles the eccentric case which arises when adding an even block
   of rows to an odd block of rows in the non-power-of-2 DADD algorithm.
   It sums the maximum drift from the larger (upper) block with the maximum
   drift from the lower (smaller) block.  This is the only case where paths of
   different total drift are combined.

*/

void topdownsinglesum(unsigned short length,unsigned short height,unsigned short *lower,unsigned short *upper)
    {long i;
     unsigned short smallheight;
     smallheight = height -1; 
     for(i=0;i<length-smallheight;i++)
	 upper[i] = lower[i]+upper[i+smallheight];
     for(i=length-smallheight;i<length;i++)     
	 upper[i] = lower[i];
     }

/*  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    THE FOLLOWING THREE SUBROUTINES CONSTITUTE THE GENERAL INVERSE DADD
    ALGORITHM. THE INVERSE ALGORITHM IS PASSED AN ARRAY OF PATH SUMS, FROM 
    WHICH IT COMPUTES THE ARRAY OF POWERS WHICH GAVE RISE TO THE PATH SUMS.
    
    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
*/ 

/*  

This routine inverts the DADD algorithm.  Given an array containing
    a row of drift 0 path sums
    a row of drift 1 path sums
    . 
    .
    a row of drift (rowcount -1) path sums

with the rows in the order in which they are left by in-place DADD, 
inverse DADD reconstructs as much as possible of the array of power
values fro which the path sums were created.

If the original power array is at least twice as wide as it is high, i.e.,
if each row has at least twice as many entries as the total number of rows,
then by inverting both the array of positive drift path sums and the array 
of negative drift path sums, the original array can be completely 
recovered.

The inverse algorithm first reconstructs the pair of blocks from which
the final path sums were formed, then recursively applies itself to that
pair of blocks.

  */

void geninvdadd(unsigned short rowcount,unsigned short length,unsigned short *data)
    {unsigned short i,lowercount,uppercount;
     unsigned short *x2d,*x2dp1;
     if(rowcount == 1) return;
    
     lowercount = rowcount/2;
     uppercount = rowcount - lowercount;

     for(i=0;i<lowercount;i++) //set pointers to rows of drift 2 i and 2 i + 1
         {x2d = data + length*getposition(i,lowercount);
          x2dp1 = data +length*(lowercount + getposition(i,uppercount));

          splitpath(length,i,x2d,x2dp1); //solve for the drift i rows in 
                                         // the lower and upper blocks
         }

     if(lowercount != uppercount)  //if blocks are different sizes, solve for
                                   //top row of upper block  
	 {x2dp1 = data + (rowcount-1)*length;
        topsolve(length,lowercount,x2d,x2dp1);
       }

     geninvdadd(lowercount,length,data);  //invert lower block sums
     geninvdadd(uppercount,length,data+length*lowercount);//invert upper block sums

   }    

 /* lvec points to a row of pathsums of drift 2k.
   uvec points to a row of pathsums of drift 2k+1.
   drift = 2k.
   length = length of row.
   This routine solves the system of equations which created the
   two rows from a pair of drift k rows, and replaces lvec and uvec
   with the two drift k rows. 
*/

void splitpath(unsigned short length,unsigned short drift,unsigned short *lvec,unsigned short *uvec)
    {long i;
     unsigned short temp;

     temp = uvec[length - drift -1];
     
     for(i=length-drift-1; i > -1;i--)
	 {uvec[i+drift] = lvec[i] - temp;
	  lvec[i] = temp;
	  temp = uvec[i-1] - uvec[i+drift];
	  }
     }

/*   this routine finds the maximum drift row in the larger of a pair
     of unequal blocks
*/

void topsolve(long length,unsigned short smallerblock,unsigned short *lvec,unsigned short *uvec)
    {long i;
     for(i=length-smallerblock+1; i>=0;i--)
         uvec[i+smallerblock] = uvec[i]-lvec[i];
     }



int main()
    {
     unsigned short i,j;
     int character;
 /*    for(i=0;i<15;i++)
	printf(" %d  %d  \n",i,getposition(i,15));
    printf("start after carriage return\n");

 */     
    // character = getchar(); 
    i= NROWS;
    j=NCOLS;
    printf(" NROWS =  %d  NCOLS =  %d  \n",i,j);

        //build test array
     for(i=0;i<NROWS;i++)
	  for(j=0;j<NCOLS;j++)
	     { test1[i][j] = (i+j) %8;
	    //   test2[i][j] = 0.0;
	    //   if(i==4) test1[i][j] = 1.0;
	     }
    dreverse(NROWS,NCOLS,test11,test33);
//    printf("dreverse test \n");
// printarray(NROWS,NCOLS,test11);
	struct timeval start, end;
	gettimeofday(&start, NULL);
    topdowngeneraldadd(NROWS,NCOLS,&(test1[0][0]));
    gettimeofday(&end, NULL);

   	double fstart, fend;
   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("dt = %.3lf\n", fend - fstart);
     //   generalunscramble(NROWS,NCOLS,test11,test22);

 //printarray(NROWS,NCOLS,test11);
   printf("start inverse dadd \n");
   gettimeofday(&start, NULL);
   geninvdadd(NROWS,NCOLS,test11);
   gettimeofday(&end, NULL);

   	fstart = (double) start.tv_sec + (double) start.tv_usec / 1e6;
   	fend = (double) end.tv_sec + (double) end.tv_usec / 1e6;
	printf("dt = %.3lf\n", fend - fstart);
   
 //printarray(NROWS,NCOLS,test11);

//  initdaddarr(log2(NROWS),NCOLS,test11);
     
//    invdadd(NROWS,NCOLS,test11);
//    printf("end inverse dadd \n");

//   printarray(NROWS,NCOLS,test11);

    topdowngeneraldadd(NROWS,NCOLS,test33);
//    unscramble(NROWS,NCOLS,test33);
    geninvdadd(NROWS,NCOLS,test33);
//    printf("end backward inverse dadd\n");
//    printarray(NROWS,NCOLS,test33);
    dreverse(NROWS,NCOLS,test33,test33);
//    printf("reversed version of same array\n");
//    printarray(NROWS,NCOLS,test33);
    mergehalves(NROWS,NCOLS,test11,test33);
//  printf("merged array\n");
  //printarray(NROWS,NCOLS,test33);
     for(i=0;i<NROWS;i++)
	  for(j=0;j<NCOLS;j++)
	     { test1[i][j] = (i+j)%8;
	     }
   comparrays(NROWS,NCOLS,test11,test33); 
   }




























