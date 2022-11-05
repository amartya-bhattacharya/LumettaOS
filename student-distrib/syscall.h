/* syscall.h - Defines for system calls
 * vim:ts=4 sw=4 noexpandtab
 */

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"

int32_t system_execute(const uint8_t * command);

#endif /* _SYSCALL_H */
