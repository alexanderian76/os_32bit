
#include "types.h"

#include "io.h"
// Простейший драйвер ATA PIO (Programmed Input/Output)
#define ATA_PRIMARY_DATA      0x1F0
#define ATA_PRIMARY_ERROR     0x1F1
#define ATA_PRIMARY_SECT_COUNT 0x1F2
#define ATA_PRIMARY_LBA_LO    0x1F3
#define ATA_PRIMARY_LBA_MID   0x1F4
#define ATA_PRIMARY_LBA_HI    0x1F5
#define ATA_PRIMARY_DRIVE     0x1F6
#define ATA_PRIMARY_CMD       0x1F7
#define ATA_PRIMARY_STATUS    0x1F7

#define ATA_CMD_READ          0x20
#define ATA_CMD_WRITE         0x30
#define ATA_SR_BSY            0x80
#define ATA_SR_DRQ            0x08

// Чтение сектора (512 байт) через ATA PIO
extern int ata_read_sector(uint32_t lba, void* buffer);

// Запись сектора
extern int ata_write_sector(uint32_t lba, const void* buffer);