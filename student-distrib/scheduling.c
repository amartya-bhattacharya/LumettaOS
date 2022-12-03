/* scheduling.c - Functions used to interact with the scheduler
 * vim:ts=4 sw=4 noexpandtab
 */

#include "scheduling.h"

/* Local variables */
volatile int32_t current_terminal;
volatile int32_t current_process;
volatile int32_t current_process_pid[3] = {0};


// void init_scheduling(void) {
//     current_terminal = 0;
//     current_process = 0;
//     current_process_pid[0] = 0;
//     current_process_pid[1] = 0;
//     current_process_pid[2] = 0;
// }


// void switch_terminal(int32_t terminal) {
// }
