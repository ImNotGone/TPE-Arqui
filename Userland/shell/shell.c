#include <stdint.h>
#include <stdlib.h>

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

typedef void (*fp)(void);

typedef struct command {
	char * name;
	fp exec;
} command;

static command commands[] = {
	{"help", help},
	{"inforeg", inforeg},
	{"zerodiv", zerodiv},
	{"invalid_opcode", invalid_opcode},
	{"printmem", printmem},
	{"time", time},
	{"primes", primes},
	{"fibo", fibo},
	// {"|", }
};

static commandsDim = sizeof(commands)/sizeof(commands[0]);

int main() {
	init();
	while(1) {
		putchar('>');
		command_listener();
	}
}


static void init() {
	char * str = "bienvenido a la consola\n";
	int i = 0;
	while(str[i]) {
		putchar(str[i++]);
	}
}

static void command_listener() {
	while (1);
}
static void help() {return;}
static void inforeg() {return;}
static void zerodiv() {return;}
static void invalid_opcode() {return;}
static void printmem() {return;}
static void time() {return;}
static void primes() {return;}
static void fibo() {return;}