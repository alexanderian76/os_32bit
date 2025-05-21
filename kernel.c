#include "kern.h"
#include "keyboard_map.h"
int VIDEO_MEMORY;
// IDT and IDT descriptor
struct idt_entry idt[256];
struct idt_descriptor idt_desc;
// Main kernel function
void kernel_main()
{
    // Set up IDT
    VIDEO_MEMORY = 0xB8000;
    enable_cursor(0, 11);
    start();
    print("Hello world!");
    idt_desc.limit = sizeof(idt) - 1;
    idt_desc.base = (uint32_t)&idt;

    // Set up keyboard interrupt (IRQ1 -> INT 0x21)
    idt_set_gate(0x21, (uint32_t)keyboard_handler, 0x08, 0x8E);

    // Load IDT
    asm volatile("lidt %0" : : "m"(idt_desc));

    // Remap PIC
    remap_pic();

    // Enable interrupts
    asm volatile("sti");

    // Main loop
    while (1)
        ;
}
// Read a byte from a port
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Write a byte to a port
static inline void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Keyboard interrupt handler
void keyboard_handler()
{
    // Read the scancode from the keyboard data port (0x60)
    uint8_t scancode = inb(0x60);
    uint8_t statusCode = inb(0x64);
    // Print the scancode to the screen (for debugging)
  //  print_char(statusCode);
    if(keyboard_map[(unsigned char)scancode] > 0) {

     /*   *(char*)VIDEO_MEMORY = keyboard_map[(unsigned char)scancode];
                            // Print the scancode
        VIDEO_MEMORY += 0x1; // Move to the next character position
        *(char*)VIDEO_MEMORY = 0x5;
        VIDEO_MEMORY += 0x1;*/
        print_char(keyboard_map[(unsigned char)scancode]);
        // Send End of Interrupt (EOI) to the PIC
        update_cursor(((VIDEO_MEMORY - 0xb8000) / 2) % 80, (VIDEO_MEMORY - 0xb8000) / 160);
        
    }

    outb(0x20, 0x20);
    asm("sti");
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
	uint16_t pos = y * 80 + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}


// Set up an IDT gate
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

// Remap the PIC
void remap_pic()
{
    // Initialize PICs
    outb(0x20, 0x11); // ICW1: Initialize master PIC
    outb(0xA0, 0x11); // ICW1: Initialize slave PIC

    outb(0x21, 0x20); // ICW2: Master PIC vector offset (IRQ0 -> INT 0x20)
    outb(0xA1, 0x28); // ICW2: Slave PIC vector offset (IRQ8 -> INT 0x28)

    outb(0x21, 0x04); // ICW3: Tell master PIC about slave PIC at IRQ2
    outb(0xA1, 0x02); // ICW3: Tell slave PIC its cascade identity

    outb(0x21, 0x01); // ICW4: Enable 8086 mode on master PIC
    outb(0xA1, 0x01); // ICW4: Enable 8086 mode on slave PIC

    // Mask all interrupts except keyboard (IRQ1)
    outb(0x21, 0xFD); // Master PIC mask
    outb(0xA1, 0xFF); // Slave PIC mask
}

void print_char(char ch)
{
    *(char*)VIDEO_MEMORY = ch;
    VIDEO_MEMORY += 0x1;
    *(char*)VIDEO_MEMORY = 0x5;
    VIDEO_MEMORY += 0x1;
    posX = posX + 1;
    if(posX >= 80)
    {
        posY = (posY + 1);
        posX = posX - 80;
    }
   // move_cursor(posX, posY);
    update_cursor(posX, posY);
}




void print(char* str)
{
    while(*str != '\0')
    {
        print_char(*str);
        *str++;
    }
}