[BITS 64]			; 64 bit code

SECTION .text			; Define text section

; To use some function defined in other file, import them
extern kCommonExceptionHandler, kCommonInterruptHandler, kKeyboardHandler

; Expose the name to use it in C
; ISR for Exception 
global kISRDivideError, kISRDebug, kISRNMI, kISRBreakPoint, kISROverflow
global kISRBoundRangeExceeded, kISRInvalidOpcode, kISRDeviceNotAvailable
global kISRDoubleFault, kISRCoprocessorSegmentOverrun, kISRInvalidTSS
global kISRSegmentNotPresent, kISRStackSegmentFault, kISRGeneralProtection
global kISRPageFault, kISR15, kISRFPUError, kISRAlignmentCheck
global kISRMachineCheck, kISRSIMDError, kISRETCException

; ISR for Interrupt
global kISRTimer, kISRKeyboard, kISRSlavePIC, kISRSerial2, kISRSerial1
global kISRParallel2, kISRFloppy, kISRParallel1, kISRRTC, kISRReserved
global kISRNotUsed1, kISRNotUsed2, kISRMouse, kISRCoprocessor
global kISRHDD1, kISRHDD2, kISRETCInterrupt

; Macro to store context and switch selector
%macro KSAVECONTEXT 0		; Define KSAVECONTEXT without parameter
	; Store from RBP register to GS semgment selector in stack
	push rbp
	mov rbp, rsp
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
	push r12
	push r13
	push r14
	push r15
	
	mov ax, ds		; We cannot insert DS segment selector and
	push rax		; ES segment selector directly,
				; store two in RAX register and insert to stack
	mov ax, es
	push rax
	push fs
	push gs
	
	; Switch segment selector
	mov ax, 0x10		; Store kernel data segment descriptor in AX register
	mov ds, ax		; Change kernel data segment
	mov es, ax		; From DS segment register to FS segment selector
	mov gs, ax
	mov fs, ax
%endmacro			; Macro end 

; Restore context
%macro KLOADCONTEXT 0		; Define KLOADCONTEXT without parameter
	; Restore from GS segment selector to RBP register 
	pop gs
	pop fs
	pop rax
	mov es, ax		; We cannot restore DS segment selector and
	pop rax			; ES segment selector directly,
				; store two in RAX register and restore
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
%endmacro			; Macro end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Exception Handler 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; #0, Divide Error ISR 
kISRDivideError:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number and call handler
	mov rdi, 0
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #1, Debug ISR 
kISRDebug:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number and call handler
	mov rdi, 1
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #2, NMI ISR 
kISRNMI:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number and call handler
	mov rdi, 2
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #3, BreakPoint ISR 
kISRBreakPoint:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number and call handler
	mov rdi, 3
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #4, Overflow ISR 
kISROverflow:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number and call handler
	mov rdi, 4
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #5, Bound Range Exceeded ISR 
kISRBoundRangeExceeded:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number and call handler
	mov rdi, 5
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #6, Invalid Opcode ISR 
kISRInvalidOpcode:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number and call handler
	mov rdi, 6
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #7, Device Not Availabe ISR 
kISRDeviceNotAvailable:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number and call handler
	mov rdi, 7
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #8, Double Fault ISR 
kISRDoubleFault:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number & Error and call handler
	mov rdi, 8
	mov rsi, qword [rbp + 8]
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	add rsp, 8
	iretq			; Handler done and back to previous code

; #9, Coprocessor Segment Overrun ISR 
kISRCoprocessorSegmentOverrun:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number and call handler
	mov rdi, 9
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #10, Invalid TSS ISR 
kISRInvalidTSS:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number & error code and call handler
	mov rdi, 10
	mov rsi, qword [rbp + 8]
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	add rsp, 8		; Remove error code
	iretq			; Handler done and back to previous code

; #11, Segment Not Present ISR 
kISRSegmentNotPresent:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number & error code and call handler
	mov rdi, 11
	mov rsi, qword [rbp + 8]
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	add rsp, 8		; Remove error code
	iretq			; Handler done and back to previous code

; #12, Stack Segment Fault ISR 
kISRStackSegmentFault:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number & error code and call handler
	mov rdi, 12
	mov rsi, qword [rbp + 8]
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	add rsp, 8		; Remove error code
	iretq			; Handler done and back to previous code

; #13, General Protection ISR 
kISRGeneralProtection:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number & error code and call handler
	mov rdi, 13
	mov rsi, qword [rbp + 8]
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	add rsp, 8		; Remove error code
	iretq			; Handler done and back to previous code

; #14, Page Fault ISR 
kISRPageFault:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number & error code and call handler
	mov rdi, 14
	mov rsi, qword [rbp + 8]
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	add rsp, 8		; Remove error code
	iretq			; Handler done and back to previous code

; #15, Reserved ISR 
kISR15:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 15
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #16, FPU Error ISR 
kISRFPUError:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 16
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #17, Alignment Check ISR 
kISRAlignmentCheck:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number & error code and call handler
	mov rdi, 17
	mov rsi, qword [rbp + 8]
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	add rsp, 8		; Remove error code
	iretq			; Handler done and back to previous code

; #18, Machine Check ISR 
kISRMachineCheck:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 18
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #19, SIMD Floating Pint Exception ISR 
kISRSIMDError:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 19
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #20 ~ #31, Reserved ISR 
kISRETCException:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 20
	call kCommonExceptionHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Interrupt Handler 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; #32, Timer ISR 
kISRTimer:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 32
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #33, Keyboard ISR 
kISRKeyboard:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 33
	call kKeyboardHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #34, Slave PIC ISR 
kISRSlavePIC:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 34
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #35, Serial port 2 ISR 
kISRSerial2:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 35
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #36, Serial port 1 ISR 
kISRSerial1:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 36
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #37, Parallel port 2 ISR 
kISRParallel2:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 37
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #38, Floppy disk controller ISR 
kISRFloppy:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 38
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #39, Parallel port 1 ISR 
kISRParallel1:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 39
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #40, RTC ISR 
kISRRTC:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 40
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #41, Reserved interrupt ISR 
kISRReserved:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 41
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #42, Not Used1 ISR 
kISRNotUsed1:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 42
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #43, Not Used2 ISR 
kISRNotUsed2:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 43
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #44, Mouse ISR 
kISRMouse:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 44
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #45, Coprocessor ISR 
kISRCoprocessor:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 45
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
	
; #46, HDD1 ISR 
kISRHDD1:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 46
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #47, HDD2 ISR 
kISRHDD2:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 47
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code

; #48, ETC Interrupt ISR 
kISRETCInterrupt:
	KSAVECONTEXT		; Store context and switch selector to
				; kernel data descriptor
	; Insert exception number code and call handler
	mov rdi, 48
	call kCommonInterruptHandler
	
	KLOADCONTEXT		; Restore context
	iretq			; Handler done and back to previous code
