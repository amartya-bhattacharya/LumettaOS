/* terminal.c - Functions to interact with the terminal
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"
#include "keyboard.h"

// returns only when the enter key is pressed
// always add a newline character to the end of the buffer before returning
// 128 character limit includes the newline character
// handle buffer overflow (more than 127 characters)
// handle situations where the buffer is not full but the enter key is pressed
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    return 0;
}

int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    return 0;
}

int32_t terminal_open(const uint8_t* filename) {
    return 0;
}

int32_t terminal_close(int32_t fd) {
    return 0;
}
