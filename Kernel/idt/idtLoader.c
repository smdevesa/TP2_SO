#include <stdint.h>
#include <idtLoader.h>
#include <defs.h>
#include <interrupts.h>

#pragma pack(push)		/* Push de la alineación actual */
#pragma pack (1) 		/* Alinear las siguiente estructuras a 1 byte */

/* Descriptor de interrupcion */
typedef struct {
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop)		/* Reestablece la alinceación actual */



DESCR_INT * idt = (DESCR_INT *) 0;	// IDT de 255 entradas

static void setup_IDT_entry (int index, uint64_t offset) {
    idt[index].selector = 0x08;
    idt[index].offset_l = offset & 0xFFFF;
    idt[index].offset_m = (offset >> 16) & 0xFFFF;
    idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
    idt[index].access = ACS_INT;
    idt[index].cero = 0;
    idt[index].other_cero = (uint64_t) 0;
}

void load_idt() {

    // Interrupciones de hardware
    setup_IDT_entry(0x00, (uint64_t)&_exception0Handler); // Excepcion 0
    setup_IDT_entry(0x06, (uint64_t)&_exception6Handler); // Excepcion 6
    setup_IDT_entry(0x20, (uint64_t)&_irq00Handler); // Timer
    setup_IDT_entry(0x21, (uint64_t)&_irq01Handler); // Teclado

    // Interrupciones de software
    setup_IDT_entry(0x80, (uint64_t)&_irq80Handler); // Syscalls

    // Configuracion de máscaras del PIC
    picMasterMask(0xFC); // 1111 1100 -> solo habilito el timer y el teclado.
    picSlaveMask(0xFF);  // 1111 1111 -> deshabilito todas las interrupciones del slave.

    _sti();
}

