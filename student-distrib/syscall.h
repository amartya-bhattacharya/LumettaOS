/* syscall.h - Defines for system calls
 * vim:ts=4 sw=4 noexpandtab
 */

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"

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
#define KERNEL_STACK_BOTTOM (PAGE_DIR_SIZE - 1) * _4KB

/* Externally-visible functions */
int32_t system_execute(const uint8_t * command);
int32_t open (const uint8_t* filename);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t close (int32_t fd);

#endif /* _SYSCALL_H */
