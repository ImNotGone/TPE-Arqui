#include <time.h>
#include <syscalls.h>

void printTime() {
    TTime t;
    systime(&t);
    printf("El dia de hoy es: %d/%d/%d\nLa hora actual es: %dhs %dm %ds\n", t.day, t.month, t.year, t.hour, t.min, t.sec);
}