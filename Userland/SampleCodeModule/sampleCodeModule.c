/* sampleCodeModule.c */

#include <shell.h>
#include <regsmanager.h>
#include <syscalls.h>

int main() {
    _sys_setCtrlRAction(&updateRegs);
    setUsername("Clifford");
    shell();
    return 0;
}