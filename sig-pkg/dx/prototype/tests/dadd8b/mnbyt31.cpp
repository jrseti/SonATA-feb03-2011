/*******************************************************************************

 File:    mnbyt31.cpp
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
#include <sys\timeb.h>
#include "gendadd8.h"
#include "daddsub8.cpp"
#define NROWS 24
#define NCOLS 50 
#define NROWSD NROWS/8   //"+1" in case nrows not divisible by 8  
extern "C" void topdowndaddpair(short length,short drift,signed char *inrowlower,
			       signed char *inrowupper);
extern "C" void topdownsinglesum(short length,short height,
			signed char *lower,signed char *upper);
extern "C" void daddshift(short shiftcount,long qwordcount,double *a);
				 

double test1[NROWSD][NCOLS],test2[NROWSD][NCOLS],test3[NROWSD][NCOLS];
signed char *test11 = (signed char *) &(test1[0][0]);
signed char *test22 = (signed char *) &(test2[0][0]); 
signed char *test33 = (signed char *) &(test3[0][0]);

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

void topdowngeneraldadd(short m,short length,signed char *data)
    {
     long lowbase,highbase;
     short i,ibitrev1,ibitrev2,mover2,mstar;

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
 /*
void topdowndaddpair(short length,short drift,signed char *inrowlower,
		     signed char *inrowupper)
    {short i,lminusd,temp;
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
 */
/*
void mov8(signed char *dest,signed char *src)
    {dest[0] = src[0];
     dest[1] = src[1];
     dest[2] = src[2];
     dest[3] = src[3];
     dest[4] = src[4];
     dest[5] = src[5];
     dest[6] = src[6];
     dest[7] = src[7];

     }
// simulate 8 simultaneous bytewide adds

void addeight(signed char *in1,signed char *in2,signed char *out)
   {short i;
    out[0] = in1[0] + in2[0];
    out[1] = in1[1] + in2[1];
    out[2] = in1[2] + in2[2];
    out[3] = in1[3] + in2[3];
    out[4] = in1[4] + in2[4];
    out[5] = in1[5] + in2[5];
    out[6] = in1[6] + in2[6];
    out[7] = in1[7] + in2[7];

    }
*/
/*
void topdowndaddpair(short length,short drift,signed char *inrowlower,signed char *inrowupper)

    
    {short i,lminusd,lmdm1,lmdm1ov8,lmdm1mod8;
     signed char temp1;
     double junk;  //alignment for temp
     signed char temp[8];

     lminusd = length - drift;
     lmdm1 = lminusd -1;
     lmdm1ov8 = lmdm1/8;
     lmdm1mod8 = lmdm1 % 8;
    for(i=0;i<8*lmdm1ov8;i=i+8)
       {mov8(temp,&inrowlower[i]);
	addeight(temp,&inrowupper[i+drift],&inrowlower[i]);
	addeight(temp,&inrowupper[i+drift+1],&inrowupper[i]);
	}
    if(lmdm1mod8 > 0)
	for(i=8*lmdm1ov8;i<lmdm1;i++)
	    {temp1 = inrowlower[i];
	     inrowlower[i] = temp1 + inrowupper[i+drift];
	     inrowupper[i] = temp1 + inrowupper[i+drift+1];
	     }
    
    temp1 = inrowlower[lminusd-1];
    if(lminusd>0)
    inrowlower[lminusd-1] = temp1 + inrowupper[lminusd-1+drift];

     for(i=lminusd; i<length;i++)
	 inrowupper[i] = inrowlower[i];
	
    inrowupper[lminusd-1] = temp1;
    }

*/

/* this handles the eccentric case which arises when adding an even block
   of rows to an odd block of rows in the non-power-of-2 DADD algorithm.
   It sums the maximum drift from the larger (upper) block with the maximum
   drift from the lower (smaller) block.  This is the only case where paths of
   different total drift are combined.

*/
/*
void topdownsinglesum(short length,short height,signed char *lower,signed char *upper)
    {long i;
     short smallheight;
     smallheight = height -1; 
     for(i=0;i<length-smallheight;i++)
	 upper[i] = lower[i]+upper[i+smallheight];
     for(i=length-smallheight;i<length;i++)     
	 upper[i] = lower[i];
     }
*/
/*  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    THE FOLLOWING THREE SUBROUTINES CONSTITUTE THE GENERAL INVERSE DADD
    ALGORITHM. THE INVERSE ALGORITHM IS PASSED AN ARRAY OF PATH SUMS, FROM 
    WHICH IT COMPUTES THE ARRAY OF POWERS WHICH GAVE RISE TO THE PATH SUMS.
    
    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
*/ 

/*   FIX THIS FOR GENERAL DADD
  The input array , *data, contains 2^m length nlength
  vectors.  The first vector contains drift 0 pathlength
  2^m path sums.  The second vector contains drift 1 sums...
  the 2^m th vector contains drift (2^m - 1) path sums.

  After processing, each half of the array contains
  2^(m-1) vectors, representing the various drifts of
  pathlength 2^(m-1) sums. 

  The routine then calls itself twice to further process each 
  half.  Recursion continues until the inversion is complete.

  length is the dimension of each vector
  rowcount is the number of vectors, and must be a power of 2.
  */

void geninvdadd(short rowcount,short length,signed char *data)
    {short i,lowercount,uppercount;
     signed char *x2d,*x2dp1;
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

void splitpath(short length,short drift,signed char *lvec,signed char *uvec)
    {long i;
     short temp;

     temp = uvec[length - drift -1];
     
     for(i=length-drift-1; i > -1;i--)
	 {uvec[i+drift] = lvec[i] - temp;
	  lvec[i] = temp;
	  temp = uvec[i-1] - uvec[i+drift];
	  }
     }



void topsolve(long length,short smallerblock,signed char *lvec,signed char *uvec)
    {long i;
     for(i=length-smallerblock+1; i>=0;i--)
	 uvec[i+smallerblock] = uvec[i]-lvec[i];
     }



int main()
    {
     short i,j;
     int character;
     timeb starttime,endtime;

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
	     {  test11[i*NCOLS + j] = 2;// ((i %8) == 0);  
	    //   test2[i][j] = 0.0;
	    //   if(i==4) test1[i][j] = 1.0;
	     }
    dreverse(NROWS,NCOLS,test11,test33);
//    printf("dreverse test \n");
// printarray(NROWS,NCOLS,test11);
    ftime(&starttime);
    topdowngeneraldadd((short) NROWS,(short) NCOLS,test11);
    ftime(&endtime);
    timediff(&starttime,&endtime);
//  generalunscramble(NROWS,NCOLS,test11,test22);
 //printarray(NROWS,NCOLS,test11);
   printf("start inverse dadd \n");
   geninvdadd((short) NROWS,(short) NCOLS,test11);

//  printarray(NROWS,NCOLS,test11);

//  initdaddarr(log2(NROWS),NCOLS,test11);
     
//    invdadd(NROWS,NCOLS,test11);
    printf("end inverse dadd \n");

//   printarray(NROWS,NCOLS,test11);
     printf("start second forward dadd \n");
    topdowngeneraldadd((short) NROWS,(short) NCOLS,test33);
//    unscramble(NROWS,NCOLS,test33);
    geninvdadd((short) NROWS,(short) NCOLS,test33);
    printf("end backward inverse dadd\n");
//    printarray(NROWS,NCOLS,test33);
    dreverse(NROWS,NCOLS,test33,test33);
//    printf("reversed version of same array\n");
//    printarray(NROWS,NCOLS,test33);
    mergehalves(NROWS,NCOLS,test11,test33);
//  printf("merged array\n");
//  printarray(NROWS,NCOLS,test33);
    daddshift(1,(long)NROWSD*NCOLS,test33);
 // printarray(NROWS,NCOLS,test33);
 
     for(i=0;i<NROWS;i++)
	  for(j=0;j<NCOLS;j++)
	     { test11[i*NCOLS + j] =1;// ((i %8) == 0); 
	     }
   comparrays(NROWS,NCOLS,test11,test33); 
   }




























