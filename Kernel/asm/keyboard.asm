; polling
section .text
global sys_getKey
sys_getKey:
    push rbp
    mov rbp, rsp
    mov rax,0

; no hace falta fijarse pq me mando una interrupcion   
;.loop:
    ;me fijo si hay algo para leer
    ;in al,0x64
    ;and al,0x01
    ;cmp al,0
    ;je .loop
    
    ; leo la tecla
    in al, 0x60

    mov rsp, rbp
    pop rbp
    ret