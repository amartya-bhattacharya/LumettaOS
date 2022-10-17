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

/* Externally-visible functions */

extern void keyboard_init(void);
extern void keyboard_handler(void);
extern void keyboard_handler_wrapper();

#endif /* _KEYBOARD_H */
