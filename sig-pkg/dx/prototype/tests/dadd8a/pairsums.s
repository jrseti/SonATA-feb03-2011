; TopDownPairSum: add a pair of rows
        BITS 32
        global TopDownPairSum
;
; TopDownPairSum: add a pair of rows with normalization shift
;
; Synopsis:
;		void TopDownPairSum(drift, bins, lower, upper);
;		int drift;					# drift of this pair of rows
;		int bins;					# of bins in a row
;		uint8_t *lower;				lower block row data
;		uint8_t *upper;				upper block row data
;		int shift;					# of bits to shift data
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
;		mm1: normalization shift counter
;		mm2: normalization shift mask
;		mm3: lower(n)
;		mm4: upper(n)
;		mm5: upper(n+1) (0 for last add)
;		mm6: drift % 8
;		mm7: 7 - drift %8
; Note that normalization shifting occurs immediately upon loading of a
; quadword, so that it does not have to be done again.
TopDownPairSum:
	; setup stack frame and save general registers
	push ebp
	mov ebp,esp
	sub esp,8						; reserve some local storage
	pusha

	; compute the shift mask
	mov ecx,[ebp+24]
	movd mm1,ecx

	; build the bit mask used to ensure that adjacent bins don't interfere
	; with each other.  This requires masking low bits before the shift is
	; performed
	mov al,0xff
	shl al,cl
	mov [ebp-8],al
	mov [ebp-7],al
	mov [ebp-6],al
	mov [ebp-5],al
	mov [ebp-4],al
	mov [ebp-3],al
	mov [ebp-2],al
	mov [ebp-1],al
	movq mm2,[ebp-8]
	
	; compute length of transfer
	; n = (bins - drift) / 8
	; ofs = (bins - drift) % 8
	mov ebx,[ebp+8]					; ebx = drift
	mov ecx,[ebp+12]				; ecx = bins
	sub ecx,ebx
	jle near TopDownPairExit

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
	movd mm6,edx					; transfer shift counts to MMX registers
	movd mm7,eax
	inc eax							; get (8 - ofs)
	psllq mm6,3						; convert to byteshifts
	psllq mm7,3

	; get pointers to data
	mov esi,[ebp+16]				; lower row read/store address
	mov edi,[ebp+20]				; upper row store addres
	add ebx,edi						; compute upper row read address
	mov edx,0

	; preload registers
	movq mm5,[ebx]					; load upper(n)
	pand mm5,mm2					; do shift normalization
	psrlq mm5,mm1
	add ebx,8
	jecxz TopDownPairCleanup		; test nothing to do
	
TopDownPairLoop:
	; main loop to perform a pair of adds and stores
	movq mm3,[esi+8*edx]			; load lower(n)
	pand mm3,mm2					; do normalization on lower(n)
	psrlq mm3,mm1

	movq mm4,mm5					; load upper(n) (already normalized)
	movq mm5,[ebx+8*edx]			; load upper(n+1)
	pand mm5,mm2					; do normalization on upper(n+1)
	psrlq mm5,mm1

	movq mm0,mm5					; upper(n+1) >> (8 - ofs)
	psllq mm0,mm7
	psllq mm0,8
	psrlq mm4,mm6
	paddusb mm0,mm4					; upper(n) << ofs + upper(n+1) >> (8 - ofs)
	paddusb mm0,mm3					; lower(n) + upper(n) << ofs + upper(n+1) >> (8 - ofs)
	movq [esi+8*edx],mm0			; store lower path sum (drift 2k)

	movq mm0,mm5					; upper(n+1) >> (7 - ofs)
	psllq mm0,mm7
	psrlq mm4,8						; upper(n) << (ofs+1)
	paddusb mm0,mm4					; upper(n) << (ofs+1) + upper(n+1) >> (8 - ofs)
	paddusb mm0,mm3
	movq [edi+8*edx],mm0			; store upper path sum (drift 2k+1)

	inc edx
	loop TopDownPairLoop,ecx

TopDownPairCleanup:
	movq mm3,[esi+8*edx]			; load lower(n)
	pand mm3,mm2					; do normalization on lower(n)
	psrlq mm3,mm1

	movq mm4,mm5					; load upper(n) (already normalized)

    ; do last pair of adds, for which upper(n+1) = 0 since it lies outside array
	movq mm0,mm3
	psrlq mm4,mm6					; upper(n) << ofs
	paddusb mm0,mm4					; upper(n) << ofs + upper(n+1) >> (8 - ofs)
	movq [esi+8*edx],mm0			; store lower path sum (drift 2k)

	movq mm0,mm3
	psrlq mm4,8						; upper(n) << (ofs+1)
	paddusb mm0,mm4					; upper(n) << (ofs+1) + upper(n+1) >> (8 - ofs)
	movq [edi+8*edx],mm0			; store upper path sum (drift 2k+1)

	; done with adds, just copy remainder of lower row to empty bins in upper row
	mov ecx,[ebp-4]					; recover remaining bytes
	jecxz TopDownPairExit			; may be done
TopDownPairCopy:
	inc edx
	movq mm0,[esi+8*edx]			; copy a quadword to upper row
	pand mm0,mm2
	psrlq mm0,mm1
	movq [edi+8*edx],mm0
	loop TopDownPairCopy,ecx
	
TopDownPairExit:
	emms
	popa
	mov esp,ebp
	pop ebp
	ret

	end	
