/* syscall.c - System call functions
 * vim:ts=4 sw=4 noexpandtab
 */

#include "syscall.h"
#include "lib.h"
#include "paging.h"

/* Local variables */
char buffer[32] = {0};
extern int32_t read_data(uint32_t inode, uint32_t off, uint8_t* buf, uint32_t len);

void system_execute(uint8_t * command) {
    // copy command into a buffer until /0 or /n is reached
    int i = 0;
    while (command[i] != '\0' && command[i] != '\n' && i < 32) {
        buffer[i] = command[i];
        i++;
    }
    buffer[i] = '\0';
    // check ELF header to see if it is a executable (read_data first 4 bytes)
    //read_data(0, 0, (uint8_t *)buffer, 4);  // boot->dirs[0].ind
    if (buffer[1] != 'E' || buffer[2] != 'L' || buffer[3] != 'F') {
        printf("Not an executable file");
    } else {
        printf("pass");
    }
    // set up paging for the program (flush TLB)
    // load in data
    // tss.esp0 = kernel stack pointer
    // set up and load pcb (setup fd[0] and fd[1])
}
