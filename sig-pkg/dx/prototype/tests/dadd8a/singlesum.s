; TopDownSingleSum: add a pair of rows to form a single row sum
        BITS 32
        global TopDownSingleSum
;
; TopDownSingleSum: add the maximum-drift rows from the upper and lower blocks
;
; Synopsis:
;		void TopDownSingleSum(drift, bins, lower, upper);
;		int drift;					???
;		int bins;					# of bins in a row
;		uint8_t *lower;				lower block row data
;		uint8_t *upper;				upper block row data
;		int shift;					# of bits to shift data
; Returns:
;		Nothing.
; Description:
;		Adds a pair of rows
; Notes:
;		This handles the eccentric case which arises when adding an
;		even block of rows to an odd block of rows in the non-power-of-2
;		DADD algorithm.  It sums the maximum drift from the larger
;		(upper) block with the maximum drift from the lower (smaller)
;		block.  This is the only case where paths of different total drift
;		are combined.
; len = (bins - drift - 1) / 8
; ofs = (bins - drift - 1) % 8
; For the assembler version, the following registers are used:
;		
;		eax: used during setup
;		ebx: address register for upper row reads
;		ecx: # of quadwords to read from lower row
;		edx: work register, then index counter for addressing
;		esi: address register for lower row reads
;		edi: address register for upper row writes
;
;		mm0: pathsum accumulator
;		mm1: normalization shift counter
;		mm2: normalization shift mask
;		mm3: lower(n)
;		mm4: upper(n)
;		mm5: upper(n+1) (0 for last add)
;		mm6: (drift - 1) % 8
;		mm7: 8 - (drift - 1) %8
TopDownSingleSum:
	enter 8,0
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
	; n = (bins - (drift - 1)) / 8
	; ofs = (bins - (drift - 1)) % 8
	mov ebx,[ebp+8]					; ebx = drift - 1
	dec ebx
	mov ecx,[ebp+12]				; ecx = bins
	sub ecx,ebx						; ecx = bins - drift - 1
	jle near TopDownSingleExit

	; set up registers
	mov edx,ebx						; get the drift
	and ebx,0xfffffff8
	mov [ebp-4],ebx					; save for later
	shr dword [ebp-4],3				; convert to quadword count
	shr ecx,3						; get n = (bins - drift - 1) / 8
	mov eax,8						; 
	and edx,7						; get ofs = drift % 8
	sub eax,edx						; get (8 - ofs)
	movd mm6,edx					; transfer shift counts to MMX registers
	movd mm7,eax
	psllq mm6,3						; convert to byteshifts
	psllq mm7,3

	; get pointers to data
	mov esi,[ebp+16]				; lower row read/store address
	mov edi,[ebp+20]				; upper row store address
	add ebx,edi						; compute upper row read address
	mov edx,0

	; preload registers
	movq mm5,[ebx+8*edx]			; load upper(n)
	pand mm5,mm2					; do shift normalization
	psrlq mm5,mm1
	add ebx,8						; then point to upper(n+1)
	jecxz TopDownSingleCleanup		; test nothing to do
	
TopDownSingleLoop:
	; main loop to perform a single add and store
	movq mm3,[esi+8*edx]			; load lower(n)
	pand mm3,mm2					; do normalization on lower(n)
	psrlq mm3,mm1

	movq mm4,mm5					; load upper(n)
	movq mm5,[ebx+8*edx]			; load upper(n+1)
	pand mm5,mm2					; do normalization on upper(n+1)
	psrlq mm5,mm1

	movq mm0,mm5					; upper(n+1) >> (8 - ofs)
	psllq mm0,mm7
	psrlq mm4,mm6					; upper(n) << ofs
	paddusb mm0,mm4					; upper(n) << ofs + upper(n+1) >> (8 - ofs)
	paddusb mm0,mm3					; lower(n) + upper(n) << ofs + upper(n+1) >> (8 - ofs)
	movq [edi+8*edx],mm0			; store lower path sum (drift 2k)

	inc edx
	loop TopDownSingleLoop,ecx

TopDownSingleCleanup:
	movq mm3,[esi+8*edx]			; load lower(n)
	pand mm3,mm2					; do normalization on lower(n)
	psrlq mm3,mm1

	movq mm0,mm5					; load upper(n) (already normalized)

    ; do last add, for which upper(n+1) = 0 since it lies outside the array
	psrlq mm0,mm6					; upper(n) << ofs
	paddusb mm0,mm3					; upper(n) << ofs + upper(n+1) >> (8 - ofs)
	movq [edi+8*edx],mm0			; store lower path sum (drift 2k)

	; done with adds, just copy remainder of lower row to empty bins in upper row
	mov ecx,[ebp-4]					; recover remaining bytes
	jecxz TopDownSingleExit			; may be done
TopDownSingleCopy:
	inc edx
	movq mm0,[esi+8*edx]			; copy a quadword to upper row
	pand mm0,mm2
	psrlq mm0,mm1
	movq [edi+8*edx],mm0
	loop TopDownSingleCopy,ecx
	
TopDownSingleExit:
	emms
	popa
	leave
	ret

	end	

