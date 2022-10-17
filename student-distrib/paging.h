/*
 * This sets up paging and identifies structs related to paging
 * This also supplies functions to easily edit the page directory
 * with new pagetables or other entries
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

/*
 * Set up page directory:
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


/* This is a page table entry */
struct page
{
	uint32_t p	: 1;
	uint32_t rw	: 1;
	uint32_t us	: 1;
	uint32_t pwt	: 1;
	uint32_t pcd	: 1;
	uint32_t a	: 1;
	uint32_t d	: 1;
	uint32_t ps	: 1;	// always 1
	uint32_t g	: 1;
	uint32_t avl	: 3;
	uint32_t pat	: 1;	// always 0
	uint32_t add_32_39 : 8;
	uint32_t rsvd	: 1;	// always 0
	uint32_t add_22_31 : 10;
} __attribute__((packed));


/* This is a pointer to a page table which is an array with it's own entries */
struct tableptr
{
	uint32_t p	: 1;
	uint32_t rw	: 1;
	uint32_t us	: 1;
	uint32_t pwt	: 1;
	uint32_t pcd	: 1;
	uint32_t a	: 1;
	uint32_t avl0	: 1;
	uint32_t ps	: 1;	// always 0
	uint32_t avl1_4	: 4;
	uint32_t add	: 20;
} __attribute__((packed));


/* This is a page directory entry */
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
	uint32_t pat	: 1;	// always 0
	uint32_t g	: 1;
	uint32_t avl	: 3;
	uint32_t add	: 20;
} __attribute__((packed));


/* This is a page directory */
union tblEntry
{
	uint32_t val;
	struct pgTblEntry ent;
};


/* Externally-visible functions */

/* enables paging (and PSE extension) */
void pageEnable();
/* clears pageDir */
void spawnDir();
/* clears a pagetable */
void spawnTbl(union tblEntry tab[1024]);
/* makes the default page directory setup for 3.1 */
void page_init();
/* edits a 4MiB page of the page directory for further page usage */
void chgDir(uint32_t idx, union dirEntry e);

#endif /* _PAGING_H */
