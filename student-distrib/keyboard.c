/* keyboard.c - Functions to interact with the keyboard
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"


/* Local variables */
// keyboard buffer: 128 bytes
void* keyboard_buffer[128];
uint8_t key_status = 0;
// +-------+------+--------+-----+-----+------+------+-------+
// |   7   |   6  |   5    |  4  |  3  |  2   |  1   |   0   |
// +-------+------+--------+-----+-----+------+------+-------+
// |   reserved   | scroll | num | alt | ctrl | caps | shift |
// +----------+---+--------+-----+-----+------+------+-------+


unsigned char keymap[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 0x09 */
    '9', '0', '-', '=', '\b',	/* Backspace */     // TODO handle backspace
    '\t',			/* Tab */
    'q', 'w', 'e', 'r',	/* 0x10 - 0x13 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* Ctrl */  
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 0x1E - 0x27 */
    '\'', '`',   0,		/* Shift */ 
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 0x2B - 0x31 */
    'm', ',', '.', '/',   0,				/* Shift */
    '*',
    0,	/* Alt */
    ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 0x3B - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 - 0x44 */
    0,	/* 0x45 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};


unsigned char keymap_upper[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 0x09 */
    '(', ')', '_', '+', '\b',	/* Backspace */     // TODO handle backspace
    '\t',			/* Tab */
    'Q', 'W', 'E', 'R',	/* 0x13 */
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
    0,			/* Ctrl */  
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 0x27 */
    '"', '~',   0,		/* Shift */ 
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 0x31 */
    'M', '<', '>', '?',   0,				/* Shift */
    '*',
    0,	/* Alt */
    ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 0x3B - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 - 0x44 */
    0,	/* 0x45 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};


/*
 * keyboard_init
 * DESCRIPTION: Initializes the keyboard
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Enables the keyboard IRQ
 */
void keyboard_init(void) {
    enable_irq(KEYBOARD_IRQ);
}


// reads data from the keyboard
// returns the number of bytes read
// In the case of the keyboard, read should return data from one line that has
// been terminated by pressing Enter, or as much as fits in the buffer from one
// such line. The line returned should include the line feed character
// int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
//     int i;
//     char* buffer = (char*) buf;
//     for (i = 0; i < nbytes; i++) {
//         buffer[i] = getc();
//     }
//     return nbytes;
// }


/*
 * keyboard_handler
 * Handles the keyboard interrupt, converts scancode to char and prints to screen
 * DESCRIPTION: Handles keyboard interrupts
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Prints the character corresponding to the key pressed
 */
void keyboard_handler(void) {       // same as terminal_driver?
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);     /* read scancde from keyboard data port */
    
    // update key_status
    if (scancode & 0x80) {
        // key released
        // handle key up event, check that shift, ctrl, alt, caps lock, num lock, scroll lock are not pressed anymore
        // if they are, set the corresponding bit in key_status
        if (scancode == 0xAA || scancode == 0xB6) {
            // left or right shift released
            key_status &= ~0x01;
        } else if (scancode == 0x9D) {
            // ctrl released
            key_status &= ~0x04;
        } else if (scancode == 0xB8) {
            // alt released
            key_status &= ~0x08;
        }
    } else {
        // key pressed
        // handle key down event
        // check if shift, ctrl, alt, caps lock, num lock, scroll lock are pressed
        // if they are, set the corresponding bit in key_status
        // if not, print the character corresponding to the key pressed
        // if the key is a special key, handle it accordingly
        if (scancode == 0x2A || scancode == 0x36) {
            // left or right shift pressed
            key_status |= 0x01;
        } else if (scancode == 0x3A) {
            // caps lock pressed
            if (key_status & 0x02) {
                key_status &= ~0x02;
            } else {
                key_status |= 0x02;
            }
        } else if (scancode == 0x1D) {
            // ctrl pressed
            key_status |= 0x04;
        } else if (scancode == 0x38) {
            // alt pressed
            key_status |= 0x08;
        } else if (scancode == 0x45) {
            // num lock pressed
            if (key_status & 0x10) {
                key_status &= ~0x10;
            } else {
                key_status |= 0x10;
            }
        } else if (scancode == 0x46) {
            // scroll lock pressed
            if (key_status & 0x20) {
                key_status &= ~0x20;
            } else {
                key_status |= 0x20;
            }
        } else {
            // print the character corresponding to the key pressed
            // check Ctrl+L
            if (key_status & 0x04 && scancode == 0x26) {
                clear_term();                    // clear screen
            } else {
                if (key_status & 0x02) {
                    // caps lock is on
                    // print key status
                    // printf(key_status);
                    // if scancode is a letter, print the upper case letter
                    // else print the normal character corresponding to the scancode
                    if (((scancode >= 0x10 && scancode <= 0x19) || (scancode >= 0x1E && scancode <= 0x26) || (scancode >= 0x2C && scancode <= 0x32))) {
                        // print upper case letter
                        // if shift is pressed, print the normal character corresponding to the scancode
                        if (key_status & 0x01) {
                            putc_term(keymap[scancode]);
                        } else {
                            putc_term(keymap_upper[scancode]);
                        }
                    } else {
                        // print normal character
                        // if shift is pressed, print the upper case character corresponding to the scancode
                        if (key_status & 0x01) {
                            putc_term(keymap_upper[scancode]);
                        } else {
                            putc_term(keymap[scancode]);
                        }
                    }
                } else {
                    // caps lock is off
                    // if shift is on
                    if (key_status & 0x01) {
                        putc_term(keymap_upper[scancode]);
                    } else {
                        putc_term(keymap[scancode]);
                    }
                }
            }
        }
    }

    send_eoi(KEYBOARD_IRQ);                 /* send EOI */
}
