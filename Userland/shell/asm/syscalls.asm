section .data
    syscallread     equ 0
    syscallwrite    equ 1
    syscalltime     equ 2
    syscallmemdump  equ 3
    syscallregdump  equ 4
    syscallDivWind  equ 5
    syscallSwiWind  equ 6

section .text
global sysread
global syswrite
global systime
global sysmemdump
global sysregdump
global sysDivWind
global sysSwiWind

%macro syscallHandler 1
    push rbp
    mov rbp, rsp

    mov rax, %1
    int 80h

    mov rsp, rbp
    pop rbp
    ret
%endmacro

sysread:
    syscallHandler syscallread

;
; rdi -> fd
; rsi -> buff
; rdx -> len
;
syswrite:
    syscallHandler syscallwrite

; rdi -> Ttime *
systime:
    syscallHandler syscalltime

; rdi -> direccion
; rsi -> memData[]
sysmemdump:
    syscallHandler syscallmemdump

; rdi -> TRegs *
sysregdump:
    syscallHandler syscallregdump

sysDivWind:
    syscallHandler syscallDivWind
sysSwiWind:
    syscallHandler syscallSwiWind