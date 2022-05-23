GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _syscallHandler
GLOBAL _exception0Handler

GLOBAL _exception6Handler

GLOBAL restartKernel

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscallDispatcher
EXTERN getStackBase
EXTERN main
SECTION .text

%macro pushState 0
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rsi
	push rdi
	push rbp
	push rdx
	push rcx
	push rbx
	push rax
%endmacro

%macro popState 0
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rbp
	pop rdi
	pop rsi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

%macro exceptionHandler 1
	pushState

	mov [exceptionRegisters + 8*0 ], rax
	mov [exceptionRegisters + 8*1 ], rbx
	mov [exceptionRegisters + 8*2 ], rcx
	mov [exceptionRegisters + 8*3 ], rdx
	mov [exceptionRegisters + 8*4 ], rsi
	mov [exceptionRegisters + 8*5 ], rdi
	mov [exceptionRegisters + 8*6 ], rbp
	mov rax, rsp
	add rax, 0x28
	mov [exceptionRegisters + 8*7 ], rax	; rsp
	mov [exceptionRegisters + 8*8 ], r8
	mov [exceptionRegisters + 8*9 ], r9
	mov [exceptionRegisters + 8*10], r10
	mov [exceptionRegisters + 8*11], r11
	mov [exceptionRegisters + 8*12], r12
	mov [exceptionRegisters + 8*13], r13
	mov [exceptionRegisters + 8*14], r14
	mov [exceptionRegisters + 8*15], r15
	mov rax, [rsp]
	mov [exceptionRegisters + 8*16], rax	; rip
	mov rax, [rsp+8]
	mov [exceptionRegisters + 8*17], rax	; rflags

	mov rdi, %1 ; pasaje de parametro
	mov rsi, exceptionRegisters
	call exceptionDispatcher

	popState
	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

_syscallHandler:
	pushState
	pop rax

	; reorganizo los registros para C
	mov r9, r8
	mov r8, r10
	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, rax
	call syscallDispatcher
	
	push rax
	popState
	iretq

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

_exception6Handler
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

restartKernel:
	call getStackBase
	mov rsp, rax	; reinicio el stack
	call main

SECTION .bss
	exceptionRegisters resq 18
