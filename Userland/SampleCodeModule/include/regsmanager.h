//
// Created by Santiago Devesa on 12/06/2024.
//

#ifndef TPE_ARQ_REGSMANAGER_H
#define TPE_ARQ_REGSMANAGER_H

#include <stdint.h>

#define REGS_AMOUNT 18

enum registers_idx {
    RAX_IDX = 0,
    RBX_IDX,
    RCX_IDX,
    RDX_IDX,
    RSI_IDX,
    RDI_IDX,
    RBP_IDX,
    R8_IDX,
    R9_IDX,
    R10_IDX,
    R11_IDX,
    R12_IDX,
    R13_IDX,
    R14_IDX,
    R15_IDX,
    RIP_IDX,
    RSP_IDX,
    RFLAGS_IDX
};

/**
 * @brief Updates the registers values.
 */
void updateRegs();

/**
 * @brief Returns the name of a register.
 * @param reg: the register index.
 * @return the name of the register.
 */
const char * getRegName(int reg);

/**
 * @brief Returns the value of a register.
 * @param reg: the register index.
 * @return the value of the register.
 */
uint64_t getRegValue(int reg);

/**
 * @brief Returns if the registers were updated.
 * @return 1 if the registers were updated, 0 otherwise.
 */
unsigned char regsUpdated();

#endif
