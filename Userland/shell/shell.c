#include <shell.h>

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
static void displayError(int64_t add, const char *command);
static void updateStatus();
static void printStatus(int screen);
static void resetPaused();
static void resetRunning();
static void resetStarted();
static void resetPrintmemAddresses();
static void waitForEnterOrStop();
static int64_t parsePrintmem(char * commandBuffer);

// -------------- Flags ------------------------------
// Inicializadas con valor distinto de cero para que no esten en bss
static COMMANDS running[] = {NONE, NONE};
static bool stopped = TRUE;
static bool paused[] = {TRUE, TRUE};
static bool leftExcep = TRUE, rightExcep = TRUE;
static bool comingFromException = TRUE;
static bool started[] = {TRUE, TRUE};
static int64_t printmemAddresses[] = {INVALID_ADDRESS, INVALID_ADDRESS};

// nonIterableCommands functions
// only does 1 repetition
static bool nonIterableCommandHasNext(int64_t screen) {
    return (bool) !started[screen];
}

static void nonIterableCommandReset(int64_t screen) {
    started[screen] = FALSE;
}

static command commands[] = {
    {"help",            HELP_DESC,          help,              {nonIterableCommandHasNext, help,               nonIterableCommandReset}},
    {"inforeg",         INFOREG_DESC,       regDump,           {nonIterableCommandHasNext, regDump,            nonIterableCommandReset}},
    {"zerodiv",         ZERODIV_DESC,       zerodiv,           {nonIterableCommandHasNext, zerodiv,            nonIterableCommandReset}},
    {"invalid_opcode",  INVALID_OP_DESC,    invalidopcode,     {nonIterableCommandHasNext, invalidopcode,      nonIterableCommandReset}},
    {"time",            TIME_DESC,          printTime,         {nonIterableCommandHasNext, printTime,          nonIterableCommandReset}},
    {"primes",          PRIMES_DESC,        primes,            {primesHasNext,             primesNext,         primesReset}},
    {"fibonacci",       FIBONACCI_DESC,     fibonacci,         {fiboHasNext,               fiboNext,           fiboReset}},
    {"printmem",        PRINTMEM_DESC,      (voidfp) memDump,  {nonIterableCommandHasNext, memDump,            nonIterableCommandReset}},
    {"|",               PIPE_DESC,          (voidfp) screenDiv,{nonIterableCommandHasNext, (nextfp) screenDiv, nonIterableCommandReset}}
};

static int commandsDim = sizeof(commands)/sizeof(commands[0]);

//--------------------- Main functions ----------------

int main() {
    // se hace este checkeo para poder continuar programas si hubo un restart
    if(running[LEFT_SCREEN] >= 0 && running[RIGHT_SCREEN] >= 0) {
        screenDiv(commands[running[LEFT_SCREEN]], commands[running[RIGHT_SCREEN]]);
    } else if(running[FULL_SCREEN] == NONE) {
        init();
    }

    while(TRUE) {
        printf(CONSOLE_PROMPT);
        command_listener();
    }
}

static void init() {
    // Se actualizan todas las flags con sus valores correspondientes de inicio
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
    // Le mostramos info al usuario
    puts(WELCOME_MESSAGE);
    help();
}

static void command_listener() {
    char commandBuffer[COMMAND_BUFFER_SIZE];
    int i;
    scanf("%"VALUE_TO_STRING(COMMAND_BUFFER_SIZE)"s", commandBuffer);

    if(strcmp(commandBuffer, "") == 0) return;

    // Vemos si el comando matchea con alguno
    // Que no requiere un parseo especial del input
    for (i = 0; i < commandsDim - 2; i++) {
        if (strcmp(commandBuffer, commands[i].name) == 0) {
            running[FULL_SCREEN] = i;
            commands[i].exec();
            running[FULL_SCREEN] = NONE;
            return;
        }
    }

    // Vemos si comando es el pipe

    char leftStr[COMMAND_BUFFER_SIZE]; // String a la izquierda del pipe
    char rightStr[COMMAND_BUFFER_SIZE]; // String a la derecha del pipe

    if (!strDivide(commandBuffer, leftStr, rightStr, '|')) { // strDivide devuelve 0 si no encontro el divider
        // El comando no es el pipe, queda validar que es printmem para una sola pantalla

        int64_t address = parsePrintmem(commandBuffer);

        if (address >= 0) { // El comando es printmem con una direccion valida como argumento
            running[FULL_SCREEN] = PRINTMEM;
            printmemAddresses[FULL_SCREEN] = address;
            memDump(printmemAddresses[FULL_SCREEN]);
            printmemAddresses[FULL_SCREEN] = address;
            running[FULL_SCREEN] = NONE;
            return;
        }

        // El comando no es printmem o el argumento recibido es invalido
        displayError(address, commandBuffer);
        return;
    }

    // strDivide retorno 1, entonces el comando es el pipe
    // Queda validar que los comandos a los lados del pipe son validos

    command leftCommand, rightCommand;
    bool leftFound = FALSE, rightFound = FALSE;

    // Vemos si matchean con algunos de los comandos que no son printmem
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

    // Si alguno de los dos comandos no matcheo con los anteriores
    // Vemos si son printmem

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

    // Ambos comandos matchearon con alguno
    if (leftFound && rightFound) {
        screenDiv(leftCommand, rightCommand);
        return;
    }

    // Algun comando del pipe es invalido
    // Con el valor de leftAddress y rightAddress
    // podemos saber el tipo de error

    resetRunning();
    resetPrintmemAddresses();
    if (!leftFound) {
        fprintf(STDERR, "Error in left command: ");
        displayError(leftAddress, leftStr);
    }

    if (!rightFound) {
        fprintf(STDERR, "Error in right command: ");
        displayError(rightAddress, rightStr);
    }
}


//------------------- commands implemented in this file ---------------
static void help() {
    puts("\n=== GENERAL INFO ===\n");
    puts("\'s\' stops iterable or pipe programs");
    puts("\'a\' pauses left side or individual programs");
    puts("\'d\' pauses right side programs");
    puts("\n====================\n");
    puts("The available commands are:\n");
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
    if(comingFromException == FALSE) { // Si venimos de una excep no queremos limpiar la pantalla
        sysDivWind();
        leftExcep = FALSE;
        rightExcep = FALSE;
    }
    handlePipe(leftCommand, rightCommand);

    // Si el usurario corto el programa salimos directamente
    if(stopped) {
        stopped = FALSE;
        sysOneWind();
        return;
    }

    // Si ambos comandos terminaron por su cuenta esperamos
    // un input del usuario antes de retornar
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

// Si commandBuffer es "printmem" sin argumentos -> retorna ARGUMENT_MISSING
// Si commandBuffer no contiene "printmem" -> retorna NOT_PRINTMEM
// Si commandBuffer es "printmem" pero el argumento que recibe es invalido -> retorna INVALID_ARGUMENT
// Si commandBuffer contiene "printmem" y el argumento es valido -> retorna el argumento como valor numerico
// El argumento de printmem debe estar separado por un espacio como minimo, el resto de los espacios son ignorados
static int64_t parsePrintmem(char * commandBuffer) {
    if (strcmp(commandBuffer, "printmem") == 0)
        return ARGUMENT_MISSING;

    if (strncmp(commandBuffer, "printmem ", 9) != 0)
        return NOT_PRINTMEM;

    // Validacion del argumento

    int i = 9;
    while (commandBuffer[i] == ' ') i++; // Ignoro espacios

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
    // Iteramos por ambos programas, los programas no iterables (no son ni fibo ni primes) se ejecutan una vez y listo
    while (!stopped && (leftCommand.it.hasNext(LEFT_SCREEN) || rightCommand.it.hasNext(RIGHT_SCREEN))) {
        updateStatus(); // Vemos si el usuario quiere pausar o parar los programas

        // programa de la ventana izquierda
        if (!leftExcep && !paused[LEFT_SCREEN] && leftCommand.it.hasNext(LEFT_SCREEN)) {
            sysSetWind(LEFT_SCREEN);
            leftExcep = TRUE;
            started[LEFT_SCREEN] = TRUE;
            leftCommand.it.next(ADDR_OR_SCREEN(LEFT_SCREEN));
            leftExcep = FALSE;
        }

        // programa de la ventana derecha
        if (!rightExcep && !paused[RIGHT_SCREEN] && rightCommand.it.hasNext(RIGHT_SCREEN)) {
            sysSetWind(RIGHT_SCREEN);
            rightExcep = TRUE;
            started[RIGHT_SCREEN] = TRUE;
            rightCommand.it.next(ADDR_OR_SCREEN(RIGHT_SCREEN));
            rightExcep = FALSE;
        }
    }
    // termino la iteracion, reseteamos las flags
    leftCommand.it.reset(LEFT_SCREEN);
    rightCommand.it.reset(RIGHT_SCREEN);
    resetPaused();
    resetRunning();
    resetStarted();
    resetPrintmemAddresses();
    comingFromException = FALSE;
}

// Displays Error in STDERR
static void displayError(int64_t add, const char *command) {
    switch(add) {
        case ARGUMENT_MISSING:
            fputs(STDERR, ARGUMENT_MISSING_MESSAGE); break;
        case INVALID_ADDRESS:
            fprintf(STDERR, INVALID_ARGUMENT_MESSAGE, command+9); break;
        case NOT_PRINTMEM:
            fprintf(STDERR, INVALID_COMMAND_MESSAGE_FORMAT, command); break;
        default:
            return;
    }
}

static void waitForEnterOrStop() {
    char c;
    while ((c = getchar()) != '\n' && c != CMD_STOP_KEY);
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

static void printStatus(int screen) {
    sysSetWind(screen);
    printf("%s", paused[screen]?"Paused":"\b\b\b\b\b\b");
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
