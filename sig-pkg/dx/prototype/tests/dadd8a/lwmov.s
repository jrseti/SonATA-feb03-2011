; lwmov: move data using longword moves
        BITS 32
        global lwmov
;
; lwmov: move a block of data
;
; Synopsis:
;		void lwmov(uint8_t *src, uint8_t dest, uint32_t len);
;		uint8_t *src;				# beginning of source data
;		uint8_t *dest;				# beginning of dest data
;		uint32_t len;				# # of bytes to move
; Returns:
;		Nothing.
; Description:
;		Moves the src data to the destination data 4 bytes at a time.
; Notes:
;		Simple version to simply test speed; assumes transfers are a
;		multiple of 8 bytes.
;		Efficient implementation of this function in assembler relies
;		upon quad-word alignment of the data to be shifted.
;		src and dest can be the same.
;
lwmov:
	; set up stack frame and save general registers
	enter 0,0
	pusha
	pushf
	cld

	mov ecx,[ebp+16]				; get # of bytes to move
	shr ecx,2						; convert to 4-byte count

	mov esi,[ebp+8]					; source pointer
	mov edi,[ebp+12]				; dest pointer

	rep movsd

	popf
	popa
	leave
	ret

	end
