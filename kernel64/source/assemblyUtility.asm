[BITS 64]			; 64 bit code

SECTION .text			; Define text section

; Expose the name to use it in C
global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTR, kLoadIDTR

; Read 1 byte from port 
; 	PARAM: port number
kInPortByte:
	push rdx		; Store temporary register used in a funtion into stack
				; This will be reloaded at the end of the function
	
	mov rdx, rdi		; Store parameter 1 in RDX
	mov rax, 0		; Initiate RAX
	in al, dx		; Read one byte( port address is DX )
				; the byte is stored in AL. AL is used as a return
	
	pop rdx			; Roll back the temporary register
	ret	
	
; Write 1 byte at port
; 	PARAM: port number, data
kOutPortByte:
	push rdx		; Store temporary register used ina function into stack
	push rax
	
	mov rdx, rdi		; Store parameter 1 in RDX
	mov rax, rsi		; Stroe parameter 2 in RAX
	out dx, al		; Write one byte( port address is DX )
				; data is from in AL
	
	pop rax			; Roll back the temporary register
	pop rdx	
	ret 
	
; Set GDT table in GDTR register
;	PARAM: Address of structure to store GDT table
kLoadGDTR:
	lgdt [rdi]		; Load the parameter 1 and set the GDT table
	ret

; Set TSS segment register in TR register
;	PARAM: Offset of TSS segment register
kLoadTR:
	ltr di			; Set the parameter 1 and load TSS segment
	ret 
	
; Set IDT table in IDTR register
;	PARAM: Address of structure to store IDT table
kLoadIDTR:
	lidt [rdi]		; Load the parameter 1 and set the IDT table
	ret 
	