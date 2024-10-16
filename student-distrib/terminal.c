/* terminal.c - Functions to interact with the terminal
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"
#include "keyboard.h"

/* Local variables */
// must have a separate input buffer for each terminal
// must save save the current text screen and cursor position as the terminal state
// switching terminals is equivalent to switching the terminal states

// returns only when the enter key is pressed
// always add a newline character to the end of the buffer before returning
// 128 character limit includes the newline character
// handle buffer overflow (more than 127 characters)
// handle situations where the buffer is not full but the enter key is pressed

/* terminal_read
 * DESCRIPTION: Reads from the keyboard buffer
 * INPUTS: fd - file descriptor
 *         buf - buffer to read into
 *         nbytes - number of bytes to read
 * OUTPUTS: NONE
 * RETURN VALUE: number of bytes read
 * SIDE EFFECTS: Reads from the keyboard buffer
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    // read from keyboard buffer to buf until a newline '\n' or as much as fits in the buffer
    // if the buffer is full, return -1
	int i;
   // char c = 0;
    if (buf == NULL) {
        return -1;
    }
    if (nbytes > 128) nbytes = 128;
    //char temp_buffer[128] = {0};

    sti();

    // while(keyboard_buffer[0] == '\0');
    i = 0;
    while(!enterpress);
    for (i = 0; i < 128; i++) {
        if ((char) keyboard_buffer[i] == '\n')
        break;
    }

    //clear buffer
    //putc(i + '0');
    strncpy((char*)buf, keyboard_buffer, i + 1);
    memset(keyboard_buffer, 0, 128);
    enterpress = 0;
    // add newline character to the end of the buffer
    // *((char*)buf + i + 1) = '\n';
    // sti();

    cli();
    
    return i + 1;
}


// writes nbytes of data from buf to the screen
// returns the number of bytes written
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    // write to screen
    // return number of bytes written
	int i;
    cli();
    if (nbytes <= 0 || buf == NULL) {
        sti();
        return -1;
    } else {
        for (i = 0; i < nbytes; i++) {
            putc_term(((char*)buf)[i]);
        }
        sti();
        return nbytes;
    }
    
}


// initializes the terminal
int32_t terminal_open(const uint8_t* filename) {
    clear_term();
    return 0;
}

// clears any terminal-specific data
int32_t terminal_close(int32_t fd) {
    return 0;
}

int32_t terminal_open_fail(const uint8_t* filename){ //stdin and stdout
    return -1;
}

int32_t terminal_close_fail(int32_t fd){
    return -1;
}
