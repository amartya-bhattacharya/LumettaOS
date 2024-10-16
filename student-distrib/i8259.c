/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC
 * Mask out all interrupts on the PIC, then initialize the PIC, initialize the devices,
 * and as part of each devices initialization, unmask the interrupts for that device
 * on the PIC */
void i8259_init(void) {
    /* Mask all of the interrupts */
    master_mask = 0xFF;
    slave_mask = 0xFF;
    
    /* Start the initialization sequence in cascade mode */
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);

    /* Set the master PIC to start at IRQ 0x20 */
    outb(ICW2_MASTER, MASTER_8259_PORT + 1);

    /* Set the slave PIC to start at IRQ 0x28 */
    outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);

    /* Tell the master PIC that there is a slave PIC at IRQ 2 */
    outb(ICW3_MASTER, MASTER_8259_PORT + 1);

    /* Tell the slave PIC its cascade identity */
    outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);

    /* Set the PICs to 8086 mode */
    outb(ICW4, MASTER_8259_PORT + 1);
    outb(ICW4, SLAVE_8259_PORT + 1);

    outb(master_mask, MASTER_8259_PORT + 1);
    outb(slave_mask, SLAVE_8259_PORT + 1);

    enable_irq(2);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    if (irq_num < 0 || irq_num > 15) {      /* Check if the IRQ is valid */
        return;
    }

    uint16_t port;
    uint8_t data;

    if (irq_num < 8) {
        port = MASTER_8259_PORT + 1;
    } else {
        port = SLAVE_8259_PORT + 1;
        irq_num -= 8;
    }

    data = inb(port) & ~(1 << irq_num);
    outb(data, port);
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    if (irq_num < 0 || irq_num > 15) {      /* Check if the IRQ is valid */
        return;
    }
    
    uint16_t port;
    uint8_t data;

    if (irq_num < 8) {
        port = MASTER_8259_PORT + 1;
    } else {
        port = SLAVE_8259_PORT + 1;
        irq_num -= 8;
    }

    data = inb(port) | (1 << irq_num);
    outb(data, port);
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num < 0 || irq_num > 15) {      /* Check if the IRQ is valid */
        return;
    }
    
    if (irq_num <= 7) {
        outb(EOI | irq_num, MASTER_8259_PORT);
    } else {
        outb(EOI | 2, MASTER_8259_PORT);
        outb(EOI | (irq_num - 8), SLAVE_8259_PORT);
    }
}
