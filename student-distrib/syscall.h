/* syscall.h - Defines for system calls
 * vim:ts=4 sw=4 noexpandtab
 */

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "filesystem.h"

/* Page directory and page table constants */
#define PAGE_DIR_SIZE 1024
#define _132MB 0x08400000
#define _128MB 0x08000000
#define PROC_OFFSET 0x00048000
#define _8MB 0x00800000
#define _4MB 0x00400000
#define _8KB 0x00002000
#define _4KB 0x00001000
#define MASK_8KB 0xFFFFE000
#define MAX_PROCESSES 6
#define MAX_FILES 8
#define USER_STACK_POINTER 
#define KERNEL_STACK_BOTTOM (PAGE_DIR_SIZE - 1) * _4KB

/* System call numbers */
#define SYS_HALT 1
#define SYS_EXECUTE 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_GETARGS 7
#define SYS_VIDMAP 8
#define SYS_SET_HANDLER 9
#define SYS_SIGRETURN 10

/* Local functions */
int32_t sys_halt(uint8_t status);
int32_t sys_execute(const uint8_t * command);
int32_t sys_open (const uint8_t* filename);
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes);
int32_t sys_close (int32_t fd);
int32_t sys_getargs (uint8_t* buf, int32_t nbytes);

/* Wrapper function for syscall handler */
void syscall_wrapper();
extern void setup_context_switch(uint32_t esp, uint32_t eip);

/* Assembly functions */
/*static inline int32_t execute(const uint8_t * command) {
    int32_t ret;
    asm volatile (
        "push %ebx"
        "push %ecx"
        "push %edx"
        "movl 8(%ebp), %ebx"
        "movl $2, %eax"
        "int $0x80"
        "pop %edx"
        "pop %ecx"
        "pop %ebx"
        :"=r" (ret)
    );
    return ret;
}*/

/*static inline int32_t halt (uint8_t status) {
    int32_t ret;
    asm volatile (
        "push %ebx"
        "push %ecx"
        "push %edx"
        "movl 8(%ebp), %ebx"
        "movl $1, %eax"
        "int $0x80"
        "pop %edx"
        "pop %ecx"
        "pop %ebx"
        :"=r" (ret)
    );
    return ret;
}*/

#endif /* _SYSCALL_H */
