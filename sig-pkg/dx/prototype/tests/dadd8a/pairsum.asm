; TopDownPairSum: add a pair of rows
        BITS 32
        global TopDownPairSum
;
; TopDownPairSum: add a pair of rows
;
; Synopsis:
;		void TopDownPairSum(drift, bins, lower, upper);
;		int drift;					# drift of this pair of rows
;		int bins;					# of bins in a row
;		uint8_t *lower;				lower block row data
;		uint8_t *upper;				upper block row data
; Returns:
;		Nothing.
; Description:
;		Adds a pair of rows of drift k from the upper and lower blocks.
;		The drift 2k sum is stored in the lower block, while the drift
;		2k+1 sum is stored in the lower block, shifted by one bin.
; Notes:
;		The adds are actually performed 8 bins at a time.
;
; len = (bins - drift - 1) / 8
; ofs = (bins - drift - 1) % 8
; For the assembler version, the following registers are used:
;		
;		eax: used during setup
;		ebx: address register for upper row reads
;		ecx: # of quadwords to read from lower row
;		edx: used during setup
;		esi: address register for lower row reads
;		edi: address register for upper row writes
;
;		mm0: pathsum accumulator
;		mm1: work register
;		mm2: 8 - drift % 8
;		mm3: lower(n)
;		mm4: upper(n)
;		mm5: upper(n+1) (0 for last add)
;		mm6: drift % 8
;		mm7: 7 - drift %8
TopDownPairSum:
	; setup stack frame and save general registers
	push ebp
	mov ebp,esp
	sub esp,4						; reserve some local storage
	pusha

	; compute length of transfer
	; n = (bins - drift) / 8
	; ofs = (bins - drift) % 8
	mov ebx,[ebp+8]					; ebx = drift
	mov ecx,[ebp+12]				; ecx = bins
	sub ecx,ebx
	jle near TopDownPairSumExit

	; set up registers
	mov edx,ebx						; get the drift
	and ebx,0xfffffff8
	mov [ebp-4],ebx					; save for later
	shr dword [ebp-4],3				; convert to quadword count
	sub ecx,1
	shr ecx,3						; get n = (bins - drift - 1) / 8
	mov eax,7						; 
	and edx,eax						; get ofs = drift % 8
	sub eax,edx						; get (7 - ofs)
	shl edx,3						; convert ofs shift to bits
	shl eax,3						; convert (7 - ofs) shift to bits
	movd mm6,edx					; transfer shift counts to MMX registers
	movd mm7,eax
	add eax,8						; get (8 - ofs)
	movd mm2,eax

	; get pointers to data
	mov esi,[ebp+16]				; lower row read/store address
	mov edi,[ebp+20]				; upper row store addres
	add ebx,edi						; compute upper row read address
	mov edx,0

	; preload registers
	movq mm3,[esi]					; load lower(n)
	movq mm4,[ebx]					; load upper(n)
	add ebx,8
	movq mm5,[ebx]					; load upper(n+1)

TopDownPairLoop:
	; main loop to perform a pair of adds and stores
	movq mm0,mm4					; upper(n) << ofs
	psrlq mm0,mm6
	movq mm1,mm5					; upper(n+1) >> (8 - ofs)
	psllq mm1,mm2
	paddusb mm0,mm1					; upper(n) << ofs + upper(n+1) >> (8 - ofs)
	paddusb mm0,mm3					; lower(n) + upper(n) << ofs + upper(n+1) >> (8 - ofs)
	movq [esi+8*edx],mm0			; store lower path sum (drift 2k)

	movq mm0,mm4					; upper(n) << (ofs+1)
	psrlq mm0,mm6
	psrlq mm0,8
	movq mm1,mm5					; upper(n+1) >> (8 - ofs)
	psllq mm1,mm7
	paddusb mm0,mm1					; upper(n) << (ofs+1) + upper(n+1) >> (8 - ofs)
	paddusb mm0,mm3
	movq [edi+8*edx],mm0			; store upper path sum (drift 2k+1)

	; prepare for next iteration
	inc edx
	movq mm4,mm5					; upper(n) <= upper(n+1)
	movq mm3,[esi+8*edx]			; load lower(n+1)
	movq mm5,[ebx+8*edx]			; load upper(n+2)
	dec ecx
	cmp ecx,0						; test for last iteration
	jg TopDownPairLoop				; normal if more to go
	jl TopDownPairSumCleanup		; all done with sums; just copy rest
	psubusb mm5,mm5					; clear upper(n+1) for last time through
	jmp TopDownPairLoop

TopDownPairSumCleanup:
	; done with adds, just copy lower row to empty bins in upper row
	mov ecx,[ebp-4]					; recover remaining bytes
TopDownPairSumCopy:
	jecxz TopDownPairSumExit		; may be done
	movq mm0,[esi+8*edx]			; copy a quadword to upper row
	movq [edi+8*edx],mm0
	dec ecx
	inc edx
	jmp TopDownPairSumCopy
	
TopDownPairSumExit:
	emms
	popa
	mov esp,ebp
	pop ebp
	ret

	end	
