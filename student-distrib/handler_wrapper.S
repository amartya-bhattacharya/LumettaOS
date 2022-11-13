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

    pushal  #4 bytes * 8 registers
    pushfl  #4 bytes
    movl %cr2, %eax
    pushl %eax  #4 bytes
    pushl 40(%esp)
    call PF 
    addl $8, %esp;
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl de_handler_wrapper
.align 4
de_handler_wrapper:
    pushal
    pushfl
    call DE
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl db_handler_wrapper
.align 4
db_handler_wrapper:
    pushal
    pushfl
    call DB
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl nmi_handler_wrapper
.align 4
nmi_handler_wrapper:
    pushal
    pushfl
    call NMI
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl bp_handler_wrapper
.align 4
bp_handler_wrapper:
    pushal
    pushfl
    call BP
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl of_handler_wrapper
.align 4
of_handler_wrapper:
    pushal
    pushfl
    call OF
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl br_handler_wrapper
.align 4
br_handler_wrapper:
    pushal
    pushfl
    call BR
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl ud_handler_wrapper
.align 4
ud_handler_wrapper:
    pushal
    pushfl
    call UD
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl nm_handler_wrapper
.align 4
nm_handler_wrapper:
    pushal
    pushfl
    call NM
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl df_handler_wrapper
.align 4
df_handler_wrapper:
    pushal
    pushfl
    call DF
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl cso_handler_wrapper
.align 4
cso_handler_wrapper:
    pushal
    pushfl
    call CSO
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl ts_handler_wrapper
.align 4
ts_handler_wrapper:
    pushal
    pushfl
    call TS
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl np_handler_wrapper
.align 4
np_handler_wrapper:
    pushal
    pushfl
    call NP
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl ss_handler_wrapper
.align 4
ss_handler_wrapper:
    pushal
    pushfl
    call SS
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl gp_handler_wrapper
.align 4
gp_handler_wrapper:
    pushal
    pushfl
    call GP
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl r_handler_wrapper
.align 4
r_handler_wrapper:
    pushal
    pushfl
    call R
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl mf_handler_wrapper
.align 4
mf_handler_wrapper:
    pushal
    pushfl
    call MF
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl ac_handler_wrapper
.align 4
ac_handler_wrapper:
    pushal
    pushfl
    call AC
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl mc_handler_wrapper
.align 4
mc_handler_wrapper:
    pushal
    pushfl
    call MC
    popfl
    popal
    iret

# wrapper for rtc interrupt handler
.globl xf_handler_wrapper
.align 4
xf_handler_wrapper:
    pushal
    pushfl
    call XF
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
    pushl %edx
    pushl %ecx
    pushl %ebx
    cmpl $5, %eax
    ja invalid
    call *sys_call_table(, %eax, 4)
    movl %eax, eax_mem
    popl %ebx       #pop all registers
    popl %ecx
    popl %edx
    popfl
    popal
    iret

    invalid:
    popl %ebx       #pop all registers
    popl %ecx
    popl %edx
    popfl
    popal
    movl $-1, %eax  #move invalid value into eax
    iret

    sys_call_table:
    # .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
    .long sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close




