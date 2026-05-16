#ifndef EHCI_H
#define EHCI_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


// --- Регистры EHCI (смещения от базы MMIO) ---
#define CAPLENGTH_OFFSET    0x00
#define HCSPARAMS_OFFSET    0x04
#define HCCPARAMS_OFFSET    0x08

// Operational Registers (смещение от базы + CAPLENGTH)
#define USBCMD_OFFSET       0x00
#define USBSTS_OFFSET       0x04
#define USBINTR_OFFSET      0x08
#define FRINDEX_OFFSET      0x0C
#define PERIODICLISTBASE    0x14
#define ASYNCLISTADDR       0x18
#define CONFIGFLAG          0x40
#define PORTSC_BASE         0x44  // Первый порт (Port 0)

// Биты USBCMD
#define CMD_RUN_STOP        (1 << 0)
#define CMD_HCRESET         (1 << 1)
#define CMD_ASYNC_ENABLE    (1 << 5)
#define CMD_PERIODIC_ENABLE (1 << 4)

// Биты USBSTS
#define STS_HCHALTED        (1 << 12)
#define STS_USBINT          (1 << 0)

// Биты CONFIGFLAG
#define CF_FLAG             (1 << 0)

// Биты PORTSC
#define PORT_CURRENT_CONNECT    (1 << 0)
#define PORT_ENABLE             (1 << 2)
#define PORT_RESET              (1 << 8)
#define PORT_POWER              (1 << 12)

// --- Структуры DMA (32-байтные, выровнены по 64) ---
typedef struct {
    volatile uint32_t next_td_ptr;
    volatile uint32_t alt_next_td_ptr;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t ext_buffer[3];
} __attribute__((aligned(64))) ehci_td_t;

typedef struct {
    volatile uint32_t horiz_link_ptr;
    volatile uint32_t caps;
    volatile uint32_t curr_td_ptr;
    volatile uint32_t next_td_ptr;
    volatile uint32_t alt_next_td_ptr;
    volatile uint32_t token;
    volatile uint32_t buffer[5];
    volatile uint32_t pad[2];
} __attribute__((aligned(64))) ehci_qh_t;

// QH caps
#define QH_CAPS_SPEED_HIGH  (2 << 12)
#define QH_MAX_PACKET_MASK  0x07FF

// TD токены
#define TD_TOKEN_ACTIVE     (1 << 7)
#define TD_PID_OUT          0x00E1
#define TD_PID_IN           0x00E9
#define TD_PID_SETUP        0x002D

#define MAKE_TD_TOKEN(size, pid, ioc)  (((size) << 16) | ((ioc) ? (1 << 15) : 0) | (pid) | TD_TOKEN_ACTIVE)

// --- Параметры контроллера (HCSPARAMS) ---
#define HCS_N_PORTS_MASK    0x0F  // Количество портов

// --- Прототипы ---
bool ehci_init(uint32_t mmio_phys_base);
void ehci_poll();
bool ehci_init_auto(void);




#endif