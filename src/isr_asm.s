.extern ISR_HANDLER

.global ISR0
ISR0:
    push $0
    push $0
    jmp isr_common

.global ISR1
ISR1:
    push $0
    push $1
    jmp isr_common

.global IRQ1
IRQ1:
    push $0
    push $33  // keyboard interrupt number1
    jmp isr_common

.global isr_common
isr_common:
    pusha

    xor %eax, %eax
    mov %ds, %ax
    push %eax
    mov $0x10 , %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push %esp
    call ISR_Handler
    add $4, %esp

    pop %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa
    add $8, %esp
    iret
