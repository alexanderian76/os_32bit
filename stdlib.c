#include <stddef.h>
#include "kern.h"

// Простейший аллокатор памяти
  // Начало кучи
uint32_t heap_end = (uint32_t)&heap_start;

void *malloc(size_t size) {
    void *ptr = (void*)heap_end;
    heap_end += size;
    return ptr;
}

void free(void *ptr) {
    // В этой простой реализации free ничего не делает
    (void)ptr;
}