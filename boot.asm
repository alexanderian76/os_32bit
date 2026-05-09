[org 0x7c00]                        
KERNEL_LOCATION equ 0x1000
                                    

mov [BOOT_DISK], dl                 

                                    
xor ax, ax                          
mov es, ax
mov ds, ax
mov bp, 0x8000
mov sp, bp


; Для флешки нужно:
; 1. Сбросить систему дисков (это помогает с флешками)
mov ah, 0x00
mov dl, [BOOT_DISK]
int 0x13

; 2. Загружать больше секторов (LBA-адресация вместо CHS)
; Используем расширенное чтение (int 0x13 с AH=0x42)
; Проверяем поддержку LBA
mov ah, 0x41
mov bx, 0x55AA
mov dl, [BOOT_DISK]
int 0x13
jc .use_chs  ; если не поддерживается, используем CHS (старый метод)

; LBA чтение (рекомендуется для флешек)
mov si, DAPACK  ; адрес структуры Disk Address Packet
mov ah, 0x42
mov dl, [BOOT_DISK]
int 0x13
;jc .read_error
jmp .read_ok

.use_chs:

mov bx, KERNEL_LOCATION
mov dh, 40 ; количество секторов, загружаемых с диска

mov ah, 0x02
mov al, dh 
mov ch, 0x00
mov dh, 0x00
mov cl, 0x02
mov dl, [BOOT_DISK]
int 0x13                ; no error management, do your homework!

.read_ok:                             
mov ah, 0x0
mov al, 0x3
int 0x10                ; text mode
;mov ax, 0x0013 ; это для графического режима
;int 0x10

CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

cli
lgdt [GDT_descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE_SEG:start_protected_mode

jmp $
                                    
BOOT_DISK: db 0

; Структура для LBA чтения
align 4
DAPACK:
    db 0x10        ; размер структуры (16 байт)
    db 0           ; зарезервировано
    db 40          ; количество секторов для чтения (как в вашем коде)
    db 0           ; зарезервировано
    dw KERNEL_LOCATION  ; смещение буфера
    dw 0           ; сегмент буфера (0)
    dq 1         

GDT_start:
    GDT_null:
        dd 0x0
        dd 0x0

    GDT_code:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10011010
        db 0b11001111
        db 0x0

    GDT_data:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0

GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1
    dd GDT_start


[bits 32]
%ifndef KERNEL_SIZE
%error "KERNEL_SIZE not defined"
%endif
start_protected_mode:

section .text
start:
    ; 1. Перемещаем ядро на его финальный адрес 0x100000
    mov esi, 0x1000       ; Откуда копируем (физический адрес загрузки)
    mov edi, 0x100000     ; Куда копируем (виртуальный адрес ядра)
        
    mov ecx, KERNEL_SIZE     ; Размер ядра в байтах
    cld
    rep movsb


    mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov ebp, 0x90000		; 32 bit stack base pointer
	mov esp, ebp

    ; 3. Прыгаем на перемещенную функцию kernel_main
    jmp 0x100000  ; Переход по абсолютному адресу 0x100000 + смещение kernel_main



 ;   jmp KERNEL_LOCATION

                                     
 
times 510-($-$$) db 0              
dw 0xaa55