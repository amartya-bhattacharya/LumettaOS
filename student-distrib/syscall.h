/* syscall.h - Defines for system calls
 * vim:ts=4 sw=4 noexpandtab
 */

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"

int32_t system_execute(const uint8_t * command);
int32_t open (const uint8 t* filename);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t close (int32_t fd);

#endif /* _SYSCALL_H */
