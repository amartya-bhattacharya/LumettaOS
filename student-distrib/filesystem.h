/* filesystem file
 * vim:ts=4 sw=4 noexpandtab
 */

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"

#define BLKSIZE 4096

struct dentry
{
	uint8_t name[32];	//if all 32 chars are filled no '\0'
	uint32_t ft;
	uint32_t ind;		// inode index
	uint8_t res[24];
} __attribute__((packed));

struct inode
{
	uint32_t len;
	uint32_t data[1023];	//total size 1024 longs, len is 1 long
} __attribute__((packed));

struct block
{
	uint8_t data[BLKSIZE];
} __attribute__((packed));

struct bootblock
{
	uint32_t nent;
	uint32_t nnod;
	uint32_t nblck;
	uint8_t res[52];		//first 64 bytes
	struct dentry dirs[63];	//63 groups of 64 bytes
} __attribute__((packed));

struct fap
{
	int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
	int32_t (*open)(const uint8_t* filename);
	int32_t (*close)(int32_t fd);
} __attribute__((packed));


struct file_desc
{
	struct fap* f_op;	//jump table i.e. pointer to an array of functions
	uint32_t inode;
	uint32_t file_position;
	uint32_t flag;
} __attribute__((packed));

int32_t get_filetype(const uint8_t* fname);

int32_t get_inode(const uint8_t* fname);

int32_t read_dentry_by_name(const uint8_t* fname, struct dentry* dent);

int32_t read_dentry_by_index(uint32_t i, struct dentry* dent);

int32_t read_data(uint32_t inode, uint32_t off, uint8_t* buf, uint32_t len);

int32_t dir_open(const uint8_t* fn);

int32_t dir_read(int32_t fd, void* buf, int32_t n);

int32_t dir_write(int32_t fd, const void* buf, int32_t n);

int32_t dir_close(int32_t fd);

int32_t dir_execute(const uint8_t* command);

int32_t dir_halt (uint8_t status);

int32_t file_open(const uint8_t* fn);

int32_t file_read(int32_t fd, void* buf, int32_t n);

int32_t file_write(int32_t fd, const void* buf, int32_t n);

int32_t file_close(int32_t fd);

int32_t file_execute(const uint8_t* command);

int32_t file_halt (uint8_t status);

#endif
