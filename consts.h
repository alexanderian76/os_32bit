#pragma once
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C
#define CTRL_KEY_CODE 0x1D

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

//*********************************************
#define NULL 0
#define BOX_SINGLELINE 1
#define BOX_DOUBLELINE 2
#define BOX_MAX_WIDTH 78
#define BOX_MAX_HEIGHT 23
#define VGA_MAX_WIDTH 80
#define VGA_MAX_HEIGHT 25




#define UP 0x48
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d



#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01


extern int GLOBAL_VIDEO_ADDRESS;