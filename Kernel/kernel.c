#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <drivers/graphics.h>
#include <drivers/RTC.h>
#include <drivers/keyboard.h>
#include <interrupts/time.h>
#include <interrupts/idtLoader.h>
#include <interrupts/syscalls.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const shellAddress = (void *)0x400000;

typedef int (*EntryPoint)();

extern void _hlt();
static void printLogo();

void clearBSS(void *bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					- sizeof(uint64_t)					  // Begin at the top of the stack
	);
}

void * initializeKernelBinary() {
	// char buffer[10];

	// gPrint("[x64BareBones]");
	// gNewline();

	// gPrint("CPU Vendor:");
	// gPrint(cpuVendor(buffer));
	// gNewline();

	// gPrint("[Loading modules]");
	// gNewline();
	void *moduleAddresses[] = {
		shellAddress,
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	// gPrint("[Done]");
	// gNewline();
	// gNewline();

	// gPrint("[Initializing kernel's binary]");
	// gNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	// gPrint("  text: 0x");
	// gPrintHex((uint64_t)&text);
	// gNewline();
	// gPrint("  rodata: 0x");
	// gPrintHex((uint64_t)&rodata);
	// gNewline();
	// gPrint("  data: 0x");
	// gPrintHex((uint64_t)&data);
	// gNewline();
	// gPrint("  bss: 0x");
	// gPrintHex((uint64_t)&bss);
	// gNewline();

	// gPrint("[Done]");
	// gNewline();
	// gNewline();
	load_idt();
	initGraphics();
	return getStackBase();
}

int init_shell() {
	return ((EntryPoint)shellAddress)();
}

int main() {
	static int booted = 0;
	
	// si es estoy recuperandome de una excepcion voy derecho a la shell
	// de esta manera la shell pude ejecutar el pipe y aparenta ejecutar
	// el programa de primos o fibonacci mientras en la otra pantalla 
	// hay una exepcion
	if (booted) {
		init_shell();
	}

	printLogo();
	gPrint("Presione enter para acceder a la consola");
	char c;
	do {
		// freno la ejecucion del programa hasta la siguiente interrupcion
		_hlt();
		// lo sigo haciendo hasta que la interrupccion sea debido a un enter
	} while ((c = getchar()) != '\n');
	gClear();
	booted = 1;
	init_shell();

	while (1);
	return 0;
}

#define LOGO_WIDTH	4
#define LOGO_HEIGHT	5
static void printLogo() {
	// https://patorjk.com/software/taag/#p=testall&f=Cosmike&t=BYTE
	/*
	"  _____  ____________
	  / _ ) \/ /_  __/ __/
	 / _  |\ \/ / / / _/
	/____/ /_/ /_/ /___/
	"
	*/
	gcolor C1 = {96, 60, 20};
	gcolor C2 = {156, 39, 6};
	gcolor C3 = {212, 91, 18};
	gcolor C4 = {243, 188, 46};
	gcolor BLACK = {0x00, 00, 00};
	gcolor colors[] = {C1, C2, C3, C4};
	char * logo[LOGO_HEIGHT][LOGO_WIDTH] = {
		{"      /\\/\\/\\/\\/\\    ", "/\\/\\    /\\/\\  ", "/\\/\\/\\/\\/\\/\\  ", "/\\/\\/\\/\\/\\/\\ \n"},
		{"     /\\/\\    /\\/\\  ", "/\\/\\    /\\/\\      ", "/\\/\\      ", "/\\            \n"}, 
		{"    /\\/\\/\\/\\/\\      ", "/\\/\\/\\/\\        ", "/\\/\\      ", "/\\/\\/\\/\\/\\     \n",}, 
		{"   /\\/\\    /\\/\\      ", "/\\/\\          ", "/\\/\\      ", "/\\/\\            \n"},
		{"  /\\/\\/\\/\\/\\        ", "/\\/\\          ", "/\\/\\      ", "/\\/\\/\\/\\/\\/\\     \n"}
	};

	for(int i = 0; i < LOGO_HEIGHT; i++) {
		for(int j = 0; j < LOGO_WIDTH; j++) {
			gPrintColor(logo[i][j], BLACK, colors[j]);

		}
	}
	gNewline();
	return;
}
