extern int GLOBAL_VIDEO_ADDRESS = 0;
extern int posX = 1;
extern int posY = 1;
#include "types.h"



uint8_t inb (uint16_t _port);

void outb (uint16_t _port, uint8_t value);
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);



void update_cursor(uint16_t x, uint16_t y);


void disable_cursor();


void print_char(char ch);




void print(char* str);
