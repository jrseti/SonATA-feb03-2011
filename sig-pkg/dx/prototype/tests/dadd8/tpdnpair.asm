        BITS 32
;        segment _TEXT public use32 'CODE
        global topdowndaddpair
topdowndaddpair:
   ;    
   ;    void topdowndaddpair(short length,short drift,signed char *inrowlower,signed char *inrowupper)
   ;    
        push      ebp
        mov       ebp,esp
        add       esp,-16
        push      ebx
        push      esi
        push      edi
        mov       esi,dword [ebp+20]
        mov       edi,dword [ebp+16]
   ;    
   ;    
   ;        
   ;        {short i,lminusd,lmdm1,lmdm1ov8,lmdm1mod8;
   ;         signed char temp1;
   ;         double junk;  //alignment for temp
   ;         signed char temp[8];
   ;    
   ;         lminusd = length - drift;
   ;    
?live1@1792: ; ESI = inrowupper, EDI = inrowlower
@93:
        mov       ax,word [ebp+8]
        sub       ax,word [ebp+12]
        mov       word [ebp-2],ax
   ;    
   ;         lmdm1 = lminusd -1;
   ;    
        dec       ax
        mov       word [ebp-4],ax
        mov       bx,ax
   ;    
   ;         lmdm1ov8 = lmdm1/8;
        sar       ax,3
        mov       word [ebp-6],ax
        mov       cx,ax   ;       set loop counter
   ;    
   ;         lmdm1mod8 = lmdm1 % 8;
        and       bx,7 
   ;    
        mov       word [ebp-8],bx
   ;    
   ;        for(i=0;i<8*lmdm1ov8;i=i+8)
   ;    
   ;    &&&&&&&&&&&&&&&&&&&&&&&&&
        movsx     eax,word[ebp+12] ;pick up drift
        add       eax,esi          ;eax points to inrowupper + drift
        xor       ebx,ebx
        sub       cx,7
        jmp       cmp1
bigloop:
        movq      mm0,[edi+ebx*8]
        movq      mm2,[edi+ebx*8+8]
        movq      mm1,mm0
        movq      mm4,[edi+ebx*8+16] 
        movq      mm3,mm2
        movq      mm6,[edi+ebx*8+24]
        movq      mm5,mm4
        paddusb   mm0,[eax+ebx*8]
        movq      mm7,mm6
        paddusb   mm2,[eax+ebx*8+8]
        paddusb   mm4,[eax+ebx*8+16]
        paddusb   mm6,[eax+ebx*8+24]
        paddusb   mm1,[eax+ebx*8+1]
        paddusb   mm3,[eax+ebx*8+9]
        paddusb   mm5,[eax+ebx*8+17]
        paddusb   mm7,[eax+ebx*8+25]
        movq      [edi+ebx*8],mm0
        movq      [edi+ebx*8+8],mm2
        movq      [edi+ebx*8+16],mm4
        movq      [edi+ebx*8+24],mm6
        movq      [esi+ebx*8],mm1
        movq      [esi+ebx*8+8],mm3
        movq      [esi+ebx*8+16],mm5
        movq      [esi+ebx*8+24],mm7
        add       bx,4
cmp1:   cmp       bx,cx
        jl        bigloop
        add       cx,7
        jmp       cmp2
   ;    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
@96:    movq      mm0,[edi+ebx*8]  ; inrowlower[i]  -> mm0
        movq      mm1,mm0
        paddusb   mm0,[eax +ebx*8] 
        paddusb   mm1,[eax +ebx*8+1]
        movq      [edi+ebx*8],mm0
        movq      [esi+ebx*8],mm1
        inc       bx
cmp2:
        cmp       bx,cx
        jl        short @96
   ;    
   ;        if(lmdm1mod8 > 0)
   ;    
?live1@1920: ; ESI = inrowupper, EDI = inrowlower
        cmp       word [ebp-8],0
        jle       short @99
   ;    
   ;            for(i=8*lmdm1ov8;i<lmdm1;i++)
   ;    
        mov       bx,word [ebp-6]
        shl       ebx,3
        cmp       bx,word [ebp-4]
        jge       short @101
   ;    
   ;                {temp1 = inrowlower[i];
   ;    
?live1@1952: ; EBX = i, ESI = inrowupper, EDI = inrowlower
@100:
        movsx     eax,bx
        mov       al,byte [edi+eax]
   ;    
   ;                 inrowlower[i] = temp1 + inrowupper[i+drift];
   ;    
?live1@1968: ; EBX = i, ESI = inrowupper, EDI = inrowlower, EAX = temp1
        movsx     edx,word [ebp+12]
        movsx     ecx,bx
        lea       ecx,[esi+ecx]
        mov       dl,byte [ecx+edx]
        add       dl,al
        push      edx
        movsx     edx,bx
        pop       ecx
        mov       byte [edi+edx],cl
   ;    
   ;                 inrowupper[i] = temp1 + inrowupper[i+drift+1];
   ;    
        movsx     edx,word [ebp+12]
        movsx     ecx,bx
        lea       ecx,[esi+ecx]
        add       al,byte [ecx+edx+1]
        movsx     edx,bx
        mov       byte [esi+edx],al
        inc       ebx
        cmp       bx,word [ebp-4]
        jl        short @100
   ;    
   ;                 }
   ;        
   ;        temp1 = inrowlower[lminusd-1];
   ;    
?live1@2000: ; ESI = inrowupper, EDI = inrowlower
@101:
@99:
        movsx     eax,word [ebp-2]
        mov       al,byte [edi+eax-1]
   ;    
   ;        if(lminusd>0)
   ;    
?live1@2016: ; ESI = inrowupper, EDI = inrowlower, EAX = temp1
        cmp       word [ebp-2],0
        jle       short @103
   ;    
   ;        inrowlower[lminusd-1] = temp1 + inrowupper[lminusd-1+drift];
   ;    
        movsx     edx,word [ebp+12]
        movsx     ecx,word [ebp-2]
        lea       ecx,[esi+ecx]
        mov       dl,byte [ecx+edx-1]
        add       dl,al
        movsx     ecx,word [ebp-2]
        mov       byte [edi+ecx-1],dl
   ;    
   ;    
   ;         for(i=lminusd; i<length;i++)
   ;    
@103:
        mov       bx,word [ebp-2]
        cmp       bx,word [ebp+8]
        jge       short @105
   ;    
   ;             inrowupper[i] = inrowlower[i];
   ;    
?live1@2064: ; EBX = i, ESI = inrowupper, EDI = inrowlower, EAX = temp1
@104:
        movsx     edx,bx
        mov       cl,byte [edi+edx]
        movsx     edx,bx
        mov       byte [esi+edx],cl
        inc       ebx
        cmp       bx,word [ebp+8]
        jl        short @104
   ;    
   ;            
   ;        inrowupper[lminusd-1] = temp1;
   ;    
?live1@2080: ; ESI = inrowupper, EAX = temp1
@105:
        movsx     ecx,word [ebp-2]
        mov       byte [esi+ecx-1],al
   ;    
   ;        }
   ;    
?live1@2096: ; 
@107:   emms
        pop       edi
        pop       esi
        pop       ebx
        mov       esp,ebp
        pop       ebp
        ret 
        end
