#include "paging.h"

static union dirEntry pageDir[1024] __attribute__((aligned(4096)));

static union tblEntry table[1024] __attribute__((aligned(4096)));

/*
 * Here we have the page enabler
 * This assigns the location of the page table
 * Then it enables Page Size Extension
 *	(this allows for 4MiB page directory inserts instead of making
 *	a shit ton of 4KiB page tables)
 * Then it enables the page tabling function of the CPU
 *
 * according to guide, there should only be one '%' in basic inline assembly
 */
void pageEnable()
{
	asm(	"movl pageDir, %eax\n\t"
		"movl %eax, %cr3\n\t");	//move the table pointer to cr3

	asm(	"movl %cr4, %eax\n\t"
		"orl $0x10, %eax\n\t"	//bit 4 (5th bit)
		"movl %eax, %cr4\n\t");	//enables PSE (extention that allows 4MiB pages)
	
	asm(	"movl %cr0, %eax\n\t"
		"orl $0x80000000, %eax\n\t"
		"movl %eax, %cr0\n\t");	//this enables the PG bit
	return;
}

void spawnDir()
{
	int i;
	for(i = 0;i < 1024;i++)
	{
		pageDir[i].val = 0x2;	//enable rw on everything
	}				//(since p isn't enabled nothing is accessible)
	return;
}

/*
 * Sets up the pages so that the kernel segment
 * is direct to physical memory, and the video memory
 * maps directly to physical.
 * Accessing anything else will result in a fault.
 */
void setup()
{
	union dirEntry vidTable;
	union dirEntry kernel;	//this is the 4MiB page for the kernel
	vidTable.val = (unsigned)table | 3;	//trick from OSdev, sets p and rw
	kernel.val = 0;
	kernel.whole.p = 1;
	kernel.whole.rw = 1;
	kernel.whole.ps = 1;
	kernel.whole.add_22_31 = 0x400000 >> 22;	//0x400000 is 4mb kernel.val |= 0x4000000 also works
	spawnDir();
	pageDir[1] = kernel;
	pageEnable();
	return;
}
