#include <_stdio.h>
#include <_string.h>
// printf
// http://www.firmcodes.com/write-printf-function-c/
static char * convert(uint64_t num, uint64_t base) {
    static char REPRESENTATION[] = "0123456789ABCDEF";
    static char buffer[50];
    int i = 50;
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

int64_t vfprintf(uint64_t fd, const char * fmt, va_list args) {
    int64_t bytesWritten = 0;
    for(int64_t i = 0; fmt[i] != '\0'; i++) {
        if(fmt[i] != '%') fputchar(fd, fmt[i]);
        if(fmt[i] == '%' && fmt[i+1] != '\0') {
            // salteo el %
            i++;
            switch (fmt[i]) {
                case 'c': fputchar(fd, va_arg(args, int)); break;
                case 'd': vfprintf(fd, convert(va_arg(args, int), 10), args); break;
                case 'o': vfprintf(fd, "0", args); vfprintf(fd, convert(va_arg(args, int), 8), args); break;
                case 'x': vfprintf(fd, "0x", args); vfprintf(fd, convert(va_arg(args, int), 16), args); break;
                case 's': vfprintf(fd, va_arg(args, char *), args); break;
                case '%': fputchar(fd, '%'); break;

                // si *fmt[i] era 0 o si no tengo la key salgo
                default: bytesWritten = -1; return bytesWritten;
            }
        }
    }
    
    return bytesWritten;
}

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

int64_t scanf(const char * fmt, ...){
    return 0;
}

int64_t puts(const char * str) {
    int64_t out = syswrite(1, str, strlen(str));
    putchar('\n');
    return out + 1;
}

int64_t putchar(int64_t c){
    return fputchar(STDOUT, c);
}

int64_t getchar(){
    char c;
    while(sysread(STDIN, &c, 1) == 0);
    return c;
}
