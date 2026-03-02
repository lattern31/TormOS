#include "isr.h"
#include "idt.h"
#include "gdt.h"
#include "keyboard.h"
#include "stdio.h"
#include "pic.h"
#include "ports.h"

void __attribute__((cdecl)) ISR_Handler(Registers* regs);
extern void __attribute__((cdecl)) ISR0();
extern void __attribute__((cdecl)) ISR1();
extern void __attribute__((cdecl)) IRQ1();

void ISR_init() {
    idt_setgate(0, ISR0, 0x08, 0x8E);
    idt_setgate(33, IRQ1, 0x08, 0x8E);
    pic_enable_irq(1);
}
void __attribute__((cdecl)) ISR_Handler(Registers* regs) {
    if (regs->interrupt == 33) {
        keyboard_handler();
    } else {
        term_print("Interrupt ");
        char buf[8];
        int_to_str(buf, regs->interrupt);
        term_print(buf);
        term_print("\n");
    }
    if (regs->interrupt >= 32 && regs->interrupt < 48) {
        if (regs->interrupt >= 40) {
            // Slave PIC
            outb(0xA0, 0x20);
        }
        // Master PIC
        outb(0x20, 0x20);
    }

}
