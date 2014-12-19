[BITS 32]				; Below codes are 32 bit

; Make function call path for C
global kReadCPUID, kSwitchAndExecute64bitKernel

SECTION .text				; Define text section

; Return CPUID
;	PARAM: DWORD dwEAX, DWORD * pdwEAX, * pdwEBX, * pdwECX, * pdwEDX
kReadCPUID:
	push ebp			; Insert Base Pointer Register (EBP)
	mov ebp, esp			; Move Stack Pointer Register (ESP)
					; to Base Pointer Register (EBP)
	push eax 			; EAX ~ ESI are temporary values for next stack
	push ebx
	push ecx 
	push edx 
	push esi
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Execute CPUIS instruction with EAX
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov eax, dword [ebp + 8]	; Store parameter 1(dwEAX) in EAX 
	cpuid				; Execute CPUID instruction
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Store return values to parameters
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; *pdwEAX
	mov esi, dword [ebp + 12]	; Store parameter 2(pdwEAX) in ESI
	mov dword [esi], eax		; pdwEAX is a pointer.
					; Store parameter 2(EAX) in the address of pdwEAX
	
	; *pdwEBX
	mov esi, dword [ebp + 16]	; Store parameter 3(pdwEBX) in ESI
	mov dword [esi], ebx		; pdwEBX is a pointer.
					; Store parameter 3(EBX) in the address of pdwEBX
	
	; *pdwECX
	mov esi, dword [ebp + 20]	; Store parameter 4(pdwECX) in ESI
	mov dword [esi], ecx		; pdwECX is a pointer.
					; Store parameter 4(ECX) in the address of pdwECX
	
	; *pdwEDX
	mov esi, dword [ebp + 24]	; Store parameter 5(pdwEDX) in ESI
	mov dword [esi], edx		; pdwEDX is a pointer.
					; Store parameter 5(EDX) in the address of pdwEDX
	
	pop esi				; Restore ESI ~ EBP
	pop edx 			; The order is reversed
	pop ecx	
	pop ebx 
	pop eax 
	pop ebp 			; Restore Base Pointer Register (EBP)
	ret 				; Return back to previous location
	
; Change to IA-32e and Execute 64 bit Kernel 
;	PARAM: No
kSwitchAndExecute64bitKernel:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; PAE bit in CR4 control register set 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov eax, cr4			; Store the value of CR4 control register to EAX register
	or eax, 0x20			; Set PAE bit (bit 5) as 1
	mov cr4, eax 			; Store PAE bit in CR4 control register
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Enable cache and Store PML4T address 
; in CR3
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov eax, 0x100000		; Store PML4T(0x100000, 1MB) in EAX 
	mov cr3, eax			; Store 0x10000(1MB) in CR3 control register
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set IA32_EFER.LM as 1 to enable IA-32e
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov ecx, 0xC0000080		; Store address of IA32_EFER MSR
	rdmsr				; Read MSR register
	
	or eax, 0x0100			; Set LME bit (bit 8) in lower 32 bit of IA32_EFER MSR	
	wrmsr				; Write MSR register
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Enable cache and paging function
; by setting CR0 control register's
; NW bit (bit 29), CD bit (bit 30),
; PG bit (bit 31), 0, 0, 1 respectively
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov eax, cr0			; Store CRO control regitser in EAX register
	or eax, 0xE0000000		; Set NW bit (bit 29), CD bit (bit 30),
					; PG bit (bit 31) as 1 by OR
	xor eax, 0x60000000		; Set NW bit (bit 29), CD bit (bit 30) as 0 by XOR
	mov cr0, eax 			; Restore CR0 control register
	
	jmp 0x08: 0x200000		; Change CS segment selector to IA-32e code segment descriptor
					; and Jump to 0x200000 (2MB)
	
	; Not execute
	jmp $
	
