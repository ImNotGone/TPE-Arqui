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

void clearBSS(void *bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void *getStackBase()
{
	return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					- sizeof(uint64_t)					  // Begin at the top of the stack
	);
}

void *initializeKernelBinary()
{
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

int init_shell()
{
	return ((EntryPoint)shellAddress)();
}

int main()
{
	// gPrint("[Kernel Main]");
	// gNewline();
	// gPrint("  Sample code module at 0x");
	// gPrintHex((uint64_t)sampleCodeModuleAddress);
	// gNewline();
	// gPrint("  Calling the sample code module returned: ");
	// gPrintHex(((EntryPoint)sampleCodeModuleAddress)());
	// gNewline();
	// gNewline();

	// gPrint("  Sample data module at 0x");
	// gPrintHex((uint64_t)sampleDataModuleAddress);
	// gNewline();
	// gPrint("  Sample data module contents: ");
	// gPrint((char*)sampleDataModuleAddress);
	// gNewline();

	// gPrint("[Finished]");
	// gNewline();
	// gPrintAtt("ARQUITECTURA DE LAS COMPUTADORAS", (WHITE << 4) | C1);

	/*
		gNewline();
		gPrintDec(getRTCHours());
		gPrintChar(':');
		gPrintDec(getRTCMinutes());
		gPrintChar(':');
		gPrintDec(getRTCSeconds());
		gNewline();
		gPrintDec(getRTCDayOfMonth());
		gPrintChar('/');
		gPrintDec(getRTCMonth());
		gPrintChar('/');
		gPrintDec(getRTCYear());
	*/

	// codigo de teclado
	/*
	gClear();

	gNewline();
	gPrint("esperando tecla");

	char c=getchar();
	gNewline();
	gPrint("tecla Recibida");
	gNewline();
*/
	/*
	gPrintHex(c);
	gPrintChar(getchar());
	for (int j =1 ; j < 1000 ;j++) {
		//gPrintHex(getKey());
		gPrintChar(getchar());
	}
	*/
	// char c[128];
	// syscallDispatcher(0, 0, c, 6, 0, 0);
	// gNewline();
	// syscallDispatcher(1, 1, c, 6, 0, 0);
	// Ttime t;
	// syscallDispatcher(2, (uint64_t)&t, 0, 0, 0, 0);
	static int booted = 0;
	if (booted)
	{
		init_shell();
	}
	// https://patorjk.com/software/taag/#p=testall&f=Cosmike&t=BYTE
	/*
"   _____  ____________
  / _ ) \\/ /_  __/ __/
 / _  |\\  / / / / _/
/____/ /_/ /_/ /___/
"
*/
	gcolor C1 = {96, 60, 20};
	gcolor C2 = {156, 39, 6};
	gcolor C3 = {212, 91, 18};
	gcolor C4 = {243, 188, 46};
	gcolor BLACK = {0x00, 00, 00};
	gPrintColor("      /\\/\\/\\/\\/\\    ", BLACK, C1);
	gPrintColor("/\\/\\    /\\/\\  ", BLACK, C2);
	gPrintColor("/\\/\\/\\/\\/\\/\\  ", BLACK, C3);
	gPrintColor("/\\/\\/\\/\\/\\/\\ \n", BLACK, C4);
	gPrintColor("     /\\/\\    /\\/\\  ", BLACK, C1);
	gPrintColor("/\\/\\    /\\/\\      ", BLACK, C2);
	gPrintColor("/\\/\\      ", BLACK, C3);
	gPrintColor("/\\            \n", BLACK, C4);
	gPrintColor("    /\\/\\/\\/\\/\\      ", BLACK, C1);
	gPrintColor("/\\/\\/\\/\\        ", BLACK, C2);
	gPrintColor("/\\/\\      ", BLACK, C3);
	gPrintColor("/\\/\\/\\/\\/\\     \n", BLACK, C4);
	gPrintColor("   /\\/\\    /\\/\\      ", BLACK, C1);
	gPrintColor("/\\/\\          ", BLACK, C2);
	gPrintColor("/\\/\\      ", BLACK, C3);
	gPrintColor("/\\/\\            \n", BLACK, C4);
	gPrintColor("  /\\/\\/\\/\\/\\        ", BLACK, C1);
	gPrintColor("/\\/\\          ", BLACK, C2);
	gPrintColor("/\\/\\      ", BLACK, C3);
	gPrintColor("/\\/\\/\\/\\/\\/\\     \n", BLACK, C4);
	gPrint("\n");
	gPrint("Presione enter para acceder a la consola");
	char c;
	do
	{
		// freno la ejecucion del programa hasta un enter
		_hlt();
	} while ((c = getchar()) != '\n');
	gClear();
	booted = 1;
	init_shell();
	// testWrite();
	// testRead();
	// gPrint("Sali del read");
	// testWrite();
	while (1)
		;
	return 0;
}
