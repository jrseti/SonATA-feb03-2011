	.386p
	model flat
	ifndef	??version
	?debug	macro
	endm
	endif
	?debug	S "shift.cpp"
	?debug	T "shift.cpp"
_TEXT	segment dword public use32 'CODE'
_TEXT	ends
_DATA	segment dword public use32 'DATA'
_DATA	ends
_BSS	segment dword public use32 'BSS'
_BSS	ends
DGROUP	group	_BSS,_DATA
_TEXT	segment dword public use32 'CODE'
@daddshift$qslpd	proc	near
?live1@0:
   ;	
   ;	void daddshift(short shiftcount,long qwordcount,double *a)
   ;	
	push      ebp
	mov       ebp,esp
	add       esp,-20
	push      ebx
	push      esi
	push      edi
	mov       esi,dword ptr [ebp+8]
   ;	
   ;	    {short smallmask,character;
   ;	      long i,j,mediummask,qwc8;
   ;	      long *temp0,*temp1;
   ;	      double largemask; 
   ;	      
   ;	      largemask = 0;
   ;	
?live1@16: ; ESI = shiftcount
@1:
	xor       eax,eax
	mov       dword ptr [ebp-20],eax
	mov       dword ptr [ebp-16],eax
   ;	
   ;	      largemask = largemask + largemask; 
   ;	
	fld       qword ptr [ebp-20]
	fadd      qword ptr [ebp-20]
	fstp      st(0)
   ;	
   ;	      smallmask = (0xff) >> shiftcount;
   ;	
	mov       ecx,esi
	mov       eax,255
	sar       eax,cl
   ;	
   ;	      mediummask = smallmask + 256*smallmask +(256*256)*smallmask + (256*256*256)*smallmask;
   ;	
?live1@64: ; ESI = shiftcount, EAX = smallmask
	movsx     edx,ax
	movsx     ecx,ax
	shl       ecx,8
	add       edx,ecx
	movsx     ecx,ax
	shl       ecx,16
	add       edx,ecx
	movsx     eax,ax
	shl       eax,24
	add       edx,eax
	mov       dword ptr [ebp-8],edx
   ;	
   ;	      qwc8 = 8*(qwordcount/8);
   ;	
?live1@80: ; ESI = shiftcount
	mov       edx,dword ptr [ebp+12]
	test      edx,edx
	jns       short @2
	add       edx,7
@2:
	sar       edx,3
	shl       edx,3
	mov       dword ptr [ebp-12],edx
   ;	
   ;	            for(i=0;i<qwc8;i=i+8)  //set up unrolled loop to do 8 qwords at a time.
   ;	
	xor       eax,eax
	mov       dword ptr [ebp-4],eax
	mov       edx,dword ptr [ebp-4]
	cmp       edx,dword ptr [ebp-12]
	jge       short @4
   ;	
   ;	            for(j=0;j<8;j++)
   ;	
@3:
	xor       ebx,ebx
   ;	
   ;	                  {temp0 = (long *) &a[i+j];
   ;	
?live1@128: ; EBX = j, ESI = shiftcount
@5:
	mov       eax,dword ptr [ebp-4]
	add       eax,ebx
	shl       eax,3
	add       eax,dword ptr [ebp+16]
   ;	
   ;	                   temp1 = temp0 + 1;
   ;	
?live1@144: ; EAX = temp0, EBX = j, ESI = shiftcount
	lea       edx,dword ptr [eax+4]
   ;	
   ;	                    *temp0 = (*temp0 >> shiftcount) & mediummask;
   ;	
?live1@160: ; EAX = temp0, EDX = temp1, EBX = j, ESI = shiftcount
	mov       ecx,esi
	mov       edi,dword ptr [eax]
	sar       edi,cl
	and       edi,dword ptr [ebp-8]
	mov       dword ptr [eax],edi
   ;	
   ;	                    *temp1 = (*temp1 >> shiftcount) & mediummask;
   ;	
?live1@176: ; EDX = temp1, EBX = j, ESI = shiftcount
	mov       ecx,esi
	mov       eax,dword ptr [edx]
	sar       eax,cl
	and       eax,dword ptr [ebp-8]
	mov       dword ptr [edx],eax
	inc       ebx
	cmp       ebx,8
	jl        short @5
	add       dword ptr [ebp-4],8
	mov       edx,dword ptr [ebp-4]
	cmp       edx,dword ptr [ebp-12]
	jl        short @3
   ;	
   ;	           //         printf(" %p  %p  %x  %x \n",temp0,temp1,*temp0,*temp1); 
   ;	           //         character = getchar();
   ;	
   ;	                   }
   ;	        for(i=qwc8; i<qwordcount;i++)
   ;	
?live1@192: ; ESI = shiftcount
@4:
	mov       eax,dword ptr [ebp-12]
	mov       dword ptr [ebp-4],eax
	mov       edx,dword ptr [ebp-4]
	cmp       edx,dword ptr [ebp+12]
	jge       short @10
   ;	
   ;	              {temp0 = (long *) &a[i]  ;
   ;	
@9:
	mov       eax,dword ptr [ebp-4]
	shl       eax,3
	add       eax,dword ptr [ebp+16]
   ;	
   ;	                temp1 = temp0 + 1;
   ;	
?live1@224: ; EAX = temp0, ESI = shiftcount
	lea       edx,dword ptr [eax+4]
   ;	
   ;	                    *temp0 = (*temp0 >> shiftcount) & mediummask;
   ;	
?live1@240: ; EAX = temp0, EDX = temp1, ESI = shiftcount
	mov       ecx,esi
	mov       ebx,dword ptr [eax]
	sar       ebx,cl
	and       ebx,dword ptr [ebp-8]
	mov       dword ptr [eax],ebx
   ;	
   ;	                    *temp1 = (*temp1 >> shiftcount) & mediummask;
   ;	
?live1@256: ; EDX = temp1, ESI = shiftcount
	mov       ecx,esi
	mov       eax,dword ptr [edx]
	sar       eax,cl
	and       eax,dword ptr [ebp-8]
	mov       dword ptr [edx],eax
	inc       dword ptr [ebp-4]
	mov       edx,dword ptr [ebp-4]
	cmp       edx,dword ptr [ebp+12]
	jl        short @9
   ;	
   ;	              }
   ;	     }
   ;	
?live1@272: ; 
@10:
@12:
	pop       edi
	pop       esi
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret 
@daddshift$qslpd	endp
_TEXT	ends
	public	@daddshift$qslpd
	?debug	D "shift.cpp" 9556 40800
	end
