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
	mov esi,[ebp+8]				; get ptr to packed array
	mov edi,[ebp+12]			; get ptr to unpacked array
	shr dword [ebp+20],2		; getting 8 bins at a time
	sub eax,eax					; clear load register
	sub edx,edx					; clear work register
unpack1:
	mov ecx,[ebp+20]			; get number of columns (bins)
unpack2:
	lodsb						; get next byte to unpack
	mov edx,eax					; convert low 2 bits to byte
	and edx,0x3					; mask off garbage
	shl eax,6					; position next 2 bits
	and eax,0x3f00				; mask away low 2 bits
	or edx,eax					; combine with low byte
	and edx,0x303				; mask off garbage
	shl eax,6					; position next 2 bits
	and eax,0xf0000				; mask away low 2 bits
	or edx,eax					; combine with low 2 bytes
	and edx,0x30303				; mask off garbage
	shl eax,6					; position last 2 bits
	and eax,0x3000000			; mask away low 2 bits
	or eax,edx					; combine with low 3 bytes
	stosd						; store the result
	loop unpack2,ecx			; test done with row
	dec dword [ebp+16]			; see if we've done all the rows
	jg unpack1
	popf
	popad
	pop ebp
	ret

	end
	
	
