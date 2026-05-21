section .text
[bits 32]
global _start
global stack_bottom
global stack_top
extern kernel_main

; Определяем стек
section .bss
align 16

stack_bottom:
    resb 65536      ; 64KB стек
stack_top:

section .text
_start:
    ; Устанавливаем указатель стека
    mov esp, stack_top
    
    ; Сохраняем границы стека (опционально)
    push dword stack_bottom
    push dword stack_top
    
    call kernel_main
    
    ; Если вернулись - зависаем
    cli
.hang:
    hlt
    jmp .hang