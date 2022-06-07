; polling
section .text
global sys_getKey
sys_getKey:
    push rbp
    mov rbp, rsp
    mov rax,0

    ; no hace falta fijarse si hay algo para leer
    ; porque me mando a leer la interrupcion de teclado
    ; leo la tecla
    in al, 0x60

    mov rsp, rbp
    pop rbp
    ret