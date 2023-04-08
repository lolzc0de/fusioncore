%include "fckrnl/asm/helper.asm"

extern isr_handler

global _isr_handler
global _isr_vector

_isr_handler:
	cld
	pusha64

	mov rdi, rsp
	call isr_handler
	mov rsp, rax

	popa64
	add rsp, 16

	iretq

%macro ISR_NAME 1
dq _ISR%1
%endmacro

%macro ISR_ERR 1
_ISR%1:
	push %1
	jmp _isr_handler
%endmacro

%macro ISR_NO_ERR 1
_ISR%1:
	push 0
	push %1
	jmp _isr_handler
%endmacro

; Exceptions
ISR_NO_ERR 0
ISR_NO_ERR 1
ISR_NO_ERR 2
ISR_NO_ERR 3
ISR_NO_ERR 4
ISR_NO_ERR 5
ISR_NO_ERR 6
ISR_NO_ERR 7
ISR_ERR 8
ISR_NO_ERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NO_ERR 15
ISR_NO_ERR 16
ISR_ERR 17
ISR_NO_ERR 18
ISR_NO_ERR 19
ISR_NO_ERR 20
ISR_ERR 21
ISR_NO_ERR 22
ISR_NO_ERR 23
ISR_NO_ERR 24
ISR_NO_ERR 25
ISR_NO_ERR 26
ISR_NO_ERR 27
ISR_NO_ERR 28
ISR_NO_ERR 29
ISR_NO_ERR 30
ISR_NO_ERR 31

; standard ISA IRQs
ISR_NO_ERR 32
ISR_NO_ERR 33
ISR_NO_ERR 34
ISR_NO_ERR 35
ISR_NO_ERR 36
ISR_NO_ERR 37
ISR_NO_ERR 38
ISR_NO_ERR 39
ISR_NO_ERR 40
ISR_NO_ERR 41
ISR_NO_ERR 42
ISR_NO_ERR 43
ISR_NO_ERR 44
ISR_NO_ERR 45
ISR_NO_ERR 46
ISR_NO_ERR 47

; remaining IRQs
%assign i 48
%rep 256
ISR_NO_ERR i
%assign i i+1
%endrep

_isr_vector:
%assign i 0
%rep 256
	ISR_NAME i
%assign i i+1
%endrep