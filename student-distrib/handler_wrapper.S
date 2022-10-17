# handler_wrapper.S - common assembly linkage wrapper for handler functions
# vim:ts=4 noexpandtab

#define ASM     1

# wrapper for keyboard interrupt handler
.globl keyboard_handler_wrapper
.align 4
keyboard_handler_wrapper:
    pushal
    pushfl
    call keyboard_handler
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl rtc_handler_wrapper
.align 4
rtc_handler_wrapper:
    pushal
    pushfl
    call rtc_handler
    popfl
    popal
    iret
