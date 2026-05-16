#ifndef MM_H
#define MM_H

#include "types.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "kern.h"
#include "string.h"


// --- Конфигурация ---
#define PAGE_SIZE           4096
#define DMA_POOL_SIZE       (1024 * 1024)  // 1 МБ для DMA-буферов

// Биты в PDE/PTE
#define PAGE_PRESENT    0x001
#define PAGE_WRITABLE   0x002
#define PAGE_USER       0x004
#define PAGE_SIZE_4MB   0x080   // Только для PDE

// --- Структура для отслеживания выделений ---
typedef struct {
    void*  virt_addr;
    uint32_t phys_addr;
    size_t size;
    bool   used;
} dma_allocation_t;

#define MAX_DMA_ALLOCATIONS 256

// --- Глобальные переменные менеджера памяти ---
static uint8_t dma_pool[DMA_POOL_SIZE] __attribute__((aligned(PAGE_SIZE)));
static size_t dma_pool_offset = 0;

static dma_allocation_t dma_allocations[MAX_DMA_ALLOCATIONS];
static int dma_alloc_count = 0;

// --- Инициализация (вызывается один раз) ---
static inline void mm_init(void) {
    dma_pool_offset = 0;
    dma_alloc_count = 0;
    
    for (int i = 0; i < MAX_DMA_ALLOCATIONS; i++) {
        dma_allocations[i].used = false;
    }
    
    print("MM: DMA pool initialized (");
    print_hex(DMA_POOL_SIZE);
    print(" bytes at ");
    print_hex((uint32_t)dma_pool);
    print(")\n");
}

// --- Выделение памяти с заданным выравниванием ---
// Возвращает виртуальный адрес, физический записывает в *phys_out
static inline void* dma_alloc(size_t size, size_t alignment, uint32_t* phys_out) {
    if (dma_alloc_count >= MAX_DMA_ALLOCATIONS) {
        print("MM: DMA allocation table full!\n");
        return NULL;
    }
    
    // Вычисляем выровненное смещение
    uintptr_t addr = (uintptr_t)dma_pool + dma_pool_offset;
    size_t align_offset = 0;
    
    if (alignment > 0) {
        align_offset = (alignment - (addr % alignment)) % alignment;
    }
    
    addr += align_offset;
    size_t total_size = align_offset + size;
    
    // Проверяем, хватает ли места
    if (dma_pool_offset + total_size > DMA_POOL_SIZE) {
        print("MM: DMA pool exhausted!\n");
        return NULL;
    }
    
    // В identity-mapped окружении физический адрес равен виртуальному
    uint32_t phys = (uint32_t)addr;
    
    // Записываем в таблицу выделений
    dma_allocations[dma_alloc_count].virt_addr = (void*)addr;
    dma_allocations[dma_alloc_count].phys_addr = phys;
    dma_allocations[dma_alloc_count].size = size;
    dma_allocations[dma_alloc_count].used = true;
    dma_alloc_count++;
    
    // Обновляем смещение
    dma_pool_offset += total_size;
    
    // Очищаем выделенную память
    memset((void*)addr, 0, size);
    
    if (phys_out) {
        *phys_out = phys;
    }
    
    return (void*)addr;
}


// --- Маппинг MMIO (Memory-Mapped I/O) ---
static inline void* map_mmio(uint32_t phys_addr, size_t size) {
    // В простейшем случае (identity mapping) возвращаем тот же адрес
    // В реальной ОС здесь нужно выделить виртуальные страницы и замапить их
    return (void*)(uintptr_t)phys_addr;
}

// --- Освобождение DMA-памяти (упрощённое, без реального освобождения) ---
static inline void dma_free(void* virt) {
    for (int i = 0; i < dma_alloc_count; i++) {
        if (dma_allocations[i].virt_addr == virt) {
            dma_allocations[i].used = false;
            return;
        }
    }
}

// --- Дамп статистики ---
static inline void mm_dump_stats(void) {
    print("MM: DMA pool usage: ");
    print_hex(dma_pool_offset);
    print(" / ");
    print_hex(DMA_POOL_SIZE);
    print(" bytes\n");
    print("MM: Active allocations: ");
    print_hex(dma_alloc_count);
    print_char('\n');
}




// Получение текущей Page Directory из CR3
static inline uint32_t get_cr3(void) {
    uint32_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

// Получение текущего CR0
static inline uint32_t get_cr0(void) {
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    return cr0;
}

// Проверка, включён ли пейджинг (бит 31 в CR0)
static inline bool paging_enabled(void) {
    return (get_cr0() >> 31) & 1;
}

// Функция, которую вы будете использовать
static inline uint32_t virt_to_phys(void* virt_addr) {
    // Если пейджинг выключен — физический адрес равен виртуальному
    if (!paging_enabled()) {
        return (uint32_t)(uintptr_t)virt_addr;
    }
    
    // Разбираем виртуальный адрес на компоненты
    uint32_t vaddr = (uint32_t)(uintptr_t)virt_addr;
    uint32_t pd_index = (vaddr >> 22) & 0x3FF;    // Старшие 10 бит
    uint32_t pt_index = (vaddr >> 12) & 0x3FF;    // Средние 10 бит
    uint32_t offset   = vaddr & 0xFFF;            // Младшие 12 бит
    
    // Получаем физический адрес Page Directory
    uint32_t pd_phys = get_cr3() & ~0xFFF;  // Очищаем флаги (младшие 12 бит)
    
    // Читаем PDE (Page Directory Entry)
    // Page Directory находится в физической памяти, 
    // нам нужен её виртуальный адрес для доступа.
    // Если у вас identity mapping для ядра, можно использовать pd_phys напрямую,
    // иначе нужно найти виртуальный адрес для pd_phys.
    uint32_t* page_dir = (uint32_t*)(uintptr_t)pd_phys;  // Только при identity mapping!
    uint32_t pde = page_dir[pd_index];
    
    // Проверяем, что PDE присутствует
    if (!(pde & PAGE_PRESENT)) {
        return 0;  // Страница не отображена
    }
    
    // Проверяем, 4MB ли это страница (бит Page Size)
    if (pde & PAGE_SIZE_4MB) {
        // Большая страница 4 МБ — физический адрес напрямую из PDE
        uint32_t phys_base = pde & 0xFFC00000;  // Старшие 10 бит физического адреса
        return phys_base | (vaddr & 0x003FFFFF); // Добавляем смещение внутри 4 МБ
    }
    
    // Это обычная страница 4 КБ — читаем Page Table
    uint32_t pt_phys = pde & ~0xFFF;  // Физический адрес Page Table
    
    // Page Table тоже в физической памяти, нужен виртуальный адрес
    uint32_t* page_table = (uint32_t*)(uintptr_t)pt_phys;  // Только при identity mapping!
    uint32_t pte = page_table[pt_index];
    
    // Проверяем, что PTE присутствует
    if (!(pte & PAGE_PRESENT)) {
        return 0;  // Страница не отображена
    }
    
    // Получаем физический адрес
    uint32_t phys_base = pte & ~0xFFF;  // Очищаем флаги (младшие 12 бит)
    return phys_base | offset;           // Добавляем смещение внутри страницы
}

#endif