	BITS 32
	global unpackb
; unpack(packed, unpacked, rows, cols, xlat);
; unsigned short packed[rows][cols];
; unsigned long long unpacked[rows][cols];
; int rows;
; int cols;
; unsigned long long *xlat;
unpackb:
	push ebp					; set up the stack frame
	mov ebp,esp
	pushad						; save all registers
	pushf						; save flags
	cld							; set forward direction
	mov ebx,[ebp+8]				; get ptr to packed array
	mov edi,[ebp+12]			; get ptr to unpacked array
	shr dword [ebp+20],2		; getting 8 bins at a time
	sub edx,edx					; clear index reg
unpack1:
	mov ecx,[ebp+20]			; get number of columns (bins)
unpack2:    
	mov word dx,[ebx]			; get a word (2 bytes) of 8 bins
;	movq mm0,[esi+edi*8]		; load the translated (8 bytes) value
;	movq [ebx],mm0				; store the unpacked data
;	push dword [esi+edi*8]		; load the translated (8 bytes) value
;	pop dword [ebx]				; store the unpacked data
	mov esi,[ebp+24]			; get ptr to translation table
	lea esi,[esi+edx*8]			; 
	lodsd						; load the translated value (8 bytes)
	stosd
	inc ebx						; bump ptr to the next value
	inc ebx
	loop unpack2,ecx			; test done with row
	dec dword [ebp+16]			; see if we've done all the rows
	jg unpack1
;	emms						; release MMX registers
	popf
	popad
	pop ebp
	ret

	end
	
	
