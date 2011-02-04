	BITS 32
	global unpack
; unpack(packed, unpacked, rows, cols, xlat);
; unsigned short packed[rows][cols];
; unsigned long long unpacked[rows][cols];
; int rows;
; int cols;
; unsigned long long *xlat;
unpack:
	push ebp					; set up the stack frame
	mov ebp,esp
	pushad						; save all registers
	pushf						; save flags
	cld							; set forward direction
	mov edx,[ebp+8]				; get ptr to packed array
	mov edi,[ebp+12]			; get ptr to unpacked array
	mov ebx,[ebp+24]			; get ptr to translation table
	shr dword [ebp+20],3		; getting 8 bins at a time
	sub eax,eax					; clear index reg
unpack1:
	mov ecx,[ebp+20]			; get number of columns (bins)
unpack2:    
	movzx eax,word [edx]		; get a word (2 bytes) of 8 bins
;	movq mm0,[esi+edi*8]		; load the translated (8 bytes) value
;	movq [ebx],mm0				; store the unpacked data
;	push dword [esi+edi*8]		; load the translated (8 bytes) value
;	pop dword [ebx]				; store the unpacked data
	lea esi,[ebx+eax*8]			; 
	lodsd						; load the translated value (8 bytes)
	stosd
	lodsd						; load the translated value (8 bytes)
	stosd
	add edx,2
	loop unpack2,ecx			; test done with row
	dec dword [ebp+16]			; see if we've done all the rows
	jg unpack1
;	emms						; release MMX registers
	popf
	popad
	pop ebp
	ret

	end
	
	
