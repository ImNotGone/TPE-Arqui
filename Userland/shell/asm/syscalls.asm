section .data
    syscallread     equ 0
    syscallwrite    equ 1
    syscalltime     equ 2
section .text
global sysread
global syswrite
global systime

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
    
systime:
    push rbp
    mov rbp, rsp

    mov rax, syscalltime
    int 80h

    mov rsp, rbp
    pop rbp
    ret