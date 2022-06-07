#include <_stdio.h>
#include <_string.h>


// --------- Auxiliary macros and functions for internal use -------
#define IS_DIGIT(c) (c <= '9' && c >= '0')

static char * numToString(uint64_t num, uint64_t base) {
    static char REPRESENTATION[] = "0123456789ABCDEF";
    static char buffer[BUFF_SIZE];
    int i = BUFF_SIZE;
    buffer[--i] = '\0';
    if(num == 0) {
        buffer[--i] = '0';
        return &buffer[i];
    }

    while(num != 0) {
        buffer[--i] = REPRESENTATION[num%base];
        num /= base;
    }

    return &buffer[i];
}

static int64_t fputchar(uint64_t fd, int64_t c) {
    if(syswrite(fd, (char *)&c, 1) == -1) {
        return -1;
    }
    return c;
}

static int64_t vfprintf(uint64_t fd, const char * fmt, va_list args) {
    int64_t bytesWritten = 0;
    for(int64_t i = 0; fmt[i] != '\0'; i++) {
        if(fmt[i] != '%') fputchar(fd, fmt[i]);
        if(fmt[i] == '%' && fmt[i+1] != '\0') {
            // salteo el %
            i++;
            switch (fmt[i]) {
                case 'c': fputchar(fd, va_arg(args, int)); break;
                case 'd': vfprintf(fd, numToString(va_arg(args, uint64_t), 10), args); break;
                case 'o': vfprintf(fd, "0", args); vfprintf(fd, numToString(va_arg(args, uint64_t), 8), args); break;
                case 'x': vfprintf(fd, "0x", args); vfprintf(fd, numToString(va_arg(args, uint64_t), 16), args); break;
                case 's': vfprintf(fd, va_arg(args, char *), args); break;
                case '%': fputchar(fd, '%'); break;

                // si *fmt[i] era 0 o si no tengo la key salgo
                default: bytesWritten = -1; return bytesWritten;
            }
        }
    }
    
    return bytesWritten;
}

// ---------- Main lib functions ------------
// printf
// http://www.firmcodes.com/write-printf-function-c/
int64_t fprintf(uint64_t fd, const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    int64_t out = vfprintf(fd, fmt, args);
    
    va_end(args);
    return out;
}

int64_t printf(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    int64_t out = vfprintf(STDOUT, fmt, args);
    
    va_end(args);
    return out;
}

// scanf identifies strings, char, decimal, octal and hexadecimal
// https://iq.opengenus.org/how-printf-and-scanf-function-works-in-c-internally/
int64_t scanf(const char * fmt, ...){
    va_list vl;
    int j=0, resp = 0, buffLen = BUFF_SIZE;
    char buff[BUFF_SIZE] = {0}, c;
    const char *outPos;

    // Para que el usuario vea lo que esta escribiendo
    int i = 0;
    while((c = (int8_t)getchar()) != '\n') {
        if(c == '\b' && i > 0) {
            putchar(c);
            i--;
        }
        if(c != '\b' && i < BUFF_SIZE) {
            putchar(c);
            buff[i++] = c;
        }
    }
    buff[i] = 0;
    putchar('\n');

    // Scan del format
    va_start( vl, fmt );
    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%' && fmt[i + 1] != '\0') {
            i++; // salteo el %

            // Si hay un numero, el usuario quiere leer como maximo ese numero de caracteres, sino usamos BUFF_SIZE
            if (IS_DIGIT(fmt[i])) {
                buffLen = (int) strtol(&fmt[i], &outPos, 10);
                i += (int)(outPos - &fmt[i]);
            }


            switch (fmt[i]) {
                case 's': {
                    char * aux = (char *) va_arg( vl, char * );
                    int k;
                    for (k = 0; buff[k] != '\0' && k < buffLen - 1; k++)
                        aux[k] = buff[j++];
                    aux[k] = '\0';
                    break;
                }
                case 'c': {
                    *(char *)va_arg( vl, char* ) = buff[j];
                    j++;
                    resp++;
                    break;
                }
                case 'd': {
                    *(int *) va_arg( vl, int* ) = (int) strtol(&buff[j], &outPos, 10);
                    j += (int)(outPos - &buff[j]);
                    resp++;
                    break;
                }
                case 'x': {
                    *(int *) va_arg( vl, int* ) = (int) strtol(&buff[j], &outPos, 16);
                    j += (int)(outPos - &buff[j]);
                    resp++;
                    break;
                }
            }
        }
    }
    va_end(vl);
    return resp;
}

int64_t fputs(uint64_t fd, const char * str) {
    int64_t out = syswrite(fd, str, strlen(str));
    fputchar(fd, '\n');
    return out + 1;
}

int64_t puts(const char * str) {
    return fputs(STDOUT, str);
}

int64_t putchar(int64_t c){
    return fputchar(STDOUT, c);
}

int64_t getchar(){
    char c;
    while(sysread(STDIN, &c, 1) == 0);
    return c;
}
