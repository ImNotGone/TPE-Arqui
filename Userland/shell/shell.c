#include <stdint.h>
#include <dumps.h>
#include <time.h>
#include <_stdio.h>
#include <_string.h>
#include <syscalls.h>

#define UINT_MAX 4294967295
#define COMMAND_BUFFERSIZE 256

typedef void (*fp)(void);

typedef struct command {
    char * name;
    char * desc;
    fp exec;
    int pausable;
} command;

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
static void screenDiv(command leftCommand, command rightCommand);


static command commands[] = {
	{"help", "shows all available commands", help, 0},
	{"inforeg", "prints register snapshot, take a snapshot using \'cntrl + s\'", inforeg, 0},
	{"zerodiv", "generates a zero divition exeption", zerodiv, 0},
	{"invalid_opcode", "generates an invalid operation exception", invalidopcode, 0},
	{"printmem", "prints the 32 bytes which follow the recieved address", printmem, 0},
	{"time", "prints the current system time", time, 0},
	{"primes", "prints primes", primes, 1},
	{"fibo", "prints the fibonacci series",fibo, 1},
	{"|", "allows to divide the screen and run 2 programms", (fp) screenDiv, 0}
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
	char commandBuffer[COMMAND_BUFFERSIZE];
	int c;
	int i = 0;
	while((c = getchar()) != '\n' && i < COMMAND_BUFFERSIZE) {
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
	for (i = 0; i < commandsDim - 1; i++) {
		if (strcmp(commandBuffer, commands[i].name) == 0) {
			commands[i].exec();
			return;
		}
	}
    char leftStr[COMMAND_BUFFERSIZE];
    char rightStr[COMMAND_BUFFERSIZE];

    if (!strDivide(commandBuffer, leftStr, rightStr, '|')) {
        printf("%s -> comando invalido\n", commandBuffer);
        return;
    }

    command leftCommand, rightCommand;
    int leftFound = 0, rightFound = 0;
    for (i = 0; i < commandsDim - 1; i++) {
        if (strcmp(leftStr, commands[i].name) == 0) {
            leftCommand = commands[i];
            leftFound = 1;
        }
        if (strcmp(rightStr, commands[i].name) == 0) {
            rightFound = 1;
            rightCommand = commands[i];
        }
    }

    if (leftFound && rightFound) {
        screenDiv(leftCommand, rightCommand);
        return;
    }

    if (!leftFound)
        printf("%s -> comando invalido\n", leftStr);
    if (!rightFound)
        printf("%s -> comando invalido\n", rightStr);


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
    for (uint64_t number = 1; number < UINT64_MAX; number+= 2, isPrime = 1) {
        for (uint64_t i = 2; i * i <= number && isPrime; i++) {
            if (number % i == 0)
                isPrime = 0;
        }
        if (isPrime)
            printf("-- %d\n", number);
    }
}
static void fibo() {
    printf("fibo(%d) = %d\n", 0, 0);
    for (uint64_t i = 1, prev = 0, current = 1, next = 1; current <= UINT64_MAX - prev; i++, prev = current, current = next, next = prev + current)
        printf("fibo(%d) = %d\n", i, current);
}
static void screenDiv(command leftCommand, command rightCommand) {
    sysDivWind();
    if (!leftCommand.pausable && !rightCommand.pausable) {
        leftCommand.exec();
        sysSwiWind();
        rightCommand.exec();
        sysSwiWind();
        return;
    }

    if (leftCommand.pausable) {
        sysSwiWind();
        rightCommand.exec();
        sysSwiWind();
        leftCommand.exec();
        return;
    }

    if (rightCommand.pausable) {
        leftCommand.exec();
        sysSwiWind();
        rightCommand.exec();
        sysSwiWind();
        return;
    }
}