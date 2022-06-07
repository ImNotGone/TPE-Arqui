#include <time.h>
#include <syscalls.h>

void printTime() {
    TTime t;
    systime(&t);
    printf("Today is: %d/%d/%d\nThe actual time is: %dhs %dm %ds\n", t.day, t.month, t.year, t.hour, t.min, t.sec);
}