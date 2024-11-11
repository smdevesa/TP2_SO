#ifndef TPE_ARQ_VIDEODRIVER_H
#define TPE_ARQ_VIDEODRIVER_H

#include <stdint.h>

/**
 * @brief Draws a pixel in the screen on the given coordinates.
 * @param hexColor: the color of the pixel in hexadecimal. Usage: 0x00RRGGBB.
 * @param x: the x coordinate of the pixel.
 * @param y: the y coordinate of the pixel.
 */
void drawPixel(uint32_t hexColor, uint64_t x, uint64_t y);

/**
 * @brief Draws a character in the screen on the given coordinates using the
 * 8x16 font in font.c
 * @param c: the character to draw.
 * @param hexColor: the color of the character in hexadecimal. Usage:
 * 0x00RRGGBB.
 * @param x: the x coordinate of the character.
 * @param y: the y coordinate of the character.
 */
void drawChar(char c, uint32_t charColor, uint32_t bgColor, uint64_t x,
			  uint64_t y);

/**
 * @brief Draws a rectangle in the screen on the given coordinates.
 * @param hexColor: the color of the rectangle in hexadecimal. Usage:
 * 0x00RRGGBB.
 * @param x: the x coordinate of the rectangle.
 * @param y: the y coordinate of the rectangle.
 * @param width: the width of the rectangle.
 * @param height: the height of the rectangle.
 * @return 0 if the rectangle was drawn successfully, 1 if the rectangle was out
 * of bounds.
 */
int drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width,
				  uint64_t height);

/**
 * @brief Clears the screen.
 */
void clearScreen();

/**
 * @brief Returns the width of the screen in pixels.
 */
uint16_t getScreenWidth();

/**
 * @brief Returns the height of the screen in pixels.
 */
uint16_t getScreenHeight();

/**
 * @brief Sets the scale of the screen.
 * @param newScale: the new scale of the screen (supported 1, 2 or 3).
 * @return 1 if the scale was set successfully, 0 if the scale is not supported.
 */
uint64_t setScale(uint64_t newScale);

/**
 * @brief Returns the current scale of the screen.
 */
uint8_t getScale();

#endif // TPE_ARQ_VIDEODRIVER_H
