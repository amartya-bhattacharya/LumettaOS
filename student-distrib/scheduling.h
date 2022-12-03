// /* scheduling.h - Defines for scheduling
//  * vim:ts=4 sw=4 noexpandtab
//  */

// #ifndef _SCHEDULING_H
// #define _SCHEDULING_H

// #include "types.h"

// /* Externally visible variables */
// extern int32_t current_terminal;    // current display
// extern int32_t current_process;
// extern int32_t current_process_pid[3];

// /* Externally visible functions */
// extern void init_scheduling(void);  // set up shells
// extern void schedule(void);
// extern void switch_terminal(int32_t terminal);

// void setup_shells();
// void task_switch(int PID);
// void vidchange(int from, int to);
// void init_terms();

// void asynchronous_task_switch(int new_display);

// #endif /* _SCHEDULING_H */