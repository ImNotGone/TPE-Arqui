#include <stdint.h>
#include <dumps.h>
#include <time.h>
#include <_stdio.h>
#include <_string.h>
#include <syscalls.h>

#define UINT_MAX 4294967295

static void init();
static void command_listener();
static void help();
static void inforeg();
extern void zerodiv();
extern void invalidopcode();
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
	{"inforeg", "prints register snapshot, take a snapshot using \'cntrl + s\'", inforeg},
	{"zerodiv", "generates a zero divition exeption", zerodiv},
	{"invalid_opcode", "generates an invalid operation exception", invalidopcode},
	{"printmem", "prints the 32 bytes which follow the recieved address", printmem},
	{"time", "prints the current system time", time},
	{"primes", "prints primes", primes},
	{"fibo", "prints the fibonacci series",fibo},
	{"|", "allows to divide the screen and run 2 programms", screenDiv}
};

static int commandsDim = sizeof(commands)/sizeof(commands[0]);

int main() {
	init();
	while(1) {
		putchar('>');
		command_listener();
	}
}


static void init() {
	puts("Bienvenido a la consola");
	help();
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
	for (i = 0; i < commandsDim; i++) {
		if (strcmp(commandBuffer, commands[i].name) == 0) {
			commands[i].exec();
			return;
		}
	}
	printf("%s -> comando invalido\n", commandBuffer);
}
static void help() {
	puts("Los comandos disponibles son:");
	for(int i = 0; i < commandsDim; i++) {
		printf("%d) %s => %s\n", i, commands[i].name, commands[i].desc);
	}
}
static void inforeg() {
    regDump();
}

static void printmem() {
    uint64_t address;
    uint8_t arr[32];
	for(int i = 0; i < 32; i++) {
		arr[i] = i+1;
	}
    scanf("%d",&address);
    memDump((uint64_t)arr);
}
static void time() {
    printTime();
}
static void primes() {
    int isPrime = 1;
    puts("Los numeros primos son:");
    for (unsigned int number = 1; number < UINT_MAX; number+= 2, isPrime = 1) {
        for (int i = 2; i * i <= number && isPrime; i++) {
            if (number % i == 0)
                isPrime = 0;
        }
        if (isPrime)
            printf("-- %d\n", number);
    }
}
static void fibo() {
    printf("fibo(%d) = %d\n", 0, 0);
    for (unsigned int i = 1, prev = 0, current = 1, next = 1; current <= UINT_MAX - prev; i++, prev = current, current = next, next = prev + current)
        printf("fibo(%d) = %d\n", i, current);
}
static void screenDiv() {return;}