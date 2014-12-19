[BITS 16]				; Below codes are 16 bit

SECTION .text				; Define text section

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Code Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
	mov ax, 0x1000			; 0x10000 is entry point of Protected mode
	mov ds, ax				; Set DS as 0x1000
	mov es, ax				; Set ES as 0x1000

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Enable A20 Gate
; If fail, try with system control port
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Enable A20 Gate with BIOS system
	mov ax, 0x2401			; Enable A20 Gate
	int 0x15				; BIOS Interrupt call

	jc .A20GATEERROR			; Check A20 Gate enabled or disabled
	jmp .A20GATESUCCESS
 
.A20GATEERROR:
	; If fail, try with system control port
	in al, 0x92				; Read system control port 1 byte, and store it at AL
	or al, 0x02				; AL | 0x02
	and al, 0xFE			; Prevent reset
	out 0x92, al			; restore AL to system control port

.A20GATESUCCESS:
	cli						; Disable interrupt
	lgdt [ GDTR ] 			; Load GDT table by setting GDTR data structure
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Enter Protected Mode
;	Disable paging, cache, internal FPU, align check
;	Enable protectedMode
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov eax, 0x4000003B		; PG=0, CD=1, NW=0, AM=0, WP-=0, ET=1, TS=1, EN=0, MP=1, PE=1
	mov cr0, eax			; Set CR0

	; Replace standard of kernel code segment as 0x00
	; and change EIP as 0x00
	; CS segment selector : EIP
	jmp dword 0x18: ( PROTECTEDMODE - $$ + 0x10000 )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Enter Protected Mode
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
[BITS 32]
PROTECTEDMODE:
	mov ax, 0x20			; Data segment descriptor for protected mode is 0x10
	mov ds, ax				; Set DS
	mov es, ax				; Set ES
	mov fs, ax				; Set FS
	mov gs, ax				; Set GS

	; Stack is 64KB, 0x00000000 ~ 0x0000FFFF
	mov ss, ax				; Set SS
	mov esp, 0xFFFE			; Set address of ESP as 0xFFFE
	mov ebp, 0xFFFE			; Set address of EBP as 0xFFFE

	; Print that current mode is protected mode
	push ( SWITCHSUCCESSMESSAGE - $$ + 0x10000)	
							; Insert message to stack
	push 2					; Y axis point
	push 0					; X axis point
	call PRINTMESSAGE		; Call PRINTMESSAGE function
	add esp, 12				; Remove params

	jmp dword 0x18: 0x10200	; 0x10200 is address for C kernel

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Function Code Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Function for message print
; PARAM: x, t, message
PRINTMESSAGE:
	push ebp				; Insert base point (BP) to stack
	mov ebp, esp			; Set EBP as ESP
	push esi		
	push edi
	push eax
	push ecx
	push edx

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Calculate the video address form X, Y
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Get line address from Y
	mov eax, dword [ ebp + 12 ]	
							; Set EAX with parameter 2 (4*(1+2))
	mov esi, 160			; Set SI with the numver of byte
							; as one line
	mul esi					; Y address = AX * SI
	mov edi, eax			; EDI = EAX

; Get line address from X
	mov eax, dword [ ebp + 8 ]	
							; Set EAX with parameter 1 (4*(1+1))
	mov esi, 2				; Set SI with the numver of byte
							; as one word
	mul esi					; X address = AX * SI
	mov edi, eax			; EDI = EAX

	; Address of print message	
	mov esi, dword [ ebp + 16 ]	
							; parameter 3 (4*(1+3))
	
.MESSAGELOOP:               ; Print message
    mov cl, byte [ esi ]    ; Copy the value of esi address to CL register.
                            ; CL register is low 1 byte of CX register

    cmp cl, 0               ; if charater is 0, it means end.
    je .MESSAGEEND          ; if cl is 0, jump to .MESSAGEEND

    mov byte [ edi + 0xB8000 ], cl   
				; if cl is not 0, print value of edi + 0xB8000 address

    add esi, 1              ; Move to the next character
    add edi, 2              ; Move to the next address
	
	jmp .MESSAGELOOP		; Move to the nuext iteration

.MESSAGEEND:
    pop edx                 ; Pop ES ~ DX from stack
    pop ecx
    pop eax
    pop edi
    pop esi
    pop ebp                 ; Restore BP
    ret                     ; Return to function call

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Data Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Align below data within 8 byte
align 8, db 0

; The end of GDTR is aligned within 8 byte
dw 0x0000
; Define GDTR data structure
GDTR:
	dw GDTEND - GDT - 1		; Total size of GDT
	dd ( GDT - $$ + 0x10000 )	
					; Start address of GDT

; Define GDT
GDT:
	; NULL descriptor
	NULLDescriptor:
		dw 0x0000
		dw 0x0000
		db 0x00
		db 0x00
		db 0x00
		db 0x00
		
	; Code segment descriptor for IA-32e mode
	IA_32eCODEDESCRIPTOR:
		dw 0xFFFF		; Limit [15:0]
		dw 0x0000		; Base [15:0]
		db 0x00			; Base [23:16]
		db 0x9A			; P=1, DPL=0, Code segment, Execute/Read
		db 0xAF			; G=1, D=0, L=1, Limit[19:16]
		db 0x00			; Base [31:24]
		
	; Data segment descriptor for IA-32e mode
	IA_32eDATADESCRIPTOR:
		dw 0xFFFF		; Limit [15:0]
		dw 0x0000		; Base [15:0]
		db 0x00			; Base [23:16]
		db 0x92			; P=1, DPL=0, Data segment, Read/Write
		db 0xAF			; G=1, D=0, L=1, Limit[19:16]
		db 0x00			; Base [31:24]

	; Code segment descriptor for protected mode
	CODEDESCRIPTOR:
		dw 0xFFFF		; Limit [15:0]
		dw 0x0000		; Base [15:0]
		db 0x00			; Base [23:16]
		db 0x9A			; P=1, DPL=0, Code segment, Execute/Read
		db 0xCF			; G=1, D=1, L=0, Limit[19:16]
		db 0x00			; Base [31:24]

	; Data segment descriptor for protected mode
	DATADESCRIPTOR:
		dw 0xFFFF		; Limit [15:0]
		dw 0x0000		; Base [15:0]
		db 0x00			; Base [23:16]
		db 0x92			; P=1, DPL=0, Data segment, Read/Write
		db 0xCF			; G=1, D=1, L=0, Limit[19:16]
		db 0x00			; Base [31:24]
GDTEND:
 
SWITCHSUCCESSMESSAGE:
	db 'Switch To Protected Mode Success~!!', 0

times 512 - ( $ - $$ ) db 0x00		
					; Fill 512 bytes

	
