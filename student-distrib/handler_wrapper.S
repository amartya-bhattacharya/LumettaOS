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

# wrapper for page fault handler
.globl pf_handler_wrapper
.align 4
pf_handler_wrapper:

    pushal
    pushfl
    pushl 8(%esp)
    call PF 
    addl $4, %esp;
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

    // execute:
    // call sys_execute

    // read:
    // call sys_read

    // write:
    // call sys_write

    // open:
    // call sys_open
    
    // close:
    // call sys_close





    //     pushl %ebx
    //     movl 0x10(%esp), %edx   #third arg 
    //     movl 0x0C(%esp), %ecx   #second arg
    //     movl 0x08(%esp), %ebx   #first arg
    //     movl $0x05, %eax        #system call
    //     int 0x80
    //     cmpl $0x0FFFFF001, %eax
    //     jb done
    //     xorl %edx, %edx
    //     subl %eax, %edx
    //     pushl %edx
    //     call __errno_location
    //     popl %ecx
    //     movl %ecx, (%eax)
    //     orl $0xFFFFFFFF, %eax

    // __errno_location:
    //     call getIP
    // raddr: addl $errno-raddr, %eax
    //         ret
    // getIP: movl (%esp), %eax
    //         ret




