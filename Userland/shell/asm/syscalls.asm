section .data
    syscallread     equ 0
    syscallwrite    equ 1

section .text
global sysread
global syswrite


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
    
