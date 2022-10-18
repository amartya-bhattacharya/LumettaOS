/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "i8259.h"
#include "x86_desc.h"
#include "lib.h"


/*
 * rtc_init
 * DESCRIPTION: Initializes the RTC
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Enables the RTC, sets the frequency to the default
 */
void rtc_init(void) {
    outb(RTC_REG_B, RTC_PORT);          /* select register B, and disable NMI */
    char prev = inb(RTC_DATA);          /* read the current value of register B */
    outb(RTC_REG_B, RTC_PORT);          /* set the index again (a read will reset the index to register D) */
    outb(prev | 0x40, RTC_DATA);        /* write the previous value ORed with 0x40 */
    rtc_change_freq(RTC_BASE_RATE);     /* set the frequency to 1024 Hz */

    enable_irq(RTC_IRQ);                /* enable interrupts */
}


/*
 * rtc_change_freq
 * DESCRIPTION: Changes the frequency of the RTC
 * INPUTS: rate - the rate to change to
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Changes the frequency of the RTC
 */
void rtc_change_freq(int rate) {
    if (rate < RTC_BASE_RATE || rate > RTC_MAX_RATE) {      /* check if the rate is valid */
        return;
    }
    rate &= 0x0F;                           /* rate must be above 2 and not over 15 */
    outb(RTC_REG_A, RTC_PORT);              /* select register A, and disable NMI */
    char prev = inb(RTC_DATA);              /* read the current value of register A */
    outb(RTC_REG_A, RTC_PORT);              /* reset index to A */
    outb((prev & 0xF0) | rate, RTC_DATA);   /* write the previous value ORed with 0x40 */

    enable_irq(RTC_IRQ);                    /* enable interrupts */
}


/*
 * rtc_handler
 * DESCRIPTION: Handles the RTC interrupt
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Sends an EOI to the RTC
 */
void rtc_handler(void) {
    outb(RTC_REG_C, RTC_PORT);              /* select register C */
    inb(RTC_DATA);                          /* just throw away contents */
    send_eoi(RTC_IRQ);                      /* send EOI */
}
