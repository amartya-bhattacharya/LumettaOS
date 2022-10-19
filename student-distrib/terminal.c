/* terminal.c - Functions to interact with the terminal
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"
#include "keyboard.h"

/* Local variables */

// returns only when the enter key is pressed
// always add a newline character to the end of the buffer before returning
// 128 character limit includes the newline character
// handle buffer overflow (more than 127 characters)
// handle situations where the buffer is not full but the enter key is pressed
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    // read from keyboard buffer to buf until a newline '\n'
    // if the buffer is full, return -1
    for (int i = 0; i < 128; i++) {
        if (((char*)buf)[i] == '\n') {
            return i;
        }
    }
    return 0;
}

// writes nbytes of data from buf to the screen
// returns the number of bytes written
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    // write to screen
    // return number of bytes written
    if (nbytes < 0 || nbytes > 128) {
        return -1;
    } else {
        for (int i = 0; i < nbytes; i++) {
            putc(((char*)buf)[i]);
        }
        return nbytes;
    }
}

// initializes the terminal
int32_t terminal_open(const uint8_t* filename) {
    clear();
    return 0;
}

// clears any terminal-specific data
int32_t terminal_close(int32_t fd) {
    return 0;
}
