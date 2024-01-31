	.file	"kernel.cpp"
	.text
.Ltext0:
	.file 0 "/home/alexander/\320\240\320\260\320\261\320\276\321\207\320\270\320\271 \321\201\321\202\320\276\320\273/Projects/os_daedalus" "kernel.cpp"
	.globl	GLOBAL_VIDEO_ADDRESS
	.bss
	.align 4
	.type	GLOBAL_VIDEO_ADDRESS, @object
	.size	GLOBAL_VIDEO_ADDRESS, 4
GLOBAL_VIDEO_ADDRESS:
	.zero	4
	.globl	posX
	.data
	.align 4
	.type	posX, @object
	.size	posX, 4
posX:
	.long	1
	.globl	posY
	.align 4
	.type	posY, @object
	.size	posY, 4
posY:
	.long	1
	.section	.rodata
.LC0:
	.string	"9999"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.file 1 "kernel.cpp"
	.loc 1 6 23
	.cfi_startproc
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x7c,0x6
	subl	$4, %esp
	.loc 1 9 15
	call	_Z14disable_cursorv
	.loc 1 11 10
	subl	$12, %esp
	pushl	$.LC0
	call	_Z5printPc
	addl	$16, %esp
	.loc 1 13 5
	nop
	.loc 1 14 1
	movl	-4(%ebp), %ecx
	.cfi_def_cfa 1, 0
	leave
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	_Z3inbt
	.type	_Z3inbt, @function
_Z3inbt:
.LFB1:
	.loc 1 17 30
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$20, %esp
	movl	8(%ebp), %eax
	movw	%ax, -20(%ebp)
	.loc 1 19 5
	movzwl	-20(%ebp), %eax
	movl	%eax, %edx
#APP
# 19 "kernel.cpp" 1
	inb %dx, %al
# 0 "" 2
#NO_APP
	movb	%al, -1(%ebp)
	.loc 1 20 12
	movzbl	-1(%ebp), %eax
	.loc 1 21 1
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	_Z3inbt, .-_Z3inbt
	.globl	_Z4outbth
	.type	_Z4outbth, @function
_Z4outbth:
.LFB2:
	.loc 1 23 43
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movw	%dx, -4(%ebp)
	movb	%al, -8(%ebp)
	.loc 1 24 5
	movzwl	-4(%ebp), %edx
	movzbl	-8(%ebp), %eax
#APP
# 24 "kernel.cpp" 1
	outb %al, %dx
# 0 "" 2
	.loc 1 25 1
#NO_APP
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	_Z4outbth, .-_Z4outbth
	.globl	_Z13enable_cursorhh
	.type	_Z13enable_cursorhh, @function
_Z13enable_cursorhh:
.LFB3:
	.loc 1 28 1
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movb	%dl, -4(%ebp)
	movb	%al, -8(%ebp)
	.loc 1 30 6
	pushl	$10
	pushl	$980
	call	_Z4outbth
	addl	$8, %esp
	.loc 1 31 18
	pushl	$981
	call	_Z3inbt
	addl	$4, %esp
	.loc 1 31 26 discriminator 1
	andl	$-64, %eax
	.loc 1 31 34 discriminator 1
	orb	-4(%ebp), %al
	.loc 1 31 6 discriminator 1
	movzbl	%al, %eax
	pushl	%eax
	pushl	$981
	call	_Z4outbth
	addl	$8, %esp
	.loc 1 33 6
	pushl	$11
	pushl	$980
	call	_Z4outbth
	addl	$8, %esp
	.loc 1 34 18
	pushl	$981
	call	_Z3inbt
	addl	$4, %esp
	.loc 1 34 26 discriminator 1
	andl	$-32, %eax
	.loc 1 34 34 discriminator 1
	orb	-8(%ebp), %al
	.loc 1 34 6 discriminator 1
	movzbl	%al, %eax
	pushl	%eax
	pushl	$981
	call	_Z4outbth
	addl	$8, %esp
	.loc 1 35 1
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	_Z13enable_cursorhh, .-_Z13enable_cursorhh
	.globl	_Z13update_cursortt
	.type	_Z13update_cursortt, @function
_Z13update_cursortt:
.LFB4:
	.loc 1 41 1
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	8(%ebp), %edx
	movl	12(%ebp), %eax
	movw	%dx, -20(%ebp)
	movw	%ax, -24(%ebp)
	.loc 1 42 20
	movzwl	-24(%ebp), %ecx
	movzwl	%cx, %eax
	imull	$52429, %eax, %eax
	shrl	$16, %eax
	movl	%eax, %edx
	shrw	$4, %dx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	subl	%eax, %ecx
	movl	%ecx, %edx
	.loc 1 42 31
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$4, %eax
	movl	%eax, %edx
	.loc 1 42 11
	movzwl	-20(%ebp), %eax
	addl	%edx, %eax
	movw	%ax, -2(%ebp)
	.loc 1 43 6
	pushl	$15
	pushl	$980
	call	_Z4outbth
	addl	$8, %esp
	.loc 1 44 6
	movzwl	-2(%ebp), %eax
	movzbl	%al, %eax
	pushl	%eax
	pushl	$981
	call	_Z4outbth
	addl	$8, %esp
	.loc 1 45 6
	pushl	$14
	pushl	$980
	call	_Z4outbth
	addl	$8, %esp
	.loc 1 46 30
	movzwl	-2(%ebp), %eax
	shrw	$8, %ax
	.loc 1 46 6
	movzbl	%al, %eax
	pushl	%eax
	pushl	$981
	call	_Z4outbth
	addl	$8, %esp
	.loc 1 47 1
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	_Z13update_cursortt, .-_Z13update_cursortt
	.globl	_Z14disable_cursorv
	.type	_Z14disable_cursorv, @function
_Z14disable_cursorv:
.LFB5:
	.loc 1 51 1
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	.loc 1 52 6
	pushl	$10
	pushl	$980
	call	_Z4outbth
	addl	$8, %esp
	.loc 1 53 6
	pushl	$32
	pushl	$981
	call	_Z4outbth
	addl	$8, %esp
	.loc 1 54 1
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	_Z14disable_cursorv, .-_Z14disable_cursorv
	.globl	_Z10print_charc
	.type	_Z10print_charc, @function
_Z10print_charc:
.LFB6:
	.loc 1 58 1
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$4, %esp
	movl	8(%ebp), %eax
	movb	%al, -4(%ebp)
	.loc 1 59 5
	movl	GLOBAL_VIDEO_ADDRESS, %eax
	movl	%eax, %edx
	.loc 1 59 34
	movzbl	-4(%ebp), %eax
	movb	%al, (%edx)
	.loc 1 60 26
	movl	GLOBAL_VIDEO_ADDRESS, %eax
	addl	$1, %eax
	movl	%eax, GLOBAL_VIDEO_ADDRESS
	.loc 1 61 5
	movl	GLOBAL_VIDEO_ADDRESS, %eax
	.loc 1 61 34
	movb	$5, (%eax)
	.loc 1 62 26
	movl	GLOBAL_VIDEO_ADDRESS, %eax
	addl	$1, %eax
	movl	%eax, GLOBAL_VIDEO_ADDRESS
	.loc 1 63 17
	movl	posX, %eax
	addl	$1, %eax
	.loc 1 63 10
	movl	%eax, posX
	.loc 1 64 13
	movl	posX, %eax
	.loc 1 64 5
	cmpl	$79, %eax
	jle	.L11
	.loc 1 66 22
	movl	posY, %eax
	leal	1(%eax), %ecx
	.loc 1 66 27
	movl	$-2004318071, %edx
	movl	%ecx, %eax
	imull	%edx
	leal	(%edx,%ecx), %eax
	sarl	$4, %eax
	movl	%ecx, %edx
	sarl	$31, %edx
	subl	%edx, %eax
	imull	$30, %eax, %edx
	movl	%ecx, %eax
	subl	%edx, %eax
	.loc 1 66 14
	movl	%eax, posY
	.loc 1 67 21
	movl	posX, %eax
	subl	$80, %eax
	.loc 1 67 14
	movl	%eax, posX
.L11:
	.loc 1 71 1
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	_Z10print_charc, .-_Z10print_charc
	.globl	_Z5printPc
	.type	_Z5printPc, @function
_Z5printPc:
.LFB7:
	.loc 1 77 1
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	.loc 1 78 15
	pushl	$114
	call	_Z10print_charc
	addl	$4, %esp
	.loc 1 79 5
	jmp	.L13
.L14:
	.loc 1 81 20
	movl	8(%ebp), %eax
	movzbl	(%eax), %eax
	.loc 1 81 19
	movsbl	%al, %eax
	pushl	%eax
	call	_Z10print_charc
	addl	$4, %esp
	.loc 1 82 9
	addl	$1, 8(%ebp)
.L13:
	.loc 1 79 11
	movl	8(%ebp), %eax
	movzbl	(%eax), %eax
	.loc 1 79 16
	testb	%al, %al
	jne	.L14
	.loc 1 84 1
	nop
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	_Z5printPc, .-_Z5printPc
.Letext0:
	.file 2 "kernel.h"
	.file 3 "types.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x1e3
	.value	0x5
	.byte	0x1
	.byte	0x4
	.long	.Ldebug_abbrev0
	.uleb128 0x9
	.long	.LASF24
	.byte	0x21
	.long	.LASF0
	.long	.LASF1
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x3
	.long	.LASF2
	.byte	0x1
	.long	0x36
	.uleb128 0x5
	.byte	0x3
	.long	GLOBAL_VIDEO_ADDRESS
	.uleb128 0xa
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.long	.LASF3
	.byte	0x2
	.long	0x36
	.uleb128 0x5
	.byte	0x3
	.long	posX
	.uleb128 0x3
	.long	.LASF4
	.byte	0x3
	.long	0x36
	.uleb128 0x5
	.byte	0x3
	.long	posY
	.uleb128 0x6
	.long	.LASF6
	.byte	0x1
	.byte	0x17
	.long	0x68
	.uleb128 0x4
	.byte	0x1
	.byte	0x8
	.long	.LASF5
	.uleb128 0x6
	.long	.LASF7
	.byte	0x2
	.byte	0x18
	.long	0x7a
	.uleb128 0x4
	.byte	0x2
	.byte	0x7
	.long	.LASF8
	.uleb128 0x5
	.long	.LASF10
	.byte	0x4c
	.long	.LASF12
	.long	.LFB7
	.long	.LFE7-.LFB7
	.uleb128 0x1
	.byte	0x9c
	.long	0xa8
	.uleb128 0x2
	.string	"str"
	.byte	0x4c
	.byte	0x12
	.long	0xa8
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.long	0xae
	.uleb128 0x4
	.byte	0x1
	.byte	0x6
	.long	.LASF9
	.uleb128 0x7
	.long	.LASF11
	.byte	0x39
	.long	.LASF13
	.long	.LFB6
	.long	.LFE6-.LFB6
	.uleb128 0x1
	.byte	0x9c
	.long	0xdb
	.uleb128 0x2
	.string	"ch"
	.byte	0x39
	.byte	0x16
	.long	0xae
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.byte	0
	.uleb128 0xc
	.long	.LASF25
	.byte	0x1
	.byte	0x32
	.byte	0x6
	.long	.LASF26
	.long	.LFB5
	.long	.LFE5-.LFB5
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x5
	.long	.LASF14
	.byte	0x28
	.long	.LASF15
	.long	.LFB4
	.long	.LFE4-.LFB4
	.uleb128 0x1
	.byte	0x9c
	.long	0x130
	.uleb128 0x2
	.string	"x"
	.byte	0x28
	.byte	0x1d
	.long	0x6f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x2
	.string	"y"
	.byte	0x28
	.byte	0x29
	.long	0x6f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x8
	.string	"pos"
	.byte	0x2a
	.byte	0xb
	.long	0x6f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -10
	.byte	0
	.uleb128 0x5
	.long	.LASF16
	.byte	0x1b
	.long	.LASF17
	.long	.LFB3
	.long	.LFE3-.LFB3
	.uleb128 0x1
	.byte	0x9c
	.long	0x165
	.uleb128 0x1
	.long	.LASF18
	.byte	0x1b
	.byte	0x1c
	.long	0x5d
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x1
	.long	.LASF19
	.byte	0x1b
	.byte	0x32
	.long	0x5d
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x7
	.long	.LASF20
	.byte	0x17
	.long	.LASF21
	.long	.LFB2
	.long	.LFE2-.LFB2
	.uleb128 0x1
	.byte	0x9c
	.long	0x19a
	.uleb128 0x1
	.long	.LASF22
	.byte	0x17
	.byte	0x15
	.long	0x6f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x1
	.long	.LASF23
	.byte	0x17
	.byte	0x24
	.long	0x5d
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0xd
	.string	"inb"
	.byte	0x1
	.byte	0x11
	.byte	0x9
	.long	.LASF27
	.long	0x5d
	.long	.LFB1
	.long	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.long	0x1d4
	.uleb128 0x1
	.long	.LASF22
	.byte	0x11
	.byte	0x17
	.long	0x6f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x8
	.string	"rv"
	.byte	0x12
	.byte	0x13
	.long	0x68
	.uleb128 0x2
	.byte	0x91
	.sleb128 -9
	.byte	0
	.uleb128 0xe
	.long	.LASF28
	.byte	0x1
	.byte	0x6
	.byte	0x11
	.long	.LFB0
	.long	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 2
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 16
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 3
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7a
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x1f
	.uleb128 0x1b
	.uleb128 0x1f
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7a
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.long	0x1c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.value	0
	.value	0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF3:
	.string	"posX"
.LASF4:
	.string	"posY"
.LASF2:
	.string	"GLOBAL_VIDEO_ADDRESS"
.LASF25:
	.string	"disable_cursor"
.LASF15:
	.string	"_Z13update_cursortt"
.LASF10:
	.string	"print"
.LASF20:
	.string	"outb"
.LASF24:
	.string	"GNU C++17 13.2.1 20230801 -m32 -mtune=generic -march=x86-64 -g -ffreestanding -fno-pie"
.LASF21:
	.string	"_Z4outbth"
.LASF26:
	.string	"_Z14disable_cursorv"
.LASF9:
	.string	"char"
.LASF28:
	.string	"main"
.LASF6:
	.string	"uint8_t"
.LASF5:
	.string	"unsigned char"
.LASF18:
	.string	"cursor_start"
.LASF16:
	.string	"enable_cursor"
.LASF27:
	.string	"_Z3inbt"
.LASF23:
	.string	"value"
.LASF8:
	.string	"short unsigned int"
.LASF19:
	.string	"cursor_end"
.LASF14:
	.string	"update_cursor"
.LASF7:
	.string	"uint16_t"
.LASF13:
	.string	"_Z10print_charc"
.LASF17:
	.string	"_Z13enable_cursorhh"
.LASF11:
	.string	"print_char"
.LASF22:
	.string	"_port"
.LASF12:
	.string	"_Z5printPc"
	.section	.debug_line_str,"MS",@progbits,1
.LASF0:
	.string	"kernel.cpp"
.LASF1:
	.string	"/home/alexander/\320\240\320\260\320\261\320\276\321\207\320\270\320\271 \321\201\321\202\320\276\320\273/Projects/os_daedalus"
	.ident	"GCC: (GNU) 13.2.1 20230801"
	.section	.note.GNU-stack,"",@progbits
