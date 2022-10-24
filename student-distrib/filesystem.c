/* filesystem file
 * vim:ts=4 sw=4 noexpandtab
 */

#include "filesystem.h"
#include "lib.h"
//#include "paging.h"

//address of bootblock which is also address of start of filesystem
static struct bootblock* boot;

//inode number for current file
static uint32_t file = 64;	//64 is always out of bounds

//saves amount of bytes already read in a file
static uint32_t offset;

//saves index of last accessed directory
static uint32_t dnum;

//static union tblEntry fstable[1024] __attribute__((aligned(4096)));

/*
 * Changes dent to be that of corresponding filename
 * Inputs: filename, dentry
 * Outputs: success/failure, changed dentry
 */
int32_t read_dentry_by_name(const uint8_t* fname, struct dentry* dent)
{
	int i;
	for(i = 0;i < boot->nent;i++)	//iterate through dentries
	{
		if(strncmp((int8_t*)fname, (int8_t*)(boot->dirs[i].name), 32) == 0)	//maximum size of a filename 
		{
			*dent = boot->dirs[i];
			return 0;
		}
	}
	return -1;
}

/*
 * Takes index of data entries and puts corresponding one into dent
 * Inputs: index, dentry
 * Outputs: success/failure, changed dentry
 */
int32_t read_dentry_by_index(uint32_t i, struct dentry* dent)
{
	if(boot->nent < i)
		return -1;
	*dent = boot->dirs[i];
	return 0;
}

/*
 * Reads length amount of bytes from file inode starting at offset bytes in file
 * Inputs: inode#, offset, buffer, length
 * Outputs: bytes written, changed buffer
 */
int32_t read_data(uint32_t nd, uint32_t off, uint8_t* buf, uint32_t len)
{
	uint32_t i;
	struct block* blk;
	struct inode* nod;
	if(nd >= boot->nnod)
		return -1;
	nod = (struct inode*)(boot + nd + 1);	//inode block is 4096 bytes, offset from boot
	if(nod->len < len + off)	//if asking for more data than available
	{	//adjusts len to read maximum number of bytes
		len = nod->len - off;
	}

	//ptr calculation: since boot is of size 4096 bytes if I add 1 to it it adds 4096 bytes to the address,
	//therefore I can just add the number of blocks
	blk = (struct block*)(boot + boot->nnod + 1 + nod->data[off / BLKSIZE]);
	for(i = 0;len - i > 0;i++)
	{
		buf[i] = blk->data[off % BLKSIZE];
		off++;
		if(off % BLKSIZE == 0)
		{	//I can cut the if statement and set the blk ptr every loop too if it saves time
			blk = (struct block*)(boot + boot->nnod + 1 + nod->data[off / BLKSIZE]);
		}
	}
	return len;
}

/*
 * Loads pointer of boot block, which is the start of a list
 * of 4KiB blocks that make up the filesystem.
 * Input: address of boot block
 * Output: success
 */
int32_t dir_open(const uint8_t* fn)
{
	/*union dirEntry e;
	e.val = (unsigned)fstable | 3;
	spawnTbl(fstable);
	fstable[0].p = 1;		//setup boot block
	chgDir(2, e);			//puts filesystem in the virtual memory space right after kernel*/
	boot = (struct bootblock*)fn;
	dnum = 0;
	return 0;
}

/*
 * Reads a filename
 * Inputs: buf
 * Outputs: success, changed buf
 */
int32_t dir_read(int32_t fd, void* buf, int32_t n)
{
	int32_t i;
	struct dentry d;
	if(read_dentry_by_index(dnum, &d))
		return -1;
	dnum++;
	for(i = 0;i < n;i++)
	{
		((uint8_t*)buf)[i] = d.name[i];
		if(d.name[i] == 0)
		{
			n = i + 1;
			break;
		}
	}
	return n;
}

/*
 * Does nothing
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t n)
{
	return -1;
}

/*
 * "Closes" the filesystem
 * Sets index of last accessed directory to 0
 * Input: address of filesystem (irrelevant with only one)
 * Output: success
 */
int32_t dir_close(int32_t fd)
{
	dnum = 0;
	return 0;
}

/*
 * Takes a filename and makes adjusts the inode* to point to the file
 * Input: filename
 * Output: success or failure of finding/loading file
 */
int32_t file_open(const uint8_t* fn)
{
	struct dentry d;
	if(read_dentry_by_name(fn, &d))
		return -1;
	file = d.ind;
	offset = 0;
	return 0;
}

/*
 * Reads n bytes of data from file into buf
 * Inputs: buf, n	(might remove fd since file is loaded in open()?)
 * Outputs: number of bytes read, changed buf
 */
int32_t file_read(int32_t fd, void* buf, int32_t n)
{
	n = read_data(file, offset, (uint8_t*)buf, n);
	offset += n;
	return n;
}

/*
 * Does nothing
 */
int32_t file_write(int32_t fd, const void* buf, int32_t n)
{
	return -1;
}

/*
 * Sets the file pointer to NULL so that files can't be edited post close
 * Input: none; Output: success
 */
int32_t file_close(int32_t fd)
{
	file = 64;	//sets file to something out of bounds
	offset = 0;
	return 0;
}
