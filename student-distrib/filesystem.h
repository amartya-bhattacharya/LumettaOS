/* filesystem file
 * vim:ts=4 sw=4 noexpandtab
 */

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"

struct dentry
{
	char name[32];	//if all 32 chars are filled no '\0'
	uint32_t ft;
	uint32_t ind;
	uint32_t reserve	: 24;
} __attribute__((packed));

struct inode
{
	uint32_t len;
	uint32_t data[1023];
} __attribute__((packed));

struct block
{
	uint32_t data[1024];
} __attribute__((packed));

struct bootblock
{
	uint32_t nent;
	uint32_t nnod;
	uint32_t nblck;
	uint32_t res[13];		//first 64 bytes
	struct dentry dirs[63];	//63 groups of 64 bytes
} __attribute__((packed));

int32_t read_dentry_by_name(const uint8_t* fname, struct dentry* dent);

int32_t read_dentry_by_index(uint32_t i, struct dentry* dent);

int32_t read_data(uint32_t inode, uint32_t off, uint8_t* buf, uint32_t len);

int32_t dir_open(const uint8_t* fn);

int32_t dir_read(int32_t fd, void* buf, int32_t n);

int32_t dir_write(int32_t fd, const void* buf, int32_t n);

int32_t dir_close(int32_t fd);

int32_t file_open(const uint8_t* fn);

int32_t file_read(int32_t fd, void* buf, int32_t n);

int32_t file_write(int32_t fd, const void* buf, int32_t n);

int32_t file_close(int32_t fd);

#endif
