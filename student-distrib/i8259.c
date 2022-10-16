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
    /* Save the current masks */
    uint8_t m1, m2;
    m1 = inb(MASTER_8259_PORT + 1);
    m2 = inb(SLAVE_8259_PORT + 1);

    /* Mask all of the interrupts */
    master_mask = 0xFF;
    slave_mask = 0xFF;
    outb(master_mask, MASTER_8259_PORT + 1);
    io_wait();
    outb(slave_mask, SLAVE_8259_PORT + 1);
    io_wait();
    
    /* Start the initialization sequence in cascade mode */
    outb(ICW1, MASTER_8259_PORT);
    io_wait();
    outb(ICW1, SLAVE_8259_PORT);
    io_wait();

    /* Set the master PIC to start at IRQ 0x20 */
    outb(ICW2_MASTER, MASTER_8259_PORT + 1);
    io_wait();

    /* Set the slave PIC to start at IRQ 0x28 */
    outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);
    io_wait();

    /* Tell the master PIC that there is a slave PIC at IRQ 2 */
    outb(ICW3_MASTER, MASTER_8259_PORT + 1);
    io_wait();

    /* Tell the slave PIC its cascade identity */
    outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);
    io_wait();

    /* Set the PICs to 8086 mode */
    outb(ICW4, MASTER_8259_PORT + 1);
    io_wait();
    outb(ICW4, SLAVE_8259_PORT + 1);
    io_wait();

    /* Restore the masks */
    // restore masks or unmask interrupts one device at a time using enable disable?
    outb(m1, MASTER_8259_PORT + 1);
    io_wait();
    outb(m2, SLAVE_8259_PORT + 1);
    io_wait();

    enable_irq(0x2);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
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
    if (irq_num > 7) {
        outb(EOI | irq_num, SLAVE_8259_PORT);
    }
    outb(EOI | irq_num, MASTER_8259_PORT);
}
