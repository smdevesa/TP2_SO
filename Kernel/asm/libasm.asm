GLOBAL cpuVendor
GLOBAL _getScancode
GLOBAL _checkKeyboardBuffer
GLOBAL _readTime
GLOBAL _getRegisters
GLOBAL _updateRegisters
GLOBAL _outb
GLOBAL _inb

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

_getScancode:
    xor rax, rax
    in al, 0x60
    ret

_readTime:
    push dx
    mov dx, 0x70
    mov al, dil
    out dx, al
    xor rax, rax
    in al, 0x71
    pop dx
    ret

_updateRegisters:
    mov [_registersMemory], rax
    mov [_registersMemory + 1*8], rbx
    mov [_registersMemory + 2*8], rcx
    mov [_registersMemory + 3*8], rdx
    mov [_registersMemory + 4*8], rsi
    mov [_registersMemory + 5*8], rdi
    mov [_registersMemory + 6*8], rbp
    mov [_registersMemory + 7*8], r8
    mov [_registersMemory + 8*8], r9
    mov [_registersMemory + 9*8], r10
    mov [_registersMemory + 10*8], r11
    mov [_registersMemory + 11*8], r12
    mov [_registersMemory + 12*8], r13
    mov [_registersMemory + 13*8], r14
    mov [_registersMemory + 14*8], r15
    mov rax, [rsp]
    mov [_registersMemory + 15*8], rax ; instruction pointer
    mov rax, rsp
    mov [_registersMemory + 16*8], rax
    ret

_getRegisters:
    mov rax, _registersMemory
    ret

; void outb(uint16_t port, uint8_t value)
_outb:
    push rdx

    mov dx, di
    mov al, sil
    out dx, al

    pop rdx
    ret

; extern uint8_t inb(uint16_t port)
_inb:
    push rdx

    mov dx, di
    in al, dx

    pop rdx
    ret

section .bss
_registersMemory resq 17