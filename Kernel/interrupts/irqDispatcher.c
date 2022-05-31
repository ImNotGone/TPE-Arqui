#include <interrupts/time.h>
#include <drivers/keyboard.h>
#include <stdint.h>
#include <drivers/graphics.h>

static void int_20();
static void int_21();

typedef void (*TIrqHandler)();

TIrqHandler irqHandlers[] = {
	// 0 -> time
	int_20,
	// 1 -> keyboard
	int_21	
};

static uint8_t irqHandlersDim = sizeof(irqHandlers)/sizeof(irqHandlers[0]);

void irqDispatcher(uint64_t irq) {
	if(irq >= irqHandlersDim) 
		return;
	// function call
	irqHandlers[irq]();
	return;
}

static void int_20() {
	timer_handler();
	// cada medio segundo actualizo el cursor
	if(ticks_elapsed() % 9 == 0) {
		gCursorBlink();
	}
	/*
	static long secondsElapsed = 0;
	if(ticks_elapsed() % 90 == 0) {
		ncPrint("segundos: ");
		ncPrintDec(ticks_elapsed()/18);
		ncNewline();
	}
	*/
}

static void int_21() {
	keyboard_handler();
}