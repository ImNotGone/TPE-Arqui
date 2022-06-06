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

#define ARGUMENT_MISSING -1
#define NOT_PRINTMEM -2
#define INVALID_ADDRESS -3

extern void zerodiv();
extern void invalidopcode();

#endif//_SHELL_H
