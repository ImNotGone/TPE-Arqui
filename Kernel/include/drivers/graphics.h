#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <stdint.h>


//https://wiki.osdev.org/VESA_Video_Modes
struct vbe_mode_info_structure {
	uint16_t 	attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t 	window_a;			// deprecated
	uint8_t 	window_b;			// deprecated
	uint16_t 	granularity;		// deprecated; used while calculating bank numbers
	uint16_t 	window_size;
	uint16_t 	segment_a;
	uint16_t 	segment_b;
	uint32_t 	win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t 	pitch;			// number of bytes per horizontal line
	uint16_t 	width;			// width in pixels
	uint16_t 	height;			// height in pixels
	uint8_t 	w_char;			// unused...
	uint8_t 	y_char;			// ...
	uint8_t 	planes;
	uint8_t 	bpp;			// bits per pixel in this mode
	uint8_t 	banks;			// deprecated; total number of banks in this mode
	uint8_t 	memory_model;
	uint8_t 	bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t 	image_pages;
	uint8_t 	reserved0;
 
	uint8_t 	red_mask;
	uint8_t 	red_position;
	uint8_t 	green_mask;
	uint8_t 	green_position;
	uint8_t 	blue_mask;
	uint8_t 	blue_position;
	uint8_t 	reserved_mask;
	uint8_t 	reserved_position;
	uint8_t 	direct_color_attributes;
 
	uint8_t 	*framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t 	off_screen_mem_off;
	uint16_t 	off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t 	reserved1[206];
} __attribute__ ((packed));

typedef struct color {
	uint8_t R;
	uint8_t G;
	uint8_t B; // -> red, green blue color chanells
} gcolor;

typedef struct window {
	uint8_t sx, sy; // -> start x, start y
	uint8_t cx, cy; // -> current x, current y
	uint8_t height, width; // -> window height, window width
	gcolor background, foreground; // window background, window foreground
} gwindow;

void gSetDefaultBackground(gcolor background);
void gSetDefaultForeground(gcolor foreground);

void gPutcharColor(uint8_t c, gcolor background, gcolor foreground);
void gPutchar(uint8_t c);
void gPrintColor(const char * str, gcolor background, gcolor foreground);
void gPrint(const char * str);

void gPrintDec(uint64_t value);
void gPrintHex(uint64_t value);
void gPrintBin(uint64_t value);
void gPrintBase(uint64_t value, uint32_t base);

void gNewline();
void gClear();
void gCursorBlink();
void gBackSpace();

gcolor gGetDefaultBackground();
gcolor gGetDefaultForeground();

int8_t initGraphics();
int8_t divideWindows();
int8_t setWindow(uint8_t window);
#endif//_GRAPHICS_H