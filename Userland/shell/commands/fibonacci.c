#include <fibonacci.h>
#include <_stdio.h>

// Fibonacci
typedef struct fiboInternalState {
    uint64_t i;
    uint64_t prev;
    uint64_t current;
    uint64_t next;
} fiboInternalState;

static fiboInternalState fiboInternalStates[] = {{0, 0, 1, 1}, {0, 0, 1, 1}};

void fiboNext(int64_t screen) {
    if (fiboInternalStates[screen].i == 0) {
        printf("fibonacci(%d) = %d\n", 0, 0);
        fiboInternalStates[screen].i++;
        return;
    }

    printf("fibonacci(%d) = %d\n", fiboInternalStates[screen].i, fiboInternalStates[screen].current);
    fiboInternalStates[screen].i++;
    fiboInternalStates[screen].prev = fiboInternalStates[screen].current;
    fiboInternalStates[screen].current = fiboInternalStates[screen].next;
    fiboInternalStates[screen].next = fiboInternalStates[screen].prev + fiboInternalStates[screen].current;
}

bool fiboHasNext(int64_t screen) {
    return fiboInternalStates[screen].current <= UINT64_MAX - fiboInternalStates[screen].prev; // prevents overflow
}

void fiboReset(int64_t screen) {
    fiboInternalStates[screen].i = 0;
    fiboInternalStates[screen].prev = 0;
    fiboInternalStates[screen].current = 1;
    fiboInternalStates[screen].next = 1;
}