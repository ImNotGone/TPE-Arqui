#include <drivers/graphics.h>
#include <drivers/font.h>

// si queremos que no se inicien en bss tendriamos que no iniciarlos en 0
static gwindow windows[2]; // -> pantallas
static uint8_t currentWindow = 0;
static uint8_t cantWindows = 0;
static char buffer[64] = { '0' };

static const struct vbe_mode_info_structure * graphicsInfo = (struct vbe_mode_info_structure *) 0x5C00;
static uint8_t fontHeight;
static uint8_t fontWidth;
static uint8_t cursorActive = 0;
// BLACK
static gcolor DEFAULT_BACKGROUND = {0x00, 0x00, 0x00};
// WHITE
static gcolor DEFAULT_FOREGROUND = {0xFF,0xFF,0xFF};

static uint32_t uintToBase(uint64_t value, char * buff, uint32_t base);

#define COLORCHANELLS   3
#define CHAR_CURSOR     '_'

static uint8_t * getPixel(int x, int y) {
    return (graphicsInfo->framebuffer+3*(graphicsInfo->width*y+x));
}

static void drawPixel(int x, int y, gcolor color){
    uint8_t * pixel = getPixel(x, y);
    pixel[0] = color.B;
    pixel[1] = color.G;
    pixel[2] = color.R;
}

static void gUpdatePos() {
    windows[currentWindow].cy += ((windows[currentWindow].cx + 1) == windows[currentWindow].width)?1:0;
    windows[currentWindow].cx = (windows[currentWindow].cx + 1)% windows[currentWindow].width;
}

static void scrollUp(){
    // itero en las filas
    uint8_t * filaActual;
    uint8_t * filaSig;
    int i;
    for(i = 0; i < (windows[currentWindow].height - 1) * fontHeight; i++){

        // me guardo la fila actual y la siguiente
        filaActual = getPixel(windows[currentWindow].sx * fontWidth, windows[currentWindow].sy + i);
        filaSig  = getPixel(windows[currentWindow].sx * fontWidth, windows[currentWindow].sy + fontHeight + i);

        // itero por las columnas y copio la informacion 
        for(int j=0; j < windows[currentWindow].width * fontWidth * COLORCHANELLS ; j++){
            filaActual[j] = filaSig[j];
        }
    }
    // limpio la ultima linea
    for(;i < (windows[currentWindow].height) * fontHeight; i++) {
        filaActual = getPixel(windows[currentWindow].sx * fontWidth, windows[currentWindow].sy + i);
        for(int j=0; j < windows[currentWindow].width * fontWidth * COLORCHANELLS; j+=COLORCHANELLS){
            filaActual[j]   = DEFAULT_BACKGROUND.B;
            filaActual[j+1] = DEFAULT_BACKGROUND.G;
            filaActual[j+2] = DEFAULT_BACKGROUND.R;
        }
    }
    windows[currentWindow].cy -= 1;
}

static void gSetCursor(gcolor color) {
    if(!cursorActive) return;
    gPutcharColor(CHAR_CURSOR, windows[currentWindow].background, color);
    if(windows[currentWindow].cx == 0){
        // subo una fila
        windows[currentWindow].cy -= 1;
        // me paro en la ultima posicion de la fila                    
        windows[currentWindow].cx = windows[currentWindow].width-1;
        return;
    }
    windows[currentWindow].cx = (windows[currentWindow].cx-1) % windows[currentWindow].width;
}

void gSetDefaultBackground(gcolor background) {
    windows[currentWindow].background = background;
}
void gSetDefaultForeground(gcolor foreground) {
    windows[currentWindow].foreground = foreground;
}

gcolor gGetDefaultBackground() {
    return windows[currentWindow].background;
}
gcolor gGetDefaultForeground() {
    return windows[currentWindow].foreground;
}


int8_t initGraphics() {
    cursorActive = 0;
    currentWindow = 0;
    cantWindows = 1;
    fontHeight = getFontHeight();
    fontWidth = getFontWidth();
    gwindow aux = {
        .sx = 0,
        .sy = 0,
        .cx = 0,
        .cy = 0,
        .height = graphicsInfo->height/fontHeight,
        .width  = graphicsInfo->width/fontWidth,
        .background = DEFAULT_BACKGROUND,
        .foreground = DEFAULT_FOREGROUND,
    };
    windows[currentWindow] = aux;
    gClear();
    cursorActive = 1;
    return 1;
}


void gClearAll() {
    for (int i = 0; i < graphicsInfo->height; i++) {
        for (int j = 0; j < graphicsInfo->width; j++)
            drawPixel(j, i, DEFAULT_BACKGROUND);
    }
}

int8_t divideWindows() {
    cursorActive = 0;
    gClearAll();
    currentWindow = 0;
    cantWindows = 2;
    fontHeight = getFontHeight();
    fontWidth = getFontWidth();
    gwindow aux = {
        .sx = 0,
        .sy = 0,
        .cx = 0,
        .cy = 0,
        .height = graphicsInfo->height/fontHeight,
        .width  = graphicsInfo->width/(fontWidth*2),
        .background = DEFAULT_BACKGROUND,
        .foreground = DEFAULT_FOREGROUND,
    };
    windows[currentWindow] = aux;
    currentWindow = 1;
    windows[currentWindow] = aux;
    windows[currentWindow].width -= 1;
    windows[currentWindow].sx = graphicsInfo->width/(fontWidth*2) + 1;
    currentWindow = 0;

    // linea de division
    int x = (graphicsInfo->width + fontWidth)/2;
    for(int y = 0; y < graphicsInfo->height; y++) {
        drawPixel(x, y, DEFAULT_FOREGROUND);            
    }
    cursorActive = 1;
    return 1;
}

int8_t setWindow(uint8_t window) {
    if(window > cantWindows) return -1;
    currentWindow = window;
    return 1;
}

void gPutcharColor(uint8_t c, gcolor background, gcolor foreground) {
    switch (c) {
        case '\b': gBackSpace(); return;
        case '\n': gNewline(); return;
    }

    if(windows[currentWindow].cy == windows[currentWindow].height)
        scrollUp();
    
    const uint8_t * bitmap = getcharFont(c);
    // current bit y
    uint16_t cby = (windows[currentWindow].sy + windows[currentWindow].cy)*fontHeight;
    // current bit x
    uint16_t cbx = (windows[currentWindow].sx + windows[currentWindow].cx)*fontWidth;
    uint16_t aux = cbx;
    uint8_t mask;
    for(int i = 0; i < fontHeight; i++, cby++) {
        mask = 0x80;
        cbx = aux;    
        for(int j = 0; j < fontWidth; j++, mask >>= 1, cbx++) {
            drawPixel(cbx, cby, (bitmap[i] & mask)?foreground:background);
        }
    }
    
    gUpdatePos();
    cursorActive = 1;
}

void gPutchar(uint8_t c) {
    gPutcharColor(c, windows[currentWindow].background, windows[currentWindow].foreground);
}

void gPrintColor(const char * str, gcolor background, gcolor foreground) {
    for(int i = 0; str[i] != 0; i++) {
        gPutcharColor(str[i], background, foreground);
    }
}

void gPrint(const char * str) {
    gPrintColor(str, windows[currentWindow].background, windows[currentWindow].foreground);
}

void gNewline() {
    gSetCursor(windows[currentWindow].background);
    windows[currentWindow].cx = 0;
    windows[currentWindow].cy += 1;
    gSetCursor(windows[currentWindow].foreground);
}

void gPrintDec(uint64_t value)
{
	gPrintBase(value, 10);
}

void gPrintHex(uint64_t value)
{
	gPrintBase(value, 16);
}

void gPrintBin(uint64_t value)
{
	gPrintBase(value, 2);
}

void gPrintBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    gPrint(buffer);
}

void gClear()
{
    windows[currentWindow].cx = 0;
    windows[currentWindow].cy = 0;
    for(int i=0; i < windows[currentWindow].height ; i++){
        for(int j=0; j < windows[currentWindow].width ; j++){
            // current bit y
            uint16_t cby = (windows[currentWindow].sy + windows[currentWindow].cy)*fontHeight;
            // current bit x
            uint16_t cbx = (windows[currentWindow].sx + windows[currentWindow].cx)*fontWidth;
            uint16_t aux = cbx;
            for(int k = 0; k < fontHeight; k++, cby++) {
                cbx = aux;
                for(int l = 0; l < fontWidth; l++, cbx++) {
                    drawPixel(cbx, cby, windows[currentWindow].background);
                }
            }
            gUpdatePos();
        }
    }
    windows[currentWindow].cx = 0;
    windows[currentWindow].cy = 0;
    cursorActive = 1;
	gSetCursor(windows[currentWindow].foreground);
}


static uint32_t uintToBase(uint64_t value, char * buff, uint32_t base)
{
	char *p = buff;
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

	// Terminate string in buff.
	*p = 0;

	//Reverse string in buff.
	p1 = buff;
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

void gCursorBlink() {
    gcolor foreground[] = {windows[currentWindow].background, windows[currentWindow].foreground};
    static int select = 0;
    gSetCursor(foreground[select++]);
    select = select % 2;
}

static void doBackSpace() {
    if(windows[currentWindow].cx == 0 && windows[currentWindow].cy == 0){
        return;
    }

    // la columna es 0 -> la fila != 0
    if(windows[currentWindow].cx == 0){
        // subo una fila
        windows[currentWindow].cy -= 1;
        // me paro en la ultima posicion de la fila                    
        windows[currentWindow].cx = windows[currentWindow].width-1;
        // imprimo un "blank"
        gPutcharColor(' ', windows[currentWindow].background, windows[currentWindow].foreground);
        // me vuelvo a mover para atras
        // subo una fila
        windows[currentWindow].cy -= 1;
        // me paro en la ultima posicion de la fila   
        windows[currentWindow].cx = windows[currentWindow].width-1;
        return;
    }

    // la fila es 0 -> la columna != 0
    windows[currentWindow].cx = (windows[currentWindow].cx-1) % windows[currentWindow].width;
    gPutcharColor(' ', windows[currentWindow].background, windows[currentWindow].foreground);
    windows[currentWindow].cx = (windows[currentWindow].cx-1) % windows[currentWindow].width;
}

void gBackSpace() {
	gSetCursor(windows[currentWindow].background);
    doBackSpace();
	gSetCursor(windows[currentWindow].foreground);
}