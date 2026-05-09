ENTRY(start)

SECTIONS
{
    . = 0x100000;  /* Виртуальный адрес, по которому будет исполняться ядро */
kernel_start = .;
    .text : AT(ADDR(.text) - 0x100000 + 0x1000)  /* Физический адрес в загрузчике — 0x1000 */
    {
        *(.text)
        *(.text.*)
    }

    /* Аналогично для .data, .rodata, .bss... */
    .data : AT(ADDR(.data) - 0x100000 + 0x1000) { *(.data) *(.data.*) }
    .rodata : AT(ADDR(.rodata) - 0x100000 + 0x1000) { *(.rodata) *(.rodata.*) }
    .bss : AT(ADDR(.bss) - 0x100000 + 0x1000) { *(.bss) *(.bss.*) *(COMMON) }

    kernel_end = .;
kernel_size = kernel_end - kernel_start;

     . = ALIGN(4096);
    page_tables_start = .;
    . += 4 * 1024 * 1024;  /* 4MB для таблиц страниц */
    page_tables_end = .;
}