
section .text
	[bits 32]

	xor eax, eax
	mov eax, 0h
    mov bh, 0x3
    mov bl, 'A'
    call print_char

    mov ax, 5h
    mov bx, 10h
   

    
    call move_cursor


	
	xor eax, eax
	mov eax, 0x2
	mov bh, 0x5
    mov bl, 'B'
    call print_char
	
	xor eax, eax
	mov eax, 0x4
	mov bh, 0x7
    mov bl, 'C'
    call print_char

	call print_alphabet

	mov ax, 0x0
    mov bx, 0x1a
   

    
    call move_cursor
	
	xor esi, esi
	mov esi, file_stage2_bin
	call puts

    jmp $

VGA equ 0xb8000


puts:
    push eax
    push ebx
	push edx
	xor eax, eax
	xor edx, edx
	mov eax, 0x5c
.loop_puts:
	mov bl, [esi + edx]
	cmp bl, 0x0
    je .done_puts
    mov bh, 0x3
    call print_char
	add edx, 0x1
	add eax, 0x2
    jmp .loop_puts
.done_puts:
	pop edx
    pop ebx
    pop eax
    ret

print_char:
    mov [VGA + eax], bx
    
    ret


move_cursor:
VGA.Width equ 80
 
.SetCoords:
; input bx = x, ax = y
; modifies ax, bx, dx
 
	mov dl, VGA.Width
	mul dl
	add bx, ax
 
.SetOffset:
; input bx = cursor offset
; modifies al, dx
 
	mov dx, 0x03D4
	mov al, 0x0F
	out dx, al
 
	inc dl
	mov al, bl
	out dx, al
 
	dec dl
	mov al, 0x0E
	out dx, al
 
	inc dl
	mov al, bh
	out dx, al
	ret


print_alphabet:
	xor eax, eax
	xor bx, bx
	mov bh, 0x3
	mov bl, 0x41
	.loop:
	call print_char
	add eax, 0x2
	inc bl
	cmp bh, 0x3
	jne .change_color
	mov bh, 0x5
	jmp .no_change_color
	.change_color:
	mov bh, 0x3
	.no_change_color:
	cmp bl, 0x5b
	jne .loop
    ret

file_stage2_bin:        db 'TEST STRING qweqweqwr 777'