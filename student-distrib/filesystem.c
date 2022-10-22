/* filesystem file
 * vim:ts=4 sw=4 noexpandtab
 */

#include "filesystem.h"
#include "paging.h"

static struct uint32_t* filesystem;
static union tblEntry fstable[1024] __attribute__((aligned(4096)));

int32_t read_dentry_by_name(const uint8_t* fname, struct dentry* dent)
{
	return -1;
}

int32_t read_dentry_by_index(uint32_t i, struct dentry* dent)
{
	return -1;
}

int32_t read_data(uint32_t inode, uint32_t off, uint8_t* buf, uint32_t len)
{
	return -1;
}

void filesystem_init(uint32_t* add)
{
	union dirEntry e;
	e.val = (unsigned)fstable | 3;
	spawnTbl(fstable);
	chgDir(0, e);
	filesystem = add;
	return;
}
