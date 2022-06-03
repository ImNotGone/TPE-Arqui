#include <stdint.h>
#include <dumps.h>
#include <time.h>
#include <_stdio.h>
#include <_string.h>
#include <syscalls.h>

#define COMMAND_BUFFER_SIZE 256
#define LEFT_SCREEN 0
#define FULL_SCREEN 0
#define RIGHT_SCREEN 1
#define FALSE 0
#define TRUE !FALSE

#define ARGUMENT_MISSING -1
#define NOT_PRINTMEM -2
#define INVALID_ADDRESS -3
typedef struct iterator {
    int started;
    int (*hasNext) (int, int);
    void (*next) (int64_t);
    void (*reset) (int);
} iterator;

typedef void (*fp)(void);

typedef struct command {
    char * name;
    char * desc;
    fp exec;
    iterator it;
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
static void screenDiv(command leftCommand, command rightCommand, int64_t leftAddress, int64_t rightAddress);
static void checkExited();
static void resetPaused();
static int64_t parsePrintmem(char * commandBuffer);



// Fibonacci
typedef struct fiboInternalState {
    uint64_t i;
    uint64_t prev;
    uint64_t current;
    uint64_t next;
} fiboInternalState;

static fiboInternalState fiboInternalStates[] = {{0, 0, 1, 1}, {0, 0, 1, 1}};

static void fiboNext(int screen) {
    if (fiboInternalStates[screen].i == 0) {
        printf("fibo(%d) = %d\n", 0, 0);
        fiboInternalStates[screen].i++;
        return;
    }

    printf("fibo(%d) = %d\n", fiboInternalStates[screen].i, fiboInternalStates[screen].current);
    fiboInternalStates[screen].i++;
    fiboInternalStates[screen].prev = fiboInternalStates[screen].current;
    fiboInternalStates[screen].current = fiboInternalStates[screen].next;
    fiboInternalStates[screen].next = fiboInternalStates[screen].prev + fiboInternalStates[screen].current;
}

static int fiboHasNext(int started, int screen) {
    return fiboInternalStates[screen].current <= UINT64_MAX - fiboInternalStates[screen].prev;
}

static void fiboReset(int screen) {
    fiboInternalStates[screen].i = 0;
    fiboInternalStates[screen].prev = 0;
    fiboInternalStates[screen].current = 1;
    fiboInternalStates[screen].next = 1;
}

// Primes
typedef struct primesInternalState {
    uint64_t number;
} primeInternalState;

static primeInternalState primeInternalStates[] = {{1}, {1}};

static int isPrime(int n) {
    if(n <= 1) return FALSE;
    if(n <= 3) return TRUE;
    if(n % 2 == 0 || n % 3 == 0) return FALSE;

    for(uint64_t i = 5; i * i <= n; i +=6) {
        if(n % i == 0 || n % (i+2) == 0) 
            return FALSE;
    }
    return TRUE;
}

static int calculateNextPrime(int n) {
    do {
        n++;
    }while (!isPrime(n));
    return n;    
}

static void primesNext(int screen) {
    int n = calculateNextPrime(primeInternalStates[screen].number);
    printf("-- %d\n", n);
    primeInternalStates[screen].number = n;
}

static int primesHasNext(int started, int screen) {
    return primeInternalStates[screen].number <=UINT64_MAX;
}

static void primesReset(int screen) {
    primeInternalStates[screen].number = 1;
}

// nonIterableCommands functions
static int nonIterableCommandHasNext(int started, int screen) {
    return !started;
}

static void nonIterableCommandReset(int screen) {}



static int stoped = FALSE;
static int paused[] = {FALSE, FALSE};

static command commands[] = {
        {"help", "shows all available commands", help,                                          {0, nonIterableCommandHasNext, help, nonIterableCommandReset}},
        {"inforeg", "prints register snapshot, take a snapshot using \'cntrl + s\'", inforeg,   {0, nonIterableCommandHasNext, inforeg, nonIterableCommandReset}},
        {"zerodiv", "generates a zero divition exeption", zerodiv,                              {0, nonIterableCommandHasNext, zerodiv, nonIterableCommandReset}},
        {"invalid_opcode", "generates an invalid operation exception", invalidopcode,           {0, nonIterableCommandHasNext, invalidopcode, nonIterableCommandReset}},
        {"time", "prints the current system time", time,                                        {0, nonIterableCommandHasNext, time, nonIterableCommandReset}},
        {"primes", "prints primes", primes,                                                     {0, primesHasNext, primesNext, primesReset}},
        {"fibo", "prints the fibonacci series",fibo,                                            {0, fiboHasNext, fiboNext, fiboReset}},
        {"printmem", "prints the 32 bytes which follow the recieved address", printmem,         {0, nonIterableCommandHasNext, printmem, nonIterableCommandReset}},
        {"|", "allows to divide the screen and run 2 programms", (fp) screenDiv,                {0, nonIterableCommandHasNext, (void (*) (int))screenDiv, nonIterableCommandReset}}
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
    char commandBuffer[COMMAND_BUFFER_SIZE];
    int c;
    int i = 0;
    while((c = getchar()) != '\n' && i < COMMAND_BUFFER_SIZE) {
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
    if(strcmp(commandBuffer, "") == 0) return;
    for (i = 0; i < commandsDim - 2; i++) {
        if (strcmp(commandBuffer, commands[i].name) == 0) {
            commands[i].exec();
            return;
        }
    }

    char leftStr[COMMAND_BUFFER_SIZE];
    char rightStr[COMMAND_BUFFER_SIZE];

    if (!strDivide(commandBuffer, leftStr, rightStr, '|')) {
        int64_t address = parsePrintmem(commandBuffer);

        if (address == ARGUMENT_MISSING)
            puts("printmem debe recibir la direccion como argumento");

        if (address == INVALID_ADDRESS)
            puts("Argumento invalido");

        if (address >= 0)
            printmem(address);

        if (address != NOT_PRINTMEM)
            return;
    }

    command leftCommand, rightCommand;
    int leftFound = FALSE, rightFound = FALSE;
    for (i = 0; i < commandsDim - 2; i++) {
        if (strcmp(leftStr, commands[i].name) == 0) {
            leftCommand = commands[i];
            leftFound = TRUE;
        }
        if (strcmp(rightStr, commands[i].name) == 0) {
            rightFound = TRUE;
            rightCommand = commands[i];
        }
    }

    int64_t leftAddress = -1, rightAddress = -1;

    if (!leftFound) {
        leftAddress = parsePrintmem(leftStr);
        if (leftAddress >= 0) {
            leftCommand = commands[7];
            leftFound = TRUE;
        }
    }

    if (!rightFound) {
        rightAddress = parsePrintmem(rightStr);
        if (rightAddress >= 0) {
            rightCommand = commands[7];
            rightFound = TRUE;
        }
    }

    if (leftFound && rightFound) {
        screenDiv(leftCommand, rightCommand, leftAddress, rightAddress);
        return;
    }

    if (!leftFound) {
        if (leftAddress == ARGUMENT_MISSING)
            puts("printmem de la izquierda debe recibir la direccion como argumento");

        if (leftAddress == INVALID_ADDRESS)
            puts("Argumento del printmem de la izquierda invalido");

        if (leftAddress == NOT_PRINTMEM)
            printf("%s -> comando invalido\n", leftStr);
    }

    if (!rightFound) {
        if (rightAddress == ARGUMENT_MISSING)
            puts("printmem de la derecha debe recibir la direccion como argumento");

        if (rightAddress == INVALID_ADDRESS)
            puts("Argumento del printmem de la derecha invalido");

        if (rightAddress == NOT_PRINTMEM)
            printf("%s -> comando invalido\n", rightStr);
    }
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

static void printmem(uint64_t address) {
    memDump(address);
}
static void time() {
    printTime();
}
static void primes() {
    while (!stoped && primesHasNext(0, FULL_SCREEN)) {
        if(!paused[FULL_SCREEN])
            primesNext(FULL_SCREEN);
        checkExited();
    }
    primesReset(FULL_SCREEN);
    resetPaused();
    stoped = FALSE;
}
static void fibo() {

    while (!stoped && fiboHasNext(0, FULL_SCREEN)) {
        if(!paused[FULL_SCREEN])
            fiboNext(FULL_SCREEN);
        checkExited();
    }
    fiboReset(FULL_SCREEN);
    resetPaused();
    stoped = FALSE;
}

static int64_t parsePrintmem(char * commandBuffer) {
    if (strcmp(commandBuffer, "printmem") == 0)
        return ARGUMENT_MISSING;

    if (strncmp(commandBuffer, "printmem ", 9) != 0)
        return NOT_PRINTMEM;

    int i = 9;
    int j;
    while (commandBuffer[i] == ' ')
        i++;

    char argument[COMMAND_BUFFER_SIZE];

    for (j = 0; commandBuffer[i] != '\0' && j < COMMAND_BUFFER_SIZE; i++, j++)
        argument[j] = commandBuffer[i];
    argument[j] = '\0';

    const char * out;
    int64_t address = strtol(argument, &out, 16);

    if (address < 0 || *out !='\0')
        return INVALID_ADDRESS;

    return address;
}

static void waitForEnter() {
    while (getchar() != '\n');
}

static void checkExited() {
    char c;
    if(sysread(STDIN, &c, 1) == 0) return;
    switch (c) {
        case 's': stoped = 1; break;
        case 'a': paused[LEFT_SCREEN] = !paused[LEFT_SCREEN]; break;
        case 'd': paused[RIGHT_SCREEN] = !paused[RIGHT_SCREEN]; break;
        default:break;
    }
}

static void resetPaused() {
    paused[LEFT_SCREEN] = FALSE;
    paused[RIGHT_SCREEN] = FALSE;
}

static void handlePipe(command leftCommand, command rightCommand, int64_t leftAddress, int64_t rightAddress) {
    sysDivWind();
    while (!stoped && (leftCommand.it.hasNext(leftCommand.it.started, LEFT_SCREEN) || rightCommand.it.hasNext(rightCommand.it.started, RIGHT_SCREEN))) {
        checkExited();

        sysSetWind(LEFT_SCREEN);
        if (!paused[LEFT_SCREEN] && leftCommand.it.hasNext(leftCommand.it.started, LEFT_SCREEN)) {
            leftCommand.it.next((leftAddress) >= 0 ? leftAddress : LEFT_SCREEN);
            leftCommand.it.started = TRUE;
        }

        sysSetWind(RIGHT_SCREEN);
        if (!paused[RIGHT_SCREEN] && rightCommand.it.hasNext(rightCommand.it.started, RIGHT_SCREEN)) {
            rightCommand.it.next((rightAddress) >= 0 ? rightAddress : LEFT_SCREEN);
            rightCommand.it.started = TRUE;
        }
    }
    leftCommand.it.reset(LEFT_SCREEN);
    rightCommand.it.reset(RIGHT_SCREEN);
    resetPaused();
}

static void screenDiv(command leftCommand, command rightCommand, int64_t leftAddress, int64_t rightAddress) {
    sysDivWind();
    handlePipe(leftCommand, rightCommand, leftAddress, rightAddress);
    if(stoped) {
        stoped = FALSE;
        sysOneWind();
        return;
    }
    waitForEnter();
    sysOneWind();
}