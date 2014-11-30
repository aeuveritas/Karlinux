[ORG 0x00]				; Start address is 0x00
[BITS 16]				; Code is 16 bit

SECTION .text				; Define text section

	jmp 0x07C0:START		; insert 0x07C0 to CS register and move to START.

START:
	mov ax, 0x07C0			; start address of bootloader is 0x07C0
	mov ds, ax			; Move value of AX to DS 
	
	mov ax, 0xB800			; Move 0xB800 to AX register
	mov es, ax			; Move value of AX to DS
					; DS is base address, 
					; so after this every address is + 0xB800.

	mov si, 0			; Initiate SI register(String Original Index Register)
	
.SCREENCLEARLOOP:			; Clear screen
	mov byte [ es: si ], 0		; Initiate video memory
	mov byte [ es: si + 1], 0x0A	; Assign character feature.
	
	add si, 2			; Move to next video memory
	
	cmp si, 80 * 25 * 2		; Total number of register is 80 * 25 * 2
	jl .SCREENCLEARLOOP		; if si is less than 80 * 25 * 2
					; jump to .SCREENCLEARLOOP
									
	mov si, 0			; Initiate SI register
	mov di, 0			; Initiate DI register
	
.MESSAGELOOP:				; Print message
	mov cl, byte [ si + MESSAGE1]	; Copy the value of si + MESSAGE1 address to CL register.
					; CL register is low 1 byte of CX register
									
	cmp cl, 0 			; if charater is 0, it means end.
	je .MESSAGEEND			; if cl is 0, jump to .MESSAGEEND
	
	mov byte [ es: di ], cl		; if cl is not 0, print value of es + di address
	
	add si, 1			; Move to the next character
	add di, 2			; Move to the next address
	
	jmp .MESSAGELOOP		; Move to the next iteration

.MESSAGEEND:

	jmp $				; Infinite Loop
	
MESSAGE1: 
	db 'Karlinux OS Boot Loader Start!!', 0	
					; Message

	mov byte [ es: 0x00 ], 'M'	; Character 'M'
	mov byte [ es: 0x01 ], 0x4A 	; Character is Red, Background is Green.

	;jmp $				; At this point, infinite loop

	times 510 - ( $ - $$ )	db	0x00	; $: adress of current line
					; $$: start address of current section
					; $ - $$: Offset from current section
					; 510 - ( $ - $$ ): from here to address 510
					; db 0x00: Define 1 byte as 0x00
					; times: repeat
					; From current to address 510, fill 0x00
									
	db 0x55 			; Define 1 byte as 0x55
	db 0xAA				; Define 1 byte as 0xAA
					; Address 511, 512 are 0x55, 0xAA
	
