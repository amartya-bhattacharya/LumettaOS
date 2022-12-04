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
	char* byte;
	char* save = (char*)(0xB8 + (0xC0 - 0xB8) * (curterm + 1));
	cli();
	//printf("switching terms");
	//while(1);
	for(byte = (char*)0xB8;byte < (char*)0xC0;byte++)
	{	//save vid buffer for terminal
		*save = *byte;
		save++;
	}
	curterm = t;
	if(procpid[t] < 0)
	{	//when first opening the other terms they'll have no program
		sys_execute((uint8_t*)"shell");
		return;		//should never reach this
	}
	switchproc(procpid[t]);
	sti();
	return;
}
