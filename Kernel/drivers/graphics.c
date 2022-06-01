#include <drivers/graphics.h>
#include <drivers/font.h>

static gwindow windows[2]; // -> pantallas
static int currentWindow = 0;
static char buffer[64] = { '0' };

static const struct vbe_mode_info_structure * graphicsInfo = (struct vbe_mode_info_structure *) 0x5C00;
static uint8_t fontHeight;
static uint8_t fontWidth;
static int cursorActive = 0;
// BLACK
static gcolor DEFAULT_BACKGROUND = {0x00, 0x00, 0x00};
// WHITE
static gcolor DEFAULT_FOREGROUND = {0xFF,0xFF,0xFF};

static uint32_t uintToBase(uint64_t value, char * buff, uint32_t base);

static void replaceRow(uint8_t *filaActual, const uint8_t *filaSig);

#define COLORCHANELLS 3

static uint8_t * getPixel(int x, int y) {
    return (uint8_t *) (graphicsInfo->framebuffer+3*(graphicsInfo->width*y+x));
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
    for(i = 0; i < (windows[currentWindow].height) * fontHeight - 1; i++){

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
        filaActual = getPixel(windows[currentWindow].sx, windows[currentWindow].sy + i);
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
    gPutcharColor('_', DEFAULT_BACKGROUND, color);
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
    DEFAULT_BACKGROUND = background;
}
void gSetDefaultForeground(gcolor foreground) {
    DEFAULT_FOREGROUND = foreground;
}

void initGraphics() {
    currentWindow = 0;
    fontHeight = getFontHeight();
    fontWidth = getFontWidth();
    gwindow aux = {
        .sx = 0,
        .sy = 0,
        .cx = 0,
        .cy = 0,
        .height = graphicsInfo->height/fontHeight,
        .width  = graphicsInfo->width/fontWidth,
    };
    windows[currentWindow] = aux;
    gClear();
    cursorActive = 1;
}

void gClearAll() {
    for (int i = 0; i < graphicsInfo->height; i++) {
        for (int j = 0; j < graphicsInfo->width; j++)
            drawPixel(j, i, DEFAULT_BACKGROUND);
    }
}

void divideWindows() {
    gClearAll();
    currentWindow = 0;
    fontHeight = getFontHeight();
    fontWidth = getFontWidth();
    gwindow aux = {
        .sx = 0,
        .sy = 0,
        .cx = 0,
        .cy = 0,
        .height = graphicsInfo->height/fontHeight,
        .width  = graphicsInfo->width/(fontWidth*2),
    };
    // limpio todo
    windows[currentWindow] = aux;
    currentWindow = 1;
    windows[currentWindow] = aux;
    windows[currentWindow].width -= 1;
    windows[currentWindow].sx = graphicsInfo->width/(fontWidth*2) + 1;
    
    currentWindow = 0;
    // linea de division
    int x = graphicsInfo->width + fontWidth;
    x /= 2;
    for(int y = 0; y < graphicsInfo->height; y++) {
        drawPixel(x, y, DEFAULT_FOREGROUND);            
    }
    cursorActive = 1;
}

void switchWindow() {
    currentWindow = (currentWindow+1) %2;
}

void gPutcharColor(uint8_t c, gcolor background, gcolor foreground) {
    switch (c) {
        case '\b': gBackSpace(); return;
        case '\n': gNewline(); return;
    }

    if(windows[currentWindow].cy == windows[currentWindow].height)
        scrollUp();
    
    uint8_t * bitmap = getcharFont(c);
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
    gPutcharColor(c, DEFAULT_BACKGROUND, DEFAULT_FOREGROUND);
}

void gPrintColor(const char * str, gcolor background, gcolor foreground) {
    for(int i = 0; str[i] != 0; i++) {
        gPutcharColor(str[i], background, foreground);
    }
}

void gPrint(const char * str) {
    gPrintColor(str, DEFAULT_BACKGROUND, DEFAULT_FOREGROUND);
}

void gNewline() {
    gSetCursor(DEFAULT_BACKGROUND);
    windows[currentWindow].cx = 0;
    windows[currentWindow].cy += 1;
    gSetCursor(DEFAULT_FOREGROUND);
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
                    drawPixel(cbx, cby, DEFAULT_BACKGROUND);
                }
            }
            gUpdatePos();
        }
    }
    windows[currentWindow].cx = 0;
    windows[currentWindow].cy = 0;
    cursorActive = 1;
	gSetCursor(DEFAULT_FOREGROUND);
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
    gcolor foreground[] = {DEFAULT_BACKGROUND, DEFAULT_FOREGROUND};
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
        gPutcharColor(' ', DEFAULT_BACKGROUND, DEFAULT_FOREGROUND);
        // me vuelvo a mover para atras
        // subo una fila
        windows[currentWindow].cy -= 1;
        // me paro en la ultima posicion de la fila   
        windows[currentWindow].cx = windows[currentWindow].width-1;
        return;
    }

    // la fila es 0 -> la columna != 0
    windows[currentWindow].cx = (windows[currentWindow].cx-1) % windows[currentWindow].width;
    gPutcharColor(' ', DEFAULT_BACKGROUND, DEFAULT_FOREGROUND);
    windows[currentWindow].cx = (windows[currentWindow].cx-1) % windows[currentWindow].width;
}

void gBackSpace() {
	gSetCursor(DEFAULT_BACKGROUND);
    doBackSpace();
	gSetCursor(DEFAULT_FOREGROUND);
}