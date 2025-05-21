#include "types.h"
extern unsigned long long int p4_table;

void RemapPIC();
void clear_screen();
void PrepareInterrupts();
void InitPS2Mouse();
void io_wait();
void KeyboardInt_Handler();
void io_wait();
void itoa(int num, char *number);
void PIC_EndMaster();
void PIC_EndSlave();
//void GeneralProtectionFault_Handler(struct interrupt_frame* frame);

//__attribute__((interrupt)) int MKeyHandler(struct interrupt_frame* frame);

struct IDTDescEntry {
    uint16_t offset0; 
    uint16_t selector;  
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t ignore;
};

struct IDTR {
    uint16_t Limit;
    uint32_t Offset;
} __attribute__((packed));


uint32_t GetOffset(struct IDTDescEntry* idtEntry);
void SetOffset(uint32_t offset, struct IDTDescEntry* idtEntry);

#define IDT_TA_InterruptGate    0b10001110
#define IDT_TA_CallGate         0b10001100
#define IDT_TA_TrapGate         0b10001111
