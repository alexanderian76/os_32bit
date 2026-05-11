[org 0x7c00]
[bits 16]

start:
    ; Настройка сегментов
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    
    ; Сохраняем диск
    mov [disk], dl
    
    ; Очистка экрана
    mov ax, 0x0003
    int 0x10
    
    ; Вывод сообщения
    mov si, msg
    call print
    
    ; Задержка для USB
    mov cx, 0xFFFF
.delay:
    loop .delay
    
    ; Пытаемся прочитать сектор (всего 1 сектор для теста)
    mov ah, 0x02
    mov al, 1        ; 1 сектор
    mov ch, 0
    mov cl, 2        ; 2-й сектор
    mov dh, 0
    mov dl, [disk]
    mov bx, 0x1000   ; буфер
    int 0x13
    jc .error
    
    ; Успех
    mov si, ok_msg
    call print
    
    ; Бесконечный цикл (не переходим в protected mode)
    jmp $

.error:
    mov si, error_msg
    call print
    mov al, ah
    call print_hex
    jmp $

print:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print
.done:
    ret

print_hex:
    push ax
    shr al, 4
    call print_nibble
    pop ax
    and al, 0x0F
print_nibble:
    add al, '0'
    cmp al, '9'
    jle .digit
    add al, 7
.digit:
    mov ah, 0x0E
    int 0x10
    ret

msg db "Booting...", 13, 10, 0
ok_msg db "OK!", 0
error_msg db "Error: ", 0
disk db 0

times 510-($-$$) db 0
dw 0xAA55