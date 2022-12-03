/* pit.c - Functions used to interact with the PIT
 * vim:ts=4 sw=4 noexpandtab
 */

#include "pit.h"
#include "lib.h"
#include "i8259.h"
#include "filesystem.h"
#include "paging.h"
#include "x86_desc.h"
#include "scheduling.h"


void pit_init(void) {
    int divisor = PIT_FREQ / PIT_RATE;      /* Calculate our divisor */
    outb(PIT_CMD, PIT_CMD_PORT);            /* Set our command byte to 0x36 */
    outb(divisor & 0xFF, PIT_PORT);         /* Set low byte of divisor */
    outb(divisor >> 8, PIT_PORT);           /* Set high byte of divisor */
    enable_irq(PIT_IRQ);                    /* Enable PIT IRQ */
}

void pit_handler(void) {
    send_eoi(PIT_IRQ);                      /* Send EOI */
    cli();
    
    // task switching steps
    // 1. save esp and ebp
    // 2. switch paging, flush tlb

    union dirEntry d;
    pcb_t* pcb = get_pcb();

    // save esp and ebp
    asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=r"(pcb->saved_esp), "=r"(pcb->saved_ebp)
    );

    // launch two or more shells
    // maybe call scheduler here?

    pcb = get_pcb();

    d.val = 7;		//sets P and RW bits
    d.whole.ps = 1;
	d.whole.add_22_31 = (_8MB + _4MB * pcb->pid) >> 22; //bit shift by 22 to access correct address
	chgDir(32, d);	//32 = 128 / 4 (all programs are in the 128-132 MiB vmem page
    flushTLB();

    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8MB - (pcb->pid) * _8KB - 4; //subtract 4 for padding

    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        :
        :"r"(pcb->saved_esp), "r"(pcb->saved_ebp)
    );

    sti();
}
