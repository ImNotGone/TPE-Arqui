#include <drivers/keyboard.h>

#define BUFFER_SIZE     1024
#define LSHIFT          0x2A
#define RSHIFT          0x36
#define BMAYUS          0x3A
#define RCNTRL          0xE01D
#define LCNTRL          0x1D
#define UNPRESSED_BIT   0x80
#define KEY_BITS_MASK   0x7F
#define LOWERMAP        0
#define UPPERMAP        1
#define KEY_VALUE(key)  ((key) & KEY_BITS_MASK)
#define IS_PRESSED(key) (((key) & UNPRESSED_BIT) == 0)
#define IS_SHIFT(key)   (KEY_VALUE(key) == LSHIFT || KEY_VALUE(key) == RSHIFT)
#define IS_BMAYUS(key)  (KEY_VALUE(key) == BMAYUS)
#define IS_CONTROL(key) (KEY_VALUE(key) == LCNTRL || KEY_VALUE(key) == RCNTRL)
#define TO_UPPER(key)   (('a' <= (key) && (key) <= 'z')?(key) - 'a' + 'A':(key))
#define TO_LOWER(key)   (('A' <= (key) && (key) <= 'Z')?(key) - 'A' + 'a':(key))
#define IS_ALPHA(key)   ('A' <= TO_UPPER(key) && TO_UPPER(key) <= 'Z')
#define ESC 0x01
#define F1  0x3B
#define F2  0x3C
#define F3  0x3D
#define F4  0x3E
#define F5  0x3F
#define F6  0x40
#define F7  0x41
#define F8  0x42
#define F9  0x43
#define F10 0x44

#define SAVE_REGS_KEY 's'

/*
#define F11 0x45
#define F12 0x46
*/

// ONLY PRINTABLE
static uint8_t lowerMapping[] = {
      0,  0/*27*/, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
   '\b', 0/*'\t'*/, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
   '\n',    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';','\'', '~',
      0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',
      0,  ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,    0,  0/*38*/,   0, '-',  0/*37*/,   0,  0/*39*/, '+',   0,  0/*40*/,   0,   0,   0,
      0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

static uint8_t upperMapping[] = {
      0,  0/*27*/, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
   '\b', 0/*'\t'*/, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
   '\n',    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':','\"', '~',
      0,  '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0, '*',
      0,  ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,    0,   0,   0, '-',   0,   0,   0, '+',   0,   0,   0,   0,   0,
      0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

static uint8_t * keyMapping[] = {lowerMapping, upperMapping};

// empiezo en lowerMapping
static uint8_t map = LOWERMAP;
static uint8_t keyBuffer[BUFFER_SIZE];
static uint8_t bufferFirst = 0;
static uint8_t bufferLast = 0;
static uint8_t bufferElementCount = 0;
static uint8_t blockMayus = 0;
static uint8_t cntrlPressed = 0;
static uint8_t inforegFlag = 0;

static uint8_t handlekey(uint8_t key) {
    if(cntrlPressed && (IS_PRESSED(key)) && SAVE_REGS_KEY == keyMapping[map][KEY_VALUE(key)]) {
        inforegFlag = 1;
        return 0;
    }
    inforegFlag = 0;
    if(IS_CONTROL(key)) {
        cntrlPressed = (IS_PRESSED(key));
        return 0;
    }
    if(IS_SHIFT(key)) {
        map = (IS_PRESSED(key));
        return 0;
    }
    if(IS_BMAYUS(key) && IS_PRESSED(key)) {
        blockMayus = !blockMayus;
        return 0;
    }
    if(!IS_PRESSED(key) || keyMapping[map][key] == 0) return 0;    
    key = keyMapping[map][key];
    if(IS_ALPHA(key) && blockMayus) {
        key = (map == LOWERMAP)? TO_UPPER(key):TO_LOWER(key);
    }
    return key;
}

extern uint8_t sys_getKey();

uint8_t getKey() {
    uint8_t out;
    do {
        out = handlekey(sys_getKey());
    } while (out == 0);    
    return out;
}

int bufferIsEmpty() {
    return bufferElementCount == 0;
}

int bufferIsFull() {
    return bufferElementCount == BUFFER_SIZE;
}

static void appendInBuffer(uint8_t key) {
    // err
    if(bufferIsFull()) {
        //exep?
    }
    keyBuffer[bufferLast++] = key;
    bufferLast = bufferLast % BUFFER_SIZE;
    bufferElementCount++;
    return;
}

static uint8_t getNextInBuffer() {
    if(bufferIsEmpty()) {
        return 0;
    }
    bufferElementCount--;
    uint8_t out = keyBuffer[bufferFirst++];
    bufferFirst = bufferFirst % BUFFER_SIZE;
    return out;
}

void keyboard_handler() {
    uint8_t out = handlekey(sys_getKey());
    if(out == 0) 
        return;
    appendInBuffer(out);
}

// TODO revisar el return value
uint8_t getchar() {
    return getNextInBuffer();
}

uint8_t mustUpdateInforeg() {
    return inforegFlag;
}