#include <stdint.h>

#define IS_ALPHA(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#define IS_DIGIT(c) (c <= '9' && c >= '0')
#define TO_UPPER(c) (c - ((c >= 'a' && c <= 'z') ? 'a'-'A' : 0))


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
    while (*str1 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

// https://stackoverflow.com/questions/17251272/implementation-of-strtol-function-cast
int64_t strtol(const char *str, const char **endptr, int base) {

    if ((base < 2 || base > 36))
        return 0;

    int digit;
    int64_t number = 0;
    int negative = 0;
    int finished = 0;
    int i = 0;

    if (str[i] == '\0')
        return 0;


    while (str[i] == ' ' || str[i] == '+')
        i++;

    if (str[i] == '-') {
        negative = 1;
        i++;
    }

    if (base == 16 && str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X'))
        i += 2;

    if (base == 8 && str[i] == '0')
        i++;

    // Base autodetectada
    if (base == 0) {
        if (str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X'))
            base = 16;
        else if (str[i] == '0')
            base = 8;
        else
            base = 10;

        i += base == 16 ? 2 : (base == 8 ? 1 : 0);
    }


    for (; str[i] != '\0' && !finished; i++) {
        digit = -1;
        if (IS_DIGIT(str[i]) || IS_ALPHA(str[i]))
            digit = TO_UPPER(str[i]) - (IS_DIGIT(str[i] ? '0' : 55));

        if (digit < base && digit != -1)
            number = number * base + digit * (negative ? -1 : 1);
        else
            finished = 1;
    }


    if (!IS_DIGIT(str[i - 1]) && !IS_ALPHA(str[i - 1]))
        i = 0;

    if (endptr)
        *endptr = &str[i - 1];

    return number;
}
int64_t strDivide(const char * str, char * buff1, char * buff2, char divider) {
    int found = 0;
    char * toInsert = buff1;
    int i,j;
    for (i = 0, j = 0; str[i] != '\0'; i++) {
        if (str[i] == divider && !found) {
            found = 1;
            toInsert = buff2;
            buff1[j] = '\0';
            j = 0;
            i++;
        }
        toInsert[j++] = str[i];
    }
    buff2[j] = '\0';
    return found;
}


