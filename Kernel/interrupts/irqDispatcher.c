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

// rutina para manejar la interrupcion del timer
static void int_20() {
	// aumento la cantidad de tiks (1 cada 55ms)
	timer_handler();
	// aproximadamente cada medio segundo actualizo el cursor
	if(ticks_elapsed() % 9 == 0) {
		gCursorBlink();
	}
}

// rutina para manejar la interrupcion del teclado
static void int_21() {
	keyboard_handler();
}