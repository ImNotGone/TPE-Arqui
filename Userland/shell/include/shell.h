#ifndef _SHELL_H
#define _SHELL_H

#include <stdint.h>
#include <dumps.h>
#include <time.h>
#include <_stdio.h>
#include <_string.h>
#include <_stdbool.h>
#include <syscalls.h>
#include <primes.h>
#include <fibonacci.h>

#define COMMAND_BUFFER_SIZE 256
#define LEFT_SCREEN 0
#define FULL_SCREEN 0
#define RIGHT_SCREEN 1

typedef enum ERRORS {ARGUMENT_MISSING = -1, NOT_PRINTMEM = -2, INVALID_ADDRESS = -3} ERRORS;

#define WELCOME_MESSAGE                 "Welcome to the console"
#define ARGUMENT_MISSING_MESSAGE        "printmem: argument missing, please give an initial address as shows \'printmem ACADE123h\'"
#define INVALID_ARGUMENT_MESSAGE        "printmem: invalid argument \'%s\', should only contain this characters \"0123456789ABCDEF\"\n"
#define INVALID_COMMAND_MESSAGE_FORMAT  "%s: command not found\n"
// for use on pipe command
// selector for next, in case of printmem command
#define ADDR_OR_SCREEN(screen) (printmemAddresses[(screen)] >= 0 ? printmemAddresses[(screen)] : (screen))

// To expand defined value to a string
#define VALUE_TO_STRING(s)      LITERAL_TO_STRING(s)
#define LITERAL_TO_STRING(s)    #s
#define CMD_BUFF_FORMAT         "%"VALUE_TO_STRING(COMMAND_BUFFER_SIZE)"s"

// Pipe / iterable command auxiliary keys
#define CMD_STOP_KEY        's'
#define LEFTCMD_PAUSE_KEY   'a'
#define RIGHTCMD_PAUSE_KEY  'd'

#define CONSOLE_PROMPT  "@byte: > "

#define HELP_DESC       "Shows all available commands."
#define INFOREG_DESC    "Prints a register snapshot.\nsnapshots are taken with \'ctrl + s\'"
#define ZERODIV_DESC    "Generates a zero divison exception"
#define INVALID_OP_DESC "Generates an invalid operation code exception"
#define TIME_DESC       "Prints current system time"
#define PRIMES_DESC     "Prints prime numbers"
#define FIBONACCI_DESC  "Prints numbers in the fibonacci sequence"
#define PRINTMEM_DESC   "Prints the 32 bytes following the received address.\nUse example: printmem C0FFEE"
#define PIPE_DESC       "Divides the screen and runs 2 programs.\n"\
                        VALUE_TO_STRING(LEFTCMD_PAUSE_KEY)" pauses left side, "\
                        VALUE_TO_STRING(RIGHTCMD_PAUSE_KEY)" pauses right side.\n"\
                        VALUE_TO_STRING(CMD_STOP_KEY)" stops execution."

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

// Assembly routines
extern void zerodiv();
extern void invalidopcode();

#endif//_SHELL_H
