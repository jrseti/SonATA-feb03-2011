/*******************************************************************************

 File:    tpdnbyt.cpp
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

// -*-C-*-
#include <stdio.h>
#include <sys\timeb.h>

#define NROWS 512
#define NCOLS 16384
#define NROWSD NROWS/8


extern "C" void topdowndaddpair(short length,short drift,signed char *inrowlower,
			 signed char *inrowupper);
//set up arrays as doubles to force alignment to 8 byte boundaries 

double test1[NROWSD][NCOLS],test2[NROWSD][NCOLS],test3[NROWSD][NCOLS];
signed char *test11 = (signed char *) &(test1[0][0]);
signed char *test22 = (signed char *) &(test2[0][0]);
signed char *test33 = (signed char *) &(test3[0][0]);


void printrow(short n,signed char *a)
    {short i,temp;
     for(i=0;i<n;i++)
	 {temp = a[i];
	  printf("%d ",temp);
	  }
      printf("\n");
      }
 /* the arguments for this routine are the same as those for
     the routine "splitpath".  this routine was used to
     debug daddinv.

  */   
void printrows(short length,short drift,signed char *br,
	       signed char *tr,signed char *twod,signed char *twodp1)
     {printf("dedrifting %ld and %ld to %ld\n",2*drift,
	2*drift + 1,drift);
       printrow(length,twodp1);
       printrow(length,twod);
       printrow(length,tr);
       printrow(length,br);
       }

short log2(short n)
    {short tmpq,count;
     tmpq = n;
     count = 0;
     while(tmpq > 1)
	{tmpq = tmpq/2;
	  count++;
	 }
      return count;
      }  


//returns the bit reversal of n, where n is assumed to have bitcount bits
short bitreverser(short bitcount,short n)
    {short i,rev;
     rev = 0;
     for(i=0;i<bitcount;i++)    
       {rev = rev << 1;
	rev = rev + (n % 2);
	n = n >> 1;
       }
     return rev;
     }

//
void unscramble(short rows,short cols,signed char *data)
    {short m,temp,i,j,ibitrev;
     m = log2(rows);
     for(i=0;i<rows;i++)
	 {ibitrev = bitreverser(m,i);
	  if(i < ibitrev)
	      for(j=0;j<cols;j++)
		  {temp = data[i*cols+j];
		   data[i*cols+j] = data[ibitrev*cols+j];
		   data[ibitrev*cols+j] = temp;
		   }
	    }
      }




/* copy right half of array rightgood to right half of array leftgood */

void mergehalves(short rowcount,short colcount,signed char *rightgood,signed char *leftgood)
{long i,j,index;
 for(i=0;i<rowcount;i++)
   {index = i*colcount;
    for(j=colcount/2;j<colcount;j++)
      leftgood[index+j] = rightgood[index+j];
  }
}


void comparrays(short rowcount,short colcount,signed char *ar1,signed char *ar2)
{long i,j,index,character;
 
 for(i=0;i<rowcount;i++)
   {index = i*colcount;
    for(j=0;j<colcount;j++)
      if(ar1[index+j] !=  ar2[index+j])
	 {printf("array compare failed at index pair (%ld , %ld)\n",i,j);
	   character = getchar();  
	 // return;
	}
  }
      printf("arrays compared perfectly!!\n");
       
}

 void printarray(short rowcount,short colcount,signed char *a)
	{long i,j,index,temp,character;
	  for(i=0;i<rowcount;i++)
	      {for(j=0;j<colcount;j++)
		  {index = colcount*i + j;
		   temp = a[index];
		   printf("%ld ",temp);
		  }
	   printf("\n");
	   }
	 character = getchar();
	}

/* inar and outar are two-dimensional arrays.
    This routine sets each row of outar to the reverse
    of the corresponding row of inar.  If inar and outar
    point to the same array the routine will work.
*/

void dreverse(short rowcount,short colcount,signed char *inar,signed char *
	      outar)
    {long i,j,indx;
     signed char temp;

     for(i=0;i<rowcount;i++)
	 {indx = colcount*i;
	  for(j=0;j<(colcount+1)/2;j++)
	      {temp = inar[indx + j];
	       outar[indx +j] = inar[indx+colcount-1-j];
	       outar[indx+colcount-1-j] = temp;
	       }
	   }
      }



short pwrof2(short n)
    {short temp,i;
     temp = 1;
      for(i=0;i<n;i++)
	  temp = 2*temp;
      return temp;
      }
     
short odd(short m)
{return (m % 2);
 }
void initdaddarr(short m,short rowlength,signed char *inar)
    {long arlength,i;
      arlength = rowlength*pwrof2(m);
      for(i=0;i<arlength;i++)
	  inar[i] = 0.0;
      }

void movearr(short m,short rowlength,signed char *inar,signed char *outar)
    {long arlength,i;
      arlength = rowlength*pwrof2(m);
      for(i=0;i<arlength;i++)
	  outar[i] = inar[i];
      }

/* add a pair of row vectors of drift k to build row vectors of
   drift 2 k and 2 k + 1.  the drift 2k vector replaces the first drift k
   vector, and the drift 2k+1 vector replaces the second drift k vector.

 */

/*
void topdowndaddpair(short length,short drift,signed char *inrowlower,
		     signed char *inrowupper)
    {short i,lminusd;
     signed char temp;
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
// simulate a packed 4 wide add

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
void topdowndadd(short m,short length,signed char *data)
    {
     long lowbase,highbase;
     short half,i,ibitrev;
     half = 1;
     if(m > 1)
	  {half = pwrof2(m-1);
	   topdowndadd(m-1,length,data);
	   topdowndadd(m-1,length,&data[half*length]);
	   }
     for(i=0;i<half;i++)
	 {ibitrev = bitreverser(m-1,i);
	  lowbase = ibitrev*length;
	  highbase = (ibitrev + half)*length;
	  topdowndaddpair(length,i,&data[lowbase],&data[highbase]);
	  }
     }



/*
  v2d is a vector of path sums of drift 2k.
  w2dp1 is a vector of path sums of drift 2k+1.
  ad and bd are vectors of drift k path sums, ad
  is earlier in time than bd.
*/

void splitpath(short nlength,short kdrift,signed char *ad,signed char *bd,
	       signed char *v2d,signed char *w2dp1)
    {short i;
     for(i=nlength-kdrift-1;i<nlength;i++)
	 ad[i] = w2dp1[i];
     for(i=nlength-kdrift-1;i>0;i--)
	 {bd[i+kdrift] = v2d[i] - ad[i];
	  ad[i-1] = w2dp1[i-1] -bd[i+kdrift];
	  }
     }


/*
  The input array , *in, contains 2^m length nlength
  vectors.  The first vector contains drift 0 pathlength
  2^m path sums.  The second vector contains drift 1 sums...
  the 2^m th vector contains drift (2^m - 1) path sums.

  The output is a pair of arrays.  Each array contains
  2^(m-1) vectors, representing the various drifts of
  pathlength 2^(m-1) sums.

  each vector is of length n.

  subinvdadd does the real work.  invdadd itself simply passes the parameters
  through to subinvdadd, and, if necessary, copies the solution array to the
  input array.
  */

void subinvdadd(short m,short n, signed char *in,signed char *topout,
		signed char *botout)
  {long i,height;
    signed char *x2d,*x2dp1,*toprow,*botrow;
    int charac;

 //   printf("startinversedadd\n");
 //   printarray(pwrof2(m),16,in);
    if(m == 0) return;

    height = pwrof2(m-1);

    initdaddarr(m-1,n,topout); //initialize arrays
    initdaddarr(m-1,n,botout);

    for(i=0;i<height;i++)
	{x2d = in +2*i*n;
	 x2dp1 = in +(2*i+1)*n;
	 botrow = botout + n*i;
	 toprow = topout + n*i;
   //      printrows(n,i,botrow,toprow,x2d,x2dp1);
	 splitpath(n,i,botrow,toprow,x2d,x2dp1);
    //     printrows(n,i,botrow,toprow,x2d,x2dp1);
    //     charac = getchar();
   //      printarray(pwrof2(m-1),16,botout);
   //      printarray(pwrof2(m-1),16,topout);
	 }

     subinvdadd(m-1,n,botout,in+(height*n)/2,in);
     subinvdadd(m-1,n,topout,in+ (3*height*n)/2,in+height*n);
     }

void invdadd(short m,short n, signed char *in,signed char *work)
{short rowcount = pwrof2(m);
  subinvdadd(m,n,in,work+(rowcount*n)/2,work);
 if(odd(m) == 1) movearr(m,n,work,in);
 } 

void timediff(struct timeb *starttime,struct timeb *endtime)
    {float tdiff;
     long stsec,etsec;
     float stmil,etmil;


     printf(" %ld  %ld  %d  %d  \n",starttime->time,endtime->time, 
			      starttime->millitm,endtime->millitm);
     stsec =  starttime->time;
     etsec =  endtime->time;
     stmil = (float) starttime->millitm;
     etmil =  (float) endtime->millitm;

     tdiff =((float) (etsec - stsec)) + (etmil - stmil)/1000.;
     
     printf("timediff is %f  seconds\n",tdiff);
     }


int main()
    {
     short i,j;
     int character;

     timeb starttime,endtime;
  
    printf("start after carriage return\n");
      character = getchar(); 
    
     for(i=0;i<NROWS;i++)
	  for(j=0;j<NCOLS;j++)
	     { test11[i*NCOLS +j] =  (i+j)%4;
	    //   test2[i][j] = 0.0;
	    //   if(i==4) test1[i][j] = 1.0;
	     }
    dreverse(NROWS,NCOLS,test11,test33);
//    printf("dreverse test \n");
//    printarray(NROWS,NCOLS,test33);
    ftime(&starttime);
    topdowndadd(log2(NROWS),NCOLS,&(test1[0][0]));
    ftime(&endtime);
    timediff(&starttime,&endtime);
    unscramble(NROWS,NCOLS,test11);
//  printarray(NROWS,NCOLS,test11);
//    printf("start inverse dadd \n");

//  initdaddarr(log2(NROWS),NCOLS,test11);
     
    invdadd(log2(NROWS),NCOLS,test11,test22);
//    printf("end inverse dadd \n");

//    printarray(NROWS,NCOLS,test11);

    topdowndadd(log2(NROWS),NCOLS,test33);
    unscramble(NROWS,NCOLS,test33);
    invdadd(log2(NROWS),NCOLS,test33,test22);
//    printf("end backward inverse dadd\n");
//    printarray(NROWS,NCOLS,test33);
    dreverse(NROWS,NCOLS,test33,test33);
//    printf("reversed version of same array\n");
//    printarray(NROWS,NCOLS,test33);
    mergehalves(NROWS,NCOLS,test11,test33);
//    printf("merged array\n");
//    printarray(NROWS,NCOLS,test33);
     for(i=0;i<NROWS;i++)
	  for(j=0;j<NCOLS;j++)
	     { test11[i*NCOLS+j] =  (i+j)%4;
	     }
   comparrays(NROWS,NCOLS,test11,test33); 
   }




























