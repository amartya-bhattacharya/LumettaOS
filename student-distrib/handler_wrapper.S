# handler_wrapper.S - wrapper for handler functions
# vim:ts=4 noexpandtab

#define ASM     1

.globl keyboard_handler_wrapper
.align 4
keyboard_handler_wrapper:
    pushal
    pushfl
    call keyboard_handler
    popfl
    popal
    iret

.globl rtc_handler_wrapper
.align 4
rtc_handler_wrapper:
    pushal
    pushfl
    call rtc_handler
    popfl
    popal
    iret
