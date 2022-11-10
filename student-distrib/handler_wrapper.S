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

# wrapper for systemcall interrupt handler
.globl syscall_wrapper
.align 4
eax_mem: .long 0x0

syscall_wrapper:
    pushal          #push all registers
    pushfl
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    cmpl $4, %eax
    ja invalid
    call *sys_call_table(, %eax, 4)
    movl %eax, eax_mem
    popl %ebx       #pop all registers
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popfl
    popal
    iret

    invalid:
    popl %ebx       #pop all registers
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popfl
    popal
    movl $-1, %eax  #move invalid value into eax
    ret

    sys_call_table:
    # .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
    .long sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close




