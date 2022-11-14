/* filesystem file
 * vim:ts=4 sw=4 noexpandtab
 */

#include "filesystem.h"
#include "lib.h"
//#include "paging.h"

#define _8MB 0x00800000
#define _4MB 0x00400000
#define _8KB 0x00002000
#define _4KB 0x00001000

pcb_t* curr_pcb[6] =
{
	(pcb_t*)(_8MB - _8KB),
	(pcb_t*)(_8MB - 2 * _8KB),
	(pcb_t*)(_8MB - 3 * _8KB),
	(pcb_t*)(_8MB - 4 * _8KB),
	(pcb_t*)(_8MB - 5 * _8KB),
	(pcb_t*)(_8MB - 6 * _8KB)
};  // array of pointers to pcb's

//address of bootblock which is also address of start of filesystem
static struct bootblock* boot;
/*
 * The next 3 variables were used before the implementation of pcb
 */
//inode number for current file
//static uint32_t file[8] = {0, 0, 64, 64, 64, 64, 64, 64};	//64 is always out of bounds

//array that shows available files
//static uint8_t avlfiles = 0xFC;

//saves amount of bytes already read in a file
//static uint32_t offset[8];

//saves index of last accessed directory
//static uint32_t dnum;

//static union tblEntry fstable[1024] __attribute__((aligned(4096)));

/*
 * get_PCB
 * DESCRIPTION: initializes PCB
 * INPUTS: NONE
 * OUTPUTS: intialized PCB  
 */
pcb_t* get_pcb() {
    pcb_t* pcb;
	uint32_t e;	//this is esp
	asm(
		"movl %%esp, %0;"
		: "=r" (e)
	);
	//0x3FF is 1023, this calculates whether esp is in p0, p1, etc
	// e / _8KB is 0x3FF if esp is in p0, 0x3FE if in p1, etc.
	e = 0x3FF - (e / _8KB);
	if(e >= 6)	//number of shells
		return NULL;
	pcb = curr_pcb[e];
    return pcb;
}

/*
 * get_filetype
 * DESCRIPTION: helper function that gets the file type for sys_open
 * INPUTS: fname (file name)
 * OUTPUTS: returns file type, returns -1 if invalid  
 */
/*int32_t get_filetype(const uint8_t* fname){
	struct dentry d;
	
	if(read_dentry_by_name(fname, &d))
		return -1;
	
	return (int32_t)d.ft;
}*/

/*
 * get inode
 * DESCRIPTION: helper function that gets the inode for sys_open
 * INPUTS: fname (file name)
 * OUTPUTS: returns inode, returns -1 if invalid  
 */
/*int32_t get_inode(const uint8_t* fname){
	struct dentry d;
	if(read_dentry_by_name(fname, &d))
		return -1;
	return d.ind;
}*/
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
	//dnum = 0;
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
	pcb_t* p = get_pcb();
	if(read_dentry_by_index(p->file_desc_tb[fd].file_position, &d))
		return -1;
	p->file_desc_tb[fd].file_position++;
	for(i = 0;i < n;i++)
	{
		((uint8_t*)buf)[i] = d.name[i];
		if(d.name[i] == 0)
		{
			n = i + 1;
			break;
		}
	}
	return i;
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
	pcb_t* p = get_pcb();
	p->file_desc_tb[fd].flag = 0;
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
	int32_t i;
	pcb_t* p = get_pcb();
	if(read_dentry_by_name(fn, &d))
		return -1;
	for(i = 2;i < 8;i++)
	{
		if(p->file_desc_tb[i].flag == 0)
		{
			p->file_desc_tb[i].flag = 1;
			p->file_desc_tb[i].file_position = 0;
			p->file_desc_tb[i].inode = d.ind;
			return i;
		}
	}
	return -1;	//no more available files
}

/*
 * Reads n bytes of data from file into buf
 * Inputs: buf, n	(might remove fd since file is loaded in open()?)
 * Outputs: number of bytes read, changed buf
 */
int32_t file_read(int32_t fd, void* buf, int32_t n)
{
	pcb_t* p = get_pcb();
	if(fd > 7)
		return -1;
	n = read_data(p->file_desc_tb[fd].inode, p->file_desc_tb[fd].file_position,
		(uint8_t*)buf, n);
	p->file_desc_tb[fd].file_position += n;
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
	pcb_t* p = get_pcb();
	if(fd > 7 || fd < 2)	//if file is invalid
		return -1;
	p->file_desc_tb[fd].flag = 0;
	return 0;
}
