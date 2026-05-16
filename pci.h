#ifndef PCI_H
#define PCI_H

#include "types.h"


// --- Константы PCI ---
#define PCI_CONFIG_ADDRESS  0xCF8
#define PCI_CONFIG_DATA     0xCFC

// Классы устройств
#define PCI_CLASS_SERIAL_USB        0x0C
#define PCI_SUBCLASS_USB_EHCI       0x03
#define PCI_PROG_IF_EHCI            0x20
#define PCI_PROG_IF_XHCI            0x30

// Заголовки PCI
#define PCI_HEADER_TYPE_MULTIFUNC   0x80
#define PCI_HEADER_TYPE_BRIDGE      0x01

// Смещения в конфигурационном пространстве
#define PCI_VENDOR_ID        0x00
#define PCI_DEVICE_ID        0x02
#define PCI_COMMAND          0x04
#define PCI_STATUS           0x06
#define PCI_CLASS            0x0B  // На самом деле 0x0B (класс), 0x0A (субкласс), 0x09 (прог-иф)
#define PCI_HEADER_TYPE      0x0E
#define PCI_BAR0             0x10
#define PCI_BAR1             0x14
#define PCI_BAR2             0x18
#define PCI_BAR3             0x1C
#define PCI_BAR4             0x20
#define PCI_BAR5             0x24
#define PCI_SECONDARY_BUS    0x19  // Для мостов PCI-to-PCI

// Команды
#define PCI_CMD_IO_SPACE     (1 << 0)
#define PCI_CMD_MEM_SPACE    (1 << 1)
#define PCI_CMD_BUS_MASTER   (1 << 2)

// --- Структура устройства PCI ---
typedef struct {
    uint8_t  bus;
    uint8_t  device;
    uint8_t  function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
    uint8_t  prog_if;
    uint8_t  header_type;
    uint32_t bars[6];         // Base Address Registers
} pci_device_t;

// --- Прототипы ---
void pci_init(void);
short pci_find_ehci(pci_device_t* out_device);
uint32_t pci_read_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_write_config(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);
void pci_enable_mmio(pci_device_t* dev);
void pci_dump_device(pci_device_t* dev);

// Коллбэк для сканирования устройств (опционально)
typedef void (*pci_scan_callback_t)(pci_device_t* dev);
void pci_scan(pci_scan_callback_t callback);

#endif