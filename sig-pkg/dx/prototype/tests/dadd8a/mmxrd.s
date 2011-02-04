; mmxread: read data
        BITS 32
        global mmxrd
;
; mmxrd: reads the data using MMX instructions
;
; Synopsis:
;		void mmxrd(uint8_t *src, uint8_t dest, uint32_t len);
;		uint8_t *src;				# beginning of source data
;		uint8_t *dest;				# beginning of dest data
;		uint32_t len;				# # of bytes to move
; Returns:
;		Nothing.
; Description:
;		Moves the src data to the destination data 8 bytes at a time.
; Notes:
;		Simple version to simply test speed; assumes transfers are a
;		multiple of 8 bytes.
;		Efficient implementation of this function in assembler relies
;		upon quad-word alignment of the data to be shifted.
;		src and dest can be the same.
;
mmxrd:
	; set up stack frame and save general registers
	enter 0,0
	pusha

	mov ecx,[ebp+16]				; get # of bytes to move
	shr ecx,3						; convert to 8-byte count

	mov esi,[ebp+8]					; source pointer
	mov edi,[ebp+12]				; dest pointer

mmxloop:
	movq mm0,[esi]
;	movq [edi],mm0
	add esi,8
;	add edi,8
	loop mmxloop

	emms
	popa
	leave
	ret

	end
