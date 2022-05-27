section .data
    syscallread     equ 0
    syscallwrite    equ 1
    syscalltime     equ 2
    syscallmemdump  equ 3
    syscallregdump  equ 4

section .text
global sysread
global syswrite
global systime
global sysmemdump
global sysregdump


sysread:
    push rbp
    mov rbp, rsp

    mov rax, syscallread
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;
; rdi -> fd
; rsi -> buff
; rdx -> len
;
syswrite:
    push rbp
    mov rbp, rsp

    mov rax, syscallwrite
    int 80h

    mov rsp, rbp
    pop rbp
    ret

; rdi -> Ttime *
systime:
    push rbp
    mov rbp, rsp

    mov rax, syscalltime
    int 80h

    mov rsp, rbp
    pop rbp
    ret

; rdi -> direccion
; rsi -> memData[]
sysmemdump:
    push rbp
    mov rbp, rsp
    
    mov rax, syscallmemdump
    int 80h
    
    mov rsp, rbp
    pop rbp
    ret

; rdi -> TRegs *
sysregdump:
    push rbp
    mov rbp, rsp

    mov rax, syscallregdump
    int 80h

    mov rsp, rbp
    pop rbp
    ret