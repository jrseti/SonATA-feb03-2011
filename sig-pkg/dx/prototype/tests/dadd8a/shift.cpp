/*******************************************************************************

 File:    shift.cpp
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

void daddshift(short shiftcount,long qwordcount,double *a)
    {short smallmask,character;
      long i,j,mediummask,qwc8;
      long *temp0,*temp1;
      double largemask; 
      
      largemask = 0;
      largemask = largemask + largemask; 
      smallmask = (0xff) >> shiftcount;
      mediummask = smallmask + 256*smallmask +(256*256)*smallmask + (256*256*256)*smallmask;
      qwc8 = 8*(qwordcount/8);
            for(i=0;i<qwc8;i=i+8)  //set up unrolled loop to do 8 qwords at a time.
            for(j=0;j<8;j++)
                  {temp0 = (long *) &a[i+j];
                   temp1 = temp0 + 1;
                    *temp0 = (*temp0 >> shiftcount) & mediummask;
                    *temp1 = (*temp1 >> shiftcount) & mediummask;
           //         printf(" %p  %p  %x  %x \n",temp0,temp1,*temp0,*temp1); 
           //         character = getchar();

                   }
        for(i=qwc8; i<qwordcount;i++)
              {temp0 = (long *) &a[i]  ;
                temp1 = temp0 + 1;
                    *temp0 = (*temp0 >> shiftcount) & mediummask;
                    *temp1 = (*temp1 >> shiftcount) & mediummask;
              }
     }

