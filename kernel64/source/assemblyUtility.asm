[BITS 64]			; 64 bit code

SECTION .text			; Define text section

; Expose the name to use it in C
global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTR, kLoadIDTR
global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS
global kReadTSC
global kSwitchContext

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
	
	
; Enable Interrupt 
; 	PARAM: Nope
kEnableInterrupt:
	sti			; Enable interrupt 
	ret 
	
; Disable Interrupt
; 	PARAM: Nope
kDisableInterrupt:
	cli			; Disable interrupt 
	ret 
	
; Return read value of RFLAGS register
; 	PARAM: Nope
kReadRFLAGS:
	pushfq			; Store RFLAGS in stack
	pop rax			; Store RFLGAS register value to RAX register
	ret 
	
; Return time stamp counter
; 	PARAM: Nope
kReadTSC:
	push rdx		; Store RDX register in stack
	
	rdtsc			; Read time stamp counter and store it in RDX:RAX
	
	shl rdx, 32		; Read upper 32 bit of RDX register and lower 32 bit of RAX register
	or rax, rdx		; and OR them, then store it in RAX register
	
	pop rdx
	ret 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Assembly functions for task
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Save current context and change selector
%macro KSAVECONTEXT 0		; KSAVECONTEXT without parameter
	; Store all registers from RBP register to GS segment selector
	push rbp
	push rax
	push rbx
	push rcx
	push rdx
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r11
	push r12
	push r13
	push r14
	push r15
	
	mov ax, ds		; DS selector and ES segment selector do not be stored in stack directly,
	push rax		; so, we move those via RAX register
	mov ax, es		
	push rax
	push fs
	push gs
%endmacro			; macro end 

; Restore context
%macro KLOADCONTEXT 0		; KLOADCONTEXT without paremter
	; Restore all registers
	pop gs
	pop fs
	pop rax
	mov es, ax
	pop rax
	mov ds, ax
	
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rdx
	pop rcx
	pop rbx
	pop rax
	pop rbp
%endmacro			; macro end 

; Save current context and restore context of next task
;	PARAM: Current Context, Next Context
kSwitchContext:
	push rbp		; Store RBP register in stack, and RSP register in RBP register
	mov rbp, rsp
	
	; If current context is null, do not store context
	pushfq				; Store RFLAGS register if below cmp change RFLAGS
	cmp rdi, 0			; If current context is null, move the restore context direclty
	je .LoadContext
	popfq				; Restore RFLAGS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Store current context
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	push rax			; RAX is used as an offset, and store RAX in stack

	; Order is SS, RSP, RFLAGS, CS, RIP
	mov ax, ss			; Store SS register
	mov qword[rdi+(23*8)], rax

	mov rax, rbp		; Store RSP which is in RBP
	add rax, 16			; RSP register is stored as removing "Push RBP" and "Return Address"
	mov qword[rdi+(22*8)], rax
	
	pushfq				; Store RFLAGS
	pop rax
	mov qword[rdi+(21*8)], rax
	
	mov ax, cs			; Store CS
	mov qword[rdi+(20*8)], rax

	mov rax, qword[rbp+8]
						; Set RIP register as "Return Address"
	mov qword[rdi+(19*8)], rax
						; so after context restoring, move to previous point when this function is called.
	
	; Restoring all registers and store ohter context
	pop rax
	pop rbp

	; Change stack to store context
	add rdi, (19*8)
	mov rsp, rdi
	sub rdi, (19*8)

	; Store other registers in the context structure
	KSAVECONTEXT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Restore context for next task
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.LoadContext:
	mov rsp, rsi

	; Restore all register form context structure
	KLOADCONTEXT
	iretq

