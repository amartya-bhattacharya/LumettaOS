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
#define RTC_IRQ     8

/* RTC frequencies */
#define RTC_BASE_RATE   6

/* Externally-visible functions */
void rtc_init(void);
void rtc_handler(void);

#endif /* _RTC_H */
