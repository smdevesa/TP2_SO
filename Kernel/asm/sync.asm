; sync.asm

GLOBAL acquire
GLOBAL release

section .text
; void acquire(lock_t *lock)
acquire:
    mov al, 0
.retry:
    xchg [rdi], al
    test al, al
    jz .retry
    ret

; void release(lock_t *lock)
release:
    mov byte [rdi], 1
    ret