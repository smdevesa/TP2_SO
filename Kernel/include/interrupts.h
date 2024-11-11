/*
 *   interrupts.h
 *
 *  Created on: Apr 18, 2010
 *      Author: anizzomc
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include "idtLoader.h"
#include <stdint.h>

// Hardware interrupts handlers
void _irq00Handler(void); // Timertick
void _irq01Handler(void); // Keyboard
void _irq02Handler(void); // Unused
void _irq03Handler(void); // Unused
void _irq04Handler(void); // Unused
void _irq05Handler(void); // Unused

// Software interrupts handlers
void _irq80Handler(void); // System calls

// Exception handlers
void _exception0Handler(void);
void _exception6Handler(void);

void _cli(void);

void _sti(void);

void _hlt(void);

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

// Termina la ejecución de la cpu.
void haltcpu(void);

#endif /* INTERRUPTS_H_ */
