# handler_wrapper.S - wrapper for handler functions
# vim:ts=4 noexpandtab

#define ASM     1

.globl keyboard_handler_wrapper
.align 4
keyboard_handler_wrapper:
    pushal
    cld /* clear direction flag */
    call keyboard_handler
    popal
    iret

.globl rtc_handler_wrapper
.align 4
rtc_handler_wrapper:
    pushal
    cld /* clear direction flag */
    call rtc_handler
    popal
    iret
