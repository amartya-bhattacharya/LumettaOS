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

/* RTC frequencies */
#define RTC_BASE_FREQ   2
#define RTC_MAX_FREQ    1024

/* for testing */
#define RTC_TEST_EN 0

/* Externally-visible functions */
/* Set the RTC rate to the given frequency */
void rtc_set_rate(uint32_t freq);
/* Initialize the RTC */
void rtc_init(void);
/* Read from the RTC */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
/* Change the RTC frequency */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
/* Reset the RTC frequency */
int32_t rtc_open(const uint8_t* filename);
/* Close the RTC */
int32_t rtc_close(int32_t fd);  // TODO do I need this?
/* RTC interrupt handler */
void rtc_handler(void);
/* Wrapper function for RTC handler */
void rtc_handler_wrapper();

extern int interrupt_count;
extern int i_rtc;
#endif /* _RTC_H */
