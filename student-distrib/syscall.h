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
#define PROC_OFFSET 0x00480000
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

/* Struct for the PCB */
typedef struct pcb_t {
    struct file_desc file_desc_tb[8];   // file descriptor array
    uint8_t args[128];                  // arguments
    uint32_t pid;
    uint32_t parent_pid;
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint32_t active;
} pcb_t;

extern pcb_t * curr_pcb[MAX_PROCESSES];

/* Local functions */
pcb_t * get_pcb();
int32_t system_halt(uint8_t status);
int32_t system_execute(const uint8_t * command);
int32_t open (const uint8_t* filename);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t close (int32_t fd);

/* Assembly functions */
static inline int32_t execute(const uint8_t * command) {
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
}

static inline int32_t halt (uint8_t status) {
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
}

#endif /* _SYSCALL_H */
