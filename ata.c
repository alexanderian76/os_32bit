#include "ata.h"

// Чтение сектора (512 байт) через ATA PIO
int ata_read_sector(uint32_t lba, void* buffer) {
    // Ждём готовности диска
    while (inb(ATA_PRIMARY_STATUS) & ATA_SR_BSY);
    
    // Настраиваем параметры
    outb(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));  // Master, LBA
    outb(ATA_PRIMARY_SECT_COUNT, 1);
    outb(ATA_PRIMARY_LBA_LO, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HI, (lba >> 16) & 0xFF);
    
    // Отправляем команду чтения
    outb(ATA_PRIMARY_CMD, ATA_CMD_READ);
    
    // Ждём готовности данных
    while (inb(ATA_PRIMARY_STATUS) & ATA_SR_BSY);
    
    if (!(inb(ATA_PRIMARY_STATUS) & ATA_SR_DRQ)) {
        return -1;  // Ошибка
    }
    
    // Читаем данные (256 слов по 16 бит)
    uint16_t* buf = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        buf[i] = inw(ATA_PRIMARY_DATA);
    }
    
    return 1;
}

// Запись сектора
int ata_write_sector(uint32_t lba, const void* buffer) {
    while (inb(ATA_PRIMARY_STATUS) & ATA_SR_BSY);
    
    outb(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECT_COUNT, 1);
    outb(ATA_PRIMARY_LBA_LO, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HI, (lba >> 16) & 0xFF);
    
    outb(ATA_PRIMARY_CMD, ATA_CMD_WRITE);
    
    while (inb(ATA_PRIMARY_STATUS) & ATA_SR_BSY);
    
    if (!(inb(ATA_PRIMARY_STATUS) & ATA_SR_DRQ)) {
        return -1;
    }
    
    const uint16_t* buf = (const uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        outw(ATA_PRIMARY_DATA, buf[i]);
    }
    
    // Ждём завершения записи
    while (inb(ATA_PRIMARY_STATUS) & ATA_SR_BSY);
    
    return 1;
}