#include <_stdio.h>
#include <stdint.h>

uint64_t strlen(const char * str) {
    uint64_t i = 0;
    while(str[i] != '\0') {
        i++;
    }
    return i;
}

int64_t strncmp(const char * str1, const char * str2, uint64_t n) {
    while(n != 0 && *str1 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
        n--;
    }
    if(n == 0) return 0;
    return *str1 - *str2;
}

int64_t strcmp(const char * str1, const char * str2) {
    while(*str1 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}


