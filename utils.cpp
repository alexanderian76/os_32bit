#include "utils.h"

uint8_t inb (uint16_t _port) {
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outb (uint16_t _port, uint8_t value) {
    __asm__ __volatile__ ("outb %1, %0" : :  "dN" (_port), "a" (value));
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
    //
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}




void update_cursor(uint16_t x, uint16_t y)
{
	uint16_t pos = (y % 20) * 80 + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
