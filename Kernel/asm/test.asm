section .text
GLOBAL testWrite
testWrite:
    push rbp
    mov rbp, rsp

    mov rax, 1
    mov rdi, STDOUT
    mov rsi, hello
    mov rdx, 6
    mov r10, 0
    mov r8, 0
    int 80h

    mov rax, 1
    mov rdi, STDERR
    mov rsi, error
    mov rdx, 6
    mov r10, 1
    mov r8, 1
    int 80h

    mov rsp, rbp
    pop rbp
    ret

section .data
    hello db "hello", 0Ah
    lhello equ $-hello
    error db "error", 0Ah
    lerror equ $-error
    STDOUT equ 1
    STDERR equ 2

