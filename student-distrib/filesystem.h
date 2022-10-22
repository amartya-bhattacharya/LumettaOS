/* filesystem file
 * vim:ts=4 sw=4 noexpandtab
 */

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"

struct dentry
{
	char name[32];	//if all 32 chars are filled no '\0'
	uint8_t ft;
	uint32_t idx;
};

int32_t read_dentry_by_name(const uint8_t* fname, dentry* dent);

int32_t read_dentry_by_index(uint32_t i, dentry* dent);

int32_t read_data(uint32_t inode, uint32_t off, uint8_t* buf, uint32_t len);

#endif
