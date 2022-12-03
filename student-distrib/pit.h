/* pit.h - Defines used in interactions with the PIT
 * vim:ts=4 sw=4 noexpandtab
 */

#ifndef _TIMER_H
#define _TIMER_H

#include "types.h"

/* Frequency of the PIT */
#define PIT_FREQ 1193182
#define PIT_RATE 20

/* Ports that each PIC sits on */
#define PIT_PORT 0x40
#define PIT_CMD_PORT 0x43

/* PIT commands */
#define PIT_CMD 0x36

/* PIT IRQ */
#define PIT_IRQ 0

/* Paging definitions */
#define _8MB 0x00800000
#define _4MB 0x00400000
#define _8KB 0x00002000

/* Externally-visible functions */
void pit_init(void);
void pit_handler(void);

#endif /* _TIMER_H */
