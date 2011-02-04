	BITS 32
	global unpack
; unpack(packed, unpacked, rows, bins, slicebins, daddbins, xlat);
; unsigned short packed[rows][slicewidth];
; unsigned long long unpacked[rows][daddwidth];
; int rows;
; int bins;
; int slicebins
; int daddbins;
; unsigned long long *xlat;
unpack:
	push ebp					; set up the stack frame
	mov ebp,esp
	pushad						; save all registers
	shr dword [ebp+24],2		; convert bins to bytes (4 bins/byte)
	mov ebx,[ebp+32]			; get ptr to translation table
	shr dword [ebp+20],3		; getting 8 bins at a time
unpack1:
	mov edx,[ebp+8]				; get ptr to packed array
	mov edi,[ebp+12]			; get ptr to unpacked array
	mov ecx,[ebp+20]			; get number of columns (bins)
unpack2:
	movzx eax,word [edx]		; get a word (2 bytes) of 8 bins
	movq mm0,[ebx+eax*8]		; load the translated (8 bytes) value
	inc edx
	inc edx
	movq [edi],mm0				; store the unpacked data
	add edi,8					; bump unpacked ptr to the next value
	loop unpack2,ecx			; test done with row

; done with a row; now skip to next row of both source and destination
	mov eax,[ebp+24]
	add [ebp+8],eax				; skip to beginning of next source row
	mov eax,[ebp+28]
	add [ebp+12],eax			; skip to beginning of next dest row
	dec dword [ebp+16]			; see if we've done all the rows
	jg unpack1
	emms						; release MMX registers
	popad
	pop ebp
	ret

	end
	
	
