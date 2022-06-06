#include <shell.h>

// type definitions
typedef bool (* hasNextfp)  (int64_t);
typedef void (* nextfp)     (int64_t);
typedef void (* resetfp)    (int64_t);

typedef struct iterator {
    hasNextfp hasNext;
    nextfp    next;
    resetfp   reset;
} iterator;

typedef void (*voidfp)(void);

typedef struct command {
    char * name;
    char * desc;
    voidfp exec;
    iterator it;
} command;

typedef enum COMMANDS {NONE = -1, HELP, INFOREG, ZERODIV, INVALID_OPCODE, TIME, PRIMES, FIBONACCI, PRINTMEM, PIPE, CANT_COMMANDS} COMMANDS;

// -------- main functions -------------------------------------
static void init();
static void command_listener();

// ------------ commands implemented on this file -----------------------
static void help();
static void primes();
static void fibonacci();
static void screenDiv(command leftCommand, command rightCommand);

//----- auxiliary functions ------------

static void iterate(hasNextfp hasNext, nextfp next, resetfp reset);
static void handlePipe(command leftCommand, command rightCommand);
static void pipeError(int64_t add, const char * side, const char * command);
static void updateStatus();
static void printStatus();
static void resetPaused();
static void resetRunning();
static void resetStarted();
static void resetPrintmemAddresses();
static void waitForEnterOrStop();
static int64_t parsePrintmem(char * commandBuffer);

// -------------- Flags -- no inicializadas en 0 para que no esten en bss B-)
static COMMANDS running[] = {NONE, NONE};
static bool stopped = TRUE;
static bool paused[] = {TRUE, TRUE};
static bool leftExcep = TRUE, rightExcep = TRUE;
static bool comingFromException = TRUE;
static bool started[] = {TRUE, TRUE};
static int64_t printmemAddresses[] = {INVALID_ADDRESS, INVALID_ADDRESS};

// for use on pipe command
// selector for next, in case of printmem command
#define ADDR_OR_SCREEN(screen) (printmemAddresses[(screen)] >= 0 ? printmemAddresses[(screen)] : (screen))

// To expand defined value to a string
#define VALUE_TO_STRING(s) LITERAL_TO_STRING(s)
#define LITERAL_TO_STRING(s) #s

// Pipe / iterable command auxiliary keys
#define CMD_STOP_KEY 's'
#define LEFTCMD_PAUSE_KEY 'a'
#define RIGHTCMD_PAUSE_KEY 'd'

// nonIterableCommands functions
// only does 1 repetition
static bool nonIterableCommandHasNext(int64_t screen) {
    return (bool) !started[screen];
}

static void nonIterableCommandReset(int64_t screen) {
    started[screen] = FALSE;
}

static command commands[] = {
        {"help", "shows all available commands",                                     help,              {nonIterableCommandHasNext, help,               nonIterableCommandReset}},
        {"inforeg", "prints register snapshot, take a snapshot using \'cntrl + s\'", regDump,           {nonIterableCommandHasNext, regDump,            nonIterableCommandReset}},
        {"zerodiv", "generates a zero divition exeption",                            zerodiv,           {nonIterableCommandHasNext, zerodiv,            nonIterableCommandReset}},
        {"invalid_opcode", "generates an invalid operation exception",               invalidopcode,     {nonIterableCommandHasNext, invalidopcode,      nonIterableCommandReset}},
        {"time", "prints the current system time",                                   printTime,         {nonIterableCommandHasNext, printTime,          nonIterableCommandReset}},
        {"primes", "prints primes",                                                  primes,            {primesHasNext,             primesNext,         primesReset}},
        {"fibonacci", "prints the fibonacci series",                                 fibonacci,         {fiboHasNext,               fiboNext,           fiboReset}},
        {"printmem", "prints the 32 bytes which follow the recieved address",        (voidfp) memDump,  {nonIterableCommandHasNext, memDump,            nonIterableCommandReset}},
        {"|", "allows to divide the screen and run 2 programms",                     (voidfp) screenDiv,{nonIterableCommandHasNext, (nextfp) screenDiv, nonIterableCommandReset}}
};

static int commandsDim = sizeof(commands)/sizeof(commands[0]);

//--------------------- Main functions ----------------

int main() {
    if(running[LEFT_SCREEN] >= 0 && running[RIGHT_SCREEN] >= 0) {
        screenDiv(commands[running[LEFT_SCREEN]], commands[running[RIGHT_SCREEN]]);
    } else if(running[FULL_SCREEN] == NONE) {
        init();
    }

    while(TRUE) {
        putchar('>');
        command_listener();
    }
}

static void init() {
    stopped = FALSE;
    started[LEFT_SCREEN] = FALSE;
    started[RIGHT_SCREEN] = FALSE;
    leftExcep = FALSE;
    rightExcep = FALSE;
    comingFromException = FALSE;
    sysOneWind();
    resetRunning();
    resetPaused();
    resetStarted();
    puts("Bienvenido a la consola");
    help();
}

static void command_listener() {
    char commandBuffer[COMMAND_BUFFER_SIZE];
    int i;
    scanf("%"VALUE_TO_STRING(COMMAND_BUFFER_SIZE)"s", commandBuffer); // maybe usar una funcion?
    if(strcmp(commandBuffer, "") == 0) return;
    for (i = 0; i < commandsDim - 2; i++) {
        if (strcmp(commandBuffer, commands[i].name) == 0) {
            running[FULL_SCREEN] = i;
            commands[i].exec();
            running[FULL_SCREEN] = NONE;
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

        if (address == NOT_PRINTMEM)
            printf("%s -> comando invalido\n", commandBuffer);

        if (address >= 0) {
            running[FULL_SCREEN] = PRINTMEM;
            printmemAddresses[FULL_SCREEN] = address;
            memDump(printmemAddresses[FULL_SCREEN]);
            printmemAddresses[FULL_SCREEN] = address;
            running[FULL_SCREEN] = NONE;
        }


        return;
    }

    command leftCommand, rightCommand;
    bool leftFound = FALSE, rightFound = FALSE;
    for (i = 0; i < commandsDim - 2; i++) {
        
        if (strcmp(leftStr, commands[i].name) == 0) {
            leftCommand = commands[i];
            leftFound = TRUE;
            running[LEFT_SCREEN] = i;
        }
        if (strcmp(rightStr, commands[i].name) == 0) {
            rightFound = TRUE;
            rightCommand = commands[i];
            running[RIGHT_SCREEN] = i;
        }
    }

    int64_t leftAddress, rightAddress;

    if (!leftFound) {
        leftAddress = parsePrintmem(leftStr);
        if (leftAddress >= 0) {
            leftCommand = commands[PRINTMEM];
            leftFound = TRUE;
            printmemAddresses[LEFT_SCREEN] = leftAddress;
            running[LEFT_SCREEN] = PRINTMEM;
        }
    }

    if (!rightFound) {
        rightAddress = parsePrintmem(rightStr);
        if (rightAddress >= 0) {
            rightCommand = commands[PRINTMEM];
            rightFound = TRUE;
            printmemAddresses[RIGHT_SCREEN] = rightAddress;
            running[RIGHT_SCREEN] = PRINTMEM;
        }
    }

    if (leftFound && rightFound) {
        //void(*fun)(command, command) = (void(*)(command, command)) commands[8].exec;
        //fun(leftCommand, rightCommand);
        screenDiv(leftCommand, rightCommand);
        return;
    }
    resetRunning();
    resetPrintmemAddresses();
    if (!leftFound)
        pipeError(leftAddress, "izquierda", leftStr);

    if (!rightFound) 
        pipeError(rightAddress, "derecha", rightStr);
}


//------------------- commands implemented in this file ---------------
static void help() {
    puts("Los comandos disponibles son:");
    for(int i = 0; i < commandsDim; i++) {
        printf("%d) %s => %s\n", i + 1, commands[i].name, commands[i].desc);
    }
}

static void primes() {
    iterate(primesHasNext, primesNext, primesReset);
}

static void fibonacci() {
    iterate(fiboHasNext, fiboNext, fiboReset);
}

static void screenDiv(command leftCommand, command rightCommand) {
    if(comingFromException == FALSE) {
        sysDivWind();
        leftExcep = FALSE;
        rightExcep = FALSE;
    }
    handlePipe(leftCommand, rightCommand);
    if(stopped) {
        stopped = FALSE;
        sysOneWind();
        return;
    }
    sysSetWind(LEFT_SCREEN);
    printf("\n=== BOTH COMMANDS HAVE FINISHED ===\n");
    printf("Press enter or %c to return to console\n", CMD_STOP_KEY);
    waitForEnterOrStop();
    sysOneWind();
}

//----------------- Auxiliary functions -------------------------------------------

static void iterate(hasNextfp hasNext, nextfp next, resetfp reset) {
    while (!stopped && hasNext(FULL_SCREEN)) {
        if(!paused[FULL_SCREEN])
            next(FULL_SCREEN);
        updateStatus();
    }
    reset(FULL_SCREEN);
    resetPaused();
    stopped = FALSE;
}

static int64_t parsePrintmem(char * commandBuffer) {
    if (strcmp(commandBuffer, "printmem") == 0)
        return ARGUMENT_MISSING;

    if (strncmp(commandBuffer, "printmem ", 9) != 0)
        return NOT_PRINTMEM;

    int i = 9;
    while (commandBuffer[i] == ' ') i++;

    char argument[COMMAND_BUFFER_SIZE];

    int j;
    for (j = 0; commandBuffer[i] != '\0' && j < COMMAND_BUFFER_SIZE; i++, j++)
        argument[j] = commandBuffer[i];
    argument[j] = '\0';

    const char * out;
    int64_t address = strtol(argument, &out, 16);

    if (address < 0 || *out != '\0')
        return INVALID_ADDRESS;

    return address;
}

static void handlePipe(command leftCommand, command rightCommand) {
    comingFromException = TRUE;
    while (!stopped && (leftCommand.it.hasNext(LEFT_SCREEN) || rightCommand.it.hasNext(RIGHT_SCREEN))) {
        updateStatus();

        if (!leftExcep && !paused[LEFT_SCREEN] && leftCommand.it.hasNext(LEFT_SCREEN)) {
            sysSetWind(LEFT_SCREEN);
            leftExcep = TRUE;
            started[LEFT_SCREEN] = TRUE;
            leftCommand.it.next(ADDR_OR_SCREEN(LEFT_SCREEN));
            leftExcep = FALSE;
        }

        if (!rightExcep && !paused[RIGHT_SCREEN] && rightCommand.it.hasNext(RIGHT_SCREEN)) {
            sysSetWind(RIGHT_SCREEN);
            rightExcep = TRUE;
            started[RIGHT_SCREEN] = TRUE;
            rightCommand.it.next(ADDR_OR_SCREEN(RIGHT_SCREEN));
            rightExcep = FALSE;
        }
    }
    leftCommand.it.reset(LEFT_SCREEN);
    rightCommand.it.reset(RIGHT_SCREEN);
    resetPaused();
    resetRunning();
    resetStarted();
    resetPrintmemAddresses();
    comingFromException = FALSE;
}

static void pipeError(int64_t add, const char * side, const char * command) {
    switch(add) {
        case ARGUMENT_MISSING:
            printf("printmem de la %s debe recibir la direccion como argumento\n", side); break;
        case INVALID_ADDRESS:
            printf("Argumento del printmem de la %s invalido\n", side); break;
        case NOT_PRINTMEM:
            printf("%s -> comando invalido\n", command); break;
        default:
            return;
    }
}


static void waitForEnterOrStop() {
    char c;
    while ((c = getchar()) != '\n' && c != CMD_STOP_KEY);
}

static void printStatus(int screen) {
    sysSetWind(screen);
    printf("%s", paused[screen]?"Paused":"\b\b\b\b\b\b");
}

static void updateStatus() {
    char c;
    if(sysread(STDIN, &c, 1) == 0) return;
    switch (c) { 
        case CMD_STOP_KEY:       stopped = 1;                                         puts("\n== STOPED ==");    break;
        case LEFTCMD_PAUSE_KEY:  paused[LEFT_SCREEN]  = (bool) !paused[LEFT_SCREEN];  printStatus(LEFT_SCREEN);  break;
        case RIGHTCMD_PAUSE_KEY: paused[RIGHT_SCREEN] = (bool) !paused[RIGHT_SCREEN]; printStatus(RIGHT_SCREEN); break;
        default: break;
    }
}

static void resetPaused() {
    paused[LEFT_SCREEN] = FALSE;
    paused[RIGHT_SCREEN] = FALSE;
}

static void resetRunning() {
    running[LEFT_SCREEN] = NONE;
    running[RIGHT_SCREEN] = NONE;
}

static void resetStarted() {
    started[LEFT_SCREEN] = FALSE;
    started[RIGHT_SCREEN] = FALSE;
}

static void resetPrintmemAddresses() {
    printmemAddresses[LEFT_SCREEN] = INVALID_ADDRESS;
    printmemAddresses[RIGHT_SCREEN] = INVALID_ADDRESS;
}
