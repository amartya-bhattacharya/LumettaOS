/* paging.c - the C part of the paging
 * vim:ts=8 noexpandtab
 */

#include "paging.h"

/* Local Variables */
static union dirEntry pageDir[1024] __attribute__((aligned(4096)));
static union tblEntry table[1024] __attribute__((aligned(4096)));


/*
 * Here we have the page enabler
 * This assigns the location of the page table
 * Then it enables Page Size Extension
 * (this allows for 4MiB page directory inserts instead of making
 * a shit ton of 4KiB page tables)
 * Then it enables the page tabling function of the CPU
 * According to guide, there should only be one '%' in basic inline assembly
 */
void pageEnable()
{
	asm(	"movl $pageDir, %eax\n\t"
		"movl %eax, %cr3\n\t");		/* move the table pointer to cr3 */

	asm(	"movl %cr4, %eax\n\t"
		"orl $0x10, %eax\n\t"		/* bit 4 (5th bit) */
		"movl %eax, %cr4\n\t");		/* enables PSE (extention that allows 4MiB pages) */
	
	asm(	"movl %cr0, %eax\n\t"
		"orl $0x80000000, %eax\n\t"
		"movl %eax, %cr0\n\t");		/* this enables the PG bit */
	return;
}

/*
 * This function clears the page directory
 * It is called in page_init()
 */
void spawnDir()
{
	int i;
	for(i = 0; i < 1024; i++)
	{
		pageDir[i].val = 0;		/* initialize table with nothing accessible */
	}
	return;
}

/*
 * This function clears a given page table
 * It is called in page_init()
 */
void spawnTbl(union tblEntry tab[1024])
{
	int i;
	for(i = 0; i < 1024; i++)
		tab[i].val = 0;
}

/*
 * Sets up the pages so that the kernel segment
 * is direct to physical memory, and the video memory
 * maps directly to physical.
 * Accessing anything else will result in a fault.
 */
void page_init()
{
	int i;
	union dirEntry vidTable;
	union dirEntry kernel;			/* this is the 4MiB page for the kernel */
	union tblEntry vidPg;
	vidTable.val = (unsigned)table | 3;		/* trick from OSdev, sets p and rw bits */
	kernel.val = 0;
	kernel.whole.p = 1;
	kernel.whole.rw = 1;	//kernel can still write
	kernel.whole.ps = 1;
	kernel.whole.g = 1;
	kernel.whole.add_22_31 = 0x1;	/* 0x400000 is 4mb kernel.val |= 0x400000 also works */
	//vidPg.val = 0x3;	//sets p and rw bits
	//vidPg.ent.add = 0xB8000 >> 12;	//bits 31-12 (just 0xB8)
	spawnTbl(table);
	for(i = 0xB8; i < 0xC0; i++)
	{
		vidPg.val = 0x3;
		vidPg.ent.add = i;		/* assigns the physical memory to be the same as virtual memory */
		table[i] = vidPg;		/* vidmem starts at 0xB8000, divide by 4KiB to get index */
	}
	spawnDir();
	pageDir[0] = vidTable;
	pageDir[1] = kernel;

	//setup pages so kernel can access process memory at all times
	kernel.whole.add_22_31 = 0x2;
	pageDir[2] = kernel;
	kernel.whole.add_22_31 = 0x3;
	pageDir[3] = kernel;
	kernel.whole.add_22_31 = 0x4;
	pageDir[4] = kernel;
	kernel.whole.add_22_31 = 0x5;
	pageDir[5] = kernel;
	kernel.whole.add_22_31 = 0x6;
	pageDir[6] = kernel;
	kernel.whole.add_22_31 = 0x7;
	pageDir[7] = kernel;
	pageEnable();
	return;
}

/*
 * Changes one entry of the pageDir
 * This allows any kernel program to set up their own pages for their own program,
 * or for a user program that it will call
 */
void chgDir(uint32_t idx, union dirEntry e)
{
	pageDir[idx] = e;
	return;
}

void flushTLB()
{
	asm(	"movl %cr3, %eax\n\t"
		"movl %eax, %cr3\n\t");		//moves pagedir pointer into cr3 which causes flush
	return;
}
