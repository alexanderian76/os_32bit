#include "pci.h"
#include "io.h"   // Ваши функции inb/outb/inl/outl
#include "kern.h" // Ваш printf/logging
#include <stddef.h>

// --- Низкоуровневый доступ к конфигурационному пространству PCI ---

uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    // Формируем адрес для порта 0xCF8
    uint32_t address = (uint32_t)(((uint32_t)bus << 16) |
                                  ((uint32_t)device << 11) |
                                  ((uint32_t)function << 8) |
                                  (offset & 0xFC) | // Обнуляем два младших бита (двойное слово)
                                  0x80000000        // Enable bit
    );

    // Отправляем адрес в порт 0xCF8
    outl(PCI_CONFIG_ADDRESS, address);

    // Читаем данные из порта 0xCFC
    return inl(PCI_CONFIG_DATA);
}

void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value)
{
    uint32_t address = (uint32_t)(((uint32_t)bus << 16) |
                                  ((uint32_t)device << 11) |
                                  ((uint32_t)function << 8) |
                                  (offset & 0xFC) |
                                  0x80000000);

    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
}

// --- Чтение целых регистров ---

uint16_t pci_read_vendor_id(uint8_t bus, uint8_t device, uint8_t function)
{
    return pci_read_config(bus, device, function, PCI_VENDOR_ID) & 0xFFFF;
}

uint16_t pci_read_device_id(uint8_t bus, uint8_t device, uint8_t function)
{
    return (pci_read_config(bus, device, function, PCI_DEVICE_ID) >> 16) & 0xFFFF;
}

uint8_t pci_read_class(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t reg = pci_read_config(bus, device, function, PCI_CLASS);
    return (reg >> 24) & 0xFF;
}

uint8_t pci_read_subclass(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t reg = pci_read_config(bus, device, function, PCI_CLASS);
    return (reg >> 16) & 0xFF;
}

uint8_t pci_read_prog_if(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t reg = pci_read_config(bus, device, function, PCI_CLASS);
    return (reg >> 8) & 0xFF;
}

uint8_t pci_read_header_type(uint8_t bus, uint8_t device, uint8_t function)
{
    return pci_read_config(bus, device, function, PCI_HEADER_TYPE) & 0xFF;
}

uint32_t pci_read_bar(uint8_t bus, uint8_t device, uint8_t function, int bar_index)
{
    return pci_read_config(bus, device, function, PCI_BAR0 + (bar_index * 4));
}

// --- Чтение вторичной шины для мостов ---
uint8_t pci_read_secondary_bus(uint8_t bus, uint8_t device, uint8_t function)
{
    return (pci_read_config(bus, device, function, PCI_SECONDARY_BUS) >> 8) & 0xFF;
}

// --- Проверка существования устройства ---
static short pci_device_exists(uint8_t bus, uint8_t device, uint8_t function)
{
    uint16_t vendor = pci_read_vendor_id(bus, device, function);
    return (vendor != 0xFFFF);
}

// --- Включение MMIO и Bus Mastering ---
void pci_enable_mmio(pci_device_t *dev)
{
    uint32_t cmd = pci_read_config(dev->bus, dev->device, dev->function, PCI_COMMAND);
    cmd |= PCI_CMD_MEM_SPACE | PCI_CMD_BUS_MASTER;
    pci_write_config(dev->bus, dev->device, dev->function, PCI_COMMAND, cmd);
}

// --- Рекурсивное сканирование шины ---
static void pci_scan_bus(uint8_t bus, pci_scan_callback_t callback);

static void pci_scan_function(uint8_t bus, uint8_t device, uint8_t function, pci_scan_callback_t callback)
{
    if (pci_device_exists(bus, device, function) == 0)
    {
        return;
    }

    pci_device_t dev;
    dev.bus = bus;
    dev.device = device;
    dev.function = function;
    dev.vendor_id = pci_read_vendor_id(bus, device, function);
    dev.device_id = pci_read_device_id(bus, device, function);
    dev.class_code = pci_read_class(bus, device, function);
    dev.subclass = pci_read_subclass(bus, device, function);
    dev.prog_if = pci_read_prog_if(bus, device, function);
    dev.header_type = pci_read_header_type(bus, device, function);

    // Читаем все BAR
    for (int i = 0; i < 6; i++)
    {
        dev.bars[i] = pci_read_bar(bus, device, function, i);
    }
   //   pci_dump_device(&dev);
    // Вызываем коллбэк, если есть
    if (callback)
    {
        callback(&dev);
    }

    // Если это PCI-to-PCI мост — сканируем вторичную шину
    if ((dev.header_type & 0x7F) == PCI_HEADER_TYPE_BRIDGE)
    {
        uint8_t secondary_bus = pci_read_secondary_bus(bus, device, function);
        pci_scan_bus(secondary_bus, callback);
    }
}

static void pci_scan_device(uint8_t bus, uint8_t device, pci_scan_callback_t callback)
{
    uint8_t function = 0;
    uint8_t header_type = pci_read_header_type(bus, device, function);

    pci_scan_function(bus, device, function, callback);

    // Если устройство многофункциональное — сканируем остальные функции
    if (header_type & PCI_HEADER_TYPE_MULTIFUNC)
    {
        for (function = 1; function < 8; function++)
        {
            if (pci_device_exists(bus, device, function) == 1)
            {
                pci_scan_function(bus, device, function, callback);
            }
        }
    }
}

static void pci_scan_bus(uint8_t bus, pci_scan_callback_t callback)
{
    for (uint8_t device = 0; device < 32; device++)
    {
        pci_scan_device(bus, device, callback);
    }
}

// --- Инициализация ---
void pci_init(void)
{
    print("PCI: Scanning bus 0...\n");

    // Проверяем, есть ли вообще PCI (хост-бридж на шине 0, устройстве 0, функции 0)
    if (pci_device_exists(0, 0, 0) == 0)
    {
        print("PCI: No host bridge found. PCI not available.\n");
        return;
    }

    pci_scan_bus(0, NULL);
    print("PCI: Bus scan complete.\n");
}

// --- Сканирование с коллбэком (публичная функция) ---
void pci_scan(pci_scan_callback_t callback)
{
    pci_scan_bus(0, callback);
}

// --- Поиск EHCI-контроллера ---
short pci_find_ehci(pci_device_t *out_device)
{
    print("PCI_EHCI_FIND\n");
    short found = 0;
    pci_device_t found_dev;

    pci_scan_bus(0, NULL); // Убедимся, что список актуален

    // Второй проход — ищем именно EHCI
    // (для простоты используем тот же механизм, но с прямым поиском)
    for (int bus = 0; bus < 256 && found == 0; bus++)
    {
        for (int device = 0; device < 32 && found == 0; device++)
        {
            if (pci_device_exists(bus, device, 0) == 0)
                continue;

            uint8_t class_code = pci_read_class(bus, device, 0);
            uint8_t subclass = pci_read_subclass(bus, device, 0);
            uint8_t prog_if = pci_read_prog_if(bus, device, 0);

            if (class_code == PCI_CLASS_SERIAL_USB &&
                subclass == PCI_SUBCLASS_USB_EHCI &&
                prog_if == PCI_PROG_IF_EHCI)
            {

                found_dev.bus = bus;
                found_dev.device = device;
                found_dev.function = 0;
                found_dev.vendor_id = pci_read_vendor_id(bus, device, 0);
                found_dev.device_id = pci_read_device_id(bus, device, 0);
                found_dev.class_code = class_code;
                found_dev.subclass = subclass;
                found_dev.prog_if = prog_if;
                found_dev.header_type = pci_read_header_type(bus, device, 0);

                // BAR0 обычно содержит MMIO-адрес EHCI
                found_dev.bars[0] = pci_read_bar(bus, device, 0, 0);
                for (int i = 1; i < 6; i++)
                {
                    found_dev.bars[i] = pci_read_bar(bus, device, 0, i);
                }
                pci_dump_device(&found_dev);
                found = 1;
            }
        }
    }

    if (found == 1 && out_device)
    {
        out_device->bus = found_dev.bus;
        out_device->device = found_dev.device;
        out_device->function = found_dev.function;
        out_device->vendor_id = found_dev.vendor_id;
        out_device->device_id = found_dev.device_id;
        out_device->class_code = found_dev.class_code;
        out_device->subclass = found_dev.subclass;
        out_device->prog_if = found_dev.prog_if;
        out_device->header_type = found_dev.header_type;
        for (int i = 0; i < 6; i++)
        {
            out_device->bars[i] = found_dev.bars[i];
        }
        
    }

    return found;
}

// --- Отладочный вывод ---
void pci_dump_device(pci_device_t *dev)
{
    // print("PCI %02x:%02x.%x: Vendor=%04x Device=%04x Class=%02x Sub=%02x Prog=%02x\n",
    //      dev->bus, dev->device, dev->function,
    //      dev->vendor_id, dev->device_id,
    //      dev->class_code, dev->subclass, dev->prog_if);
    print("PCI");
    print_hex(dev->bus);
    print(": ");
    print_hex(dev->device);
    print(". ");
    print_hex(dev->function);
    print(": Vendor=");
    print_hex(dev->vendor_id);
    print(" Device=");
    print_hex(dev->device_id);
    print(" Class=");
    print_hex(dev->class_code);
    print(" Sub=");
    print_hex(dev->subclass);
    print(" Prog=");
    print_hex(dev->prog_if);
    print_char('\n');

    for (int i = 0; i < 6; i++)
    {
        if (dev->bars[i])
        {
            uint32_t bar = dev->bars[i];
            short is_io = bar & 1;
            uint32_t base = bar & ~0xF; // Для памяти (младшие 4 бита — флаги)
            if (is_io == 1)
            {
                base = bar & ~0x3; // Для IO (младшие 2 бита — флаги)
                print("  BAR");
                print_hex(i);
                print(": IO  ");
                print_hex(base);
                print_char('\n'); //, i, base);
            }
            else
            {
                // print("  BAR%d: MMIO 0x%x\n", i, base);
                print("  BAR");
                print_hex(i);
                print(": MMIO  ");
                print_hex(base);
                print_char('\n');
            }
        }
    }
}