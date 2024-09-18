GLOBAL _getRegs
GLOBAL _updateRegs

section .text

_updateRegs:
    mov [regsv + 8 * 0], rax
    mov [regsv + 8 * 1], rbx
    mov [regsv + 8 * 2], rcx
    mov [regsv + 8 * 3], rdx
    mov [regsv + 8 * 4], rsi
    mov [regsv + 8 * 5], rdi
    mov [regsv + 8 * 6], rbp
    mov [regsv + 8 * 7], r8
    mov [regsv + 8 * 8], r9
    mov [regsv + 8 * 9], r10
    mov [regsv + 8 * 10], r11
    mov [regsv + 8 * 11], r12
    mov [regsv + 8 * 12], r13
    mov [regsv + 8 * 13], r14
    mov [regsv + 8 * 14], r15
    mov rax, [rsp] ; RIP
    mov [regsv + 8 * 15], rax
    mov rax, rsp
    mov [regsv + 8 * 16], rax ; RSP
    xor rax, rax
    pushfq
    pop rax
    mov [regsv + 8 * 17], rax ; RFLAGS
    ret

_getRegs:
    mov rax, regsv
    ret

section .bss

regsv resq 18