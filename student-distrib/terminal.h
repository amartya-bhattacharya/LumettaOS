/* terminal.h - Defines for terminal driver
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"

/* Number of terminals */
#define NUM_TERMINALS 3

/* Number of rows and columns in a terminal */
#define NUM_ROWS 25
#define NUM_COLS 80

/* Externally-visible functions */

/* Read from the terminal */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
/* Write to the terminal */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
/* Open the terminal */
int32_t terminal_open(const uint8_t* filename);
/* Close the terminal */
int32_t terminal_close(int32_t fd);

#endif /* _TERMINAL_H */
