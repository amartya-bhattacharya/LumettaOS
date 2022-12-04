/* keyboard.c - Functions to interact with the keyboard
 * vim:ts=4 noexpandtab shiftwidth=4
 */

#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"
#include "terminal.h"
#include "scheduling.h"


/* Local variables */
// keyboard buffer
char keyboard_buffer[3][KEYBOARD_BUFFER_SIZE];
// keyboard buffer index
volatile int keyboard_buffer_index[3] = {0, 0, 0};
// special key flag
volatile uint8_t key_status = 0;
volatile int enterpress = 0;
// +-------+------+--------+-----+-----+------+------+-------+
// |   7   |   6  |   5    |  4  |  3  |  2   |  1   |   0   |
// +-------+------+--------+-----+-----+------+------+-------+
// |   reserved   | scroll | num | alt | ctrl | caps | shift |
// +----------+---+--------+-----+-----+------+------+-------+


unsigned char keymap[KEYBOARD_SCANCODE_SIZE] =
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


unsigned char keymap_upper[KEYBOARD_SCANCODE_SIZE] =
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


unsigned char handle_standard_key(uint8_t scancode) {
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
                return (keymap[scancode]);
            } else {
                return (keymap_upper[scancode]);
            }
        } else {
            // print normal character
            // if shift is pressed, print the upper case character corresponding to the scancode
            if (key_status & 0x01) {
                return (keymap_upper[scancode]);
            } else {
                return (keymap[scancode]);
            }
        }
    } else {
        // caps lock is off
        // if shift is on
        if (key_status & 0x01) {
            return (keymap_upper[scancode]);
        } else {
            return (keymap[scancode]);
        }
    }
}

/*
 * keyboard_handler
 * Handles the keyboard interrupt, converts scancode to char and prints to screen
 * DESCRIPTION: Handles keyboard interrupts
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: Prints the character corresponding to the key pressed
 */
void keyboard_handler(void) {
    int i;
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
                // write to terminal (-1 is because there needs space for \n)
                unsigned char c = handle_standard_key(scancode);
                if ((c == '\b' && keyboard_buffer_index[curterm] != 0)) {
                    if (keyboard_buffer[curterm][keyboard_buffer_index[curterm] - 1] == '\t') {
                        // handle tab backspace
                        for (i = 0; i < 4; i++) {
                            backspace_pressed();
                            move_cursor();
                        }
                    } else if (keyboard_buffer_index[curterm] != 0) {
                        putc_term(c);
                    }
                    keyboard_buffer_index[curterm]--;
                }
                else if (keyboard_buffer_index[curterm] < KEYBOARD_BUFFER_SIZE - 1 && c != '\b') {
                    // backspace normal character
                    putc_term(c);
                }
                // write to keyboard buffer
                if (c != 0 && keyboard_buffer_index[curterm] < KEYBOARD_BUFFER_SIZE - 1 && c != '\b') {
                    keyboard_buffer[curterm][keyboard_buffer_index[curterm]] = c;
                    keyboard_buffer_index[curterm]++;
                }
				if (c == '\n')
				{
					if(keyboard_buffer_index[curterm] == KEYBOARD_BUFFER_SIZE - 1)
					{	//edge case if newline is last char
						keyboard_buffer[curterm][keyboard_buffer_index[curterm]] = c;
						keyboard_buffer_index[curterm]++;
						putc_term(c);
					}
                    enterpress = 1;
					// terminal_write(0, keyboard_buffer, keyboard_buffer_index);      // TODO move this into tests.c
					keyboard_buffer_index[curterm] = 0;
                    // memset(keyboard_buffer, 0, 128);
				}
            }
        }
    }

    send_eoi(KEYBOARD_IRQ);                 /* send EOI */
}
