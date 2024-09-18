
#ifndef TPE_ARQ_FONT_H
#define TPE_ARQ_FONT_H

#include <stdint.h>
#include <stddef.h>

// Funciones de informacion de la fuente

/*
 * Devuelve el ancho de la fuente en pixeles.
 */
uint8_t getFontWidth();

/*
 * Devuelve el alto de la fuente en pixeles.
 */
uint8_t getFontHeight();

// Funciones para la impresion

/*
 * Devuelve un puntero a la representacion de un caracter en la fuente
 * o NULL si el caracter no esta en la fuente.
 * @param c: el caracter a representar.
 */
uint8_t * getFontChar(char c);

#endif //TPE_ARQ_FONT_H
