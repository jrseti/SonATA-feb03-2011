; Threshold: thresholds an array of pathsums
        BITS 32
        global Threshold
;
; Threshold: perform a threshold operation
;
; Synopsis:
;		void DaddShift(rows, bins, rowbins, data, threshold);
;		int rows;					# of rows to shift
;		uint32_t bins;				# of bins to shift in a row
;		uint32_t rowbins;			# total # of bins in a row
;		uint8_t *data;				# beginning of data
;		uint8_t threshold;			# threshold value
; Returns:
;		Nothing.
; Description:
;		Subtracts the threshold value from every bin in the
;		pathsum array.
; Notes:
;		The array is thresholded a row at a time.
;		Efficient implementation of this function in assembler relies
;		upon quad-word alignment of the data to be shifted.
;
Threshold:
	; set up stack frame and save general registers
	enter 8,0
	pusha

	; build the threshold register
	mov eax,[ebp+24]				; get threshold
	mov [ebp-8],al
	mov [ebp-7],al
	mov [ebp-6],al
	mov [ebp-5],al
	mov [ebp-4],al
	mov [ebp-3],al
	mov [ebp-2],al
	mov [ebp-1],al
	movq mm2,[ebp-8]

	mov eax,[ebp+8]					; get # of rows to threshold

	; compute # of quadwords in each row
	mov ecx,[ebp+12]				; get # of bins to threshold
	add ecx,7
	shr ecx,3
	mov ebx,ecx
	mov edx,[ebp+16]				; get total # of bins in a row

	mov edi,[ebp+20]				; get ptr to data

DaddShiftReset:
	mov esi,edi
DaddShiftLoop:
	movq mm0,[esi]					; get next 8 bins
	psubusw mm0,mm2
	movq [esi],mm4					; store result
	add esi,8
	loop DaddShiftLoop,ecx
	dec eax
	jle DaddShiftExit
	add edi,edx						; bump to next row
	mov ecx,ebx						; reload word count
	jmp DaddShiftLoop

DaddShiftExit:
	emms
	popa
	leave
	ret

	end
