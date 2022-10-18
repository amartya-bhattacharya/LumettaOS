/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "i8259.h"
#include "x86_desc.h"
#include "lib.h"

/* Local variables */
uint8_t rtc_status = 0;

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
    rtc_open();                         /* set the frequency to 2 Hz */

    enable_irq(RTC_IRQ);                /* enable interrupts */
    rtc_status = 1;                     /* set the status to open */
}


/*
 * rtc_change_freq
 * DESCRIPTION: Changes the frequency of the RTC
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Changes the frequency of the RTC to 2 Hz
 */
void rtc_open(void) {                         /* rate must be above 2 and not over 15 */
    outb(RTC_REG_A, RTC_PORT);              /* select register A, and disable NMI */
    char prev = inb(RTC_DATA);              /* read the current value of register A */
    outb(RTC_REG_A, RTC_PORT);              /* reset index to A */
    outb((prev & 0xF0) | RTC_BASE_RATE, RTC_DATA);   /* write only our rate to A */
}   // TODO do I have to disable interrupts for the duration of this function?


/*
 * rtc_read
 * DESCRIPTION: Reads data from the RTC
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: Wait for an interrupt to occur, then return 0
 * SIDE EFFECTS: Reads data from the RTC
 */
int rtc_read(void) {
    /* wait for the next interrupt */
    while (rtc_status == 0) {
        /* do nothing */
    }
    rtc_status = 0;
    return 0;
}


/*
 * rtc_write        // TODO virtualization
 * DESCRIPTION: Writes data to the RTC, sets the frequency to the given rate
 * INPUTS: freq - the rate to change the frequency to
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Changes the frequency of the RTC
 */
void rtc_write(int32_t freq) {      // TODO must get its input parameter through a buffer and not read the value directly
    int i;
    int rate = 0;
    for (i = 6; i < 15; i++) {              /* parameter check */
        if (freq == (32768 >> (i - 1))) {   
            rate = i;
            break;
        }
    }
    if (rate == 0) {        
        return;
    }

    rate &= 0x0F;                           /* rate must be above 6 and not over 15 */
    outb(RTC_REG_A, RTC_PORT);              /* select register A, and disable NMI */
    char prev = inb(RTC_DATA);              /* read the current value of register A */
    outb(RTC_REG_A, RTC_PORT);              /* reset index to A */
    outb((prev & 0xF0) | rate, RTC_DATA);   /* write the previous value ORed with 0x40 */
}   // TODO do I have to disable interrupts for the duration of this function?


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
