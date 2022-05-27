#include <interrupts/exceptions.h>
#include <drivers/keyboard.h>
#include <drivers/naiveConsole.h>

#define NULL (void *)0
typedef void (*fp)(void);

static void zero_division();
static void invalidopcode();
static void reserved();

extern void restartKernel();
extern void _hlt();
typedef struct exception {
	char * errmsg;
	fp handle;
} texception;

texception exceptions[] = {
	// 0x0
	{"0 div err", zero_division},
	// 0x1
	{"", reserved},
	// 0x2
	{"", reserved},
	// 0x3
	{"", reserved},
	// 0x4
	{"", reserved},
	// 0x5
	{"", reserved},
	// 0x6
	{"Invalid Opcode", invalidopcode},

};

static void excepHandler(fp excep);

void exceptionDispatcher(int exception, uint64_t exceptionRegisters[18]) {
	static char * registers[] = {
		"rax   : ",
		"rbx   : ",
		"rcx   : ",
		"rdx   : ",
		"rsi   : ",
		"rdi   : ",
		"rbp   : ",
		"rsp   : ",
		"r8    : ",
		"r9    : ",
		"r10   : ",
		"r11   : ",
		"r12   : ",
		"r13   : ",
		"r14   : ",
		"r15   : ",
		"rip   : ",
		"rflags: ",
	};
	ncPrint("Exception ");
	ncPrintDec(exception);
	ncNewline();
	ncPrint(exceptions[exception].errmsg);
	ncNewline();
	for(int i = 0; i < 18; i++) {
		ncPrint(registers[i]);
		ncPrintHex(exceptionRegisters[i]);
		ncNewline();
	}
	excepHandler(exceptions[exception].handle);
}

static void excepHandler(fp handler){
	handler();
	ncPrint("Presione enter para continuar");
	int c;
	do{
		// freno la ejecucion del programa hasta un enter
		_hlt();
	} while((c=getchar()) != '\n');
	ncClear();
	restartKernel();
}

static void zero_division() {
	return;
}

static void invalidopcode() {
	return;
}

static void reserved() {
	return;
}