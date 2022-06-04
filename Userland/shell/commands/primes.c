#include <primes.h>
#include <_stdio.h>

// Primes
typedef struct primesInternalState {
    uint64_t number;
} primeInternalState;

static primeInternalState primeInternalStates[] = {{1}, {1}};

static bool isPrime(int n) {
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

void primesNext(int screen) {
    int n = calculateNextPrime(primeInternalStates[screen].number);
    printf("-- %d\n", n);
    primeInternalStates[screen].number = n;
}

bool primesHasNext(int screen) {
    return primeInternalStates[screen].number <=UINT64_MAX;
}

void primesReset(int screen) {
    primeInternalStates[screen].number = 1;
}