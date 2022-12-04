/* scheduling.h - Defines for scheduling
 * vim:ts=4 sw=4 noexpandtab
 */

#ifndef _SCHEDULING_H
#define _SCHEDULING_H

#include "types.h"

/* Externally visible variables */
extern volatile int32_t curterm;    // current display
//extern int32_t current_process;
extern volatile int32_t procpid[3];

/* Externally visible functions */
void sched_init(void);
//switches the terminal to the one
void switchterm(int32_t t);


#endif /* _SCHEDULING_H */
