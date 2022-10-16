/*This sets up paging and identifies structs related to paging*/

#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

/*
 * set up page directory:
 * 	0-4mb links to a page table which has entries 4kb each
 *		must create tableEntry
 * 	4-8mb is a 4mb page directory entry (must have PSE enabled in CR4)
 *		dirEntry must either be a 4mb or pagetable pointer type
 *	all other entries unused (4mb entries with no access)
 */

/*
 * This is a pointer to a page table which is an array with it's own entries
 * Or it's own 4mb entry as a whole
 * The PS bit is how the CPU determines if it is a 4mb page or ptr
 * The PAT bit is funky and I have no clue how it works,
 * controls something about how the CPU caches the pages
 * AVL bits can be used by the OS for anything (just keep them 0)
 */

struct page
{
	uint32_t p	: 1;
	uint32_t rw	: 1;
	uint32_t us	: 1;
	uint32_t pwt	: 1;
	uint32_t pcd	: 1;
	uint32_t a	: 1;
	uint32_t d	: 1;
	uint32_t ps	: 1;	//always 1
	uint32_t g	: 1;
	uint32_t avl	: 3;
	uint32_t pat	: 1;	//always 0
	uint32_t add_32_39 : 8;
	uint32_t rsvd	: 1;	//always 0
	uint32_t add_22_31 : 10;
} __attribute__((packed));

struct tableptr
{
	uint32_t p	: 1;
	uint32_t rw	: 1;
	uint32_t us	: 1;
	uint32_t pwt	: 1;
	uint32_t pcd	: 1;
	uint32_t a	: 1;
	uint32_t avl0	: 1;
	uint32_t ps	: 1;	//always 0
	uint32_t avl1_4	: 4;
	uint32_t add	: 20;
} __attribute__((packed));

union dirEntry
{
	uint32_t val;
	struct page whole;
	struct tableptr ptr;
};

/*
 * 4kib entry in the page table linked by the tableptr
 * bits have same meanings as tableptr and whole
 */
struct pgTblEntry
{
	uint32_t p	: 1;
	uint32_t rw	: 1;
	uint32_t us	: 1;
	uint32_t pwt	: 1;
	uint32_t pcd	: 1;
	uint32_t a	: 1;
	uint32_t d	: 1;
	uint32_t pat	: 1;	//always 0
	uint32_t g	: 1;
	uint32_t avl	: 3;
	uint32_t add	: 20;
} __attribute__((packed));
union tblEntry
{
	uint32_t val;
	struct pgTblEntry ent;
};

void pageEnable();

void spawnDir();

#endif
