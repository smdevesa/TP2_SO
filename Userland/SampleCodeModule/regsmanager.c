//
// Created by Santiago Devesa on 12/06/2024.
//

#include <stdint.h>
#include <regsmanager.h>
#include <stddef.h>

extern uint64_t * _getRegs();
extern void _updateRegs();

static uint64_t regs[REGS_AMOUNT];
static const char * regNames[REGS_AMOUNT] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RSP", "RFLAGS"
};

static unsigned char updated = 0;

void updateRegs() {
    _updateRegs();
    uint64_t * newRegs = _getRegs();
    for(int i=0; i<REGS_AMOUNT; i++) {
        regs[i] = newRegs[i];
    }
    updated = 1;
}

const char * getRegName(int reg) {
    if(reg < 0 || reg >= REGS_AMOUNT)
        return NULL;

    return regNames[reg];
}

uint64_t getRegValue(int reg) {
    if(reg < 0 || reg >= REGS_AMOUNT)
        return 0;
    return regs[reg];
}

unsigned char regsUpdated() {
    return updated;
}

