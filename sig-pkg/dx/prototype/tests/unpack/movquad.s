	BITS 32
	global movquad
; movquad(src, dest, n);
; unsigned long long *src;
; unsigned long long *dest;
; unsigned int n;
movquad:
	push ebp					; set up the stack frame
	mov ebp,esp
	pushad						; save all registers
	mov esi,[ebp+8]
	mov edi,[ebp+12]
	mov ecx,[ebp+16]
;	shl ecx,1
	mov edx,8
movq1:
;	rep movsd
	movq mm0,[esi]
	movq [edi],mm0
	add esi,edx
	add edi,edx
;	movsd
;	movsd
;	lodsd
;	stosd
;	lodsd
;	stosd
	loop movq1,ecx
	emms
	popad
	pop ebp
	ret

	end
	
	
