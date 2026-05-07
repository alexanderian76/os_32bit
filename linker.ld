ENTRY(start)

SECTIONS
{
    . = 0x1000;
    
    kernel_start = .;
    
    .text : {
        *(.text)
        *(.text.*)
    }
    
    .data : {
        *(.data)
        *(.data.*)
    }
    
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    kernel_end = .;
}