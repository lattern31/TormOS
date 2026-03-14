#include "stdint.h"
#include "ports.h"

// Порты PIC
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

// Команды ICW (Initialization Command Words)
#define ICW1_INIT 0x11
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01


void pic_init() {
    inb(PIC1_DATA);
    inb(PIC2_DATA);
    
    // Инициализация Master PIC
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, 0x20);    // IRQ 0-7 -> INT 0x20-0x27
    outb(PIC1_DATA, 0x04);    // Slave PIC на IRQ2
    outb(PIC1_DATA, ICW4_8086);
    
    // Инициализация Slave PIC  
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_DATA, 0x28);    // IRQ 8-15 -> INT 0x28-0x2F
    outb(PIC2_DATA, 0x02);    // Подключен к IRQ2 Master
    outb(PIC2_DATA, ICW4_8086);
    
    // Восстанавливаем маски
    outb(PIC1_DATA, 0xFD);
    outb(PIC2_DATA, 0xFF);
}

void pic_enable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

void pic_disable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    
    value = inb(port) | (1 << irq);
    outb(port, value);
}
