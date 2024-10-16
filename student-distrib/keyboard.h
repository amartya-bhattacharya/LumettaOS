/* keyboard.c - Defines used to interact with the keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

/* Keyboard I/O ports */
#define KEYBOARD_DATA_PORT 0x60

/* Keyboard IRQ */
#define KEYBOARD_IRQ 0x01

/* Keyboard scan code set 1 size */
#define KEYBOARD_SCANCODE_SIZE 128

/* Keyboard buffer */
#define KEYBOARD_BUFFER_SIZE 128

/* Externally-visible variables */
extern char keyboard_buffer[128];
volatile extern int enterpress;

/* Externally-visible functions */

/* Initialize the keyboard */
extern void keyboard_init(void);
/* Enable (unmask) the keyboard IRQ */
extern void keyboard_handler(void);
/* Wrapper function for keyboard_handler */
extern void keyboard_handler_wrapper();

#endif /* _KEYBOARD_H */
