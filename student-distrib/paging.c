#include "paging.h"

static union dirEntry pageDir[1024] __attribute__((aligned(4096)));

static union tblEntry table[1024] __attribute__((aligned(4096)));

/*
 * Here we have the page enabler
 * This assigns the location of the page table
 * Then it enables the page tabling function of the CPU
 *
 * according to guide, there should only be one '%' in basic inline assembly
 */
void pageEnable()
{
	asm(	"movl pageDir, %eax\n\t"
		"movl %eax, %cr3\n\t");	//move the table pointer to cr3
	
	asm(	"movl %cr0, %eax\n\t"
		"orl $0x80000000, %eax\n\t"
		"movl %eax, %cr0\n\t");
}

void spawnDir()
{
	int i;
	for(i = 0;i < 1024;i++)
	{
		pageDir[i].val = 0;
	}
}
