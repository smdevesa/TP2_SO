
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq80Handler
GLOBAL _exception6Handler
GLOBAL _exception0Handler

GLOBAL _getSnapshot

EXTERN irqDispatcher
EXTERN syscallDispatcher
EXTERN exceptionDispatcher
EXTERN getStackBase

section .rodata
    userland equ 0x400000

SECTION .text

%macro pushState 1
    %if %1
        push rax
    %endif
	push rbx
	push rcx
	push rdx
	push rbp
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
%endmacro

%macro popState 1
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
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	%if %1
	    pop rax
	%endif
%endmacro

%macro fillSnapshot 0
    mov [regs], rax
    mov [regs+8], rbx
    mov [regs+16], rcx
    mov [regs+24], rdx
    mov [regs+32], rsi
    mov [regs+40], rdi
    mov [regs+48], rbp
    mov [regs+56], r8
    mov [regs+64], r9
    mov [regs+72], r10
    mov [regs+80], r11
    mov [regs+88], r12
    mov [regs+96], r13
    mov [regs+104], r14
    mov [regs+112], r15
    mov rax, [rsp + 24] ;rsp
    mov [regs+120], rax
    mov rax, [rsp] ;rip
    mov [regs+128], rax
    mov rax, [rsp+16] ;rflags
    mov [regs+136], rax
    mov rax, [rsp+8] ;cs
    mov [regs+144], rax
    mov rax, [rsp+32] ;ss
    mov [regs+152], rax
%endmacro

%macro irqHandlerMaster 1
	pushState 1

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState 1
	iretq
%endmacro

%macro exceptionHandler 1

    fillSnapshot

	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher

	call getStackBase

	mov [rsp+24], rax

	mov rax, userland
	mov [rsp], rax

	iretq
%endmacro




_getSnapshot:
   mov rax, regs
   ret



_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;Syscall
_irq80Handler:
    pushState 0
    mov r9, rax
    call syscallDispatcher
    popState 0
    iretq


;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid Opcode Exception
_exception6Handler:
    exceptionHandler 6

haltcpu:
	cli
	hlt
	ret



SECTION .bss
	aux resq 1
	regs resq 20