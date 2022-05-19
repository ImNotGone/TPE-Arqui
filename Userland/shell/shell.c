#include <stdint.h>
#include <_stdio.h>
#include <_string.h>

static void init();
static void command_listener();
static void help();
static void inforeg();
static void zerodiv();
static void invalid_opcode();
static void printmem();
static void time();
static void primes();
static void fibo();
static void screenDiv();

typedef void (*fp)(void);

typedef struct command {
	char * name;
	char * desc;
	fp exec;
} command;

static command commands[] = {
	{"help", "shows all available commands", help},
	{"inforeg", "prints register snapshot", inforeg},
	{"zerodiv", "generates a zero divition exeption", zerodiv},
	{"invalid_opcode", "generates an invalid operation exception", invalid_opcode},
	{"printmem", "prints the 32 bytes which follow the recieved address", printmem},
	{"time", "prints the current system time", time},
	{"primes", "prints primes", primes},
	{"fibo", "prints the fibonacci series",fibo},
	{"|", "allows to divide the screen and run 2 programms", screenDiv}
};

static int commandsDim = sizeof(commands)/sizeof(commands[0]);

int main() {
	char c[4];


	init();
	while(1) {
		putchar('>');
		command_listener();
	}
	return 0;
}


static void init() {
	char str[] = "bienvenido a la consola\n";
	printf(str);
}

static void command_listener() {
	static char commandBuffer[128];
	int c;
	int i = 0;
	while((c = getchar()) != '\n' && i < 128) {
		if(c == '\b') {
			if(i > 0) {
 				putchar(c);
				i--;
			}
		} 
		if(c != '\b') {
			putchar(c);
			commandBuffer[i++] = c;
		}
	}
	commandBuffer[i] = 0;
	putchar('\n');
	for (int i = 0; i < commandsDim; i++) {
		if (strcmp(commandBuffer, commands[i].name) == 0) {
			commands[i].exec();
			return;
		}
	}
	printf("%s comando invalido\n", commandBuffer);
}
static void help() {
	puts("Los comandos disponibles son:");
	for(int i = 0; i < commandsDim; i++) {
		printf("%d) %s => %s\n", i, commands[i].name, commands[i].desc);
	}
}
static void inforeg() {return;}
static void zerodiv() {return;}
static void invalid_opcode() {return;}
static void printmem() {return;}
static void time() {return;}
static void primes() {return;}
static void fibo() {return;}
static void screenDiv() {return;}