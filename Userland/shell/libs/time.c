#include <time.h>

void printTime() {
    uint32_t dateData[DATE_DATA_SIZE];
    systime(dateData);
    printf("El dia de hoy es: %d/%d/%d\nLa hora actual es: %dhs %dm %ds\n", dateData[YEAR], dateData[MONTH], dateData[DAY], dateData[HOUR], dateData[MINUTE], dateData[SECOND]);
}