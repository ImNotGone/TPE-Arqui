#include <drivers/naiveConsole.h>

#define DEFAULTCOLOR (BLACK << 4 | WHITE)
#define WCURSOR (WHITE << 4)
#define BCURSOR (BLACK << 4)
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

static char buffer[64] = { '0' };
static const uint32_t width = 80;
static const uint32_t height = 25;
static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * currentVideo = (uint8_t*)0xB8000;
static uint8_t * endOfVideo	= (uint8_t*)0xB8000+80*25*2;

void ncCursorBlink() {
	currentVideo[1] = currentVideo[1]? BCURSOR:WCURSOR;
}

void ncSetCursor(uint8_t color) {
	currentVideo[1] = color;
}

void ncBackSpace() {
	ncSetCursor(DEFAULTCOLOR);
	if(currentVideo != video) {
		*(--currentVideo) = DEFAULTCOLOR; 
		*(--currentVideo) = ' ';
	}
	ncSetCursor(WCURSOR);
}

void ncScrollUp() {
	int i;
	for(i = 0; i < (height - 1) * width; i++) {
		int j = i + width;
		video[i*2] = video[j*2];
		video[i*2+1] = video[j*2+1];
	}
	for(;i < height * width; i++) {
		video[i*2] = ' ';
		video[i*2+1] = DEFAULTCOLOR;
	}
	currentVideo -= width*2;
}

void ncPrintAtt(const char * string, char att) {
	int i;

	for (i = 0; string[i] != 0; i++)
		ncPrintCharAtt(string[i], att);
}

void ncPrintCharAtt(char character, char att) {
	*currentVideo++ = character;
	*currentVideo++ = att;
	if(currentVideo == endOfVideo) {
		ncScrollUp();
	}
	ncSetCursor(WCURSOR);
}

void ncPrint(const char * string)
{
	int i;

	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

void ncPrintChar(char character)
{
	ncPrintCharAtt(character, DEFAULTCOLOR);	
}

void ncNewline()
{
	do
	{
		ncPrintChar(' ');
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    ncPrint(buffer);
}

void ncClear()
{
	int i;

	for (i = 0; i < height * width; i++) {
		video[i * 2] = ' ';
		video[i * 2 + 1] = DEFAULTCOLOR;
	}
	currentVideo = video;
}

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}
