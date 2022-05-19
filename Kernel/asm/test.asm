section .text
GLOBAL testWrite
GLOBAL testRead
EXTERN ncPrint
testRead:
    push rbp
    mov rbp, rsp

    mov rax, 0
    mov rdi, 0
    mov rsi, buff
    mov rdx, 6
    mov r10, 0
    mov r8, 0
    int 80h

    

    mov rsp, rbp
    pop rbp
    ret

testWrite:
    push rbp
    mov rbp, rsp
    mov rax, 1
    mov rdi, 1
    mov rsi, buff
    mov rdx, 6
    mov r10, 1
    mov r8, 1
    int 80h

    mov rsp, rbp
    pop rbp
    ret

section .bss
    buff resb 128

section .data
    msg db "llegue hasta aca", 00