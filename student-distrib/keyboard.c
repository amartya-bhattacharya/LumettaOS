/* keyboard.c - Functions to interact with the keyboard
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "debug.h"
#include "tests.h"

/* Local variables */
// keycode array
char scancodes[59] = {
    '\0',                                       // 0x00
    '\0',                                       // 0x01
    '1',                                        // 0x02
    '2',                                        // 0x03
    '3',                                        // 0x04
    '4',                                        // 0x05
    '5',                                        // 0x06
    '6',                                        // 0x07
    '7',                                        // 0x08
    '8',                                        // 0x09
    '9',                                        // 0x0A
    '0',                                        // 0x0B
    '-',                                        // 0x0C
    '=',                                        // 0x0D
    '\b',                                       // 0x0E
    '\t',                                       // 0x0F
    'q',                                        // 0x10
    'w',                                        // 0x11
    'e',                                        // 0x12
    'r',                                        // 0x13
    't',                                        // 0x14
    'y',                                        // 0x15
    'u',                                        // 0x16
    'i',                                        // 0x17
    'o',                                        // 0x18
    'p',                                        // 0x19
    '[',                                        // 0x1A
    ']',                                        // 0x1B
    '\r',                                       // 0x1C
    '\0',                                       // 0x1D
    'a',                                        // 0x1E
    's',                                        // 0x1F
    'd',                                        // 0x20
    'f',                                        // 0x21
    'g',                                        // 0x22
    'h',                                        // 0x23
    'j',                                        // 0x24
    'k',                                        // 0x25
    'l',                                        // 0x26
    ';',                                        // 0x27
    '\'',                                       // 0x28
    '`',                                        // 0x29
    '\0',                                       // 0x2A // handle shift
    '\\',                                       // 0x2B
    'z',                                        // 0x2C
    'x',                                        // 0x2D
    'c',                                        // 0x2E
    'v',                                        // 0x2F
    'b',                                        // 0x30
    'n',                                        // 0x31
    'm',                                        // 0x32
    ',',                                        // 0x33
    '.',                                        // 0x34
    '/',                                        // 0x35
    '\0',                                       // 0x36 // handle shift
    '\0',                                       // 0x37
    '\0',                                       // 0x38
    ' ',                                        // 0x39
    '\0',                                       // 0x3A // handle caps lock
};

char scancodes_upper[59] = {
    '\0',                                       // 0x00
    '\0',                                       // 0x01
    '!',                                        // 0x02
    '@',                                        // 0x03
    '#',                                        // 0x04
    '$',                                        // 0x05
    '%',                                        // 0x06
    '^',                                        // 0x07
    '&',                                        // 0x08
    '*',                                        // 0x09
    '(',                                        // 0x0A
    ')',                                        // 0x0B
    '_',                                        // 0x0C
    '+',                                        // 0x0D
    '\b',                                       // 0x0E
    '\t',                                       // 0x0F
    'Q',                                        // 0x10
    'W',                                        // 0x11
    'E',                                        // 0x12
    'R',                                        // 0x13
    'T',                                        // 0x14
    'Y',                                        // 0x15
    'U',                                        // 0x16
    'I',                                        // 0x17
    'O',                                        // 0x18
    'P',                                        // 0x19
    '{',                                        // 0x1A
    '}',                                        // 0x1B
    '\r',                                       // 0x1C
    '\0',                                       // 0x1D
    'A',                                        // 0x1E
    'S',                                        // 0x1F
    'D',                                        // 0x20
    'F',                                        // 0x21
    'G',                                        // 0x22
    'H',                                        // 0x23
    'J',                                        // 0x24
    'K',                                        // 0x25
    'L',                                        // 0x26
    ':',                                        // 0x27
    '"',                                        // 0x28
    '~',                                        // 0x29
    '\0',                                       // 0x2A
    '|',                                        // 0x2B
    'Z',                                        // 0x2C
    'X',                                        // 0x2D
    'C',                                        // 0x2E
    'V',                                        // 0x2F
    'B',                                        // 0x30
    'N',                                        // 0x31
    'M',                                        // 0x32
    '<',                                        // 0x33
    '>',                                        // 0x34
    '?',                                        // 0x35
    '\0',                                       // 0x36
    '\0',                                       // 0x37
    '\0',                                       // 0x38
    ' ',                                        // 0x39
    '\0',                                       // 0x3A // handle caps lock
};

// need a keyboard init function?
// if so, need to make sure it's called after IDT is initialized
void keyboard_init(void) {
    enable_irq(KEYBOARD_IRQ);
    // outb(KEYBOARD_DATA_PORT, KEYBOARD_STATUS_PORT);
}

// keyboard handle function: convert scancode into char, putc
void keyboard_handler(void) {
    // get scancode
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    // check if scancode is valid
    if (scancode > 0x3A) {
        // invalid scancode
        return;
    }
    // check if scancode is a special key
    if (scancode == 0x2A || scancode == 0x36) {
        // shift key
        // handle
        return;
    }
    if (scancode == 0x3A) {
        // caps lock key
        // handle
        return;
    }
    // valid scancode
    // convert scancode to char
    char c = scancodes[scancode];
    // putc
    putc(c);
}
