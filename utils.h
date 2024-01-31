#include "types.h"

uint8_t inb (uint16_t _port);
void outb (uint16_t _port, uint8_t value);
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

void update_cursor(uint16_t x, uint16_t y);