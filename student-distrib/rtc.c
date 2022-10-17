/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "i8259.h"
#include "x86_desc.h"
#include "lib.h"

// use test_interrupts to test rtc and show that rtc interrupts are being recieved
// virtualize the rtc by setting the frequency to the highest possible frequency and
// wait until you recieve x interupts to return back to the process such that
// those x interrupts at the highest frequency are equivalent to 1 interrupt at the
// frequency the process wants

void rtc_init(void) {
    // rate is the lower 4 bits of register A
    // int rate = RTC_REG_A & 0x0F;
    // set the rate to the highest possible rate
    // rate &= 0x0F;
    // cli();                 // disable interrupts
    outb(RTC_REG_B, RTC_PORT);      // select register B
    char prev = inb(RTC_DATA);      // read the current value of register B
    outb(RTC_REG_B, RTC_PORT);      // set the index again
    outb(prev | 0x40, RTC_DATA);    // write the previous value ORed with 0x40

    outb(RTC_REG_A, RTC_PORT);      // select register A
    prev = inb(RTC_DATA);      // read the current value of register A
    outb(RTC_REG_A, RTC_PORT);      // set the index again
    outb((prev & 0xF0) | 6, RTC_DATA);    // write the previous value ORed with 0x40

    // sti();                          // enable interrupts
    enable_irq(RTC_IRQ);
}

void rtc_handler(void) {
    // send eoi to rtc
    // read from register c to reset the interrupt
    outb(RTC_REG_C, RTC_PORT);
    inb(RTC_DATA);
    // test_interrupts();
    //clear();
    printf("RTC HANDLER\n");
    //putc('R');
    send_eoi(RTC_IRQ);
}
