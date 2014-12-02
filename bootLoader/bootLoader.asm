;[ORG 0x00]				; Start address is 0x00
[BITS 16]				; Code is 16 bit

SECTION .text				; Define text section

	jmp 0x07C0:START		; Insert 0x07C0 to CS register and move to START.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Environment setting
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
TOTALSECTORCOUNT:
	dw	1024			; Maximum image size is 1152 sector(0x90000byte)
					; except for bootloader

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Code Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
	mov ax, 0x07C0			; Start address of bootloader is 0x07C0
	mov ds, ax			; Move value of AX to DS (Data Segment Reigster)
	mov ax, 0xB800			; Move 0xB800 to AX register
	mov es, ax			; Move value of AX to ES
					; ES is related to video.

	; Make a stack section from 0x0000:0000 ~ 0x0000:FFFF (64KB)
	mov ax, 0x0000			; Start address of stack is 0x0000
	mov ss, ax			; Move value of AS to SS (Stack Segment Register)
	mov sp, 0xFFFE			; Set SP as 0xFFFE, SP is Stack Pointer
	mov bp, 0xFFFE			; Set BP as 0xFFFE, BP is Data pointer for Stack

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Clear Screen and color changes to green
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  
	mov si, 0			; Initiate SI register(String Original Index Register)
	
.SCREENCLEARLOOP:			; Clear screen
	mov byte [ es: si ], 0		; Initiate video memory
	mov byte [ es: si + 1], 0x0A	; Assign character feature.
	
	add si, 2			; Move to next video memory
	
	cmp si, 80 * 25 * 2		; Total number of register is 80 * 25 * 2
	jl .SCREENCLEARLOOP		; if si is less than 80 * 25 * 2
					; jump to .SCREENCLEARLOOP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Print start message
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	push MESSAGE1			; Insert address of message to stack
	push 0				; Insert point of Y axis to stack
	push 0				; Insert point of X axis to stack
	call PRINTMESSAGE		; Call PRINTMESSAGE function
	add sp, 6			; Remove parameters inserted

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Print message of OS image copying 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	push IMAGELOADINGMESSAGE	; Insert address of message to stack
	push 1				; Insert point of Y axis to stack
	push 0				; Insert point of X axis to stack
	call PRINTMESSAGE		; Call PRINTMESSAGE function
	add sp, 6			; Remove parameters inserted

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Load OS image from disk
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Reset first before reading disk
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RESETDISK:				; Code for reset disk
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Call BIOS Reset Function
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;Service number is 0, Drive number(0=Floppy)
	mov ax, 0
	mov dl, 0
	int 0x13
	; Move to Error handler when error occurs
	jc HANDLEDISKERROR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Read sector from disk
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Set 0x10000 to address disk data will be copied.
	mov si, 0x1000			; Initiate SI register with memory copy start address
	mov es, si			; Set ES register
	mov bx, 0x0000			; Set address disk data will be copied (0x1000:0000 = 0x10000)

	mov di, word [ TOTALSECTORCOUNT ]
					; Set DI register as the number of OS image size

READDATA:
	; Check reading done
	cmp di, 0			; Compare 0 with the number of remained sectorwhich will be copied
	je READEND			; If it is 0, reading has been done
	sub di, 0x1			; Sub 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Call BIOS Read Function
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov ah, 0x02			; BIOS service number 2 (Read Sector)
	mov al, 0x1			; The number of reading sector is 1
	mov ch, byte [ TRACKNUMBER ] 	; Set track number of reading
	mov cl, byte [ SECTORNUMBER ]	; Set sector number of reading
	mov dh, byte [ HEADNUMBER ]	; Set head number of reading
	mov dl, 0x00			; Set drive number of reading
	int 0x13			; Execute interrupt service
	jc HANDLEDISKERROR		; If there is error, move to HANDLEDISKERROR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Calculate address, track, head, sector address
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	add si, 0x0020			; 1 sector reading is 512 byte(0x200)
	mov es, si			; ES is base register
	
	; After 1 sector reading, increase sector number and
	; Check the last sector(18)
	; If it is not the last sector, move to READDATA
	mov al, byte [ SECTORNUMBER ]	; Set AL register as settor number
	add al, 0x01			; Increase sector number
	mov byte [ SECTORNUMBER ], al	; Set increased setcor number to SCTORNUMBER
	cmp al, 19			; Compare increased sector number with 19
	jl READDATA			; If it is less than 19, move to READDATA

	; If all sector is read, toggle head and
	; Set sector number as 1
	xor byte [ HEADNUMBER ], 0x01	; Toggle the head
	mov byte [ SECTORNUMBER ], 0x01	; Set sector number as 1

	; If head is 0, both headers are read.
	; Move to next and increase track number
	cmp byte [ HEADNUMBER ], 0x00	; Compare HEADNUMBER with 0x00
	jne READDATA			; If head is not 0, move to READDATA

	; Increase track number by 1 and
	; Move to READDATA in order to read sector
	add byte [ TRACKNUMBER ], 0x01	; Increase track number by 1	
	jmp READDATA			; Move to READDATA
READEND:
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Print the message OS loading is done
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	push LOADINGCOMPLETEMESSAGE	; Insert address of message to stack
	push 1				; Insert point of Y axis to stack
	push 20				; Insert point of X axis to stack
	call PRINTMESSAGE		; Call PRINTMESSAGE function
	add sp, 6			; Remove parameters inserted

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Execute virtual OS loaded
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	jmp 0x1000:0x0000

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Function Code Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Function for handling disk error
HANDLEDISKERROR:
	push DISKERRORMESSAGE		; Insert address of error message to stack
	push 1				; Insert point of Y axis to stack
	push 20				; Insert point of X acis to stack
	call PRINTMESSAGE		; Call PRINTMESSAGE function
	
	jmp $				; Infinite loop

; Function for message print
; PARAM: x, y, message
PRINTMESSAGE:
	push bp				; Insert base point register(BP) to stack
	mov bp, sp			; Set BP as SP
					; BP is used to access parameters
	
	push es 			; Insert ES ~ DX to stack
	push si				; currnet values sould be recoveried after
	push di				; function execution
	push ax	
	push cx
	push dx

	; Set ES as video mode address
	mov ax, 0xB800			; Move 0xB800 to AX register
	mov es, ax			; Set ES with AX
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Calculate the vidoe address from X, Y
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Get line address from Y
	mov ax, word [ bp + 6 ] 	; Set AX with parameter 2 (2*(1+2))
	mov si, 160			; Set SI with the number of byte 
					; as one line
	mul si				; Y address = AX * SI
	mov di, ax			; DI = AX

	; Get line address from X
	mov ax, word [ bp + 4 ] 	; Set AX with parameter 1 (2*(1+1))
	mov si, 2			; Set SI with the number of byte 
					; as one word
	mul si				; X address = AX * SI
	mov di, ax			; DI = AX

	; Address of print message
	mov si, word [ bp + 8 ] 	; parameter 3 (2*(1+3))
	
.MESSAGELOOP:				; Print message
	mov cl, byte [ si ]		; Copy the value of si address to CL register.
					; CL register is low 1 byte of CX register
									
	cmp cl, 0 			; if charater is 0, it means end.
	je .MESSAGEEND			; if cl is 0, jump to .MESSAGEEND
	
	mov byte [ es: di ], cl		; if cl is not 0, print value of es + di address
	
	add si, 1			; Move to the next character
	add di, 2			; Move to the next address
	
	jmp .MESSAGELOOP		; Move to the next iteration

.MESSAGEEND:
	pop dx				; Pop ES ~ DX from stack
	pop cx				
	pop ax
	pop di
	pop si
	pop es
	pop bp				; Restore BP
	ret				; Return to function call
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Data Section
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
MESSAGE1: 
	db 'Karlinux OS Boot Loader Start!!', 0	

DISKERRORMESSAGE:
	db 'DISK ERROR~!!', 0

IMAGELOADINGMESSAGE:
	db 'OS Image Loading...', 0

LOADINGCOMPLETEMESSAGE:
	db 'Complete~!!', 0

; Variable for disk reading
SECTORNUMBER:
	db 0x02				; Start sector number for OS image
HEADNUMBER:
	db 0x00 			; Start head number of OS image
TRACKNUMBER:
	db 0x00				; Start track number of OS image

	times 510 - ( $ - $$ )	db	0x00	
					; $: adress of current line
					; $$: start address of current section
					; $ - $$: Offset from current section
					; 510 - ( $ - $$ ): from here to address 510
					; db 0x00: Define 1 byte as 0x00
					; times: repeat
					; From current to address 510, fill 0x00
									
	db 0x55 			; Define 1 byte as 0x55
	db 0xAA				; Define 1 byte as 0xAA
					; Address 511, 512 are 0x55, 0xAA
	
