/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "x86_desc.h"

// use test_interrupts to test rtc and show that rtc interrupts are being recieved
// virtualize the rtc by setting the frequency to the highest possible frequency and
// wait until you recieve x interupts to return back to the process such that
// those x interrupts at the highest frequency are equivalent to 1 interrupt at the
// frequency the process wants

void rtc_init(void) {
    // rate is the lower 4 bits of register A
    int rate = RTC_REG_A & 0x0F;
    // set the rate to the highest possible rate
    rate &= 0x0F;
    disable_ints();                 // disable interrupts
    outb(RTC_REG_B, RTC_PORT);      // select register B
    char prev = inb(RTC_DATA);      // read the current value of register B
    outb(RTC_REG_B, RTC_PORT);      // set the index again
    outb(prev | 0x40, RTC_DATA);    // write the previous value ORed with 0x40
    enable_ints();                  // enable interrupts
}

void rtc_handler(void) {
    // send eoi to rtc
    send_eoi(RTC_IRQ);
    // call test_interrupts from lib.c
    test_interrupts();
    // read from register c to reset the interrupt
    outb(RTC_REG_C, RTC_PORT);
    inb(RTC_DATA);
}