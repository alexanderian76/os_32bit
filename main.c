#include "main.h"
#include "types.h"
#include "consts.h"
#include "kernel.h"
struct IDTR idtr;
__attribute__((interrupt)) void MKeyHandler(struct interrupt_frame* frame) {
  KeyboardInt_Handler();
//PIC_EndMaster();
  //  asm ("sti");
}
__attribute__((interrupt)) void GeneralProtectionFault_Handler(struct interrupt_frame* frame){
  clear_screen();
    char* str = "00000000";
    uint8_t t = inb(0xE9);
    itoa(t, str);
    print(str);
    //kprint_newline();
    print("General protection fault detected");
    while(1) {

    }
}

__attribute__((interrupt))  void PageFault_Handler(struct interrupt_frame* frame){
  clear_screen();
    print("Page fault detected");
   // int *t = (int*)0x80000000000;
   // *t = 2;
    while(1) {

    }
}

__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame){
  clear_screen();
    print("Double fault detected");
    while(1) {

    }
}
void PrepareInterrupts(){
    idtr.Limit = 0x0FFF;
    idtr.Offset = 4096;

   /* struct IDTDescEntry* int_PageFault = (struct IDTDescEntry*)(idtr.Offset + 0xE * sizeof(struct IDTDescEntry));
    SetOffset((uint64_t)PageFault_Handler, int_PageFault);
    int_PageFault->type_attr = IDT_TA_InterruptGate;
    int_PageFault->selector = 0x08;


    struct IDTDescEntry* int_DoubleFault = (struct IDTDescEntry*)(idtr.Offset + 0x8 * sizeof(struct IDTDescEntry));
    SetOffset((uint64_t)DoubleFault_Handler, int_DoubleFault);
    int_DoubleFault->type_attr = IDT_TA_InterruptGate;
    int_DoubleFault->selector = 0x08;


    struct IDTDescEntry* int_GeneralFault = (struct IDTDescEntry*)(idtr.Offset + 0xD * sizeof(struct IDTDescEntry));
    SetOffset((uint64_t)GeneralProtectionFault_Handler, int_GeneralFault);
    int_GeneralFault->type_attr = IDT_TA_InterruptGate;
    int_GeneralFault->selector = 0x08;


    struct IDTDescEntry* int_Keyboard = (struct IDTDescEntry*)(idtr.Offset + 0x21 * sizeof(struct IDTDescEntry));
    SetOffset((uint64_t)MKeyHandler, int_Keyboard);
    int_Keyboard->type_attr = IDT_TA_InterruptGate;
    int_Keyboard->selector = 0x08;

    struct IDTDescEntry* int_Mouse = (struct IDTDescEntry*)(idtr.Offset + 0x2C * sizeof(struct IDTDescEntry));
    SetOffset((uint64_t)DoubleFault_Handler, int_Mouse);
    int_Mouse->type_attr = IDT_TA_InterruptGate;
    int_Mouse->selector = 0x08;
*/
/*
    struct IDTDescEntry* int_Mouse = (struct IDTDescEntry*)(idtr.Offset + 0x2C * sizeof(struct IDTDescEntry));
    int_Mouse->SetOffset((uint64_t)MMouseHandler);
    int_Mouse->type_attr = IDT_TA_InterruptGate;
    int_Mouse->selector = 0x08;
*/
   struct IDTDescEntry* int_Keyboard = (struct IDTDescEntry*)(idtr.Offset + 0x21 * sizeof(struct IDTDescEntry));
    SetOffset((uint64_t)MKeyHandler, int_Keyboard);
    int_Keyboard->type_attr = IDT_TA_InterruptGate;
    int_Keyboard->selector = 0x08;
    asm volatile ("lidt %0" : : "m" (idtr));

    RemapPIC();

    

    

}
void io_wait(){
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}
void PIC_EndMaster(){
    outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave(){
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void RemapPIC(){
    uint8_t a1, a2; 

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);

}





void SetOffset(uint64_t offset, struct IDTDescEntry* idtEntry){
    idtEntry->offset0 = (uint16_t)(offset & 0x000000000000ffff);
    idtEntry->offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    idtEntry->offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);

    idtEntry->ist = 0;
    idtEntry->ignore = 0;

}

uint64_t GetOffset(struct IDTDescEntry* idtEntry){ 
    uint64_t offset = 0;
    offset |= (uint64_t)idtEntry->offset0;
    offset |= (uint64_t)idtEntry->offset1 << 16;
    offset |= (uint64_t)idtEntry->offset2 << 32;
    return offset;
}

//--------------------------------------



uint32_t strlen(const char* str)
{
  uint32_t length = 0;
  while(str[length])
    length++;
  return length;
}

uint32_t digit_count(int num)
{
  uint32_t count = 0;
  if(num == 0)
    return 1;
  while(num > 0){
    count++;
    num = num/10;
  }
  return count;
}
void clear_screen()
{
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		print_char(' ');
        i++;
	}
	update_cursor(0, 0);
	//set_cursor(0);
}

void itoa(int num, char *number)
{
  int dgcount = digit_count(num);
  int index = dgcount - 1;
  char x;
  if(num == 0 && dgcount == 1){
    number[0] = '0';
    number[1] = '\0';
  }else{
    while(num != 0){
      x = num % 10;
      number[index] = x + '0';
      index--;
      num = num / 10;
    }
    number[dgcount] = '\0';
  }
}

void KeyboardInt_Handler(){
    
   // uint8_t scancode = inb(0x60);

unsigned char status;
	char keycode;
    unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	/* write EOI */
    //outb(0x20, 0x20);
//	outb(0xa0, 0x20);
        
	//status = inb(KEYBOARD_STATUS_PORT);
 // keycode = inb(KEYBOARD_DATA_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	print_char('1');

   // while(true);
}






extern void __test__() {
    GLOBAL_VIDEO_ADDRESS = 0xb8000;
    print_char('1');
    while(1)
    {
        
    }
            
   
}
