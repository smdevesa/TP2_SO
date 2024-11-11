// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <font.h>
#include <videoDriver.h>

#define BLACK 0x00000000

#define MIN_SCALE 1
#define MAX_SCALE 3
#define DEFAULT_SCALE 1

struct vbe_mode_info_structure {
	uint16_t
		attributes;		  // deprecated, only bit 7 should be of interest to you, and it
						  // indicates the mode supports a linear frame buffer.
	uint8_t window_a;	  // deprecated
	uint8_t window_b;	  // deprecated
	uint16_t granularity; // deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode
						   // without returning to real mode
	uint16_t pitch;		   // number of bytes per horizontal line
	uint16_t width;		   // width in pixels
	uint16_t height;	   // height in pixels
	uint8_t w_char;		   // unused...
	uint8_t y_char;		   // ...
	uint8_t planes;
	uint8_t bpp;   // bits per pixel in this mode
	uint8_t banks; // deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may
					   // be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer; // physical address of the linear frame buffer; write
						  // here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size; // size of memory in the framebuffer but not
								  // being displayed on the screen
	uint8_t reserved1[206];
} __attribute__((packed));

typedef struct vbe_mode_info_structure *VBEInfoPtr;

static volatile VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

static volatile int scale = DEFAULT_SCALE;

static void drawScaledPixel(uint32_t hexColor, uint64_t x, uint64_t y);

uint64_t setScale(uint64_t newScale) {
	if (newScale >= MIN_SCALE && newScale <= MAX_SCALE) {
		scale = newScale;
		return 1;
	}
	return 0;
}

uint8_t getScale() {
	return scale;
}

uint16_t getScreenWidth() {
	return VBE_mode_info->width;
}

uint16_t getScreenHeight() {
	return VBE_mode_info->height;
}

void drawPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
	uint8_t *framebuffer = (uint8_t *) (uintptr_t) VBE_mode_info->framebuffer;
	uint64_t offset =
		(x * ((VBE_mode_info->bpp) / 8)) + (y * VBE_mode_info->pitch);
	framebuffer[offset] = (hexColor) & 0xFF;
	framebuffer[offset + 1] = (hexColor >> 8) & 0xFF;
	framebuffer[offset + 2] = (hexColor >> 16) & 0xFF;
}

static void drawScaledPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
	for (int i = 0; i < scale; i++) {
		for (int j = 0; j < scale; j++) {
			drawPixel(hexColor, x + i, y + j);
		}
	}
}

void drawChar(char c, uint32_t charColor, uint32_t bgColor, uint64_t x,
			  uint64_t y) {
	uint8_t *charBitmap = getFontChar(c);

	// The character is not in the font.
	if (charBitmap == NULL)
		return;

	for (int i = 0; i < getFontHeight(); i++) {
		for (int j = 0; j < getFontWidth(); j++) {
			// Check if the bit is set
			if ((charBitmap[i] >> (getFontWidth() - j - 1)) & 1) {
				drawScaledPixel(charColor, x, y);
			}
			else {
				drawScaledPixel(bgColor, x, y);
			}
			x += scale;
		}
		x -= getFontWidth() * scale;
		y += scale;
	}
}

int drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t width,
				  uint64_t height) {
	// No space to draw the rectangle.
	if ((x + width) > getScreenWidth() || (y + height) > getScreenHeight())
		return 0;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			drawPixel(hexColor, x + j, y + i);
		}
	}
	return 1;
}

void clearScreen() {
	drawRectangle(BLACK, 0, 0, getScreenWidth(), getScreenHeight());
}