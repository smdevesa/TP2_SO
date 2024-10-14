GLOBAL _sys_write,
GLOBAL _sys_read
GLOBAL _sys_drawRectangle
GLOBAL _sys_clearScreen
GLOBAL _sys_getCoords
GLOBAL _sys_undrawChar
GLOBAL _sys_getScreenInfo
GLOBAL _sys_getFontInfo
GLOBAL _sys_getTime
GLOBAL _sys_setFontScale
GLOBAL _sys_getRegisters
GLOBAL _sys_sleep
GLOBAL _sys_playSound
GLOBAL _sys_yield
GLOBAL _sys_getpid
GLOBAL _sys_createProcess
GLOBAL _sys_kill
GLOBAL _sys_block
GLOBAL _sys_unblock
GLOBAL _sys_changePriority
GLOBAL _sys_exit
GLOBAL _sys_malloc
GLOBAL _sys_free

section .text

%macro syscall 1
    mov rax, %1
    int 80h
    ret
%endmacro

_sys_read: syscall 0x00
_sys_write: syscall 0x01
_sys_drawRectangle: syscall 0x02
_sys_clearScreen: syscall 0x03
_sys_getCoords: syscall 0x04
_sys_getScreenInfo: syscall 0x05
_sys_getFontInfo: syscall 0x06
_sys_getTime: syscall 0x07
_sys_setFontScale: syscall 0x08
_sys_getRegisters: syscall 0x09
_sys_sleep: syscall 0x0A
_sys_playSound: syscall 0x0B
_sys_yield: syscall 0x0C
_sys_getpid: syscall 0x0D
_sys_createProcess: syscall 0x0E
_sys_kill: syscall 0x0F
_sys_block: syscall 0x10
_sys_unblock: syscall 0x11
_sys_changePriority: syscall 0x12
_sys_exit: syscall 0x13
_sys_malloc: syscall 0x14
_sys_free: syscall 0x15
