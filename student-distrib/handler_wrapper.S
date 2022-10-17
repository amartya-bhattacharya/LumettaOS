# handler_wrapper.S - wrapper for handler functions
# vim:ts=4 noexpandtab

#include "keyboard.h"
#include "rtc.h"

.globl handler_wrapper
.align 4

keyboard_handler_wrapper:
    pushl
    cld /* clear direction flag */
    call keyboard_handler
    popl
    iret

rtc_handler_wrapper:
    pushl
    cld /* clear direction flag */
    call rtc_handler
    popl
    iret