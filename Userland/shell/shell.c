#include <shell.h>

typedef struct iterator {
    bool (*hasNext) (int);
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
static void primes();
static void fibo();
static void screenDiv(command leftCommand, command rightCommand, int64_t leftAddress, int64_t rightAddress);
static void waitForEnter();
static void checkExited();
static void resetPaused();
static void resetRunning();
static int64_t parsePrintmem(char * commandBuffer);

// para que no esten en bss B-)
static int running[] = {NO_PROGRAM_RUNNING, NO_PROGRAM_RUNNING};
static bool stoped = TRUE;
static bool paused[] = {TRUE, TRUE};
static bool leftExep = TRUE, rightExep = TRUE;
static bool commingFromExeption = TRUE;
static bool nonIterable[] = {TRUE, TRUE};

// nonIterableCommands functions
// only does 1 repetition
static bool nonIterableCommandHasNext(int screen) {
    bool out = nonIterable[screen];
    nonIterable[screen] = FALSE;
    return out;
}

static void nonIterableCommandReset(int screen) {
    nonIterable[screen] = TRUE;
}

static command commands[] = {
        {"help", "shows all available commands", help,                                          {nonIterableCommandHasNext, help, nonIterableCommandReset}},
        {"inforeg", "prints register snapshot, take a snapshot using \'cntrl + s\'", inforeg,   {nonIterableCommandHasNext, inforeg, nonIterableCommandReset}},
        {"zerodiv", "generates a zero divition exeption", zerodiv,                              {nonIterableCommandHasNext, zerodiv, nonIterableCommandReset}},
        {"invalid_opcode", "generates an invalid operation exception", invalidopcode,           {nonIterableCommandHasNext, invalidopcode, nonIterableCommandReset}},
        {"time", "prints the current system time", printTime,                                   {nonIterableCommandHasNext, printTime, nonIterableCommandReset}},
        {"primes", "prints primes", primes,                                                     {primesHasNext, (void (*) (int64_t))primesNext, primesReset}},
        {"fibo", "prints the fibonacci series",fibo,                                            {fiboHasNext, (void (*) (int64_t))fiboNext, fiboReset}},
        {"printmem", "prints the 32 bytes which follow the recieved address", (fp) memDump,     {nonIterableCommandHasNext, (void (*) (int64_t)) memDump, nonIterableCommandReset}},
        {"|", "allows to divide the screen and run 2 programms", (fp) screenDiv,                {nonIterableCommandHasNext, (void (*) (int64_t)) screenDiv, nonIterableCommandReset}}
};

static int commandsDim = sizeof(commands)/sizeof(commands[0]);

int main() {
    if(running[LEFT_SCREEN] >= 0 && running[RIGHT_SCREEN] >= 0) {
        screenDiv(commands[running[LEFT_SCREEN]], commands[running[RIGHT_SCREEN]], -1, -1);
    } else if(running[FULL_SCREEN] == NO_PROGRAM_RUNNING) {
        init();
    }
    /*
    if (running[LEFT_SCREEN] >= 0 && running[RIGHT_SCREEN] >= 0) {
        puts("Se ha reiniciado el kernel mientras se corrian programas");
        printf("En la ventana izquierda se estaba corriendo: %s\nEn la ventana derecha se estaba corriendo: %s\n", commands[running[LEFT_SCREEN]].name, commands[running[RIGHT_SCREEN]].name);
        char c;
        do {
            printf("Si desea reaunudar %s presione 0\nSi desea reaunudar %s presione 1\nSi desea reaunudar ambos programas presione 2\nSi desea volver a la terminal presione 3\n", commands[running[LEFT_SCREEN]].name, commands[running[RIGHT_SCREEN]].name);
        } while ((c = getchar()) < '0' || c > '3');

        switch (c) {
            case '0':
                commands[running[RIGHT_SCREEN]].it.reset(RIGHT_SCREEN); running[RIGHT_SCREEN] = NO_PROGRAM_RUNNING; commands[running[LEFT_SCREEN]].exec(); waitForEnter();break;
            case '1':
                commands[running[LEFT_SCREEN]].it.reset(LEFT_SCREEN); running[LEFT_SCREEN] = NO_PROGRAM_RUNNING; commands[running[RIGHT_SCREEN]].exec(); waitForEnter();break;
            case '2':
                screenDiv(commands[running[LEFT_SCREEN]], commands[running[RIGHT_SCREEN]], -1, -1); break;
            default:
                break;
        }

        if (running[LEFT_SCREEN] >= 0)
            commands[running[LEFT_SCREEN]].it.reset(LEFT_SCREEN);
        if (running[RIGHT_SCREEN]>= 0)
            commands[running[RIGHT_SCREEN]].it.reset(RIGHT_SCREEN);

    } else if (running[LEFT_SCREEN] >= 0 || running[RIGHT_SCREEN] >= 0) {
        int runningScreen = running[LEFT_SCREEN] >= 0 ? LEFT_SCREEN : RIGHT_SCREEN;
        printf("Se ha reiniciado el kernel mientras se corria %s\n", commands[running[runningScreen]].name);
        char c;
        do {
            puts("Si desea reaunudar el programa presione 0\nSi desea volver a la terminal presione 1");
        } while ((c = getchar()) < '0' || c > '1');

        if (c == '0')
            commands[running[runningScreen]].exec();

        commands[running[runningScreen]].it.reset(runningScreen);
    }
    */
    while(TRUE) {
        putchar('>');
        command_listener();
    }
}


static void init() {
    stoped = FALSE;
    paused[0] = FALSE;
    paused[1] = FALSE;
    leftExep = FALSE;
    rightExep = FALSE;
    commingFromExeption = FALSE;
    sysOneWind();
    resetRunning();
    resetPaused();
    puts("Bienvenido a la consola");
    help();
}

static void pipeError(int add, const char * side, const char * command) {
    switch(add) {
        case ARGUMENT_MISSING:
            printf("printmem de la %s debe recibir la direccion como argumento\n", side); break;
        case INVALID_ADDRESS:
            printf("Argumento del printmem de la %s invalido\n", side); break;
        case NOT_PRINTMEM:
            printf("%s -> comando invalido\n", command); break;
    }
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
            running[FULL_SCREEN] = i;
            commands[i].exec();
            running[FULL_SCREEN] = NO_PROGRAM_RUNNING;
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
            running[FULL_SCREEN] = 7;
            memDump(address);
            running[FULL_SCREEN] = NO_PROGRAM_RUNNING;
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

    int64_t leftAddress = -1, rightAddress = -1;

    if (!leftFound) {
        leftAddress = parsePrintmem(leftStr);
        if (leftAddress >= 0) {
            leftCommand = commands[7];
            leftFound = TRUE;
            running[LEFT_SCREEN] = 7;
        }
    }

    if (!rightFound) {
        rightAddress = parsePrintmem(rightStr);
        if (rightAddress >= 0) {
            rightCommand = commands[7];
            rightFound = TRUE;
            running[RIGHT_SCREEN] = 7;
        }
    }

    if (leftFound && rightFound) {
        screenDiv(leftCommand, rightCommand, leftAddress, rightAddress);
        return;
    }
    resetRunning();
    if (!leftFound)
        pipeError(leftAddress, "izquierda", leftStr);

    if (!rightFound) 
        pipeError(rightAddress, "derecha", rightStr);
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

static void primes() {
    while (!stoped && primesHasNext(FULL_SCREEN)) {
        if(!paused[FULL_SCREEN])
            primesNext(FULL_SCREEN);
        checkExited();
    }
    primesReset(FULL_SCREEN);
    resetPaused();
    stoped = FALSE;
}
static void fibo() {

    while (!stoped && fiboHasNext(FULL_SCREEN)) {
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
    while (commandBuffer[i] == ' ') i++;

    char argument[COMMAND_BUFFER_SIZE];

    int j;
    for (j = 0; commandBuffer[i] != '\0' && j < COMMAND_BUFFER_SIZE; i++, j++)
        argument[j] = commandBuffer[i];
    argument[j] = '\0';

    const char * out;
    int64_t address = strtol(argument, &out, 16);

    if (address < 0 || *out !='\0')
        return INVALID_ADDRESS;

    return address;
}

static void waitForEnterOrS() {
    char c;
    while ((c = getchar()) != '\n' && c != 's');
    return;
}

static void updateStatus(int screen) {
    sysSetWind(screen);
    printf("%s", paused[screen]?"Paused":"\b\b\b\b\b\b");
}

static void checkExited() {
    char c;
    if(sysread(STDIN, &c, 1) == 0) return;
    switch (c) { 
        case 's': stoped = 1; puts("== STOPED =="); break;
        case 'a': paused[LEFT_SCREEN] = !paused[LEFT_SCREEN]; updateStatus(LEFT_SCREEN); break;
        case 'd': paused[RIGHT_SCREEN] = !paused[RIGHT_SCREEN]; updateStatus(RIGHT_SCREEN); break;
        default: break;
    }
}

static void resetPaused() {
    paused[LEFT_SCREEN] = FALSE;
    paused[RIGHT_SCREEN] = FALSE;
}

static void resetRunning() {
    running[LEFT_SCREEN] = NO_PROGRAM_RUNNING;
    running[RIGHT_SCREEN] = NO_PROGRAM_RUNNING;
}

static void handlePipe(command leftCommand, command rightCommand, int64_t leftAddress, int64_t rightAddress) {
    commingFromExeption = TRUE;
    bool remainsLeft = !leftExep;
    bool remainsRight = !rightExep;
    while (!stoped && (remainsLeft || remainsRight)) {
        checkExited();

        if (!leftExep && !paused[LEFT_SCREEN] && (remainsLeft = leftCommand.it.hasNext(LEFT_SCREEN))) {
            sysSetWind(LEFT_SCREEN);
            leftExep = TRUE;
            leftCommand.it.next((leftAddress) >= 0 ? leftAddress : LEFT_SCREEN);
            leftExep = FALSE;
        }

        if (!rightExep && !paused[RIGHT_SCREEN] && (remainsRight = rightCommand.it.hasNext(RIGHT_SCREEN))) {
            sysSetWind(RIGHT_SCREEN);
            rightExep = TRUE;
            rightCommand.it.next((rightAddress) >= 0 ? rightAddress : RIGHT_SCREEN);
            rightExep = FALSE;
        }
    }
    leftCommand.it.reset(LEFT_SCREEN);
    rightCommand.it.reset(RIGHT_SCREEN);
    resetPaused();
    resetRunning();
    commingFromExeption = FALSE;
}

static void screenDiv(command leftCommand, command rightCommand, int64_t leftAddress, int64_t rightAddress) {
    if(commingFromExeption == FALSE) {
        sysDivWind();
        leftExep = FALSE;
        rightExep = FALSE;
    }
    handlePipe(leftCommand, rightCommand, leftAddress, rightAddress);
    if(stoped) {
        stoped = FALSE;
        sysOneWind();
        return;
    }
    sysSetWind(LEFT_SCREEN);
    printf("\nBoth commands have finished\n");
    printf("Press enter or s to return to console\n");
    waitForEnterOrS();
    sysOneWind();
}