#include "kern.h"
#include "keyboard_map.h"
int VIDEO_MEMORY;
// IDT and IDT descriptor
struct idt_entry idt[256];
struct idt_descriptor idt_desc;
char b[10000000];
extern uint32_t kernel_end;
extern uint32_t page_tables_start;
extern uint32_t page_tables_end;
char *memory = (char *)&page_tables_start;
// Main kernel function
void kernel_main()
{
    /*  int i;
      for (i = 0; i < 1024; i++)
      {
          // This sets the following flags to the pages:
          //   Supervisor: Only kernel-mode can access them
          //   Write Enabled: It can be both read from and written to
          //   Not Present: The page table is not present
          page_directory[i] = 0x00000002;
      }

      // we will fill all 1024 entries in the table, mapping 4 megabytes
      for (int i = 0; i < 1024; i++)
      {
          // As the address is page aligned, it will always leave 12 bits zeroed.
          // Those bits are used by the attributes ;)
          first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
      }

      page_directory[0] = ((unsigned int)first_page_table) | 0x83;
  */
    enable_pse(); // Включаем поддержку 4MB страниц

    setup_paging_4mb_pages();
    loadPageDirectory(page_directory);
    enablePaging();

    VIDEO_MEMORY = 0xB8000;
    // Set up IDT

    enable_cursor(0, 11);
    start();

b[90000] = 'q';
    print("Initializing... \n");

    idt_desc.limit = sizeof(idt) - 1;
    idt_desc.base = (uint32_t)&idt;

    // Set up keyboard interrupt (IRQ1 -> INT 0x21)
    idt_set_gate(0x21, (uint32_t)keyboard_handler, 0x08, 0x8E);
    idt_set_gate(0x0E, (uint32_t)page_fault_handler, 0x08, 0x8E);

    // Load IDT
    asm volatile("lidt %0" : : "m"(idt_desc));

    // Remap PIC
    remap_pic();

    // Enable interrupts
    asm volatile("sti");

    print("Kernel end at: ");
    print_hex((uint32_t)&kernel_end);
    print(" ");

    print("Table start: ");
    print_hex((uint32_t)&page_tables_start);
    print("\n");

    print("Table end at: ");
    print_hex((uint32_t)&page_tables_end);
    print("\n");

    /*char *message = (char *)0x200000; // последний адрес (всего смаплено 4 мегабайта)
    message[0] = 'H';
    message[1] = 'i';
    print_char(message[0]);
    print_char(message[1]);*/

    uint32_t test_addr = 0x007fffff; //- (uint32_t)&page_tables_start; //только 128мб в qemu есть ограничение 0x80000000
    print("Testing page at: ");
    print_hex(test_addr);
    print_char('\n');

    // Читаем текущее значение
    uint8_t old_value = *(uint8_t *)test_addr;
    print_hex(old_value);
    print(" ");

    // Пытаемся записать
    *(uint8_t *)test_addr = 0xAA;
    print("Write done. Reading back: ");
    uint8_t new_value = *(uint8_t *)test_addr;
    print_char(new_value);
    print(" ");
    print_hex(new_value);
    print("\n");
    if (new_value == 0xAA)
    {
        print("Memory works! Keyboard should work...\n");
    }
    else
    {
        print("Memory NOT writable! Page fault?\n");
    }

    print("System ready. Type something...\n");

    /* for (int i = 0; i < 100; i++)
     {
         put_pixel(i, i, 4);
     }
         */
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
    if (keyboard_map[(unsigned char)scancode] == 0x8)
    {
        clearScreen();
    }
    else if (scancode < 128 && keyboard_map[(unsigned char)scancode] > 0 && keyboard_map[(unsigned char)scancode] != '\n')
    {

        /*   *(char*)VIDEO_MEMORY = keyboard_map[(unsigned char)scancode];
                               // Print the scancode
           VIDEO_MEMORY += 0x1; // Move to the next character position
           *(char*)VIDEO_MEMORY = 0x5;
           VIDEO_MEMORY += 0x1;*/

        print_char(keyboard_map[(unsigned char)scancode]);
        // Send End of Interrupt (EOI) to the PIC
        update_cursor(((VIDEO_MEMORY - 0xb8000) / 2) % 80, (VIDEO_MEMORY - 0xb8000) / 160);
        memory += 1;
        *memory = keyboard_map[(unsigned char)scancode];
    }
    else if (keyboard_map[(unsigned char)scancode] == '\n' && memory >= 2 && ((int)memory % 2 == 0) && memory > &page_tables_start)
    {
        /*  for(int i = 0; i < 0x00300000; i++) {
              memory += 1;
              *memory = '7';
          }*/
        print_char(memory[0] + memory[-1] - 48);
        memory = memory - 2;
        update_cursor(((VIDEO_MEMORY - 0xb8000) / 2) % 80, (VIDEO_MEMORY - 0xb8000) / 160);
        print_char(keyboard_map[(unsigned char)scancode]);
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
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
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
    if (VIDEO_MEMORY < 0xbffff)
    {
        if (ch == '\n')
        {
            VIDEO_MEMORY += 158 - (VIDEO_MEMORY - 0xb8000) % 160;
            ch = '\0';
        }
        *(char *)VIDEO_MEMORY = ch;
        VIDEO_MEMORY += 0x1;
        *(char *)VIDEO_MEMORY = 0x5;
        VIDEO_MEMORY += 0x1;
        // posX = posX + 1;

        int posX = ((VIDEO_MEMORY - 0xb8000) / 2) % 80;
        int posY = (VIDEO_MEMORY - 0xb8000) / 160;

        if (posX >= 80)
        {
            posY = (VIDEO_MEMORY - 0xb8000) / 160; //(posY + 1);
            posX = posX - 80;
        }
        // move_cursor(posX, posY);
        update_cursor(posX, posY);
    }
}

void print(char *str)
{
    while (*str != '\0')
    {
        print_char(*str);
        *str++;
    }
}

void clearScreen()
{
    update_cursor(0, 0);
    while (VIDEO_MEMORY > 0xb8000)
    {
        *(char *)VIDEO_MEMORY = 0x0;
        VIDEO_MEMORY -= 0x1;

        *(char *)VIDEO_MEMORY = 0x7;
        VIDEO_MEMORY -= 0x1;
    }
    *(char *)VIDEO_MEMORY = 0x0;
}

void print_hex(uint32_t num)
{
    const char *hex_chars = "0123456789ABCDEF";
    char buffer[9]; // 8 hex digits + null
    buffer[8] = '\0';

    for (int i = 7; i >= 0; i--)
    {
        buffer[i] = hex_chars[num & 0xF];

        num >>= 4;
    }
    print("0x");
    print(buffer);
}

void __stack_chk_fail(void)
{
    // Если это случилось - стек переполнен
    print("Stack smashing detected!\n");
    while (1)
        ; // Бесконечный цикл (panic)
}

void page_fault_handler()
{
    unsigned int fault_addr;
    asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

    print("PAGE FAULT at address: ");
    print_hex(fault_addr);
    print("\n");

    while (1)
        ;
}

// Включение PSE для 4MB страниц
void enable_pse()
{
    unsigned int cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x00000010; // Устанавливаем бит PSE
    asm volatile("mov %0, %%cr4" : : "r"(cr4));
}

void setup_paging_4mb_pages()
{
    // Page directory entries для 4MB страниц
    for (int i = 0; i < 2; i++)
    {
        // Каждая запись покрывает 4MB
        // Физический адрес = i * 4MB
        page_directory[i] = (i * 4 * 1024 * 1024) | 0x83; // Present, RW, Supervisor, 4MB page
    }
}

// Функция для установки одного пикселя
void put_pixel(int x, int y, unsigned char color)
{
    // 1. Проверяем границы экрана
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
    {
        // 2. Вычисляем смещение: y * ширина_экрана + x
        unsigned int offset = y * SCREEN_WIDTH + x;
        // 3. Получаем указатель на начало видеопамяти и записываем цвет
        unsigned char *video_memory = (unsigned char *)VIDEO_GRAPHICS_ADDR;
        video_memory[offset] = color;
    }
}