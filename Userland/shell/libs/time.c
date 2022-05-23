#include <time.h>
#include <syscalls.h>

void printTime() {
    Ttime t;
    systime(&t);
    printf("El dia de hoy es: %d/%d/%d\nLa hora actual es: %dhs %dm %ds\n", t.year, t.month, t.day, t.hour, t.min, t.sec);
}