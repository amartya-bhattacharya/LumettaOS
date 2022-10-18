/* rtc.h - Defines used in interactions with the RTC
 * vim:ts=4 noexpandtab
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"

/* RTC ports */
#define RTC_PORT    0x70
#define RTC_DATA    0x71

/* RTC register addresses */
#define RTC_REG_A   0x8A
#define RTC_REG_B   0x8B
#define RTC_REG_C   0x0C

/* RTC IRQ */
#define RTC_IRQ     0x08

/* RTC rates */
#define RTC_BASE_RATE   15
#define RTC_MAX_RATE    6

/* Externally-visible functions */

/* Initialize the RTC */
void rtc_init(void);
/* Reset the RTC frequency */
void rtc_open(void);
/* Read from the RTC */
int rtc_read(void);
/* Change the RTC frequency */
void rtc_write(int rate);
/* RTC interrupt handler */
void rtc_handler(void);
/* Wrapper function for RTC handler */
void rtc_handler_wrapper();

#endif /* _RTC_H */
