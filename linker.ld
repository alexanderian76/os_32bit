ENTRY(kernel_main)  /* Entry point of the kernel */

SECTIONS
{
    . = 0x1000;  /* Load the kernel at 0x1000 */

    .text : {
        *(.text)  /* All .text sections (code) */
        *(.rodata*) 
        *(.data)  
        *(COMMON)  /* All COMMON sections (uninitialized global variables) */
        *(.bss) 
    }

}