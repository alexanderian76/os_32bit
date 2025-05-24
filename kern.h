
#include <stdint.h>
// IDT entry structure
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

// IDT descriptor structure
struct idt_descriptor {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void remap_pic();
void keyboard_handler();
extern void start();
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void print_char(char);
void print(char*);
void update_cursor(uint16_t x, uint16_t y);
void disable_cursor();
int posX = 1;
int posY = 1;


uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

extern void loadPageDirectory(unsigned int*);
extern void enablePaging();
