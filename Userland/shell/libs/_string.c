#include <stdint.h>

// ---- Auxiliary Macros for internal use ----------------------------
#define IS_ALPHA(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define IS_DIGIT(c) ((c) <= '9' && (c) >= '0')
#define TO_UPPER(c) ((c) - (((c) >= 'a' && (c) <= 'z') ? 'a'-'A' : 0))
#define VALID_NUM_FOR_BASE(c, base) (((base) <= 10 && ((c) >= '0' && (c) < '0' + ((base) <= 10 ? (base) : 10))) || ((base) > 10 && (IS_DIGIT(c) || (((c) >= 'a' && (c) < 'a' + ((base) - 10)) || ((c) >= 'A' && (c) < 'A' + ((base) - 10))))))
#define VALUE_OF(x) (TO_UPPER(x) - (IS_DIGIT(x) ? '0' : 55))

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

    // Base invalida
    if ((base < 2 || base > 36) || str[0] == '\0' || !VALID_NUM_FOR_BASE(str[0], base))
        return -1;


    int digit;
    int64_t number = 0;
    int negative = 0;
    int finished = 0;
    int i = 0;

    // ignoramos espacios y el +
    while (str[i] == ' ' || str[i] == '+')
        i++;

    // numero negativo
    if (str[i] == '-') {
        negative = 1;
        i++;
    }

    // Salteo el 0x de los numeros hexa
    if (base == 16 && str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X'))
        i += 2;

    // Salteo el 0 de los octales
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


    // Calculo el numero, si aparece un caracter que no es digito en la base con la que operamos, salimos
    for (; str[i] != '\0' && !finished; i++) {
        digit = -1;
        if (VALID_NUM_FOR_BASE(str[i], base))
            digit = VALUE_OF(str[i]);

        if (digit < base && digit != -1)
            number = number * base + digit * (negative ? -1 : 1);
        else
            finished = 1;
    }


    if (!IS_DIGIT(str[i - 1]) && !IS_ALPHA(str[i - 1]))
        i = 0;

    if (finished)
        i--;

    if (endptr)
        *endptr = &str[i];

    return number;
}

void trim(char *str) {
    uint64_t i, j, k;
    for (i = 0; str[i] == ' '; i++);

    if (str[i] == '\0')
        return;

    for (j = strlen(str) - 1; j >= 0 && str[j] == ' '; j--);

    for (k = 0; i <= j; k++, i++)
        str[k] = str[i];

    str[k] = '\0';

}

int64_t strDivide(const char * str, char * leftBuff, char * rightBuff, char divider) {
    int found = 0;
    char * toInsert = leftBuff;
    int i,j;
    for (i = 0, j = 0; str[i] != '\0'; i++) {
        if (str[i] == divider && !found) {
            found = 1;
            toInsert = rightBuff;
            leftBuff[j] = '\0';
            j = 0;
            i++;
            while (str[i] == ' ') // ignoro espacios
                i++;
        }
        toInsert[j++] = str[i];
    }
    toInsert[j] = '\0';

    trim(leftBuff); // ignoro espacios
    trim(rightBuff);

    return found;
}


