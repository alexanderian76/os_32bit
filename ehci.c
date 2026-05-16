#include "ehci.h"
#include "pci.h"
#include "mm.h"

// --- Глобальные переменные ---
static volatile uint32_t* ehci_caps = NULL;
static volatile uint32_t* ehci_op = NULL;
static uint8_t num_ports = 0;
static bool initialized = false;

// DMA-структуры
static ehci_qh_t* async_qh_virt = NULL;
static uint32_t   async_qh_phys = 0;
static ehci_td_t* dummy_td_virt = NULL;
static uint32_t   dummy_td_phys = 0;

// --- Чтение/запись регистров ---
static inline uint32_t ehci_read_caps(int offset) {
    return ehci_caps[offset / 4];
}

static inline uint32_t ehci_read_op(int offset) {
    return ehci_op[offset / 4];
}

static inline void ehci_write_op(int offset, uint32_t val) {
    ehci_op[offset / 4] = val;
}

// --- Выделение и настройка DMA-структур ---
static bool create_dma_structures(void) {
    async_qh_virt = (ehci_qh_t*)dma_alloc(sizeof(ehci_qh_t), 64, &async_qh_phys);
    if (!async_qh_virt) {
        print("EHCI: Failed to allocate async QH!\n");
        return false;
    }
    
    dummy_td_virt = (ehci_td_t*)dma_alloc(sizeof(ehci_td_t), 64, &dummy_td_phys);
    if (!dummy_td_virt) {
        print("EHCI: Failed to allocate dummy TD!\n");
        return false;
    }
    
    print("EHCI: DMA structures allocated (QH=0x");
    print_hex(async_qh_phys);
    print(", TD=0x");
    print_hex(dummy_td_phys);
    print(")\n");
    
    return true;
}

// --- Настройка асинхронного списка ---
static void setup_async_list(void) {
    // Головной QH: зациклен сам на себя
    async_qh_virt->horiz_link_ptr = async_qh_phys | 0x02; // QH type
    async_qh_virt->caps = QH_CAPS_SPEED_HIGH | (64 & QH_MAX_PACKET_MASK) | (1 << 30); // H-bit
    
    // Dummy TD: завершающий
    dummy_td_virt->next_td_ptr = 0x01; // Terminate
    dummy_td_virt->token = 0;
    
    // Привязываем TD к QH
    async_qh_virt->next_td_ptr = dummy_td_phys;
    
    // Устанавливаем в регистр контроллера
    ehci_write_op(ASYNCLISTADDR, async_qh_phys);
    
    print("EHCI: Async list configured\n");
}

// --- Основная инициализация ---
bool ehci_init(uint32_t mmio_phys_base) {
    print("EHCI: Initializing with MMIO base 0x");
    print_hex(mmio_phys_base);
    print("\n");
    
    // 1. Мапим регистры
    ehci_caps = (volatile uint32_t*)map_mmio(mmio_phys_base, 0x100);
    if (!ehci_caps) {
        print("EHCI: Failed to map capability registers!\n");
        return false;
    }
    
    uint8_t cap_length = ehci_read_caps(CAPLENGTH_OFFSET) & 0xFF;
    print("EHCI: Capability length = ");
    print_hex(cap_length);
    print("\n");
    
    ehci_op = (volatile uint32_t*)map_mmio(mmio_phys_base + cap_length, 0x100);
    if (!ehci_op) {
        print("EHCI: Failed to map operational registers!\n");
        return false;
    }
    
    // Читаем параметры
    uint32_t hcsparams = ehci_read_caps(HCSPARAMS_OFFSET);
    num_ports = hcsparams & HCS_N_PORTS_MASK;
    if (num_ports == 0) num_ports = 1;
    
    print("EHCI: Number of ports = ");
    print_hex(num_ports);
    print("\n");
    
    // 2. Сброс контроллера
    print("EHCI: Resetting controller...\n");
    ehci_write_op(USBCMD_OFFSET, CMD_HCRESET);
    
    // Таймаут 100 мс (примерно)
    for (volatile int timeout = 0; timeout < 1000000; timeout++) {
        if (!(ehci_read_op(USBCMD_OFFSET) & CMD_HCRESET)) break;
    }
    
    if (ehci_read_op(USBCMD_OFFSET) & CMD_HCRESET) {
        print("EHCI: Reset timeout!\n");
        return false;
    }
    print("EHCI: Reset complete\n");
    
    // Ждём остановки
    while (!(ehci_read_op(USBSTS_OFFSET) & STS_HCHALTED));
    
    // 3. Маршрутизация
    ehci_write_op(CONFIGFLAG, CF_FLAG);
    
    // 4. Создаём DMA-структуры
    if (!create_dma_structures()) return false;
    setup_async_list();
    
    // 5. Очищаем прерывания
    ehci_write_op(USBSTS_OFFSET, 0x3F);
    
    // 6. Запускаем
    print("EHCI: Starting controller...\n");
    uint32_t cmd = ehci_read_op(USBCMD_OFFSET);
    cmd |= CMD_ASYNC_ENABLE | CMD_RUN_STOP;
    cmd &= ~CMD_PERIODIC_ENABLE;
    ehci_write_op(USBCMD_OFFSET, cmd);
    
    // Ждём запуска
    for (volatile int timeout = 0; timeout < 1000000; timeout++) {
        if (!(ehci_read_op(USBSTS_OFFSET) & STS_HCHALTED)) break;
    }
    
    if (ehci_read_op(USBSTS_OFFSET) & STS_HCHALTED) {
        print("EHCI: Start timeout!\n");
        return false;
    }
    
    initialized = true;
    print("EHCI: Initialization successful!\n");
    return true;
}

// --- Автоматический поиск и инициализация ---
bool ehci_init_auto(void) {
    pci_device_t ehci_dev;
    
    if (pci_find_ehci(&ehci_dev) == 1) {
        print("EHCI: Found via PCI at ");
        print_hex(ehci_dev.bus);
        print(":");
        print_hex(ehci_dev.device);
        print(".");
        print_hex(ehci_dev.function);
        print("\n");
        
        uint32_t mmio_phys = ehci_dev.bars[0] & ~0xF;
        pci_enable_mmio(&ehci_dev);
        
        return ehci_init(mmio_phys);
    } else {
        print("EHCI: Not found via PCI, trying signature scan...\n");
        
        // Запасной поиск по сигнатуре
        uint32_t common_addrs[] = {
            0xF0900000, 0xF0800000, 0xFEB00000, 0xFED1C000
        };
        
        for (int i = 0; i < 4; i++) {
            if (ehci_init(common_addrs[i])) {
                return true;
            }
        }
        
        print("EHCI: Not found anywhere!\n");
        return false;
    }
}



// --- Проверка портов ---
void ehci_poll(void) {
    if (!initialized) return;
    
    for (int i = 0; i < num_ports; i++) {
        uint32_t portsc = ehci_read_op(PORTSC_BASE + (i * 4));
        
        if (portsc & PORT_CURRENT_CONNECT) {
            print("EHCI: Device connected on port ");
            print_hex(i);
            print("\n");
        }
    }
}