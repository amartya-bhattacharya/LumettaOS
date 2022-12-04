/* scheduling.c - Functions used to interact with the scheduler
 * vim:ts=4 sw=4 noexpandtab
 */

#include "scheduling.h"
#include "syscall.h"
#include "lib.h"

/* Local variables */
volatile int32_t curterm;
//volatile int32_t current_process;
volatile int32_t procpid[3];


void sched_init(void) {
	curterm = 0;
	//current_process = 0;
	procpid[0] = -1;
	procpid[1] = -1;
	procpid[2] = -1;
}

void switchterm(int32_t t)
{
	cli();
	curterm = t;
	clear_term();
	if(procpid[t] < 0)
	{	//when first opening the other terms they'll have no program
		sys_execute((uint8_t*)"shell");
		return;		//should never reach this
	}
	switchproc(procpid[t]);
	sti();
	return;
}
