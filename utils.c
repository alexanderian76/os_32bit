#pragma once
#include "kernel.h"
#include "types.h"
#include "consts.h"
#include "main.h"


extern int posX = 1;
extern int posY = 1;

uint8_t inb (uint16_t port) {
    unsigned char rv;
    asm volatile ( "inb %1, %0"
                   : "=a"(rv)
                   : "Nd"(port)
                   : "memory");
    //__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outb (uint16_t port, uint8_t value) {
    asm volatile ( "outb %0, %1" : : "a"(value), "Nd"(port) : "memory");
   // __asm__ __volatile__ ("outb %1, %0" : :  "dN" (port), "a" (value));
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
    //
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
    return;
}




void update_cursor(uint16_t x, uint16_t y)
{
	uint16_t pos = y * 80 + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    return;
}


void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}
void print(char* str)
{
    while(*str != '\0')
    {
        print_char(*str);
        *str++;
    }
    return;
}

void print_char(char ch)
{
    *(char*)GLOBAL_VIDEO_ADDRESS = ch;
    GLOBAL_VIDEO_ADDRESS += 0x1;
    *(char*)GLOBAL_VIDEO_ADDRESS = 0x5;
    GLOBAL_VIDEO_ADDRESS += 0x1;
    posX = posX + 1;
    if(posX >= 80)
    {
        posY = (posY + 1);
        posX = posX - 80;
    }
  //  return;
   // move_cursor(posX, posY);
    update_cursor(posX, posY);
    return;
}







extern void __test__() {
    GLOBAL_VIDEO_ADDRESS = 0xb8000;
    print_char('1');
    while(1)
    {
        
    }
            
   
}

